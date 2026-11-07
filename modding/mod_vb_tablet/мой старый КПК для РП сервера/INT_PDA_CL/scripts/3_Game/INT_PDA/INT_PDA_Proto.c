// INT_PDA — wire format shared by both sides (spec.md §7). Every packet starts with the operation code and the
// protocol version; a packet of another version is dropped. Every string stays below MAX_WIRE_BYTES (a string of
// 1024 bytes or more throws «String CORRUPTED» on 1.30); lists carry their count first and readers reject absurd
// counts, so a broken packet can never allocate without limit.

class INT_PDA_Proto
{
	static ScriptRPC Begin(int op)
	{
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(op);
		rpc.Write(INT_PDA_Const.PROTOCOL);
		return rpc;
	}

	static bool ReadHeader(ParamsReadContext ctx, out int op)
	{
		int wireVersion;
		if (!ctx.Read(op) || !ctx.Read(wireVersion))
			return false;
		return wireVersion == INT_PDA_Const.PROTOCOL;
	}

	static void WriteStr(ParamsWriteContext ctx, string s)
	{
		ctx.Write(INT_PDA_Util.FitBytes(s, INT_PDA_Const.MAX_WIRE_BYTES));
	}

	static bool ReadCount(ParamsReadContext ctx, out int n, int max)
	{
		if (!ctx.Read(n))
			return false;
		return n >= 0 && n <= max;
	}
}

// What the client needs from the server config (part of WELCOME).
class INT_PDA_Settings
{
	bool DebugLog;
	bool DisableVanillaChat = true;
	int ChatMaxChars = 200;
	float FloodSeconds = 2;				// client-side anti-flood (server checks only a hard floor)
	float SameTextSeconds = 10;
	float NearbyRadius = 5;				// «Рядом» list of the invite panel (metres)
	bool AnonymousAllowed = true;
	bool HudFeedDefault = true;			// PDA feed on the HUD for new players
	int HudFeedLines = 3;
	float LowBatteryPercent = 10;
	int NewsRotateSeconds = 8;			// «Новости Зоны» line of the PDA: next live news every N s (admin setting)
	int NewsMinutesDefault = 0;			// lifetime offered for a new news item (admin form), 0 = until deleted

	void Write(ParamsWriteContext ctx)
	{
		ctx.Write(DebugLog);
		ctx.Write(DisableVanillaChat);
		ctx.Write(ChatMaxChars);
		ctx.Write(FloodSeconds);
		ctx.Write(SameTextSeconds);
		ctx.Write(NearbyRadius);
		ctx.Write(AnonymousAllowed);
		ctx.Write(HudFeedDefault);
		ctx.Write(HudFeedLines);
		ctx.Write(LowBatteryPercent);
		ctx.Write(NewsRotateSeconds);
		ctx.Write(NewsMinutesDefault);
	}

	bool Read(ParamsReadContext ctx)
	{
		if (!ctx.Read(DebugLog) || !ctx.Read(DisableVanillaChat) || !ctx.Read(ChatMaxChars))
			return false;
		if (!ctx.Read(FloodSeconds) || !ctx.Read(SameTextSeconds) || !ctx.Read(NearbyRadius))
			return false;
		if (!ctx.Read(AnonymousAllowed) || !ctx.Read(HudFeedDefault) || !ctx.Read(HudFeedLines))
			return false;
		if (!ctx.Read(LowBatteryPercent))
			return false;
		return ctx.Read(NewsRotateSeconds) && ctx.Read(NewsMinutesDefault);
	}
}

// Own account as the client sees it after login.
class INT_PDA_Profile
{
	string SteamId;
	string Callsign;
	string Discord;
	int Avatar;
	string Registered;			// "YYYY-MM-DD HH:MM:SS" server time
	int DaysInZone;
	string FactionId;			// "" = no faction
	string FactionName;
	int FactionEmblem;
	int Role;					// INT_PDA_Const.ROLE_*
	string Rank;
	int LifeSec;				// play time of the current character (server stat) when the profile was sent

	void Write(ParamsWriteContext ctx)
	{
		INT_PDA_Proto.WriteStr(ctx, SteamId);
		INT_PDA_Proto.WriteStr(ctx, Callsign);
		INT_PDA_Proto.WriteStr(ctx, Discord);
		ctx.Write(Avatar);
		INT_PDA_Proto.WriteStr(ctx, Registered);
		ctx.Write(DaysInZone);
		INT_PDA_Proto.WriteStr(ctx, FactionId);
		INT_PDA_Proto.WriteStr(ctx, FactionName);
		ctx.Write(FactionEmblem);
		ctx.Write(Role);
		INT_PDA_Proto.WriteStr(ctx, Rank);
		ctx.Write(LifeSec);
	}

	bool Read(ParamsReadContext ctx)
	{
		if (!ctx.Read(SteamId) || !ctx.Read(Callsign) || !ctx.Read(Discord) || !ctx.Read(Avatar))
			return false;
		if (!ctx.Read(Registered) || !ctx.Read(DaysInZone) || !ctx.Read(FactionId) || !ctx.Read(FactionName))
			return false;
		return ctx.Read(FactionEmblem) && ctx.Read(Role) && ctx.Read(Rank) && ctx.Read(LifeSec);
	}

	bool InFaction()
	{
		return FactionId != "";
	}
}

// One chat line. Anonymous lines reach players without Author/AuthorSteam; admins get them (FL_ADMIN_VIEW).
class INT_PDA_ChatLine
{
	int Channel;				// INT_PDA_Const.CH_*
	int Flags;					// INT_PDA_Const.FL_*
	string Time;				// "DD.MM HH:MM" server time
	string Author;				// callsign ("" for anonymous / system)
	string AuthorSteam;			// only in admin copies and private lines
	int Avatar;
	string FactionId;			// author's faction (the client colours the callsign by its own relations)
	string FactionName;
	int FactionEmblem;
	string To;					// private: the other side's callsign
	string Text;
	// public / faction lines: server id (0 = none) and the quote of the line this one answers. The server builds the
	// quote from its own history (a client sends only ReplyId), so a reply can't put words into somebody's mouth;
	// the quoted author is "" for an anonymous line (ReplyFlags carry FL_ANON / FL_AS_FACTION of the original).
	int Id;
	int ReplyId;
	int ReplyFlags;
	string ReplyAuthor;
	string ReplyText;

	void Write(ParamsWriteContext ctx)
	{
		ctx.Write(Channel);
		ctx.Write(Flags);
		INT_PDA_Proto.WriteStr(ctx, Time);
		INT_PDA_Proto.WriteStr(ctx, Author);
		INT_PDA_Proto.WriteStr(ctx, AuthorSteam);
		ctx.Write(Avatar);
		INT_PDA_Proto.WriteStr(ctx, FactionId);
		INT_PDA_Proto.WriteStr(ctx, FactionName);
		ctx.Write(FactionEmblem);
		INT_PDA_Proto.WriteStr(ctx, To);
		INT_PDA_Proto.WriteStr(ctx, Text);
		ctx.Write(Id);
		ctx.Write(ReplyId);
		ctx.Write(ReplyFlags);
		INT_PDA_Proto.WriteStr(ctx, ReplyAuthor);
		INT_PDA_Proto.WriteStr(ctx, ReplyText);
	}

	bool Read(ParamsReadContext ctx)
	{
		if (!ctx.Read(Channel) || !ctx.Read(Flags) || !ctx.Read(Time) || !ctx.Read(Author))
			return false;
		if (!ctx.Read(AuthorSteam) || !ctx.Read(Avatar) || !ctx.Read(FactionId) || !ctx.Read(FactionName))
			return false;
		if (!ctx.Read(FactionEmblem) || !ctx.Read(To) || !ctx.Read(Text))
			return false;
		return ctx.Read(Id) && ctx.Read(ReplyId) && ctx.Read(ReplyFlags) && ctx.Read(ReplyAuthor) && ctx.Read(ReplyText);
	}

	bool IsReply()
	{
		return ReplyId > 0;
	}

	void CopyFrom(INT_PDA_ChatLine o)
	{
		Channel = o.Channel;
		Flags = o.Flags;
		Time = o.Time;
		Author = o.Author;
		AuthorSteam = o.AuthorSteam;
		Avatar = o.Avatar;
		FactionId = o.FactionId;
		FactionName = o.FactionName;
		FactionEmblem = o.FactionEmblem;
		To = o.To;
		Text = o.Text;
		Id = o.Id;
		ReplyId = o.ReplyId;
		ReplyFlags = o.ReplyFlags;
		ReplyAuthor = o.ReplyAuthor;
		ReplyText = o.ReplyText;
	}
}

class INT_PDA_Member
{
	string SteamId;
	string Callsign;
	string Rank;
	int Role;					// INT_PDA_Const.ROLE_*
	string Joined;				// "YYYY-MM-DD" server time
	[NonSerialized()]
	bool Online;

	void Write(ParamsWriteContext ctx, bool withSteam)
	{
		string steam = "";
		if (withSteam)
			steam = SteamId;
		INT_PDA_Proto.WriteStr(ctx, steam);
		INT_PDA_Proto.WriteStr(ctx, Callsign);
		INT_PDA_Proto.WriteStr(ctx, Rank);
		ctx.Write(Role);
		INT_PDA_Proto.WriteStr(ctx, Joined);
		ctx.Write(Online);
	}

	bool Read(ParamsReadContext ctx)
	{
		if (!ctx.Read(SteamId) || !ctx.Read(Callsign) || !ctx.Read(Rank))
			return false;
		return ctx.Read(Role) && ctx.Read(Joined) && ctx.Read(Online);
	}
}

class INT_PDA_Relation
{
	string FactionId;
	string Name;
	int Emblem;
	int Rel;					// INT_PDA_Const.REL_*
	string Reason;

	void Write(ParamsWriteContext ctx)
	{
		INT_PDA_Proto.WriteStr(ctx, FactionId);
		INT_PDA_Proto.WriteStr(ctx, Name);
		ctx.Write(Emblem);
		ctx.Write(Rel);
		INT_PDA_Proto.WriteStr(ctx, Reason);
	}

	bool Read(ParamsReadContext ctx)
	{
		return ctx.Read(FactionId) && ctx.Read(Name) && ctx.Read(Emblem) && ctx.Read(Rel) && ctx.Read(Reason);
	}
}

class INT_PDA_Mark
{
	int Id;
	int Kind;					// INT_PDA_Const.MARK_*
	string Name;
	float X;
	float Z;
	string Author;

	void Write(ParamsWriteContext ctx)
	{
		ctx.Write(Id);
		ctx.Write(Kind);
		INT_PDA_Proto.WriteStr(ctx, Name);
		ctx.Write(X);
		ctx.Write(Z);
		INT_PDA_Proto.WriteStr(ctx, Author);
	}

	bool Read(ParamsReadContext ctx)
	{
		return ctx.Read(Id) && ctx.Read(Kind) && ctx.Read(Name) && ctx.Read(X) && ctx.Read(Z) && ctx.Read(Author);
	}
}

// A faction as its members see it (OP_FACTION_DATA). Steam ids of members only for leader / deputy / admin.
class INT_PDA_FactionView
{
	static const int MAX_MEMBERS = 300;
	static const int MAX_LIST = 64;

	string Id;
	string Name;
	int Emblem;
	string Motto;
	string Base;
	string SpawnPos;
	ref array<ref INT_PDA_Member> Members = new array<ref INT_PDA_Member>();
	ref array<ref INT_PDA_Relation> Relations = new array<ref INT_PDA_Relation>();
	ref array<string> Ranks = new array<string>();
	ref array<ref INT_PDA_Mark> Marks = new array<ref INT_PDA_Mark>();

	void Write(ParamsWriteContext ctx, bool withSteam)
	{
		INT_PDA_Proto.WriteStr(ctx, Id);
		INT_PDA_Proto.WriteStr(ctx, Name);
		ctx.Write(Emblem);
		INT_PDA_Proto.WriteStr(ctx, Motto);
		INT_PDA_Proto.WriteStr(ctx, Base);
		INT_PDA_Proto.WriteStr(ctx, SpawnPos);
		int n = Math.Min(Members.Count(), MAX_MEMBERS);
		ctx.Write(n);
		int i;
		for (i = 0; i < n; i++)
			Members[i].Write(ctx, withSteam);
		n = Math.Min(Relations.Count(), MAX_LIST);
		ctx.Write(n);
		for (i = 0; i < n; i++)
			Relations[i].Write(ctx);
		n = Math.Min(Ranks.Count(), MAX_LIST);
		ctx.Write(n);
		for (i = 0; i < n; i++)
			INT_PDA_Proto.WriteStr(ctx, Ranks[i]);
		n = Math.Min(Marks.Count(), MAX_LIST);
		ctx.Write(n);
		for (i = 0; i < n; i++)
			Marks[i].Write(ctx);
	}

	bool Read(ParamsReadContext ctx)
	{
		if (!ctx.Read(Id) || !ctx.Read(Name) || !ctx.Read(Emblem) || !ctx.Read(Motto) || !ctx.Read(Base) || !ctx.Read(SpawnPos))
			return false;
		int n;
		int i;
		if (!INT_PDA_Proto.ReadCount(ctx, n, MAX_MEMBERS))
			return false;
		Members.Clear();
		for (i = 0; i < n; i++)
		{
			INT_PDA_Member m = new INT_PDA_Member();
			if (!m.Read(ctx))
				return false;
			Members.Insert(m);
		}
		if (!INT_PDA_Proto.ReadCount(ctx, n, MAX_LIST))
			return false;
		Relations.Clear();
		for (i = 0; i < n; i++)
		{
			INT_PDA_Relation r = new INT_PDA_Relation();
			if (!r.Read(ctx))
				return false;
			Relations.Insert(r);
		}
		if (!INT_PDA_Proto.ReadCount(ctx, n, MAX_LIST))
			return false;
		Ranks.Clear();
		string rank;
		for (i = 0; i < n; i++)
		{
			if (!ctx.Read(rank))
				return false;
			Ranks.Insert(rank);
		}
		if (!INT_PDA_Proto.ReadCount(ctx, n, MAX_LIST))
			return false;
		Marks.Clear();
		for (i = 0; i < n; i++)
		{
			INT_PDA_Mark k = new INT_PDA_Mark();
			if (!k.Read(ctx))
				return false;
			Marks.Insert(k);
		}
		return true;
	}

	int RelationTo(string factionId)
	{
		foreach (INT_PDA_Relation r : Relations)
		{
			if (r.FactionId == factionId)
				return r.Rel;
		}
		return INT_PDA_Const.REL_NEUTRAL;
	}
}

class INT_PDA_News
{
	int Id;
	string Time;
	string Title;
	string Text;
	int Minutes;				// how long it is shown, set by the admin (0 = until deleted)
	int ExpireAt;				// server: UTC seconds (INT_PDA_Util.NowSec) when it goes; 0 = never
	[NonSerialized()]
	int LeftSec = -1;			// on the wire: seconds left when sent (-1 = never)
	[NonSerialized()]
	int ExpireMs = -1;			// client: GetGame().GetTime() when it goes (-1 = never)

	void Write(ParamsWriteContext ctx)
	{
		ctx.Write(Id);
		INT_PDA_Proto.WriteStr(ctx, Time);
		INT_PDA_Proto.WriteStr(ctx, Title);
		INT_PDA_Proto.WriteStr(ctx, Text);
		ctx.Write(Minutes);
		ctx.Write(LeftSec);
	}

	bool Read(ParamsReadContext ctx)
	{
		if (!ctx.Read(Id) || !ctx.Read(Time) || !ctx.Read(Title) || !ctx.Read(Text))
			return false;
		if (!ctx.Read(Minutes) || !ctx.Read(LeftSec))
			return false;
		ExpireMs = -1;
		if (LeftSec >= 0 && GetGame())
			ExpireMs = GetGame().GetTime() + LeftSec * 1000;
		return true;
	}

	// client: still to be shown (the server sends only live news; this hides one that ran out while cached)
	bool IsLive()
	{
		if (ExpireMs < 0 || !GetGame())
			return true;
		return GetGame().GetTime() < ExpireMs;
	}
}

// A faction mate's position (one-shot answer, no tracking).
class INT_PDA_MatePos
{
	string Callsign;
	float X;
	float Z;

	void Write(ParamsWriteContext ctx)
	{
		INT_PDA_Proto.WriteStr(ctx, Callsign);
		ctx.Write(X);
		ctx.Write(Z);
	}

	bool Read(ParamsReadContext ctx)
	{
		return ctx.Read(Callsign) && ctx.Read(X) && ctx.Read(Z);
	}
}

// Admin tab: one account (the admins see the password and the Steam ID — owner brief, admin item 4).
class INT_PDA_AdminRow
{
	string SteamId;
	string Callsign;
	string Password;
	string Discord;
	string Registered;
	string LastSeen;
	string GameName;
	string FactionId;
	string FactionName;
	int Role;
	string Rank;
	int Avatar;
	bool Online;

	void Write(ParamsWriteContext ctx)
	{
		INT_PDA_Proto.WriteStr(ctx, SteamId);
		INT_PDA_Proto.WriteStr(ctx, Callsign);
		INT_PDA_Proto.WriteStr(ctx, Password);
		INT_PDA_Proto.WriteStr(ctx, Discord);
		INT_PDA_Proto.WriteStr(ctx, Registered);
		INT_PDA_Proto.WriteStr(ctx, LastSeen);
		INT_PDA_Proto.WriteStr(ctx, GameName);
		INT_PDA_Proto.WriteStr(ctx, FactionId);
		INT_PDA_Proto.WriteStr(ctx, FactionName);
		ctx.Write(Role);
		INT_PDA_Proto.WriteStr(ctx, Rank);
		ctx.Write(Avatar);
		ctx.Write(Online);
	}

	bool Read(ParamsReadContext ctx)
	{
		if (!ctx.Read(SteamId) || !ctx.Read(Callsign) || !ctx.Read(Password) || !ctx.Read(Discord))
			return false;
		if (!ctx.Read(Registered) || !ctx.Read(LastSeen) || !ctx.Read(GameName) || !ctx.Read(FactionId))
			return false;
		return ctx.Read(FactionName) && ctx.Read(Role) && ctx.Read(Rank) && ctx.Read(Avatar) && ctx.Read(Online);
	}
}

// Admin tab: a faction in the list.
class INT_PDA_FactionSummary
{
	string Id;
	string Name;
	int Emblem;
	int Members;
	int Online;
	string Leader;

	void Write(ParamsWriteContext ctx)
	{
		INT_PDA_Proto.WriteStr(ctx, Id);
		INT_PDA_Proto.WriteStr(ctx, Name);
		ctx.Write(Emblem);
		ctx.Write(Members);
		ctx.Write(Online);
		INT_PDA_Proto.WriteStr(ctx, Leader);
	}

	bool Read(ParamsReadContext ctx)
	{
		return ctx.Read(Id) && ctx.Read(Name) && ctx.Read(Emblem) && ctx.Read(Members) && ctx.Read(Online) && ctx.Read(Leader);
	}
}
