/**
 * SFP_Clock_digital
 * --------------------
 * Отображает на модели:
 *  - время сервера (world date): HH:MM
 *  - локальную температуру там, где находятся часы (учитывает UniversalTemperatureSource, напр. костёр)
 *
 * Требования к CfgVehicles (обязательно, иначе НЕ будет действий ВКЛ/ВЫКЛ):
 *  - attachments[] = {"BatteryD"};
 *  - class EnergyManager { hasIcon=1; plugType=1; energyUsagePerSecond=...; updateInterval=1; };
 *
 * HiddenSelections (имена должны 1:1 совпадать с моделью):
 *  time_hourX_
 *  time_hour_X
 *  time_minuteX_
 *  time_minute_X
 *  Weather_temper
 *  Weather_temper_numerX_
 *  Weather_temper_numer_X
 */
class SFP_Clock_digital extends ItemBase
{
	// --- texture paths
	protected static const string TEX_NUM_PREFIX = "SFP3\\1_item\\Soviet_Clock\\data_2\\RedTime\\RedNumber_";
	protected static const string TEX_NUM_SUFFIX = "_co.paa";
	protected static const string TEX_PLUS       = "SFP3\\1_item\\Soviet_Clock\\data_2\\RedTime\\RedNumber_plus_co.paa";
	protected static const string TEX_MINUS      = "SFP3\\1_item\\Soviet_Clock\\data_2\\RedTime\\RedNumber_munis_co.paa";
	// Прозрачная "пустая" текстура, чтобы не было цифр, когда часы выключены/без батарейки
	protected static const string TEX_CLEAR      = "#(argb,8,8,3)color(0,0,0,0,ca)";

	// --- weather icon textures (display)
	protected static const string TEX_WEATHER_CLEAR_DAY        = "SFP3\\1_item\\Soviet_Clock\\data_2\\RedTime\\RedWeather_ClearSky_co.paa";
	protected static const string TEX_WEATHER_CLOUDY           = "SFP3\\1_item\\Soviet_Clock\\data_2\\RedTime\\RedWeather_cloudy_co.paa";
	protected static const string TEX_WEATHER_FEW_CLOUDS_DAY   = "SFP3\\1_item\\Soviet_Clock\\data_2\\RedTime\\RedWeather_few_clouds_co.paa";
	protected static const string TEX_WEATHER_CLEAR_NIGHT      = "SFP3\\1_item\\Soviet_Clock\\data_2\\RedTime\\RedWeather_night_ClearSky_co.paa";
	protected static const string TEX_WEATHER_FEW_CLOUDS_NIGHT = "SFP3\\1_item\\Soviet_Clock\\data_2\\RedTime\\RedWeather_night_few_clouds_co.paa";
	protected static const string TEX_WEATHER_RAIN_NIGHT       = "SFP3\\1_item\\Soviet_Clock\\data_2\\RedTime\\RedWeather_night_rain_co.paa";
	protected static const string TEX_WEATHER_RAIN_DAY         = "SFP3\\1_item\\Soviet_Clock\\data_2\\RedTime\\RedWeather_rain_co.paa";
	protected static const string TEX_WEATHER_SHOWER_RAIN      = "SFP3\\1_item\\Soviet_Clock\\data_2\\RedTime\\RedWeather_shower_rain_co.paa";
	protected static const string TEX_WEATHER_SNOW             = "SFP3\\1_item\\Soviet_Clock\\data_2\\RedTime\\RedWeather_snow_co.paa";
	protected static const string TEX_WEATHER_THUNDERSTORM     = "SFP3\\1_item\\Soviet_Clock\\data_2\\RedTime\\RedWeather_thunderstorm_co.paa";

	// --- selection names
	protected static const string SEL_TIME_HOUR_TENS = "time_hourX_";
	protected static const string SEL_TIME_HOUR_ONES = "time_hour_X";
	protected static const string SEL_TIME_MIN_TENS  = "time_minuteX_";
	protected static const string SEL_TIME_MIN_ONES  = "time_minute_X";

	protected static const string SEL_TEMP_SIGN = "Weather_temper";
	protected static const string SEL_TEMP_TENS = "Weather_temper_numerX_";
	protected static const string SEL_TEMP_ONES = "Weather_temper_numer_X";

	// Иконка погоды на дисплее. ВАЖНО: имя селекции должно совпадать с P3D.
	// Если у тебя в модели называется иначе - просто поменяй эту строку.
	protected static const string SEL_WEATHER_ICON = "Weather_icon";

	// --- weather icon codes (synced)
	protected static const int WICON_CLEAR_DAY        = 0;
	protected static const int WICON_CLOUDY           = 1;
	protected static const int WICON_FEW_CLOUDS_DAY   = 2;
	protected static const int WICON_CLEAR_NIGHT      = 3;
	protected static const int WICON_FEW_CLOUDS_NIGHT = 4;
	protected static const int WICON_RAIN_NIGHT       = 5;
	protected static const int WICON_RAIN_DAY         = 6;
	protected static const int WICON_SHOWER_RAIN      = 7;
	protected static const int WICON_SNOW             = 8;
	protected static const int WICON_THUNDERSTORM     = 9;

	// --- selection indices cache
	protected bool m_SelCacheInit;
	protected int m_SelTimeHourTens = -1;
	protected int m_SelTimeHourOnes = -1;
	protected int m_SelTimeMinTens  = -1;
	protected int m_SelTimeMinOnes  = -1;
	protected int m_SelTempSign     = -1;
	protected int m_SelTempTens     = -1;
	protected int m_SelTempOnes     = -1;
	protected int m_SelWeatherIcon  = -1;

	// --- synced values (server -> clients)
	protected int m_SyncHour;
	protected int m_SyncMinute;
	protected int m_SyncTemp;
	protected int m_SyncWeatherIcon;
	protected bool m_SyncIsOn;

	// --- throttling on server
	protected int m_LastUpdateMS;
	protected int m_LastSentHour   = -1;
	protected int m_LastSentMinute = -1;
	protected int m_LastSentTemp   = 9999;
	protected int m_LastSentWeatherIcon = -1;
	protected bool m_LastSentIsOn;

	// Частота пересчёта/синхры на СЕРВЕРЕ (в мс). Можно смело править под себя.
	protected static const int UPDATE_INTERVAL_MS = 500;

	void SFP_Clock_digital()
	{
		RegisterNetSyncVariableInt("m_SyncHour", 0, 23);
		RegisterNetSyncVariableInt("m_SyncMinute", 0, 59);
		RegisterNetSyncVariableInt("m_SyncTemp", -99, 99);
		RegisterNetSyncVariableInt("m_SyncWeatherIcon", 0, 9);
		RegisterNetSyncVariableBool("m_SyncIsOn");
	}

	override void EEInit()
	{
		super.EEInit();
		InitSelectionCache();
		// По умолчанию пустой дисплей (цифры появятся только когда часы реально работают)
		if (!GetGame().IsDedicatedServer())
			ClearDisplay();
	}

	protected void InitSelectionCache()
	{
		if (m_SelCacheInit)
			return;

		m_SelTimeHourTens = GetHiddenSelectionIndex(SEL_TIME_HOUR_TENS);
		m_SelTimeHourOnes = GetHiddenSelectionIndex(SEL_TIME_HOUR_ONES);
		m_SelTimeMinTens  = GetHiddenSelectionIndex(SEL_TIME_MIN_TENS);
		m_SelTimeMinOnes  = GetHiddenSelectionIndex(SEL_TIME_MIN_ONES);

		m_SelTempSign = GetHiddenSelectionIndex(SEL_TEMP_SIGN);
		m_SelTempTens = GetHiddenSelectionIndex(SEL_TEMP_TENS);
		m_SelTempOnes = GetHiddenSelectionIndex(SEL_TEMP_ONES);

		m_SelWeatherIcon = GetHiddenSelectionIndex(SEL_WEATHER_ICON);

		m_SelCacheInit = true;
	}

	// --- Actions (ВКЛ/ВЫКЛ)
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionTurnOnWhileInHands);
		AddAction(ActionTurnOffWhileInHands);
	}

	// Энергоменеджер вызывает это, когда устройство реально работает
	override void OnWorkStart()
	{
		super.OnWorkStart();
		if (GetGame().IsServer())
		{
			m_SyncIsOn = true;
		}
		ServerUpdateSync(true);
	}

	override void OnWork(float consumed_energy)
	{
		super.OnWork(consumed_energy);
		ServerUpdateSync(false);
	}

	override void OnWorkStop()
	{
		super.OnWorkStop();

		// Сервер сообщает клиентам, что устройство выключено -> клиенты очищают дисплей
		if (GetGame().IsServer())
		{
			if (m_SyncIsOn)
			{
				m_SyncIsOn = false;
				m_LastSentIsOn = false;
				SetSynchDirty();
			}
		}

		if (!GetGame().IsDedicatedServer())
			ClearDisplay();
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		UpdateDisplayFromSync(false);
	}

	//--------------------------------------------------------------------------------
	// SERVER: compute values + sync
	//--------------------------------------------------------------------------------
	protected void ServerUpdateSync(bool force)
	{
		if (!GetGame().IsServer())
			return;

		int now = GetGame().GetTime();
		if (!force)
		{
			if ((now - m_LastUpdateMS) < UPDATE_INTERVAL_MS)
				return;
		}
		m_LastUpdateMS = now;

		int y, mo, d, h, mi;
		GetGame().GetWorld().GetDate(y, mo, d, h, mi);

		int tempI = Math.Round(GetLocalAmbientTemperature());
		tempI = Math.Clamp(tempI, -99, 99);

		int weatherIcon = ComputeWeatherIconCode(h);

		// Пока мы в OnWork/OnWorkStart — часы точно "работают"
		bool isOn = true;

		bool changed = force;
		if (isOn != m_LastSentIsOn) { m_LastSentIsOn = isOn; changed = true; }
		if (h != m_LastSentHour)   { m_LastSentHour = h; changed = true; }
		if (mi != m_LastSentMinute){ m_LastSentMinute = mi; changed = true; }
		if (tempI != m_LastSentTemp){ m_LastSentTemp = tempI; changed = true; }
		if (weatherIcon != m_LastSentWeatherIcon){ m_LastSentWeatherIcon = weatherIcon; changed = true; }

		if (changed)
		{
			m_SyncIsOn = isOn;
			m_SyncHour = h;
			m_SyncMinute = mi;
			m_SyncTemp = tempI;
			m_SyncWeatherIcon = weatherIcon;
			SetSynchDirty();
		}

		// на listen-server (SP/host) можно сразу отрисовать
		if (GetGame().IsClient() && !GetGame().IsDedicatedServer())
			UpdateDisplayFromSync(force);
	}

	//--------------------------------------------------------------------------------
	// CLIENT: apply textures
	//--------------------------------------------------------------------------------
	protected void UpdateDisplayFromSync(bool force)
	{
		if (GetGame().IsDedicatedServer())
			return;

		InitSelectionCache();
		if (!m_SyncIsOn)
		{
			ClearDisplay();
			return;
		}
		UpdateTimeDigits(m_SyncHour, m_SyncMinute);
		UpdateTemperatureDigits(m_SyncTemp);
		UpdateWeatherIcon(m_SyncWeatherIcon);
	}

	protected void ClearDisplay()
	{
		InitSelectionCache();
		SetObjectTextureSafe(m_SelTimeHourTens, TEX_CLEAR);
		SetObjectTextureSafe(m_SelTimeHourOnes, TEX_CLEAR);
		SetObjectTextureSafe(m_SelTimeMinTens,  TEX_CLEAR);
		SetObjectTextureSafe(m_SelTimeMinOnes,  TEX_CLEAR);

		SetObjectTextureSafe(m_SelTempSign, TEX_CLEAR);
		SetObjectTextureSafe(m_SelTempTens, TEX_CLEAR);
		SetObjectTextureSafe(m_SelTempOnes, TEX_CLEAR);

		SetObjectTextureSafe(m_SelWeatherIcon, TEX_CLEAR);
	}

	// На всякий случай: если батарейку вытащили вручную (даже когда часы были выкл)
	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);
		if (slot_name == "BatteryD")
		{
			if (GetGame().IsServer())
			{
				m_SyncIsOn = false;
				m_LastSentIsOn = false;
				SetSynchDirty();
			}
			if (!GetGame().IsDedicatedServer())
				ClearDisplay();
		}
	}

	protected void UpdateTimeDigits(int hour, int minute)
	{
		hour = Math.Clamp(hour, 0, 23);
		minute = Math.Clamp(minute, 0, 59);

		int hT = hour / 10;
		int hO = hour % 10;
		int mT = minute / 10;
		int mO = minute % 10;

		SetDigitTexture(m_SelTimeHourTens, hT);
		SetDigitTexture(m_SelTimeHourOnes, hO);
		SetDigitTexture(m_SelTimeMinTens, mT);
		SetDigitTexture(m_SelTimeMinOnes, mO);
	}

	protected void UpdateTemperatureDigits(int temperature)
	{
		bool neg = (temperature < 0);
		int absT = Math.AbsInt(temperature);

		int tT = absT / 10;
		int tO = absT % 10;

		if (neg)
			SetObjectTextureSafe(m_SelTempSign, TEX_MINUS);
		else
			SetObjectTextureSafe(m_SelTempSign, TEX_PLUS);

		SetDigitTexture(m_SelTempTens, tT);
		SetDigitTexture(m_SelTempOnes, tO);
	}

	//--------------------------------------------------------------------------------
	// WEATHER ICON
	//--------------------------------------------------------------------------------
	protected void UpdateWeatherIcon(int iconCode)
	{
		InitSelectionCache();
		if (m_SelWeatherIcon < 0)
			return;

		string tex = GetWeatherIconTexture(iconCode);
		if (tex == "")
			tex = TEX_CLEAR;

		SetObjectTextureSafe(m_SelWeatherIcon, tex);
	}

	protected string GetWeatherIconTexture(int iconCode)
	{
		switch (iconCode)
		{
			case WICON_CLEAR_DAY:
				return TEX_WEATHER_CLEAR_DAY;
			case WICON_CLOUDY:
				return TEX_WEATHER_CLOUDY;
			case WICON_FEW_CLOUDS_DAY:
				return TEX_WEATHER_FEW_CLOUDS_DAY;
			case WICON_CLEAR_NIGHT:
				return TEX_WEATHER_CLEAR_NIGHT;
			case WICON_FEW_CLOUDS_NIGHT:
				return TEX_WEATHER_FEW_CLOUDS_NIGHT;
			case WICON_RAIN_NIGHT:
				return TEX_WEATHER_RAIN_NIGHT;
			case WICON_RAIN_DAY:
				return TEX_WEATHER_RAIN_DAY;
			case WICON_SHOWER_RAIN:
				return TEX_WEATHER_SHOWER_RAIN;
			case WICON_SNOW:
				return TEX_WEATHER_SNOW;
			case WICON_THUNDERSTORM:
				return TEX_WEATHER_THUNDERSTORM;
		}
		return TEX_WEATHER_CLEAR_DAY;
	}

	protected bool IsNightHour(int hour)
	{
		// Простая граница "ночь/день". Если нужно - подгони под свою карту.
		return (hour < 6 || hour >= 20);
	}

	protected int ComputeWeatherIconCode(int hour)
	{
		Weather w = GetGame().GetWeather();
		if (!w)
			return WICON_CLEAR_DAY;

		float over = w.GetOvercast().GetActual();
		float rain = w.GetRain().GetActual();
		float snow = w.GetSnowfall().GetActual();

		bool isNight = IsNightHour(hour);

		// Snow has its own icon (night variant not provided)
		if (snow > 0.15)
			return WICON_SNOW;

		// Rainy states
		if (rain > 0.10)
		{
			// Thunderstorm approximation: very overcast + strong rain
			if (over > 0.85 && rain > 0.50)
				return WICON_THUNDERSTORM;

			// Heavy rain / shower
			if (rain > 0.70)
				return WICON_SHOWER_RAIN;

			// Normal rain
			if (isNight)
				return WICON_RAIN_NIGHT;
			return WICON_RAIN_DAY;
		}

		// No precipitation: decide by overcast
		if (over < 0.25)
		{
			if (isNight)
				return WICON_CLEAR_NIGHT;
			return WICON_CLEAR_DAY;
		}

		if (over < 0.60)
		{
			if (isNight)
				return WICON_FEW_CLOUDS_NIGHT;
			return WICON_FEW_CLOUDS_DAY;
		}

		return WICON_CLOUDY;
	}

	protected void SetObjectTextureSafe(int selectionIndex, string texturePath)
	{
		if (selectionIndex < 0)
			return;
		SetObjectTexture(selectionIndex, texturePath);
	}

	protected void SetDigitTexture(int selectionIndex, int digit)
	{
		if (selectionIndex < 0)
			return;
		digit = Math.Clamp(digit, 0, 9);
		SetObjectTexture(selectionIndex, TEX_NUM_PREFIX + digit.ToString() + TEX_NUM_SUFFIX);
	}

	//--------------------------------------------------------------------------------
	// TEMPERATURE (local, includes UniversalTemperatureSource)
	//--------------------------------------------------------------------------------
	protected vector GetClockWorldPosition()
	{
		EntityAI root = GetHierarchyRoot();
		if (root)
			return root.GetPosition();
		return GetPosition();
	}

	protected float GetLocalAmbientTemperature()
	{
		Mission mission = GetGame().GetMission();
		if (!mission)
			return 0.0;

		WorldData wd = mission.GetWorldData();
		if (!wd)
			return 0.0;

		vector pos = GetClockWorldPosition();

		// базовая температура от погоды/высоты
		float baseTemp = wd.GetTemperature(this, EEnvironmentTemperatureComponent.ALTITUDE | EEnvironmentTemperatureComponent.OVERCAST | EEnvironmentTemperatureComponent.FOG);

		// температура от источников (костёр и т.п.)
		float utsTemp = GetUTSAverageTemperatureAt(pos);

		// если источник "сильнее" базы — берём его
		if (utsTemp > 0.0 && utsTemp > baseTemp)
			baseTemp = utsTemp;
		else if (utsTemp < 0.0 && utsTemp < baseTemp)
			baseTemp = utsTemp;

		return baseTemp;
	}

	protected float GetUTSAverageTemperatureAt(vector pos)
	{
		array<UTemperatureSource> sources = new array<UTemperatureSource>();
		GatherTemperatureSourcesAt(pos, sources);

		int count = sources.Count();
		if (count == 0)
			return 0.0;

		float minV = 999999.0;
		float maxV = -999999.0;

		for (int i = 0; i < count; i++)
		{
			float v = CalcTemperatureFromTemperatureSourceAt(sources.Get(i), pos);
			if (v < minV) minV = v;
			if (v > maxV) maxV = v;
		}

		if (maxV > 0.0 && minV < 0.0)
			return (maxV + minV) * 0.5;

		return maxV;
	}

	protected void GatherTemperatureSourcesAt(vector pos, out notnull array<UTemperatureSource> outSources)
	{
		outSources.Clear();

		float r = GameConstants.ENVIRO_TEMP_SOURCES_LOOKUP_RADIUS;

		vector minPos = pos - Vector(r / 2, r / 2, r);
		vector maxPos = pos + Vector(r / 2, r / 2, r);

		array<EntityAI> ents = new array<EntityAI>();
		DayZPlayerUtils.SceneGetEntitiesInBox(minPos, maxPos, ents, QueryFlags.DYNAMIC | QueryFlags.STATIC);

		foreach (EntityAI ent : ents)
		{
			if (!ent)
				continue;

			if (ent.IsUniversalTemperatureSource() && ent != this)
			{
				UniversalTemperatureSource uts = ent.GetUniversalTemperatureSource();
				if (!uts)
					continue;

				if (!uts.GetLambda() || !uts.GetLambda().AffectsPlayer())
					continue;

				if (vector.DistanceSq(pos, ent.GetPosition()) > Math.SqrFloat(uts.GetMaxRange()))
					continue;

				outSources.Insert(uts);
			}
		}
	}

	protected float CalcTemperatureFromTemperatureSourceAt(notnull UTemperatureSource uts, vector receiverPos)
	{
		float distance = vector.Distance(receiverPos, uts.GetPosition());
		distance = Math.Max(distance, 0.1);

		float temperature;
		if (distance > uts.GetFullRange())
		{
			float distFactor = Math.InverseLerp(uts.GetMaxRange(), uts.GetFullRange(), distance);
			temperature = uts.GetTemperatureCap() * distFactor;
		}
		else
		{
			temperature = uts.GetTemperatureCap();
		}

		return temperature;
	}
}
