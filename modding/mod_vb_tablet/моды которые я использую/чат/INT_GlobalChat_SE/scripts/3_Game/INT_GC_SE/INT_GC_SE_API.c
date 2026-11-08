// INT GlobalChat (server) — public API for other server mods (spec.md §4.7, owner D11).
// Soft dependency: wrap calls in #ifdef INT_GLOBALCHAT_SE and check INT_GC_SE_API.API_VERSION. Lines appear in
// every client's chat as «СЕРВЕР» system lines. Only additions in later versions.
//
//   #ifdef INT_GLOBALCHAT_SE
//   INT_GC_SE_API.Broadcast("Рестарт сервера через 15 минут");
//   INT_GC_SE_API.BroadcastKey("STR_MYMOD_RESTART", "15");      // each client translates the key itself
//   INT_GC_SE_API.SendTo(identity, "Ваш заказ готов");
//   #endif

class INT_GC_SE_API
{
	static const int API_VERSION = 1;

	protected static bool Ready()
	{
		return GetGame() && GetGame().IsDedicatedServer();
	}

	protected static string Clean(string text)
	{
		return INT_GC_Util.CleanText(text, INT_GC_SE_Config.Data().MaxMessageLength * 2);
	}

	// System line for everybody.
	static bool Broadcast(string text)
	{
		if (!Ready())
			return false;
		string t = Clean(text);
		if (t == "")
			return false;
		INT_GC_SE_Core.SendSys(null, INT_GC_Const.SYS_TEXT, t, null);
		return true;
	}

	// System line for one player.
	static bool SendTo(PlayerIdentity identity, string text)
	{
		if (!Ready() || !identity)
			return false;
		string t = Clean(text);
		if (t == "")
			return false;
		INT_GC_SE_Core.SendSys(identity, INT_GC_Const.SYS_TEXT, t, null);
		return true;
	}

	// Stringtable key (with or without '#') and up to three %1..%3 arguments; translated by every client.
	static bool BroadcastKey(string key, string a1 = "", string a2 = "", string a3 = "")
	{
		if (!Ready())
			return false;
		return SendKeyInternal(null, key, a1, a2, a3);
	}

	static bool SendKeyTo(PlayerIdentity identity, string key, string a1 = "", string a2 = "", string a3 = "")
	{
		if (!Ready() || !identity)
			return false;
		return SendKeyInternal(identity, key, a1, a2, a3);
	}

	protected static bool SendKeyInternal(PlayerIdentity identity, string key, string a1, string a2, string a3)
	{
		string k = key + "";
		k.TrimInPlace();
		if (k == "" || k.Length() > INT_GC_Const.MAX_KEY_BYTES)
			return false;
		array<string> args = new array<string>();
		args.Insert(Clean(a1));
		args.Insert(Clean(a2));
		args.Insert(Clean(a3));
		INT_GC_SE_Core.SendSys(identity, INT_GC_Const.SYS_KEY, k, args);
		return true;
	}

	// SteamID64 has the permanent chat ban (file or automoderation this restart).
	static bool IsMuted(string steamId64)
	{
		return INT_GC_SE_Config.IsMuted(steamId64);
	}
}
