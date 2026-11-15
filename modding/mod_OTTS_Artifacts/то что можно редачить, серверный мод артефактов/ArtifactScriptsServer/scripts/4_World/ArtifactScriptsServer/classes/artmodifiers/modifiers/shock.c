// DayZavr 2026-11-14: артефакты больше не вырубают сами.
// Ваниль восстанавливает шок 5/с в сознании и 1/с без сознания, отключка при 25, пробуждение при 50.
// Раньше минус 20 и больше за 3 с вырубал навсегда, а любой минус не давал очнуться после нокаута.
// Теперь минус к шоку не опускает его ниже SHOCK_FLOOR и не действует без сознания:
// он только делает игрока уязвимее в бою (меньше запас до отключки).
class ArtModifierShock: ArtModifierBase
{
	protected const float SHOCK_FLOOR = 30;

	override void Process()
	{
		float shock = m_Player.GetHealth("GlobalHealth", "Shock");
		float add = m_Value * GetRelativeValue();

		if (add < 0)
		{
			if (m_Player.IsUnconscious() || shock <= SHOCK_FLOOR)
				return;

			shock = Math.Max(shock + add, SHOCK_FLOOR);
		}
		else
		{
			shock += add;
		}

		m_Player.SetHealth("GlobalHealth", "Shock", Math.Clamp(shock, 0, 100));
	}
}