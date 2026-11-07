// INT_PDA (server) — mission hooks: load once at start, spawn of fresh characters (point, clothes, the class chosen
// in the «Персонаж» tab), session end on disconnect, chat history to disk at the end. No OnUpdate, no timers.

modded class MissionServer
{
	override void OnInit()
	{
		super.OnInit();
		INT_PDA_Log.s_Tag = "[INT_PDA_SE]";
		INT_PDA_SE_Store.LoadAll();
		INT_PDA_Log.s_Debug = INT_PDA_SE_Store.Cfg().DebugMode != 0;
		INT_PDA_SE_Core.Init();
		INT_PDA_Log.Info("конфиг: " + INT_PDA_SE_Store.s_Source + ", админов " + INT_PDA_SE_Store.Cfg().Admins.Count().ToString() + ", группировок " + INT_PDA_SE_Store.s_Factions.Count().ToString() + ", отладка " + INT_PDA_SE_Store.Cfg().DebugMode.ToString());
		INT_PDA_Log.Raw("ready " + INT_PDA_Const.VERSION + " (SE " + INT_PDA_SE_Core.SE_BUILD + ")");
	}

	// Fresh character (first join or after death): our point before the vanilla creation, our clothes after it. The
	// class chosen in the «Персонаж» tab is forced for the time of the vanilla OnClientNewEvent: its random pick
	// (DayZGame.CreateRandomPlayer) and its main-menu pick (ProcessLoginData) both give that class. The class can't be
	// set in CreateCharacter alone: the vanilla mission init.c (CustomMission) overrides it without super.
	override PlayerBase OnClientNewEvent(PlayerIdentity identity, vector pos, ParamsReadContext ctx)
	{
		INT_PDA_SpawnSet spawnSet;
		INT_PDA_SpawnSet fallback;
		if (identity)
		{
			spawnSet = INT_PDA_SE_Spawn.SetFor(identity.GetPlainId(), fallback);
			INT_PDA_Chars.s_ForceType = INT_PDA_SE_Char.TypeFor(identity.GetPlainId());
		}
		if (spawnSet)
			pos = INT_PDA_SE_Spawn.PickPos(spawnSet, fallback, pos);
		PlayerBase player = super.OnClientNewEvent(identity, pos, ctx);
		INT_PDA_Chars.s_ForceType = "";
		if (player && spawnSet)
			INT_PDA_SE_Spawn.Dress(player, spawnSet, fallback);
		INT_PDA_SE_Char.Applied(identity, player);
		return player;
	}

	// The main-menu character of the login data (custom respawn mode) is replaced by the chosen class; its clothes stay.
	override bool ProcessLoginData(ParamsReadContext ctx)
	{
		bool ok = super.ProcessLoginData(ctx);
		if (ok && INT_PDA_Chars.s_ForceType != "")
			GetGame().GetMenuDefaultCharacterData(false).SetCharacterType(INT_PDA_Chars.s_ForceType);
		return ok;
	}

	// A mission that keeps the vanilla CreateCharacter (or a cfgPlayerSpawnPresets preset with its own classes): the
	// chosen class there as well.
	override PlayerBase CreateCharacter(PlayerIdentity identity, vector pos, ParamsReadContext ctx, string characterName)
	{
		if (INT_PDA_Chars.s_ForceType != "")
			characterName = INT_PDA_Chars.s_ForceType;
		return super.CreateCharacter(identity, pos, ctx, characterName);
	}

	override void OnClientDisconnectedEvent(PlayerIdentity identity, PlayerBase player, int logoutTime, bool authFailed)
	{
		if (identity)
			INT_PDA_SE_Core.OnDisconnect(identity.GetPlainId());
		super.OnClientDisconnectedEvent(identity, player, logoutTime, authFailed);
	}

	override void OnMissionFinish()
	{
		INT_PDA_SE_Store.Shutdown();
		INT_PDA_SE_Core.Shutdown();
		super.OnMissionFinish();
	}
}
