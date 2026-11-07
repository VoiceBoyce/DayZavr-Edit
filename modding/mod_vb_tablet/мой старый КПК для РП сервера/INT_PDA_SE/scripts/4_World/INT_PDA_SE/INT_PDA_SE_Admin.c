// INT_PDA (server) — the «Администрация» tab (owner brief, admin items 1-21). Only SteamIDs from the config. Every
// request is answered once; account lists are paged (AdminPageSize files read per page) and searched through the
// callsign index file NAMES (no file is opened to filter), so even thousands of accounts never stall the server.
// Every change is written to Logs/INT_PDA_Admin_<date>.log in server-machine time.

class INT_PDA_SE_Admin
{
	static int s_Denied;			// counters for Debug and the self-test
	static int s_Requests;

	static void OnAdmin(PlayerIdentity sender, PlayerBase player, INT_PDA_SE_Session s, ParamsReadContext ctx)
	{
		int sub;
		if (!ctx.Read(sub))
			return;
		if (!s.Admin)
		{
			s_Denied++;
			INT_PDA_Log.Warn("админ-запрос от не-админа " + s.Steam + " (" + INT_PDA_Log.Clip(INT_PDA_SE_Core.NameOf(sender), 32) + ")");
			Reply(sender, sub, INT_PDA_Const.R_NO_RIGHTS);
			return;
		}
		s_Requests++;
		switch (sub)
		{
			case INT_PDA_Const.ADM_PLAYERS:
				OnPlayers(sender, ctx);
				return;
			case INT_PDA_Const.ADM_SET_PASSWORD:
				OnSetPassword(sender, s, ctx);
				return;
			case INT_PDA_Const.ADM_DELETE_ACCOUNT:
				OnDeleteAccount(sender, s, ctx);
				return;
			case INT_PDA_Const.ADM_FACTIONS:
				OnFactions(sender, ctx);
				return;
			case INT_PDA_Const.ADM_FACTION_SAVE:
				OnFactionSave(sender, s, ctx);
				return;
			case INT_PDA_Const.ADM_FACTION_DELETE:
				OnFactionDelete(sender, s, ctx);
				return;
			case INT_PDA_Const.ADM_SET_ROLE:
				OnSetRole(sender, s, ctx);
				return;
			case INT_PDA_Const.ADM_KICK:
				OnKick(sender, s, ctx);
				return;
			case INT_PDA_Const.ADM_JOIN:
				OnJoin(sender, s, ctx);
				return;
			case INT_PDA_Const.ADM_RANKS:
				OnRanks(sender, s, ctx);
				return;
			case INT_PDA_Const.ADM_RELATION:
				OnRelation(sender, s, ctx);
				return;
			case INT_PDA_Const.ADM_RANK_RENAME:
				OnRankRename(sender, s, ctx);
				return;
			case INT_PDA_Const.ADM_RELOAD_DB:
				ReloadDb(sender, s);
				return;
			case INT_PDA_Const.ADM_RELOAD_FACTIONS:
				ReloadFactions(sender, s);
				return;
			case INT_PDA_Const.ADM_RELOAD_CONFIG:
				ReloadConfig(sender, s);
				return;
			case INT_PDA_Const.ADM_NEWS_POST:
				OnNewsPost(sender, s, ctx);
				return;
			case INT_PDA_Const.ADM_NEWS_DELETE:
				OnNewsDelete(sender, s, ctx);
				return;
			case INT_PDA_Const.ADM_NEWS_EDIT:
				OnNewsEdit(sender, s, ctx);
				return;
			case INT_PDA_Const.ADM_SETTINGS:
				OnSettings(sender, s, ctx);
				return;
		}
		Reply(sender, sub, INT_PDA_Const.R_BAD_DATA);
	}

	// --- answers ----------------------------------------------------------------------------------------------------
	static ScriptRPC Begin(int sub, int code)
	{
		INT_PDA_SE_Core.NoteResult(INT_PDA_Const.OP_ADMIN, sub, code);
		ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_ADMIN_DATA);
		rpc.Write(sub);
		rpc.Write(code);
		return rpc;
	}

	static void Reply(PlayerIdentity to, int sub, int code)
	{
		INT_PDA_SE_Core.SendTo(Begin(sub, code), to, INT_PDA_Const.OP_ADMIN_DATA);
	}

	static void Audit(INT_PDA_SE_Session s, string text)
	{
		string line = INT_PDA_Util.Stamp() + " | " + s.Steam + " (" + s.GameName + ") | " + text;
		INT_PDA_SE_Files.AppendLine(INT_PDA_SE_Paths.LogDir() + "/INT_PDA_Admin_" + INT_PDA_Util.Day() + ".log", line);
		INT_PDA_Log.Info("админ: " + text);
	}

	static INT_PDA_AdminRow RowOf(INT_PDA_Account a)
	{
		INT_PDA_AdminRow r = new INT_PDA_AdminRow();
		r.SteamId = a.SteamId;
		r.Callsign = a.Callsign;
		r.Password = a.Password;
		r.Discord = a.Discord;
		r.Registered = a.Registered;
		r.LastSeen = a.LastSeen;
		r.GameName = a.GameName;
		r.FactionId = a.FactionId;
		r.Role = a.Role;
		r.Rank = a.Rank;
		r.Avatar = a.Avatar;
		INT_PDA_FactionData f = INT_PDA_SE_Store.Faction(a.FactionId);
		if (f)
			r.FactionName = f.Name;
		r.Online = INT_PDA_SE_Core.Session(a.SteamId) != null;
		return r;
	}

	// --- accounts ---------------------------------------------------------------------------------------------------
	// Page of accounts. filter: "" = all (Players + GP), a SteamID = that account, otherwise a part of a callsign
	// (matched against the NAMES of the index files — nothing is opened to filter).
	static void OnPlayers(PlayerIdentity sender, ParamsReadContext ctx)
	{
		int offset;
		string filter;
		if (!ctx.Read(offset) || !ctx.Read(filter))
			return;
		filter.TrimInPlace();
		int page = INT_PDA_SE_Store.Cfg().AdminPageSize;
		if (offset < 0)
			offset = 0;
		array<string> steams = new array<string>();
		int total = 0;
		string exactOwner = "";
		if (filter != "" && !INT_PDA_Util.IsSteamId(filter))
			exactOwner = INT_PDA_SE_Store.NameOwner(filter);
		if (exactOwner == INT_PDA_SE_Store.OWNER_UNREADABLE)
			exactOwner = "";
		if (INT_PDA_Util.IsSteamId(filter))
		{
			if (INT_PDA_SE_Store.HasAccount(filter))
				steams.Insert(filter);
			total = steams.Count();
		}
		else if (exactOwner != "")
		{
			// exact callsign: one tiny index file, no folder listing
			steams.Insert(exactOwner);
			total = 1;
		}
		else if (filter != "")
		{
			string key = INT_PDA_Util.CallsignKey(filter);
			array<string> names = new array<string>();
			INT_PDA_SE_Files.ListJson(INT_PDA_SE_Paths.NamesDir(), names);
			array<string> hits = new array<string>();
			foreach (string n : names)
			{
				if (n.IndexOf(key) >= 0)
					hits.Insert(n);
			}
			total = hits.Count();
			for (int i = offset; i < hits.Count() && steams.Count() < page; i++)
			{
				string cs = hits[i].Substring(0, hits[i].Length() - 5);
				string owner = INT_PDA_SE_Store.NameOwner(cs);
				if (owner != "" && owner != INT_PDA_SE_Store.OWNER_UNREADABLE)
					steams.Insert(owner);
			}
		}
		else
		{
			array<string> gp = new array<string>();
			array<string> pl = new array<string>();
			INT_PDA_SE_Files.ListJson(INT_PDA_SE_Paths.GpDir(), gp);
			INT_PDA_SE_Files.ListJson(INT_PDA_SE_Paths.PlayersDir(), pl);
			array<string> all = new array<string>();
			all.InsertAll(gp);
			all.InsertAll(pl);
			total = all.Count();
			for (int k = offset; k < all.Count() && steams.Count() < page; k++)
				steams.Insert(all[k].Substring(0, all[k].Length() - 5));
		}
		ScriptRPC rpc = Begin(INT_PDA_Const.ADM_PLAYERS, INT_PDA_Const.R_OK);
		rpc.Write(offset);
		rpc.Write(total);
		array<ref INT_PDA_AdminRow> rows = new array<ref INT_PDA_AdminRow>();
		foreach (string steam : steams)
		{
			INT_PDA_Account a = INT_PDA_SE_Store.Account(steam);
			if (!INT_PDA_SE_Core.Session(steam))
				INT_PDA_SE_Store.ForgetAccount(steam);
			if (a)
			{
				rows.Insert(RowOf(a));
			}
			else if (INT_PDA_SE_Store.HasAccount(steam))
			{
				// the file is there but does not read: a stub row so the admin can find and delete it
				INT_PDA_Account stub = INT_PDA_Account.ForRead();
				stub.SteamId = steam;
				stub.Callsign = "[BROKEN]";
				rows.Insert(RowOf(stub));
			}
		}
		rpc.Write(rows.Count());
		foreach (INT_PDA_AdminRow r : rows)
			r.Write(rpc);
		// trailing: the real page size (AdminPageSize) for a client that reads it; a reader that stops after the
		// rows (the current INT_PDA_Client) is not disturbed by it
		rpc.Write(page);
		INT_PDA_SE_Core.SendTo(rpc, sender, INT_PDA_Const.OP_ADMIN_DATA);
	}

	static void OnSetPassword(PlayerIdentity sender, INT_PDA_SE_Session s, ParamsReadContext ctx)
	{
		string steam;
		string pw;
		if (!ctx.Read(steam) || !ctx.Read(pw))
			return;
		INT_PDA_Account a = INT_PDA_SE_Store.Account(steam);
		if (!a)
		{
			Reply(sender, INT_PDA_Const.ADM_SET_PASSWORD, INT_PDA_Const.R_NOT_FOUND);
			return;
		}
		if (!INT_PDA_Util.IsPassword(pw))
		{
			Reply(sender, INT_PDA_Const.ADM_SET_PASSWORD, INT_PDA_Const.R_PASSWORD_FORMAT);
			return;
		}
		a.Password = pw;
		int code = INT_PDA_Const.R_OK;
		if (!INT_PDA_SE_Store.SaveAccount(a))
			code = INT_PDA_Const.R_WRITE_FAILED;
		if (!INT_PDA_SE_Core.Session(steam))
			INT_PDA_SE_Store.ForgetAccount(steam);
		Audit(s, "сменил пароль аккаунта " + a.Callsign + " (" + steam + ")");
		Reply(sender, INT_PDA_Const.ADM_SET_PASSWORD, code);
	}

	static void OnDeleteAccount(PlayerIdentity sender, INT_PDA_SE_Session s, ParamsReadContext ctx)
	{
		string steam;
		if (!ctx.Read(steam))
			return;
		INT_PDA_Account a = INT_PDA_SE_Store.Account(steam);
		// an account file that does not read can still be deleted by its SteamID (frees the player; .broken copy stays)
		if (!a && (!INT_PDA_Util.IsSteamId(steam) || !INT_PDA_SE_Store.HasAccount(steam)))
		{
			Reply(sender, INT_PDA_Const.ADM_DELETE_ACCOUNT, INT_PDA_Const.R_NOT_FOUND);
			return;
		}
		INT_PDA_FactionData f = null;
		string callsign = steam;
		if (a)
		{
			f = INT_PDA_SE_Store.Faction(a.FactionId);
			callsign = a.Callsign;
		}
		else
		{
			f = INT_PDA_SE_Store.FactionOfMember(steam);
			INT_PDA_Member gone = null;
			if (f)
				gone = f.FindMember(steam);
			if (gone)
			{
				callsign = gone.Callsign;
				INT_PDA_SE_Store.DropNameOf(gone.Callsign, steam);
			}
		}
		if (f)
		{
			int idx = f.MemberIndex(steam);
			if (idx >= 0)
				f.Members.RemoveOrdered(idx);
			INT_PDA_SE_Store.SaveFaction(f);
		}
		INT_PDA_SE_Store.DeleteAccount(steam);
		INT_PDA_SE_Session ts = INT_PDA_SE_Core.Session(steam);
		if (ts)
		{
			ts.LoggedIn = false;
			ts.Listening = false;
			if (INT_PDA_SE_Core.Online(ts))
				INT_PDA_SE_Core.SendWelcome(ts.Identity, ts, null);
		}
		if (f)
			INT_PDA_SE_Faction.PushFaction(f);
		Audit(s, "удалил аккаунт " + callsign + " (" + steam + ")");
		Reply(sender, INT_PDA_Const.ADM_DELETE_ACCOUNT, INT_PDA_Const.R_OK);
	}

	// --- factions ---------------------------------------------------------------------------------------------------
	// id "" = the list; an id = the full view of that faction (with Steam IDs).
	static void OnFactions(PlayerIdentity sender, ParamsReadContext ctx)
	{
		string id;
		if (!ctx.Read(id))
			return;
		if (id != "")
		{
			INT_PDA_FactionData f = INT_PDA_SE_Store.Faction(id);
			if (!f)
			{
				Reply(sender, INT_PDA_Const.ADM_FACTIONS, INT_PDA_Const.R_NOT_FOUND);
				return;
			}
			ScriptRPC one = Begin(INT_PDA_Const.ADM_FACTIONS, INT_PDA_Const.R_OK);
			bool full = true;
			one.Write(full);
			INT_PDA_SE_Faction.ViewOf(f).Write(one, true);
			INT_PDA_SE_Core.SendTo(one, sender, INT_PDA_Const.OP_ADMIN_DATA);
			return;
		}
		ScriptRPC rpc = Begin(INT_PDA_Const.ADM_FACTIONS, INT_PDA_Const.R_OK);
		bool list = false;
		rpc.Write(list);
		rpc.Write(INT_PDA_SE_Store.s_Factions.Count());
		foreach (string fid, INT_PDA_FactionData g : INT_PDA_SE_Store.s_Factions)
		{
			INT_PDA_FactionSummary sm = new INT_PDA_FactionSummary();
			sm.Id = fid;
			sm.Name = g.Name;
			sm.Emblem = g.Emblem;
			sm.Members = g.Members.Count();
			sm.Online = INT_PDA_SE_Store.OnlineCount(g);
			foreach (INT_PDA_Member m : g.Members)
			{
				if (m.Role == INT_PDA_Const.ROLE_LEADER)
					sm.Leader = m.Callsign;
			}
			sm.Write(rpc);
		}
		INT_PDA_SE_Core.SendTo(rpc, sender, INT_PDA_Const.OP_ADMIN_DATA);
	}

	// Create (id "") or edit a faction. spawnPos: "x, y, z" or "" (keep). The spawn set of a new faction copies the
	// clothes of Spawn/Default.json; the owner edits Spawn/<id>.json by hand afterwards (brief, admin item 13).
	static void OnFactionSave(PlayerIdentity sender, INT_PDA_SE_Session s, ParamsReadContext ctx)
	{
		string id;
		string name;
		int emblem;
		string motto;
		string base;
		string spawnPos;
		if (!ctx.Read(id) || !ctx.Read(name) || !ctx.Read(emblem) || !ctx.Read(motto) || !ctx.Read(base) || !ctx.Read(spawnPos))
			return;
		name = INT_PDA_Util.CleanLine(name, 32, INT_PDA_Const.MAX_NAME_BYTES);
		if (name == "")
		{
			Reply(sender, INT_PDA_Const.ADM_FACTION_SAVE, INT_PDA_Const.R_TEXT);
			return;
		}
		// names are unique, case-insensitive (Cyrillic too); the faction count is small, all are in memory
		string lowName = INT_PDA_Util.LowerAll(name);
		foreach (string fid, INT_PDA_FactionData other : INT_PDA_SE_Store.s_Factions)
		{
			if (other && fid != id && INT_PDA_Util.LowerAll(other.Name) == lowName)
			{
				Reply(sender, INT_PDA_Const.ADM_FACTION_SAVE, INT_PDA_Const.R_NAME_TAKEN);
				return;
			}
		}
		INT_PDA_FactionData f;
		bool created = false;
		if (id == "")
		{
			f = INT_PDA_FactionData.ForRead();
			f.Id = INT_PDA_SE_Store.NewFactionId();
			f.Created = INT_PDA_Util.Stamp();
			f.Ranks.Copy(INT_PDA_SE_Store.Cfg().DefaultRanks);
			created = true;
		}
		else
		{
			f = INT_PDA_SE_Store.Faction(id);
			if (!f)
			{
				Reply(sender, INT_PDA_Const.ADM_FACTION_SAVE, INT_PDA_Const.R_NOT_FOUND);
				return;
			}
		}
		f.Name = name;
		f.Emblem = emblem;
		f.Motto = motto;
		f.Base = base;
		f.Clean();
		if (!INT_PDA_SE_Store.SaveFaction(f))
		{
			Reply(sender, INT_PDA_Const.ADM_FACTION_SAVE, INT_PDA_Const.R_WRITE_FAILED);
			return;
		}
		spawnPos.TrimInPlace();
		INT_PDA_SpawnSet sp = INT_PDA_SE_Store.SpawnSet(f.Id);
		if (!sp)
		{
			// members of a new faction come back naked (owner 2026-10-30): no clothes, no items, only the point; the
			// owner fills Spawn/<id>.json by hand when the faction gets its own gear
			sp = INT_PDA_SpawnSet.ForRead();
			sp.m_OwnerSteamID = "EVERYONE";
		}
		if (spawnPos != "")
		{
			sp.m_SpawnPosList.Clear();
			sp.m_SpawnPosList.Insert(spawnPos);
		}
		INT_PDA_SE_Store.SaveSpawnSet(f.Id, sp);
		if (created)
			Audit(s, "создал группировку " + f.Name + " (" + f.Id + ")");
		else
			Audit(s, "изменил группировку " + f.Name + " (" + f.Id + ")");
		ScriptRPC rpc = Begin(INT_PDA_Const.ADM_FACTION_SAVE, INT_PDA_Const.R_OK);
		INT_PDA_Proto.WriteStr(rpc, f.Id);
		INT_PDA_SE_Core.SendTo(rpc, sender, INT_PDA_Const.OP_ADMIN_DATA);
		INT_PDA_SE_Faction.PushFaction(f);
	}

	// Every member leaves (accounts move back to Players/), then the files go.
	static void OnFactionDelete(PlayerIdentity sender, INT_PDA_SE_Session s, ParamsReadContext ctx)
	{
		string id;
		if (!ctx.Read(id))
			return;
		INT_PDA_FactionData f = INT_PDA_SE_Store.Faction(id);
		if (!f)
		{
			Reply(sender, INT_PDA_Const.ADM_FACTION_DELETE, INT_PDA_Const.R_NOT_FOUND);
			return;
		}
		array<string> members = new array<string>();
		foreach (INT_PDA_Member m : f.Members)
			members.Insert(m.SteamId);
		foreach (string steam : members)
		{
			INT_PDA_Account a = INT_PDA_SE_Store.Account(steam);
			if (a && a.FactionId == id)
			{
				int left = INT_PDA_SE_Faction.Remove(a, true);
				if (left != INT_PDA_Const.R_OK)
					INT_PDA_Log.Warn("удаление группировки " + id + ": аккаунт " + steam + " не записан (остался в ней)");
			}
			if (!INT_PDA_SE_Core.Session(steam))
				INT_PDA_SE_Store.ForgetAccount(steam);
		}
		string name = f.Name;
		INT_PDA_SE_Store.DeleteFaction(id);
		// ids are reused ("gp001" again for the next new faction): an invitation into the deleted one must not open it
		foreach (string invSteam, INT_PDA_SE_Session invSession : INT_PDA_SE_Core.s_Sessions)
		{
			if (invSession && invSession.InviteFaction == id)
			{
				invSession.InviteFaction = "";
				invSession.InviteFrom = "";
			}
		}
		foreach (string online : members)
			INT_PDA_SE_Core.PushProfile(online);
		Audit(s, "удалил группировку " + name + " (" + id + "), участников " + members.Count().ToString());
		Reply(sender, INT_PDA_Const.ADM_FACTION_DELETE, INT_PDA_Const.R_OK);
	}

	// Leader / deputy / member. A faction has one leader: the old leader becomes a member.
	static void OnSetRole(PlayerIdentity sender, INT_PDA_SE_Session s, ParamsReadContext ctx)
	{
		string id;
		string steam;
		int role;
		if (!ctx.Read(id) || !ctx.Read(steam) || !ctx.Read(role))
			return;
		INT_PDA_FactionData f = INT_PDA_SE_Store.Faction(id);
		INT_PDA_Member m;
		if (f)
			m = f.FindMember(steam);
		if (!f || !m)
		{
			Reply(sender, INT_PDA_Const.ADM_SET_ROLE, INT_PDA_Const.R_NOT_FOUND);
			return;
		}
		role = INT_PDA_Util.ClampI(role, INT_PDA_Const.ROLE_MEMBER, INT_PDA_Const.ROLE_LEADER);
		// a failed write is reported, not rolled back: the roles stay set in memory (the next faction / account save
		// writes them), several files may already carry the new roles
		bool written = true;
		if (role == INT_PDA_Const.ROLE_LEADER)
		{
			foreach (INT_PDA_Member o : f.Members)
			{
				if (o.Role != INT_PDA_Const.ROLE_LEADER || o.SteamId == steam)
					continue;
				if (!SetMemberRole(f, o, INT_PDA_Const.ROLE_MEMBER))
					written = false;
			}
		}
		if (!SetMemberRole(f, m, role))
			written = false;
		if (!INT_PDA_SE_Store.SaveFaction(f))
			written = false;
		INT_PDA_SE_Faction.PushFaction(f);
		Audit(s, "роль " + role.ToString() + " для " + m.Callsign + " в " + f.Name);
		int code = INT_PDA_Const.R_OK;
		if (!written)
			code = INT_PDA_Const.R_WRITE_FAILED;
		Reply(sender, INT_PDA_Const.ADM_SET_ROLE, code);
	}

	// False = the member's account file could not be written.
	protected static bool SetMemberRole(INT_PDA_FactionData f, INT_PDA_Member m, int role)
	{
		bool ok = true;
		m.Role = role;
		INT_PDA_Account a = INT_PDA_SE_Store.Account(m.SteamId);
		if (a)
		{
			a.Role = role;
			ok = INT_PDA_SE_Store.SaveAccount(a);
			INT_PDA_SE_Core.PushProfile(m.SteamId);
			if (!INT_PDA_SE_Core.Session(m.SteamId))
				INT_PDA_SE_Store.ForgetAccount(m.SteamId);
		}
		return ok;
	}

	static void OnKick(PlayerIdentity sender, INT_PDA_SE_Session s, ParamsReadContext ctx)
	{
		string id;
		string steam;
		if (!ctx.Read(id) || !ctx.Read(steam))
			return;
		INT_PDA_FactionData f = INT_PDA_SE_Store.Faction(id);
		INT_PDA_Member m;
		if (f)
			m = f.FindMember(steam);
		if (!f || !m)
		{
			Reply(sender, INT_PDA_Const.ADM_KICK, INT_PDA_Const.R_NOT_FOUND);
			return;
		}
		string callsign = m.Callsign;
		int code = INT_PDA_Const.R_OK;
		bool gone = true;
		INT_PDA_Account a = INT_PDA_SE_Store.Account(steam);
		if (a && a.FactionId == id)
		{
			code = INT_PDA_SE_Faction.Remove(a, false);
			// his account could not be written: he stays (nothing changed)
			if (a.InFaction())
				gone = false;
		}
		else
		{
			f.Members.RemoveOrdered(f.MemberIndex(steam));
			if (!INT_PDA_SE_Store.SaveFaction(f))
				code = INT_PDA_Const.R_WRITE_FAILED;
		}
		if (!INT_PDA_SE_Core.Session(steam))
			INT_PDA_SE_Store.ForgetAccount(steam);
		if (!gone)
		{
			Reply(sender, INT_PDA_Const.ADM_KICK, code);
			return;
		}
		INT_PDA_SE_Core.PushProfile(steam);
		INT_PDA_SE_Faction.PushFaction(f);
		INT_PDA_SE_Chat.System(f.Id, "@KICKED_ADMIN|" + callsign);
		Audit(s, "исключил " + callsign + " (" + steam + ") из " + f.Name);
		Reply(sender, INT_PDA_Const.ADM_KICK, code);
	}

	// The admin joins any faction (brief, admin item 18); from another faction he moves over.
	static void OnJoin(PlayerIdentity sender, INT_PDA_SE_Session s, ParamsReadContext ctx)
	{
		string id;
		if (!ctx.Read(id))
			return;
		INT_PDA_FactionData f = INT_PDA_SE_Store.Faction(id);
		INT_PDA_Account a = INT_PDA_SE_Store.Account(s.Steam);
		if (!f || !a)
		{
			Reply(sender, INT_PDA_Const.ADM_JOIN, INT_PDA_Const.R_NOT_FOUND);
			return;
		}
		INT_PDA_FactionData old = INT_PDA_SE_Store.Faction(a.FactionId);
		// already there: re-joining would drop his role and rank
		if (old && old.Id == f.Id)
		{
			Reply(sender, INT_PDA_Const.ADM_JOIN, INT_PDA_Const.R_IN_FACTION);
			return;
		}
		if (old)
		{
			int left = INT_PDA_SE_Faction.Remove(a, false);
			if (a.InFaction())
			{
				// his account could not be written: still in the old faction, no move
				Reply(sender, INT_PDA_Const.ADM_JOIN, left);
				return;
			}
			INT_PDA_SE_Faction.PushFaction(old);
		}
		int code = INT_PDA_SE_Faction.Join(a, f, INT_PDA_Const.ROLE_MEMBER);
		if (code == INT_PDA_Const.R_OK)
		{
			INT_PDA_SE_Core.SendProfile(sender, a);
			INT_PDA_SE_Faction.PushFaction(f);
			Audit(s, "вступил в группировку " + f.Name);
		}
		else if (old)
		{
			// he left the old faction but the new one did not take him: his profile must not show the old one
			INT_PDA_SE_Core.SendProfile(sender, a);
		}
		Reply(sender, INT_PDA_Const.ADM_JOIN, code);
	}

	static void OnRanks(PlayerIdentity sender, INT_PDA_SE_Session s, ParamsReadContext ctx)
	{
		string id;
		int n;
		if (!ctx.Read(id) || !INT_PDA_Proto.ReadCount(ctx, n, 32))
			return;
		array<string> ranks = new array<string>();
		string r;
		for (int i = 0; i < n; i++)
		{
			if (!ctx.Read(r))
				return;
			r = INT_PDA_Util.CleanLine(r, 24, INT_PDA_Const.MAX_NAME_BYTES);
			if (r != "" && ranks.Find(r) < 0)
				ranks.Insert(r);
		}
		INT_PDA_FactionData f = INT_PDA_SE_Store.Faction(id);
		if (!f)
		{
			Reply(sender, INT_PDA_Const.ADM_RANKS, INT_PDA_Const.R_NOT_FOUND);
			return;
		}
		array<string> oldRanks = new array<string>();
		oldRanks.Copy(f.Ranks);
		f.Ranks.Copy(ranks);
		// a member whose rank was removed keeps no rank (renames go through ADM_RANK_RENAME and keep the holders)
		array<string> lost = new array<string>();
		array<string> lostRank = new array<string>();
		foreach (INT_PDA_Member m : f.Members)
		{
			if (m.Rank == "" || f.Ranks.Find(m.Rank) >= 0)
				continue;
			lost.Insert(m.SteamId);
			lostRank.Insert(m.Rank);
			m.Rank = "";
		}
		// one faction file write and one push for the whole list (not one per member)
		if (!INT_PDA_SE_Store.SaveFaction(f))
		{
			f.Ranks.Copy(oldRanks);
			for (int k = 0; k < lost.Count(); k++)
			{
				INT_PDA_Member back = f.FindMember(lost[k]);
				if (back)
					back.Rank = lostRank[k];
			}
			Reply(sender, INT_PDA_Const.ADM_RANKS, INT_PDA_Const.R_WRITE_FAILED);
			return;
		}
		int code = INT_PDA_Const.R_OK;
		foreach (string steam : lost)
		{
			if (!INT_PDA_SE_Faction.StoreAccountRank(f, steam, ""))
				code = INT_PDA_Const.R_WRITE_FAILED;
			INT_PDA_SE_Core.PushProfile(steam);
		}
		INT_PDA_SE_Faction.PushFaction(f);
		Audit(s, "ранги " + f.Name + ": " + ranks.Count().ToString() + ", сняты у " + lost.Count().ToString());
		Reply(sender, INT_PDA_Const.ADM_RANKS, code);
	}

	// Renames one rank in place (its position stays); every holder carries the new name — faction list, accounts,
	// profiles. One faction file write, one push. The faction file failing rolls everything back.
	static void OnRankRename(PlayerIdentity sender, INT_PDA_SE_Session s, ParamsReadContext ctx)
	{
		string id;
		string oldRank;
		string newRank;
		if (!ctx.Read(id) || !ctx.Read(oldRank) || !ctx.Read(newRank))
			return;
		INT_PDA_FactionData f = INT_PDA_SE_Store.Faction(id);
		if (!f)
		{
			Reply(sender, INT_PDA_Const.ADM_RANK_RENAME, INT_PDA_Const.R_NOT_FOUND);
			return;
		}
		int at = f.Ranks.Find(oldRank);
		if (at < 0)
		{
			Reply(sender, INT_PDA_Const.ADM_RANK_RENAME, INT_PDA_Const.R_NOT_FOUND);
			return;
		}
		newRank = INT_PDA_Util.CleanLine(newRank, 24, INT_PDA_Const.MAX_NAME_BYTES);
		if (newRank == "")
		{
			Reply(sender, INT_PDA_Const.ADM_RANK_RENAME, INT_PDA_Const.R_TEXT);
			return;
		}
		if (newRank == oldRank)
		{
			Reply(sender, INT_PDA_Const.ADM_RANK_RENAME, INT_PDA_Const.R_OK);
			return;
		}
		if (f.Ranks.Find(newRank) >= 0)
		{
			Reply(sender, INT_PDA_Const.ADM_RANK_RENAME, INT_PDA_Const.R_BAD_DATA);
			return;
		}
		f.Ranks.Set(at, newRank);
		array<string> holders = new array<string>();
		foreach (INT_PDA_Member m : f.Members)
		{
			if (m.Rank != oldRank)
				continue;
			m.Rank = newRank;
			holders.Insert(m.SteamId);
		}
		if (!INT_PDA_SE_Store.SaveFaction(f))
		{
			f.Ranks.Set(at, oldRank);
			foreach (string back : holders)
			{
				INT_PDA_Member hm = f.FindMember(back);
				if (hm)
					hm.Rank = oldRank;
			}
			Reply(sender, INT_PDA_Const.ADM_RANK_RENAME, INT_PDA_Const.R_WRITE_FAILED);
			return;
		}
		int code = INT_PDA_Const.R_OK;
		foreach (string steam : holders)
		{
			if (!INT_PDA_SE_Faction.StoreAccountRank(f, steam, newRank))
				code = INT_PDA_Const.R_WRITE_FAILED;
			INT_PDA_SE_Core.PushProfile(steam);
		}
		INT_PDA_SE_Faction.PushFaction(f);
		Audit(s, "ранг «" + oldRank + "» -> «" + newRank + "» в " + f.Name + ", носителей " + holders.Count().ToString());
		Reply(sender, INT_PDA_Const.ADM_RANK_RENAME, code);
	}

	static void OnRelation(PlayerIdentity sender, INT_PDA_SE_Session s, ParamsReadContext ctx)
	{
		string a;
		string b;
		int rel;
		string reason;
		if (!ctx.Read(a) || !ctx.Read(b) || !ctx.Read(rel) || !ctx.Read(reason))
			return;
		INT_PDA_FactionData fa = INT_PDA_SE_Store.Faction(a);
		INT_PDA_FactionData fb = INT_PDA_SE_Store.Faction(b);
		if (!fa || !fb || a == b)
		{
			Reply(sender, INT_PDA_Const.ADM_RELATION, INT_PDA_Const.R_NOT_FOUND);
			return;
		}
		rel = INT_PDA_Util.ClampI(rel, INT_PDA_Const.REL_NEUTRAL, INT_PDA_Const.REL_ENEMY);
		INT_PDA_RelationEntry e = INT_PDA_SE_Store.s_Relations.Find(a, b);
		bool created = false;
		int oldRel = INT_PDA_Const.REL_NEUTRAL;
		string oldReason = "";
		if (!e)
		{
			e = new INT_PDA_RelationEntry();
			e.A = a;
			e.B = b;
			INT_PDA_SE_Store.s_Relations.Items.Insert(e);
			created = true;
		}
		else
		{
			oldRel = e.Rel;
			oldReason = e.Reason;
		}
		e.Rel = rel;
		e.Reason = INT_PDA_Util.CleanLine(reason, 120, 400);
		if (!INT_PDA_SE_Store.SaveRelations())
		{
			// not on disk: the relation in memory goes back too (it would flip back on restart)
			if (created)
			{
				int idx = INT_PDA_SE_Store.s_Relations.Items.Find(e);
				if (idx >= 0)
					INT_PDA_SE_Store.s_Relations.Items.RemoveOrdered(idx);
			}
			else
			{
				e.Rel = oldRel;
				e.Reason = oldReason;
			}
			Reply(sender, INT_PDA_Const.ADM_RELATION, INT_PDA_Const.R_WRITE_FAILED);
			return;
		}
		INT_PDA_SE_Faction.PushFaction(fa);
		INT_PDA_SE_Faction.PushFaction(fb);
		Audit(s, "отношения " + fa.Name + " / " + fb.Name + " = " + rel.ToString());
		Reply(sender, INT_PDA_Const.ADM_RELATION, INT_PDA_Const.R_OK);
	}

	// --- reloads (admin items 9, 10; Ctrl+Alt+P) --------------------------------------------------------------------
	static void ReloadDb(PlayerIdentity sender, INT_PDA_SE_Session s)
	{
		INT_PDA_SE_Store.s_Accounts.Clear();
		foreach (string steam, INT_PDA_SE_Session o : INT_PDA_SE_Core.s_Sessions)
		{
			INT_PDA_Account a = INT_PDA_SE_Store.Account(steam);
			if (!a)
			{
				o.LoggedIn = false;
				o.Listening = false;
			}
		}
		MarkOnline();
		Audit(s, "перезагрузил базу аккаунтов");
		Reply(sender, INT_PDA_Const.ADM_RELOAD_DB, INT_PDA_Const.R_OK);
	}

	static void ReloadFactions(PlayerIdentity sender, INT_PDA_SE_Session s)
	{
		INT_PDA_SE_Store.LoadFactions();
		INT_PDA_SE_Store.LoadRelations();
		INT_PDA_SE_Store.LoadSpawn();
		MarkOnline();
		foreach (string id, INT_PDA_FactionData f : INT_PDA_SE_Store.s_Factions)
			INT_PDA_SE_Faction.PushFaction(f);
		Audit(s, "перезагрузил базу группировок (" + INT_PDA_SE_Store.s_Factions.Count().ToString() + ")");
		Reply(sender, INT_PDA_Const.ADM_RELOAD_FACTIONS, INT_PDA_Const.R_OK);
	}

	// Ctrl+Alt+P (owner 2026-10-22): re-read the config and the spawn sets from disk, normalise, save back, resend.
	static void ReloadConfig(PlayerIdentity sender, INT_PDA_SE_Session s)
	{
		INT_PDA_SE_Config keep = INT_PDA_SE_Store.s_Cfg;
		INT_PDA_SE_Store.LoadConfig();
		// a typo in the file must not drop every admin (nobody could fix it and reload again): keep what runs
		if (INT_PDA_SE_Store.s_Source == "broken" && keep)
		{
			INT_PDA_SE_Store.s_Cfg = keep;
			Audit(s, "перечитал конфиг: файл не читается, остались прежние настройки");
			Reply(sender, INT_PDA_Const.ADM_RELOAD_CONFIG, INT_PDA_Const.R_BAD_DATA);
			return;
		}
		// the file was deleted: a fresh default config, but the admins stay
		if (INT_PDA_SE_Store.s_Source == "created" && keep && keep.Admins)
		{
			INT_PDA_SE_Store.s_Cfg.Admins.Copy(keep.Admins);
			INT_PDA_SE_Store.SaveConfig();
		}
		INT_PDA_SE_Store.LoadSpawn();
		INT_PDA_Log.s_Debug = INT_PDA_SE_Store.Cfg().DebugMode != 0;
		ResendSettings();
		Audit(s, "перечитал конфиг (" + INT_PDA_SE_Store.s_Source + ")");
		Reply(sender, INT_PDA_Const.ADM_RELOAD_CONFIG, INT_PDA_Const.R_OK);
	}

	static void ResendSettings()
	{
		foreach (string steam, INT_PDA_SE_Session o : INT_PDA_SE_Core.s_Sessions)
		{
			o.Admin = INT_PDA_SE_Store.Cfg().IsAdmin(steam);
			if (INT_PDA_SE_Core.Online(o))
				INT_PDA_SE_Core.SendWelcome(o.Identity, o, INT_PDA_SE_Store.Account(steam));
		}
	}

	// Online flags of every faction member after a reload.
	static void MarkOnline()
	{
		foreach (string fid, INT_PDA_FactionData f : INT_PDA_SE_Store.s_Factions)
		{
			foreach (INT_PDA_Member m : f.Members)
				m.Online = INT_PDA_SE_Core.Session(m.SteamId) != null;
		}
	}

	// --- news, settings ---------------------------------------------------------------------------------------------
	static void OnNewsPost(PlayerIdentity sender, INT_PDA_SE_Session s, ParamsReadContext ctx)
	{
		string title;
		string text;
		if (!ctx.Read(title) || !ctx.Read(text))
			return;
		// lifetime in minutes (optional field: -1 = the config default, 0 = until deleted)
		int minutes = -1;
		ctx.Read(minutes);
		title = INT_PDA_Util.CleanLine(title, 48, INT_PDA_Const.MAX_NAME_BYTES * 2);
		text = INT_PDA_Util.CleanLine(text, 300, INT_PDA_Const.MAX_WIRE_BYTES - 100);
		if (title == "" || text == "")
		{
			Reply(sender, INT_PDA_Const.ADM_NEWS_POST, INT_PDA_Const.R_TEXT);
			return;
		}
		INT_PDA_News n = INT_PDA_SE_Store.AddNews(title, text, minutes);
		INT_PDA_SE_Chat.FlashNews(n);
		Audit(s, "новость: " + title + " (" + n.Minutes.ToString() + " мин)");
		Reply(sender, INT_PDA_Const.ADM_NEWS_POST, INT_PDA_Const.R_OK);
	}

	// A published item changed (text, lifetime counted again from now): every listener gets the live list again,
	// quietly (no banner, no sound) — the «Новости Зоны» line and the tab show the new text at once.
	static void OnNewsEdit(PlayerIdentity sender, INT_PDA_SE_Session s, ParamsReadContext ctx)
	{
		int id;
		string title;
		string text;
		int minutes;
		if (!ctx.Read(id) || !ctx.Read(title) || !ctx.Read(text) || !ctx.Read(minutes))
		{
			Reply(sender, INT_PDA_Const.ADM_NEWS_EDIT, INT_PDA_Const.R_BAD_DATA);
			return;
		}
		title = INT_PDA_Util.CleanLine(title, 48, INT_PDA_Const.MAX_NAME_BYTES * 2);
		text = INT_PDA_Util.CleanLine(text, 300, INT_PDA_Const.MAX_WIRE_BYTES - 100);
		if (title == "" || text == "")
		{
			Reply(sender, INT_PDA_Const.ADM_NEWS_EDIT, INT_PDA_Const.R_TEXT);
			return;
		}
		INT_PDA_News n = INT_PDA_SE_Store.EditNews(id, title, text, minutes);
		if (!n)
		{
			Reply(sender, INT_PDA_Const.ADM_NEWS_EDIT, INT_PDA_Const.R_NOT_FOUND);
			return;
		}
		INT_PDA_SE_Chat.PushNews();
		Audit(s, "изменил новость " + id.ToString() + ": " + title + " (" + n.Minutes.ToString() + " мин)");
		Reply(sender, INT_PDA_Const.ADM_NEWS_EDIT, INT_PDA_Const.R_OK);
	}

	static void OnNewsDelete(PlayerIdentity sender, INT_PDA_SE_Session s, ParamsReadContext ctx)
	{
		int id;
		if (!ctx.Read(id))
			return;
		int code = INT_PDA_Const.R_NOT_FOUND;
		if (INT_PDA_SE_Store.DeleteNews(id))
		{
			code = INT_PDA_Const.R_OK;
			INT_PDA_SE_Chat.PushNews();
		}
		Audit(s, "удалил новость " + id.ToString());
		Reply(sender, INT_PDA_Const.ADM_NEWS_DELETE, code);
	}

	static void OnSettings(PlayerIdentity sender, INT_PDA_SE_Session s, ParamsReadContext ctx)
	{
		float radius;
		if (!ctx.Read(radius))
			return;
		INT_PDA_SE_Config cfg = INT_PDA_SE_Store.Cfg();
		cfg.NearbyInviteRadius = INT_PDA_Util.ClampF(radius, 1, 50, 5);
		// the news line settings (optional fields of the same request)
		int rotate;
		int defMinutes;
		if (ctx.Read(rotate) && ctx.Read(defMinutes))
		{
			cfg.NewsRotateSeconds = INT_PDA_Util.ClampI(rotate, 3, 300);
			cfg.NewsMinutesDefault = INT_PDA_Util.ClampI(defMinutes, 0, 525600);
		}
		INT_PDA_SE_Store.SaveConfig();
		ResendSettings();
		Audit(s, "настройки: радиус «Рядом» = " + cfg.NearbyInviteRadius.ToString() + ", смена новости " + cfg.NewsRotateSeconds.ToString() + " с, показ новости по умолчанию " + cfg.NewsMinutesDefault.ToString() + " мин");
		Reply(sender, INT_PDA_Const.ADM_SETTINGS, INT_PDA_Const.R_OK);
	}
}
