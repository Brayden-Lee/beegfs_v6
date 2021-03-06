#include <program/Program.h>
#include "SetChannelDirectMsgEx.h"

bool SetChannelDirectMsgEx::processIncoming(ResponseContext& ctx)
{
   LogContext log("SetChannelDirect incoming");

   LOG_DEBUG_CONTEXT(log, 4, "Received a SetChannelDirectMsg from: " + ctx.peerName() );

   LOG_DEBUG_CONTEXT(log, 5, "Value: " + StringTk::intToStr(getValue() ) );

   ctx.getSocket()->setIsDirect(getValue() );

   return true;
}


