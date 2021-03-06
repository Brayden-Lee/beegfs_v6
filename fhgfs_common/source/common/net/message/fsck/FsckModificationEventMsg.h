#ifndef FSCKMODIFICATIONEVENTMSG_H
#define FSCKMODIFICATIONEVENTMSG_H

#include <common/net/message/AcknowledgeableMsg.h>
#include <common/toolkit/serialization/Serialization.h>
#include <common/toolkit/ListTk.h>

class FsckModificationEventMsg : public AcknowledgeableMsgSerdes<FsckModificationEventMsg>
{
   public:
      FsckModificationEventMsg(UInt8List* modificationEventTypeList, StringList* entryIDList,
         bool eventsMissed = false) :
         BaseType(NETMSGTYPE_FsckModificationEvent)
      {
         this->modificationEventTypeList = modificationEventTypeList;
         this->entryIDList = entryIDList;
         this->eventsMissed = eventsMissed;
      }

      // only for deserialization
      FsckModificationEventMsg() : BaseType(NETMSGTYPE_FsckModificationEvent)
      {
      }

   private:
      UInt8List* modificationEventTypeList;
      StringList* entryIDList;
      bool eventsMissed;

      // for deserialization
      struct {
         UInt8List eventTypes;
         StringList entryIDList;
      } parsed;

   public:
      UInt8List& getModificationEventTypeList()
      {
         return *modificationEventTypeList;
      }

      StringList& getEntryIDList()
      {
         return *entryIDList;
      }

      bool getEventsMissed()
      {
         return this->eventsMissed;
      }

      // inliner
      virtual TestingEqualsRes testingEquals(NetMessage* msg)
      {
         FsckModificationEventMsg* msgIn = (FsckModificationEventMsg*) msg;

         if(!ListTk::listsEqual(this->modificationEventTypeList, msgIn->modificationEventTypeList) )
            return TestingEqualsRes_FALSE;

         if ( ! ListTk::listsEqual(this->entryIDList, msgIn->entryIDList) )
            return TestingEqualsRes_FALSE;

         if ( this->eventsMissed != msgIn->getEventsMissed() )
            return TestingEqualsRes_FALSE;

         return TestingEqualsRes_TRUE;
      }

      template<typename This, typename Ctx>
      static void serialize(This obj, Ctx& ctx)
      {
         ctx
            % serdes::backedPtr(obj->modificationEventTypeList, obj->parsed.eventTypes)
            % serdes::backedPtr(obj->entryIDList, obj->parsed.entryIDList)
            % obj->eventsMissed;

         obj->serializeAckID(ctx);
      }
};

#endif /*FSCKMODIFICATIONEVENTMSG_H*/
