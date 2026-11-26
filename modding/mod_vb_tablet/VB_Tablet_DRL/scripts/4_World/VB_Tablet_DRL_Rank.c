// Ранг планшета в крафте DRL_CraftMenu (сервер, 2026-11-26): рецепты с "RequiredRank" в CraftConfig.json открываются
// с этой ступени ранга. Ступень = номер ранга в планшете («Ступень 5 из 10» = Боец, 10 000 опыта по ranks.json),
// названия — те же, что на странице ранга (#STR_VB_TAB_RANKN_*). Опыт считается так же, как для создания клана.
modded class DRL_RankProvider
{
	override int Level(PlayerBase player)
	{
		if (!player || !player.GetIdentity())
			return 0;
		PlayerIdentity id = player.GetIdentity();
		VB_TabletRecord rec = VB_TabletPlayers.Get(id.GetPlainId(), id.GetName());
		if (!rec)
			return 0;
		VB_TabletRates rates = VB_TabletConfig.Get().Rates;
		int xp = rec.ToStats().Xp(rates);
		return rates.RankOf(xp) + 1;
	}

	override void Names(array<string> names)
	{
		for (int i = 0; i < VB_RANK_COUNT; i++)
		{
			names.Insert("#STR_VB_TAB_RANKN_" + i.ToString());
		}
	}
}
