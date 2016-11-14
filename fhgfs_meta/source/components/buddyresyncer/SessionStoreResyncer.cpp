#include "SessionStoreResyncer.h"

#include <common/toolkit/MessagingTk.h>
#include <common/net/message/storage/mirroring/ResyncSessionStoreMsg.h>
#include <common/net/message/storage/mirroring/ResyncSessionStoreRespMsg.h>
#include <common/toolkit/StringTk.h>
#include <program/Program.h>
#include <app/App.h>

#include <boost/scoped_array.hpp>

SessionStoreResyncer::SessionStoreResyncer(const NumNodeID& buddyNodeID)
 : buddyNodeID(buddyNodeID) {}

void SessionStoreResyncer::doSync()
{
   App* app = Program::getApp();
   SessionStore* sessions = app->getMirroredSessions();
   NodeStoreServersEx* metaNodes = app->getMetaNodes();
   const uint64_t numSessions = sessions->getSize();

   numSessionsToSync.set(numSessions);

   // Serialize sessions store into buffer
   std::pair<std::unique_ptr<char[]>, size_t> sessionStoreSerBuf = sessions->serializeToBuf();

   if (sessionStoreSerBuf.second == 0)
   {
      // Serialization failed.
      errors.set(1);
      return;
   }

   LOG(DEBUG, "Serialized session store", as("size", sessionStoreSerBuf.second));

   ResyncSessionStoreMsg msg(sessionStoreSerBuf.first.get(), sessionStoreSerBuf.second);
   RequestResponseArgs rrArgs(NULL, &msg, NETMSGTYPE_ResyncSessionStoreResp);
   RequestResponseNode rrNode(buddyNodeID, metaNodes);
   msg.registerStreamoutHook(rrArgs);

   FhgfsOpsErr requestRes = MessagingTk::requestResponseNode(&rrNode, &rrArgs);

   if (requestRes != FhgfsOpsErr_SUCCESS)
   {
      errors.set(1);
      LOG(ERR, "Request failed.", requestRes);
      return;
   }

   ResyncSessionStoreRespMsg* resp = (ResyncSessionStoreRespMsg*)rrArgs.outRespMsg;
   FhgfsOpsErr retVal = resp->getResult();

   LOG(DEBUG, "ResyncSessionStoreRespMsg", retVal);

   if (retVal != FhgfsOpsErr_SUCCESS)
      errors.set(1);
   else
      numSessionsSynced.set(numSessions);
}
