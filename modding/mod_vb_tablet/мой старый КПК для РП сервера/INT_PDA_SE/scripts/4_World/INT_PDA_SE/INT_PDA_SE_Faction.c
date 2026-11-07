// INT_PDA (server) — factions («группировки», spec.md §3; owner 2026-10-22): admins create factions and appoint the
// leader / deputies; the leader and deputies invite players (by Steam ID; the «Рядом» list is built by the client),
// the leader gives and takes ranks from the faction's own rank list; relations are set by the admin only. Faction
// mates' positions and SOS are one-shot answers to a player's request — nothing is tracked or repeated.

class INT_PDA_SE_Faction
{
	// Online flag of a member (HELLO / disconnect). Memory only.
	static void SetOnline(INT_PDA_Account a, bool online)
	{
		INT_PDA_FactionData f = INT_PDA_SE_Store.Faction(a.FactionId);
		if (!f)
			return;
		INT_PDA_Member m = f.FindMember(a.SteamId);
		if (m)
			m.Online = online;
	}

	static bool CanManage(INT_PDA_Account a, INT_PDA_SE_Session s)
	{
		if (s && s.Admin)
			return true;
		return a && a.InFaction() && a.Role >= INT_PDA_Const.ROLE_DEPUTY;
	}

	static INT_PDA_FactionView ViewOf(INT_PDA_FactionData f)
	{
		INT_PDA_FactionView v = new INT_PDA_FactionView();
		v.Id = f.Id;
		v.Name = f.Name;
		v.Emblem = f.Emblem;
		v.Motto = f.Motto;
		v.Base = f.Base;
		INT_PDA_SpawnSet sp = INT_PDA_SE_Store.SpawnSet(f.Id);
		if (sp && sp.m_SpawnPosList.Count() > 0)
			v.SpawnPos = sp.m_SpawnPosList[0];
		foreach (INT_PDA_Member m : f.Members)
			v.Members.Insert(m);
		v.Ranks.Copy(f.Ranks);
		foreach (INT_PDA_Mark k : f.Marks)
			v.Marks.Insert(k);
		foreach (string id, INT_PDA_FactionData o : INT_PDA_SE_Store.s_Factions)
		{
			if (id == f.Id)
				continue;
			INT_PDA_Relation r = new INT_PDA_Relation();
			r.FactionId = id;
			r.Name = o.Name;
			r.Emblem = o.Emblem;
			INT_PDA_RelationEntry e = INT_PDA_SE_Store.s_Relations.Find(f.Id, id);
			if (e)
			{
				r.Rel = e.Rel;
				r.Reason = e.Reason;
			}
			v.Relations.Insert(r);
		}
		return v;
	}

	static void SendFaction(PlayerIdentity to, INT_PDA_SE_Session s, INT_PDA_Account a)
	{
		INT_PDA_FactionData f = INT_PDA_SE_Store.Faction(a.FactionId);
		ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_FACTION_DATA);
		bool has = f != null;
		rpc.Write(has);
		if (has)
			ViewOf(f).Write(rpc, CanManage(a, s));
		if (to)
			INT_PDA_SE_Core.SendTo(rpc, to, INT_PDA_Const.OP_FACTION_DATA);
		else
			INT_PDA_SE_Core.SendToSession(rpc, s, INT_PDA_Const.OP_FACTION_DATA);
	}

	// After any change of a faction: every online, logged-in member gets the new view.
	static void PushFaction(INT_PDA_FactionData f)
	{
		if (!f)
			return;
		foreach (INT_PDA_Member m : f.Members)
		{
			if (!m.Online)
				continue;
			INT_PDA_SE_Session s = INT_PDA_SE_Core.Session(m.SteamId);
			INT_PDA_Account a = INT_PDA_SE_Store.Account(m.SteamId);
			if (s && s.LoggedIn && INT_PDA_SE_Core.Online(s) && a)
				SendFaction(s.Identity, s, a);
		}
	}

	static void OnFaction(PlayerIdentity sender, INT_PDA_SE_Session s)
	{
		INT_PDA_Account a = INT_PDA_SE_Core.LoggedAccount(s);
		if (!a)
			return;
		SendFaction(sender, s, a);
	}

	// --- membership -------------------------------------------------------------------------------------------------
	// Puts an account into a faction (files: account moves Players/ -> GP/, faction member list). A failed write of
	// either file is rolled back in memory (the account file is written back to Players/) and reported.
	static int Join(INT_PDA_Account a, INT_PDA_FactionData f, int role)
	{
		if (a.InFaction())
			return INT_PDA_Const.R_IN_FACTION;
		string oldJoined = a.Joined;
		a.FactionId = f.Id;
		a.Role = role;
		a.Rank = "";
		if (f.Ranks.Count() > 0)
			a.Rank = f.Ranks[0];
		a.Joined = INT_PDA_Util.Day();
		if (!INT_PDA_SE_Store.SaveAccount(a))
		{
			a.FactionId = "";
			a.Role = INT_PDA_Const.ROLE_NONE;
			a.Rank = "";
			a.Joined = oldJoined;
			return INT_PDA_Const.R_WRITE_FAILED;
		}
		INT_PDA_Member m = new INT_PDA_Member();
		m.SteamId = a.SteamId;
		m.Callsign = a.Callsign;
		m.Rank = a.Rank;
		m.Role = role;
		m.Joined = a.Joined;
		m.Online = INT_PDA_SE_Core.Session(a.SteamId) != null;
		f.Members.Insert(m);
		if (!INT_PDA_SE_Store.SaveFaction(f))
		{
			int at = f.MemberIndex(a.SteamId);
			if (at >= 0)
				f.Members.RemoveOrdered(at);
			a.FactionId = "";
			a.Role = INT_PDA_Const.ROLE_NONE;
			a.Rank = "";
			a.Joined = oldJoined;
			if (!INT_PDA_SE_Store.SaveAccount(a))
				INT_PDA_Log.Warn("вступление не записано, аккаунт " + a.SteamId + " тоже не откатился на диске");
			return INT_PDA_Const.R_WRITE_FAILED;
		}
		// a pending invitation (into this or another faction) is spent: the member must not «accept» it later
		INT_PDA_SE_Session js = INT_PDA_SE_Core.Session(a.SteamId);
		if (js)
		{
			js.InviteFaction = "";
			js.InviteFrom = "";
		}
		INT_PDA_Log.Info("вступление в группировку " + f.Name + ": " + a.Callsign + " (" + a.SteamId + ")");
		return INT_PDA_Const.R_OK;
	}

	// Takes an account out of its faction (kick, leave, admin, faction deleted when keepList = true). The account file
	// goes first: when it can't be written nothing changes (rolled back, R_WRITE_FAILED). When only the faction file
	// fails afterwards the member is out (account + memory) and R_WRITE_FAILED tells it; the next faction save writes
	// the list. Callers check a.InFaction() to know whether he left.
	static int Remove(INT_PDA_Account a, bool keepList)
	{
		INT_PDA_FactionData f = INT_PDA_SE_Store.Faction(a.FactionId);
		string oldFaction = a.FactionId;
		int oldRole = a.Role;
		string oldRank = a.Rank;
		string oldJoined = a.Joined;
		a.FactionId = "";
		a.Role = INT_PDA_Const.ROLE_NONE;
		a.Rank = "";
		a.Joined = "";
		if (!INT_PDA_SE_Store.SaveAccount(a))
		{
			a.FactionId = oldFaction;
			a.Role = oldRole;
			a.Rank = oldRank;
			a.Joined = oldJoined;
			INT_PDA_Log.Warn("выход из группировки не записан (аккаунт " + a.SteamId + ")");
			return INT_PDA_Const.R_WRITE_FAILED;
		}
		if (!f || keepList)
			return INT_PDA_Const.R_OK;
		int idx = f.MemberIndex(a.SteamId);
		if (idx < 0)
			return INT_PDA_Const.R_OK;
		f.Members.RemoveOrdered(idx);
		if (!INT_PDA_SE_Store.SaveFaction(f))
		{
			INT_PDA_Log.Warn("файл группировки " + f.Id + " не записан после выхода " + a.SteamId);
			return INT_PDA_Const.R_WRITE_FAILED;
		}
		return INT_PDA_Const.R_OK;
	}

	static void OnInvite(PlayerIdentity sender, PlayerBase player, INT_PDA_SE_Session s, ParamsReadContext ctx)
	{
		string target;
		if (!ctx.Read(target))
			return;
		target.TrimInPlace();
		INT_PDA_Account a = INT_PDA_SE_Core.LoggedAccount(s);
		INT_PDA_FactionData f;
		if (a)
			f = INT_PDA_SE_Store.Faction(a.FactionId);
		if (!a || !f || !CanManage(a, s))
		{
			INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_INVITE, INT_PDA_Const.R_NO_RIGHTS);
			return;
		}
		if (!INT_PDA_Util.IsSteamId(target))
		{
			INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_INVITE, INT_PDA_Const.R_BAD_DATA);
			return;
		}
		INT_PDA_SE_Session ts = INT_PDA_SE_Core.Session(target);
		if (!INT_PDA_SE_Core.Online(ts))
		{
			INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_INVITE, INT_PDA_Const.R_OFFLINE);
			return;
		}
		INT_PDA_Account ta = INT_PDA_SE_Store.Account(target);
		if (!ta)
		{
			INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_INVITE, INT_PDA_Const.R_NOT_REGISTERED);
			return;
		}
		if (ta.InFaction())
		{
			INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_INVITE, INT_PDA_Const.R_IN_FACTION);
			return;
		}
		ts.InviteFaction = f.Id;
		ts.InviteFrom = a.Callsign;
		ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_INVITE_IN);
		INT_PDA_Proto.WriteStr(rpc, f.Id);
		INT_PDA_Proto.WriteStr(rpc, f.Name);
		rpc.Write(f.Emblem);
		INT_PDA_Proto.WriteStr(rpc, a.Callsign);
		INT_PDA_SE_Core.SendToSession(rpc, ts, INT_PDA_Const.OP_INVITE_IN);
		INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_INVITE, INT_PDA_Const.R_OK);
	}

	static void OnInviteAnswer(PlayerIdentity sender, INT_PDA_SE_Session s, ParamsReadContext ctx)
	{
		string factionId;
		bool accept;
		if (!ctx.Read(factionId) || !ctx.Read(accept))
			return;
		INT_PDA_Account a = INT_PDA_SE_Core.LoggedAccount(s);
		if (!a || s.InviteFaction == "" || s.InviteFaction != factionId)
		{
			INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_INVITE_ANSWER, INT_PDA_Const.R_NOT_FOUND);
			return;
		}
		string from = s.InviteFrom;
		s.InviteFaction = "";
		s.InviteFrom = "";
		if (!accept)
			return;
		INT_PDA_FactionData f = INT_PDA_SE_Store.Faction(factionId);
		if (!f)
		{
			INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_INVITE_ANSWER, INT_PDA_Const.R_NOT_FOUND);
			return;
		}
		int code = Join(a, f, INT_PDA_Const.ROLE_MEMBER);
		INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_INVITE_ANSWER, code);
		if (code != INT_PDA_Const.R_OK)
			return;
		INT_PDA_SE_Core.SendProfile(sender, a);
		PushFaction(f);
		INT_PDA_SE_Chat.System(f.Id, "@JOINED|" + a.Callsign + "|" + from);
	}

	static void OnKick(PlayerIdentity sender, INT_PDA_SE_Session s, ParamsReadContext ctx)
	{
		string target;
		if (!ctx.Read(target))
			return;
		INT_PDA_Account a = INT_PDA_SE_Core.LoggedAccount(s);
		INT_PDA_FactionData f;
		if (a)
			f = INT_PDA_SE_Store.Faction(a.FactionId);
		if (!a || !f || !CanManage(a, s))
		{
			INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_KICK, INT_PDA_Const.R_NO_RIGHTS);
			return;
		}
		INT_PDA_Member m = f.FindMember(target);
		if (!m || target == a.SteamId)
		{
			INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_KICK, INT_PDA_Const.R_NOT_FOUND);
			return;
		}
		// a deputy cannot kick the leader or another deputy
		if (!s.Admin && a.Role < INT_PDA_Const.ROLE_LEADER && m.Role >= INT_PDA_Const.ROLE_DEPUTY)
		{
			INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_KICK, INT_PDA_Const.R_NO_RIGHTS);
			return;
		}
		INT_PDA_Account ta = INT_PDA_SE_Store.Account(target);
		string kickedName = m.Callsign;
		int code = INT_PDA_Const.R_OK;
		// only a member of THIS faction is removed from his account; a stale list entry (his account says another
		// faction) is just dropped from this list — like the admin path
		if (ta && ta.FactionId == f.Id)
		{
			code = Remove(ta, false);
			if (ta.InFaction())
			{
				// his account could not be written: nothing changed
				INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_KICK, code);
				return;
			}
		}
		else
		{
			f.Members.RemoveOrdered(f.MemberIndex(target));
			if (!INT_PDA_SE_Store.SaveFaction(f))
				code = INT_PDA_Const.R_WRITE_FAILED;
		}
		INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_KICK, code);
		INT_PDA_SE_Core.PushProfile(target);
		PushFaction(f);
		INT_PDA_SE_Chat.System(f.Id, "@KICKED|" + kickedName);
	}

	static void OnSetRank(PlayerIdentity sender, INT_PDA_SE_Session s, ParamsReadContext ctx)
	{
		string target;
		string rank;
		if (!ctx.Read(target) || !ctx.Read(rank))
			return;
		INT_PDA_Account a = INT_PDA_SE_Core.LoggedAccount(s);
		INT_PDA_FactionData f;
		if (a)
			f = INT_PDA_SE_Store.Faction(a.FactionId);
		// ranks are given by the leader (owner 2026-10-22) — and by admins
		if (!a || !f || (!s.Admin && a.Role != INT_PDA_Const.ROLE_LEADER))
		{
			INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_SET_RANK, INT_PDA_Const.R_NO_RIGHTS);
			return;
		}
		int code = SetRank(f, target, rank);
		INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_SET_RANK, code);
	}

	// Shared with the admin tab. "" takes the rank away. The faction file failing rolls the rank back; the account file
	// failing afterwards is reported (the faction list already has the new rank, the profile follows on its next save).
	static int SetRank(INT_PDA_FactionData f, string target, string rank)
	{
		INT_PDA_Member m = f.FindMember(target);
		if (!m)
			return INT_PDA_Const.R_NOT_FOUND;
		if (rank != "" && f.Ranks.Find(rank) < 0)
			return INT_PDA_Const.R_BAD_DATA;
		string oldRank = m.Rank;
		m.Rank = rank;
		if (!INT_PDA_SE_Store.SaveFaction(f))
		{
			m.Rank = oldRank;
			return INT_PDA_Const.R_WRITE_FAILED;
		}
		int code = INT_PDA_Const.R_OK;
		if (!StoreAccountRank(f, target, rank))
			code = INT_PDA_Const.R_WRITE_FAILED;
		INT_PDA_SE_Core.PushProfile(target);
		PushFaction(f);
		return code;
	}

	// The account copy of a member's rank (one file, only for a member of THIS faction). The account of an offline
	// player is not kept in the cache. False = the account file could not be written.
	static bool StoreAccountRank(INT_PDA_FactionData f, string steam, string rank)
	{
		bool ok = true;
		INT_PDA_Account ta = INT_PDA_SE_Store.Account(steam);
		if (ta && ta.FactionId == f.Id && ta.Rank != rank)
		{
			ta.Rank = rank;
			ok = INT_PDA_SE_Store.SaveAccount(ta);
		}
		if (!INT_PDA_SE_Core.Session(steam))
			INT_PDA_SE_Store.ForgetAccount(steam);
		return ok;
	}

	static void OnLeave(PlayerIdentity sender, INT_PDA_SE_Session s)
	{
		INT_PDA_Account a = INT_PDA_SE_Core.LoggedAccount(s);
		if (!a || !a.InFaction())
		{
			INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_LEAVE, INT_PDA_Const.R_NOT_IN_FACTION);
			return;
		}
		INT_PDA_FactionData f = INT_PDA_SE_Store.Faction(a.FactionId);
		string name = a.Callsign;
		int code = Remove(a, false);
		INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_LEAVE, code);
		// his account could not be written: he is still a member, nothing to announce
		if (a.InFaction())
			return;
		INT_PDA_SE_Core.SendProfile(sender, a);
		if (f)
		{
			PushFaction(f);
			INT_PDA_SE_Chat.System(f.Id, "@LEFT|" + name);
		}
	}

	// --- map: mates, SOS, marks -------------------------------------------------------------------------------------
	static void OnMates(PlayerIdentity sender, PlayerBase player, INT_PDA_SE_Session s, ParamsReadContext ctx, int now)
	{
		INT_PDA_Account a = INT_PDA_SE_Core.LoggedAccount(s);
		INT_PDA pda;
		if (!a || !INT_PDA_SE_Core.CheckPda(player, s, ctx, INT_PDA.SLOT_GPS, pda))
			return;
		if (!s.Admin && (!pda || !pda.HasGp()))
			return;
		if (now - s.LastMatesMs < INT_PDA_Const.MATES_GAP_MS)
			return;
		s.LastMatesMs = now;
		INT_PDA_FactionData f = INT_PDA_SE_Store.Faction(a.FactionId);
		if (!f)
			return;
		array<ref INT_PDA_MatePos> list = new array<ref INT_PDA_MatePos>();
		foreach (INT_PDA_Member m : f.Members)
		{
			if (!m.Online || m.SteamId == s.Steam)
				continue;
			INT_PDA_SE_Session ms = INT_PDA_SE_Core.Session(m.SteamId);
			if (!ms || !ms.Identity)
				continue;
			Man mp = ms.Identity.GetPlayer();
			if (!mp || !mp.IsAlive())
				continue;
			vector pos = mp.GetPosition();
			INT_PDA_MatePos p = new INT_PDA_MatePos();
			p.Callsign = m.Callsign;
			p.X = pos[0];
			p.Z = pos[2];
			list.Insert(p);
			if (list.Count() >= 100)
				break;
		}
		ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_MATES_DATA);
		rpc.Write(list.Count());
		foreach (INT_PDA_MatePos q : list)
			q.Write(rpc);
		INT_PDA_SE_Core.SendTo(rpc, sender, INT_PDA_Const.OP_MATES_DATA);
	}

	static void OnSos(PlayerIdentity sender, PlayerBase player, INT_PDA_SE_Session s, ParamsReadContext ctx, int now)
	{
		INT_PDA_Account a = INT_PDA_SE_Core.LoggedAccount(s);
		INT_PDA pda;
		if (!a || !player || !INT_PDA_SE_Core.CheckPda(player, s, ctx, INT_PDA.SLOT_GPS, pda))
		{
			INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_SOS, INT_PDA_Const.R_NO_MODULE);
			return;
		}
		if (!s.Admin && (!pda || !pda.HasGp()))
		{
			INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_SOS, INT_PDA_Const.R_NO_MODULE);
			return;
		}
		INT_PDA_FactionData f = INT_PDA_SE_Store.Faction(a.FactionId);
		if (!f)
		{
			INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_SOS, INT_PDA_Const.R_NOT_IN_FACTION);
			return;
		}
		if (now - s.LastSosMs < INT_PDA_Const.SOS_GAP_MS)
		{
			INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_SOS, INT_PDA_Const.R_TOO_FAST);
			return;
		}
		s.LastSosMs = now;
		vector pos = player.GetPosition();
		int sent = 0;
		foreach (INT_PDA_Member m : f.Members)
		{
			if (!m.Online || m.SteamId == s.Steam)
				continue;
			INT_PDA_SE_Session ms = INT_PDA_SE_Core.Session(m.SteamId);
			if (!CanHearSos(ms))
				continue;
			ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_SOS_IN);
			INT_PDA_Proto.WriteStr(rpc, a.Callsign);
			rpc.Write(pos[0]);
			rpc.Write(pos[2]);
			INT_PDA_SE_Core.SendTo(rpc, ms.Identity, INT_PDA_Const.OP_SOS_IN);
			sent++;
		}
		INT_PDA_Log.Info("SOS: " + a.Callsign + " (" + f.Name + "), получателей " + sent.ToString());
		// the gap is spent either way (the signal went out); the sender learns that nobody could receive it
		if (sent == 0)
		{
			INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_SOS, INT_PDA_Const.R_NO_RECIPIENTS);
			return;
		}
		INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_SOS, INT_PDA_Const.R_OK);
	}

	// SOS receivers: logged-in, alive faction mates whose OWN working PDA carries the GP module (the chat module and
	// the chat «listening» do not matter). Checked at the moment of the SOS through that one player's inventory — no
	// world scan, nothing tracked. A config admin on the Alt+L PDA (no item, every module) hears it while listening.
	static bool CanHearSos(INT_PDA_SE_Session ms)
	{
		if (!ms || !ms.LoggedIn || !ms.Identity)
			return false;
		PlayerBase mp = PlayerBase.Cast(ms.Identity.GetPlayer());
		if (!mp || !mp.IsAlive())
			return false;
		if (OwnGpPda(mp, ms.Steam))
			return true;
		return ms.Admin && ms.Listening;
	}

	// A working PDA of this player (his own or not owned yet) with the GP module: in hands first, then his inventory.
	static INT_PDA OwnGpPda(PlayerBase p, string steam)
	{
		INT_PDA hands = INT_PDA.Cast(p.GetHumanInventory().GetEntityInHands());
		if (IsGpPda(hands, steam))
			return hands;
		array<EntityAI> items = new array<EntityAI>();
		p.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
		foreach (EntityAI e : items)
		{
			INT_PDA pda = INT_PDA.Cast(e);
			if (IsGpPda(pda, steam))
				return pda;
		}
		return null;
	}

	static bool IsGpPda(INT_PDA pda, string steam)
	{
		if (!pda || !pda.IsWorkingPda())
			return false;
		return pda.HasGp() && pda.IsOwnedBy(steam);
	}

	static void OnMarkAdd(PlayerIdentity sender, INT_PDA_SE_Session s, ParamsReadContext ctx)
	{
		int kind;
		string name;
		float x;
		float z;
		if (!ctx.Read(kind) || !ctx.Read(name) || !ctx.Read(x) || !ctx.Read(z))
			return;
		INT_PDA_Account a = INT_PDA_SE_Core.LoggedAccount(s);
		INT_PDA_FactionData f;
		if (a)
			f = INT_PDA_SE_Store.Faction(a.FactionId);
		if (!a || !f || !CanManage(a, s))
		{
			INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_MARK_ADD, INT_PDA_Const.R_NO_RIGHTS);
			return;
		}
		if (f.Marks.Count() >= INT_PDA_SE_Store.Cfg().MarksMax)
		{
			INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_MARK_ADD, INT_PDA_Const.R_FULL);
			return;
		}
		INT_PDA_Mark k = new INT_PDA_Mark();
		f.NextMarkId = f.NextMarkId + 1;
		k.Id = f.NextMarkId;
		k.Kind = INT_PDA_Util.ClampI(kind, INT_PDA_Const.MARK_BASE, INT_PDA_Const.MARK_NOTE);
		k.Name = INT_PDA_Util.CleanLine(name, 32, INT_PDA_Const.MAX_NAME_BYTES);
		k.X = INT_PDA_Util.ClampF(x, -100000, 100000, 0);
		k.Z = INT_PDA_Util.ClampF(z, -100000, 100000, 0);
		k.Author = a.Callsign;
		f.Marks.Insert(k);
		if (!INT_PDA_SE_Store.SaveFaction(f))
		{
			// not on disk: the mark goes away again (it would vanish on restart)
			f.Marks.RemoveOrdered(f.Marks.Count() - 1);
			f.NextMarkId = f.NextMarkId - 1;
			INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_MARK_ADD, INT_PDA_Const.R_WRITE_FAILED);
			return;
		}
		INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_MARK_ADD, INT_PDA_Const.R_OK);
		PushFaction(f);
	}

	static void OnMarkDel(PlayerIdentity sender, INT_PDA_SE_Session s, ParamsReadContext ctx)
	{
		int id;
		if (!ctx.Read(id))
			return;
		INT_PDA_Account a = INT_PDA_SE_Core.LoggedAccount(s);
		INT_PDA_FactionData f;
		if (a)
			f = INT_PDA_SE_Store.Faction(a.FactionId);
		if (!a || !f || !CanManage(a, s))
			return;
		for (int i = 0; i < f.Marks.Count(); i++)
		{
			if (f.Marks[i].Id == id)
			{
				f.Marks.RemoveOrdered(i);
				INT_PDA_SE_Store.SaveFaction(f);
				PushFaction(f);
				return;
			}
		}
	}
}
