// «Вырезать трофей» (ТЗ 2.7), серверная часть: трофеи по таблице босса, урон инструменту, кровь на руках.
// Само действие (условия, время, анимация) — VB_AnomaliesBosses, VB_AB_Trophy.c.

modded class VB_AB_ActionCutTrophy
{
	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);

		VB_AB_BossBase boss = VB_AB_BossBase.Cast(action_data.m_Target.GetObject());
		if (!boss || !boss.VB_CarveServer(action_data.m_Player))
			return;		// второй резавший опоздал: тело уже разрезано

		if (action_data.m_MainItem)
			MiscGameplayFunctions.DealAbsoluteDmg(action_data.m_MainItem, boss.VB_GetToolDamage());
		PluginLifespan life = PluginLifespan.Cast(GetPlugin(PluginLifespan));
		if (life)
			life.UpdateBloodyHandsVisibility(action_data.m_Player, true);
	}
};
