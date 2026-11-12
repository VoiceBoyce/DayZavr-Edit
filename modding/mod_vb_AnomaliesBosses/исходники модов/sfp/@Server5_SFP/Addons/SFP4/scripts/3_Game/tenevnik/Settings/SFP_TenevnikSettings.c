class SFP_TenevnikSettings
{
	// ---- клоны ----
	int		m_CloneCountMax				= 5; 	// поддерживаемое максимальное количество клонов
	int		m_CloneSpawnInitialMin		= 3; 	// сколько клонов заспавнить сразу при агре
	int		m_CloneSpawnInitialMax		= 5;
	float	m_CloneHealth				= 1.0;
	float	m_CloneRespawnMin			= 6.0;
	float	m_CloneRespawnMax			= 12.0;
	float	m_CloneSpawnDistMin			= 8.0;
	float	m_CloneSpawnDistMax			= 20.0;
	
	// ---- аура ----
	float	m_AuraRadius					= 60.0;
	float	m_AuraActiveMin				= 3.0;
	float	m_AuraActiveMax				= 10.0;
	float	m_AuraReloadMin				= 3.0;
	float	m_AuraReloadMax				= 7.0;
	float	m_ReflectedDamageHP			= 2.0;
	
	// ---- основной мутант ----
	float	m_TenevnikHealth			= 500.0;
	
	// ---- визуал (тепловое искажение) ----
	// В ваниле значения обычно < 1.0 (например костёр: радиус ~0.23, мощность ~0.010).
	// Здесь делаем заметно сильнее "жара".
	float	m_HeatHazeRadiusActive		= 0.18;
	float	m_HeatHazePowerActive		= 0.018;
	float	m_HeatHazeRadiusReload		= 0.08;
	float	m_HeatHazePowerReload		= 0.008;

	// файлы
	static const string DIR = "$profile:SFP";
	static const string PATH = "$profile:SFP/SFP_TenevnikSettings.json";
	
	private static ref SFP_TenevnikSettings s_Instance;

	static SFP_TenevnikSettings Get()
	{
		if (!s_Instance)
			Load();
		return s_Instance;
	}

	static void Load()
	{
		if (!FileExist(DIR))
			MakeDirectory(DIR);

		s_Instance = new SFP_TenevnikSettings();
		string err;
		if (FileExist(PATH))
		{
			if (!JsonFileLoader<SFP_TenevnikSettings>.LoadFile(PATH, s_Instance, err))
			{
				SFP_Log.Error("TenevnikSettings: Load failed: " + err);
				// резервная копия повреждённого файла
				string backup = PATH + ".broken";
				CopyFile(PATH, backup);
				JsonFileLoader<SFP_TenevnikSettings>.SaveFile(PATH, s_Instance, err);
			}
		}
		else
		{
			if (!JsonFileLoader<SFP_TenevnikSettings>.SaveFile(PATH, s_Instance, err))
				SFP_Log.Error("TenevnikSettings: Save failed: " + err);
		}
	}
}
