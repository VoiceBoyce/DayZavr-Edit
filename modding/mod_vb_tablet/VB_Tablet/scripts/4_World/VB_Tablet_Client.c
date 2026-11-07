// Клиентская сессия: запросы к серверу и последнее полученное состояние.
class VB_TabletClient
{
	static const float HELLO_GAP_SEC = 2.0;
	static const float HELLO_TIMEOUT_SEC = 5.0;

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
	// Цены настроек клана: конфиг сервера, приходят в HELLO.
	int ClanPrice = 1000000;
	int ClanRank = 2;
	int ClanMax = 20;
	int MaxClans = 20;
	int BankMax = 1000000;
	int ClanTax = 5000;
	int PriceName = 25000;
	int PriceTag = 15000;
	int PriceEmblem = 20000;
	int PriceColor = 5000;
	string PickMember;	// uid участника: кого показать в карточке при переходе на экран «Участники»
	bool AtmWanted;		// действие у банкомата: открыть окно при следующем кадре миссии
	bool HasContracts;
	bool MyReplace;		// бесплатная замена ещё не истрачена
	int ContractLeft;
	float ContractAt;
	ref array<ref VB_ClanContract> Contracts = new array<ref VB_ClanContract>();
	ref array<ref VB_ClanContract> MyContracts = new array<ref VB_ClanContract>();
	ref array<string> Banned = new array<string>();
	string AdminClan;	// чей состав лежит в AdminUids
	ref array<string> AdminUids = new array<string>();
	ref array<string> AdminNames = new array<string>();
	ref array<bool> AdminLeaders = new array<bool>();
	protected float m_HelloAt = -100;

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

	static void Reset()
	{
		SetAimLock(false);
		s_Instance = null;
		VB_C_ACCENT = VB_ACCENT_DEFAULT;
	}

	void RequestHello()
	{
		if (HasHello || !GetGame() || GetGame().IsDedicatedServer())
			return;
		float now = GetGame().GetTickTime();
		if (now - m_HelloAt < HELLO_GAP_SEC)
			return;
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
			return;
		m_HelloAt = now;
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
		if (HasHello || !GetGame() || m_HelloAt < 0)
			return false;
		return GetGame().GetTickTime() - m_HelloAt > HELLO_TIMEOUT_SEC;
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
			if (!ctx.Read(ClanPrice) || !ctx.Read(ClanRank) || !ctx.Read(ClanMax) || !ctx.Read(MaxClans) || !ctx.Read(BankMax) || !ctx.Read(ClanTax))
				return;
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
			if (ctx.Read(code))
				LastResult = code;
			ResultVersion++;
		}
		else if (op == VB_TabletOp.NOTIFY)
		{
			ReadNotify(ctx);
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

	protected void ReadClanPart(int op, ParamsReadContext ctx)
	{
		int n;
		int i;
		switch (op)
		{
			case VB_TabletOp.CLAN:
				bool has;
				if (!ctx.Read(has))
					return;
				if (has)
				{
					VB_Clan c = new VB_Clan();
					if (!c.ReadHead(ctx))
						return;
					Clan = c;
					VB_C_ACCENT = c.Color;
				}
				else
				{
					Clan = null;
					VB_C_ACCENT = VB_ACCENT_DEFAULT;
				}
				if (!ctx.Read(n))
					return;
				Invites.Clear();
				for (i = 0; i < n; i++)
				{
					VB_ClanInvite inv = new VB_ClanInvite();
					if (!inv.Read(ctx))
						return;
					Invites.Insert(inv);
				}
				if (!ctx.Read(AppliedClan))
					return;
				NotifyMapClan();
				break;
			case VB_TabletOp.PLAYER_CARD:
				string cardName;
				VB_TabletStats card = new VB_TabletStats();
				if (!ctx.Read(cardName) || !card.Read(ctx))
					return;
				Cards.Set(cardName, card);
				break;
			case VB_TabletOp.CLAN_LIST:
				if (!ctx.Read(n))
					return;
				Clans.Clear();
				for (i = 0; i < n; i++)
				{
					VB_ClanRow row = new VB_ClanRow();
					if (!row.Read(ctx))
						return;
					Clans.Insert(row);
				}
				break;
			case VB_TabletOp.CLAN_JOURNAL:
				if (!ctx.Read(n))
					return;
				Journal.Clear();
				for (i = 0; i < n; i++)
				{
					VB_ClanJournalRow jr = new VB_ClanJournalRow();
					if (!jr.Read(ctx))
						return;
					Journal.Insert(jr);
				}
				break;
			case VB_TabletOp.CLAN_NEWS:
				if (!ctx.Read(n))
					return;
				News.Clear();
				for (i = 0; i < n; i++)
				{
					VB_ClanNews nw = new VB_ClanNews();
					if (!nw.Read(ctx))
						return;
					News.Insert(nw);
				}
				break;
			case VB_TabletOp.CLAN_CHAT:
				if (!ctx.Read(n))
					return;
				Chat.Clear();
				for (i = 0; i < n; i++)
				{
					VB_ClanChatRow cr = new VB_ClanChatRow();
					if (!cr.Read(ctx))
						return;
					Chat.Insert(cr);
				}
				break;
			case VB_TabletOp.CLAN_BANK:
				if (!ctx.Read(n))
					return;
				History.Clear();
				for (i = 0; i < n; i++)
				{
					VB_ClanBankRow br = new VB_ClanBankRow();
					if (!br.Read(ctx))
						return;
					History.Insert(br);
				}
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
		if (!ctx.Read(n))
			return false;
		list.Clear();
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
			if (!ctx.Read(ContractLeft) || !ctx.Read(MyReplace))
				return false;
			ContractAt = GetGame().GetTickTime();
			if (!ReadContractList(ctx, Contracts) || !ReadContractList(ctx, MyContracts))
				return false;
			HasContracts = true;
			return true;
		}
		if (op == VB_TabletOp.ADMIN_BANNED)
		{
			if (!ctx.Read(n))
				return false;
			Banned.Clear();
			for (i = 0; i < n; i++)
			{
				string word;
				if (!ctx.Read(word))
					return false;
				Banned.Insert(word);
			}
			return true;
		}
		if (op == VB_TabletOp.ADMIN_MEMBERS)
		{
			if (!ctx.Read(AdminClan) || !ctx.Read(n))
				return false;
			AdminUids.Clear();
			AdminNames.Clear();
			AdminLeaders.Clear();
			for (i = 0; i < n; i++)
			{
				string uid;
				string name;
				bool leader;
				if (!ctx.Read(uid) || !ctx.Read(name) || !ctx.Read(leader))
					return false;
				AdminUids.Insert(uid);
				AdminNames.Insert(name);
				AdminLeaders.Insert(leader);
			}
			return true;
		}
		return false;
	}

	protected void ReadRating(ParamsReadContext ctx)
	{
		int total;
		int count;
		if (!ctx.Read(total) || !ctx.Read(count))
			return;
		RatingNames.Clear();
		RatingXp.Clear();
		RatingTags.Clear();
		RatingRanks.Clear();
		for (int i = 0; i < count; i++)
		{
			string name;
			int xp;
			string tag;
			if (!ctx.Read(name) || !ctx.Read(xp) || !ctx.Read(tag))
				return;
			RatingNames.Insert(name);
			RatingXp.Insert(xp);
			RatingTags.Insert(tag);
		}
		for (int r = 0; r < VB_RANK_COUNT; r++)
		{
			int n;
			if (!ctx.Read(n))
				return;
			RatingRanks.Insert(n);
		}
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
