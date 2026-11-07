// INT_PDA (server) — INT_PDA_CONFIG.json (owner rules 16, 17, 23). Auto-created with defaults; a valid file is
// normalised (new keys appear after an update); a broken one is never overwritten (copy *.broken-<time>, defaults in
// memory). The engine JSON reader gives ZERO to missing fields and creates objects with ZERO fields, so every
// section is pre-created by ForRead() and every value is clamped by Sanitize(). A field named "Debug" would bind to
// the vanilla class Debug — it is "DebugMode".

class INT_PDA_SE_Config
{
	int ConfigVersion = 1;
	int DebugMode = 0;							// 1 = full debug in script.log (rule 16)
	ref array<string> Admins;					// SteamID64 of the PDA admins (admin tab, Alt+L, Ctrl+Alt+P)
	int DisableVanillaChat = 1;					// 1 = the game's own chat is off on every client (owner 2026-10-22)
	int ChatMaxChars = 200;
	float FloodSeconds = 2;						// client anti-flood; the server keeps a hard floor of 1 s
	float SameTextSeconds = 10;
	int AnonymousAllowed = 1;
	int HudFeedDefault = 1;						// the PDA feed on the HUD is on for new players
	int HudFeedLines = 3;
	float NearbyInviteRadius = 5;				// metres, «Рядом» list of the invite panel (editable in the admin tab)
	float LowBatteryPercent = 10;
	int PublicHistory = 100;					// public chat lines kept (memory + Chat/Public.json)
	int PublicSaveEvery = 20;					// Chat/Public.json is rewritten every N lines and at shutdown
	int FactionHistory = 50;
	int MailMax = 50;							// offline private messages kept per player
	int NewsMax = 20;
	int NewsRotateSeconds = 8;					// «Новости Зоны» line of the PDA: next live news every N s (admin tab)
	int NewsMinutesDefault = 0;					// lifetime offered for a new news item, minutes (0 = until deleted)
	int MarksMax = 20;							// faction map marks per faction
	int CallsignReserveDays = 7;				// an old callsign stays its owner's for N days after a change (0 = free at once)
	int LoginFails = 5;							// wrong passwords before a lock
	int LoginLockSeconds = 60;
	int AdminPageSize = 50;						// accounts per admin list page (files read per request)
	ref array<string> DefaultRanks;				// rank template of a new faction (the admin edits per faction)
	int CharacterSelect = 1;					// 1 = «Персонаж» tab: the player's own survivor for every new spawn (owner 2026-10-26)
	int CharacterRespawnMinutes = 10;			// «Переродиться» once per N minutes per player (0 = no limit)
	ref array<string> Characters;				// survivor classes offered in the tab (empty = the 31 vanilla ones)

	static INT_PDA_SE_Config Defaults()
	{
		INT_PDA_SE_Config c = new INT_PDA_SE_Config();
		c.Admins = new array<string>();
		c.Characters = new array<string>();
		c.DefaultRanks = new array<string>();
		c.DefaultRanks.Insert("Новобранец");
		c.DefaultRanks.Insert("Рядовой");
		c.DefaultRanks.Insert("Опытный");
		c.DefaultRanks.Insert("Ветеран");
		c.DefaultRanks.Insert("Мастер");
		return c;
	}

	// Object to read a file into: lists exist (reader trap), fields keep their initializers.
	static INT_PDA_SE_Config ForRead()
	{
		INT_PDA_SE_Config c = new INT_PDA_SE_Config();
		c.Admins = new array<string>();
		c.DefaultRanks = new array<string>();
		c.Characters = new array<string>();
		return c;
	}

	// Clamp everything; returns how many values were out of range.
	int Sanitize()
	{
		int fixes = 0;
		if (!Admins)
			Admins = new array<string>();
		if (!DefaultRanks)
			DefaultRanks = new array<string>();
		int i;
		string s;
		for (i = Admins.Count() - 1; i >= 0; i--)
		{
			s = Admins[i];
			s.TrimInPlace();
			if (!INT_PDA_Util.IsSteamId(s))
			{
				Admins.Remove(i);
				fixes++;
				continue;
			}
			Admins[i] = s;
		}
		for (i = DefaultRanks.Count() - 1; i >= 0; i--)
		{
			s = INT_PDA_Util.CleanLine(DefaultRanks[i], 24, INT_PDA_Const.MAX_NAME_BYTES);
			if (s == "")
			{
				DefaultRanks.Remove(i);
				fixes++;
				continue;
			}
			DefaultRanks[i] = s;
		}
		fixes = fixes + FixI(DebugMode, 0, 1);
		DebugMode = INT_PDA_Util.Bit(DebugMode);
		DisableVanillaChat = INT_PDA_Util.Bit(DisableVanillaChat);
		AnonymousAllowed = INT_PDA_Util.Bit(AnonymousAllowed);
		HudFeedDefault = INT_PDA_Util.Bit(HudFeedDefault);
		fixes = fixes + FixI(ChatMaxChars, 20, 240);
		ChatMaxChars = INT_PDA_Util.ClampI(ChatMaxChars, 20, 240);
		FloodSeconds = INT_PDA_Util.ClampF(FloodSeconds, 0, 60, 2);
		SameTextSeconds = INT_PDA_Util.ClampF(SameTextSeconds, 0, 600, 10);
		HudFeedLines = INT_PDA_Util.ClampI(HudFeedLines, 1, 8);
		NearbyInviteRadius = INT_PDA_Util.ClampF(NearbyInviteRadius, 1, 50, 5);
		LowBatteryPercent = INT_PDA_Util.ClampF(LowBatteryPercent, 0, 50, 10);
		fixes = fixes + FixI(PublicHistory, 10, 500);
		PublicHistory = INT_PDA_Util.ClampI(PublicHistory, 10, 500);
		PublicSaveEvery = INT_PDA_Util.ClampI(PublicSaveEvery, 1, 500);
		FactionHistory = INT_PDA_Util.ClampI(FactionHistory, 10, 300);
		MailMax = INT_PDA_Util.ClampI(MailMax, 1, 200);
		NewsMax = INT_PDA_Util.ClampI(NewsMax, 1, 20);		// the client gets 20 at most (OnNews)
		if (NewsRotateSeconds <= 0)
			NewsRotateSeconds = 8;				// missing in an older file (the reader gives 0)
		NewsRotateSeconds = INT_PDA_Util.ClampI(NewsRotateSeconds, 3, 300);
		NewsMinutesDefault = INT_PDA_Util.ClampI(NewsMinutesDefault, 0, 525600);
		MarksMax = INT_PDA_Util.ClampI(MarksMax, 1, 60);
		CallsignReserveDays = INT_PDA_Util.ClampI(CallsignReserveDays, 0, 365);
		LoginFails = INT_PDA_Util.ClampI(LoginFails, 1, 50);
		LoginLockSeconds = INT_PDA_Util.ClampI(LoginLockSeconds, 0, 3600);
		AdminPageSize = INT_PDA_Util.ClampI(AdminPageSize, 10, 200);
		if (DefaultRanks.Count() == 0)
		{
			DefaultRanks.Insert("Новобранец");
			DefaultRanks.Insert("Рядовой");
			DefaultRanks.Insert("Опытный");
			DefaultRanks.Insert("Ветеран");
			DefaultRanks.Insert("Мастер");
		}
		CharacterSelect = INT_PDA_Util.Bit(CharacterSelect);
		CharacterRespawnMinutes = INT_PDA_Util.ClampI(CharacterRespawnMinutes, 0, 1440);
		fixes = fixes + CleanCharacters();
		return fixes;
	}

	// Survivor classes of the tab: no duplicates, only classes the engine can make a player of (a typo or a class of
	// a mod that is not loaded is dropped with a warning — a player can never be left with a class that does not spawn).
	protected int CleanCharacters()
	{
		if (!Characters)
			Characters = new array<string>();
		int fixes = 0;
		array<string> seen = new array<string>();
		string type;
		for (int i = 0; i < Characters.Count(); i++)
		{
			type = Characters[i];
			type.TrimInPlace();
			if (type == "" || seen.Find(type) >= 0)
			{
				fixes++;
				continue;
			}
			if (GetGame() && !INT_PDA_Chars.IsSurvivor(type))
			{
				INT_PDA_Log.Warn("конфиг: Characters: «" + INT_PDA_Log.Clip(type, 40) + "» — не класс персонажа, пропущен");
				fixes++;
				continue;
			}
			seen.Insert(type);
		}
		if (seen.Count() > INT_PDA_Chars.MAX_TYPES)
			seen.Resize(INT_PDA_Chars.MAX_TYPES);
		Characters.Clear();
		Characters.InsertAll(seen);
		if (Characters.Count() == 0)
			INT_PDA_Chars.Vanilla(Characters);
		return fixes;
	}

	bool OffersCharacter(string type)
	{
		return CharacterSelect != 0 && type != "" && Characters.Find(type) >= 0;
	}

	protected int FixI(int v, int lo, int hi)
	{
		if (v < lo || v > hi)
			return 1;
		return 0;
	}

	bool IsAdmin(string steam64)
	{
		return steam64 != "" && Admins.Find(steam64) >= 0;
	}

	INT_PDA_Settings ToSettings()
	{
		INT_PDA_Settings s = new INT_PDA_Settings();
		s.DebugLog = DebugMode != 0;
		s.DisableVanillaChat = DisableVanillaChat != 0;
		s.ChatMaxChars = ChatMaxChars;
		s.FloodSeconds = FloodSeconds;
		s.SameTextSeconds = SameTextSeconds;
		s.NearbyRadius = NearbyInviteRadius;
		s.AnonymousAllowed = AnonymousAllowed != 0;
		s.HudFeedDefault = HudFeedDefault != 0;
		s.HudFeedLines = HudFeedLines;
		s.LowBatteryPercent = LowBatteryPercent;
		s.NewsRotateSeconds = NewsRotateSeconds;
		s.NewsMinutesDefault = NewsMinutesDefault;
		return s;
	}
}
