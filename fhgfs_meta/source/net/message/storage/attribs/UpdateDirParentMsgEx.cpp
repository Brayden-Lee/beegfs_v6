#include <program/Program.h>
#include <common/net/message/storage/attribs/UpdateDirParentRespMsg.h>
#include <common/net/message/storage/attribs/SetLocalAttrMsg.h>
#include <common/net/message/storage/attribs/SetLocalAttrRespMsg.h>
#include <common/toolkit/MessagingTk.h>
#include <components/worker/SetChunkFileAttribsWork.h>
#include <session/EntryLock.h>
#include "UpdateDirParentMsgEx.h"


bool UpdateDirParentMsgEx::processIncoming(ResponseContext& ctx)
{
#ifdef BEEGFS_DEBUG
   const char* logContext = "UpdateDirParentMsgEx incoming";

   LOG_DEBUG(logContext, Log_DEBUG, "Received a UpdateParentEntryIDMsg from: " + ctx.peerName() );
#endif // BEEGFS_DEBUG

   App* app = Program::getApp();

   EntryInfo* entryInfo = getEntryInfo();
   NumNodeID parentNodeID = getParentNodeID();

   LOG_DEBUG("UpdateDirParentMsgEx::processIncoming", Log_DEBUG,
      "ParentID: " + entryInfo->getParentEntryID() + " EntryID: " +
      entryInfo->getEntryID() + " parentNodeID: " + parentNodeID.str() + " BuddyMirrored: " +
      (entryInfo->getIsBuddyMirrored() ? "Yes" : "No") + " Secondary: " +
      (hasFlag(NetMessageHeader::Flag_BuddyMirrorSecond) ? "Yes" : "No"));
   (void) entryInfo;
   (void) parentNodeID;

   rctx = &ctx;

   BaseType::processIncoming(ctx);

   // update operation counters
   app->getNodeOpStats()->updateNodeOp(ctx.getSocket()->getPeerIP(), MetaOpCounter_UPDATEDIRPARENT,
      getMsgHeaderUserID() );

   return true;
}

DirIDLock UpdateDirParentMsgEx::lock(EntryLockStore& store)
{
   if (rctx->isLocallyGenerated())
      return {};

   return {&store, getEntryInfo()->getEntryID(), true};
}

std::unique_ptr<MirroredMessageResponseState> UpdateDirParentMsgEx::executeLocally(
   ResponseContext& ctx, bool isSecondary)
{
   auto setRes = Program::getApp()->getMetaStore()->setDirParent(getEntryInfo(), getParentNodeID());

   if (setRes == FhgfsOpsErr_SUCCESS && shouldFixTimestamps())
   {
      auto dir = Program::getApp()->getMetaStore()->referenceDir(getEntryInfo()->getEntryID(),
            true, true);
      if (dir)
      {
         fixInodeTimestamp(*dir, dirTimestamps);
         Program::getApp()->getMetaStore()->releaseDir(dir->getID());
      }
   }

   return boost::make_unique<ResponseState>(setRes);
}

bool UpdateDirParentMsgEx::forwardToSecondary(ResponseContext& ctx)
{
   return sendToSecondary(ctx, *this, NETMSGTYPE_UpdateDirParentResp) == FhgfsOpsErr_SUCCESS;
}
