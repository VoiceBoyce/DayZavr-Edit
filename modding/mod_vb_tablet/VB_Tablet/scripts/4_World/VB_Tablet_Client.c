// Клиентская сессия: запросы к серверу и последнее полученное состояние.
class VB_TabletClient
{
	static const float HELLO_GAP_SEC = 2.0;
	static const float HELLO_TIMEOUT_SEC = 5.0;
	static const int MAX_ROWS = 1000;	// предел строк в пакете: больше сервер не шлёт, мусор отбрасывается

	protected static ref VB_TabletClient s_Instance;

	string ServerName;
	bool IsAdmin;
	bool HasHello;
	bool Hide3D;
	ref VB_MapState Map = new VB_MapState();	// карта и 3D метки, своя версия: Map.Version
	int DataVersion;	// растёт с каждым ответом сервера: окно перерисовывает экран
	ref VB_TabletRates Rates = new VB_TabletRates();
	ref VB_TabletStats Me = new VB_TabletStats();
	bool HasProfile;
	float ProfileAt;
	bool HasRating;
	int RatingTotal;
	ref array<string> RatingNames = new array<string>();
	ref array<int> RatingXp = new array<int>();
	ref array<string> RatingTags = new array<string>();
	ref array<int> RatingRanks = new array<int>();
	ref VB_Clan Clan;	// null: игрок без клана
	string AppliedClan;
	ref array<ref VB_ClanInvite> Invites = new array<ref VB_ClanInvite>();
	ref array<ref VB_ClanRow> Clans = new array<ref VB_ClanRow>();
	ref map<string, ref VB_TabletStats> Cards = new map<string, ref VB_TabletStats>();
	protected ref map<string, float> m_CardAt = new map<string, float>();
	ref array<ref VB_ClanJournalRow> Journal = new array<ref VB_ClanJournalRow>();
	ref array<ref VB_ClanNews> News = new array<ref VB_ClanNews>();
	ref array<ref VB_ClanChatRow> Chat = new array<ref VB_ClanChatRow>();
	ref array<ref VB_ClanBankRow> History = new array<ref VB_ClanBankRow>();
	ref VB_NameTags Tags = new VB_NameTags();	// ники над головами
	ref ScriptInvoker OnFeed = new ScriptInvoker();	// (int вид, VB_ClanJournalRow): лента клана для общего чата
	// Цены настроек клана: конфиг сервера, приходят в HELLO.
	int ClanPrice = 1500000;
	int ClanRank = 2;
	int ClanMax = 20;
	int MaxClans = 36;
	int BankMax = 100000000;
	ref array<int> TaxByLevel = {20000, 25000, 30000, 35000, 40000};	// налог в день по уровню клана 1..5
	ref array<int> WeekPrizes = {500000, 300000, 150000};	// награды недели за 1, 2, 3 место
	// Клан-чат: одна пауза на окно планшета и вкладку «Клан» общего чата. Сервер ждёт 2 с,
	// у клиента запас на задержку сети, иначе сообщение изредка получало бы отказ «слишком часто».
	static const float CHAT_GAP = 2.3;
	float ChatSentAt = -100;
	ref array<int> RewardMy = {10, 25, 60};			// влияние за личный контракт по сложности
	int PriceName = 150000;
	int PriceTag = 100000;
	int PriceEmblem = 100000;
	int PriceColor = 50000;
	string PickMember;	// uid участника: кого показать в карточке при переходе на экран «Участники»
	bool HasContracts;
	bool MyReplace;		// бесплатная замена ещё не истрачена
	int ContractLeft;
	int MyContractLeft;
	float ContractAt;
	ref array<ref VB_ClanContract> Contracts = new array<ref VB_ClanContract>();
	ref array<ref VB_ClanContract> MyContracts = new array<ref VB_ClanContract>();
	ref array<string> Banned = new array<string>();
	string AdminClan;	// чей состав лежит в AdminUids
	ref array<string> AdminUids = new array<string>();
	ref array<string> AdminNames = new array<string>();
	ref array<bool> AdminLeaders = new array<bool>();
	protected float m_HelloAt = -100;
	protected float m_HelloGap = HELLO_GAP_SEC;
	protected float m_FirstHelloAt = -1;

	static VB_TabletClient Get()
	{
		if (!s_Instance)
			s_Instance = new VB_TabletClient();
		return s_Instance;
	}

	protected static bool s_AimLocked;

	static bool IsAimLocked()
	{
		return s_AimLocked;
	}

	// Планшет открыт: персонаж ходит, камера стоит. Блокировка ванильного меню поворот мышью не держит,
	// поэтому отключаются сами команды поворота, а PlayerBase ещё и обнуляет поворот каждый кадр.
	static void SetAimLock(bool on)
	{
		if (s_AimLocked == on)
			return;
		s_AimLocked = on;
		array<string> names = {"UAAimLeft", "UAAimRight", "UAAimUp", "UAAimDown"};
		foreach (string inputName : names)
		{
			UAInput aim = GetUApi().GetInputByName(inputName);
			if (aim)
				aim.ForceDisable(on);
		}
		// Обнуление держится до DISABLED. Снимать обязательно, иначе камера останется стоять.
		if (on)
			return;
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
			return;
		HumanInputController hic = player.GetInputController();
		if (hic)
		{
			hic.OverrideAimChangeX(HumanInputControllerOverrideType.DISABLED, 0);
			hic.OverrideAimChangeY(HumanInputControllerOverrideType.DISABLED, 0);
		}
	}

	protected static ref array<int> s_MouseOff;	// команды мыши, отключённые на время планшета (id)

	// Планшет открыт: команды на кнопках и колесе мыши отключаются, клики идут только в планшет.
	// Без этого за рулём ЛКМ («действие по умолчанию») глушила двигатель (баг 2026-11-11). Клавиатура не трогается:
	// персонаж ходит, транспортом можно управлять. Интерфейсные команды UAUI* остаются.
	static void SetMouseLock(bool on)
	{
		if (on == (s_MouseOff != null))
			return;
		UAInputAPI api = GetUApi();
		if (!api)
			return;
		if (!on)
		{
			foreach (int offId : s_MouseOff)
			{
				UAInput back = api.GetInputByID(offId);
				if (back)
					back.ForceDisable(false);
			}
			s_MouseOff = null;
			// Поворот мышью тоже был среди команд мыши: если планшет ещё открыт, его блокировка возвращается.
			if (s_AimLocked)
			{
				s_AimLocked = false;
				SetAimLock(true);
			}
			return;
		}
		s_MouseOff = new array<int>();
		array<int> keep = new array<int>();
		// Свои клавиши планшета тоже: их могли переназначить на кнопку мыши, планшет должен закрываться ей же.
		array<string> ui = {"UAUISelect", "UAUIBack", "UAUIMenu", "UAUIGesturesOpen", "UAUIQuickbarRadialOpen", "UAVBTabletOpen", "UAVBTabletMap", "UAVBTablet3D"};
		foreach (string uiName : ui)
		{
			UAInput uiInput = api.GetInputByName(uiName);
			if (uiInput)
				keep.Insert(uiInput.ID());
		}
		TIntArray ids = new TIntArray();
		api.GetActiveInputs(ids);
		foreach (int id : ids)
		{
			if (keep.Find(id) >= 0)
				continue;
			UAInput input = api.GetInputByID(id);
			if (!input)
				continue;
			int was = input.AlternativeIndex();
			bool mouse = false;
			for (int a = 0; a < input.AlternativeCount() && !mouse; a++)
			{
				input.SelectAlternative(a);
				for (int k = 0; k < input.BindKeyCount(); k++)
				{
					if (input.CheckBindDevice(k, EUAINPUT_DEVICE_MOUSE))
					{
						mouse = true;
						break;
					}
				}
			}
			input.SelectAlternative(was);
			if (!mouse)
				continue;
			input.ForceDisable(true);
			s_MouseOff.Insert(id);
		}
#ifdef DIAG_DEVELOPER
		UAInput lmb = api.GetInputByName("UADefaultAction");
		bool lmbOff = lmb && s_MouseOff.Find(lmb.ID()) >= 0;
		Print(VB_TABLET_TAG + " команды мыши отключены: " + s_MouseOff.Count().ToString() + ", UADefaultAction среди них: " + lmbOff.ToString());
#endif
	}

	static void Reset()
	{
		SetMouseLock(false);
		SetAimLock(false);
		s_Instance = null;
		VB_C_ACCENT = VB_ACCENT_DEFAULT;
	}

	void RequestHello()
	{
		if (HasHello || !GetGame() || GetGame().IsDedicatedServer())
			return;
		float now = GetGame().GetTickTime();
		if (now - m_HelloAt < m_HelloGap)
			return;
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
			return;
		if (m_FirstHelloAt < 0)
			m_FirstHelloAt = now;
		m_HelloAt = now;
		// Без ответа (нет VB_Tablet_SE или другая версия) пауза растёт до 30 с, сервер не засыпается запросами.
		m_HelloGap = m_HelloGap * 2;
		if (m_HelloGap > 30)
			m_HelloGap = 30;
		int op = VB_TabletOp.HELLO;
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(op);
		rpc.Send(player, VB_TABLET_RPC_C2S, true, null);
	}

	string MyUid()
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player && player.GetIdentity())
			return player.GetIdentity().GetPlainId();
		return "";
	}

	// Статистика другого игрока: соклановец или строка рейтинга. Ответ придёт через DataVersion.
	VB_TabletStats CardOf(string name)
	{
		VB_TabletStats s;
		Cards.Find(name, s);
		return s;
	}

	void RequestCard(string uid, string name)
	{
		float now = GetGame().GetTickTime();
		float at;
		if (m_CardAt.Find(name, at) && now - at < 10)
			return;
		m_CardAt.Set(name, now);
		ScriptRPC rpc = Begin(VB_TabletOp.PLAYER_CARD);
		rpc.Write(uid);
		rpc.Write(name);
		Post(rpc);
	}

	// Налог в день для уровня клана.
	int TaxOf(int level)
	{
		if (!TaxByLevel || TaxByLevel.Count() == 0)
			return 0;
		return TaxByLevel.Get(Math.Clamp(level, 1, TaxByLevel.Count()) - 1);
	}

	// Налог своего клана в день (0 без клана).
	int MyTax()
	{
		if (!Clan)
			return 0;
		return TaxOf(Clan.Level);
	}

	// На сколько дней склада хватит налога. Налог выключен (0 в конфиге): 999, деления на ноль нет.
	int TaxDays(int bank, int level)
	{
		int tax = TaxOf(level);
		if (tax <= 0)
			return 999;
		return bank / tax;
	}

	bool Can(int perm)
	{
		return Clan && Clan.MemberCan(MyUid(), perm);
	}

	// Новый запрос к серверу: op уже записан, вызывающий дописывает поля и вызывает Post.
	ScriptRPC Begin(int op)
	{
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(op);
		return rpc;
	}

	void Post(ScriptRPC rpc)
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player)
			rpc.Send(player, VB_TABLET_RPC_C2S, true, null);
	}

	void Request(int op)
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
			return;
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(op);
		rpc.Send(player, VB_TABLET_RPC_C2S, true, null);
	}

	// Текущая жизнь идёт и между ответами сервера.
	int LifeNow()
	{
		if (!HasProfile || !GetGame())
			return 0;
		return Me.LifeSec + (GetGame().GetTickTime() - ProfileAt);
	}

	// Без ответа сервера: VB_Tablet_SE не стоит или не загрузился.
	bool IsServerMissing()
	{
		if (HasHello || !GetGame() || m_FirstHelloAt < 0)
			return false;
		return GetGame().GetTickTime() - m_FirstHelloAt > HELLO_TIMEOUT_SEC;
	}

	void OnServerMessage(ParamsReadContext ctx)
	{
		int op;
		if (!ctx.Read(op))
			return;
		if (op == VB_TabletOp.HELLO)
		{
			string name;
			bool admin;
			if (!ctx.Read(name) || !ctx.Read(admin) || !Rates.Read(ctx))
				return;
			if (!ctx.Read(PriceName) || !ctx.Read(PriceTag) || !ctx.Read(PriceEmblem) || !ctx.Read(PriceColor))
				return;
			if (!ctx.Read(ClanPrice) || !ctx.Read(ClanRank) || !ctx.Read(ClanMax) || !ctx.Read(MaxClans) || !ctx.Read(BankMax))
				return;
			array<int> taxes = new array<int>();
			if (!ctx.Read(taxes))
				return;
			if (taxes.Count() == VB_CLAN_LEVELS)
				TaxByLevel = taxes;
			array<int> prizes = new array<int>();
			array<int> rewardMy = new array<int>();
			if (!ctx.Read(prizes) || !ctx.Read(rewardMy))
				return;
			WeekPrizes = prizes;
			RewardMy = rewardMy;
			ServerName = name;
			IsAdmin = admin;
			HasHello = true;
			DataVersion++;
		}
		else if (op == VB_TabletOp.PROFILE)
		{
			if (!Me.Read(ctx))
				return;
			HasProfile = true;
			ProfileAt = GetGame().GetTickTime();
			DataVersion++;
		}
		else if (op == VB_TabletOp.RATING)
		{
			ReadRating(ctx);
		}
		else if (op == VB_TabletOp.RESULT)
		{
			int code;
			if (!ctx.Read(code))
				return;
			LastResult = code;
			ResultVersion++;
		}
		else if (op == VB_TabletOp.NOTIFY)
		{
			ReadNotify(ctx);
		}
		else if (op == VB_TabletOp.NAMETAGS)
		{
			Tags.Read(ctx);
		}
		else if (op == VB_TabletOp.CLAN_FEED)
		{
			ReadFeed(ctx);
		}
		else if (VB_MapState.IsMapOp(op))
		{
			Map.OnServer(op, ctx);
		}
		else
		{
			ReadClanPart(op, ctx);
		}
	}

	protected void ReadFeed(ParamsReadContext ctx)
	{
		int kind;
		VB_ClanJournalRow row = new VB_ClanJournalRow();
		if (!ctx.Read(kind) || !row.Read(ctx))
			return;
		OnFeed.Invoke(kind, row);
	}

	int LastResult;
	int ResultVersion;

	// Сменился клан: клановые объекты карты другие или их больше нет.
	protected void NotifyMapClan()
	{
		string clanId = "";
		if (Clan)
			clanId = Clan.Id;
		if (Map.ClanChanged(clanId) && Map.Loaded)
			Request(VB_TabletOp.MAP_GET);
	}

	// Клан сменился или пропал: списки прежнего клана (журнал, новости, чат, история склада, контракты)
	// больше не показываются, пока сервер не пришлёт новые.
	protected void SetClan(VB_Clan c)
	{
		string was = "";
		if (Clan)
			was = Clan.Id;
		string now = "";
		if (c)
			now = c.Id;
		Clan = c;
		if (c)
			VB_C_ACCENT = c.Color;
		else
			VB_C_ACCENT = VB_ACCENT_DEFAULT;
		if (was == now)
			return;
		Journal = new array<ref VB_ClanJournalRow>();
		News = new array<ref VB_ClanNews>();
		Chat = new array<ref VB_ClanChatRow>();
		History = new array<ref VB_ClanBankRow>();
		Contracts = new array<ref VB_ClanContract>();
		MyContracts = new array<ref VB_ClanContract>();
		HasContracts = false;
	}

	// Списки читаются во временный массив и подменяют прежний только целиком: обрыв пакета
	// не оставляет экран с половиной списка.
	protected void ReadClanPart(int op, ParamsReadContext ctx)
	{
		int n;
		int i;
		int total;
		switch (op)
		{
			case VB_TabletOp.CLAN:
				bool has;
				if (!ctx.Read(has))
					return;
				VB_Clan head = null;
				if (has)
				{
					head = new VB_Clan();
					if (!head.ReadHead(ctx))
						return;
				}
				if (!ctx.Read(n) || n < 0 || n > MAX_ROWS)
					return;
				array<ref VB_ClanInvite> invites = new array<ref VB_ClanInvite>();
				for (i = 0; i < n; i++)
				{
					VB_ClanInvite inv = new VB_ClanInvite();
					if (!inv.Read(ctx))
						return;
					invites.Insert(inv);
				}
				string applied;
				if (!ctx.Read(applied))
					return;
				SetClan(head);
				Invites = invites;
				AppliedClan = applied;
				NotifyMapClan();
				break;
			case VB_TabletOp.PLAYER_CARD:
				string cardName;
				VB_TabletStats card = new VB_TabletStats();
				if (!ctx.Read(cardName) || !card.Read(ctx))
					return;
				// Карточки игроков: не больше сотни в памяти, старые вытесняются целиком.
				if (Cards.Count() > 100 && !Cards.Contains(cardName))
				{
					Cards.Clear();
					m_CardAt.Clear();
				}
				Cards.Set(cardName, card);
				break;
			case VB_TabletOp.CLAN_LIST:
				if (!ctx.Read(n) || n < 0 || n > MAX_ROWS)
					return;
				array<ref VB_ClanRow> clans = new array<ref VB_ClanRow>();
				for (i = 0; i < n; i++)
				{
					VB_ClanRow row = new VB_ClanRow();
					if (!row.Read(ctx))
						return;
					clans.Insert(row);
				}
				Clans = clans;
				break;
			case VB_TabletOp.CLAN_JOURNAL:
				if (!ctx.Read(n) || n < 0 || n > MAX_ROWS)
					return;
				array<ref VB_ClanJournalRow> journal = new array<ref VB_ClanJournalRow>();
				for (i = 0; i < n; i++)
				{
					VB_ClanJournalRow jr = new VB_ClanJournalRow();
					if (!jr.Read(ctx))
						return;
					journal.Insert(jr);
				}
				Journal = journal;
				break;
			case VB_TabletOp.CLAN_NEWS:
				if (!ctx.Read(total) || !ctx.Read(n) || n < 0 || n > MAX_ROWS)
					return;
				array<ref VB_ClanNews> news = new array<ref VB_ClanNews>();
				for (i = 0; i < n; i++)
				{
					VB_ClanNews nw = new VB_ClanNews();
					if (!nw.Read(ctx))
						return;
					news.Insert(nw);
				}
				News = news;
				// Счётчик непрочитанного приходит со списком: шапка клана после новости не рассылается.
				if (Clan)
					Clan.NewsTotal = total;
				break;
			case VB_TabletOp.CLAN_CHAT:
				if (!ctx.Read(total) || !ctx.Read(n) || n < 0 || n > MAX_ROWS)
					return;
				array<ref VB_ClanChatRow> chat = new array<ref VB_ClanChatRow>();
				for (i = 0; i < n; i++)
				{
					VB_ClanChatRow cr = new VB_ClanChatRow();
					if (!cr.Read(ctx))
						return;
					chat.Insert(cr);
				}
				Chat = chat;
				if (Clan)
					Clan.ChatTotal = total;
				break;
			case VB_TabletOp.CLAN_BANK:
				if (!ctx.Read(n) || n < 0 || n > MAX_ROWS)
					return;
				array<ref VB_ClanBankRow> history = new array<ref VB_ClanBankRow>();
				for (i = 0; i < n; i++)
				{
					VB_ClanBankRow br = new VB_ClanBankRow();
					if (!br.Read(ctx))
						return;
					history.Insert(br);
				}
				History = history;
				break;
			default:
				if (!ReadBankPart(op, ctx))
					return;
				break;
		}
		DataVersion++;
	}

	// Секунды до нового дня: по часам сервера, если они уже пришли.
	int NextDayLeft()
	{
		if (HasContracts && GetGame())
		{
			int left = ContractLeft - (GetGame().GetTickTime() - ContractAt);
			if (left > 0)
				return left;
		}
		int h;
		int m;
		int s;
		GetHourMinuteSecond(h, m, s);
		return 86400 - (h * 3600 + m * 60 + s);
	}

	// Секунды до нового набора личных контрактов.
	int NextMyLeft()
	{
		if (!HasContracts || !GetGame())
			return 0;
		int left = MyContractLeft - (GetGame().GetTickTime() - ContractAt);
		if (left < 0)
			left = 0;
		return left;
	}

	// Сообщение поверх игры: работает и с закрытым планшетом.
	protected void ReadNotify(ParamsReadContext ctx)
	{
		string key;
		string arg;
		int kind;
		if (!ctx.Read(key) || !ctx.Read(arg) || !ctx.Read(kind))
			return;
		string text = Widget.TranslateString(key);
		if (arg.IndexOf("#") == 0)
			arg = Widget.TranslateString(arg);
		text.Replace("{arg}", arg);
		if (kind == 1)
			INT_UIF_Toast.Success(text);
		else if (kind == 2)
			INT_UIF_Toast.Error(text);
		else
			INT_UIF_Toast.Info(text);
	}

	protected bool ReadContractList(ParamsReadContext ctx, array<ref VB_ClanContract> list)
	{
		int n;
		if (!ctx.Read(n) || n < 0 || n > MAX_ROWS)
			return false;
		for (int i = 0; i < n; i++)
		{
			VB_ClanContract k = new VB_ClanContract();
			if (!k.Read(ctx))
				return false;
			list.Insert(k);
		}
		return true;
	}

	protected bool ReadBankPart(int op, ParamsReadContext ctx)
	{
		int n;
		int i;
		if (op == VB_TabletOp.CONTRACTS)
		{
			int left;
			int myLeft;
			bool replace;
			if (!ctx.Read(left) || !ctx.Read(myLeft) || !ctx.Read(replace))
				return false;
			array<ref VB_ClanContract> clanList = new array<ref VB_ClanContract>();
			array<ref VB_ClanContract> myList = new array<ref VB_ClanContract>();
			if (!ReadContractList(ctx, clanList) || !ReadContractList(ctx, myList))
				return false;
			ContractLeft = left;
			MyContractLeft = myLeft;
			MyReplace = replace;
			ContractAt = GetGame().GetTickTime();
			Contracts = clanList;
			MyContracts = myList;
			HasContracts = true;
			return true;
		}
		if (op == VB_TabletOp.ADMIN_BANNED)
		{
			if (!ctx.Read(n) || n < 0 || n > MAX_ROWS)
				return false;
			array<string> words = new array<string>();
			for (i = 0; i < n; i++)
			{
				string word;
				if (!ctx.Read(word))
					return false;
				words.Insert(word);
			}
			Banned = words;
			return true;
		}
		if (op == VB_TabletOp.ADMIN_MEMBERS)
		{
			string clanId;
			if (!ctx.Read(clanId) || !ctx.Read(n) || n < 0 || n > MAX_ROWS)
				return false;
			array<string> uids = new array<string>();
			array<string> names = new array<string>();
			array<bool> leaders = new array<bool>();
			for (i = 0; i < n; i++)
			{
				string uid;
				string name;
				bool leader;
				if (!ctx.Read(uid) || !ctx.Read(name) || !ctx.Read(leader))
					return false;
				uids.Insert(uid);
				names.Insert(name);
				leaders.Insert(leader);
			}
			AdminClan = clanId;
			AdminUids = uids;
			AdminNames = names;
			AdminLeaders = leaders;
			return true;
		}
		return false;
	}

	protected void ReadRating(ParamsReadContext ctx)
	{
		int total;
		int count;
		if (!ctx.Read(total) || !ctx.Read(count) || count < 0 || count > MAX_ROWS)
			return;
		array<string> names = new array<string>();
		array<int> xps = new array<int>();
		array<string> tags = new array<string>();
		array<int> ranks = new array<int>();
		for (int i = 0; i < count; i++)
		{
			string name;
			int xp;
			string tag;
			if (!ctx.Read(name) || !ctx.Read(xp) || !ctx.Read(tag))
				return;
			names.Insert(name);
			xps.Insert(xp);
			tags.Insert(tag);
		}
		for (int r = 0; r < VB_RANK_COUNT; r++)
		{
			int n;
			if (!ctx.Read(n))
				return;
			ranks.Insert(n);
		}
		RatingNames = names;
		RatingXp = xps;
		RatingTags = tags;
		RatingRanks = ranks;
		RatingTotal = total;
		HasRating = true;
		DataVersion++;
	}
}

modded class PlayerBase
{
	override void CommandHandler(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		if (VB_TabletClient.IsAimLocked() && PlayerBase.Cast(GetGame().GetPlayer()) == this)
		{
			HumanInputController aimInput = GetInputController();
			if (aimInput)
			{
				aimInput.OverrideAimChangeX(HumanInputControllerOverrideType.ENABLED, 0);
				aimInput.OverrideAimChangeY(HumanInputControllerOverrideType.ENABLED, 0);
			}
		}
		super.CommandHandler(pDt, pCurrentCommandID, pCurrentCommandFinished);
	}

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		if (rpc_type == VB_TABLET_RPC_S2C)
		{
			if (!GetGame().IsDedicatedServer())
				VB_TabletClient.Get().OnServerMessage(ctx);
			return;
		}
		super.OnRPC(sender, rpc_type, ctx);
	}
}
