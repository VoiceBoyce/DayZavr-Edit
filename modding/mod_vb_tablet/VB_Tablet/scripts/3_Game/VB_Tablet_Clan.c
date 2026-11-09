// Данные клана: общие для сервера и клиента. Сервер хранит их в JSON, клиент получает полями по ScriptRPC.

// Права рангов: биты, поэтому ранг хранит одно число.
enum VB_ClanPerm
{
	INVITE = 1,
	KICK = 2,
	ASSIGN = 4,
	RANKS = 8,
	MAP = 16,
	BANK = 32,
	SETTINGS = 64,
	NEWS = 128,
	ATM = 256,	// было «ставить банкомат»: банкомат клана убран, бит не используется
	ALL = 511
}

// Записи журнала. Текст собирает клиент по ключу STR_VB_TAB_J_<номер>, поэтому журнал двуязычный.
enum VB_ClanEvent
{
	CREATED = 0,
	JOINED = 1,
	LEFT = 2,
	KICKED = 3,
	RANK = 4,
	LEVEL = 5,
	NEWS = 6,
	SETTINGS = 7,
	MAP = 8,
	DEPOSIT = 9,
	WITHDRAW = 10,
	TAX = 11,
	CONTRACT = 12,
	ATM_SET = 13,
	ATM_MOVE = 14,
	LEADER = 15,
	WEEK_PRIZE = 16,
	FROM_ACCOUNT = 17,	// перевод с личного счёта банка на склад
	TO_ACCOUNT = 18		// со склада на личный счёт
}

// Ответы сервера на действия. Текст по ключу STR_VB_TAB_R_<номер>.
enum VB_TabletResult
{
	OK = 0,
	DENIED = 1,
	BAD_INPUT = 2,
	NAME_TAKEN = 3,
	TAG_TAKEN = 4,
	EMBLEM_TAKEN = 5,
	SERVER_FULL = 6,
	NO_MONEY = 7,
	NO_RANK = 8,
	NO_ROOM = 9,
	BANNED_WORD = 10,
	CLAN_FULL = 11,
	HAS_CLAN = 12,
	NOT_FOUND = 13,
	NOT_OPEN = 14,
	BANK_FULL = 15,
	NO_FUNDS = 16,
	RATE = 17,
	NOT_NEAR = 18,
	CREATED = 19,
	SAVED = 20,
	SENT = 21,
	NO_ATM = 22,		// рядом нет банкомата банка
	ACC_NO_FUNDS = 23,	// на личном счёте банка не хватает денег
	ACC_FULL = 24,		// личный счёт банка переполнится
	NO_BANK = 25,		// банк сервера (INT_Banking_SE) не стоит или не готов
	STORAGE = 40,	// файл не записан, операция отменена
	MAP_LIMIT = 60,	// лимит объектов карты
	MAP_D3 = 61		// лимит 3D меток
}

const int VB_CLAN_RANKS_MAX = 8;
const int VB_CLAN_JOURNAL_MAX = 200;
const int VB_CLAN_NEWS_MAX = 50;
const int VB_CLAN_HISTORY_MAX = 100;

class VB_ClanRank
{
	string Id;
	string Title;
	int Color;
	int Perms;

	void VB_ClanRank(string id = "", string title = "", int color = 0, int perms = 0)
	{
		Id = id;
		Title = title;
		Color = color;
		Perms = perms;
	}

	void Write(ScriptRPC rpc)
	{
		rpc.Write(Id);
		rpc.Write(Title);
		rpc.Write(Color);
		rpc.Write(Perms);
	}

	bool Read(ParamsReadContext ctx)
	{
		return ctx.Read(Id) && ctx.Read(Title) && ctx.Read(Color) && ctx.Read(Perms);
	}

	bool Can(int perm)
	{
		return (Perms & perm) != 0;
	}
}

class VB_ClanMember
{
	string Uid;
	string PlayerName;
	string RankId;
	int Today;
	int Total;
	string Discord;
	string LastSeen;	// дата последнего выхода
	int LastDay;		// только на сервере: номер дня последнего захода (неактивность клана, пропавший лидер)
	ref array<int> Week = {0, 0, 0, 0, 0, 0, 0};	// вклад по дням недели, с понедельника
	[NonSerialized()]
	bool Online;

	void AddToday(int value, int day)
	{
		Today = Today + value;
		Week.Set(day, Week.Get(day) + value);
	}

	void Write(ScriptRPC rpc)
	{
		rpc.Write(Uid);
		rpc.Write(PlayerName);
		rpc.Write(RankId);
		rpc.Write(Today);
		rpc.Write(Total);
		rpc.Write(Discord);
		rpc.Write(LastSeen);
		rpc.Write(Online);
		for (int i = 0; i < 7; i++)
		{
			int w = Week.Get(i);
			rpc.Write(w);
		}
	}

	bool Read(ParamsReadContext ctx)
	{
		if (!ctx.Read(Uid) || !ctx.Read(PlayerName) || !ctx.Read(RankId) || !ctx.Read(Today))
			return false;
		if (!ctx.Read(Total) || !ctx.Read(Discord) || !ctx.Read(LastSeen) || !ctx.Read(Online))
			return false;
		Week.Clear();
		for (int i = 0; i < 7; i++)
		{
			int w;
			if (!ctx.Read(w))
				return false;
			Week.Insert(w);
		}
		return true;
	}
}

class VB_ClanRequest
{
	string Uid;
	string PlayerName;
	string Discord;
	string Msg;
	string Time;
	int Xp;

	void Write(ScriptRPC rpc)
	{
		rpc.Write(Uid);
		rpc.Write(PlayerName);
		rpc.Write(Discord);
		rpc.Write(Msg);
		rpc.Write(Time);
		rpc.Write(Xp);
	}

	bool Read(ParamsReadContext ctx)
	{
		return ctx.Read(Uid) && ctx.Read(PlayerName) && ctx.Read(Discord) && ctx.Read(Msg) && ctx.Read(Time) && ctx.Read(Xp);
	}
}

class VB_ClanJournalRow
{
	int Kind;
	string Who;
	string Arg;
	string Val;
	string Day;		// ДД.ММ
	string Time;	// ЧЧ:ММ

	void Write(ScriptRPC rpc)
	{
		rpc.Write(Kind);
		rpc.Write(Who);
		rpc.Write(Arg);
		rpc.Write(Val);
		rpc.Write(Day);
		rpc.Write(Time);
	}

	bool Read(ParamsReadContext ctx)
	{
		return ctx.Read(Kind) && ctx.Read(Who) && ctx.Read(Arg) && ctx.Read(Val) && ctx.Read(Day) && ctx.Read(Time);
	}
}

class VB_ClanNews
{
	int NewsId;
	int TagIdx;
	string Head;
	string Body;
	string Author;
	string Time;
	bool Pin;

	void Write(ScriptRPC rpc)
	{
		rpc.Write(NewsId);
		rpc.Write(TagIdx);
		rpc.Write(Head);
		rpc.Write(Body);
		rpc.Write(Author);
		rpc.Write(Time);
		rpc.Write(Pin);
	}

	bool Read(ParamsReadContext ctx)
	{
		return ctx.Read(NewsId) && ctx.Read(TagIdx) && ctx.Read(Head) && ctx.Read(Body) && ctx.Read(Author) && ctx.Read(Time) && ctx.Read(Pin);
	}
}

class VB_ClanChatRow
{
	string Who;
	string Msg;
	string Day;
	string Time;

	void Write(ScriptRPC rpc)
	{
		rpc.Write(Who);
		rpc.Write(Msg);
		rpc.Write(Day);
		rpc.Write(Time);
	}

	bool Read(ParamsReadContext ctx)
	{
		return ctx.Read(Who) && ctx.Read(Msg) && ctx.Read(Day) && ctx.Read(Time);
	}
}

class VB_ClanBankRow
{
	string Who;
	int Kind;	// VB_ClanEvent: DEPOSIT, WITHDRAW, FROM_ACCOUNT, TO_ACCOUNT, TAX, CONTRACT, WEEK_PRIZE, SETTINGS
	int Sum;	// со знаком
	string Time;

	void Write(ScriptRPC rpc)
	{
		rpc.Write(Who);
		rpc.Write(Kind);
		rpc.Write(Sum);
		rpc.Write(Time);
	}

	bool Read(ParamsReadContext ctx)
	{
		return ctx.Read(Who) && ctx.Read(Kind) && ctx.Read(Sum) && ctx.Read(Time);
	}
}

// Состояние контракта.
enum VB_ContractStage
{
	ACTIVE = 0,
	READY = 1,	// личный: выполнен, ждёт «Забрать»
	DONE = 2
}

// Контракт на сегодня: клановый лежит в файле клана, личный в файле игрока.
// Копия записи конфига: правка конфига не меняет уже выданный набор.
class VB_ClanContract
{
	string Code;
	int Tier;
	string Kind;	// zombie, mutant, animal, km, hours
	int Need;
	int Have;
	int Reward;		// влияние
	int Cash;		// деньги на склад, только клановые
	int Xp;			// опыт в общий ранг, показывается у личных
	string Ico;
	string Title;
	string Hint;
	int Stage;
	// Вклад: только на сервере, клиенту не передаётся.
	ref array<string> Uids = new array<string>();
	ref array<int> Parts = new array<int>();

	void Write(ScriptRPC rpc)
	{
		rpc.Write(Code);
		rpc.Write(Tier);
		rpc.Write(Need);
		rpc.Write(Have);
		rpc.Write(Reward);
		rpc.Write(Cash);
		rpc.Write(Xp);
		rpc.Write(Ico);
		rpc.Write(Title);
		rpc.Write(Hint);
		rpc.Write(Stage);
	}

	bool Read(ParamsReadContext ctx)
	{
		if (!ctx.Read(Code) || !ctx.Read(Tier) || !ctx.Read(Need) || !ctx.Read(Have) || !ctx.Read(Reward) || !ctx.Read(Cash))
			return false;
		return ctx.Read(Xp) && ctx.Read(Ico) && ctx.Read(Title) && ctx.Read(Hint) && ctx.Read(Stage);
	}
}

// Шапка клана и состав: то, что нужно почти всем экранам.
class VB_Clan
{
	string Id;
	string Title;
	string Tag;
	string Slogan;
	string EmbShape;
	string EmbSymbol;
	int Color;
	int Level = 1;
	int Influence;
	int WeekInfluence;
	int Bank;
	int TaxFree;
	int LastTaxDay;
	int ChatTotal;	// всего сообщений и новостей: по ним клиент считает непрочитанное
	int NewsTotal;
	bool Open = true;
	string Discord;
	int ContractsDay;
	ref array<ref VB_ClanContract> Contracts = new array<ref VB_ClanContract>();
	ref array<ref VB_ClanRank> Ranks = new array<ref VB_ClanRank>();
	ref array<ref VB_ClanMember> Members = new array<ref VB_ClanMember>();
	ref array<ref VB_ClanRequest> Requests = new array<ref VB_ClanRequest>();
	ref array<int> Week = {0, 0, 0, 0, 0, 0, 0};
	// Только на сервере и в отдельных пакетах.
	ref array<ref VB_ClanJournalRow> Journal = new array<ref VB_ClanJournalRow>();
	ref array<ref VB_ClanNews> News = new array<ref VB_ClanNews>();
	ref array<ref VB_ClanChatRow> Chat = new array<ref VB_ClanChatRow>();
	ref array<ref VB_ClanBankRow> History = new array<ref VB_ClanBankRow>();
	ref array<ref VB_MapObject> MapObjects = new array<ref VB_MapObject>();
	ref array<ref VB_MapCat> MapCats = new array<ref VB_MapCat>();
	int MapNext = 1;
	int MapNextCat = 2;
	int WeekDone;		// только на сервере: номер недели, за которую клан уже получил итоги (приз выдаётся один раз)
	[NonSerialized()]
	bool Dirty;			// только на сервере: изменения ждут отложенной записи файла

	void WriteHead(ScriptRPC rpc)
	{
		rpc.Write(ChatTotal);
		rpc.Write(NewsTotal);
		rpc.Write(Id);
		rpc.Write(Title);
		rpc.Write(Tag);
		rpc.Write(Slogan);
		rpc.Write(EmbShape);
		rpc.Write(EmbSymbol);
		rpc.Write(Color);
		rpc.Write(Level);
		rpc.Write(Influence);
		rpc.Write(WeekInfluence);
		rpc.Write(Bank);
		rpc.Write(TaxFree);
		rpc.Write(Open);
		rpc.Write(Discord);
		int n = Ranks.Count();
		rpc.Write(n);
		foreach (VB_ClanRank r : Ranks)
			r.Write(rpc);
		n = Members.Count();
		rpc.Write(n);
		foreach (VB_ClanMember m : Members)
			m.Write(rpc);
		n = Requests.Count();
		rpc.Write(n);
		foreach (VB_ClanRequest q : Requests)
			q.Write(rpc);
		for (int i = 0; i < 7; i++)
		{
			int w = Week.Get(i);
			rpc.Write(w);
		}
	}

	bool ReadHead(ParamsReadContext ctx)
	{
		if (!ctx.Read(ChatTotal) || !ctx.Read(NewsTotal))
			return false;
		if (!ctx.Read(Id) || !ctx.Read(Title) || !ctx.Read(Tag) || !ctx.Read(Slogan) || !ctx.Read(EmbShape) || !ctx.Read(EmbSymbol))
			return false;
		if (!ctx.Read(Color) || !ctx.Read(Level) || !ctx.Read(Influence) || !ctx.Read(WeekInfluence) || !ctx.Read(Bank) || !ctx.Read(TaxFree))
			return false;
		if (!ctx.Read(Open) || !ctx.Read(Discord))
			return false;
		int n;
		int i;
		if (!ctx.Read(n))
			return false;
		Ranks.Clear();
		for (i = 0; i < n; i++)
		{
			VB_ClanRank r = new VB_ClanRank();
			if (!r.Read(ctx))
				return false;
			Ranks.Insert(r);
		}
		if (!ctx.Read(n))
			return false;
		Members.Clear();
		for (i = 0; i < n; i++)
		{
			VB_ClanMember m = new VB_ClanMember();
			if (!m.Read(ctx))
				return false;
			Members.Insert(m);
		}
		if (!ctx.Read(n))
			return false;
		Requests.Clear();
		for (i = 0; i < n; i++)
		{
			VB_ClanRequest q = new VB_ClanRequest();
			if (!q.Read(ctx))
				return false;
			Requests.Insert(q);
		}
		Week.Clear();
		for (i = 0; i < 7; i++)
		{
			int w;
			if (!ctx.Read(w))
				return false;
			Week.Insert(w);
		}
		return true;
	}

	VB_ClanMember Member(string uid)
	{
		foreach (VB_ClanMember m : Members)
		{
			if (m.Uid == uid)
				return m;
		}
		return null;
	}

	VB_ClanRank Rank(string id)
	{
		foreach (VB_ClanRank r : Ranks)
		{
			if (r.Id == id)
				return r;
		}
		return Ranks.Get(Ranks.Count() - 1);
	}

	int RankIndex(string id)
	{
		for (int i = 0; i < Ranks.Count(); i++)
		{
			if (Ranks.Get(i).Id == id)
				return i;
		}
		return Ranks.Count() - 1;
	}

	bool MemberCan(string uid, int perm)
	{
		VB_ClanMember m = Member(uid);
		if (!m)
			return false;
		return Rank(m.RankId).Can(perm);
	}

	bool IsLeader(string uid)
	{
		VB_ClanMember m = Member(uid);
		return m && m.RankId == "leader";
	}

	// Старше ли игрок a игрока b.
	bool Outranks(string a, string b)
	{
		VB_ClanMember ma = Member(a);
		VB_ClanMember mb = Member(b);
		if (!ma || !mb)
			return false;
		return RankIndex(ma.RankId) < RankIndex(mb.RankId);
	}

	string LeaderName()
	{
		foreach (VB_ClanMember m : Members)
		{
			if (m.RankId == "leader")
				return m.PlayerName;
		}
		return "";
	}

	int NextLevelNeed()
	{
		array<int> levels = VB_TabletRates.Levels();
		if (Level >= levels.Count())
			return levels.Get(levels.Count() - 1);
		return levels.Get(Level);
	}
}

// Строка списка кланов: рейтинг и открытая карточка клана.
class VB_ClanRow
{
	string Id;
	string Title;
	string Tag;
	string Slogan;
	string EmbShape;
	string EmbSymbol;
	int Color;
	int Level;
	int MemberCount;
	int WeekInfluence;
	int Influence;
	bool Open;
	string Leader;
	string Discord;
	int Bank;	// только для администратора, остальным 0
	int TaxFree;

	void Write(ScriptRPC rpc)
	{
		rpc.Write(Id);
		rpc.Write(Title);
		rpc.Write(Tag);
		rpc.Write(Slogan);
		rpc.Write(EmbShape);
		rpc.Write(EmbSymbol);
		rpc.Write(Color);
		rpc.Write(Level);
		rpc.Write(MemberCount);
		rpc.Write(WeekInfluence);
		rpc.Write(Influence);
		rpc.Write(Open);
		rpc.Write(Leader);
		rpc.Write(Discord);
		rpc.Write(Bank);
		rpc.Write(TaxFree);
	}

	bool Read(ParamsReadContext ctx)
	{
		if (!ctx.Read(Id) || !ctx.Read(Title) || !ctx.Read(Tag) || !ctx.Read(Slogan) || !ctx.Read(EmbShape) || !ctx.Read(EmbSymbol) || !ctx.Read(Color))
			return false;
		if (!ctx.Read(Level) || !ctx.Read(MemberCount) || !ctx.Read(WeekInfluence) || !ctx.Read(Influence) || !ctx.Read(Open))
			return false;
		return ctx.Read(Leader) && ctx.Read(Discord) && ctx.Read(Bank) && ctx.Read(TaxFree);
	}
}

// Приглашение, которое висит у игрока.
class VB_ClanInvite
{
	string ClanId;
	string Title;
	string Tag;
	string Slogan;
	string EmbShape;
	string EmbSymbol;
	int Color;
	int Level;
	int MemberCount;
	string By;
	float ExpiresAt;

	void Write(ScriptRPC rpc)
	{
		rpc.Write(ClanId);
		rpc.Write(Title);
		rpc.Write(Tag);
		rpc.Write(Slogan);
		rpc.Write(EmbShape);
		rpc.Write(EmbSymbol);
		rpc.Write(Color);
		rpc.Write(Level);
		rpc.Write(MemberCount);
		rpc.Write(By);
	}

	bool Read(ParamsReadContext ctx)
	{
		if (!ctx.Read(ClanId) || !ctx.Read(Title) || !ctx.Read(Tag) || !ctx.Read(Slogan) || !ctx.Read(EmbShape) || !ctx.Read(EmbSymbol))
			return false;
		return ctx.Read(Color) && ctx.Read(Level) && ctx.Read(MemberCount) && ctx.Read(By);
	}
}
