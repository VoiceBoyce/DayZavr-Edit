// ============================================================================
// МОНОЛИТ — общий конфиг (модуль 3_Game)
//
// Обычно ЭТОТ файл не трогают.
// Все параметры ты регулируешь в:
//   scripts\4_World\SFP\Monolit_stone_art\SFP_MonolitStone_Tuning.c
//
// Зачем нужен этот файл:
//  - его видят и 3_Game (PPE requester), и 4_World (объект/менеджер)
//  - tuning.Apply() переписывает эти static-поля на старте клиента
// ============================================================================

class SFP_MonolitStone_Config
{
	// --------------------
	// РАДИУСЫ (метры)
	// --------------------
	static float EFFECT_RADIUS				= 300.0; // общий радиус влияния (PPE)
	static float SOUND_RADIUS				= 300.0;

	// Ч/Б (потеря насыщенности -> чёрно-белое)
	static float SATURATION_START_DIST		= 200.0; // с этой дистанции начинаем уводить в ЧБ (дальше — без изменений)
	static float SATURATION_FULL_DIST		= 5.0;   // на этой дистанции и ближе — уже полностью ЧБ
	static float SATURATION_MIN				= 0.0;   // 0 = B&W
	static float SATURATION_CURVE			= 1.8;   // больше = сильнее «взрывается» возле центра

	// Зерно (FilmGrain) (важно: используем NVLIGHTPARAMS noise mult > 0)
	static float GRAIN_START_DIST			= 300.0;
	static float GRAIN_FULL_DIST			= 5.0;
	static float GRAIN_CURVE				= 2.4;
	static float GRAIN_INTENSITY_MAX		= 1.0;   // PPEFilmGrain.PARAM_INTENSITY (0..1)
	static float GRAIN_SHARPNESS_MAX		= 20.0;  // PPEFilmGrain.PARAM_SHARPNESS (0..20)
	static float GRAIN_SIZE_MIN				= 1.0;   // PPEFilmGrain.PARAM_GRAINSIZE (1..9) меньше = мельче зерно
	static float NOISE_MULT_MAX				= 35.0;  // PPELightIntensityParamsNative.PARAM_NOISE_MULT (0..50)

	// Искажение (WetDistort) — «жар/шимер»
	static float DISTORT_START_DIST			= 250.0;
	static float DISTORT_FULL_DIST			= 25.0;
	static float DISTORT_CURVE				= 1.6;
	static float DISTORT_EFFPOWER_MAX		= 1.0;
	static float DISTORT_WAVEAMP_MAX		= 0.020; // амплитуда волн (чем больше, тем сильнее «жар»)
	static float DISTORT_BLURPOWER_MAX		= 0.20;

	// --------------------
	// СВЕТ + ЗВУК
	// --------------------
	static float LIGHT_RADIUS				= 100.0;
	static float LIGHT_BRIGHTNESS			= 25.0;
	static float LIGHT_FADE_OUT_TIME		= 1.0;
	static float LIGHT_SHADOW				= 0.0;   // 0=false, 1=true
	static float LIGHT_R						= 0.15;
	static float LIGHT_G						= 0.55;
	static float LIGHT_B						= 1.00;

	static string LIGHT_MEMORYPOINT			= "monolit_light";
	
	// ТЕПЛОВОЕ ИСКАЖЕНИЕ ОТ СВЕТА (как у костра/факела)
	// ВНИМАНИЕ: это «локальный» heat-haze вокруг источника света, НЕ на весь радиус 100м.
	static float HEAT_HAZE_ENABLED			= 1.0;   // 0=false, 1=true
	static float HEAT_HAZE_RADIUS			= 0.35;  // типичные ванильные значения 0.08..0.23, тут можно больше
	static float HEAT_HAZE_POWER			= 0.020; // сила «жара»/искажения
static string SOUNDSET_MONOLIT			= "SFP_AnomalyMonolitStone_SoundSet";

	// Интервал обновления менеджера (клиент)
	static int UPDATE_INTERVAL_MS			= 33; // ~30 раз/сек

	// --------------------
	// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ
	// --------------------
	static float StrengthFromDistance(float dist, float startDist, float fullDist, float curve)
	{
		if (startDist <= fullDist)
			return 0.0;

		float t = Math.Clamp(Math.InverseLerp(startDist, fullDist, dist), 0.0, 1.0); // 0 на startDist, 1 на fullDist
		if (curve > 0.0)
			t = Math.Pow(t, curve);
		return t;
	}

	static float Lerp(float a, float b, float t)
	{
		return a + (b - a) * t;
	}
}
