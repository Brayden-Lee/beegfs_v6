#include "ReadLocalFileV2Msg.h"

bool ReadLocalFileV2Msg::deserializePayload(const char* buf, size_t bufLen)
{
   size_t bufPos = 0;
   
   // offset

   unsigned offsetLen;
   if(!Serialization::deserializeInt64(&buf[bufPos], bufLen-bufPos, &offset, &offsetLen) )
      return false;
   
   bufPos += offsetLen;

   // count

   unsigned countLen;
   if(!Serialization::deserializeInt64(&buf[bufPos], bufLen-bufPos, &count, &countLen) )
      return false;
   
   bufPos += countLen;

   // accessFlags

   unsigned accessFlagsLen;
   if(!Serialization::deserializeUInt(&buf[bufPos], bufLen-bufPos, &accessFlags, &accessFlagsLen) )
      return false;

   bufPos += accessFlagsLen;

   // fileHandleID
   
   unsigned handleBufLen;
   
   if(!Serialization::deserializeStrAlign4(&buf[bufPos], bufLen-bufPos,
      &fileHandleIDLen, &fileHandleID, &handleBufLen) )
      return false;
   
   bufPos += handleBufLen;

   // sessionID
   
   unsigned sessionBufLen;
   
   if(!Serialization::deserializeStrAlign4(&buf[bufPos], bufLen-bufPos,
      &sessionIDLen, &sessionID, &sessionBufLen) )
      return false;
   
   bufPos += sessionBufLen;
   
   // targetID

   unsigned targetBufLen;

   if(!Serialization::deserializeUShort(&buf[bufPos], bufLen-bufPos, &targetID, &targetBufLen) )
      return false;

   bufPos += targetBufLen;

   // disableIO

   unsigned disableIOBufLen;
   if(!Serialization::deserializeBool(&buf[bufPos], bufLen-bufPos, &disableIO, &disableIOBufLen) )
      return false;

   bufPos += disableIOBufLen;

   return true;
}

void ReadLocalFileV2Msg::serializePayload(char* buf)
{
   size_t bufPos = 0;
   
   // offset
   bufPos += Serialization::serializeInt64(&buf[bufPos], offset);

   // count
   bufPos += Serialization::serializeInt64(&buf[bufPos], count);

   // accessFlags
   bufPos += Serialization::serializeUInt(&buf[bufPos], accessFlags);

   // fileHandleID
   bufPos += Serialization::serializeStrAlign4(&buf[bufPos], fileHandleIDLen, fileHandleID);

   // sessionID
   bufPos += Serialization::serializeStrAlign4(&buf[bufPos], sessionIDLen, sessionID);

   // targetID
   bufPos += Serialization::serializeUShort(&buf[bufPos], targetID);

   // disableIO
   bufPos += Serialization::serializeBool(&buf[bufPos], disableIO);
}

