// SFP NonStop energy drink - shared constants
class SFP_NonStopConstants
{
	// Unique ID for our custom modifier (must not collide with vanilla eModifiers values)
	static const int MDF_NONSTOP_STAMINA_REGEN = 7001;


	// Уникальный ключ для ускорения восстановления стамины (enum-ключ, но под капотом int).
	static const EStaminaMultiplierTypes STAMINA_RECOVERY_TYPE = 8003;

	// Balance values
	static const float STAMINA_RECOVERY_MULTIPLIER = 3; // В 3 раза быстрее восстановление стамины пока активен бафф
	static const float ENERGY_DRAIN_MULTIPLIER = 1.20;     // 20% faster hunger (energy) drain while buff is active

	// Duration per one standard "drink sip" (UAQuantityConsumed.DRINK)
	static const float BUFF_SECONDS_PER_SIP = 10.0;
}
