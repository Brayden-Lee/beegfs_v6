#include "OpenFileMsg.h"


const struct NetMessageOps OpenFileMsg_Ops = {
   .serializePayload = OpenFileMsg_serializePayload,
   .deserializePayload = _NetMessage_deserializeDummy,
   .processIncoming = NetMessage_processIncoming,
   .getSupportedHeaderFeatureFlagsMask = NetMessage_getSupportedHeaderFeatureFlagsMask,
   .supportsSequenceNumbers = true,
};

void OpenFileMsg_serializePayload(NetMessage* this, SerializeCtx* ctx)
{
   OpenFileMsg* thisCast = (OpenFileMsg*)this;

   // clientNumID
   NumNodeID_serialize(&thisCast->clientNumID, ctx);

   // accessFlags
   Serialization_serializeUInt(ctx, thisCast->accessFlags);

   // entryInfo
   EntryInfo_serialize(thisCast->entryInfoPtr, ctx);
}
