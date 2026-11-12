// ============================================================================
// МОНОЛИТ (SFP_Artefact_Monolit) — НАСТРОЙКИ (КРУТИТЬ ЗДЕСЬ)
// Путь по твоему требованию:
//    scripts\4_World\SFP\Monolit_stone_art\SFP_MonolitStone_Tuning.c
//
// Это главный файл для регулировки всего эффекта:
//  - радиусы действия (звук / постпроц)
//  - когда начинается ЧБ и когда уже полностью ЧБ
//  - насколько «кашей» становится зерно у центра
//  - сила искажения «жар/шимер»
//  - свет (радиус/яркость/цвет) и memory point
//
// Важно: значения здесь пишутся в SFP_MonolitStone_Config и дальше используются
// клиент-менеджером и PPE requester'ом.
// ============================================================================

class SFP_MonolitStone_Tuning
{
	static void Apply()
	{
		// --------------------
		// РАДИУСЫ (метры) — КРУТИТЬ ТУТ
		// --------------------
		// Радиусы берутся из JSON ($profile:SFP/MonolitStoneSFP.json) на сервере
		// и синхронизируются на клиент через net sync переменные объекта.
		// Поэтому тут радиусы НЕ перетираем (чтобы не конфликтовать с серверной конфигурацией).

		// --------------------
		// Ч/Б (потеря насыщенности) — КРУТИТЬ ТУТ
		// --------------------
		SFP_MonolitStone_Config.SATURATION_START_DIST = 200.0; // [C] отсюда начинаем уводить в ЧБ (дальше — без изменений)
		SFP_MonolitStone_Config.SATURATION_FULL_DIST  = 5.0;   // [D] тут уже полностью ЧБ
		SFP_MonolitStone_Config.SATURATION_MIN        = 0.0;   // [E] 0 = ЧБ, 0.2 = «почти ЧБ», 1 = без изменений
		SFP_MonolitStone_Config.SATURATION_CURVE      = 1.8;   // [F] кривая: больше = эффект «взрывается» ближе к центру

		// --------------------
		// ЗЕРНО (FilmGrain) — КРУТИТЬ ТУТ
		// --------------------
		SFP_MonolitStone_Config.GRAIN_START_DIST      = 300.0; // [G] откуда начинает проявляться зерно
		SFP_MonolitStone_Config.GRAIN_FULL_DIST       = 5.0;   // [H] где зерно уже на максимуме
		SFP_MonolitStone_Config.GRAIN_CURVE           = 2.8;   // [I] кривая: больше = резче усиливается возле центра
		SFP_MonolitStone_Config.GRAIN_INTENSITY_MAX   = 1.0;   // [J] 0..1 — базовая интенсивность FilmGrain
		SFP_MonolitStone_Config.NOISE_MULT_MAX        = 50.0;  // [K] 0..50 — главный «усилитель каши» (это то, что ты просил)
		SFP_MonolitStone_Config.GRAIN_SHARPNESS_MAX   = 20.0;  // [L] 0..20 — резкость зерна
		SFP_MonolitStone_Config.GRAIN_SIZE_MIN        = 1.0;   // [M] 1..9 — размер зёрен (меньше = мельче)

		// --------------------
		// ИСКАЖЕНИЕ «жар/шимер» (WetDistort) — КРУТИТЬ ТУТ
		// --------------------
		SFP_MonolitStone_Config.DISTORT_START_DIST    = 0.0; // [N] откуда начинается искажение
		SFP_MonolitStone_Config.DISTORT_FULL_DIST     = 0.0;  // [O] где искажение уже на максимуме
		SFP_MonolitStone_Config.DISTORT_CURVE         = 0.0;   // [P] кривая
		SFP_MonolitStone_Config.DISTORT_WAVEAMP_MAX   = 0.0; // [Q] амплитуда волн (больше = сильнее «жарит»)
		SFP_MonolitStone_Config.DISTORT_BLURPOWER_MAX = 0.0;  // [R] размыливание в искажении

		// --------------------
		// СВЕТ (PointLight) — КРУТИТЬ ТУТ
		// --------------------
		// LIGHT_RADIUS тоже приходит из JSON и синхры (не перетираем здесь)
		SFP_MonolitStone_Config.LIGHT_BRIGHTNESS = 25.0;  // [T] яркость
		SFP_MonolitStone_Config.LIGHT_R = 0.15;           // [U] цвет (RGB 0..1)
		SFP_MonolitStone_Config.LIGHT_G = 0.55;           //     небесно-голубой ближе к синему
		SFP_MonolitStone_Config.LIGHT_B = 1.00;
		SFP_MonolitStone_Config.LIGHT_SHADOW = 0.0;       // как у костра/большого огня (без теней)
		SFP_MonolitStone_Config.LIGHT_MEMORYPOINT = "monolit_light"; // [V] точка из модели (memory point)

		// --------------------
		// ТЕПЛОВОЕ ИСКАЖЕНИЕ ОТ СВЕТА (ванильное) — КРУТИТЬ ТУТ
		// Пример из ванили: FireplaceLight / TorchLight / RoadflareLight
		// Важно: HeatHaze — это локальная «марева» вокруг источника света.
		// Если тебе нужно искажение на много метров/по радиусу — это уже PPE (WetDistort), не свет.
		// --------------------
		SFP_MonolitStone_Config.HEAT_HAZE_ENABLED = 1.0;  // [Y] 0=выкл, 1=вкл
		SFP_MonolitStone_Config.HEAT_HAZE_RADIUS  = 10.75; // [Z] «радиус марева» (ваниль: ~0.08..0.23)
		SFP_MonolitStone_Config.HEAT_HAZE_POWER   = 0.030;// [AA] сила искажения

		// --------------------
		// Саундсет (играет ЛОКАЛЬНО игроку, не от объекта) — КРУТИТЬ ТУТ
		// --------------------
		SFP_MonolitStone_Config.SOUNDSET_MONOLIT = "SFP_AnomalyMonolitStone_SoundSet"; // [W]

		// Частота обновления (мс) — КРУТИТЬ ТУТ
		// Меньше = плавнее/точнее, но дороже по производительности.
		SFP_MonolitStone_Config.UPDATE_INTERVAL_MS = 33; // [X] ~30 раз/сек
	}
}
