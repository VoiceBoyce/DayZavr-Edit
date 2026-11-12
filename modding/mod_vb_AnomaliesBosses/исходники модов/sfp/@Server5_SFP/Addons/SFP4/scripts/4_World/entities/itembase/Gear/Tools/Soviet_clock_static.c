/**
 * SFP_Clock_Digital_static
 * -----------------------
 * Статичные цифровые часы (как объект на карте / building).
 * Конфиг-класс у тебя: class SFP_Clock_Digital_static : HouseNoDestruct
 *
 * Показывают:
 *  - время сервера (world date): HH:MM
 *  - локальную температуру в точке часов (учитывает UniversalTemperatureSource: костёр и т.п.)
 *  - иконку погоды (Weather_icon)
 *
 * Режим "далеко/близко":
 *  - если рядом НЕТ игроков (ACTIVE_RADIUS_M), дисплей гаснет
 *  - при приближении игрока включается и обновляется
 */
 
class SFP_Clock_Electronika_static : CrashBase
{
	// -------------------------------------------------------------------------
	// TEXTURES: digits
	// -------------------------------------------------------------------------
	protected static const string TEX_NUM_PREFIX = "SFP3\\1_item\\Soviet_Clock\\data\\time\\number_";
	protected static const string TEX_NUM_SUFFIX = "_co.paa";
	protected static const string TEX_PLUS       = "SFP3\\1_item\\Soviet_Clock\\data\\time\\number_plus_co.paa";
	protected static const string TEX_MINUS      = "SFP3\\1_item\\Soviet_Clock\\data\\time\\number_munis_co.paa";
	// Прозрачная "пустая" текстура, чтобы не было цифр, когда часы выключены/без батарейки
	protected static const string TEX_CLEAR      = "#(argb,8,8,3)color(0,0,0,0,ca)";

	// -------------------------------------------------------------------------
	// TEXTURES: weather icons
	// -------------------------------------------------------------------------
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

	// -------------------------------------------------------------------------
	// SELECTION NAMES
	// -------------------------------------------------------------------------
	protected static const string SEL_TIME_HOUR_TENS = "time_hourX_";
	protected static const string SEL_TIME_HOUR_ONES = "time_hour_X";
	protected static const string SEL_TIME_MIN_TENS  = "time_minuteX_";
	protected static const string SEL_TIME_MIN_ONES  = "time_minute_X";

	protected static const string SEL_TEMP_SIGN = "Weather_temper";
	protected static const string SEL_TEMP_TENS = "Weather_temper_numerX_";
	protected static const string SEL_TEMP_ONES = "Weather_temper_numer_X";

	protected static const string SEL_WEATHER_ICON = "Weather_icon";

	// -------------------------------------------------------------------------
	// WEATHER ICON CODES (SYNCED)
	// -------------------------------------------------------------------------
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

	// -------------------------------------------------------------------------
	// DISTANCE / PERFORMANCE
	// -------------------------------------------------------------------------
	protected static const float ACTIVE_RADIUS_M      = 150.0;
	protected static const int   PROXIMITY_CHECK_MS   = 2000;
	protected static const int   UPDATE_INTERVAL_MS   = 1000;

	// -------------------------------------------------------------------------
	// CACHE: hidden selection indices
	// -------------------------------------------------------------------------
	protected bool m_SelCacheInit;
	protected int m_SelTimeHourTens = -1;
	protected int m_SelTimeHourOnes = -1;
	protected int m_SelTimeMinTens  = -1;
	protected int m_SelTimeMinOnes  = -1;
	protected int m_SelTempSign     = -1;
	protected int m_SelTempTens     = -1;
	protected int m_SelTempOnes     = -1;
	protected int m_SelWeatherIcon  = -1;

	// -------------------------------------------------------------------------
	// SYNCED VALUES (server -> clients)
	// -------------------------------------------------------------------------
	protected bool m_SyncActive;
	protected int  m_SyncHour;
	protected int  m_SyncMinute;
	protected int  m_SyncTemp;
	protected int  m_SyncWeatherIcon;

	// -------------------------------------------------------------------------
	// SERVER STATE
	// -------------------------------------------------------------------------
	protected bool m_ServerActive;
	protected ref array<Man> m_PlayersCache;
	protected int m_LastUpdateMS;
	protected int m_LastSentHour   = -1;
	protected int m_LastSentMinute = -1;
	protected int m_LastSentTemp   = 9999;
	protected int m_LastSentWeatherIcon = -1;

	void SFP_Clock_Electronika_static()
	{
		m_PlayersCache = new array<Man>();
		RegisterNetSyncVariableBool("m_SyncActive");
		RegisterNetSyncVariableInt("m_SyncHour", 0, 23);
		RegisterNetSyncVariableInt("m_SyncMinute", 0, 59);
		RegisterNetSyncVariableInt("m_SyncTemp", -99, 99);
		RegisterNetSyncVariableInt("m_SyncWeatherIcon", 0, 9);
	}

	override void EEInit()
	{
		super.EEInit();
		InitSelectionCache();

		m_SyncActive = false;
		m_ServerActive = false;
		m_LastUpdateMS = 0;

		// Listen-server: погасить сразу визуально
		if (GetGame().IsClient() && !GetGame().IsDedicatedServer())
			ClearDisplay();

		if (GetGame().IsServer())
		{
			ProximityTick();
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.ProximityTick, PROXIMITY_CHECK_MS, true);
		}
	}

	override void EEDelete(EntityAI parent)
	{
		if (GetGame().IsServer())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(this.ServerTick);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(this.ProximityTick);
		}
		super.EEDelete(parent);
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		if (!GetGame().IsDedicatedServer())
			UpdateDisplayFromSync();
	}

	// -------------------------------------------------------------------------
	// SELECTION CACHE
	// -------------------------------------------------------------------------
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

	// -------------------------------------------------------------------------
	// PROXIMITY
	// -------------------------------------------------------------------------
	protected void ProximityTick()
	{
		if (!GetGame().IsServer())
			return;

		bool hasNear = HasNearbyPlayers();

		if (hasNear && !m_ServerActive)
		{
			m_ServerActive = true;
			m_SyncActive = true;
			m_LastUpdateMS = 0;

			ServerTick();
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.ServerTick, UPDATE_INTERVAL_MS, true);
			SetSynchDirty();

			if (GetGame().IsClient() && !GetGame().IsDedicatedServer())
				UpdateDisplayFromSync();
			return;
		}

		if (!hasNear && m_ServerActive)
		{
			m_ServerActive = false;
			m_SyncActive = false;
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(this.ServerTick);
			SetSynchDirty();

			if (GetGame().IsClient() && !GetGame().IsDedicatedServer())
				ClearDisplay();
		}
	}

	protected bool HasNearbyPlayers()
	{
		if (!m_PlayersCache)
			m_PlayersCache = new array<Man>();

		m_PlayersCache.Clear();
		GetGame().GetPlayers(m_PlayersCache);

		float rSq = Math.SqrFloat(ACTIVE_RADIUS_M);
		vector pos = GetPosition();

		for (int i = 0; i < m_PlayersCache.Count(); i++)
		{
			Man man = m_PlayersCache.Get(i);
			if (!man)
				continue;

			PlayerBase pb;
			if (Class.CastTo(pb, man))
			{
				if (!pb.IsAlive())
					continue;
			}

			if (vector.DistanceSq(pos, man.GetPosition()) <= rSq)
				return true;
		}

		return false;
	}

	// -------------------------------------------------------------------------
	// SERVER TICK
	// -------------------------------------------------------------------------
	protected void ServerTick()
	{
		if (!GetGame().IsServer())
			return;
		if (!m_ServerActive)
			return;

		int now = GetGame().GetTime();
		if ((now - m_LastUpdateMS) < UPDATE_INTERVAL_MS)
			return;
		m_LastUpdateMS = now;

		int y, mo, d, h, mi;
		GetGame().GetWorld().GetDate(y, mo, d, h, mi);

		int tempI = Math.Round(GetLocalAmbientTemperature());
		tempI = Math.Clamp(tempI, -99, 99);

		int iconCode = ComputeWeatherIconCode(h);

		bool changed = false;
		if (h != m_LastSentHour)                 { m_LastSentHour = h; changed = true; }
		if (mi != m_LastSentMinute)             { m_LastSentMinute = mi; changed = true; }
		if (tempI != m_LastSentTemp)            { m_LastSentTemp = tempI; changed = true; }
		if (iconCode != m_LastSentWeatherIcon)  { m_LastSentWeatherIcon = iconCode; changed = true; }

		if (changed)
		{
			m_SyncHour = h;
			m_SyncMinute = mi;
			m_SyncTemp = tempI;
			m_SyncWeatherIcon = iconCode;
			SetSynchDirty();
		}

		if (GetGame().IsClient() && !GetGame().IsDedicatedServer())
			UpdateDisplayFromSync();
	}

	// -------------------------------------------------------------------------
	// CLIENT: APPLY TEXTURES
	// -------------------------------------------------------------------------
	protected void UpdateDisplayFromSync()
	{
		if (GetGame().IsDedicatedServer())
			return;

		InitSelectionCache();
		if (!m_SyncActive)
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

	protected void UpdateWeatherIcon(int iconCode)
	{
		string tex = TEX_WEATHER_CLEAR_DAY;
		switch (iconCode)
		{
			case WICON_CLEAR_DAY:
				tex = TEX_WEATHER_CLEAR_DAY;
				break;
			case WICON_CLOUDY:
				tex = TEX_WEATHER_CLOUDY;
				break;
			case WICON_FEW_CLOUDS_DAY:
				tex = TEX_WEATHER_FEW_CLOUDS_DAY;
				break;
			case WICON_CLEAR_NIGHT:
				tex = TEX_WEATHER_CLEAR_NIGHT;
				break;
			case WICON_FEW_CLOUDS_NIGHT:
				tex = TEX_WEATHER_FEW_CLOUDS_NIGHT;
				break;
			case WICON_RAIN_NIGHT:
				tex = TEX_WEATHER_RAIN_NIGHT;
				break;
			case WICON_RAIN_DAY:
				tex = TEX_WEATHER_RAIN_DAY;
				break;
			case WICON_SHOWER_RAIN:
				tex = TEX_WEATHER_SHOWER_RAIN;
				break;
			case WICON_SNOW:
				tex = TEX_WEATHER_SNOW;
				break;
			case WICON_THUNDERSTORM:
				tex = TEX_WEATHER_THUNDERSTORM;
				break;
		}

		SetObjectTextureSafe(m_SelWeatherIcon, tex);
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

	// -------------------------------------------------------------------------
	// WEATHER LOGIC
	// -------------------------------------------------------------------------
	protected bool IsNightHour(int hour)
	{
		if (hour >= 20)
			return true;
		if (hour <= 5)
			return true;
		return false;
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

		if (snow > 0.15)
			return WICON_SNOW;

		if (rain > 0.10)
		{
			if (over > 0.85 && rain > 0.50)
				return WICON_THUNDERSTORM;
			if (rain > 0.70)
				return WICON_SHOWER_RAIN;
			if (isNight)
				return WICON_RAIN_NIGHT;
			return WICON_RAIN_DAY;
		}

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

	// -------------------------------------------------------------------------
	// TEMPERATURE (local incl. UTS)
	// -------------------------------------------------------------------------
	protected float GetLocalAmbientTemperature()
	{
		Mission mission = GetGame().GetMission();
		if (!mission)
			return 0.0;

		WorldData wd = mission.GetWorldData();
		if (!wd)
			return 0.0;

		vector pos = GetPosition();
		float baseTemp = wd.GetTemperature(this, EEnvironmentTemperatureComponent.ALTITUDE | EEnvironmentTemperatureComponent.OVERCAST | EEnvironmentTemperatureComponent.FOG);
		float utsTemp = GetUTSAverageTemperatureAt(pos);

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
