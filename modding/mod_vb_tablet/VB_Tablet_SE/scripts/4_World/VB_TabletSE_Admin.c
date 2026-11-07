// Администратор: переименование, смена лидера, роспуск, запрещённые слова.
// Каждая операция проверяет список администраторов на сервере и пишется в лог.
class VB_TabletAdmin
{
	static void Handle(PlayerBase player, PlayerIdentity id, int op, ParamsReadContext ctx)
	{
		if (!VB_TabletConfig.Get().IsAdmin(id.GetPlainId()))
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.DENIED);
			return;
		}
		string a;
		string b;
		string d;
		switch (op)
		{
			case VB_TabletOp.ADMIN_MEMBERS:
				if (ctx.Read(a))
					SendMembers(player, id, a);
				break;
			case VB_TabletOp.ADMIN_RENAME:
				if (ctx.Read(a) && ctx.Read(b) && ctx.Read(d))
					Rename(player, id, a, b, d);
				break;
			case VB_TabletOp.ADMIN_LEADER:
				if (ctx.Read(a) && ctx.Read(b))
					SetLeader(player, id, a, b);
				break;
			case VB_TabletOp.ADMIN_DISSOLVE:
				if (ctx.Read(a) && ctx.Read(b))
					Dissolve(player, id, a, b);
				break;
			case VB_TabletOp.ADMIN_BANNED:
				SendBanned(player, id);
				break;
			case VB_TabletOp.ADMIN_BAN_ADD:
				if (ctx.Read(a))
					BanAdd(player, id, a);
				break;
			case VB_TabletOp.ADMIN_BAN_DEL:
				if (ctx.Read(a))
					BanDel(player, id, a);
				break;
		}
	}

	protected static void Audit(PlayerIdentity id, string text)
	{
		Print(VB_TABLET_TAG + " админ " + id.GetName() + " (" + id.GetPlainId() + "): " + text);
	}

	protected static void SendMembers(PlayerBase player, PlayerIdentity id, string clanId)
	{
		VB_Clan c = VB_TabletClans.Find(clanId);
		if (!c)
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.NOT_FOUND);
			return;
		}
		int op = VB_TabletOp.ADMIN_MEMBERS;
		int n = c.Members.Count();
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(op);
		rpc.Write(clanId);
		rpc.Write(n);
		foreach (VB_ClanMember m : c.Members)
		{
			string uid = m.Uid;
			string name = m.PlayerName;
			bool leader = m.RankId == "leader";
			rpc.Write(uid);
			rpc.Write(name);
			rpc.Write(leader);
		}
		rpc.Send(player, VB_TABLET_RPC_S2C, true, id);
	}

	protected static bool TagValid(string tag)
	{
		int n = tag.Length();
		if (n < 2 || n > 4)
			return false;
		string allowed = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
		for (int i = 0; i < n; i++)
		{
			string ch = tag.Get(i);
			if (allowed.IndexOf(ch) < 0)
				return false;
		}
		return true;
	}

	protected static void Rename(PlayerBase player, PlayerIdentity id, string clanId, string title, string tag)
	{
		VB_Clan c = VB_TabletClans.Find(clanId);
		int len = title.LengthUtf8();
		int res = VB_TabletResult.SAVED;
		if (!c)
			res = VB_TabletResult.NOT_FOUND;
		else if (len < 3 || len > 24 || !TagValid(tag))
			res = VB_TabletResult.BAD_INPUT;
		else if (VB_TabletClans.HasBanned(title) || VB_TabletClans.HasBanned(tag))
			res = VB_TabletResult.BANNED_WORD;
		else if (!VB_TabletClans.NameFree(title, c))
			res = VB_TabletResult.NAME_TAKEN;
		else if (!VB_TabletClans.TagFree(tag, c))
			res = VB_TabletResult.TAG_TAKEN;
		if (res != VB_TabletResult.SAVED)
		{
			VB_TabletServer.SendResult(player, id, res);
			return;
		}
		Audit(id, "клан " + c.Id + " переименован: " + c.Title + " [" + c.Tag + "] -> " + title + " [" + tag + "]");
		c.Title = title;
		c.Tag = tag;
		VB_TabletClans.Log(c, VB_ClanEvent.SETTINGS, id.GetName());
		VB_TabletClans.Save(c);
		VB_TabletServer.PushClan(c);
		VB_TabletServer.SendResult(player, id, res);
		VB_TabletServer.SendClanList(player, id);
	}

	// Для клана, чей лидер пропал. Старый лидер получает второй ранг сверху.
	protected static void SetLeader(PlayerBase player, PlayerIdentity id, string clanId, string target)
	{
		VB_Clan c = VB_TabletClans.Find(clanId);
		if (!c || !c.Member(target) || c.Ranks.Count() < 2)
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.NOT_FOUND);
			return;
		}
		VB_ClanMember m = c.Member(target);
		string second = c.Ranks.Get(1).Id;
		foreach (VB_ClanMember o : c.Members)
		{
			if (o.RankId == "leader")
				o.RankId = second;
		}
		m.RankId = "leader";
		Audit(id, "клан " + c.Id + " лидер: " + m.PlayerName + " (" + m.Uid + ")");
		VB_TabletClans.Log(c, VB_ClanEvent.LEADER, m.PlayerName);
		VB_TabletClans.Save(c);
		VB_TabletServer.PushClan(c);
		VB_TabletServer.SendResult(player, id, VB_TabletResult.SAVED);
		VB_TabletServer.SendClanList(player, id);
	}

	protected static void Dissolve(PlayerBase player, PlayerIdentity id, string clanId, string confirm)
	{
		VB_Clan c = VB_TabletClans.Find(clanId);
		if (!c)
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.NOT_FOUND);
			return;
		}
		if (confirm != c.Title)
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.BAD_INPUT);
			return;
		}
		Audit(id, "клан распущен: " + c.Id + " " + c.Title + " [" + c.Tag + "], на складе было " + c.Bank.ToString());
		VB_TabletClans.Dissolve(c);
		VB_TabletServer.SendResult(player, id, VB_TabletResult.OK);
		VB_TabletServer.SendClanList(player, id);
	}

	protected static void SendBanned(PlayerBase player, PlayerIdentity id)
	{
		array<string> words = VB_TabletConfig.Get().BannedWords;
		int op = VB_TabletOp.ADMIN_BANNED;
		int n = words.Count();
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(op);
		rpc.Write(n);
		foreach (string w : words)
		{
			string word = w;
			rpc.Write(word);
		}
		rpc.Send(player, VB_TABLET_RPC_S2C, true, id);
	}

	protected static void BanAdd(PlayerBase player, PlayerIdentity id, string word)
	{
		string w = word + "";
		w.TrimInPlace();
		w.ToLower();
		int len = w.LengthUtf8();
		VB_TabletConfig cfg = VB_TabletConfig.Get();
		if (len < 1 || len > 24)
		{
			VB_TabletServer.SendResult(player, id, VB_TabletResult.BAD_INPUT);
			return;
		}
		if (cfg.BannedWords.Find(w) < 0)
		{
			cfg.BannedWords.Insert(w);
			if (!cfg.Store())
			{
				VB_TabletServer.SendResult(player, id, VB_TabletResult.STORAGE);
				SendBanned(player, id);
				return;
			}
			Audit(id, "запрещённое слово добавлено: " + w);
		}
		SendBanned(player, id);
	}

	protected static void BanDel(PlayerBase player, PlayerIdentity id, string word)
	{
		VB_TabletConfig cfg = VB_TabletConfig.Get();
		int i = cfg.BannedWords.Find(word);
		if (i >= 0)
		{
			cfg.BannedWords.Remove(i);
			if (!cfg.Store())
				VB_TabletServer.SendResult(player, id, VB_TabletResult.STORAGE);
			else
				Audit(id, "запрещённое слово убрано: " + word);
		}
		SendBanned(player, id);
	}
}
