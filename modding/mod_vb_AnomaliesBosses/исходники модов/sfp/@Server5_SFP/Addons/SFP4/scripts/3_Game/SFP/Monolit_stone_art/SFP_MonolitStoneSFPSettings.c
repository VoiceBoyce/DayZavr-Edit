// ============================================================================
// МОНОЛИТ — настройки в JSON
// Файл создаётся в профиле:
//   $profile:SFP/MonolitStoneSFP.json
//
// Можно настроить:
//  - радиус действия звука
//  - радиус действия света
//  - радиус включения экранных эффектов (PPE)
// ============================================================================

class SFP_MonolitStoneSFPSettings
{
	float m_SoundRadius_M      = 300.0;
	float m_LightRadius_M      = 100.0;
	float m_ScreenEffectsRadius_M = 300.0;

	static const string DIR  = "$profile:SFP";
	static const string PATH = "$profile:SFP/MonolitStoneSFP.json";

	private static ref SFP_MonolitStoneSFPSettings s_Instance;

	static SFP_MonolitStoneSFPSettings Get()
	{
		if (!s_Instance)
			Load();
		return s_Instance;
	}

	void Sanitize()
	{
		m_SoundRadius_M = Math.Clamp(m_SoundRadius_M, 0.0, 2000.0);
		m_LightRadius_M = Math.Clamp(m_LightRadius_M, 0.0, 500.0);
		m_ScreenEffectsRadius_M = Math.Clamp(m_ScreenEffectsRadius_M, 0.0, 2000.0);
	}

	static void Load()
	{
		if (!FileExist(DIR))
			MakeDirectory(DIR);

		s_Instance = new SFP_MonolitStoneSFPSettings();
		string err;

		if (FileExist(PATH))
		{
			if (!JsonFileLoader<SFP_MonolitStoneSFPSettings>.LoadFile(PATH, s_Instance, err))
			{
				SFP_Log.Error("MonolitStoneSettings: Load failed: " + err);
				string backup = PATH + ".broken";
				CopyFile(PATH, backup);
				s_Instance = new SFP_MonolitStoneSFPSettings();
				s_Instance.Sanitize();
				JsonFileLoader<SFP_MonolitStoneSFPSettings>.SaveFile(PATH, s_Instance, err);
			}
			else
			{
				s_Instance.Sanitize();
				// перезаписываем, чтобы добавить новые поля при обновлениях
				JsonFileLoader<SFP_MonolitStoneSFPSettings>.SaveFile(PATH, s_Instance, err);
			}
		}
		else
		{
			s_Instance.Sanitize();
			if (!JsonFileLoader<SFP_MonolitStoneSFPSettings>.SaveFile(PATH, s_Instance, err))
				SFP_Log.Error("MonolitStoneSettings: Save failed: " + err);
		}
	}
}
