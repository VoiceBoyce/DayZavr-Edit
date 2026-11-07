// INT_PDA (server) — «Персонаж» tab (owner 2026-10-26: the vanilla character choice works badly). The chosen survivor
// class lives in the account (Character); MissionServer.OnClientNewEvent makes every new character of this SteamID of
// that class (first join, after a death, after «Переродиться»; INT_PDA_SE_Mission forces the vanilla picks). «Переродиться» = save + the current character dies
// like the vanilla respawn of an unconscious one (admin log line, SetHealth 0, the body keeps all items); the client
// then asks the engine for the respawn itself. Once per CharacterRespawnMinutes per player (memory only: a restart
// clears it). No timers, no scans.

class INT_PDA_SE_Char
{
	static ref map<string, int> s_LastRespawnMs = new map<string, int>();
	static int s_Applied;			// new characters made of a chosen class (Debug, self-test)
	static int s_Reborn;			// characters killed by «Переродиться»

	static INT_PDA_SE_Config Cfg()
	{
		return INT_PDA_SE_Store.Cfg();
	}

	// MissionServer.OnClientNewEvent: the class a new character of this SteamID must have ("" = the game decides).
	static string TypeFor(string steam)
	{
		if (Cfg().CharacterSelect == 0 || !INT_PDA_Util.IsSteamId(steam))
			return "";
		INT_PDA_Account a = INT_PDA_SE_Store.PeekAccount(steam);
		if (!a || a.Character == "")
			return "";
		if (!Cfg().OffersCharacter(a.Character))
		{
			INT_PDA_Log.Dbg("персонаж " + a.Character + " больше не в списке Characters — игра выбирает сама: " + steam);
			return "";
		}
		return a.Character;
	}

	// After the vanilla creation: counts and logs what was made (a mod that makes the character its own way shows here).
	static void Applied(PlayerIdentity identity, PlayerBase player)
	{
		if (!identity || !player)
			return;
		string want = TypeFor(identity.GetPlainId());
		if (want == "")
			return;
		string made = player.GetType();
		if (made == want)
		{
			s_Applied++;
			INT_PDA_Log.Dbg("новый персонаж " + identity.GetPlainId() + ": " + made);
			return;
		}
		INT_PDA_Log.Warn("новый персонаж " + identity.GetPlainId() + ": " + made + " вместо выбранного " + want + " (персонажа создаёт другой мод или init.c миссии)");
	}

	static int WaitSec(string steam, int now)
	{
		int minutes = Cfg().CharacterRespawnMinutes;
		int last;
		if (minutes <= 0 || !s_LastRespawnMs.Find(steam, last))
			return 0;
		int left = last + minutes * 60000 - now;
		if (left <= 0)
			return 0;
		return (left + 999) / 1000;
	}

	// --- requests ---------------------------------------------------------------------------------------------------
	static void OnGet(PlayerIdentity sender, INT_PDA_SE_Session s, int now)
	{
		INT_PDA_Account a = INT_PDA_SE_Core.LoggedAccount(s);
		if (!a)
		{
			INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_CHAR, INT_PDA_Const.R_NOT_LOGGED_IN);
			return;
		}
		SendData(sender, s, a, now);
	}

	static void OnSave(PlayerIdentity sender, INT_PDA_SE_Session s, ParamsReadContext ctx, int now)
	{
		string type;
		if (!ctx.Read(type))
			return;
		INT_PDA_Account a;
		int code = Check(s, type, a);
		if (code == INT_PDA_Const.R_OK)
			code = Store(a, type);
		INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_CHAR_SAVE, code);
		if (a)
			SendData(sender, s, a, now);
	}

	static void OnRespawn(PlayerIdentity sender, PlayerBase player, INT_PDA_SE_Session s, ParamsReadContext ctx, int now)
	{
		string type;
		if (!ctx.Read(type))
			return;
		INT_PDA_Account a;
		int code = Check(s, type, a);
		// a dead / unconscious character is not reborn from the PDA (the vanilla respawn is there for that)
		if (code == INT_PDA_Const.R_OK && player && (!player.IsAlive() || player.IsUnconscious()))
			code = INT_PDA_Const.R_BAD_DATA;
		if (code == INT_PDA_Const.R_OK && WaitSec(s.Steam, now) > 0)
			code = INT_PDA_Const.R_WAIT;
		if (code == INT_PDA_Const.R_OK)
			code = Store(a, type);
		if (code == INT_PDA_Const.R_OK)
		{
			s_LastRespawnMs.Set(s.Steam, now);
			s_Reborn++;
			INT_PDA_Log.Info("перерождение: " + a.Callsign + " (" + s.Steam + ") -> " + type);
			Kill(player);
		}
		INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_CHAR_RESPAWN, code);
		if (a)
			SendData(sender, s, a, now);
	}

	// Logged in, the feature on, the class offered.
	protected static int Check(INT_PDA_SE_Session s, string type, out INT_PDA_Account a)
	{
		a = INT_PDA_SE_Core.LoggedAccount(s);
		if (!a)
			return INT_PDA_Const.R_NOT_LOGGED_IN;
		if (Cfg().CharacterSelect == 0)
			return INT_PDA_Const.R_NO_RIGHTS;
		if (!Cfg().OffersCharacter(type))
			return INT_PDA_Const.R_BAD_DATA;
		return INT_PDA_Const.R_OK;
	}

	protected static int Store(INT_PDA_Account a, string type)
	{
		if (a.Character == type)
			return INT_PDA_Const.R_OK;
		string old = a.Character;
		a.Character = type;
		if (INT_PDA_SE_Store.SaveAccount(a))
			return INT_PDA_Const.R_OK;
		a.Character = old;
		return INT_PDA_Const.R_WRITE_FAILED;
	}

	// The vanilla way of OnClientRespawnEvent for an unconscious / restrained character: admin log line, health 0.
	protected static void Kill(PlayerBase player)
	{
		if (!player || !player.IsAlive())
			return;
		PluginAdminLog adm = PluginAdminLog.Cast(GetPlugin(PluginAdminLog));
		if (adm)
			adm.PlayerKilledByRespawn(player);
		player.SetHealth("", "", 0.0);
	}

	// --- answer -----------------------------------------------------------------------------------------------------
	static INT_PDA_CharData DataFor(INT_PDA_Account a, string steam, int now)
	{
		INT_PDA_CharData d = new INT_PDA_CharData();
		d.Enabled = Cfg().CharacterSelect != 0;
		if (d.Enabled)
			d.Types.InsertAll(Cfg().Characters);
		if (a)
			d.Chosen = a.Character;
		d.WaitSec = WaitSec(steam, now);
		d.CooldownMin = Cfg().CharacterRespawnMinutes;
		SpawnWear(steam, d.Wear);
		return d;
	}

	static void SendData(PlayerIdentity to, INT_PDA_SE_Session s, INT_PDA_Account a, int now)
	{
		ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_CHAR_DATA);
		DataFor(a, s.Steam, now).Write(rpc);
		if (to)
			INT_PDA_SE_Core.SendTo(rpc, to, INT_PDA_Const.OP_CHAR_DATA);
		else
			INT_PDA_SE_Core.SendToSession(rpc, s, INT_PDA_Const.OP_CHAR_DATA);
	}

	// The clothes the player's spawn set puts on a new character (the same choice as INT_PDA_SE_Spawn.Dress), for the
	// preview: head, body, legs, feet, back, gloves; empty when the vanilla gear stays (or a faction member is naked).
	static void SpawnWear(string steam, array<string> outList)
	{
		outList.Clear();
		INT_PDA_SpawnSet fallback;
		INT_PDA_SpawnSet clothes = INT_PDA_SE_Spawn.SetFor(steam, fallback);
		if (!clothes || INT_PDA_SE_Spawn.IsNaked(clothes))
			return;
		outList.Insert(clothes.m_HeadItem);
		outList.Insert(clothes.m_TopItem);
		outList.Insert(clothes.m_PantsItem);
		outList.Insert(clothes.m_BootsItem);
		outList.Insert(clothes.m_RukzakItem);
		outList.Insert(clothes.m_PerchatkiItem);
	}
}
