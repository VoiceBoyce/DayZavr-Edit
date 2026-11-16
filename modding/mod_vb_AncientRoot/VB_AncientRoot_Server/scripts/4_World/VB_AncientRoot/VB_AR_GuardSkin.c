// Разделка стража рощи (только сервер). Ванильное «Разделать» (ActionSkinning.OnFinishProgressServer -> SpawnItems):
// у стража — ванильный лут по конфигу (шкура, мясо, трофеи MSFZ) и сверху свой: клык волку, шанс на отросток и пыльцу
// (VB_RootGuards.VB_SkinServer, VB_AR_RootGuards.c). Призванный страж (волна за дурную славу, волк светоча, стая
// Хранителя — VB_RootGuards.VB_Summoned) не даёт ничего: тело рассыпается трухой (иначе ферма клыков и трофеев).
// У всех остальных (животные, тела игроков, чужие мутанты) — как в ванили.
// Разрешение разделки Хранителя (он по классу зомби) — клиентский VB_AR_Guard.c.

modded class ActionSkinning
{
	override void SpawnItems(ActionData action_data)
	{
		EntityAI body;
		if (action_data && action_data.m_Target)
			body = EntityAI.Cast(action_data.m_Target.GetObject());
		if (VB_RootGuards.VB_Is(body) && VB_RootGuards.VB_Summoned(body))
		{
			// ванильный OnFinishProgressServer после SpawnItems сам удалит тело
			PlayerBase p = action_data.m_Player;
			if (p)
				NotificationSystem.SendNotificationToPlayerExtended(p, 5, "Тело рассыпалось трухой", "Призванный лесом страж не оставляет добычи.");
			Print("[VB_AncientRoot] разделал призванного стража " + body.GetType() + ": " + VB_AR_ItemsServer.VB_Who(p) + " — без добычи");
			return;
		}

		super.SpawnItems(action_data);

		if (VB_RootGuards.VB_Is(body))
			VB_RootGuards.VB_SkinServer(body, action_data.m_Player);
	}
}
