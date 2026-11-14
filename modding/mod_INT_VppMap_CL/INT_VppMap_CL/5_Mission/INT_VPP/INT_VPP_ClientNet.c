// INT_VPP «Экспедиция» B — CLIENT network side (NET). Reference: NET_API.md.
// INT_VPP_ClientRpc = CF receivers (INT_Settings, INT_Zones, INT_ZoneResult; labels since 2026-11-13: INT_Labels,
// INT_LabelResult) + the CF senders behind INT_VPP_Client (3_Game cache).
// MissionGameplay owns it: registers in the constructor, says Hello 3 s after OnMissionStart (again after 15 s while
// no INT_Settings arrived, at most 3 tries), and at OnMissionFinish unregisters and drops INT_VPP_Client completely.
// Never runs on the dedicated server (MissionGameplay does not exist there; every entry re-checks).

class INT_VPP_ClientRpc : INT_VPP_ClientTransport
{
	protected bool m_Registered;
	protected bool m_ProtoWarned;

	void ~INT_VPP_ClientRpc()
	{
		INT_Unregister();
	}

	void INT_Register()
	{
		if (m_Registered)
			return;
		GetRPCManager().AddRPC(INT_VPP_Net.RPC_NS, INT_VPP_Net.RPC_SETTINGS, this, SingleplayerExecutionType.Client);
		GetRPCManager().AddRPC(INT_VPP_Net.RPC_NS, INT_VPP_Net.RPC_ZONES, this, SingleplayerExecutionType.Client);
		GetRPCManager().AddRPC(INT_VPP_Net.RPC_NS, INT_VPP_Net.RPC_ZONE_RESULT, this, SingleplayerExecutionType.Client);
		GetRPCManager().AddRPC(INT_VPP_Net.RPC_NS, INT_VPP_Net.INT_RPC_LABELS, this, SingleplayerExecutionType.Client);
		GetRPCManager().AddRPC(INT_VPP_Net.RPC_NS, INT_VPP_Net.INT_RPC_LABEL_RESULT, this, SingleplayerExecutionType.Client);
		GetRPCManager().AddRPC(INT_VPP_Net.RPC_NS, INT_VPP_Net.INT_RPC_RF_IMPORT, this, SingleplayerExecutionType.Client);
		m_Registered = true;
	}

	void INT_Unregister()
	{
		if (!m_Registered)
			return;
		m_Registered = false;
		if (!GetGame())
			return;
		GetRPCManager().RemoveRPC(INT_VPP_Net.RPC_NS, INT_VPP_Net.RPC_SETTINGS);
		GetRPCManager().RemoveRPC(INT_VPP_Net.RPC_NS, INT_VPP_Net.RPC_ZONES);
		GetRPCManager().RemoveRPC(INT_VPP_Net.RPC_NS, INT_VPP_Net.RPC_ZONE_RESULT);
		GetRPCManager().RemoveRPC(INT_VPP_Net.RPC_NS, INT_VPP_Net.INT_RPC_LABELS);
		GetRPCManager().RemoveRPC(INT_VPP_Net.RPC_NS, INT_VPP_Net.INT_RPC_LABEL_RESULT);
		GetRPCManager().RemoveRPC(INT_VPP_Net.RPC_NS, INT_VPP_Net.INT_RPC_RF_IMPORT);
	}

	// ---- senders (INT_VPP_ClientTransport) ----
	override bool SendHello()
	{
		if (GetGame().IsDedicatedServer())
			return false;
		int intProto = INT_VPP_Net.PROTOCOL;
		Param1<int> prm = new Param1<int>(intProto);
		GetRPCManager().SendRPC(INT_VPP_Net.RPC_NS, INT_VPP_Net.RPC_HELLO, prm, true, null);
		return true;
	}

	override bool SendZoneSave(INT_VPP_Zone z)
	{
		if (GetGame().IsDedicatedServer())
			return false;
		if (!z)
			return false;
		int intProto = INT_VPP_Net.PROTOCOL;
		Param2<int, ref INT_VPP_Zone> prm = new Param2<int, ref INT_VPP_Zone>(intProto, z);
		GetRPCManager().SendRPC(INT_VPP_Net.RPC_NS, INT_VPP_Net.RPC_ZONE_SAVE, prm, true, null);
		return true;
	}

	override bool SendZoneDelete(int id)
	{
		if (GetGame().IsDedicatedServer())
			return false;
		int intProto = INT_VPP_Net.PROTOCOL;
		Param2<int, int> prm = new Param2<int, int>(intProto, id);
		GetRPCManager().SendRPC(INT_VPP_Net.RPC_NS, INT_VPP_Net.RPC_ZONE_DELETE, prm, true, null);
		return true;
	}

	// admin text labels (2026-11-13): client -> server, admin only (the server re-checks before reading)
	override bool INT_SendLabelSave(INT_VPP_Label l)
	{
		if (GetGame().IsDedicatedServer())
			return false;
		if (!l)
			return false;
		int intProto = INT_VPP_Net.PROTOCOL;
		Param2<int, ref INT_VPP_Label> prm = new Param2<int, ref INT_VPP_Label>(intProto, l);
		GetRPCManager().SendRPC(INT_VPP_Net.RPC_NS, INT_VPP_Net.INT_RPC_LABEL_SAVE, prm, true, null);
		return true;
	}

	override bool INT_SendLabelDelete(int id)
	{
		if (GetGame().IsDedicatedServer())
			return false;
		int intProto = INT_VPP_Net.PROTOCOL;
		Param2<int, int> prm = new Param2<int, int>(intProto, id);
		GetRPCManager().SendRPC(INT_VPP_Net.RPC_NS, INT_VPP_Net.INT_RPC_LABEL_DELETE, prm, true, null);
		return true;
	}

	// server markers (2026-11-14): map admins only (the server checks again before reading the payload)
	override bool INT_SendSrvMarkerSave(MarkerInfo m)
	{
		if (GetGame().IsDedicatedServer())
			return false;
		if (!m)
			return false;
		int intProto = INT_VPP_Net.PROTOCOL;
		Param2<int, ref MarkerInfo> prm = new Param2<int, ref MarkerInfo>(intProto, m);
		GetRPCManager().SendRPC(INT_VPP_Net.RPC_NS, INT_VPP_Net.INT_RPC_SRVMARKER_SAVE, prm, true, null);
		return true;
	}

	override bool INT_SendSrvMarkerDelete(int id)
	{
		if (GetGame().IsDedicatedServer())
			return false;
		int intProto = INT_VPP_Net.PROTOCOL;
		Param2<int, int> prm = new Param2<int, int>(intProto, id);
		GetRPCManager().SendRPC(INT_VPP_Net.RPC_NS, INT_VPP_Net.INT_RPC_SRVMARKER_DELETE, prm, true, null);
		return true;
	}

	// ---- receivers (CF calls them by name) ----
	// server -> client: Param2<int protocol, INT_VPP_ClientSettings>
	void INT_Settings(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client)
			return;
		if (GetGame().IsDedicatedServer())
			return;
		Param2<int, ref INT_VPP_ClientSettings> data;
		if (!ctx.Read(data))
			return;
		if (!data)
			return;
		int intProto = data.param1;
		if (intProto != INT_VPP_Net.PROTOCOL)
		{
			INT_WarnProto(intProto);
			return;
		}
		INT_VPP_ClientSettings s = data.param2;
		if (!s)
			return;
		INT_VPP_Client cl = INT_VPP_Client.Get();
		cl.ApplySettings(s);
	}

	// server -> client: Param6<int protocol, int msgId, int part, int parts, int changedId, array<INT_VPP_Zone>>
	void INT_Zones(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client)
			return;
		if (GetGame().IsDedicatedServer())
			return;
		Param6<int, int, int, int, int, ref array<ref INT_VPP_Zone>> data;
		if (!ctx.Read(data))
			return;
		if (!data)
			return;
		int intProto = data.param1;
		if (intProto != INT_VPP_Net.PROTOCOL)
		{
			INT_WarnProto(intProto);
			return;
		}
		int msgId = data.param2;
		int part = data.param3;
		int parts = data.param4;
		int changedId = data.param5;
		array<ref INT_VPP_Zone> list = data.param6;
		INT_VPP_Client cl = INT_VPP_Client.Get();
		cl.ApplyZonesPart(msgId, part, parts, changedId, list);
	}

	// server -> THIS client only: Param5<int protocol, int kind, int zoneId, bool ok, string reason> (answer to our
	// own save / delete; a refusal is followed by a fresh INT_Settings at most every 2 s).
	void INT_ZoneResult(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client)
			return;
		if (GetGame().IsDedicatedServer())
			return;
		Param5<int, int, int, bool, string> data;
		if (!ctx.Read(data))
			return;
		if (!data)
			return;
		int intProto = data.param1;
		if (intProto != INT_VPP_Net.PROTOCOL)
		{
			INT_WarnProto(intProto);
			return;
		}
		int kind = data.param2;
		int zoneId = data.param3;
		bool ok = data.param4;
		string reason = data.param5;
		INT_VPP_Client cl = INT_VPP_Client.Get();
		cl.ApplyResult(kind, zoneId, ok, reason);
	}

	// server -> client: Param7<int protocol, int msgId, int part, int parts, int changedId, int maxLabels, array<INT_VPP_Label>>
	void INT_Labels(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client)
			return;
		if (GetGame().IsDedicatedServer())
			return;
		Param7<int, int, int, int, int, int, ref array<ref INT_VPP_Label>> data;
		if (!ctx.Read(data))
			return;
		if (!data)
			return;
		int intProto = data.param1;
		if (intProto != INT_VPP_Net.PROTOCOL)
		{
			INT_WarnProto(intProto);
			return;
		}
		int msgId = data.param2;
		int part = data.param3;
		int parts = data.param4;
		int changedId = data.param5;
		int maxLabels = data.param6;
		array<ref INT_VPP_Label> list = data.param7;
		INT_VPP_Client cl = INT_VPP_Client.Get();
		cl.INT_ApplyLabelsPart(msgId, part, parts, changedId, maxLabels, list);
	}

	// server -> THIS client only: Param5<int protocol, int kind, int labelId, bool ok, string reason>
	void INT_LabelResult(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client)
			return;
		if (GetGame().IsDedicatedServer())
			return;
		Param5<int, int, int, bool, string> data;
		if (!ctx.Read(data))
			return;
		if (!data)
			return;
		int intProto = data.param1;
		if (intProto != INT_VPP_Net.PROTOCOL)
		{
			INT_WarnProto(intProto);
			return;
		}
		int kind = data.param2;
		int labelId = data.param3;
		bool ok = data.param4;
		string reason = data.param5;
		INT_VPP_Client cl = INT_VPP_Client.Get();
		cl.INT_ApplyLabelResult(kind, labelId, ok, reason);
		// server markers (2026-11-14) answer on the same RPC with kinds 5 / 6
		cl.INT_ApplySrvMarkerResult(kind, labelId, ok, reason);
	}

	// server -> client (2026-11-13): Param2<int protocol, int on>; only a server with Settings.json
	// INT_ImportRFMapMarkers 1 sends it. The player's RF_MAP markers are imported once per server (INT_VPP_RFImport).
	void INT_RFImport(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Client)
			return;
		if (GetGame().IsDedicatedServer())
			return;
		Param2<int, int> data;
		if (!ctx.Read(data))
			return;
		if (!data)
			return;
		int intProto = data.param1;
		if (intProto != INT_VPP_Net.PROTOCOL)
		{
			INT_WarnProto(intProto);
			return;
		}
		int on = data.param2;
		INT_VPP_RFIResult res = INT_VPP_RFImport.INT_OnServerAllows(on);
		if (!res)
			return;
		if (res.m_INT_Code != INT_VPP_RFImport.INT_RES_IMPORTED || res.m_INT_Added <= 0)
			return;
		// the new markers: 3D now, and on the map if it is open
		g_Game.Display3dMarkers();
		UIManager um = GetGame().GetUIManager();
		if (!um)
			return;
		VPPMapMenu menu = VPPMapMenu.Cast(um.FindMenu(VPP_MENU_MAP));
		if (menu)
			menu.DisplayClientMarkers();
	}

	protected void INT_WarnProto(int intProto)
	{
		if (m_ProtoWarned)
			return;
		m_ProtoWarned = true;
		int mine = INT_VPP_Net.PROTOCOL;
		Print("[INT_VPP] server protocol " + intProto.ToString() + " differs from client protocol " + mine.ToString() + ", map data ignored");
	}
}

modded class MissionGameplay
{
	protected ref INT_VPP_ClientRpc m_INT_VPP_ClientRpc;
	protected int m_INT_VPP_HelloTries;

	void MissionGameplay()
	{
		if (GetGame().IsDedicatedServer())
			return;
		// 2026-11-12: Client.json moves once to $profile:INT_Mods/INT_VppMap/ before anything reads it
		INT_VPP_Prefs.INT_MigrateOnce();
		// 2026-11-13: a new session may import RF_MAP markers once (when its server asks for it)
		INT_VPP_RFImport.INT_ResetSession();
		INT_VPP_ClientRpc rpc = new INT_VPP_ClientRpc();
		m_INT_VPP_ClientRpc = rpc;
		rpc.INT_Register();
		INT_VPP_Client cl = INT_VPP_Client.Get();
		cl.SetTransport(rpc);
	}

	override void OnMissionStart()
	{
		super.OnMissionStart();
		if (GetGame().IsDedicatedServer())
			return;
		m_INT_VPP_HelloTries = 0;
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(this.INT_VPP_HelloTick);
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(this.INT_VPP_HelloTick, INT_VPP_Net.HELLO_FIRST_DELAY_MS, false);
	}

	override void OnMissionFinish()
	{
		super.OnMissionFinish();
		if (GetGame().IsDedicatedServer())
			return;
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(this.INT_VPP_HelloTick);
		if (m_INT_VPP_ClientRpc)
			m_INT_VPP_ClientRpc.INT_Unregister();
		m_INT_VPP_ClientRpc = null;
		INT_VPP_Client.Shutdown();
	}

	// Hello now; check again in HELLO_RETRY_MS; stop when INT_Settings arrived or after HELLO_MAX_TRIES hellos.
	void INT_VPP_HelloTick()
	{
		INT_VPP_Client cl = INT_VPP_Client.Peek();
		if (!cl)
			return;
		if (cl.HasSettings())
			return;
		int tries = m_INT_VPP_HelloTries;
		if (tries >= INT_VPP_Net.HELLO_MAX_TRIES)
		{
			Print("[INT_VPP] no answer from the server after " + tries.ToString() + " hellos (zones / map admin off)");
			return;
		}
		m_INT_VPP_HelloTries++;
		cl.SendHello();
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(this.INT_VPP_HelloTick, INT_VPP_Net.HELLO_RETRY_MS, false);
	}
}
