// SFP Vodka effects — общие константы
//
// ► СЮДА ТЫ ЛЕЗЕШЬ В ПЕРВУЮ ОЧЕРЕДЬ, ЧТОБЫ «КРУТИТЬ» БАЛАНС:
//   1) Как быстро напиваться и как долго держится эффект
//   2) Насколько жёстко режется стамина (реген / максимум / расход)
//   3) Шанс вырубиться (удар по шоку)
//   4) Согревание / комфорт пока пьян
//
// ВАЖНО: скрипт привязан к ЖИДКОСТИ, а не к бутылке.
// Если в бутылку налить воду/что-то другое — эффекта не будет.

class SFP_VodkaConstants
{
	// Маска (ID) жидкости, которая считается «наша водка».
	// Тут стоит ЧИСЛО, которое ты добавляешь в config.cpp в CfgLiquidDefinitions (type = ...)
	// и которое вписываешь в бутылку в liquidContainerType / varLiquidTypeInit.
	static const int VODKA_LIQUID_MASK = 33554432; // СВОЙ liquid ID (должен совпадать с config.cpp)

	// Сколько «доз» в полной бутылке.
	// Чем больше — тем быстрее растёт уровень опьянения за один и тот же объём.
	static const float DOSES_PER_BOTTLE = 6.0;

	// Сколько секунд «сгорает» одна доза.
	// Чем меньше — тем быстрее протрезвевает. Чем больше — тем дольше держит.
	static const float SECONDS_PER_DOSE = 60.0;

	// Мягкий потолок по дозам. 18 доз = 3 бутылки (если DOSES_PER_BOTTLE = 6).
	// Если хочешь, чтобы можно было «нажраться» сильнее — увеличивай.
	static const float MAX_LEVEL = 18.0;

	// До какого значения усиливаем эффекты.
	// 1.0 ≈ одна полная бутылка, 2.0 ≈ две, 3.0 ≈ три.
	static const float INTENSITY_MAX = 3.0;

	// --- Стамина (сильно режем) ---
	// Множитель восстановления стамины (1.0 = как обычно).
	// При 1 бутылке стремимся к STAMINA_RECOVERY_AT_1_BOTTLE,
	// при «нажрался» стремимся к STAMINA_RECOVERY_MIN.
	static const float STAMINA_RECOVERY_AT_1_BOTTLE = 0.35;
	static const float STAMINA_RECOVERY_MIN = 0.05;

	// Множитель МАКСИМАЛЬНОЙ стамины (cap) (1.0 = как обычно).
	// При 1 бутылке → STAMINA_CAP_AT_1_BOTTLE, при сильной пьянке → STAMINA_CAP_MIN.
	static const float STAMINA_CAP_AT_1_BOTTLE = 0.55;
	static const float STAMINA_CAP_MIN = 0.15;

	// Множитель РАСХОДА стамины (>1 = стамина тратится быстрее при беге/драке и т.п.).
	static const float STAMINA_DEPLETE_AT_1_BOTTLE = 1.70;
	static const float STAMINA_DEPLETE_MAX = 3.10;

	// Сколько секунд плавно «затухают» визуальные эффекты после окончания.
	static const float VISUAL_FADE_OUT_SEC = 2.0;

	// --- Риск бессознанки (удар по шоку) ---
	// С какого уровня начинаем бросать «шанс вырубиться».
	// По умолчанию = 1 бутылка.
	static const float UNCON_START_LEVEL = DOSES_PER_BOTTLE;
	// Шанс вырубиться на ~1 бутылке.
	static const float UNCON_PROB_AT_1_BOTTLE = 0.20;
	// Шанс вырубиться на ~2 бутылках.
	static const float UNCON_PROB_AT_2_BOTTLES = 0.45;
	// Максимальный шанс (потолок).
	static const float UNCON_PROB_MAX = 0.70;
	// Насколько сильно бьём по шоку (чем больше — тем вероятнее бессознанка).
	static const float UNCON_SHOCK_HIT_MIN = 15.0;
	static const float UNCON_SHOCK_HIT_MAX = 60.0;

	// --- Согревание / комфорт пока пьян (сервер) ---
	// HeatBuffer: 0..1 (полоска тепла/буфера). HeatComfort: примерно -1..1.
	// Делается небольшим, чтобы не было имбы.
	static const float HEATBUFFER_GAIN_PER_SEC_AT_1_BOTTLE = 0.006;
	static const float HEATBUFFER_GAIN_MAX_PER_SEC = 0.012;
	static const float HEATCOMFORT_GAIN_PER_SEC_AT_1_BOTTLE = 0.020;
	static const float HEATCOMFORT_GAIN_MAX_PER_SEC = 0.040;

	// Примечание: писк/шум в ушах (tinnitus) УБРАН по просьбе.
}
