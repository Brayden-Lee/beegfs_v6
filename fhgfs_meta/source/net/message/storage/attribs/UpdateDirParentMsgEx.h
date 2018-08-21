#ifndef UPDATEDIRPARENTMSGEX1_H_
#define UPDATEDIRPARENTMSGEX1_H_

#include <storage/MetaStore.h>
#include <common/storage/StorageErrors.h>
#include <common/net/message/storage/attribs/UpdateDirParentMsg.h>
#include <common/net/message/storage/attribs/UpdateDirParentRespMsg.h>
#include <net/message/MirroredMessage.h>

class UpdateDirParentMsgEx : public MirroredMessage<UpdateDirParentMsg, DirIDLock>
{
   public:
      typedef ErrorCodeResponseState<UpdateDirParentRespMsg, NETMSGTYPE_UpdateDirParent>
         ResponseState;

      virtual bool processIncoming(ResponseContext& ctx) override;

      std::unique_ptr<MirroredMessageResponseState> executeLocally(ResponseContext& ctx,
         bool isSecondary) override;

      DirIDLock lock(EntryLockStore& store) override;

      bool isMirrored() override { return getEntryInfo()->getIsBuddyMirrored(); }

   private:
      ResponseContext* rctx;

      bool forwardToSecondary(ResponseContext& ctx) override;

      FhgfsOpsErr processSecondaryResponse(NetMessage& resp) override
      {
         return (FhgfsOpsErr) static_cast<UpdateDirParentRespMsg&>(resp).getValue();
      }

      const char* mirrorLogContext() const override { return "UpdateDirParentMsgEx/forward"; }
};


#endif /*UPDATEDIRPARENTMSGEX1_H_*/
