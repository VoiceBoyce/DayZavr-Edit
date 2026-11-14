//INT (2026-11-12): VPP's server half, moved from the client PBO into the server-only PBO INT_VppMap_SE.
//The 4 RPC names, the RPC_MissionServer namespace and the payloads stay exactly as they were (clients send them by name).
//INT (review, same day): the 4 RPCs are registered FIRST, so a bad VPPMapConfig.json can no longer leave the server
//without them (ServerMarkersCache also sets such a file aside before VPP's throwing reader sees it); each RPC answers
//one sender at most once per 2 s (INT_VPP_ServerState.INT_VppRpcAllow, extras dropped); the file is still re-read for
//every answered request (admins edit it live). The client asks once, 5 s after the mission start.
modded class MissionServer {
    private ref ServerMarkersCache m_ServerMarkersCache;
    void MissionServer() {
        GetRPCManager().AddRPC( "RPC_MissionServer", "MarkerCheck", this, SingleplayerExecutionType.Server );
        GetRPCManager().AddRPC( "RPC_MissionServer", "OwnPositionMarkerDisabled", this, SingleplayerExecutionType.Server );
        GetRPCManager().AddRPC( "RPC_MissionServer", "SessionMarkers", this, SingleplayerExecutionType.Server );
        GetRPCManager().AddRPC( "RPC_MissionServer", "ForceMapItem", this, SingleplayerExecutionType.Server );
        m_ServerMarkersCache = new ServerMarkersCache();
    }

    // This method is only relevant on the server. It will respond to each client with a bool saying whether or not 3d markers are allowed to be used.
    void MarkerCheck( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target ) {
        if (type != CallType.Server || !m_ServerMarkersCache) return;
        if (!INT_VPP_ServerState.INT_VppRpcAllow(sender, "MarkerCheck")) return;
        GetRPCManager().SendRPC( "RPC_MissionGameplay", "MarkerCheck", new Param1<bool>(m_ServerMarkersCache.CanUse3DMarkers()), true, sender);
    }

    // This method is only relevant on the server. It will respond to each client with a bool saying whether or not 3d markers are allowed to be used.
    void OwnPositionMarkerDisabled( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target ) {
        if (type != CallType.Server || !m_ServerMarkersCache) return;
        if (!INT_VPP_ServerState.INT_VppRpcAllow(sender, "OwnPositionMarkerDisabled")) return;
        GetRPCManager().SendRPC( "RPC_MissionGameplay", "OwnPositionMarkerDisabled", new Param1<bool>(m_ServerMarkersCache.OwnPositionMarkerDisabled()), true, sender);
    }

    // This method is only relevant on the server. It will respond to each client with a list of all server markers.
    void SessionMarkers( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target ) {
        if (type != CallType.Server || !m_ServerMarkersCache) return;
        if (!INT_VPP_ServerState.INT_VppRpcAllow(sender, "SessionMarkers")) return;
            m_ServerMarkersCache.LoadCache();
            ref array<ref MarkerInfo> markerInfos = m_ServerMarkersCache.GetCachedArray();
            //INT (2026-11-14): every marker carries its server id (in-game edits by map admins address it)
            INT_VPP_SrvMarkers.INT_AssignIds(markerInfos);

            //Legacy code
            foreach (MarkerInfo markerInfo : markerInfos) {
                string iconPath = markerInfo.GetIconPath();
                iconPath.Replace("\\scripts\\", "\\");
                markerInfo.SetIconPath(iconPath);
            }
            GetRPCManager().SendRPC( "RPC_MissionGameplay", "SessionMarkers", new Param1<ref array<ref MarkerInfo>>(markerInfos), true, sender);
    }

    void ForceMapItem(CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target){
        if (type != CallType.Server || !m_ServerMarkersCache) return;
        if (!INT_VPP_ServerState.INT_VppRpcAllow(sender, "ForceMapItem")) return;
        GetRPCManager().SendRPC( "RPC_MissionGameplay", "ForceMapItem", new Param1<bool>(m_ServerMarkersCache.ForceToUseMapItem()), true, sender);
    }
};