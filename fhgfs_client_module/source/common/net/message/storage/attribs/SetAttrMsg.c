#include "SetAttrMsg.h"


const struct NetMessageOps SetAttrMsg_Ops = {
   .serializePayload = SetAttrMsg_serializePayload,
   .deserializePayload = _NetMessage_deserializeDummy,
   .processIncoming = NetMessage_processIncoming,
   // TODO this always returns zero (same in the WriteLocalFileMsg which definitely supports some
   //      feature flags - is this correct there)
   .getSupportedHeaderFeatureFlagsMask = NetMessage_getSupportedHeaderFeatureFlagsMask,
   .supportsSequenceNumbers = true,
};

void SetAttrMsg_serializePayload(NetMessage* this, SerializeCtx* ctx)
{
   SetAttrMsg* thisCast = (SetAttrMsg*)this;

   // validAttribs
   Serialization_serializeInt(ctx, thisCast->validAttribs);

   // mode
   Serialization_serializeInt(ctx, thisCast->attribs.mode);

   // modificationTimeSecs
   Serialization_serializeInt64(ctx, thisCast->attribs.modificationTimeSecs);

   // lastAccessTimeSecs
   Serialization_serializeInt64(ctx, thisCast->attribs.lastAccessTimeSecs);

   // userID
   Serialization_serializeUInt(ctx, thisCast->attribs.userID);

   // groupID
   Serialization_serializeUInt(ctx, thisCast->attribs.groupID);

   // entryInfo
   EntryInfo_serialize(thisCast->entryInfoPtr, ctx);
}
