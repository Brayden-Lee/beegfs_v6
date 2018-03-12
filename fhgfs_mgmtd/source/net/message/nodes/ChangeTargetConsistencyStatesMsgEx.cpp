#include <common/net/message/control/GenericResponseMsg.h>
#include <common/net/message/nodes/ChangeTargetConsistencyStatesRespMsg.h>
#include <common/nodes/TargetStateStore.h>
#include <program/Program.h>

#include "ChangeTargetConsistencyStatesMsgEx.h"

bool ChangeTargetConsistencyStatesMsgEx::processIncoming(ResponseContext& ctx)
{
   const char* logContext = "Change target states incoming";
   LOG_DEBUG(logContext, Log_DEBUG, "Received a ChangeTargetConsistencyStatesMsg from: "
      + ctx.peerName() );

   App* app = Program::getApp();

   if (app->isShuttingDown())
   {
      ctx.sendResponse(GenericResponseMsg(GenericRespMsgCode_TRYAGAIN, "Mgmdt shutting down."));
      return true;
   }

   MgmtdTargetStateStore* stateStore;
   MirrorBuddyGroupMapper* mirrorBuddyGroupMapper;

   NodeType nodeType = getNodeType();

   switch (nodeType)
   {
      case NODETYPE_Storage:
         mirrorBuddyGroupMapper = app->getStorageBuddyGroupMapper();
         stateStore = app->getTargetStateStore();
         break;

      case NODETYPE_Meta:
         mirrorBuddyGroupMapper = app->getMetaBuddyGroupMapper();
         stateStore = app->getMetaStateStore();
         break;

      default:
         LOG(ERR, "Invalid node type.",
               as("Node Type", Node::nodeTypeToStr(nodeType)),
               as("Sender", ctx.peerName())
            );
         return false;
   }

   FhgfsOpsErr result = FhgfsOpsErr_UNKNOWNTARGET;

   result = stateStore->changeConsistencyStatesFromLists(getTargetIDs(), getOldStates(),
      getNewStates(), mirrorBuddyGroupMapper);

   ctx.sendResponse(ChangeTargetConsistencyStatesRespMsg(result) );

   stateStore->saveStatesToFile();

   return true;
}
