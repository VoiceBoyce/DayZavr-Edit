// INT Quests (server) — start / stop and player connect / disconnect (spec.md §4.1). Called from MissionServer.

class INT_QST_SE_Main
{
	static bool s_Started;

	static void Start()
	{
		if (s_Started)
			return;
		s_Started = true;
		INT_QST_Log.s_Tag = "[INT_QST_SE]";
		INT_QST_SE_FileLog.Open();
		INT_QST_SE_Store.LoadAll();
		INT_QST_SE_Players.Init();
		INT_QST_SE_Givers.SpawnAll();
		INT_QST_SE_Hub.s_OnKill.Insert(INT_QST_SE_Progress.OnKill);
		INT_QST_SE_Hub.s_OnOrphanKill.Insert(INT_QST_SE_Grenades.OnOrphanKill);
		INT_QST_SE_Core.Init();
		string bank = "нет";
		if (INT_QST_SE_Bank.IsAvailable())
			bank = "да";
		string info = "квестов " + INT_QST_SE_Store.s_Catalog.m_List.Count().ToString() + ", веток " + INT_QST_SE_Store.s_Catalog.m_BranchOrder.Count().ToString();
		info = info + ", квестовиков " + INT_QST_GiverReg.s_Slots.Count().ToString() + ", админов " + INT_QST_SE_Store.s_Config.Admins.Count().ToString() + ", деньги на счёт: " + bank;
		INT_QST_Log.Info(info);
		INT_QST_SE_FileLog.Line("Старт: " + info + ", проблем " + INT_QST_SE_Store.s_Problems.Count().ToString());
		foreach (string pr : INT_QST_SE_Store.s_Problems)
			INT_QST_SE_FileLog.Line("  проблема: " + pr);
		if (!INT_QST_SE_Bank.Compiled())
			INT_QST_Log.Info("INT_Banking не загружен — денежные награды недоступны");
		INT_QST_Log.Raw("ready " + INT_QST_Const.VERSION);
	}

	static void Stop()
	{
		INT_QST_SE_Core.Shutdown();
		if (INT_QST_SE_Hub.s_OnKill)
			INT_QST_SE_Hub.s_OnKill.Remove(INT_QST_SE_Progress.OnKill);
		if (INT_QST_SE_Hub.s_OnOrphanKill)
			INT_QST_SE_Hub.s_OnOrphanKill.Remove(INT_QST_SE_Grenades.OnOrphanKill);
		INT_QST_SE_FileLog.Line("Остановка сервера");
	}

	static void OnConnect(PlayerBase player, PlayerIdentity identity)
	{
		if (!identity)
			return;
		INT_QST_SE_Players.Open(identity);
	}

	// `uid` from vanilla PlayerDisconnected is identity.GetId() (the HASHED id, missionserver.c), and the identity may
	// already be gone: the session is then found by that hash (review 2026-10-22: sessions of such players never closed).
	static void OnDisconnect(PlayerIdentity identity, string uid)
	{
		string id = "";
		if (identity)
			id = identity.GetPlainId();
		else
		{
			INT_QST_SE_Session hs = INT_QST_SE_Players.FindByHash(uid);
			if (hs)
				id = hs.m_Uid;
		}
		if (id == "")
			return;
		INT_QST_SE_Players.Close(id);
		INT_QST_SE_Core.Forget(id);
	}
}
