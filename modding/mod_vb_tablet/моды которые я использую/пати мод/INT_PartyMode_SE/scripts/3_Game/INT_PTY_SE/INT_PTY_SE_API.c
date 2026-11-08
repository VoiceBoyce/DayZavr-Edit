// INT PartyMode — public SERVER API for other mods (spec.md §2 D12). Detect the mod with `#ifdef INT_PARTYMODE_SE`
// (the define exists only where the server part is loaded) and call these statics from any script module of the
// server. Players are identified by Steam64 (PlayerIdentity.GetPlainId()).
//
//   INT_PTY_SE_API.AreFriends(a, b)             — true when a and b are friends (mutual by design)
//   INT_PTY_SE_API.GetFriends(steam, ids)       — fills ids with the friends of steam, returns the count
//   INT_PTY_SE_API.OnFriendsChanged()           — ScriptInvoker, called with (string a, string b, bool added)
//
// Cost: memory lookups while one of the players is online; a query about two OFFLINE players reads one small file.

class INT_PTY_SE_API
{
	static const int API_VERSION = 1;

	protected static ref ScriptInvoker s_OnFriendsChanged;

	static ScriptInvoker OnFriendsChanged()
	{
		if (!s_OnFriendsChanged)
			s_OnFriendsChanged = new ScriptInvoker();
		return s_OnFriendsChanged;
	}

	static bool AreFriends(string a, string b)
	{
		if (a == b || !INT_PTY_Util.IsSteamId(a) || !INT_PTY_Util.IsSteamId(b))
			return false;
		INT_PTY_SE_PlayerFile f = INT_PTY_SE_Store.Cached(a);
		if (f)
			return f.Has(b);
		f = INT_PTY_SE_Store.Cached(b);
		if (f)
			return f.Has(a);
		f = INT_PTY_SE_Store.Peek(a);
		return f.Has(b);
	}

	static int GetFriends(string steam, array<string> ids)
	{
		if (!ids)
			return 0;
		ids.Clear();
		if (!INT_PTY_Util.IsSteamId(steam))
			return 0;
		INT_PTY_SE_PlayerFile f = INT_PTY_SE_Store.Peek(steam);
		foreach (INT_PTY_SE_FriendEntry e : f.Friends)
		{
			ids.Insert(e.Id);
		}
		return ids.Count();
	}

	// Internal: called by the core after both files are saved.
	static void Notify(string a, string b, bool added)
	{
		if (s_OnFriendsChanged)
			s_OnFriendsChanged.Invoke(a, b, added);
	}
}
