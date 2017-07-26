#include <common/components/streamlistenerv2/IncomingPreprocessedMsgWork.h>
#include <common/net/message/control/GenericResponseMsg.h>
#include <common/net/message/storage/creating/RmDirRespMsg.h>
#include <common/net/message/storage/creating/RmLocalDirMsg.h>
#include <common/net/message/storage/creating/RmLocalDirRespMsg.h>
#include <common/toolkit/MessagingTk.h>
#include <components/ModificationEventFlusher.h>
#include <program/Program.h>
#include <session/EntryLock.h>
#include "RmDirMsgEx.h"


bool RmDirMsgEx::processIncoming(ResponseContext& ctx)
{
#ifdef BEEGFS_DEBUG
   const char* logContext = "RmDirMsg incoming";

   LOG_DEBUG(logContext, 4, "Received a RmDirMsg from: " + ctx.peerName() );
#endif // BEEGFS_DEBUG

   BaseType::processIncoming(ctx);

   // update operation counters
   Program::getApp()->getNodeOpStats()->updateNodeOp(ctx.getSocket()->getPeerIP(),
      MetaOpCounter_RMDIR, getMsgHeaderUserID() );

   return true;
}

std::tuple<HashDirLock, DirIDLock, DirIDLock, ParentNameLock> RmDirMsgEx::lock(EntryLockStore& store)
{
   MetaStore* metaStore = Program::getApp()->getMetaStore();
   EntryInfo delEntryInfo;

   DirInode* parentDir = metaStore->referenceDir(
         getParentInfo()->getEntryID(), getParentInfo()->getIsBuddyMirrored(), true);
   if (!parentDir)
      return {};
   else
   {
      parentDir->getDirEntryInfo(getDelDirName(), delEntryInfo);
      metaStore->releaseDir(getParentInfo()->getEntryID());
   }

   DirIDLock parentDirLock;
   DirIDLock delDirLock;
   HashDirLock hashLock;

   if (resyncJob && resyncJob->isRunning())
      hashLock = {&store, MetaStorageTk::getMetaInodeHash(delEntryInfo.getEntryID())};

   // lock directories in deadlock-avoidance order, see MirroredMessage::lock()
   if (delEntryInfo.getEntryID() < getParentInfo()->getEntryID())
   {
      delDirLock = {&store, delEntryInfo.getEntryID(), true};
      parentDirLock = {&store, getParentInfo()->getEntryID(), true};
   }
   else if (delEntryInfo.getEntryID() == getParentInfo()->getEntryID())
   {
      delDirLock = {&store, delEntryInfo.getEntryID(), true};
   }
   else
   {
      parentDirLock = {&store, getParentInfo()->getEntryID(), true};
      delDirLock = {&store, delEntryInfo.getEntryID(), true};
   }

   ParentNameLock parentNameLock(&store, getParentInfo()->getEntryID(), getDelDirName());

   return std::make_tuple(
         std::move(hashLock),
         std::move(parentDirLock),
         std::move(delDirLock),
         std::move(parentNameLock));
}

std::unique_ptr<RmDirMsgEx::ResponseState> RmDirMsgEx::rmDir(ResponseContext& ctx,
   const bool isSecondary)
{
   MetaStore* metaStore = Program::getApp()->getMetaStore();
   ModificationEventFlusher* modEventFlusher = Program::getApp()->getModificationEventFlusher();
   bool modEventLoggingEnabled = modEventFlusher->isLoggingEnabled();

   FhgfsOpsErr retVal;

   EntryInfo* parentInfo = this->getParentInfo();
   const std::string& delDirName = this->getDelDirName();

   EntryInfo outDelEntryInfo;

   // reference parent
   DirInode* parentDir = metaStore->referenceDir(parentInfo->getEntryID(),
      parentInfo->getIsBuddyMirrored(), true);
   if(!parentDir)
      return boost::make_unique<ResponseState>(FhgfsOpsErr_PATHNOTEXISTS);

   DirEntry removeDirEntry(delDirName);
   bool getEntryRes = parentDir->getDirDentry(delDirName, removeDirEntry);
   if(!getEntryRes)
      retVal = FhgfsOpsErr_PATHNOTEXISTS;
   else
   {
      int additionalFlags = 0;

      const std::string& parentEntryID = parentInfo->getEntryID();
      removeDirEntry.getEntryInfo(parentEntryID, additionalFlags, &outDelEntryInfo);

      App* app = Program::getApp();
      NumNodeID ownerNodeID = outDelEntryInfo.getOwnerNodeID();

      // no-comms path: the dir inode is owned by us
      if ((!isMirrored() && ownerNodeID == app->getLocalNode().getNumID()) ||
            (isMirrored() &&
               ownerNodeID.val() == app->getMetaBuddyGroupMapper()->getLocalGroupID()))
         retVal = app->getMetaStore()->removeDirInode(outDelEntryInfo.getEntryID(),
               outDelEntryInfo.getIsBuddyMirrored());
      else if (!isSecondary)
         retVal = rmRemoteDirInode(&outDelEntryInfo);
      else
         retVal = FhgfsOpsErr_SUCCESS;

      if(retVal == FhgfsOpsErr_SUCCESS)
      { // local removal succeeded => remove meta dir
         retVal = parentDir->removeDir(delDirName, NULL);
      }
   }

   if (retVal == FhgfsOpsErr_SUCCESS && shouldFixTimestamps())
      fixInodeTimestamp(*parentDir, dirTimestamps);

   // clean-up
   metaStore->releaseDir(parentInfo->getEntryID() );

   // respond early because adding modification events may block for a long time
   earlyComplete(ctx, ResponseState(retVal));

   if (retVal == FhgfsOpsErr_SUCCESS && modEventLoggingEnabled)
   {
      const std::string& entryID = outDelEntryInfo.getEntryID();
      modEventFlusher->add(ModificationEvent_DIRREMOVED, entryID);
   }

   return {};
}

/**
 * Remove the inode of this directory
 */
FhgfsOpsErr RmDirMsgEx::rmRemoteDirInode(EntryInfo* delEntryInfo)
{
   const char* logContext = "RmDirMsg (rm dir inode)";

   FhgfsOpsErr retVal = FhgfsOpsErr_SUCCESS;

   App* app = Program::getApp();
   NumNodeID ownerNodeID = delEntryInfo->getOwnerNodeID();

   LOG_DEBUG(logContext, Log_DEBUG,
      "Removing dir inode from metadata node: " + ownerNodeID.str() + "; "
      "dirname: " + delEntryInfo->getFileName() + "; isBuddyMirrored: " +
      StringTk::intToStr(delEntryInfo->getIsBuddyMirrored()) );

   // prepare request

   RmLocalDirMsg rmMsg(delEntryInfo);

   RequestResponseArgs rrArgs(NULL, &rmMsg, NETMSGTYPE_RmLocalDirResp);

   RequestResponseNode rrNode(ownerNodeID, app->getMetaNodes() );
   rrNode.setTargetStates(app->getMetaStateStore() );

   if (delEntryInfo->getIsBuddyMirrored())
      rrNode.setMirrorInfo(app->getMetaBuddyGroupMapper(), false);

   do // (this loop just exists to enable the "break"-jump, so it's not really a loop)
   {
      // send request to other mds and receive response

      FhgfsOpsErr requestRes = MessagingTk::requestResponseNode(&rrNode, &rrArgs);

      if(requestRes != FhgfsOpsErr_SUCCESS)
      { // communication error
         LogContext(logContext).log(Log_WARNING,
            "Communication with metadata server failed. "
            "ownerNodeID: " + ownerNodeID.str() + "; "
            "dirID: "       + delEntryInfo->getEntryID()       + "; " +
            "dirname: "     + delEntryInfo->getFileName() );
         retVal = requestRes;
         break;
      }

      // correct response type received
      RmLocalDirRespMsg* rmRespMsg = (RmLocalDirRespMsg*)rrArgs.outRespMsg;

      retVal = rmRespMsg->getResult();
      if(unlikely( (retVal != FhgfsOpsErr_SUCCESS) && (retVal != FhgfsOpsErr_NOTEMPTY) ) )
      { // error: dir inode not removed
         std::string errString = FhgfsOpsErrTk::toErrString(retVal);

         LogContext(logContext).log(Log_DEBUG,
            "Metadata server was unable to remove dir inode. "
            "ownerNodeID: " + ownerNodeID.str() + "; "
            "dirID: "       + delEntryInfo->getEntryID()       + "; " +
            "dirname: "     + delEntryInfo->getFileName()      + "; " +
            "error: "       + errString);

         break;
      }

      // success: dir inode removed
      LOG_DEBUG(logContext, Log_DEBUG,
         "Metadata server removed dir inode: "
         "ownerNodeID: " + ownerNodeID.str() + "; "
         "dirID: "       + delEntryInfo->getEntryID()       + "; " +
         "dirname: "     + delEntryInfo->getFileName() );

   } while(0);


   return retVal;
}

bool RmDirMsgEx::forwardToSecondary(ResponseContext& ctx)
{
   return sendToSecondary(ctx, *this, NETMSGTYPE_RmDirResp) == FhgfsOpsErr_SUCCESS;
}
