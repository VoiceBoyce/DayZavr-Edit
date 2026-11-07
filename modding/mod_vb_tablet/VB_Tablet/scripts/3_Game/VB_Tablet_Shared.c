// VB_Tablet: то, что нужно и клиенту, и серверу. Протокол менять только вместе с VB_Tablet_SE.
//
// Обмен идёт через ScriptRPC на PlayerBase, поля пишутся по одному.
//   Клиент -> сервер, VB_TABLET_RPC_C2S: int op, дальше данные операции.
//   Сервер -> клиент, VB_TABLET_RPC_S2C: int op, дальше данные операции.

const string VB_TABLET_VERSION = "0.1.0";
const string VB_TABLET_TAG = "[VB_Tablet]";

// Ваниль занимает меню 1..48, фреймворк 9110xxx.
const int VB_TABLET_MENU_ID = 9141001;
const int VB_TABLET_RPC_C2S = 9141011;
const int VB_TABLET_RPC_S2C = 9141012;

enum VB_TabletOp
{
	HELLO = 1,		// C2S пусто; S2C: string имя сервера, bool администратор, VB_TabletRates, int цены: название, тег, эмблема, цвет
	PROFILE = 2,	// C2S пусто; S2C: VB_TabletStats
	RATING = 3,		// C2S пусто; S2C: int мест всего, int строк, строки (string, int опыт, string тег), int[10] игроков по рангам
	RESULT = 4,		// S2C: int VB_TabletResult
	CLAN = 10,		// C2S пусто; S2C: bool есть клан, VB_Clan.WriteHead, инвайты, id клана заявки
	CLAN_LIST = 11,	// C2S пусто; S2C: int строк, VB_ClanRow
	CLAN_JOURNAL = 12,	// C2S пусто; S2C: int строк, VB_ClanJournalRow
	CLAN_NEWS = 13,	// C2S пусто; S2C: int строк, VB_ClanNews
	CLAN_CHAT = 14,	// C2S пусто; S2C: int строк, VB_ClanChatRow
	CLAN_BANK = 15,	// C2S пусто; S2C: int строк, VB_ClanBankRow
	CLAN_CREATE = 20,	// string название, тег, слоган, форма, символ, int цвет
	CLAN_APPLY = 21,	// string клан, текст, discord
	CLAN_APPLY_CANCEL = 22,
	CLAN_REQUEST = 23,	// string uid, bool принять
	CLAN_INVITE = 24,	// string ник
	CLAN_INVITE_ANSWER = 25,	// string клан, bool принять
	CLAN_KICK = 26,		// string uid
	CLAN_SET_RANK = 27,	// string uid, string ранг
	CLAN_LEAVE = 28,
	CLAN_DISSOLVE = 29,	// string название клана для подтверждения
	CLAN_LEADER = 30,	// string uid нового лидера
	CLAN_SETTINGS = 31,	// string название, тег, слоган, форма, символ, int цвет, bool набор, string discord
	CLAN_RANKS = 32,	// int строк, VB_ClanRank
	NEWS_SAVE = 33,		// int id (0 новая), int тип, string заголовок, текст, bool закрепить
	NEWS_DELETE = 34,	// int id
	NEWS_PIN = 35,		// int id
	CHAT_SEND = 36,		// string текст
	PLAYER_CARD = 80,	// string uid (или пусто), string имя; S2C: string имя, VB_TabletStats
	DISCORD_SET = 37,	// string имя
	ATM_PLACE = 40,		// float x, y, z, угол
	ATM_DEPOSIT = 41,	// int сумма
	ATM_WITHDRAW = 42,	// int сумма
	CONTRACTS = 43,		// C2S пусто; S2C: int секунд до нового набора, bool замена доступна, int строк, VB_ClanContract клановые, int строк, VB_ClanContract личные
	CONTRACT_CLAIM = 44,	// int индекс личного контракта
	CONTRACT_REPLACE = 45,	// int индекс личного контракта
	NOTIFY = 46,		// S2C: string ключ, string аргумент, int вид (0 инфо, 1 успех, 2 ошибка)
	ADMIN_MEMBERS = 47,	// C2S string клан; S2C: string клан, int строк, строки (string uid, string ник, bool лидер)
	ADMIN_RENAME = 48,	// string клан, название, тег
	ADMIN_LEADER = 49,	// string клан, uid нового лидера
	ADMIN_DISSOLVE = 50,	// string клан, название клана для подтверждения
	ADMIN_BANNED = 51,	// C2S пусто; S2C: int строк, string слова
	ADMIN_BAN_ADD = 52,	// string слово
	ADMIN_BAN_DEL = 53,	// string слово
	MAP_GET = 60,		// C2S пусто; S2C: MAP_CONF, затем MAP_SCOPE по каждой области
	MAP_CONF = 61,		// S2C: VB_MapLimits, ключи скрытых, ключи 3D, скрытые категории
	MAP_SCOPE = 62,		// S2C: int область, int строк, VB_MapCat, int строк, VB_MapObject
	MAP_SAVE = 63,		// int прежняя область, VB_MapObject (ObjId 0 новый)
	MAP_DELETE = 64,	// int область, int id
	MAP_CAT_SAVE = 65,	// VB_MapCat (CatId 0 новая)
	MAP_CAT_DELETE = 66,	// int id
	MAP_PREF = 67,		// int VB_MapPref, int ключ, int VB_MapMode
	MAP_MATES = 68,		// C2S пусто; S2C: int строк, VB_MapMate
}

// Палитра макета. Акцент позже берётся из цвета клана.
const int VB_C_BG = 0xFF121110;
const int VB_C_SIDE = 0xFF0E0D0C;
const int VB_C_PANEL = 0xFF191816;
const int VB_C_PANEL2 = 0xFF211F1D;
const int VB_C_PANEL3 = 0xFF2B2926;
const int VB_C_LINE = 0xFF2B2926;
const int VB_C_LINE2 = 0xFF413D38;
const int VB_C_TEXT = 0xFFECE8E1;
const int VB_C_DIM = 0xFF938E86;
const int VB_C_DIM2 = 0xFF625E58;
const int VB_ACCENT_DEFAULT = 0xFFFF7A1A;
// Акцент не константа: это цвет клана, без клана оранжевый.
int VB_C_ACCENT = 0xFFFF7A1A;
const int VB_C_ON = 0xFF0E0D0C;
const int VB_C_OK = 0xFF6FBF8E;
const int VB_C_BAD = 0xFFEF5B4C;
const int VB_C_WHITE = 0xFFFFFFFF;

const string VB_TABLET_IMAGESET = "VB_Tablet_Data/gui/imagesets/vb_tablet.imageset";
const string VB_TABLET_LAYOUT_SCROLL = "VB_Tablet/gui/layouts/vb_tablet_scroll.layout";
const string VB_TABLET_LAYOUT_AREA = "VB_Tablet/gui/layouts/vb_tablet_area.layout";
const string VB_TABLET_ICON = "set:vb_tablet image:";
const string VB_TABLET_IMAGESET_EM = "VB_Tablet_Data/gui/imagesets/vb_tablet_em.imageset";
const string VB_TABLET_ICON_EM = "set:vb_tablet_em image:";
const string VB_TABLET_TEX_DEVICE = "VB_Tablet_Data/gui/textures/vb_tablet_device_ca.paa";
const string VB_TABLET_TEX_GLASS = "VB_Tablet_Data/gui/textures/vb_tablet_glass_ca.paa";
const string VB_TABLET_TEX_HAZARD = "VB_Tablet_Data/gui/textures/vb_tablet_hazard_ca.paa";
const string VB_TABLET_LAYOUT_PREVIEW = "VB_Tablet/gui/layouts/vb_tablet_preview.layout";

const int VB_C_GOLD = 0xFFE9C55B;
const int VB_C_SILVER = 0xFFCFCAC2;
const int VB_C_BRONZE = 0xFFC98A5A;
const int VB_C_INF = 0xFF8FA6FF;
const int VB_RANK_COUNT = 10;
const int VB_ACH_COUNT = 16;
const int VB_ACH_STEPS = 3;
const int VB_CLAN_LEVELS = 5;

// Опыт за действия и пороги рангов. Значения из конфига сервера, клиент получает их в HELLO.
class VB_TabletRates
{
	int XpZombie = 1;
	int XpMutant = 10;
	int XpAnimal = 2;
	int XpKm = 2;
	int XpContract = 10;
	ref array<int> RankNeed = {0, 500, 1500, 4000, 10000, 25000, 60000, 120000, 250000, 500000};

	// Пороги влияния для уровней клана: Levels[n-1] открывает уровень n. Сервер берёт их из конфига.
	[NonSerialized()]
	protected static ref array<int> s_Levels;

	static array<int> Levels()
	{
		if (!s_Levels)
		{
			s_Levels = new array<int>();
			s_Levels.Insert(0);
			s_Levels.Insert(1000);
			s_Levels.Insert(2000);
			s_Levels.Insert(5000);
			s_Levels.Insert(10000);
		}
		return s_Levels;
	}

	static void SetLevels(array<int> list)
	{
		if (!list || list.Count() != VB_CLAN_LEVELS)
			return;
		Levels().Clear();
		foreach (int v : list)
			Levels().Insert(v);
	}

	void Write(ScriptRPC rpc)
	{
		rpc.Write(XpZombie);
		rpc.Write(XpMutant);
		rpc.Write(XpAnimal);
		rpc.Write(XpKm);
		rpc.Write(XpContract);
		for (int i = 0; i < VB_RANK_COUNT; i++)
		{
			int need = RankNeed.Get(i);
			rpc.Write(need);
		}
		array<int> levels = Levels();
		foreach (int lv : levels)
			rpc.Write(lv);
	}

	bool Read(ParamsReadContext ctx)
	{
		if (!ctx.Read(XpZombie) || !ctx.Read(XpMutant) || !ctx.Read(XpAnimal) || !ctx.Read(XpKm) || !ctx.Read(XpContract))
			return false;
		RankNeed.Clear();
		for (int i = 0; i < VB_RANK_COUNT; i++)
		{
			int need;
			if (!ctx.Read(need))
				return false;
			RankNeed.Insert(need);
		}
		array<int> levels = new array<int>();
		for (int k = 0; k < VB_CLAN_LEVELS; k++)
		{
			int lv;
			if (!ctx.Read(lv))
				return false;
			levels.Insert(lv);
		}
		SetLevels(levels);
		return true;
	}

	int RankOf(int xp)
	{
		int r = 0;
		for (int i = 0; i < VB_RANK_COUNT; i++)
		{
			if (xp >= RankNeed.Get(i))
				r = i;
		}
		return r;
	}
}

// Показатели игрока: сервер считает и присылает, клиент только показывает.
class VB_TabletStats
{
	string PlayerName;
	string Discord;
	int Zombies;
	int Mutants;
	int Animals;
	int DistM;
	int LifeSec;
	int RecordSec;
	int Days;
	int Contracts;
	int ExtraXp;	// разница между опытом личных контрактов и базовым XpContract
	int AchBoss;
	int AchCar;
	int AchGather;
	int AchFish;
	int AchCraft;
	int AchBuild;
	int AchTrophy;
	int AchCard;
	int Place;
	int Cash;

	void Write(ScriptRPC rpc)
	{
		rpc.Write(PlayerName);
		rpc.Write(Discord);
		rpc.Write(Zombies);
		rpc.Write(Mutants);
		rpc.Write(Animals);
		rpc.Write(DistM);
		rpc.Write(LifeSec);
		rpc.Write(RecordSec);
		rpc.Write(Days);
		rpc.Write(Contracts);
		rpc.Write(ExtraXp);
		rpc.Write(Place);
		rpc.Write(Cash);
		rpc.Write(AchBoss);
		rpc.Write(AchCar);
		rpc.Write(AchGather);
		rpc.Write(AchFish);
		rpc.Write(AchCraft);
		rpc.Write(AchBuild);
		rpc.Write(AchTrophy);
		rpc.Write(AchCard);
	}

	bool Read(ParamsReadContext ctx)
	{
		if (!ctx.Read(PlayerName) || !ctx.Read(Discord) || !ctx.Read(Zombies) || !ctx.Read(Mutants) || !ctx.Read(Animals))
			return false;
		if (!ctx.Read(DistM) || !ctx.Read(LifeSec) || !ctx.Read(RecordSec) || !ctx.Read(Days) || !ctx.Read(Contracts) || !ctx.Read(ExtraXp) || !ctx.Read(Place) || !ctx.Read(Cash))
			return false;
		if (!ctx.Read(AchBoss) || !ctx.Read(AchCar) || !ctx.Read(AchGather) || !ctx.Read(AchFish) || !ctx.Read(AchCraft) || !ctx.Read(AchBuild) || !ctx.Read(AchTrophy) || !ctx.Read(AchCard))
			return false;
		return true;
	}

	int BaseXp(VB_TabletRates r)
	{
		int km = DistM / 1000;
		return Zombies * r.XpZombie + Mutants * r.XpMutant + Animals * r.XpAnimal + km * r.XpKm + Contracts * r.XpContract + ExtraXp;
	}

	// Значение показателя достижения. Ступень ранга считается без наград за достижения.
	int AchValue(int ach, VB_TabletRates r)
	{
		// Порядок рядами: охота, выживание, ремесло, сервер.
		switch (ach)
		{
			case 0:
				return Zombies;
			case 1:
				return Mutants;
			case 2:
				return Animals;
			case 3:
				return AchBoss;
			case 4:
				return DistM / 1000;
			case 5:
				return RecordSec / 3600;
			case 6:
				return Days;
			case 7:
				return AchCar;
			case 8:
				return AchGather;
			case 9:
				return AchFish;
			case 10:
				return AchCraft;
			case 11:
				return AchBuild;
			case 12:
				return Contracts;
			case 13:
				return AchTrophy;
			case 14:
				return AchCard;
		}
		return r.RankOf(BaseXp(r)) + 1;
	}

	int AchDone(int ach, VB_TabletRates r)
	{
		int val = AchValue(ach, r);
		int done = 0;
		for (int i = 0; i < VB_ACH_STEPS; i++)
		{
			if (val >= VB_TabletAch.Step(ach, i))
				done++;
		}
		return done;
	}

	int AchXp(VB_TabletRates r)
	{
		int xp = 0;
		for (int a = 0; a < VB_ACH_COUNT; a++)
		{
			int done = AchDone(a, r);
			for (int i = 0; i < done; i++)
				xp += VB_TabletAch.Reward(i);
		}
		return xp;
	}

	int AchCount(VB_TabletRates r)
	{
		int n = 0;
		for (int a = 0; a < VB_ACH_COUNT; a++)
			n += AchDone(a, r);
		return n;
	}

	int Xp(VB_TabletRates r)
	{
		return BaseXp(r) + AchXp(r);
	}
}

// Достижения: 8 показателей по 3 ступени. Значок и текст по индексу.
class VB_TabletAch
{
	static int Step(int ach, int step)
	{
		array<int> all = {100, 1000, 10000, 10, 100, 1000, 25, 250, 2500, 10, 100, 500, 50, 500, 5000, 24, 100, 500, 7, 30, 180, 100, 1000, 10000, 50, 500, 2500, 25, 250, 1000, 50, 500, 5000, 25, 250, 1000, 10, 100, 1000, 5, 50, 250, 15, 75, 300, 3, 6, 10};
		return all.Get(ach * VB_ACH_STEPS + step);
	}

	static int Reward(int step)
	{
		if (step == 0)
			return 50;
		if (step == 1)
			return 250;
		return 1000;
	}

	static string IconOf(int ach)
	{
		array<string> all = {"zombie", "crosshair", "stash", "sym_skull", "route", "heart", "clock", "car", "sym_tree", "fish", "sym_cog", "sym_hammer", "contract", "vault", "sym_key", "rank"};
		return all.Get(ach);
	}
}

// Проверки ввода, одинаковые на клиенте и на сервере.
class VB_TabletCheck
{
	protected static bool OnlyChars(string s, string allowed)
	{
		int n = s.Length();
		for (int i = 0; i < n; i++)
		{
			if (allowed.IndexOf(s.Get(i)) < 0)
				return false;
		}
		return true;
	}

	// Имя пользователя Discord. 0 верно, 1 длина, 2 символы, 3 две точки подряд.
	static int DiscordError(string v)
	{
		int len = v.Length();
		if (len < 2 || len > 32)
			return 1;
		if (!OnlyChars(v, "abcdefghijklmnopqrstuvwxyz0123456789_."))
			return 2;
		if (v.IndexOf("..") >= 0)
			return 3;
		return 0;
	}

	// Ссылка на сервер Discord приводится к виду discord.gg/код. Пустая строка: это не приглашение.
	static string ClanInvite(string v)
	{
		string s = v + "";
		s.TrimInPlace();
		string low = s + "";
		low.ToLower();
		array<string> heads = {"https://", "http://", "www."};
		foreach (string head : heads)
		{
			if (low.IndexOf(head) == 0)
			{
				int cut = head.Length();
				s = s.Substring(cut, s.Length() - cut);
				low = low.Substring(cut, low.Length() - cut);
			}
		}
		array<string> hosts = {"discord.gg/", "discord.com/invite/", "discordapp.com/invite/"};
		string code = "";
		foreach (string host : hosts)
		{
			if (low.IndexOf(host) == 0)
			{
				int from = host.Length();
				int rest = s.Length() - from;
				code = s.Substring(from, rest);
			}
		}
		int len = code.Length();
		if (len > 0 && code.Get(len - 1) == "/")
		{
			len = len - 1;
			code = code.Substring(0, len);
		}
		if (len < 2 || len > 32)
			return "";
		if (!OnlyChars(code, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-"))
			return "";
		return "discord.gg/" + code;
	}
}
