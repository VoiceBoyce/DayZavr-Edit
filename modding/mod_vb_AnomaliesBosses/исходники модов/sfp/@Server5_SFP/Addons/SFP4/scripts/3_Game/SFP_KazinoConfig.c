// SFP Kazino - JSON config (server authoritative)
// v2: добавлены ChanceMode ("fraction"|"percent"), WinChanceMultiplier и опциональные Combos

class SFP_KazinoWinEntry
{
	// Допустимые значения: "rifle","vodka","axe","bullet","can","rad"
	string Icon = "";

	// Выплата (0..99). Для отображения на модели используется 2 цифры (десятки/единицы).
	int Payout = 0;

	// Шанс данного выигрыша.
	// Интерпретация зависит от SFP_KazinoConfig.ChanceMode:
	// - "fraction": 0.0..1.0 (1.0 = 100%)
	// - "percent" : 0.0..100.0 (1.0 = 1%, 0.5 = 0.5%)
	float Chance = 0.0;
};

// Опциональная продвинутая таблица комбинаций.
// Позволяет задавать не только 3 одинаковых, но и смешанные комбинации.
class SFP_KazinoComboEntry
{
	// Ровно 3 иконки. Пример: ["rifle","rifle","vodka"]
	ref array<string> Icons;

	// Выплата (0..99)
	int Payout = 0;

	// Шанс (в единицах ChanceMode)
	float Chance = 0.0;

	// Если true — порядок иконок не важен (выплата за любую перестановку)
	// и при выигрыше порядок будет рандомизирован для красивой анимации.
	bool AnyOrder = true;

	void SFP_KazinoComboEntry()
	{
		Icons = new array<string>;
	}
};

class SFP_KazinoConfig
{
	// Какой предмет принимает автомат (класс)
	string AcceptItemClass = "SFP_Kazino_fishka100";

	// Какой предмет выдаёт автомат (класс)
	string PayoutItemClass = "SFP_Kazino_fishka100";

	// Как интерпретировать поле Chance в JSON:
	// - "fraction": 0..1
	// - "percent" : 0..100 (1 = 1%)
	string ChanceMode = "percent";

	// Глобальный множитель для ВСЕХ шансов (после конвертации в вероятность 0..1)
	// Пример: 0.5 = в 2 раза реже выигрыши.
	float WinChanceMultiplier = 1.2;//это крутить на общее выпадение выиграша. 1.5 часто. 1 плохо падает.

// ---- ГЛОБАЛЬНЫЙ шанс победы за один спин (в процентах) ----
// Если > 0, то именно эта величина задаёт общий шанс "выиграть что угодно" за одну попытку.
// Пример: 7.0 означает ~7% шанс на победу (в среднем ~2 победы на 30 попыток).
// Если 0 — используется старая схема: сумма Wins/Combos * WinChanceMultiplier.
float OverallWinChancePercent = 7.0;

// Комментарий, чтобы было видно в casino.json что делает поле OverallWinChancePercent
string _comment_OverallWinChancePercent = "ЭТА СТРОЧКА делает ГЛОБАЛЬНЫЙ шанс победы за один спин. Пример: 7 = 7% шанс выиграть что угодно. 0 = отключено (используется сумма Chances * WinChanceMultiplier).";


	// Список выигрышей (по умолчанию 6)
	ref array<ref SFP_KazinoWinEntry> Wins;

	// Опционально: таблица комбинаций (если задана и не пустая — будет использоваться вместо Wins)
	ref array<ref SFP_KazinoComboEntry> Combos;

	void SFP_KazinoConfig()
	{
		Wins = new array<ref SFP_KazinoWinEntry>;
		Combos = new array<ref SFP_KazinoComboEntry>;
		BuildDefaults();
	}

	void BuildDefaults()
	{
		Wins.Clear();
		Combos.Clear();

		// Дефолтные 6 выигрышей (сумма = общий шанс выигрыша)
		// По умолчанию ChanceMode = "percent", т.е. 1.0 = 1%.
		AddWin("rifle",  30, 1.0); // 1% - 3x Rifle
		AddWin("vodka",  25, 2.0); // 2% - 3x Vodka
		AddWin("axe",    20, 3.0); // 3% - 3x Axe
		AddWin("bullet", 15, 4.0); // 4% - 3x Bullet
		AddWin("can",    10, 5.0); // 5% - 3x Can
		AddWin("rad",     5, 6.0); // 6% - 3x Rad
	}

	protected void AddWin(string icon, int payout, float chance)
	{
		SFP_KazinoWinEntry e = new SFP_KazinoWinEntry;
		e.Icon = icon;
		e.Payout = payout;
		e.Chance = chance;
		Wins.Insert(e);
	}
};

class SFP_KazinoConfigManager
{
	protected static const string PROFILE_DIR  = "$profile:SFP";
	protected static const string PROFILE_FILE = PROFILE_DIR + "\\casino.json";
	protected static const string OLD_PROFILE_FILE = "$profile:SFP\\Kazino\\config.json";

	protected static ref SFP_KazinoConfig m_Config;

	static SFP_KazinoConfig Get()
	{
		if (!m_Config)
			LoadOrCreate();

		return m_Config;
	}

	static void LoadOrCreate()
	{
		MakeDirectory(PROFILE_DIR);

		m_Config = new SFP_KazinoConfig;

		string err;
		// Миграция со старого пути ($profile:SFP\Kazino\config.json) на новый ($profile:SFP\casino.json)
		if (!FileExist(PROFILE_FILE) && FileExist(OLD_PROFILE_FILE))
		{
			// Пытаемся перенести старую конфигурацию
			JsonFileLoader<SFP_KazinoConfig>.LoadFile(OLD_PROFILE_FILE, m_Config, err);
			JsonFileLoader<SFP_KazinoConfig>.SaveFile(PROFILE_FILE, m_Config, err);
		}

		if (FileExist(PROFILE_FILE))
		{
			if (!JsonFileLoader<SFP_KazinoConfig>.LoadFile(PROFILE_FILE, m_Config, err))
			{
				// если json повреждён — пересоздаём
				m_Config = new SFP_KazinoConfig;
				JsonFileLoader<SFP_KazinoConfig>.SaveFile(PROFILE_FILE, m_Config, err);
			}
		}
		else
		{
			JsonFileLoader<SFP_KazinoConfig>.SaveFile(PROFILE_FILE, m_Config, err);
		}

		ValidateAndFix();
	}

	protected static void ValidateAndFix()
	{
		if (!m_Config)
			return;

		// нормализуем режим шансов
		if (m_Config.ChanceMode == "")
			m_Config.ChanceMode = "percent";
		string cm = m_Config.ChanceMode;
		cm.ToLower();
		if (cm != "percent" && cm != "fraction")
			m_Config.ChanceMode = "percent";

		m_Config.WinChanceMultiplier = Math.Clamp(m_Config.WinChanceMultiplier, 0.0, 1000.0);
		m_Config.OverallWinChancePercent = Math.Clamp(m_Config.OverallWinChancePercent, 0.0, 100.0);
		if (m_Config._comment_OverallWinChancePercent == "")
			m_Config._comment_OverallWinChancePercent = "ЭТА СТРОЧКА делает ГЛОБАЛЬНЫЙ шанс победы за один спин. Пример: 7 = 7% шанс выиграть что угодно. 0 = отключено (используется сумма Chances * WinChanceMultiplier).";
		bool isPercent = false;
		string cm2 = m_Config.ChanceMode;
		cm2.ToLower();
		if (cm2 == "percent")
			isPercent = true;

		float maxChance = 1.0;
		if (isPercent)
			maxChance = 100.0;

		// базовая валидация
		if (!m_Config.Wins)
		{
			m_Config.Wins = new array<ref SFP_KazinoWinEntry>;
			m_Config.BuildDefaults();
		}

		if (m_Config.Wins.Count() == 0)
			m_Config.BuildDefaults();

		// правка полей и диапазонов
		foreach (SFP_KazinoWinEntry e : m_Config.Wins)
		{
			if (!e)
				continue;

			e.Chance = Math.Clamp(e.Chance, 0.0, maxChance);
			e.Payout = Math.Clamp(e.Payout, 0, 99);

			// лёгкая нормализация пустых значений
			if (e.Icon == "")
				e.Icon = "can";
		}

		// валидация комбинаций
		if (!m_Config.Combos)
			m_Config.Combos = new array<ref SFP_KazinoComboEntry>;

		foreach (SFP_KazinoComboEntry c : m_Config.Combos)
		{
			if (!c)
				continue;

			if (!c.Icons)
				c.Icons = new array<string>;

			c.Payout = Math.Clamp(c.Payout, 0, 99);
			c.Chance = Math.Clamp(c.Chance, 0.0, maxChance);

			// нормализуем размер массива до 3 (лишнее обрезаем, недостающее дополняем "can")
			while (c.Icons.Count() < 3)
				c.Icons.Insert("can");
			while (c.Icons.Count() > 3)
				c.Icons.RemoveOrdered(c.Icons.Count() - 1);
		}

		// сохраняем обратно (чтобы админ видел нормализованные значения)
		string err2;
		JsonFileLoader<SFP_KazinoConfig>.SaveFile(PROFILE_FILE, m_Config, err2);
	}
};


//--------------------------------------------------------------------------- Пояснение
/*
Если в casino.json поставить:

OverallWinChancePercent = 7.0

то это означает:

✅ каждый спин имеет 7% шанс “выиграть что угодно” (т.е. выпадение выигрышной комбинации/комбо)
и уже внутри этих 7% выбирается конкретный приз по твоим Chance (они становятся “весами”).
То есть на 30 попыток в среднем будет ~2.1 победы.

Если OverallWinChancePercent = 0 → работает старая схема (сумма Chances * WinChanceMultiplier).

*/