/*
 * Main purpose is to convert NetMessage defines into a string
 */

#ifndef NETMESSAGELOGHELPER_H_
#define NETMESSAGELOGHELPER_H_

#include <common/net/message/NetMessageTypes.h>
#include <common/logging/DefineToStrMapping.h>

/**
 * NetMessageType to human-readable string mapping
 */
class NetMsgStrMapping : public DefineToStrMapping
{
   public:
      NetMsgStrMapping()
      {
         /*
            Assignments generated with shell command:

            sed -ne "s/^\#define NETMSGTYPE_\([^ ]\+\) \+[0-9]\+\( \/\/.\+\)\?$/defineToStrMap[NETMSGTYPE_\1] = \"\1\";/p" common/net/message/NetMessageTypes.h
          */

         defineToStrMap[NETMSGTYPE_Invalid] = "Invalid";
         defineToStrMap[NETMSGTYPE_RemoveNode] = "RemoveNode";
         defineToStrMap[NETMSGTYPE_RemoveNodeResp] = "RemoveNodeResp";
         defineToStrMap[NETMSGTYPE_GetNodes] = "GetNodes";
         defineToStrMap[NETMSGTYPE_GetNodesResp] = "GetNodesResp";
         defineToStrMap[NETMSGTYPE_HeartbeatRequest] = "HeartbeatRequest";
         defineToStrMap[NETMSGTYPE_Heartbeat] = "Heartbeat";
         defineToStrMap[NETMSGTYPE_GetNodeCapacityPools] = "GetNodeCapacityPools";
         defineToStrMap[NETMSGTYPE_GetNodeCapacityPoolsResp] = "GetNodeCapacityPoolsResp";
         defineToStrMap[NETMSGTYPE_MapTargets] = "MapTargets";
         defineToStrMap[NETMSGTYPE_MapTargetsResp] = "MapTargetsResp";
         defineToStrMap[NETMSGTYPE_GetTargetMappings] = "GetTargetMappings";
         defineToStrMap[NETMSGTYPE_GetTargetMappingsResp] = "GetTargetMappingsResp";
         defineToStrMap[NETMSGTYPE_UnmapTarget] = "UnmapTarget";
         defineToStrMap[NETMSGTYPE_UnmapTargetResp] = "UnmapTargetResp";
         defineToStrMap[NETMSGTYPE_GenericDebug] = "GenericDebug";
         defineToStrMap[NETMSGTYPE_GenericDebugResp] = "GenericDebugResp";
         defineToStrMap[NETMSGTYPE_GetClientStats] = "GetClientStats";
         defineToStrMap[NETMSGTYPE_GetClientStatsResp] = "GetClientStatsResp";
         defineToStrMap[NETMSGTYPE_RefresherControl] = "RefresherControl";
         defineToStrMap[NETMSGTYPE_RefresherControlResp] = "RefresherControlResp";
         defineToStrMap[NETMSGTYPE_RefreshCapacityPools] = "RefreshCapacityPools";
         defineToStrMap[NETMSGTYPE_RefreshCapacityPoolsResp] = "RefreshCapacityPoolsResp";
         defineToStrMap[NETMSGTYPE_StorageBenchControlMsg] = "StorageBenchControlMsg";
         defineToStrMap[NETMSGTYPE_StorageBenchControlMsgResp] = "StorageBenchControlMsgResp";
         defineToStrMap[NETMSGTYPE_RegisterNode] = "RegisterNode";
         defineToStrMap[NETMSGTYPE_RegisterNodeResp] = "RegisterNodeResp";
         defineToStrMap[NETMSGTYPE_RegisterTarget] = "RegisterTarget";
         defineToStrMap[NETMSGTYPE_RegisterTargetResp] = "RegisterTargetResp";
         defineToStrMap[NETMSGTYPE_SetMirrorBuddyGroup] = "SetMirrorBuddyGroup";
         defineToStrMap[NETMSGTYPE_SetMirrorBuddyGroupResp] = "SetMirrorBuddyGroupResp";
         defineToStrMap[NETMSGTYPE_GetMirrorBuddyGroups] = "GetMirrorBuddyGroups";
         defineToStrMap[NETMSGTYPE_GetMirrorBuddyGroupsResp] = "GetMirrorBuddyGroupsResp";
         defineToStrMap[NETMSGTYPE_GetTargetStates] = "GetTargetStates";
         defineToStrMap[NETMSGTYPE_GetTargetStatesResp] = "GetTargetStatesResp";
         defineToStrMap[NETMSGTYPE_RefreshTargetStates] = "RefreshTargetStates";
         defineToStrMap[NETMSGTYPE_RefreshTargetStatesResp] = "RefreshTargetStatesResp";
         defineToStrMap[NETMSGTYPE_GetStatesAndBuddyGroups] = "GetStatesAndBuddyGroups";
         defineToStrMap[NETMSGTYPE_GetStatesAndBuddyGroupsResp] = "GetStatesAndBuddyGroupsResp";
         defineToStrMap[NETMSGTYPE_SetTargetConsistencyStates] = "SetTargetConsistencyStates";
         defineToStrMap[NETMSGTYPE_SetTargetConsistencyStatesResp] = "SetTargetConsistencyStatesResp";
         defineToStrMap[NETMSGTYPE_ChangeTargetConsistencyStates] = "ChangeTargetConsistencyStates";
         defineToStrMap[NETMSGTYPE_ChangeTargetConsistencyStatesResp] = "ChangeTargetConsistencyStatesResp";
         defineToStrMap[NETMSGTYPE_PublishCapacities] = "PublishCapacities";
         defineToStrMap[NETMSGTYPE_RemoveBuddyGroup] = "RemoveBuddyGroup";
         defineToStrMap[NETMSGTYPE_RemoveBuddyGroupResp] = "RemoveBuddyGroupResp";
         defineToStrMap[NETMSGTYPE_GetTargetConsistencyStates] = "GetTargetConsistencyStates";
         defineToStrMap[NETMSGTYPE_GetTargetConsistencyStatesResp] = "GetTargetConsistencyStatesResp";
         defineToStrMap[NETMSGTYPE_MkDir] = "MkDir";
         defineToStrMap[NETMSGTYPE_MkDirResp] = "MkDirResp";
         defineToStrMap[NETMSGTYPE_RmDir] = "RmDir";
         defineToStrMap[NETMSGTYPE_RmDirResp] = "RmDirResp";
         defineToStrMap[NETMSGTYPE_MkFile] = "MkFile";
         defineToStrMap[NETMSGTYPE_MkFileResp] = "MkFileResp";
         defineToStrMap[NETMSGTYPE_UnlinkFile] = "UnlinkFile";
         defineToStrMap[NETMSGTYPE_UnlinkFileResp] = "UnlinkFileResp";
         defineToStrMap[NETMSGTYPE_MkLocalFile] = "MkLocalFile";
         defineToStrMap[NETMSGTYPE_MkLocalFileResp] = "MkLocalFileResp";
         defineToStrMap[NETMSGTYPE_UnlinkLocalFile] = "UnlinkLocalFile";
         defineToStrMap[NETMSGTYPE_UnlinkLocalFileResp] = "UnlinkLocalFileResp";
         defineToStrMap[NETMSGTYPE_Stat] = "Stat";
         defineToStrMap[NETMSGTYPE_StatResp] = "StatResp";
         defineToStrMap[NETMSGTYPE_GetChunkFileAttribs] = "GetChunkFileAttribs";
         defineToStrMap[NETMSGTYPE_GetChunkFileAttribsResp] = "GetChunkFileAttribsResp";
         defineToStrMap[NETMSGTYPE_TruncFile] = "TruncFile";
         defineToStrMap[NETMSGTYPE_TruncFileResp] = "TruncFileResp";
         defineToStrMap[NETMSGTYPE_TruncLocalFile] = "TruncLocalFile";
         defineToStrMap[NETMSGTYPE_TruncLocalFileResp] = "TruncLocalFileResp";
         defineToStrMap[NETMSGTYPE_Rename] = "Rename";
         defineToStrMap[NETMSGTYPE_RenameResp] = "RenameResp";
         defineToStrMap[NETMSGTYPE_SetAttr] = "SetAttr";
         defineToStrMap[NETMSGTYPE_SetAttrResp] = "SetAttrResp";
         defineToStrMap[NETMSGTYPE_ListDirFromOffset] = "ListDirFromOffset";
         defineToStrMap[NETMSGTYPE_ListDirFromOffsetResp] = "ListDirFromOffsetResp";
         defineToStrMap[NETMSGTYPE_StatStoragePath] = "StatStoragePath";
         defineToStrMap[NETMSGTYPE_StatStoragePathResp] = "StatStoragePathResp";
         defineToStrMap[NETMSGTYPE_SetLocalAttr] = "SetLocalAttr";
         defineToStrMap[NETMSGTYPE_SetLocalAttrResp] = "SetLocalAttrResp";
         defineToStrMap[NETMSGTYPE_FindOwner] = "FindOwner";
         defineToStrMap[NETMSGTYPE_FindOwnerResp] = "FindOwnerResp";
         defineToStrMap[NETMSGTYPE_MkLocalDir] = "MkLocalDir";
         defineToStrMap[NETMSGTYPE_MkLocalDirResp] = "MkLocalDirResp";
         defineToStrMap[NETMSGTYPE_RmLocalDir] = "RmLocalDir";
         defineToStrMap[NETMSGTYPE_RmLocalDirResp] = "RmLocalDirResp";
         defineToStrMap[NETMSGTYPE_MovingFileInsert] = "MovingFileInsert";
         defineToStrMap[NETMSGTYPE_MovingFileInsertResp] = "MovingFileInsertResp";
         defineToStrMap[NETMSGTYPE_MovingDirInsert] = "MovingDirInsert";
         defineToStrMap[NETMSGTYPE_MovingDirInsertResp] = "MovingDirInsertResp";
         defineToStrMap[NETMSGTYPE_GetEntryInfo] = "GetEntryInfo";
         defineToStrMap[NETMSGTYPE_GetEntryInfoResp] = "GetEntryInfoResp";
         defineToStrMap[NETMSGTYPE_SetDirPattern] = "SetDirPattern";
         defineToStrMap[NETMSGTYPE_SetDirPatternResp] = "SetDirPatternResp";
         defineToStrMap[NETMSGTYPE_GetHighResStats] = "GetHighResStats";
         defineToStrMap[NETMSGTYPE_GetHighResStatsResp] = "GetHighResStatsResp";
         defineToStrMap[NETMSGTYPE_MkFileWithPattern] = "MkFileWithPattern";
         defineToStrMap[NETMSGTYPE_MkFileWithPatternResp] = "MkFileWithPatternResp";
         defineToStrMap[NETMSGTYPE_RefreshEntryInfo] = "RefreshEntryInfo";
         defineToStrMap[NETMSGTYPE_RefreshEntryInfoResp] = "RefreshEntryInfoResp";
         defineToStrMap[NETMSGTYPE_RmDirEntry] = "RmDirEntry";
         defineToStrMap[NETMSGTYPE_RmDirEntryResp] = "RmDirEntryResp";
         defineToStrMap[NETMSGTYPE_LookupIntent] = "LookupIntent";
         defineToStrMap[NETMSGTYPE_LookupIntentResp] = "LookupIntentResp";
         defineToStrMap[NETMSGTYPE_FindEntryname] = "FindEntryname";
         defineToStrMap[NETMSGTYPE_FindEntrynameResp] = "FindEntrynameResp";
         defineToStrMap[NETMSGTYPE_FindLinkOwner] = "FindLinkOwner";
         defineToStrMap[NETMSGTYPE_FindLinkOwnerResp] = "FindLinkOwnerResp";
         defineToStrMap[NETMSGTYPE_UpdateBacklink] = "UpdateBacklink";
         defineToStrMap[NETMSGTYPE_UpdateBacklinkResp] = "UpdateBacklinkResp";
         defineToStrMap[NETMSGTYPE_MirrorMetadata] = "MirrorMetadata";
         defineToStrMap[NETMSGTYPE_MirrorMetadataResp] = "MirrorMetadataResp";
         defineToStrMap[NETMSGTYPE_SetMetadataMirroring] = "SetMetadataMirroring";
         defineToStrMap[NETMSGTYPE_SetMetadataMirroringResp] = "SetMetadataMirroringResp";
         defineToStrMap[NETMSGTYPE_Hardlink] = "Hardlink";
         defineToStrMap[NETMSGTYPE_HardlinkResp] = "HardlinkResp";
         defineToStrMap[NETMSGTYPE_GetStorageTargetInfo] = "GetStorageTargetInfo";
         defineToStrMap[NETMSGTYPE_GetStorageTargetInfoResp] = "GetStorageTargetInfoResp";
         defineToStrMap[NETMSGTYPE_SetQuota] = "SetQuota";
         defineToStrMap[NETMSGTYPE_SetQuotaResp] = "SetQuotaResp";
         defineToStrMap[NETMSGTYPE_SetExceededQuota] = "SetExceededQuota";
         defineToStrMap[NETMSGTYPE_SetExceededQuotaResp] = "SetExceededQuotaResp";
         defineToStrMap[NETMSGTYPE_RequestExceededQuota] = "RequestExceededQuota";
         defineToStrMap[NETMSGTYPE_RequestExceededQuotaResp] = "RequestExceededQuotaResp";
         defineToStrMap[NETMSGTYPE_UpdateDirParent] = "UpdateDirParent";
         defineToStrMap[NETMSGTYPE_UpdateDirParentResp] = "UpdateDirParentResp";
         defineToStrMap[NETMSGTYPE_ResyncLocalFile] = "ResyncLocalFile";
         defineToStrMap[NETMSGTYPE_ResyncLocalFileResp] = "ResyncLocalFileResp";
         defineToStrMap[NETMSGTYPE_StartStorageTargetResync] = "StartStorageTargetResync";
         defineToStrMap[NETMSGTYPE_StartStorageTargetResyncResp] = "StartStorageTargetResyncResp";
         defineToStrMap[NETMSGTYPE_StorageResyncStarted] = "StorageResyncStarted";
         defineToStrMap[NETMSGTYPE_StorageResyncStartedResp] = "StorageResyncStartedResp";
         defineToStrMap[NETMSGTYPE_ListChunkDirIncremental] = "ListChunkDirIncremental";
         defineToStrMap[NETMSGTYPE_ListChunkDirIncrementalResp] = "ListChunkDirIncrementalResp";
         defineToStrMap[NETMSGTYPE_RmChunkPaths] = "RmChunkPaths";
         defineToStrMap[NETMSGTYPE_RmChunkPathsResp] = "RmChunkPathsResp";
         defineToStrMap[NETMSGTYPE_GetStorageResyncStats] = "GetStorageResyncStats";
         defineToStrMap[NETMSGTYPE_GetStorageResyncStatsResp] = "GetStorageResyncStatsResp";
         defineToStrMap[NETMSGTYPE_SetLastBuddyCommOverride] = "SetLastBuddyCommOverride";
         defineToStrMap[NETMSGTYPE_SetLastBuddyCommOverrideResp] = "SetLastBuddyCommOverrideResp";
         defineToStrMap[NETMSGTYPE_GetQuotaInfo] = "GetQuotaInfo";
         defineToStrMap[NETMSGTYPE_GetQuotaInfoResp] = "GetQuotaInfoResp";
         defineToStrMap[NETMSGTYPE_SetStorageTargetInfo] = "SetStorageTargetInfo";
         defineToStrMap[NETMSGTYPE_SetStorageTargetInfoResp] = "SetStorageTargetInfoResp";
         defineToStrMap[NETMSGTYPE_ListXAttr] = "ListXAttr";
         defineToStrMap[NETMSGTYPE_ListXAttrResp] = "ListXAttrResp";
         defineToStrMap[NETMSGTYPE_GetXAttr] = "GetXAttr";
         defineToStrMap[NETMSGTYPE_GetXAttrResp] = "GetXAttrResp";
         defineToStrMap[NETMSGTYPE_RemoveXAttr] = "RemoveXAttr";
         defineToStrMap[NETMSGTYPE_RemoveXAttrResp] = "RemoveXAttrResp";
         defineToStrMap[NETMSGTYPE_SetXAttr] = "SetXAttr";
         defineToStrMap[NETMSGTYPE_SetXAttrResp] = "SetXAttrResp";
         defineToStrMap[NETMSGTYPE_GetDefaultQuota] = "GetDefaultQuota";
         defineToStrMap[NETMSGTYPE_GetDefaultQuotaResp] = "GetDefaultQuotaResp";
         defineToStrMap[NETMSGTYPE_SetDefaultQuota] = "SetDefaultQuota";
         defineToStrMap[NETMSGTYPE_SetDefaultQuotaResp] = "SetDefaultQuotaResp";
         defineToStrMap[NETMSGTYPE_ResyncSessionStore] = "ResyncSessionStore";
         defineToStrMap[NETMSGTYPE_ResyncSessionStoreResp] = "ResyncSessionStoreResp";
         defineToStrMap[NETMSGTYPE_ResyncRawInodes] = "ResyncRawInodes";
         defineToStrMap[NETMSGTYPE_ResyncRawInodesResp] = "ResyncRawInodesResp";
         defineToStrMap[NETMSGTYPE_GetMetaResyncStats] = "GetMetaResyncStats";
         defineToStrMap[NETMSGTYPE_GetMetaResyncStatsResp] = "GetMetaResyncStatsResp";
         defineToStrMap[NETMSGTYPE_OpenFile] = "OpenFile";
         defineToStrMap[NETMSGTYPE_OpenFileResp] = "OpenFileResp";
         defineToStrMap[NETMSGTYPE_CloseFile] = "CloseFile";
         defineToStrMap[NETMSGTYPE_CloseFileResp] = "CloseFileResp";
         defineToStrMap[NETMSGTYPE_OpenLocalFile] = "OpenLocalFile";
         defineToStrMap[NETMSGTYPE_OpenLocalFileResp] = "OpenLocalFileResp";
         defineToStrMap[NETMSGTYPE_CloseChunkFile] = "CloseChunkFile";
         defineToStrMap[NETMSGTYPE_CloseChunkFileResp] = "CloseChunkFileResp";
         defineToStrMap[NETMSGTYPE_WriteLocalFile] = "WriteLocalFile";
         defineToStrMap[NETMSGTYPE_WriteLocalFileResp] = "WriteLocalFileResp";
         defineToStrMap[NETMSGTYPE_FSyncLocalFile] = "FSyncLocalFile";
         defineToStrMap[NETMSGTYPE_FSyncLocalFileResp] = "FSyncLocalFileResp";
         defineToStrMap[NETMSGTYPE_AcquireAppendLock] = "AcquireAppendLock";
         defineToStrMap[NETMSGTYPE_AcquireAppendLockResp] = "AcquireAppendLockResp";
         defineToStrMap[NETMSGTYPE_ReleaseAppendLock] = "ReleaseAppendLock";
         defineToStrMap[NETMSGTYPE_ReleaseAppendLockResp] = "ReleaseAppendLockResp";
         defineToStrMap[NETMSGTYPE_ReadLocalFileV2] = "ReadLocalFileV2";
         defineToStrMap[NETMSGTYPE_ReadLocalFileV2RespDummy] = "ReadLocalFileV2RespDummy";
         defineToStrMap[NETMSGTYPE_RefreshSession] = "RefreshSession";
         defineToStrMap[NETMSGTYPE_RefreshSessionResp] = "RefreshSessionResp";
         defineToStrMap[NETMSGTYPE_LockGranted] = "LockGranted";
         defineToStrMap[NETMSGTYPE_LockGrantedResp] = "LockGrantedResp";
         defineToStrMap[NETMSGTYPE_FLockEntry] = "FLockEntry";
         defineToStrMap[NETMSGTYPE_FLockEntryResp] = "FLockEntryResp";
         defineToStrMap[NETMSGTYPE_FLockRange] = "FLockRange";
         defineToStrMap[NETMSGTYPE_FLockRangeResp] = "FLockRangeResp";
         defineToStrMap[NETMSGTYPE_FLockAppend] = "FLockAppend";
         defineToStrMap[NETMSGTYPE_FLockAppendResp] = "FLockAppendResp";
         defineToStrMap[NETMSGTYPE_AckNotify] = "AckNotify";
         defineToStrMap[NETMSGTYPE_AckNotifyResp] = "AckNotifyResp";
         defineToStrMap[NETMSGTYPE_BumpFileVersion] = "BumpFileVersion";
         defineToStrMap[NETMSGTYPE_BumpFileVersionResp] = "BumpFileVersionResp";
         defineToStrMap[NETMSGTYPE_GetFileVersion] = "GetFileVersion";
         defineToStrMap[NETMSGTYPE_GetFileVersionResp] = "GetFileVersionResp";
         defineToStrMap[NETMSGTYPE_SetChannelDirect] = "SetChannelDirect";
         defineToStrMap[NETMSGTYPE_SetChannelDirectRespDummy] = "SetChannelDirectRespDummy";
         defineToStrMap[NETMSGTYPE_Ack] = "Ack";
         defineToStrMap[NETMSGTYPE_AckRespDummy] = "AckRespDummy";
         defineToStrMap[NETMSGTYPE_Dummy] = "Dummy";
         defineToStrMap[NETMSGTYPE_DummyRespDummy] = "DummyRespDummy";
         defineToStrMap[NETMSGTYPE_AuthenticateChannel] = "AuthenticateChannel";
         defineToStrMap[NETMSGTYPE_AuthenticateChannelRespDummy] = "AuthenticateChannelRespDummy";
         defineToStrMap[NETMSGTYPE_GenericResponse] = "GenericResponse";
         defineToStrMap[NETMSGTYPE_GenericResponseRespDummy] = "GenericResponseRespDummy";
         defineToStrMap[NETMSGTYPE_PeerInfo] = "PeerInfo";
         defineToStrMap[NETMSGTYPE_Log] = "Log";
         defineToStrMap[NETMSGTYPE_LogResp] = "LogResp";
         defineToStrMap[NETMSGTYPE_GetHostByName] = "GetHostByName";
         defineToStrMap[NETMSGTYPE_GetHostByNameResp] = "GetHostByNameResp";
         defineToStrMap[NETMSGTYPE_GetNodesFromRootMetaNode] = "GetNodesFromRootMetaNode";
         defineToStrMap[NETMSGTYPE_SendNodesList] = "SendNodesList";
         defineToStrMap[NETMSGTYPE_RequestMetaData] = "RequestMetaData";
         defineToStrMap[NETMSGTYPE_RequestStorageData] = "RequestStorageData";
         defineToStrMap[NETMSGTYPE_RequestMetaDataResp] = "RequestMetaDataResp";
         defineToStrMap[NETMSGTYPE_RequestStorageDataResp] = "RequestStorageDataResp";
         defineToStrMap[NETMSGTYPE_GetNodeInfo] = "GetNodeInfo";
         defineToStrMap[NETMSGTYPE_GetNodeInfoResp] = "GetNodeInfoResp";
         defineToStrMap[NETMSGTYPE_RetrieveDirEntries] = "RetrieveDirEntries";
         defineToStrMap[NETMSGTYPE_RetrieveDirEntriesResp] = "RetrieveDirEntriesResp";
         defineToStrMap[NETMSGTYPE_RetrieveInodes] = "RetrieveInodes";
         defineToStrMap[NETMSGTYPE_RetrieveInodesResp] = "RetrieveInodesResp";
         defineToStrMap[NETMSGTYPE_RetrieveChunks] = "RetrieveChunks";
         defineToStrMap[NETMSGTYPE_RetrieveChunksResp] = "RetrieveChunksResp";
         defineToStrMap[NETMSGTYPE_RetrieveFsIDs] = "RetrieveFsIDs";
         defineToStrMap[NETMSGTYPE_RetrieveFsIDsResp] = "RetrieveFsIDsResp";
         defineToStrMap[NETMSGTYPE_DeleteDirEntries] = "DeleteDirEntries";
         defineToStrMap[NETMSGTYPE_DeleteDirEntriesResp] = "DeleteDirEntriesResp";
         defineToStrMap[NETMSGTYPE_CreateDefDirInodes] = "CreateDefDirInodes";
         defineToStrMap[NETMSGTYPE_CreateDefDirInodesResp] = "CreateDefDirInodesResp";
         defineToStrMap[NETMSGTYPE_FixInodeOwnersInDentry] = "FixInodeOwnersInDentry";
         defineToStrMap[NETMSGTYPE_FixInodeOwnersInDentryResp] = "FixInodeOwnersInDentryResp";
         defineToStrMap[NETMSGTYPE_FixInodeOwners] = "FixInodeOwners";
         defineToStrMap[NETMSGTYPE_FixInodeOwnersResp] = "FixInodeOwnersResp";
         defineToStrMap[NETMSGTYPE_LinkToLostAndFound] = "LinkToLostAndFound";
         defineToStrMap[NETMSGTYPE_LinkToLostAndFoundResp] = "LinkToLostAndFoundResp";
         defineToStrMap[NETMSGTYPE_DeleteChunks] = "DeleteChunks";
         defineToStrMap[NETMSGTYPE_DeleteChunksResp] = "DeleteChunksResp";
         defineToStrMap[NETMSGTYPE_CreateEmptyContDirs] = "CreateEmptyContDirs";
         defineToStrMap[NETMSGTYPE_CreateEmptyContDirsResp] = "CreateEmptyContDirsResp";
         defineToStrMap[NETMSGTYPE_UpdateFileAttribs] = "UpdateFileAttribs";
         defineToStrMap[NETMSGTYPE_UpdateFileAttribsResp] = "UpdateFileAttribsResp";
         defineToStrMap[NETMSGTYPE_UpdateDirAttribs] = "UpdateDirAttribs";
         defineToStrMap[NETMSGTYPE_UpdateDirAttribsResp] = "UpdateDirAttribsResp";
         defineToStrMap[NETMSGTYPE_RemoveInodes] = "RemoveInodes";
         defineToStrMap[NETMSGTYPE_RemoveInodesResp] = "RemoveInodesResp";
         defineToStrMap[NETMSGTYPE_ChangeStripeTarget] = "ChangeStripeTarget";
         defineToStrMap[NETMSGTYPE_ChangeStripeTargetResp] = "ChangeStripeTargetResp";
         defineToStrMap[NETMSGTYPE_RecreateFsIDs] = "RecreateFsIDs";
         defineToStrMap[NETMSGTYPE_RecreateFsIDsResp] = "RecreateFsIDsResp";
         defineToStrMap[NETMSGTYPE_RecreateDentries] = "RecreateDentries";
         defineToStrMap[NETMSGTYPE_RecreateDentriesResp] = "RecreateDentriesResp";
         defineToStrMap[NETMSGTYPE_FsckModificationEvent] = "FsckModificationEvent";
         defineToStrMap[NETMSGTYPE_FsckSetEventLogging] = "FsckSetEventLogging";
         defineToStrMap[NETMSGTYPE_FsckSetEventLoggingResp] = "FsckSetEventLoggingResp";
         defineToStrMap[NETMSGTYPE_FetchFsckChunkList] = "FetchFsckChunkList";
         defineToStrMap[NETMSGTYPE_FetchFsckChunkListResp] = "FetchFsckChunkListResp";
         defineToStrMap[NETMSGTYPE_AdjustChunkPermissions] = "AdjustChunkPermissions";
         defineToStrMap[NETMSGTYPE_AdjustChunkPermissionsResp] = "AdjustChunkPermissionsResp";
         defineToStrMap[NETMSGTYPE_MoveChunkFile] = "MoveChunkFile";
         defineToStrMap[NETMSGTYPE_MoveChunkFileResp] = "MoveChunkFileResp";
         defineToStrMap[NETMSGTYPE_SetRootNodeID] = "SetRootNodeID";
         defineToStrMap[NETMSGTYPE_SetRootNodeIDResp] = "SetRootNodeIDResp";
      }

};


#endif /* NETMESSAGELOGHELPER_H_ */
