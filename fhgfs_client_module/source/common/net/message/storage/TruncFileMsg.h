#ifndef TRUNCFILEMSG_H_
#define TRUNCFILEMSG_H_

#include <common/net/message/NetMessage.h>
#include <common/storage/EntryInfo.h>


#define TRUNCFILEMSG_FLAG_USE_QUOTA       1 /* if the message contains quota informations */


struct TruncFileMsg;
typedef struct TruncFileMsg TruncFileMsg;

static inline void TruncFileMsg_init(TruncFileMsg* this);
static inline void TruncFileMsg_initFromEntryInfo(TruncFileMsg* this, int64_t filesize,
   const EntryInfo* entryInfo);

// virtual functions
extern void TruncFileMsg_serializePayload(NetMessage* this, SerializeCtx* ctx);



struct TruncFileMsg
{
   NetMessage netMessage;
   
   int64_t filesize;

   // for serialization
   const EntryInfo* entryInfoPtr;  // not owned by this object!
};

extern const struct NetMessageOps TruncFileMsg_Ops;

void TruncFileMsg_init(TruncFileMsg* this)
{
   NetMessage_init(&this->netMessage, NETMSGTYPE_TruncFile, &TruncFileMsg_Ops);
}
   
/**
 * @param entryInfo just a reference, so do not free it as long as you use this object!
 */
void TruncFileMsg_initFromEntryInfo(TruncFileMsg* this, int64_t filesize,
   const EntryInfo* entryInfo)
{
   TruncFileMsg_init(this);
   
   this->filesize = filesize;
   
   this->entryInfoPtr = entryInfo;
}

#endif /*TRUNCFILEMSG_H_*/
