/**
 * vim: set ts=4 sw=4 tw=99 noet:
 * =============================================================================
 * GameFixes_mm
 * Copyright (C) 2026 Michal "Slynx (˙·٠● S l y n x ●٠·˙)" Přikryl.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *   - Michal "Slynx (˙·٠● S l y n x ●٠·˙)" Přikryl
 *
 * Project: GameFixes_mm
 */

//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: Typed wrappers around the protobuf network messages
//
//=============================================================================

#ifndef SDK_NETMESSAGES_H
#define SDK_NETMESSAGES_H
#ifdef _WIN32
#pragma once
#endif

#include <netmessages.pb.h>
#include <networkbasetypes.pb.h>
#include <networksystem/inetchannel.h>
#include <networksystem/inetworkmessages.h>
#include <networksystem/inetworkserializer.h>
#include <networksystem_protomessages.pb.h>

#undef min
#undef max

/* =========================
Signon groups
========================= */

// SignonGroup_t and its values come from s2sdk's inetworkmessages.h.
inline constexpr const char* k_pszNetGroupNames[] =
{
    "Generic", "Local Player", "Other Players", "Entities", "Sounds", "Events",
    "Voice", "String Table", "Move", "String Command", "Signon", "System",
    "Decals", "User Messages", "Client Messages", "Spawn Groups",
    "Game Engine", "Hltv Replay"
};

/* =========================
Typed message wrappers
========================= */

// hl2sdk-cs2's CNetMessagePB is a one-parameter stub, and a class template
// cannot be redeclared with a different parameter list, so the message id,
// signon group and buffer type ride along here instead.
//
// No data members and no virtuals of its own, so the layout stays
// CNetMessagePB<T>'s. These wrappers are abstract, like the stub they derive
// from -- fine everywhere they are only cast onto engine memory.
template< int32_t ID, typename PROTO_TYPE, SignonGroup_t SIGNON = SIGNONGROUP_GENERIC,
          NetChannelBufType_t BUF_TYPE = BUF_RELIABLE, bool IS_FOR_SERVER = false >
class CNetMessagePBTyped : public CNetMessagePB< PROTO_TYPE >
{
public:
    using MyType_t = CNetMessagePBTyped< ID, PROTO_TYPE, SIGNON, BUF_TYPE, IS_FOR_SERVER >;
    using PBType_t = PROTO_TYPE;

    static constexpr int32_t             kMsgId       = ID;
    static constexpr SignonGroup_t       kSignonGroup = SIGNON;
    static constexpr NetChannelBufType_t kBufType     = BUF_TYPE;
    static constexpr bool                kIsForServer = IS_FOR_SERVER;

    static constexpr const char* GetGroupName()
    {
        return ( kSignonGroup >= 0 && kSignonGroup <= SIGNONGROUP_HLTV_REPLAY )
            ? k_pszNetGroupNames[ kSignonGroup ] : "Unknown";
    }
};

template< int32_t ID, typename PROTO_TYPE, NetChannelBufType_t BUF_TYPE = BUF_RELIABLE >
class CUserMessagePB : public CNetMessagePBTyped< ID, PROTO_TYPE, SIGNONGROUP_USER_MESSAGES, BUF_TYPE > {};

/* =========================
System messages
========================= */

class NetMessageSplitscreenUserChanged_t :   public CNetMessagePBTyped< -1, NetMessageSplitscreenUserChanged, SIGNONGROUP_SYSTEM > {};
class NetMessagePacketStart_t :              public CNetMessagePBTyped< -1, NetMessagePacketStart, SIGNONGROUP_SYSTEM > {};
class NetMessagePacketEnd_t :                public CNetMessagePBTyped< -1, NetMessagePacketEnd, SIGNONGROUP_SYSTEM > {};
class NetMessageConnectionClosed_t :         public CNetMessagePBTyped< -1, NetMessageConnectionClosed, SIGNONGROUP_SYSTEM > {};
class NetMessageConnectionCrashed_t :        public CNetMessagePBTyped< -1, NetMessageConnectionCrashed, SIGNONGROUP_SYSTEM > {};

/* =========================
Engine (net_*)
========================= */

class CNETMsg_Tick_t :                       public CNetMessagePBTyped< net_Tick, CNETMsg_Tick, SIGNONGROUP_GAME_ENGINE, BUF_UNRELIABLE > {};
class CNETMsg_StringCmd_t :                  public CNetMessagePBTyped< net_StringCmd, CNETMsg_StringCmd, SIGNONGROUP_STRING_COMMAND > {};
class CNETMsg_SetConVar_t :                  public CNetMessagePBTyped< net_SetConVar, CNETMsg_SetConVar, SIGNONGROUP_STRING_COMMAND > {};
class CNETMsg_SignonState_t :                public CNetMessagePBTyped< net_SignonState, CNETMsg_SignonState, SIGNONGROUP_SIGNON > {};
class CNETMsg_SpawnGroup_Load_t :            public CNetMessagePBTyped< net_SpawnGroup_Load, CNETMsg_SpawnGroup_Load, SIGNONGROUP_SPAWN_GROUPS > {};
class CNETMsg_SpawnGroup_ManifestUpdate_t :  public CNetMessagePBTyped< net_SpawnGroup_ManifestUpdate, CNETMsg_SpawnGroup_ManifestUpdate, SIGNONGROUP_SPAWN_GROUPS > {};
class CNETMsg_SpawnGroup_SetCreationTick_t : public CNetMessagePBTyped< net_SpawnGroup_SetCreationTick, CNETMsg_SpawnGroup_SetCreationTick, SIGNONGROUP_SPAWN_GROUPS > {};
class CNETMsg_SpawnGroup_Unload_t :          public CNetMessagePBTyped< net_SpawnGroup_Unload, CNETMsg_SpawnGroup_Unload, SIGNONGROUP_SPAWN_GROUPS > {};
class CNETMsg_SpawnGroup_LoadCompleted_t :   public CNetMessagePBTyped< net_SpawnGroup_LoadCompleted, CNETMsg_SpawnGroup_LoadCompleted, SIGNONGROUP_SPAWN_GROUPS > {};
class CNETMsg_DebugOverlay_t :               public CNetMessagePBTyped< net_DebugOverlay, CNETMsg_DebugOverlay, SIGNONGROUP_GAME_ENGINE > {};

/* =========================
Client -> Server (clc_*)
========================= */

class CCLCMsg_ClientInfo_t :                 public CNetMessagePBTyped< clc_ClientInfo, CCLCMsg_ClientInfo, SIGNONGROUP_SIGNON > {};
class CCLCMsg_Move_t :                       public CNetMessagePBTyped< clc_Move, CCLCMsg_Move, SIGNONGROUP_MOVE, BUF_UNRELIABLE > {};
class CCLCMsg_VoiceData_t :                  public CNetMessagePBTyped< clc_VoiceData, CCLCMsg_VoiceData, SIGNONGROUP_VOICE > {};
class CCLCMsg_BaselineAck_t :                public CNetMessagePBTyped< clc_BaselineAck, CCLCMsg_BaselineAck, SIGNONGROUP_GAME_ENGINE > {};
class CCLCMsg_RespondCvarValue_t :           public CNetMessagePBTyped< clc_RespondCvarValue, CCLCMsg_RespondCvarValue > {};
class CCLCMsg_LoadingProgress_t :            public CNetMessagePBTyped< clc_LoadingProgress, CCLCMsg_LoadingProgress, SIGNONGROUP_SIGNON > {};
class CCLCMsg_SplitPlayerConnect_t :         public CNetMessagePBTyped< clc_SplitPlayerConnect, CCLCMsg_SplitPlayerConnect, SIGNONGROUP_SIGNON > {};
class CCLCMsg_SplitPlayerDisconnect_t :      public CNetMessagePBTyped< clc_SplitPlayerDisconnect, CCLCMsg_SplitPlayerDisconnect, SIGNONGROUP_SIGNON > {};
class CCLCMsg_ServerStatus_t :               public CNetMessagePBTyped< clc_ServerStatus, CCLCMsg_ServerStatus > {};
class CCLCMsg_RequestPause_t :               public CNetMessagePBTyped< clc_RequestPause, CCLCMsg_RequestPause > {};
class CCLCMsg_CmdKeyValues_t :               public CNetMessagePBTyped< clc_CmdKeyValues, CCLCMsg_CmdKeyValues, SIGNONGROUP_STRING_COMMAND > {};
class CCLCMsg_RconServerDetails_t :          public CNetMessagePBTyped< clc_RconServerDetails, CCLCMsg_RconServerDetails > {};
class CCLCMsg_HltvReplay_t :                 public CNetMessagePBTyped< clc_HltvReplay, CCLCMsg_HltvReplay, SIGNONGROUP_HLTV_REPLAY > {};
class CCLCMsg_Diagnostic_t :                 public CNetMessagePBTyped< clc_Diagnostic, CCLCMsg_Diagnostic > {};

/* =========================
Server -> Client (svc_*)
========================= */

class CSVCMsg_ServerInfo_t :                 public CNetMessagePBTyped< svc_ServerInfo, CSVCMsg_ServerInfo, SIGNONGROUP_SIGNON > {};
class CSVCMsg_FlattenedSerializer_t :        public CNetMessagePBTyped< svc_FlattenedSerializer, CSVCMsg_FlattenedSerializer, SIGNONGROUP_SIGNON > {};
class CSVCMsg_ClassInfo_t :                  public CNetMessagePBTyped< svc_ClassInfo, CSVCMsg_ClassInfo, SIGNONGROUP_SIGNON > {};
class CSVCMsg_SetPause_t :                   public CNetMessagePBTyped< svc_SetPause, CSVCMsg_SetPause > {};
class CSVCMsg_CreateStringTable_t :          public CNetMessagePBTyped< svc_CreateStringTable, CSVCMsg_CreateStringTable, SIGNONGROUP_SIGNON > {};
class CSVCMsg_UpdateStringTable_t :          public CNetMessagePBTyped< svc_UpdateStringTable, CSVCMsg_UpdateStringTable, SIGNONGROUP_STRING_TABLE > {};
class CSVCMsg_VoiceInit_t :                  public CNetMessagePBTyped< svc_VoiceInit, CSVCMsg_VoiceInit, SIGNONGROUP_SIGNON > {};
class CSVCMsg_VoiceData_t :                  public CNetMessagePBTyped< svc_VoiceData, CSVCMsg_VoiceData, SIGNONGROUP_VOICE > {};
class CSVCMsg_Print_t :                      public CNetMessagePBTyped< svc_Print, CSVCMsg_Print > {};
class CSVCMsg_Sounds_t :                     public CNetMessagePBTyped< svc_Sounds, CSVCMsg_Sounds, SIGNONGROUP_SOUNDS > {};
class CSVCMsg_SetView_t :                    public CNetMessagePBTyped< svc_SetView, CSVCMsg_SetView > {};
class CSVCMsg_ClearAllStringTables_t :       public CNetMessagePBTyped< svc_ClearAllStringTables, CSVCMsg_ClearAllStringTables, SIGNONGROUP_SIGNON > {};
class CSVCMsg_CmdKeyValues_t :               public CNetMessagePBTyped< svc_CmdKeyValues, CSVCMsg_CmdKeyValues, SIGNONGROUP_STRING_COMMAND > {};
class CSVCMsg_BSPDecal_t :                   public CNetMessagePBTyped< svc_BSPDecal, CSVCMsg_BSPDecal > {};
class CSVCMsg_SplitScreen_t :                public CNetMessagePBTyped< svc_SplitScreen, CSVCMsg_SplitScreen, SIGNONGROUP_SIGNON > {};
// The one wrapper that cannot stay abstract: CServerSideClientBase embeds it by
// value, and CNetMessage leaves AsProto/AsProto2/GetNetMessage/CopyConstruct/
// GetMessageId pure (CS2 14197 dropped GetName and returns NetMessageInfo_t from
// GetNetMessage). Filling the slots adds no members and no virtuals of its own, so
// the layout is unchanged; the bodies are never reached, since this is only
// ever cast onto engine memory.
class CSVCMsg_PacketEntities_t : public CNetMessagePBTyped< svc_PacketEntities, CSVCMsg_PacketEntities, SIGNONGROUP_INVALID >
{
public:
    void* AsProto() const override { return nullptr; }
    void* AsProto2() const override { return nullptr; }
    NetMessageInfo_t* GetNetMessage() const override { return nullptr; }
    CNetMessage* CopyConstruct() const override { return nullptr; }
    NetworkMessageId GetMessageId() const override { return svc_PacketEntities; }
};
class CSVCMsg_Prefetch_t :                   public CNetMessagePBTyped< svc_Prefetch, CSVCMsg_Prefetch, SIGNONGROUP_SOUNDS > {};
class CSVCMsg_Menu_t :                       public CNetMessagePBTyped< svc_Menu, CSVCMsg_Menu > {};
class CSVCMsg_GetCvarValue_t :               public CNetMessagePBTyped< svc_GetCvarValue, CSVCMsg_GetCvarValue > {};
class CSVCMsg_StopSound_t :                  public CNetMessagePBTyped< svc_StopSound, CSVCMsg_StopSound, SIGNONGROUP_SOUNDS > {};
class CSVCMsg_PeerList_t :                   public CNetMessagePBTyped< svc_PeerList, CSVCMsg_PeerList > {};
class CSVCMsg_PacketReliable_t :             public CNetMessagePBTyped< svc_PacketReliable, CSVCMsg_PacketReliable > {};
class CSVCMsg_HLTVStatus_t :                 public CNetMessagePBTyped< svc_HLTVStatus, CSVCMsg_HLTVStatus, SIGNONGROUP_GENERIC, BUF_RELIABLE, true > {};
class CSVCMsg_ServerSteamID_t :              public CNetMessagePBTyped< svc_ServerSteamID, CSVCMsg_ServerSteamID, SIGNONGROUP_GENERIC, BUF_RELIABLE, true > {};
class CSVCMsg_FullFrameSplit_t :             public CNetMessagePBTyped< svc_FullFrameSplit, CSVCMsg_FullFrameSplit > {};
class CSVCMsg_RconServerDetails_t :          public CNetMessagePBTyped< svc_RconServerDetails, CSVCMsg_RconServerDetails > {};
class CSVCMsg_UserMessage_t :                public CNetMessagePBTyped< svc_UserMessage, CSVCMsg_UserMessage, SIGNONGROUP_USER_MESSAGES > {};
class CSVCMsg_Broadcast_Command_t :          public CNetMessagePBTyped< svc_Broadcast_Command, CSVCMsg_Broadcast_Command, SIGNONGROUP_HLTV_REPLAY > {};
class CSVCMsg_HltvFixupOperatorStatus_t :    public CNetMessagePBTyped< svc_HltvFixupOperatorStatus, CSVCMsg_HltvFixupOperatorStatus > {};
class CSVCMsg_UserCommands_t :               public CNetMessagePBTyped< svc_UserCmds, CSVCMsg_UserCommands, SIGNONGROUP_HLTV_REPLAY > {};

#endif // SDK_NETMESSAGES_H
