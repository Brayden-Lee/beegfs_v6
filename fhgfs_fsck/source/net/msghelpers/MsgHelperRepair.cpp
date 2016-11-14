#include <common/net/message/fsck/CreateDefDirInodesMsg.h>
#include <common/net/message/fsck/CreateDefDirInodesRespMsg.h>
#include <common/net/message/fsck/CreateEmptyContDirsMsg.h>
#include <common/net/message/fsck/CreateEmptyContDirsRespMsg.h>
#include <common/net/message/fsck/DeleteDirEntriesMsg.h>
#include <common/net/message/fsck/DeleteDirEntriesRespMsg.h>
#include <common/net/message/fsck/DeleteChunksMsg.h>
#include <common/net/message/fsck/DeleteChunksRespMsg.h>
#include <common/net/message/fsck/FixInodeOwnersMsg.h>
#include <common/net/message/fsck/FixInodeOwnersRespMsg.h>
#include <common/net/message/fsck/FixInodeOwnersInDentryMsg.h>
#include <common/net/message/fsck/FixInodeOwnersInDentryRespMsg.h>
#include <common/net/message/fsck/LinkToLostAndFoundMsg.h>
#include <common/net/message/fsck/LinkToLostAndFoundRespMsg.h>
#include <common/net/message/fsck/MoveChunkFileMsg.h>
#include <common/net/message/fsck/MoveChunkFileRespMsg.h>
#include <common/net/message/fsck/RecreateFsIDsMsg.h>
#include <common/net/message/fsck/RecreateFsIDsRespMsg.h>
#include <common/net/message/fsck/RecreateDentriesMsg.h>
#include <common/net/message/fsck/RecreateDentriesRespMsg.h>
#include <common/net/message/fsck/RemoveInodesMsg.h>
#include <common/net/message/fsck/RemoveInodesRespMsg.h>

#include <common/net/message/fsck/UpdateFileAttribsMsg.h>
#include <common/net/message/fsck/UpdateFileAttribsRespMsg.h>
#include <common/net/message/fsck/UpdateDirAttribsMsg.h>
#include <common/net/message/fsck/UpdateDirAttribsRespMsg.h>
#include <common/net/message/storage/creating/MkDirMsg.h>
#include <common/net/message/storage/creating/MkDirRespMsg.h>
#include <common/net/message/storage/creating/UnlinkFileMsg.h>
#include <common/net/message/storage/creating/UnlinkFileRespMsg.h>
#include <common/net/message/storage/attribs/SetLocalAttrMsg.h>
#include <common/net/message/storage/attribs/SetLocalAttrRespMsg.h>

#include <common/toolkit/MessagingTk.h>
#include <common/toolkit/MetadataTk.h>

#include <program/Program.h>

#include "MsgHelperRepair.h"

void MsgHelperRepair::deleteDanglingDirEntries(NumNodeID node, bool isBuddyMirrored,
   FsckDirEntryList* dentries, FsckDirEntryList* failedDeletes)
{
   DeleteDirEntriesMsg deleteDirEntriesMsg(dentries);

   RequestResponseNode rrNode(node, Program::getApp()->getMetaNodes());
   RequestResponseArgs rrArgs(nullptr, &deleteDirEntriesMsg, NETMSGTYPE_DeleteDirEntriesResp);

   if (isBuddyMirrored)
      rrNode.setMirrorInfo(Program::getApp()->getMetaMirrorBuddyGroupMapper(), false);

   FhgfsOpsErr commRes = MessagingTk::requestResponseNode(&rrNode, &rrArgs);

   if (commRes == FhgfsOpsErr_SUCCESS)
   {
      auto* deleteDirEntriesRespMsg = (DeleteDirEntriesRespMsg*) rrArgs.outRespMsg;

      deleteDirEntriesRespMsg->getFailedEntries().swap(*failedDeletes);

      if (! failedDeletes->empty())
      {
         for (auto iter = failedDeletes->begin(); iter != failedDeletes->end(); iter++)
            LOG(CRITICAL, "Failed to delete directory entry from metadata node.",
                 node, isBuddyMirrored, as("entryID", iter->getID()));
      }
   }
   else
   {
      *failedDeletes = *dentries;

      LOG(ERR, "Communication error occured.", node, isBuddyMirrored, commRes);
   }
}

void MsgHelperRepair::createDefDirInodes(NumNodeID node, bool isBuddyMirrored,
   const std::vector<std::tuple<std::string, bool>>& entries, FsckDirInodeList* createdInodes)
{
   StringList failedInodeIDs;

   CreateDefDirInodesMsg createDefDirInodesMsgEx(std::move(entries));

   RequestResponseNode rrNode(node, Program::getApp()->getMetaNodes());
   RequestResponseArgs rrArgs(nullptr, &createDefDirInodesMsgEx, NETMSGTYPE_CreateDefDirInodesResp);

   if (isBuddyMirrored)
      rrNode.setMirrorInfo(Program::getApp()->getMetaMirrorBuddyGroupMapper(), false);

   FhgfsOpsErr commRes = MessagingTk::requestResponseNode(&rrNode, &rrArgs);

   if (commRes == FhgfsOpsErr_SUCCESS)
   {
      auto* createDefDirInodesRespMsg = (CreateDefDirInodesRespMsg*) rrArgs.outRespMsg;

      createDefDirInodesRespMsg->getFailedInodeIDs().swap(failedInodeIDs);
      createDefDirInodesRespMsg->getCreatedInodes().swap(*createdInodes);
   }
   else
   {
      for (auto it = entries.begin(); it != entries.end(); ++it)
         failedInodeIDs.push_back(std::get<0>(*it));

      LOG(ERR, "Communication error occured.", node, isBuddyMirrored, commRes);
   }

   if (! failedInodeIDs.empty())
   {
      for (StringListIter iter = failedInodeIDs.begin(); iter != failedInodeIDs.end(); iter++)
         LOG(CRITICAL, "Failed to create default directory inode.", node, isBuddyMirrored,
               as("entryID", *iter));
   }
}

void MsgHelperRepair::correctInodeOwnersInDentry(NumNodeID node, bool isBuddyMirrored,
   FsckDirEntryList* dentries, NumNodeIDList* owners, FsckDirEntryList* failedCorrections)
{
   FixInodeOwnersInDentryMsg fixInodeOwnersMsg(*dentries, *owners);

   RequestResponseNode rrNode(node, Program::getApp()->getMetaNodes());
   RequestResponseArgs rrArgs(nullptr, &fixInodeOwnersMsg, NETMSGTYPE_FixInodeOwnersResp);

   if (isBuddyMirrored)
      rrNode.setMirrorInfo(Program::getApp()->getMetaMirrorBuddyGroupMapper(), false);

   FhgfsOpsErr commRes = MessagingTk::requestResponseNode(&rrNode, &rrArgs);

   if (commRes == FhgfsOpsErr_SUCCESS)
   {
      auto* fixInodeOwnersRespMsg = (FixInodeOwnersInDentryRespMsg*) rrArgs.outRespMsg;

      fixInodeOwnersRespMsg->getFailedEntries().swap(*failedCorrections);
   }
   else
   {
      *failedCorrections = *dentries;

      LOG(ERR, "Communication error occured.", node, isBuddyMirrored, commRes);
   }

   if (! failedCorrections->empty())
   {
      for (auto iter = failedCorrections->begin(); iter != failedCorrections->end(); iter++)
         LOG(CRITICAL, "Failed to correct inode owner information in dentry.",
               node, isBuddyMirrored, as("entryID", iter->getID()));
   }
}

void MsgHelperRepair::correctInodeOwners(NumNodeID node, bool isBuddyMirrored,
   FsckDirInodeList* dirInodes, FsckDirInodeList* failedCorrections)
{
   FixInodeOwnersMsg fixInodeOwnersMsg(dirInodes);

   RequestResponseNode rrNode(node, Program::getApp()->getMetaNodes());
   RequestResponseArgs rrArgs(nullptr, &fixInodeOwnersMsg, NETMSGTYPE_FixInodeOwnersResp);

   if (isBuddyMirrored)
      rrNode.setMirrorInfo(Program::getApp()->getMetaMirrorBuddyGroupMapper(), false);

   FhgfsOpsErr commRes = MessagingTk::requestResponseNode(&rrNode, &rrArgs);

   if (commRes == FhgfsOpsErr_SUCCESS)
   {
      FixInodeOwnersRespMsg* fixInodeOwnersRespMsg = (FixInodeOwnersRespMsg*) rrArgs.outRespMsg;

      fixInodeOwnersRespMsg->getFailedInodes().swap(*failedCorrections);
   }
   else
   {
      *failedCorrections = *dirInodes;

      LOG(ERR, "Communication error occured.", node, isBuddyMirrored, commRes);
   }

   if (! failedCorrections->empty())
   {
      for (auto iter = failedCorrections->begin(); iter != failedCorrections->end(); iter++)
         LOG(CRITICAL, "Failed to correct inode owner information.", node, isBuddyMirrored,
              as("entryID", iter->getID()));
   }
}

void MsgHelperRepair::deleteFiles(NumNodeID node, bool isBuddyMirrored, FsckDirEntryList* dentries,
   FsckDirEntryList* failedDeletes)
{
   const char* logContext = "MsgHelperRepair (deleteFiles)";

   for ( FsckDirEntryListIter iter = dentries->begin(); iter != dentries->end(); iter++ )
   {
      EntryInfo parentInfo(node, "", iter->getParentDirID(), "", DirEntryType_DIRECTORY, 0);

      std::string entryName = iter->getName();
      UnlinkFileMsg unlinkFileMsg(&parentInfo, entryName);

      RequestResponseNode rrNode(node, Program::getApp()->getMetaNodes());
      RequestResponseArgs rrArgs(nullptr, &unlinkFileMsg, NETMSGTYPE_UnlinkFileResp);

      if (isBuddyMirrored)
         rrNode.setMirrorInfo(Program::getApp()->getMetaMirrorBuddyGroupMapper(), false);

      FhgfsOpsErr commRes = MessagingTk::requestResponseNode(&rrNode, &rrArgs);

      if (commRes == FhgfsOpsErr_SUCCESS)
      {
         UnlinkFileRespMsg* unlinkFileRespMsg = (UnlinkFileRespMsg*) rrArgs.outRespMsg;

         // get result
         int unlinkRes = unlinkFileRespMsg->getValue();

         if ( unlinkRes )
         {
            LogContext(logContext).log(Log_CRITICAL,
               "Failed to delete file; entryID: " + iter->getID());
            failedDeletes->push_back(*iter);
         }
      }
      else
      {
         failedDeletes->push_back(*iter);

         LOG(ERR, "Communication error occured.", node, isBuddyMirrored, commRes);
      }
   }
}

void MsgHelperRepair::deleteChunks(Node& node, FsckChunkList* chunks, FsckChunkList* failedDeletes)
{
   const char* logContext = "MsgHelperRepair (deleteChunks)";

   bool commRes;
   char *respBuf = NULL;
   NetMessage *respMsg = NULL;

   DeleteChunksMsg deleteChunksMsg(chunks);

   commRes = MessagingTk::requestResponse(node, &deleteChunksMsg,
      NETMSGTYPE_DeleteChunksResp, &respBuf, &respMsg);

   if ( commRes )
   {
      DeleteChunksRespMsg* deleteChunksRespMsg = (DeleteChunksRespMsg*) respMsg;

      deleteChunksRespMsg->getFailedChunks().swap(*failedDeletes);

      if (! failedDeletes->empty())
      {
         for (FsckChunkListIter iter = failedDeletes->begin(); iter != failedDeletes->end();
            iter++)
         {
            LogContext(logContext).log(Log_CRITICAL, "Failed to delete chunk entry. targetID: " +
               StringTk::uintToStr(iter->getTargetID()) + " chunkID: " + iter->getID());
         }
      }

      SAFE_FREE(respBuf);
      SAFE_DELETE(respMsg);
   }
   else
   {
      SAFE_FREE(respBuf);
      SAFE_DELETE(respMsg);

      *failedDeletes = *chunks;

      LogContext(logContext).logErr("Communication error occured with node: " + node.getID());
   }
}

NodeHandle MsgHelperRepair::referenceLostAndFoundOwner(EntryInfo* outLostAndFoundEntryInfo)
{
   const char* logContext = "MsgHelperRepair (referenceLostAndFoundOwner)";
   App* app = Program::getApp();

// find owner node
   Path path("/" + std::string(META_LOSTANDFOUND_PATH));

   NodeHandle ownerNode;

   FhgfsOpsErr findRes = MetadataTk::referenceOwner(&path, false, app->getMetaNodes(), ownerNode,
      outLostAndFoundEntryInfo, app->getMetaMirrorBuddyGroupMapper());

   if ( findRes != FhgfsOpsErr_SUCCESS )
   {
      LogContext(logContext).log(Log_DEBUG, "No owner node found for lost+found. Directory does not"
         " seem to exist (yet)");
   }

   return ownerNode;
}

bool MsgHelperRepair::createLostAndFound(NodeHandle& outReferencedNode,
   EntryInfo& outLostAndFoundEntryInfo)
{
   const char* logContext = "MsgHelperRepair (createLostAndFound)";
   App* app = Program::getApp();
   NodeStore* metaNodes = app->getMetaNodes();
   bool retVal = false;

   // get root owner node and entryInfo
   NodeHandle rootNode;
   Path rootPath("");
   // rootPath.setAbsolute(true);
   EntryInfo rootEntryInfo;

   FhgfsOpsErr findRes = MetadataTk::referenceOwner(&rootPath, false, metaNodes,
      rootNode, &rootEntryInfo, app->getMetaMirrorBuddyGroupMapper());

   if ( findRes != FhgfsOpsErr_SUCCESS )
   {
      LogContext(logContext).log(Log_CRITICAL, "Unable to reference metadata node for root "
         "directory");
      return false;
   }

   // create the directory
   std::string lostFoundPathStr = META_LOSTANDFOUND_PATH;
   UInt16List preferredNodes;

   MkDirMsg mkDirMsg(&rootEntryInfo, lostFoundPathStr , 0, 0, S_IFDIR | S_IRWXU | S_IRWXG, 0000,
      &preferredNodes);

   RequestResponseNode rrNode(rootEntryInfo.getOwnerNodeID(), Program::getApp()->getMetaNodes());
   RequestResponseArgs rrArgs(nullptr, &mkDirMsg, NETMSGTYPE_MkDirResp);

   if (rootEntryInfo.getIsBuddyMirrored())
      rrNode.setMirrorInfo(Program::getApp()->getMetaMirrorBuddyGroupMapper(), false);

   FhgfsOpsErr commRes = MessagingTk::requestResponseNode(&rrNode, &rrArgs);

   if (commRes != FhgfsOpsErr_SUCCESS)
      return false;

   auto* mkDirRespMsg = (MkDirRespMsg*) rrArgs.outRespMsg;

   FhgfsOpsErr mkDirRes = (FhgfsOpsErr) mkDirRespMsg->getResult();
   if ( mkDirRes != FhgfsOpsErr_SUCCESS )
   {
      LogContext(logContext).log(Log_CRITICAL,
         std::string("Node encountered an error: ") + FhgfsOpsErrTk::toErrString(mkDirRes));
      return false;
   }

   // create seems to have succeeded
   // copy is created because delete is called on mkDirRespMsg, but we still need this object
   outLostAndFoundEntryInfo = *(mkDirRespMsg->getEntryInfo());

   outReferencedNode = outLostAndFoundEntryInfo.getIsBuddyMirrored()
      ? metaNodes->referenceNode(
            NumNodeID(
               app->getMetaMirrorBuddyGroupMapper()->getPrimaryTargetID(
                  outLostAndFoundEntryInfo.getOwnerNodeID().val())))
      : metaNodes->referenceNode(outLostAndFoundEntryInfo.getOwnerNodeID());
   retVal = true;

   return retVal;
}

void MsgHelperRepair::linkToLostAndFound(Node& lostAndFoundNode, EntryInfo* lostAndFoundInfo,
   FsckDirInodeList* dirInodes, FsckDirInodeList* failedInodes, FsckDirEntryList* createdDentries)
{
   const char* logContext = "MsgHelperRepair (linkToLostAndFound)";

   bool commRes;
   char *respBuf = NULL;
   NetMessage *respMsg = NULL;

   LinkToLostAndFoundMsg linkToLostAndFoundMsg(lostAndFoundInfo, dirInodes);

   commRes = MessagingTk::requestResponse(lostAndFoundNode, &linkToLostAndFoundMsg,
      NETMSGTYPE_LinkToLostAndFoundResp, &respBuf, &respMsg);

   if ( commRes )
   {
      LinkToLostAndFoundRespMsg* linkToLostAndFoundRespMsg = (LinkToLostAndFoundRespMsg*) respMsg;

      linkToLostAndFoundRespMsg->getFailedDirInodes().swap(*failedInodes);
      linkToLostAndFoundRespMsg->getCreatedDirEntries().swap(*createdDentries);

      SAFE_FREE(respBuf);
      SAFE_DELETE(respMsg);
   }
   else
   {
      SAFE_FREE(respBuf);
      SAFE_DELETE(respMsg);

      *failedInodes = *dirInodes;

      LogContext(logContext).logErr("Communication error occured with node: " +
         lostAndFoundNode.getID());
   }

   if (! failedInodes->empty())
   {
      for (FsckDirInodeListIter iter = failedInodes->begin(); iter != failedInodes->end();
         iter++)
      {
         LogContext(logContext).log(Log_CRITICAL, "Failed to link directory inode to lost+found. "
            "entryID: " + iter->getID());
      }
   }
}

void MsgHelperRepair::linkToLostAndFound(Node& lostAndFoundNode, EntryInfo* lostAndFoundInfo,
   FsckFileInodeList* fileInodes, FsckFileInodeList* failedInodes,
   FsckDirEntryList* createdDentries)
{
   const char* logContext = "MsgHelperRepair (linkToLostAndFound)";

   bool commRes;
   char *respBuf = NULL;
   NetMessage *respMsg = NULL;

   LinkToLostAndFoundMsg linkToLostAndFoundMsg(lostAndFoundInfo, fileInodes);

   commRes = MessagingTk::requestResponse(lostAndFoundNode, &linkToLostAndFoundMsg,
      NETMSGTYPE_LinkToLostAndFoundResp, &respBuf, &respMsg);

   if ( commRes )
   {
      LinkToLostAndFoundRespMsg* linkToLostAndFoundRespMsg = (LinkToLostAndFoundRespMsg*) respMsg;

      linkToLostAndFoundRespMsg->getFailedFileInodes().swap(*failedInodes);
      linkToLostAndFoundRespMsg->getCreatedDirEntries().swap(*createdDentries);

      SAFE_FREE(respBuf);
      SAFE_DELETE(respMsg);
   }
   else
   {
      SAFE_FREE(respBuf);
      SAFE_DELETE(respMsg);

      *failedInodes = *fileInodes;

      LogContext(logContext).logErr("Communication error occured with node: " +
         lostAndFoundNode.getID());
   }

   if (! failedInodes->empty())
   {
      for (FsckFileInodeListIter iter = failedInodes->begin(); iter != failedInodes->end();
         iter++)
      {
         LogContext(logContext).log(Log_CRITICAL, "Failed to link file inode to lost+found. "
            "entryID: " + iter->getID());
      }
   }
}

void MsgHelperRepair::createContDirs(NumNodeID node, bool isBuddyMirrored, FsckDirInodeList* inodes,
   StringList* failedCreates)
{
   // create a string list with the IDs
   std::vector<CreateEmptyContDirsMsg::Item> items;
   for (FsckDirInodeListIter iter = inodes->begin(); iter != inodes->end(); iter++)
      items.emplace_back(iter->getID(), iter->getIsBuddyMirrored());

   CreateEmptyContDirsMsg createContDirsMsg(std::move(items));

   RequestResponseNode rrNode(node, Program::getApp()->getMetaNodes());
   RequestResponseArgs rrArgs(nullptr, &createContDirsMsg, NETMSGTYPE_CreateEmptyContDirs);

   if (isBuddyMirrored)
      rrNode.setMirrorInfo(Program::getApp()->getMetaMirrorBuddyGroupMapper(), false);

   FhgfsOpsErr commRes = MessagingTk::requestResponseNode(&rrNode, &rrArgs);

   if (commRes == FhgfsOpsErr_SUCCESS)
   {
      auto* createContDirsRespMsg = (CreateEmptyContDirsRespMsg*) rrArgs.outRespMsg;

      createContDirsRespMsg->getFailedIDs().swap(*failedCreates);
   }
   else
   {
      failedCreates->clear();
      for (auto it = inodes->begin(); it != inodes->end(); ++it)
         failedCreates->push_back(it->getID());

      LOG(ERR, "Communication error occured.", node, isBuddyMirrored, commRes);
   }

   if ( !failedCreates->empty() )
   {
      for ( StringListIter iter = failedCreates->begin(); iter != failedCreates->end(); iter++ )
         LOG(CRITICAL, "Failed to create empty content directory.", as("dirID", *iter));
   }
}

void MsgHelperRepair::updateFileAttribs(NumNodeID node, bool isBuddyMirrored, FsckFileInodeList* inodes,
   FsckFileInodeList* failedUpdates)
{
   UpdateFileAttribsMsg updateFileAttribsMsg(inodes);

   RequestResponseNode rrNode(node, Program::getApp()->getMetaNodes());
   RequestResponseArgs rrArgs(nullptr, &updateFileAttribsMsg, NETMSGTYPE_UpdateFileAttribsResp);

   if (isBuddyMirrored)
      rrNode.setMirrorInfo(Program::getApp()->getMetaMirrorBuddyGroupMapper(), false);

   FhgfsOpsErr commRes = MessagingTk::requestResponseNode(&rrNode, &rrArgs);

   if (commRes == FhgfsOpsErr_SUCCESS)
   {
      auto* updateFileAttribsRespMsg = (UpdateFileAttribsRespMsg*) rrArgs.outRespMsg;

      updateFileAttribsRespMsg->getFailedInodes().swap(*failedUpdates);
   }
   else
   {
      *failedUpdates = *inodes;

      LOG(ERR, "Communication error occured.", node, isBuddyMirrored, commRes);
   }

   if ( !failedUpdates->empty() )
   {
      for (auto iter = failedUpdates->begin(); iter != failedUpdates->end(); iter++)
         LOG(CRITICAL, "Failed to update attributes of file inode.", as("entryID", iter->getID()));
   }
}

void MsgHelperRepair::updateDirAttribs(NumNodeID node, bool isBuddyMirrored,
   FsckDirInodeList* inodes, FsckDirInodeList* failedUpdates)
{
   UpdateDirAttribsMsg updateDirAttribsMsg(inodes);

   RequestResponseNode rrNode(node, Program::getApp()->getMetaNodes());
   RequestResponseArgs rrArgs(nullptr, &updateDirAttribsMsg, NETMSGTYPE_UpdateDirAttribsResp);

   if (isBuddyMirrored)
      rrNode.setMirrorInfo(Program::getApp()->getMetaMirrorBuddyGroupMapper(), false);

   FhgfsOpsErr commRes = MessagingTk::requestResponseNode(&rrNode, &rrArgs);

   if (commRes == FhgfsOpsErr_SUCCESS)
   {
      auto* updateDirAttribsRespMsg = (UpdateDirAttribsRespMsg*) rrArgs.outRespMsg;

      updateDirAttribsRespMsg->getFailedInodes().swap(*failedUpdates);
   }
   else
   {
      *failedUpdates = *inodes;

      LOG(ERR, "Communication error occured.", node, isBuddyMirrored, commRes);
   }

   if ( !failedUpdates->empty() )
   {
      for (auto iter = failedUpdates->begin(); iter != failedUpdates->end(); iter++)
         LOG(CRITICAL, "Failed to update attributes of directory inode.",
               as("entryID", iter->getID()));
   }
}

void MsgHelperRepair::recreateFsIDs(NumNodeID node, bool isBuddyMirrored,
   FsckDirEntryList* dentries, FsckDirEntryList* failedEntries)
{
   const char* logContext = "MsgHelperRepair (recreateFsIDs)";

   RecreateFsIDsMsg recreateFsIDsMsg(dentries);

   RequestResponseNode rrNode(node, Program::getApp()->getMetaNodes());
   RequestResponseArgs rrArgs(nullptr, &recreateFsIDsMsg, NETMSGTYPE_RecreateFsIDsResp);

   if (isBuddyMirrored)
      rrNode.setMirrorInfo(Program::getApp()->getMetaMirrorBuddyGroupMapper(), false);

   FhgfsOpsErr commRes = MessagingTk::requestResponseNode(&rrNode, &rrArgs);

   if (commRes == FhgfsOpsErr_SUCCESS)
   {
      RecreateFsIDsRespMsg* recreateFsIDsRespMsg = (RecreateFsIDsRespMsg*) rrArgs.outRespMsg;

      recreateFsIDsRespMsg->getFailedEntries().swap(*failedEntries);
   }
   else
   {
      *failedEntries = *dentries;

      LOG(ERR, "Communication error occured.", node, isBuddyMirrored, commRes);
   }

   if ( !failedEntries->empty() )
   {
      for ( FsckDirEntryListIter iter = failedEntries->begin(); iter != failedEntries->end();
         iter++ )
      {
         LogContext(logContext).log(Log_CRITICAL, "Failed to recreate dentry-by-ID file link."
            " entryID: " + iter->getID());
      }
   }
}

void MsgHelperRepair::recreateDentries(NumNodeID node, bool isBuddyMirrored, FsckFsIDList* fsIDs,
   FsckFsIDList* failedCreates, FsckDirEntryList* createdDentries, FsckFileInodeList* createdInodes)
{
   const char* logContext = "MsgHelperRepair (recreateDentries)";

   RecreateDentriesMsg recreateDentriesMsg(fsIDs);

   RequestResponseNode rrNode(node, Program::getApp()->getMetaNodes());
   RequestResponseArgs rrArgs(nullptr, &recreateDentriesMsg, NETMSGTYPE_RecreateDentriesResp);

   if (isBuddyMirrored)
      rrNode.setMirrorInfo(Program::getApp()->getMetaMirrorBuddyGroupMapper(), false);

   FhgfsOpsErr commRes = MessagingTk::requestResponseNode(&rrNode, &rrArgs);

   if (commRes == FhgfsOpsErr_SUCCESS)
   {
      RecreateDentriesRespMsg* recreateDentriesMsg = (RecreateDentriesRespMsg*) rrArgs.outRespMsg;

      recreateDentriesMsg->getFailedCreates().swap(*failedCreates);
      recreateDentriesMsg->getCreatedDentries().swap(*createdDentries);
      recreateDentriesMsg->getCreatedInodes().swap(*createdInodes);
   }
   else
   {
      *failedCreates = *fsIDs;

      LOG(ERR, "Communication error occured.", node, isBuddyMirrored, commRes);
   }

   if ( !failedCreates->empty() )
   {
      for ( FsckFsIDListIter iter = failedCreates->begin(); iter != failedCreates->end();
         iter++ )
      {
         LogContext(logContext).log(Log_CRITICAL, "Failed to recreate dentry."
            " entryID: " + iter->getID());
      }
   }
}

void MsgHelperRepair::fixChunkPermissions(Node& node, FsckChunkList& chunkList,
   PathInfoList& pathInfoList, FsckChunkList& failedChunks)
{
   const char* logContext = "MsgHelperRepair (fixChunkPermissions)";

   if ( chunkList.size() != pathInfoList.size() )
   {
      LogContext(logContext).logErr(
         "Failed to set uid/gid for chunks. Size of lists does not match.");
      return;
   }

   FsckChunkListIter chunksIter = chunkList.begin();
   PathInfoListIter pathInfoIter = pathInfoList.begin();
   for ( ; chunksIter != chunkList.end(); chunksIter++, pathInfoIter++ )
   {
      bool commRes;
      char *respBuf = NULL;
      NetMessage *respMsg = NULL;

      std::string chunkID = chunksIter->getID();
      uint16_t targetID = chunksIter->getTargetID();
      int validAttribs = SETATTR_CHANGE_USERID | SETATTR_CHANGE_GROUPID; // only interested in these
      SettableFileAttribs attribs = {0, 0, 0, 0, 0};
      attribs.userID = chunksIter->getUserID();
      attribs.groupID = chunksIter->getGroupID();

      bool enableCreation = false;

      PathInfo pathInfo = *pathInfoIter;
      SetLocalAttrMsg setLocalAttrMsg(chunkID, targetID, &pathInfo, validAttribs, &attribs,
         enableCreation);
      setLocalAttrMsg.addMsgHeaderFeatureFlag(SETLOCALATTRMSG_FLAG_USE_QUOTA);

      commRes = MessagingTk::requestResponse(node, &setLocalAttrMsg, NETMSGTYPE_SetLocalAttrResp,
         &respBuf, &respMsg);

      if ( commRes )
      {
         SetLocalAttrRespMsg* setLocalAttrRespMsg = (SetLocalAttrRespMsg*) respMsg;

         if ( setLocalAttrRespMsg->getResult() != FhgfsOpsErr_SUCCESS )
         {
            LogContext(logContext).logErr(
               "Failed to set uid/gid for chunk. chunkID: " + chunkID + "; targetID: "
                  + StringTk::uintToStr(targetID));

            failedChunks.push_back(*chunksIter);
         }
      }
      else
      {
         LogContext(logContext).logErr("Communication error occured with node: " + node.getID());

         failedChunks.push_back(*chunksIter);
      }

      SAFE_FREE(respBuf);
      SAFE_DELETE(respMsg);
   }
}

/*
 * NOTE: chunk gets modified! (new savedPath is set)
 */
bool MsgHelperRepair::moveChunk(Node& node, FsckChunk& chunk, const std::string& moveTo,
   bool allowOverwrite)
{
   const char* logContext = "MsgHelperRepair (moveChunks)";
   bool result;

   bool commRes;
   char *respBuf = NULL;
   NetMessage *respMsg = NULL;

   MoveChunkFileMsg moveChunkFileMsg(chunk.getID(), chunk.getTargetID(),
         chunk.getBuddyGroupID() != 0, chunk.getSavedPath()->str(), moveTo, allowOverwrite);

   commRes = MessagingTk::requestResponse(node, &moveChunkFileMsg,
      NETMSGTYPE_MoveChunkFileResp, &respBuf, &respMsg);

   if ( commRes )
   {
      MoveChunkFileRespMsg* moveChunkFileRespMsg = (MoveChunkFileRespMsg*) respMsg;

      result = moveChunkFileRespMsg->getValue() == FhgfsOpsErr_SUCCESS;

      if(!result)
      {
         LogContext(logContext).logErr(
            "Failed to move chunk. chunkID: " + chunk.getID() + "; targetID: "
               + StringTk::uintToStr(chunk.getTargetID() ) + "; fromPath: "
               + chunk.getSavedPath()->str() + "; toPath: "
               + moveTo);
      }
      else
      {
         // set newPath in chunk
         chunk.setSavedPath(Path(moveTo));
      }
   }
   else
   {
      LogContext(logContext).logErr("Communication error occured with node: " + node.getID());
      result = false;
   }

   SAFE_FREE(respBuf);
   SAFE_DELETE(respMsg);

   return result;
}

void MsgHelperRepair::deleteFileInodes(NumNodeID node, bool isBuddyMirrored,
   FsckFileInodeList& inodes, StringList& failedDeletes)
{
   std::vector<RemoveInodesMsg::Item> items;

   for (auto it = inodes.begin(); it != inodes.end(); ++it)
      items.emplace_back(it->getID(), DirEntryType_REGULARFILE, it->getIsBuddyMirrored());

   RemoveInodesMsg removeInodesMsg(std::move(items));

   RequestResponseNode rrNode(node, Program::getApp()->getMetaNodes());
   RequestResponseArgs rrArgs(nullptr, &removeInodesMsg, NETMSGTYPE_RemoveInodesResp);

   if (isBuddyMirrored)
      rrNode.setMirrorInfo(Program::getApp()->getMetaMirrorBuddyGroupMapper(), false);

   FhgfsOpsErr commRes = MessagingTk::requestResponseNode(&rrNode, &rrArgs);

   if (commRes == FhgfsOpsErr_SUCCESS)
   {
      RemoveInodesRespMsg* removeInodesRespMsg = (RemoveInodesRespMsg*) rrArgs.outRespMsg;
      failedDeletes = removeInodesRespMsg->releaseFailedEntryIDList();

      for ( StringListIter iter = failedDeletes.begin(); iter != failedDeletes.end(); iter++ )
         LOG(ERR, "Failed to delete file inode.", node, isBuddyMirrored, as("entryID", *iter));
   }
   else
   {
      LOG(ERR, "Communication error occured.", node, isBuddyMirrored, commRes);

      failedDeletes.clear();
      for (auto it = inodes.begin(); it != inodes.end(); ++it)
         failedDeletes.push_back(it->getID());
   }
}
