// ShumMutant - загрузка настроек из JSON (server/client).
// ВАЖНО: основной файл настроек создаётся автоматически при первом запуске:
//
//   $profile:\SFP\ShumMutantConfig.json
//
// На выделенном сервере это будет папка профиля сервера (например: serverProfile/SFP/ShumMutantConfig.json).
// Меняете значения -> перезапускаете сервер/клиент.
//
// Используются только ванильные API DayZ: JsonFileLoader, FileExist, MakeDirectory.

class ShumMutantConfig
{
	// Можно оставить как есть. Поле нужно только чтобы в JSON было видно, где лежит документация.
	string _comment;

	// --------------------------------------------
	// 0) Телекинез предметов (бросание ground loot в игрока)
	// ВАЖНО: самая тяжёлая для сервера часть (поиск предметов вокруг мутанта).
	// По умолчанию отключено. Включите true, если нужно.
	// --------------------------------------------
	bool  ITEM_THROW_ENABLED;


	// --------------------------------------------
	// 1) ВОЛНА (бросок) при агрессии
	// --------------------------------------------
	float WIND_RADIUS_M;				// Радиус действия (метры)
	float WIND_COOLDOWN_S;				// Кулдаун между волнами (сек)
	float WIND_FORCE_HORIZONTAL;		// Горизонтальная сила импульса
	float WIND_FORCE_UP;				// Вертикальная сила импульса (подброс вверх)
	float WIND_FORCE_RANDOM_YAW;		// Разброс направления (градусы)

	// Урон от самого момента броска (сразу, при применении импульса)
	float THROW_START_HEALTH_COEF;		// 0..1, урон по HP (как коэффициент ванильного fall damage)
	float THROW_START_SHOCK_COEF;		// 0..1, урон по Shock
	bool  THROW_ZERO_STAMINA;			// true = при каждом броске стамина становится 0

	// Порча предметов в инвентаре
	int   THROW_ITEM_DAMAGE_COUNT;		// Сколько предметов портить за один бросок
	float THROW_ITEM_DAMAGE_COEF_MIN;	// 0..1, минимальная доля от max Health предмета, снимаемая за тик
	float THROW_ITEM_DAMAGE_COEF_MAX;	// 0..1, максимальная доля от max Health предмета

	// Урон при приземлении (fallback, если ванильный fall damage не сработал после импульса)
	float THROW_LAND_MIN_HEIGHT_M;
	float THROW_LAND_HEALTH_HEIGHT_LOW;
	float THROW_LAND_HEALTH_HEIGHT_HIGH;
	float THROW_LAND_SHOCK_HEIGHT_LOW;
	float THROW_LAND_SHOCK_HEIGHT_HIGH;
	float THROW_TRACK_TIMEOUT_S;

	// Тряска камеры во время полёта (клиент)
	float THROW_CAMERA_SHAKE_STRENGTH;
	float THROW_CAMERA_SHAKE_RADIUS;
	float THROW_CAMERA_SHAKE_SMOOTH;
	float THROW_CAMERA_SHAKE_DECAY;
	float THROW_CAMERA_SHAKE_INTERVAL_S;

	// Искажение воздуха НА ИГРОКЕ при толчке/броске (эффект "телекинеза")
	bool  PUSH_HAZE_ENABLED;            // true = включить искажение на игроке при каждом толчке
	float PUSH_HAZE_DURATION_S;         // Сколько секунд держать искажение после толчка
	float PUSH_HAZE_RADIUS;             // Радиус heat haze (в метрах вокруг игрока)
	float PUSH_HAZE_POWER;              // Сила искажения (0..~0.5, дальше обычно уже "кипит" сильно)
	vector PUSH_HAZE_LOCAL_POS;         // Смещение относительно игрока (обычно чуть выше центра)
	float PUSH_HAZE_LIGHT_RADIUS;       // Технический радиус лампы (не путать с haze radius)
	float PUSH_HAZE_LIGHT_BRIGHTNESS;   // Яркость лампы (чтобы движок не удалял объект света)
	float PUSH_HAZE_LIGHT_COLOR;        // Очень слабый серый цвет (чтобы не светило, но haze работал)


	// --------------------------------------------
	// 2) ОГЛУШЕНИЕ (звон в ушах) по фокусу
	// --------------------------------------------
	float  TINNITUS_RANGE_M;
	float  TINNITUS_COOLDOWN_S;
	float  TINNITUS_PROGRESS_MIN;
	float  TINNITUS_PROGRESS_MAX;
	float  TINNITUS_SOUND_MAX;
	float  TINNITUS_BASE_DURATION_S;
	bool   TINNITUS_FALLBACK_NEAREST;

	// ВАЖНО: Если у вас нет звука — скорее всего в вашей сборке другие названия soundset/attenuation.
	string TINNITUS_SOUNDSET;			// По умолчанию: "Tinnitus_SoundSet"
	string TINNITUS_ATTENUATION;		// По умолчанию: "FlashbangAttenuation"

	// --------------------------------------------
	// 2.1) ЗВУКИ "ТЕЛЕКИНЕЗА" И ЗАЩИТЫ
	// --------------------------------------------
	// Звук телепатического удара (воспроизводится НА ИГРОКЕ при каждом телекинезном броске)
	string THROW_HIT_SOUNDSET;         // По умолчанию: "SFP_ShumF_anomaly_gravy_hit1_Soundset"

	// Звуки брони/защиты мутанта
	string INVULN_ARMOR_SOUNDSET;      // Идёт ПОКА мутант неуязвим ("броня")
	string INVULN_RELOAD_SOUNDSET;     // Идёт ПОКА защита "перезаряжается" (когда неуязвимости нет)

	// --------------------------------------------
	// 3) Анти-возвышенности: если цель недоступна для удара
	// --------------------------------------------
	float UNREACHABLE_GRACE_S;

	// --------------------------------------------
	// 4) Смерть: взрыв/шок/отбрасывание
	// --------------------------------------------
	float  DEATH_RADIUS_M;
	float  DEATH_FORCE_HORIZONTAL;
	float  DEATH_FORCE_UP;
	float  DEATH_CAMERA_SHAKE_STRENGTH;
	float  DEATH_CAMERA_SHAKE_RADIUS;
	float  DEATH_CAMERA_SHAKE_SMOOTH;
	float  DEATH_CAMERA_SHAKE_DECAY;
	string DEATH_SOUNDSET;				// По умолчанию: "FlashGrenade_Explosion_SoundSet"
	float  DEATH_SOUND_VOLUME;			// 0..1

	// --------------------------------------------
	// Постоянное искажение воздуха на мутанте (heat haze)
	// --------------------------------------------
	float  HEAT_HAZE_RADIUS;
	float  HEAT_HAZE_POWER;
	vector HEAT_HAZE_LOCAL_POS;
	float  HEAT_HAZE_LIGHT_RADIUS;
	float  HEAT_HAZE_LIGHT_BRIGHTNESS;
	float  HEAT_HAZE_LIGHT_COLOR;		// Очень слабый серый цвет (чтобы haze был видим)

	// --------------------------------------------
	// Постоянный партикл на мутанте (клиент)
	// --------------------------------------------
	bool   MUTANT_IDLE_PARTICLE_ENABLED;   // true = вешать партикл постоянно
	string MUTANT_IDLE_PARTICLE_NAME;      // Имя из ParticleList (по умолчанию: "SFP_TELE_UP_TRASH_PRT")
	vector MUTANT_IDLE_PARTICLE_LOCAL_POS; // Локальная позиция партикла на теле мутанта

	// --------------------------------------------
	// Телекинез предметов: мутант бросает предметы в фокусированного игрока
	// --------------------------------------------
	float ITEM_THROW_COOLDOWN_S;           // Отдельный таймер (сек): как часто бросать предметы В ЦЕЛЬ
	float ITEM_THROW_TARGET_RANGE_M;       // Дальность до фокус-цели, при которой кидаем предметы
	float ITEM_THROW_RADIUS_M;             // Радиус поиска предметов вокруг мутанта
	int   ITEM_THROW_MAX_ITEMS;            // Максимум предметов за 1 волну (защита от лагов)
	float ITEM_THROW_FORCE_HORIZONTAL;     // Горизонтальная сила импульса (в сторону цели)
	float ITEM_THROW_FORCE_UP;             // Подброс вверх
	float ITEM_THROW_RANDOM_SPREAD_DEG;    // Небольшой разброс направления (градусы)
	float ITEM_THROW_MIN_MASS_KG;          // Минимальная масса предмета (kg), иначе пропускаем (мусор/очень лёгкое)
	float ITEM_THROW_MAX_MASS_KG;          // Максимальная масса (kg), иначе пропускаем (слишком тяжёлое)
	bool  ITEM_THROW_SCALE_BY_MASS;        // true = масштабировать импульс по массе (чтобы тяжёлое тоже летело)
	float ITEM_THROW_MASS_SCALE_MIN;       // Минимальный множитель импульса (если scale_by_mass)
	float ITEM_THROW_MASS_SCALE_MAX;       // Максимальный множитель импульса (если scale_by_mass)

	// --------------------------------------------
	// Временная НЕУЯЗВИМОСТЬ мутанта
	// --------------------------------------------
	float INVULN_DURATION_MIN_S;		// 5..10 сек по умолчанию
	float INVULN_DURATION_MAX_S;
	float INVULN_COOLDOWN_MIN_S;		// 5..15 сек по умолчанию
	float INVULN_COOLDOWN_MAX_S;

	// Визуал неуязвимости: слабый синий свет + сильный heat haze
	vector INVULN_LIGHT_LOCAL_POS;
	float  INVULN_LIGHT_RADIUS;
	float  INVULN_LIGHT_BRIGHTNESS;
	vector INVULN_LIGHT_COLOR_RGB;		// "R G B", например "0.05 0.10 0.85"

	float  INVULN_HEAT_HAZE_RADIUS;
	float  INVULN_HEAT_HAZE_POWER;

	// Позволяет задать дефолты прямо в коде, если JSON ещё не создан.
	void SetDefaults()
	{
		_comment = "Настройки ShumMutant. Файл: $profile:\\SFP\\ShumMutantConfig.json. Меняйте значения и перезапускайте сервер.";

		WIND_RADIUS_M = 50.0;
		WIND_COOLDOWN_S = 10.0;
		WIND_FORCE_HORIZONTAL = 650.0;
		WIND_FORCE_UP = 450.0;
		WIND_FORCE_RANDOM_YAW = 180.0;

		THROW_START_HEALTH_COEF = 0.02;//дефолт 0.20
		THROW_START_SHOCK_COEF  = 0.05;
		THROW_ZERO_STAMINA = true;

		THROW_ITEM_DAMAGE_COUNT = 1;		//дефолт  18
		THROW_ITEM_DAMAGE_COEF_MIN = 0.25; //дефолт  0.55
		THROW_ITEM_DAMAGE_COEF_MAX = 0.65; //дефолт 0.95

		THROW_LAND_MIN_HEIGHT_M = 3.0;
		THROW_LAND_HEALTH_HEIGHT_LOW = 5.0;
		THROW_LAND_HEALTH_HEIGHT_HIGH = 14.0;
		THROW_LAND_SHOCK_HEIGHT_LOW = 3.0;
		THROW_LAND_SHOCK_HEIGHT_HIGH = 12.0;
		THROW_TRACK_TIMEOUT_S = 10.0;

		THROW_CAMERA_SHAKE_STRENGTH = 12.0;	// поднял по умолчанию, чтобы точно чувствовалось
		THROW_CAMERA_SHAKE_RADIUS = 25.0;
		THROW_CAMERA_SHAKE_SMOOTH = 9.0;
		THROW_CAMERA_SHAKE_DECAY = 8.0;
		THROW_CAMERA_SHAKE_INTERVAL_S = 0.08;

		PUSH_HAZE_ENABLED = true;
		PUSH_HAZE_DURATION_S = 2.5;			// пару секунд
		PUSH_HAZE_RADIUS = 1.8;				// заметный радиус искажения вокруг игрока
		PUSH_HAZE_POWER  = 0.26;				// сильное искажение (как телекинез)
		PUSH_HAZE_LOCAL_POS = "0 1.1 0";	// примерно на уровне груди/головы
		PUSH_HAZE_LIGHT_RADIUS = 4.0;
		PUSH_HAZE_LIGHT_BRIGHTNESS = 1.0;
		PUSH_HAZE_LIGHT_COLOR = 0.03;


		TINNITUS_RANGE_M = 40.0;
		TINNITUS_COOLDOWN_S = 12.0;
		TINNITUS_PROGRESS_MIN = 0.55;
		TINNITUS_PROGRESS_MAX = 1.00;
		TINNITUS_SOUND_MAX = 1.0;
		TINNITUS_BASE_DURATION_S = 8.0;
		TINNITUS_FALLBACK_NEAREST = true;

		TINNITUS_SOUNDSET = "Tinnitus_SoundSet";
		TINNITUS_ATTENUATION = "FlashbangAttenuation";

		// Звуки "телекинеза" и защиты
		THROW_HIT_SOUNDSET = "SFP_ShumF_anomaly_gravy_hit1_Soundset";
		INVULN_ARMOR_SOUNDSET = "SFP_ShumF_TeleArmor_Soundset";
		INVULN_RELOAD_SOUNDSET = "SFP_ShumF_TeleArmorReload_Soundset";

		// Постоянный партикл на мутанте
		MUTANT_IDLE_PARTICLE_ENABLED = true;
		MUTANT_IDLE_PARTICLE_NAME = "SFP_TELE_UP_TRASH_PRT";
		MUTANT_IDLE_PARTICLE_LOCAL_POS = "0 0.9 0";

		// Телекинез предметов
		ITEM_THROW_ENABLED = false;
		ITEM_THROW_COOLDOWN_S = 2.25; // отдельный таймер (сек)
		ITEM_THROW_TARGET_RANGE_M = 55.0;
		ITEM_THROW_RADIUS_M = 12.0;
		ITEM_THROW_MAX_ITEMS = 12;
		ITEM_THROW_FORCE_HORIZONTAL = 290.0;
		ITEM_THROW_FORCE_UP = 15.0;
		ITEM_THROW_RANDOM_SPREAD_DEG = 10.0;
		ITEM_THROW_MIN_MASS_KG = 0.05;
		ITEM_THROW_MAX_MASS_KG = 45.0;
		ITEM_THROW_SCALE_BY_MASS = false;
		ITEM_THROW_MASS_SCALE_MIN = 0.7;
		ITEM_THROW_MASS_SCALE_MAX = 3.0;

		UNREACHABLE_GRACE_S = 1.25;

		DEATH_RADIUS_M = 20.0;
		DEATH_FORCE_HORIZONTAL = 900.0;
		DEATH_FORCE_UP = 650.0;
		DEATH_CAMERA_SHAKE_STRENGTH = 10.0;
		DEATH_CAMERA_SHAKE_RADIUS = 30.0;
		DEATH_CAMERA_SHAKE_SMOOTH = 11.0;
		DEATH_CAMERA_SHAKE_DECAY = 10.0;
		DEATH_SOUNDSET = "FlashGrenade_Explosion_SoundSet";
		DEATH_SOUND_VOLUME = 1.0;

		HEAT_HAZE_RADIUS = 0.95;				// сильнее, чтобы было видно
		HEAT_HAZE_POWER  = 0.12;
		HEAT_HAZE_LOCAL_POS = "0 1.2 0";
		HEAT_HAZE_LIGHT_RADIUS = 3.5;
		HEAT_HAZE_LIGHT_BRIGHTNESS = 0.95;		// яркость >0, иначе движок может удалить
		HEAT_HAZE_LIGHT_COLOR = 0.04;

		INVULN_DURATION_MIN_S = 5.0;
		INVULN_DURATION_MAX_S = 10.0;
		INVULN_COOLDOWN_MIN_S = 5.0;
		INVULN_COOLDOWN_MAX_S = 15.0;

		INVULN_LIGHT_LOCAL_POS = "0 1.35 0";
		INVULN_LIGHT_RADIUS = 12.0;
		INVULN_LIGHT_BRIGHTNESS = 1.35;
		INVULN_LIGHT_COLOR_RGB = "0.05 0.10 0.85";

		INVULN_HEAT_HAZE_RADIUS = 1.35;
		INVULN_HEAT_HAZE_POWER  = 0.24;
	}
}

class ShumMutantConfigManager
{
	protected static ref ShumMutantConfig s_Cfg;

	static ShumMutantConfig Get()
	{
		if (!s_Cfg)
			s_Cfg = LoadOrCreate();
		return s_Cfg;
	}

	static string GetFolder()
	{
		// Пользователь просил "в папку SFP"
		return "$profile:SFP";
	}

	static string GetPath()
	{
		return GetFolder() + "\\ShumMutantConfig.json";
	}

	protected static ShumMutantConfig LoadOrCreate()
	{
		ShumMutantConfig cfg = new ShumMutantConfig();
		cfg.SetDefaults();

		string folder = GetFolder();
		string path = GetPath();

		if (!FileExist(folder))
			MakeDirectory(folder);

		if (FileExist(path))
		{
			// Подгружаем поверх дефолтов
			JsonFileLoader<ShumMutantConfig>.JsonLoadFile(path, cfg);
			// Пересохраняем, чтобы новые поля (и порядок) гарантированно попали в JSON
			JsonFileLoader<ShumMutantConfig>.JsonSaveFile(path, cfg);
		}
		else
		{
			// Создаем файл с дефолтными настройками
			JsonFileLoader<ShumMutantConfig>.JsonSaveFile(path, cfg);
		}

		return cfg;
	}

	// Можно вручную сбросить кэш (например, если захотите сделать hot-reload)
	static void Reset()
	{
		s_Cfg = null;
	}
}
