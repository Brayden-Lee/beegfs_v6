#ifndef MAPTARGETSMSGEX_H_
#define MAPTARGETSMSGEX_H_

#include <common/net/message/NetMessage.h>
#include <common/Common.h>

/**
 * Note: Only the receive/deserialize part of this message is implemented (so it cannot be sent).
 * Note: Processing only sends response when ackID is set (no MapTargetsRespMsg implemented).
 */

struct MapTargetsMsgEx;
typedef struct MapTargetsMsgEx MapTargetsMsgEx;

static inline void MapTargetsMsgEx_init(MapTargetsMsgEx* this);

// virtual functions
extern bool MapTargetsMsgEx_deserializePayload(NetMessage* this, DeserializeCtx* ctx);
extern bool __MapTargetsMsgEx_processIncoming(NetMessage* this, struct App* app,
   fhgfs_sockaddr_in* fromAddr, struct Socket* sock, char* respBuf, size_t bufLen);

// inliners
static inline void MapTargetsMsgEx_parseTargetIDs(MapTargetsMsgEx* this,
   UInt16List* outTargetIDs);

// getters & setters
static inline NumNodeID MapTargetsMsgEx_getNodeID(MapTargetsMsgEx* this);
static inline const char* MapTargetsMsgEx_getAckID(MapTargetsMsgEx* this);


struct MapTargetsMsgEx
{
   NetMessage netMessage;

   NumNodeID nodeID;
   unsigned ackIDLen;
   const char* ackID;

   // for serialization
   UInt16List* targetIDs;

   // for deserialization
   RawList rawTargetIDsList;
};

extern const struct NetMessageOps MapTargetsMsgEx_Ops;

void MapTargetsMsgEx_init(MapTargetsMsgEx* this)
{
   NetMessage_init(&this->netMessage, NETMSGTYPE_MapTargets, &MapTargetsMsgEx_Ops);
}

void MapTargetsMsgEx_parseTargetIDs(MapTargetsMsgEx* this, UInt16List* outTargetIDs)
{
   Serialization_deserializeUInt16List(&this->rawTargetIDsList, outTargetIDs);
}

NumNodeID MapTargetsMsgEx_getNodeID(MapTargetsMsgEx* this)
{
   return this->nodeID;
}

const char* MapTargetsMsgEx_getAckID(MapTargetsMsgEx* this)
{
   return this->ackID;
}



#endif /* MAPTARGETSMSGEX_H_ */
