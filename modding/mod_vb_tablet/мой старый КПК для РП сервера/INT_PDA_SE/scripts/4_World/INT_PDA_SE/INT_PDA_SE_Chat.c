// INT_PDA (server) — PDA chat (spec.md §3, §7; owner 2026-10-22). The vanilla chat is off; lines reach only players
// whose PDA works with a chat module ("listeners": the client reports the state change, the server drops a listener
// when his PDA stops). Public: last N lines in memory + file every Nth line. Faction: same per faction. Private: online
// listener gets it at once + a notice; otherwise the line waits in his mail file (delivered on login / PDA on).
// Anonymous lines carry no author to players; admins get the real author.

class INT_PDA_SE_Chat
{
	static const int HISTORY_BATCH = 20;
	static const int HISTORY_GAP_MS = 2000;
	static const int REPLY_CHARS = 90;			// quoted part of the answered line
	static int s_Lines;				// counters for Debug and the self-test
	static int s_Mailed;

	static void OnListen(PlayerIdentity sender, PlayerBase player, INT_PDA_SE_Session s, ParamsReadContext ctx)
	{
		bool on;
		if (!ctx.Read(on))
			return;
		if (!on)
		{
			s.Listening = false;
			return;
		}
		INT_PDA pda;
		if (!s.LoggedIn || !INT_PDA_SE_Core.CheckPda(player, s, ctx, INT_PDA.SLOT_CHAT, pda))
		{
			s.Listening = false;
			INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_LISTEN, INT_PDA_Const.R_NO_MODULE);
			return;
		}
		s.Listening = true;
		int low = 0;
		int high = 0;
		if (pda)
			pda.GetNetworkID(low, high);
		s.PdaLow = low;
		s.PdaHigh = high;
		DeliverMail(sender, s);
	}

	static void DeliverMail(PlayerIdentity to, INT_PDA_SE_Session s)
	{
		if (!FileExist(INT_PDA_SE_Paths.MailFile(s.Steam)))
			return;
		INT_PDA_ChatFile mail = INT_PDA_SE_Store.TakeMail(s.Steam);
		if (!mail || mail.Lines.Count() == 0)
			return;
		foreach (INT_PDA_ChatLine line : mail.Lines)
			SendLine(to, line);
		ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_PM_NOTICE);
		INT_PDA_Proto.WriteStr(rpc, mail.Lines[mail.Lines.Count() - 1].Author);
		rpc.Write(mail.Lines.Count());
		INT_PDA_SE_Core.SendTo(rpc, to, INT_PDA_Const.OP_PM_NOTICE);
	}

	static void OnChatSend(PlayerIdentity sender, PlayerBase player, INT_PDA_SE_Session s, ParamsReadContext ctx, int now)
	{
		int channel;
		int flags;
		string text;
		if (!ctx.Read(channel) || !ctx.Read(flags) || !ctx.Read(text))
			return;
		INT_PDA_Account a = INT_PDA_SE_Core.LoggedAccount(s);
		if (!a)
		{
			INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_CHAT_SEND, INT_PDA_Const.R_NOT_LOGGED_IN);
			return;
		}
		if (now - s.LastChatMs < 1000)
		{
			INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_CHAT_SEND, INT_PDA_Const.R_TOO_FAST);
			return;
		}
		bool asFaction = (flags & INT_PDA_Const.FL_AS_FACTION) != 0;
		string slot = INT_PDA.SLOT_CHAT;
		INT_PDA pda;
		bool pdaOk = INT_PDA_SE_Core.CheckPda(player, s, ctx, slot, pda);
		// the line this one answers (optional field: an older packet simply ends here)
		int replyId = 0;
		ctx.Read(replyId);
		if (!pdaOk)
		{
			INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_CHAT_SEND, INT_PDA_Const.R_NO_MODULE);
			return;
		}
		if ((channel == INT_PDA_Const.CH_FACTION || asFaction) && !s.Admin && (!pda || !pda.HasGp()))
		{
			INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_CHAT_SEND, INT_PDA_Const.R_NO_MODULE);
			return;
		}
		INT_PDA_FactionData f = INT_PDA_SE_Store.Faction(a.FactionId);
		if ((channel == INT_PDA_Const.CH_FACTION || asFaction) && !f)
		{
			INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_CHAT_SEND, INT_PDA_Const.R_NOT_IN_FACTION);
			return;
		}
		text = INT_PDA_Util.CleanLine(text, INT_PDA_SE_Store.Cfg().ChatMaxChars, INT_PDA_Const.MAX_WIRE_BYTES - 100);
		// '#' is not allowed in chat (owner 2026-10-23): the engine reads a text starting with it as a stringtable key
		if (text == "" || text.IndexOf("#") >= 0)
		{
			INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_CHAT_SEND, INT_PDA_Const.R_TEXT);
			return;
		}
		s.LastChatMs = now;
		flags = flags & (INT_PDA_Const.FL_ANON | INT_PDA_Const.FL_AS_FACTION);
		if (INT_PDA_SE_Store.Cfg().AnonymousAllowed == 0)
			flags = flags & ~INT_PDA_Const.FL_ANON;
		if (asFaction)
			flags = flags & ~INT_PDA_Const.FL_ANON;
		INT_PDA_ChatLine line = MakeLine(a, f, channel, flags, text);
		if (channel != INT_PDA_Const.CH_FACTION)
			line.Channel = INT_PDA_Const.CH_PUBLIC;
		if (replyId > 0)
			Quote(line, replyId, f);
		s_Lines++;
		if (channel == INT_PDA_Const.CH_FACTION)
		{
			INT_PDA_SE_Store.AddFaction(f.Id, line);
			ToFaction(f, line, s.Steam);
			return;
		}
		line.Channel = INT_PDA_Const.CH_PUBLIC;
		INT_PDA_SE_Store.AddPublic(line);
		ToListeners(line, s.Steam);
		INT_PDA_Log.Dbg("чат: " + a.Callsign + " (" + flags.ToString() + ")");
	}

	// Reply: the quote comes from the server's own history of the SAME channel (public, or the sender's faction), so
	// nobody can quote words that were never said. An anonymous original stays anonymous in the quote (no author);
	// a faction statement is quoted under the faction's name; system lines are not quoted.
	static void Quote(INT_PDA_ChatLine line, int replyId, INT_PDA_FactionData f)
	{
		array<ref INT_PDA_ChatLine> lines = null;
		if (line.Channel == INT_PDA_Const.CH_FACTION)
		{
			if (f)
				lines = INT_PDA_SE_Store.FactionChat(f.Id).Lines;
		}
		else
		{
			lines = INT_PDA_SE_Store.s_Public.Lines;
		}
		INT_PDA_ChatLine orig = INT_PDA_SE_Store.FindLine(lines, replyId);
		if (!orig || orig.Channel == INT_PDA_Const.CH_SYSTEM)
			return;
		line.ReplyId = orig.Id;
		line.ReplyFlags = orig.Flags & (INT_PDA_Const.FL_ANON | INT_PDA_Const.FL_AS_FACTION);
		if ((orig.Flags & INT_PDA_Const.FL_AS_FACTION) != 0)
			line.ReplyAuthor = orig.FactionName;
		else if ((orig.Flags & INT_PDA_Const.FL_ANON) != 0)
			line.ReplyAuthor = "";
		else
			line.ReplyAuthor = orig.Author;
		line.ReplyText = INT_PDA_Util.CleanLine(orig.Text, REPLY_CHARS, 400);
	}

	static INT_PDA_ChatLine MakeLine(INT_PDA_Account a, INT_PDA_FactionData f, int channel, int flags, string text)
	{
		INT_PDA_ChatLine line = new INT_PDA_ChatLine();
		line.Channel = channel;
		line.Flags = flags;
		line.Time = INT_PDA_SE_Clock.Short();
		line.Author = a.Callsign;
		line.AuthorSteam = a.SteamId;
		line.Avatar = a.Avatar;
		line.Text = text;
		if (f)
		{
			line.FactionId = f.Id;
			line.FactionName = f.Name;
			line.FactionEmblem = f.Emblem;
		}
		return line;
	}

	// What a recipient may see of a line: anonymous lines lose the author unless the recipient is an admin or the
	// author himself; lines written as a faction show the faction (the author stays for admins and the faction).
	static INT_PDA_ChatLine ViewFor(INT_PDA_ChatLine line, INT_PDA_SE_Session to, string recipientFaction)
	{
		bool anon = (line.Flags & INT_PDA_Const.FL_ANON) != 0;
		bool asFaction = (line.Flags & INT_PDA_Const.FL_AS_FACTION) != 0;
		if (!anon && !asFaction)
		{
			INT_PDA_ChatLine plain = new INT_PDA_ChatLine();
			plain.CopyFrom(line);
			plain.AuthorSteam = "";
			if (to && to.Admin)
				plain.AuthorSteam = line.AuthorSteam;
			return plain;
		}
		INT_PDA_ChatLine v = new INT_PDA_ChatLine();
		v.CopyFrom(line);
		bool full = to && (to.Admin || to.Steam == line.AuthorSteam);
		if (asFaction && recipientFaction != "" && recipientFaction == line.FactionId)
			full = true;
		if (full)
		{
			v.Flags = v.Flags | INT_PDA_Const.FL_ADMIN_VIEW;
			if (!to.Admin)
				v.AuthorSteam = "";
			return v;
		}
		v.AuthorSteam = "";
		if (anon)
		{
			v.Author = "";
			v.Avatar = -1;
			v.FactionId = "";
			v.FactionName = "";
			v.FactionEmblem = 0;
		}
		else
		{
			v.Author = "";
			v.Avatar = -1;
		}
		return v;
	}

	static string FactionOfSession(INT_PDA_SE_Session s)
	{
		INT_PDA_Account a = INT_PDA_SE_Store.Account(s.Steam);
		if (!a)
			return "";
		return a.FactionId;
	}

	static void ToListeners(INT_PDA_ChatLine line, string fromSteam)
	{
		foreach (string steam, INT_PDA_SE_Session s : INT_PDA_SE_Core.s_Sessions)
		{
			if (!s.LoggedIn || !s.Listening || !INT_PDA_SE_Core.Online(s))
				continue;
			SendLineTo(s, ViewFor(line, s, FactionOfSession(s)));
		}
	}

	static void ToFaction(INT_PDA_FactionData f, INT_PDA_ChatLine line, string fromSteam)
	{
		foreach (INT_PDA_Member m : f.Members)
		{
			if (!m.Online)
				continue;
			INT_PDA_SE_Session s = INT_PDA_SE_Core.Session(m.SteamId);
			if (!s || !s.LoggedIn || !s.Listening || !INT_PDA_SE_Core.Online(s))
				continue;
			// the faction channel needs the GP module too (spec §5): skipped when his listening PDA surely lacks it
			if (s.Identity && !s.Admin)
			{
				INT_PDA lp = INT_PDA_SE_Core.ListenPda(s);
				if (lp && !lp.HasGp())
					continue;
			}
			SendLineTo(s, ViewFor(line, s, f.Id));
		}
		// admins outside the faction see faction chat only in the admin tab (history), not live
	}

	static void SendLine(PlayerIdentity to, INT_PDA_ChatLine line)
	{
		ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_CHAT_LINE);
		line.Write(rpc);
		INT_PDA_SE_Core.SendTo(rpc, to, INT_PDA_Const.OP_CHAT_LINE);
	}

	// One line to a session (a player, or a self-test session: captured only).
	static void SendLineTo(INT_PDA_SE_Session s, INT_PDA_ChatLine line)
	{
		ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_CHAT_LINE);
		line.Write(rpc);
		INT_PDA_SE_Core.SendToSession(rpc, s, INT_PDA_Const.OP_CHAT_LINE);
	}

	// A system line (news flash, joins, kicks) into the public chat or one faction.
	static void System(string factionId, string text)
	{
		INT_PDA_ChatLine line = new INT_PDA_ChatLine();
		line.Channel = INT_PDA_Const.CH_SYSTEM;
		line.Time = INT_PDA_SE_Clock.Short();
		line.Text = INT_PDA_Util.CleanLine(text, 240, INT_PDA_Const.MAX_WIRE_BYTES - 100);
		if (factionId == "")
		{
			ToListeners(line, "");
			return;
		}
		INT_PDA_FactionData f = INT_PDA_SE_Store.Faction(factionId);
		if (!f)
			return;
		line.FactionId = f.Id;
		INT_PDA_SE_Store.AddFaction(f.Id, line);
		ToFaction(f, line, "");
	}

	static void OnPmSend(PlayerIdentity sender, PlayerBase player, INT_PDA_SE_Session s, ParamsReadContext ctx, int now)
	{
		string to;
		string text;
		if (!ctx.Read(to) || !ctx.Read(text))
			return;
		INT_PDA_Account a = INT_PDA_SE_Core.LoggedAccount(s);
		if (!a)
		{
			INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_PM_SEND, INT_PDA_Const.R_NOT_LOGGED_IN);
			return;
		}
		if (now - s.LastChatMs < 1000)
		{
			INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_PM_SEND, INT_PDA_Const.R_TOO_FAST);
			return;
		}
		INT_PDA pda;
		if (!INT_PDA_SE_Core.CheckPda(player, s, ctx, INT_PDA.SLOT_CHAT, pda))
		{
			INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_PM_SEND, INT_PDA_Const.R_NO_MODULE);
			return;
		}
		to.TrimInPlace();
		if (!INT_PDA_Util.IsCallsign(to))
		{
			INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_PM_SEND, INT_PDA_Const.R_NOT_FOUND);
			return;
		}
		string target = INT_PDA_SE_Store.NameOwner(to);
		if (target == "" || target == INT_PDA_SE_Store.OWNER_UNREADABLE || target == s.Steam)
		{
			INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_PM_SEND, INT_PDA_Const.R_NOT_FOUND);
			return;
		}
		text = INT_PDA_Util.CleanLine(text, INT_PDA_SE_Store.Cfg().ChatMaxChars, INT_PDA_Const.MAX_WIRE_BYTES - 100);
		if (text == "" || text.IndexOf("#") >= 0)
		{
			INT_PDA_SE_Core.SendResult(sender, INT_PDA_Const.OP_PM_SEND, INT_PDA_Const.R_TEXT);
			return;
		}
		s.LastChatMs = now;
		INT_PDA_FactionData f = INT_PDA_SE_Store.Faction(a.FactionId);
		INT_PDA_ChatLine line = MakeLine(a, f, INT_PDA_Const.CH_PRIVATE, 0, text);
		INT_PDA_Account ta = INT_PDA_SE_Store.Account(target);
		if (ta)
			line.To = ta.Callsign;
		else
			line.To = to;
		line.AuthorSteam = "";
		INT_PDA_SE_Session ts = INT_PDA_SE_Core.Session(target);
		if (ts && ts.LoggedIn && ts.Listening && INT_PDA_SE_Core.Online(ts))
		{
			SendLineTo(ts, line);
			ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_PM_NOTICE);
			INT_PDA_Proto.WriteStr(rpc, a.Callsign);
			rpc.Write(1);
			INT_PDA_SE_Core.SendToSession(rpc, ts, INT_PDA_Const.OP_PM_NOTICE);
			INT_PDA beeper = INT_PDA_SE_Core.ListenPda(ts);
			if (beeper)
				beeper.BeepServer();
		}
		else
		{
			INT_PDA_SE_Store.AddMail(target, line);
			s_Mailed++;
		}
		SendLine(sender, line);
		s_Lines++;
	}

	static void OnHistory(PlayerIdentity sender, PlayerBase player, INT_PDA_SE_Session s, ParamsReadContext ctx, int now)
	{
		int channel;
		if (!ctx.Read(channel))
			return;
		INT_PDA_Account a = INT_PDA_SE_Core.LoggedAccount(s);
		if (!a)
			return;
		if (channel == INT_PDA_Const.CH_PRIVATE)
		{
			DeliverMail(sender, s);
			return;
		}
		array<ref INT_PDA_ChatLine> lines;
		string myFaction = a.FactionId;
		if (channel == INT_PDA_Const.CH_FACTION)
		{
			if (!INT_PDA_SE_Store.Faction(a.FactionId))
				return;
			if (now - s.LastHistFacMs < HISTORY_GAP_MS)
				return;
			s.LastHistFacMs = now;
			lines = INT_PDA_SE_Store.FactionChat(a.FactionId).Lines;
		}
		else
		{
			if (now - s.LastHistPubMs < HISTORY_GAP_MS)
				return;
			s.LastHistPubMs = now;
			channel = INT_PDA_Const.CH_PUBLIC;
			lines = INT_PDA_SE_Store.s_Public.Lines;
		}
		SendHistory(sender, s, channel, lines, myFaction);
	}

	static void SendHistory(PlayerIdentity to, INT_PDA_SE_Session s, int channel, array<ref INT_PDA_ChatLine> lines, string myFaction)
	{
		int total = lines.Count();
		int pos = 0;
		bool first = true;
		while (first || pos < total)
		{
			int n = Math.Min(HISTORY_BATCH, total - pos);
			ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_CHAT_HISTORY_DATA);
			rpc.Write(channel);
			rpc.Write(first);
			rpc.Write(n);
			for (int i = 0; i < n; i++)
				ViewFor(lines[pos + i], s, myFaction).Write(rpc);
			INT_PDA_SE_Core.SendTo(rpc, to, INT_PDA_Const.OP_CHAT_HISTORY_DATA);
			pos = pos + n;
			first = false;
		}
	}

	static void OnNews(PlayerIdentity sender, INT_PDA_SE_Session s)
	{
		// only live news (a lifetime set by the admin ends them), with the seconds each one has left
		array<INT_PDA_News> items = new array<INT_PDA_News>();
		INT_PDA_SE_Store.LiveNews(items, 20);
		ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_NEWS_DATA);
		rpc.Write(items.Count());
		for (int i = 0; i < items.Count(); i++)
			items[i].Write(rpc);
		INT_PDA_SE_Core.SendTo(rpc, sender, INT_PDA_Const.OP_NEWS_DATA);
	}

	// The live news list again to every listener (an admin edited or deleted an item): no banner, no sound.
	static void PushNews()
	{
		array<INT_PDA_News> items = new array<INT_PDA_News>();
		INT_PDA_SE_Store.LiveNews(items, 20);
		foreach (string steam, INT_PDA_SE_Session s : INT_PDA_SE_Core.s_Sessions)
		{
			if (!s.LoggedIn || !s.Listening || !INT_PDA_SE_Core.Online(s))
				continue;
			ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_NEWS_DATA);
			rpc.Write(items.Count());
			for (int i = 0; i < items.Count(); i++)
				items[i].Write(rpc);
			INT_PDA_SE_Core.SendToSession(rpc, s, INT_PDA_Const.OP_NEWS_DATA);
		}
	}

	// Admin news: stored + a banner for every listener.
	static void FlashNews(INT_PDA_News n)
	{
		foreach (string steam, INT_PDA_SE_Session s : INT_PDA_SE_Core.s_Sessions)
		{
			if (!s.LoggedIn || !s.Listening || !INT_PDA_SE_Core.Online(s))
				continue;
			ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_NEWS_FLASH);
			n.Write(rpc);
			INT_PDA_SE_Core.SendToSession(rpc, s, INT_PDA_Const.OP_NEWS_FLASH);
		}
	}

	// «КПК сталкера <позывной>» of a PDA in the sender's inventory (looted PDA).
	static void OnPdaOwner(PlayerIdentity sender, PlayerBase player, INT_PDA_SE_Session s, ParamsReadContext ctx)
	{
		int low;
		int high;
		if (!ctx.Read(low) || !ctx.Read(high))
			return;
		INT_PDA pda = INT_PDA_SE_Core.PdaOf(player, low, high);
		string callsign = "";
		string steam = "";
		if (pda)
			steam = pda.OwnerSteam();
		if (steam != "")
		{
			INT_PDA_Account a = INT_PDA_SE_Store.ReadAccount(steam);
			if (a)
				callsign = a.Callsign;
		}
		ScriptRPC rpc = INT_PDA_Proto.Begin(INT_PDA_Const.OP_PDA_OWNER_DATA);
		rpc.Write(low);
		rpc.Write(high);
		INT_PDA_Proto.WriteStr(rpc, callsign);
		INT_PDA_SE_Core.SendTo(rpc, sender, INT_PDA_Const.OP_PDA_OWNER_DATA);
	}
}
