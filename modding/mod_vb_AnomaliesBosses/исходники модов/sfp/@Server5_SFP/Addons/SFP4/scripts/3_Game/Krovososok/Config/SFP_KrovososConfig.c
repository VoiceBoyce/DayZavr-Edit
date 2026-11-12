class SFP_KrovososConfigData
{
	string _comment_01 = "Минимальная задержка перед повторным уходом мутанта в невидимость, в секундах.";
	float InvisibleReturnMinSeconds = 3.0;
	
	string _comment_02 = "Максимальная задержка перед повторным уходом мутанта в невидимость, в секундах.";
	float InvisibleReturnMaxSeconds = 5.0;
	
	string _comment_03 = "Сколько ударов/попаданий подряд по невидимому мутанту нужно по умолчанию, чтобы сорвать инвиз.";
	int HitsToBreakInvisibility = 6;
	
	string _comment_04 = "Сколько суммарного урона по здоровью нужно по умолчанию, чтобы сорвать инвиз. Работает вместе с HitsToBreakInvisibility.";
	float DamageToBreakInvisibility = 35.0;
	
	string _comment_05 = "Окно, в течение которого серия попаданий считается непрерывной. Если стрелять реже — счётчик серии сбрасывается.";
	float ContinuousHitWindowSeconds = 2.5;
	
	string _comment_06 = "Сколько крови мутант снимает с игрока при успешном попадании лапой.";
	float BloodDamagePerAttack = 100.0;
	
	string _comment_07 = "Текущее здоровье мутанта при спавне. Значение будет зажато в максимум из config.cpp. Если хотите больше ванильного максимума — поднимите hitpoints в config.cpp.";
	float SpawnHealth = 2500.0;

	string _comment_08 = "Минимальная задержка отката импульсного сброса, если цель залезла выше и мутант не может её достать.";
	float ElevatedPulseCooldownMinSeconds = 5.0;

	string _comment_09 = "Максимальная задержка отката импульсного сброса, если цель залезла выше и мутант не может её достать.";
	float ElevatedPulseCooldownMaxSeconds = 7.0;

	string _comment_10 = "Максимальная дистанция от мутанта до недосягаемой цели, при которой может сработать импульсный сброс.";
	float ElevatedPulseTriggerDistance = 4.8;

	string _comment_11 = "Насколько выше мутанта должна быть цель по оси Y, чтобы считаться недосягаемой сверху.";
	float ElevatedPulseMinHeightDelta = 0.9;

	string _comment_12 = "Радиус вокруг цели, в котором игроки тоже получают импульсный толчок.";
	float ElevatedPulseAffectRadius = 15.0;

	string _comment_13 = "Горизонтальная сила импульса, который сбрасывает игроков с машины или высокого объекта.";
	float ElevatedPulseHorizontalVelocity = 8.5;

	string _comment_14 = "Вертикальная составляющая импульса, чтобы игрока немного подбросило и он слетел с края.";
	float ElevatedPulseVerticalVelocity = 4.25;
	
	string _comment_15 = "Общий множитель силы импульса. Увеличивает и боковой толчок, и подброс вверх.";
	float ElevatedPulseForceMultiplier = 0.6;

	string _comment_16 = "Точная задержка между повторными использованиями импульса в секундах. Если больше 0 — используется она, а не случайный диапазон Min/Max ниже.";
	float ElevatedPulseReuseCooldownSeconds = 0.0;

	string _comment_17 = "Как часто сервер проверяет, что цель забралась слишком высоко и её нужно сбросить импульсом. Меньше значение — быстрее реакция, но немного выше нагрузка.";
	float ElevatedPulseCheckIntervalSeconds = 0.35;
};

class SFP_KrovososConfig
{
	protected static const string DIR_PATH = "$profile:\\SFP";
	protected static const string FILE_PATH = "$profile:\\SFP\\KrovososokConfig.json";
	protected static ref SFP_KrovososConfigData s_Data;
	protected static bool s_Loaded;
	
	static SFP_KrovososConfigData Get()
	{
		if (!s_Loaded)
			Load();
		
		return s_Data;
	}
	
	static void Load()
	{
		if (s_Loaded)
			return;
		
		s_Loaded = true;
		s_Data = new SFP_KrovososConfigData();
		
		if (GetGame().IsServer())
		{
			if (!FileExist(DIR_PATH))
				MakeDirectory(DIR_PATH);
			
			if (!FileExist(FILE_PATH))
			{
				Save();
				Print("[SFP_KrovososConfig] Создан новый конфиг: " + FILE_PATH);
				return;
			}
		}
		
		if (FileExist(FILE_PATH))
		{
			string error;
			if (!JsonFileLoader<SFP_KrovososConfigData>.LoadFile(FILE_PATH, s_Data, error))
			{
				ErrorEx("[SFP_KrovososConfig] Ошибка чтения конфига: " + error);
				s_Data = new SFP_KrovososConfigData();
			}
		}
	}
	
	static void Save()
	{
		if (!s_Data)
			s_Data = new SFP_KrovososConfigData();
		
		if (!FileExist(DIR_PATH))
			MakeDirectory(DIR_PATH);
		
		string error;
		JsonFileLoader<SFP_KrovososConfigData>.SaveFile(FILE_PATH, s_Data, error);
		if (error != string.Empty)
			ErrorEx("[SFP_KrovososConfig] Ошибка сохранения конфига: " + error);
	}
};
