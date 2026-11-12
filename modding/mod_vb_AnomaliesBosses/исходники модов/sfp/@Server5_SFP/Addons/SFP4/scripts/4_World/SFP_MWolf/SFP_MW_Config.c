static const int SFP_MW_RPC_PLAY_SUMMON_EFFECT = 990001;
static const int SFP_MW_RPC_PLAY_HIGHGROUND_EFFECT = 990002;
static const string SFP_MW_CONFIG_DIR = "$profile:SFP";
static const string SFP_MW_CONFIG_PATH = "$profile:SFP/SFP_MWolfConfig.json";

class SFP_MW_ConfigData
{
	string _comment_file = "Конфиг мутанта-оборотня SFP_MutaWolf_Base. Комментарии сохранены отдельными полями, чтобы файл оставался валидным JSON для DayZ JsonFileLoader.";
	
	string _comment_mutantHealth = "Настройка здоровья мутанта.";
	float MutantHealth = 5000.0;
	
	string _comment_wolfTypes = "Каких именно волков может призывать мутант. Выбор идёт случайно из этого массива.";
	ref array<string> WolfTypes;
	
	string _comment_wolvesPerSummon = "Максимум ЖИВЫХ призванных волков одновременно. Мутант добирает только недостающее количество до этого лимита и не спавнит выше него.";
	int WolvesPerSummon = 7;
	
	string _comment_summonCooldown = "Пауза между повторными волнами призыва в секундах. Новая волна будет только если мутант в агре и живых призванных волков стало меньше лимита.";
	float SummonCooldown = 25.0;
	
	string _comment_summonAggroDelay = "Сколько секунд после входа в агро мутант ждёт перед тем, как вообще получит право запускать призыв.";
	float SummonAggroDelay = 5.0;
	
	string _comment_summonDelay = "Задержка после старта логики призыва перед фазой ослепления. После ослепления ещё идёт короткая внутренняя пауза перед звуксетом и спавном волков.";
	float SummonDelay = 2.0;
	
	string _comment_deadWolfDespawnDelay = "Через сколько секунд исчезает труп УБИТОГО призванного волка.";
	float DeadWolfDespawnDelay = 60.0;
	
	string _comment_bloodDamagePerHit = "Сколько урона по крови получает игрок за каждый успешный удар мутанта.";
	float BloodDamagePerHit = 100.0;
	
	string _comment_bleedChance = "Шанс вызвать кровотечение при успешном ударе. 0.50 = 50%.";
	float BleedChance = 0.50;
	
	string _comment_inventoryDamageMultiplier = "Во сколько раз сильнее портится экипировка/инвентарь при ударе мутанта. Значение 2.0 = x2.";
	float InventoryDamageMultiplier = 2.0;
	
	string _comment_throwCooldown = "Частота использования импульсного сброса игрока с недосягаемой возвышенности, в секундах.";
	float ThrowCooldown = 10.0;
	
	string _comment_throwHorizontalVelocity = "Горизонтальная сила импульса, которая отбрасывает игрока в сторону от мутанта.";
	float ThrowHorizontalVelocity = 7.8;
	
	string _comment_throwVerticalVelocity = "Вертикальная составляющая импульса, чтобы игрока слегка подбрасывало и срывало с края.";
	float ThrowVerticalVelocity = 2.9;
	
	string _comment_throwForceMultiplier = "Общий множитель силы импульса. Увеличивает и горизонтальный толчок, и подброс вверх.";
	float ThrowForceMultiplier = 0.95;
	
	string _comment_flashRadius = "Радиус в метрах, в котором игроки получают ослепление, тряску и шанс уронить предмет при призыве волков.";
	float FlashRadius = 70.0;
	
	string _comment_flashIntensity = "Сила светового ослепления при призыве. 0.45-0.70 = заметное среднее ослепление без сплошного белого экрана.";
	float FlashIntensity = 0.62;
	
	string _comment_flashDurationMultiplier = "Продолжительность ослепления относительно обычной флешки. 1.0 = обычная, 0.7 = немного короче стандартной.";
	float FlashDurationMultiplier = 0.95;
	
	string _comment_flashSoundIntensity = "Громкость тиннитуса/приглушения звука от эффекта ослепления.";
	float FlashSoundIntensity = 0.55;
	
	string _comment_shockBlur = "Дополнительное размытие поверх флешки во время призыва волков.";
	float ShockBlur = 0.90;
	
	string _comment_shockVignette = "Сила виньетки во время призыва волков.";
	float ShockVignette = 0.42;
	
	string _comment_shockOverlay = "Сила тёмного оверлея во время призыва волков.";
	float ShockOverlay = 0.18;
	
	string _comment_shakeStrength = "Сила тряски камеры во время призыва волков.";
	float ShakeStrength = 2.20;
	
	string _comment_shakeDuration = "Продолжительность тряски камеры в секундах во время призыва волков.";
	float ShakeDuration = 3.8;
	
	string _comment_dropChance = "Шанс уронить предмет из рук у игроков в радиусе эффекта. 0.30 = 30%.";
	float DropChance = 0.30;
	
	string _comment_highgroundCheckRadius = "Радиус вокруг основной цели, в котором стоящие рядом игроки тоже получат импульсный сброс, если они на той же возвышенности.";
	float HighgroundCheckRadius = 15.0;
	
	string _comment_highgroundTriggerDistance = "Максимальная горизонтальная дистанция от мутанта до недосягаемой цели сверху, при которой разрешён импульсный сброс.";
	float HighgroundTriggerDistance = 7.5;
	
	string _comment_highgroundMinVerticalDiff = "Минимальная разница по высоте между игроком и мутантом, чтобы считать игрока на возвышенности.";
	float HighgroundMinVerticalDiff = 1.15;
	
	string _comment_highgroundBlur = "Сила лёгкого размытия при наказании игрока на возвышенности.";
	float HighgroundBlur = 0.40;
	
	string _comment_highgroundVignette = "Сила лёгкой виньетки при наказании игрока на возвышенности.";
	float HighgroundVignette = 0.12;
	
	string _comment_highgroundOverlay = "Сила лёгкого затемняющего оверлея при наказании игрока на возвышенности.";
	float HighgroundOverlay = 0.04;
	
	string _comment_highgroundShakeStrength = "Сила тряски камеры при наказании игрока на возвышенности.";
	float HighgroundShakeStrength = 1.10;
	
	string _comment_highgroundShakeDuration = "Продолжительность тряски камеры при наказании игрока на возвышенности.";
	float HighgroundShakeDuration = 1.40;
	
	string _comment_spawnMinDistance = "Минимальная дистанция спавна волков от целевого игрока. Волки появляются заметно дальше от цели.";
	float SpawnMinDistance = 24.0;
	
	string _comment_spawnMaxDistance = "Максимальная дистанция спавна волков от целевого игрока.";
	float SpawnMaxDistance = 40.0;
	
	string _comment_spawnBackAngleMin = "Минимальный угол спавна относительно направления взгляда игрока. Используется дуга за спиной, чтобы волки не появлялись на камеру.";
	float SpawnBackAngleMin = 110.0;
	
	string _comment_spawnBackAngleMax = "Максимальный угол спавна относительно направления взгляда игрока. Используется дуга за спиной, чтобы волки не появлялись на камеру.";
	float SpawnBackAngleMax = 250.0;
	
	string _comment_spawnSearchAttempts = "Сколько раз пытаться найти безопасную точку спавна, скрытую от камеры ближайших игроков.";
	int SpawnSearchAttempts = 56;
	
	string _comment_chaseMovementSpeed = "Поле оставлено только для совместимости со старым JSON. Сейчас кастомная мобильность отключена и мутант использует стандартное поведение ИИ DayZ.";
	float ChaseMovementSpeed = 3.0;
	
	string _comment_chaseTurnSpeed = "Поле оставлено только для совместимости со старым JSON. Сейчас кастомная мобильность отключена и мутант использует стандартное поведение ИИ DayZ.";
	float ChaseTurnSpeed = 36.0;
	
	string _comment_enableDebugLog = "Включить подробные сообщения в лог сервера. true/false.";
	bool EnableDebugLog = false;
	
	void SFP_MW_ConfigData()
	{
		WolfTypes = new array<string>;
		WolfTypes.Insert("Animal_CanisLupus_Grey");
		WolfTypes.Insert("Animal_CanisLupus_White");
	}
};

class SFP_MW_ConfigManager
{
	protected static ref SFP_MW_ConfigManager s_Instance;
	protected ref SFP_MW_ConfigData m_Data;
	
	static SFP_MW_ConfigManager Get()
	{
		if (!s_Instance)
		{
			s_Instance = new SFP_MW_ConfigManager();
		}
		return s_Instance;
	}
	
	void SFP_MW_ConfigManager()
	{
		m_Data = new SFP_MW_ConfigData();
		Load();
	}
	
	SFP_MW_ConfigData GetData()
	{
		return m_Data;
	}
	
	protected void Load()
	{
		if (!GetGame().IsServer() && GetGame().IsMultiplayer())
		{
			return;
		}
		
		if (!FileExist(SFP_MW_CONFIG_DIR))
		{
			MakeDirectory(SFP_MW_CONFIG_DIR);
		}
		
		string error;
		if (!FileExist(SFP_MW_CONFIG_PATH))
		{
			JsonFileLoader<SFP_MW_ConfigData>.SaveFile(SFP_MW_CONFIG_PATH, m_Data, error);
			Print("[SFP_MWolf] Создан новый конфиг: " + SFP_MW_CONFIG_PATH);
			return;
		}
		
		SFP_MW_ConfigData loaded = new SFP_MW_ConfigData();
		if (JsonFileLoader<SFP_MW_ConfigData>.LoadFile(SFP_MW_CONFIG_PATH, loaded, error))
		{
			if (!loaded.WolfTypes || loaded.WolfTypes.Count() == 0)
			{
				loaded.WolfTypes = new array<string>;
				loaded.WolfTypes.Insert("Animal_CanisLupus_Grey");
				loaded.WolfTypes.Insert("Animal_CanisLupus_White");
			}
			m_Data = loaded;
		}
		else
		{
			Print("[SFP_MWolf] Ошибка чтения конфига, будут использованы значения по умолчанию: " + error);
			JsonFileLoader<SFP_MW_ConfigData>.SaveFile(SFP_MW_CONFIG_PATH, m_Data, error);
		}
	}
};

void SFP_MW_Log(string msg, bool force = false)
{
	SFP_MW_ConfigManager manager = SFP_MW_ConfigManager.Get();
	if (force || (manager && manager.GetData() && manager.GetData().EnableDebugLog))
	{
		Print("[SFP_MWolf] " + msg);
	}
}
