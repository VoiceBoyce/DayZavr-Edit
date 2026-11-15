// Разделка стража рощи (только сервер). Ванильное «Разделать» (ActionSkinning.OnFinishProgressServer -> SpawnItems):
// у стража — ванильный лут по конфигу (шкура, мясо, трофеи MSFZ) и сверху свой шанс на отросток и пыльцу
// (VB_RootGuards.VB_SkinServer, VB_AR_RootGuards.c). У всех остальных (животные, тела игроков, чужие мутанты) — как в ванили.
// Разрешение разделки Хранителя (он по классу зомби) — клиентский VB_AR_Guard.c.

modded class ActionSkinning
{
	override void SpawnItems(ActionData action_data)
	{
		super.SpawnItems(action_data);

		EntityAI body;
		if (action_data && action_data.m_Target)
			body = EntityAI.Cast(action_data.m_Target.GetObject());
		if (VB_RootGuards.VB_Is(body))
			VB_RootGuards.VB_SkinServer(body, action_data.m_Player);
	}
}
