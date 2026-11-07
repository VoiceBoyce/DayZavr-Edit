// INT_PDA (server) — registration and login (spec.md §3, owner 2026-10-22): one account per SteamID, the callsign is
// Latin letters only and unique (Names/ index, no scans), the login checks callsign + password against the sender's
// OWN account (nobody can log into another account). Wrong passwords lock the login for a while. On the first login
// with a PDA the PDA remembers its owner (looted PDA = trophy).

class INT_PDA_SE_Auth
{
	static int s_Registered;		// counters for Debug and the self-test
	static int s_Logins;
	static int s_LoginFails;
	// login locks by SteamID: the session goes away on disconnect, a reconnect must not lift the lock (mission time)
	protected static ref map<string, int> s_Locks = new map<string, int>();

	static void OnRegister(PlayerIdentity sender, PlayerBase player, INT_PDA_SE_Session s, ParamsReadContext ctx)
	{
		string callsign;
		string password;
		string discord;
		int avatar;
		int low;
		int high;
		if (!ctx.Read(callsign) || !ctx.Read(password) || !ctx.Read(discord) || !ctx.Read(avatar) || !ctx.Read(low) || !ctx.Read(high))
		{
			AuthResult(sender, INT_PDA_Const.R_BAD_DATA, null);
			return;
		}
		if (INT_PDA_SE_Store.HasAccount(s.Steam))
		{
			AuthResult(sender, INT_PDA_Const.R_ALREADY_REGISTERED, null);
			return;
		}
		callsign.TrimInPlace();
		if (!INT_PDA_Util.IsCallsign(callsign))
		{
			AuthResult(sender, INT_PDA_Const.R_CALLSIGN_FORMAT, null);
			return;
		}
		if (!INT_PDA_Util.IsPassword(password))
		{
			AuthResult(sender, INT_PDA_Const.R_PASSWORD_FORMAT, null);
			return;
		}
		discord = INT_PDA_Util.CleanLine(discord, INT_PDA_Const.DISCORD_MAX, INT_PDA_Const.MAX_NAME_BYTES);
		if (discord == "")
		{
			AuthResult(sender, INT_PDA_Const.R_DISCORD_FORMAT, null);
			return;
		}
		string owner = INT_PDA_SE_Store.NameOwner(callsign);
		if (owner != "" && owner != s.Steam)
		{
			AuthResult(sender, INT_PDA_Const.R_CALLSIGN_TAKEN, null);
			return;
		}
		INT_PDA_Account a = INT_PDA_Account.ForRead();
		a.SteamId = s.Steam;
		a.Callsign = callsign;
		a.Password = password;
		a.Discord = discord;
		a.Avatar = INT_PDA_Util.ClampI(avatar, 0, INT_PDA_Const.AVATARS - 1);
		a.Registered = INT_PDA_Util.Stamp();
		a.LastSeen = a.Registered;
		a.GameName = INT_PDA_SE_Core.NameOf(sender);
		if (!INT_PDA_SE_Store.SetName(callsign, s.Steam) || !INT_PDA_SE_Store.SaveAccount(a))
		{
			INT_PDA_SE_Store.DropNameOf(callsign, s.Steam);
			AuthResult(sender, INT_PDA_Const.R_WRITE_FAILED, null);
			return;
		}
		INT_PDA_SE_Store.CacheAccount(a);
		s.LoggedIn = true;
		s.Fails = 0;
		s_Registered++;
		BindPda(player, s.Steam, low, high);
		INT_PDA_Log.Info("регистрация: " + callsign + " (" + s.Steam + ", " + INT_PDA_Log.Clip(a.GameName, 32) + ")");
		AuthResult(sender, INT_PDA_Const.R_OK, a);
	}

	static void OnLogin(PlayerIdentity sender, PlayerBase player, INT_PDA_SE_Session s, ParamsReadContext ctx, int now)
	{
		string callsign;
		string password;
		int low;
		int high;
		if (!ctx.Read(callsign) || !ctx.Read(password) || !ctx.Read(low) || !ctx.Read(high))
		{
			AuthResult(sender, INT_PDA_Const.R_BAD_DATA, null);
			return;
		}
		int lockedUntil;
		if (s_Locks.Find(s.Steam, lockedUntil))
		{
			if (now < lockedUntil && lockedUntil > s.LockUntilMs)
				s.LockUntilMs = lockedUntil;
			else if (now >= lockedUntil)
				s_Locks.Remove(s.Steam);
		}
		if (now < s.LockUntilMs)
		{
			AuthResult(sender, INT_PDA_Const.R_LOCKED, null);
			return;
		}
		INT_PDA_Account a = INT_PDA_SE_Store.Account(s.Steam);
		if (!a)
		{
			AuthResult(sender, INT_PDA_Const.R_NOT_REGISTERED, null);
			return;
		}
		callsign.TrimInPlace();
		if (INT_PDA_Util.CallsignKey(callsign) != INT_PDA_Util.CallsignKey(a.Callsign) || password != a.Password)
		{
			s.Fails++;
			s_LoginFails++;
			if (s.Fails >= INT_PDA_SE_Store.Cfg().LoginFails)
			{
				s.Fails = 0;
				s.LockUntilMs = now + INT_PDA_SE_Store.Cfg().LoginLockSeconds * 1000;
				s_Locks.Set(s.Steam, s.LockUntilMs);
				INT_PDA_Log.Warn("вход заблокирован после неверных паролей: " + a.Callsign + " (" + s.Steam + ")");
			}
			AuthResult(sender, INT_PDA_Const.R_WRONG_LOGIN, null);
			return;
		}
		s.LoggedIn = true;
		s.Fails = 0;
		s_Logins++;
		// one write a day at most: last seen date and the current DayZ nick for the admin tab
		string today = INT_PDA_Util.Day();
		string gameName = INT_PDA_SE_Core.NameOf(sender);
		if (a.LastSeen.IndexOf(today) != 0 || a.GameName != gameName)
		{
			a.LastSeen = INT_PDA_Util.Stamp();
			a.GameName = gameName;
			INT_PDA_SE_Store.SaveAccount(a);
		}
		BindPda(player, s.Steam, low, high);
		INT_PDA_Log.Dbg("вход: " + a.Callsign + " (" + s.Steam + ")");
		AuthResult(sender, INT_PDA_Const.R_OK, a);
		if (a.InFaction())
			INT_PDA_SE_Faction.SendFaction(sender, s, a);
	}

	static void OnLogout(PlayerIdentity sender, INT_PDA_SE_Session s)
	{
		s.LoggedIn = false;
		s.Listening = false;
		INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_LOGOUT, INT_PDA_Const.R_OK);
	}

	static void OnSetCallsign(PlayerIdentity sender, INT_PDA_SE_Session s, ParamsReadContext ctx)
	{
		string callsign;
		if (!ctx.Read(callsign))
			return;
		INT_PDA_Account a = INT_PDA_SE_Core.LoggedAccount(s);
		if (!a)
		{
			INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_SET_CALLSIGN, INT_PDA_Const.R_NOT_LOGGED_IN);
			return;
		}
		int code = ChangeCallsign(a, callsign);
		INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_SET_CALLSIGN, code);
		if (code == INT_PDA_Const.R_OK)
			INT_PDA_SE_Core.SendProfile(sender, a);
	}

	// Shared with the admin tab. Keeps the name index, the faction member list and the account in step.
	static int ChangeCallsign(INT_PDA_Account a, string callsign)
	{
		callsign.TrimInPlace();
		// the very same callsign: nothing to write
		if (callsign == a.Callsign)
			return INT_PDA_Const.R_OK;
		if (!INT_PDA_Util.IsCallsign(callsign))
			return INT_PDA_Const.R_CALLSIGN_FORMAT;
		string owner = INT_PDA_SE_Store.NameOwner(callsign);
		if (owner != "" && owner != a.SteamId)
			return INT_PDA_Const.R_CALLSIGN_TAKEN;
		string old = a.Callsign;
		bool sameKey = INT_PDA_Util.CallsignKey(old) == INT_PDA_Util.CallsignKey(callsign);
		array<string> oldList = new array<string>();
		oldList.Copy(a.OldCallsigns);
		if (!INT_PDA_SE_Store.SetName(callsign, a.SteamId))
			return INT_PDA_Const.R_WRITE_FAILED;
		a.Callsign = callsign;
		// only the case changed (same name key): not a former callsign
		if (old != "" && !sameKey)
			a.OldCallsigns.Insert(old);
		a.Clean();
		if (!INT_PDA_SE_Store.SaveAccount(a))
		{
			// the account file still has the old name: roll the index and the cache back (no two owners of one name)
			a.Callsign = old;
			a.OldCallsigns.Copy(oldList);
			if (sameKey)
				INT_PDA_SE_Store.SetName(old, a.SteamId);
			else
				INT_PDA_SE_Store.DropNameOf(callsign, a.SteamId);
			return INT_PDA_Const.R_WRITE_FAILED;
		}
		// the old callsign stays reserved for its owner for CallsignReserveDays (owner 2026-10-23)
		if (!sameKey)
			INT_PDA_SE_Store.ReserveOldName(old, a.SteamId);
		INT_PDA_FactionData f = INT_PDA_SE_Store.Faction(a.FactionId);
		if (f)
		{
			INT_PDA_Member m = f.FindMember(a.SteamId);
			if (m)
			{
				m.Callsign = callsign;
				if (!INT_PDA_SE_Store.SaveFaction(f))
					INT_PDA_Log.Warn("смена позывного: файл группировки " + f.Id + " не записан (в памяти новый позывной)");
				// the mates' «Состав» shows the new callsign at once (not only after their next faction request)
				INT_PDA_SE_Faction.PushFaction(f);
			}
		}
		INT_PDA_Log.Info("смена позывного: " + old + " -> " + callsign + " (" + a.SteamId + ")");
		return INT_PDA_Const.R_OK;
	}

	static void OnSetPassword(PlayerIdentity sender, INT_PDA_SE_Session s, ParamsReadContext ctx)
	{
		string oldPw;
		string newPw;
		if (!ctx.Read(oldPw) || !ctx.Read(newPw))
			return;
		INT_PDA_Account a = INT_PDA_SE_Core.LoggedAccount(s);
		if (!a)
		{
			INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_SET_PASSWORD, INT_PDA_Const.R_NOT_LOGGED_IN);
			return;
		}
		if (oldPw != a.Password)
		{
			INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_SET_PASSWORD, INT_PDA_Const.R_WRONG_LOGIN);
			return;
		}
		if (!INT_PDA_Util.IsPassword(newPw))
		{
			INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_SET_PASSWORD, INT_PDA_Const.R_PASSWORD_FORMAT);
			return;
		}
		a.Password = newPw;
		int code = INT_PDA_Const.R_OK;
		if (!INT_PDA_SE_Store.SaveAccount(a))
			code = INT_PDA_Const.R_WRITE_FAILED;
		INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_SET_PASSWORD, code);
	}

	static void OnSetAvatar(PlayerIdentity sender, INT_PDA_SE_Session s, ParamsReadContext ctx)
	{
		int avatar;
		if (!ctx.Read(avatar))
			return;
		INT_PDA_Account a = INT_PDA_SE_Core.LoggedAccount(s);
		if (!a)
			return;
		a.Avatar = INT_PDA_Util.ClampI(avatar, 0, INT_PDA_Const.AVATARS - 1);
		INT_PDA_SE_Store.SaveAccount(a);
		INT_PDA_SE_Core.SendProfile(sender, a);
	}

	protected static void BindPda(PlayerBase player, string steam, int low, int high)
	{
		INT_PDA pda = INT_PDA_SE_Core.PdaOf(player, low, high);
		if (pda && pda.OwnerHash() == 0)
			pda.SetOwnerServer(steam);
	}

	protected static void AuthResult(PlayerIdentity to, int code, INT_PDA_Account a)
	{
		INT_PDA_SE_Core.NoteResult(INT_PDA_Const.OP_AUTH_RESULT, 0, code);
		ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_AUTH_RESULT);
		rpc.Write(code);
		bool ok = (code == INT_PDA_Const.R_OK) && (a != null);
		rpc.Write(ok);
		if (ok)
			INT_PDA_SE_Core.ProfileOf(a).Write(rpc);
		INT_PDA_SE_Core.SendTo(rpc, to, INT_PDA_Const.OP_AUTH_RESULT);
	}
}
