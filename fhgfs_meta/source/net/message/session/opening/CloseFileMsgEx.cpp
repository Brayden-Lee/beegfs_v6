#include <common/components/streamlistenerv2/IncomingPreprocessedMsgWork.h>
#include <common/net/message/control/GenericResponseMsg.h>
#include <common/net/message/session/opening/CloseFileRespMsg.h>
#include <common/toolkit/MessagingTk.h>
#include <common/toolkit/SessionTk.h>
#include <net/msghelpers/MsgHelperClose.h>
#include <net/msghelpers/MsgHelperLocking.h>
#include <program/Program.h>
#include <session/EntryLock.h>
#include <storage/MetaStore.h>
#include "CloseFileMsgEx.h"

FileIDLock CloseFileMsgEx::lock(EntryLockStore& store)
{
   return {&store, getEntryInfo()->getEntryID()};
}

bool CloseFileMsgEx::processIncoming(ResponseContext& ctx)
{
#ifdef BEEGFS_DEBUG
   const char* logContext = "CloseFileMsg incoming";

   LOG_DEBUG(logContext, Log_DEBUG, "Received a CloseFileMsg from: " + ctx.peerName() );
#endif // BEEGFS_DEBUG

   App* app = Program::getApp();

   LOG_DEBUG(logContext, Log_DEBUG,
      "BuddyMirrored: " + std::string(getEntryInfo()->getIsBuddyMirrored() ? "Yes" : "No") +
      " Secondary: " +
      std::string(hasFlag(NetMessageHeader::Flag_BuddyMirrorSecond) ? "Yes" : "No") );

   // update operation counters (here on top because we have an early sock release in this msg)
   app->getNodeOpStats()->updateNodeOp(ctx.getSocket()->getPeerIP(), MetaOpCounter_CLOSE,
      getMsgHeaderUserID() );

   return BaseType::processIncoming(ctx);
}

std::unique_ptr<MirroredMessageResponseState> CloseFileMsgEx::executeLocally(ResponseContext& ctx,
   bool isSecondary)
{
   if (isSecondary)
      return closeFileSecondary(ctx);
   else
      return closeFilePrimary(ctx);
}

std::unique_ptr<CloseFileMsgEx::ResponseState> CloseFileMsgEx::closeFilePrimary(
   ResponseContext& ctx)
{
   FhgfsOpsErr closeRes;
   bool unlinkDisposalFile = false;
   EntryInfo* entryInfo = getEntryInfo();

   if(isMsgHeaderFeatureFlagSet(CLOSEFILEMSG_FLAG_CANCELAPPENDLOCKS) )
   { // client requests cleanup of granted or pending locks for this handle

      unsigned ownerFD = SessionTk::ownerFDFromHandleID(getFileHandleID() );
      EntryLockDetails lockDetails(getClientNumID(), 0, 0, "", ENTRYLOCKTYPE_CANCEL);

      // TODO: handle flock for buddy mirrored files
      MsgHelperLocking::flockAppend(entryInfo, ownerFD, lockDetails);
   }

   /* two alternatives:
         1) early response before chunk file close (if client isn't interested in chunks result).
         2) normal response after chunk file close. */

   // if we are buddy mirrored, *do not* allow early close responses. since the primary will close
   // the chunk files (and update the inodes dynamic attributes), the secondary cannot do that.
   // thus we need to close all chunks, get the dynamic attributes, and push them to the secondary
   if (getEntryInfo()->getIsBuddyMirrored())
      unsetMsgHeaderFeatureFlag(CLOSEFILEMSG_FLAG_EARLYRESPONSE);

   if(isMsgHeaderFeatureFlagSet(CLOSEFILEMSG_FLAG_EARLYRESPONSE) )
   { // alternative 1: client requests an early response

      /* note: linux won't even return the vfs release() result to userspace apps, so there's
         usually no point in waiting for the chunk file close result before sending the response */

      unsigned accessFlags;
      MetaFileHandle inode;

      closeRes = MsgHelperClose::closeSessionFile(
         getClientNumID(), getFileHandleID(), entryInfo, &accessFlags, inode);

      // send response
      earlyComplete(ctx, ResponseState(closeRes));

      if(likely(closeRes == FhgfsOpsErr_SUCCESS) )
         closeRes = closeFileAfterEarlyResponse(std::move(inode), accessFlags, &unlinkDisposalFile);
   }
   else
   { // alternative 2: normal response (after chunk file close)

      closeRes = MsgHelperClose::closeFile(getClientNumID(), getFileHandleID(),
         entryInfo, getMaxUsedNodeIndex(), getMsgHeaderUserID(), &unlinkDisposalFile, &dynAttribs,
         &inodeTimestamps);

      if (getEntryInfo()->getIsBuddyMirrored() && getMaxUsedNodeIndex() >= 0)
         addMsgHeaderFeatureFlag(CLOSEFILEMSG_FLAG_DYNATTRIBS);

      // send response
      // (caller won't be interested in the unlink result below, so no reason to wait for that)
      earlyComplete(ctx, ResponseState(closeRes));
   }

   // unlink if file marked as disposable
   if( (closeRes == FhgfsOpsErr_SUCCESS) && unlinkDisposalFile)
   { // check whether file has been unlinked (and perform the unlink operation on last close)

      /* note: we do this only if also the chunk file close succeeded, because if storage servers
         are down, unlinkDisposableFile() will keep the file in the disposal store anyways */

      // this only touches timestamps on the disposal dirinode, which is not visible to the user,
      // so no need to fix up timestamps that have diverged between primary and secondary
      MsgHelperClose::unlinkDisposableFile(entryInfo->getEntryID(), getMsgHeaderUserID(),
            entryInfo->getIsBuddyMirrored());
   }

   return {};
}

std::unique_ptr<CloseFileMsgEx::ResponseState> CloseFileMsgEx::closeFileSecondary(
   ResponseContext& ctx)
{
   /*
    * TODO: handle flock for buddy mirrored files
   if(isMsgHeaderFeatureFlagSet(CLOSEFILEMSG_FLAG_CANCELAPPENDLOCKS) )
   { // client requests cleanup of granted or pending locks for this handle

      unsigned ownerFD = SessionTk::ownerFDFromHandleID(getFileHandleID() );
      EntryLockDetails lockDetails(getClientNumID(), 0, 0, "", ENTRYLOCKTYPE_CANCEL);

      MsgHelperLocking::flockAppend(entryInfo, ownerFD, lockDetails);
   }
   */

   // on secondary we only need to close the session and the meta file, because the chunk files
   // will be closed by primary

   unsigned accessFlags;
   MetaFileHandle inode;

   FhgfsOpsErr closeRes = MsgHelperClose::closeSessionFile(
      getClientNumID(), getFileHandleID(), getEntryInfo(), &accessFlags, inode);

   // the file may not be open on the secondary, in which case inode == nullptr
   if (closeRes != FhgfsOpsErr_SUCCESS)
      return boost::make_unique<ResponseState>(closeRes);

   // maybe assert?
   if (isMsgHeaderFeatureFlagSet(CLOSEFILEMSG_FLAG_DYNATTRIBS))
      inode->setDynAttribs(dynAttribs);

   fixInodeTimestamp(*inode, inodeTimestamps, getEntryInfo());

   unsigned numHardlinks;
   unsigned numInodeRefs;
   Program::getApp()->getMetaStore()->closeFile(getEntryInfo(), std::move(inode), accessFlags,
         &numHardlinks, &numInodeRefs);

   // unlink if file marked as disposable
   // this only touches timestamps on the disposal dirinode, which is not visible to the user,
   // so no need to fix up timestamps that have diverged between primary and secondary
   if( (closeRes == FhgfsOpsErr_SUCCESS) && (!numHardlinks) && (!numInodeRefs))
      MsgHelperClose::unlinkDisposableFile(getEntryInfo()->getEntryID(), getMsgHeaderUserID(),
            getEntryInfo()->getIsBuddyMirrored());

   return boost::make_unique<ResponseState>(closeRes);
}

/**
 * The rest after MsgHelperClose::closeSessionFile(), i.e. MsgHelperClose::closeChunkFile()
 * and metaStore->closeFile().
 *
 * @param inode inode for closed file (as returned by MsgHelperClose::closeSessionFile() )
 * @param maxUsedNodeIndex zero-based index, -1 means "none"
 * @param outFileWasUnlinked true if the hardlink count of the file was 0
 */
FhgfsOpsErr CloseFileMsgEx::closeFileAfterEarlyResponse(MetaFileHandle inode, unsigned accessFlags,
   bool* outUnlinkDisposalFile)
{
   MetaStore* metaStore = Program::getApp()->getMetaStore();

   unsigned numHardlinks;
   unsigned numInodeRefs;

   *outUnlinkDisposalFile = false;


   FhgfsOpsErr chunksRes = MsgHelperClose::closeChunkFile(
      getClientNumID(), getFileHandleID(), getMaxUsedNodeIndex(), *inode, getEntryInfo(),
      getMsgHeaderUserID(), NULL);


   metaStore->closeFile(getEntryInfo(), std::move(inode), accessFlags, &numHardlinks,
         &numInodeRefs);


   if (!numHardlinks && !numInodeRefs)
      *outUnlinkDisposalFile = true;

   return chunksRes;
}

bool CloseFileMsgEx::forwardToSecondary(ResponseContext& ctx)
{
   return sendToSecondary(ctx, *this, NETMSGTYPE_CloseFileResp) == FhgfsOpsErr_SUCCESS;
}
