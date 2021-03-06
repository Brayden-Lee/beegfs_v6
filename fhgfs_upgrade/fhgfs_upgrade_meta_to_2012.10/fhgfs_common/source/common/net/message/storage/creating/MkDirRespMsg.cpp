#include "MkDirRespMsg.h"

bool MkDirRespMsg::deserializePayload(const char* buf, size_t bufLen)
{
   size_t bufPos = 0;

   { // result
      unsigned resultBufLen;

      if(!Serialization::deserializeInt(&buf[bufPos], bufLen-bufPos,
         &result, &resultBufLen) )
         return false;

      bufPos += resultBufLen;
   }

   { // entryInfo
      unsigned entryLen;

      if (!this->entryInfo.deserialize(&buf[bufPos], bufLen-bufPos, &entryLen) )
         return false;

      bufPos += entryLen;
   }

   return true;
}

void MkDirRespMsg::serializePayload(char* buf)
{
   size_t bufPos = 0;

   // result
   bufPos += Serialization::serializeInt(&buf[bufPos], result);

   // entryInfo
   bufPos += this->entryInfoPtr->serialize(&buf[bufPos]);
}



