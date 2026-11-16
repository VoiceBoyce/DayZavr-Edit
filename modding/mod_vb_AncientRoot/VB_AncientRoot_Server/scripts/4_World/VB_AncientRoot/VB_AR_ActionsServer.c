// Серверная часть действий у рощи: резка сердцевины (тревога и волна, вырезание), сбор пыльцы, отлом отростка
// (тревога +1). Подношение — VB_AR_GroveServer.c.
// Сами действия (условия, время, анимации, регистрация) — VB_AncientRoot, VB_AR_Actions.c.

modded class VB_ActionCutRootHeart
{
	// начал резать: Изнанка вокруг просыпается и бьётся чаще, первая попытка поднимает волну зомби
	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);

		VB_AR_Heart heart = VB_AR_Heart.Cast(action_data.m_Target.GetObject());
		if (heart)
			heart.VB_DisturbServer(action_data.m_Player);
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);

		VB_AR_Heart heart = VB_AR_Heart.Cast(action_data.m_Target.GetObject());
		if (!heart || heart.VB_IsDead())
			return;

		heart.VB_KillServer(action_data.m_Player);
		if (action_data.m_MainItem)
			action_data.m_MainItem.AddHealth("", "", -TOOL_DAMAGE);
	}
}

modded class VB_ActionCollectSpores
{
	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);

		VB_AR_Heart heart = VB_AR_Heart.Cast(action_data.m_Target.GetObject());
		if (heart)
			heart.VB_DisturbServer(action_data.m_Player, false);
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);

		VB_AR_Heart heart = VB_AR_Heart.Cast(action_data.m_Target.GetObject());
		if (!heart || !heart.VB_TakeSporesServer())
			return;

		VB_AR_ItemsServer.VB_DropAtFeet(action_data.m_Player, SPORES_TYPE);
		if (action_data.m_MainItem)
			action_data.m_MainItem.AddQuantity(-1);
		heart.VB_AlarmServer(action_data.m_Player, 1);		// тревога рощи +1 (VB_AR_GroveServer.c)
	}
}

modded class VB_ActionBreakRootRoot
{
	// сервер: клиентская часть проверила ночь, здесь — есть ли рядом живой корень
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!super.ActionCondition(player, target, item))
			return false;

		VB_AR_Base root = VB_FindRootServer(player.GetPosition());
		if (!root)
			return false;
		// во время нашествия нечисти сбор у рощи закрыт (ТЗ 2в п.1)
		VB_AR_Heart heart = VB_AR_Heart.VB_NearestServer(root.GetPosition(), VB_AR_Pulse.LINK_DIST);
		return !heart || !heart.VB_UnderSiege();
	}

	// Пульса на сервере нет: ищем корень гнезда среди объектов рядом. Отломанные и спящие (ближайшее сердце
	// вырезано, VB_IsDormantServer) не годятся: иначе изменённый клиент отламывал корни мёртвого гнезда (№39).
	static VB_AR_Base VB_FindRootServer(vector pos)
	{
		array<Object> objs = new array<Object>;
		g_Game.GetObjectsAtPosition(pos, ROOT_DIST + 0.5, objs, null);
		VB_AR_Base best;
		VB_AR_Base root;
		float bestSq = (ROOT_DIST + 0.5) * (ROOT_DIST + 0.5);
		float dSq;
		foreach (Object o : objs)
		{
			root = VB_AR_Base.Cast(o);
			if (!root || !root.VB_IsGroundRoot() || root.VB_IsBroken() || root.VB_IsDormantServer())
				continue;
			dSq = vector.DistanceSq(pos, root.GetPosition());
			if (dSq < bestSq)
			{
				bestSq = dSq;
				best = root;
			}
		}
		return best;
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);

		VB_AR_Base root = VB_FindRootServer(action_data.m_Player.GetPosition());
		if (!root)
			return;

		root.VB_BreakServer();
		VB_AR_ItemsServer.VB_NoiseAt(root.GetPosition(), NOISE, NOISE_TIME);
		// тревога рощи +1 у сердцевины этого корня (VB_AR_GroveServer.c)
		VB_AR_Heart heart = VB_AR_Heart.VB_NearestServer(root.GetPosition(), VB_AR_Pulse.LINK_DIST);
		if (heart)
			heart.VB_AlarmServer(action_data.m_Player, 1);
		VB_AR_ItemsServer.VB_DropAtFeet(action_data.m_Player, ROOT_TYPE);
		if (action_data.m_MainItem)
			action_data.m_MainItem.AddHealth("", "", -TOOL_DAMAGE);
	}
}
