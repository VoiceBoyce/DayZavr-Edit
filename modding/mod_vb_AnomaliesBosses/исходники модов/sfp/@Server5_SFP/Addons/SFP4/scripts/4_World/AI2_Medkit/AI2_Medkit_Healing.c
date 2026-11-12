// AI2 Medkit - helper functions
// Server-side healing tick helpers for SFP_AI2_medkit_standart

void AI2_Medkit_AddHealth(PlayerBase player, float value)
{
	if (!player)
		return;

	// Safety: player might be in the process of disconnecting/despawning
	if (!player.IsAlive())
		return;

	player.AddHealth("", "Health", value);
}

void AI2_Medkit_ApplyHealing(PlayerBase player, float amount = 1.0)
{
	if (!player)
		return;

	// Instant heal
	AI2_Medkit_AddHealth(player, 20.0 * amount);

	// Heal-over-time: +2 health every second for 5 seconds
	for (int i = 1; i <= 5; i++)
	{
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AI2_Medkit_AddHealth, i * 1000, false, player, 2.0 * amount);
	}
}
