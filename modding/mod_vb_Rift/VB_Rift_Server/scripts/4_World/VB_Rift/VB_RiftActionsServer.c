// Серверная часть действий у гнезда Изнанки: резка сердца (волна, вырезание), сбор спор, отлом корня.
// Сами действия (условия, время, анимации, регистрация) — VB_Rift, VB_RiftActions.c.

modded class VB_ActionCutRiftHeart
{
	// начал резать: Изнанка вокруг просыпается и бьётся чаще, первая попытка поднимает волну зомби
	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);

		VB_RiftHeart heart = VB_RiftHeart.Cast(action_data.m_Target.GetObject());
		if (heart)
			heart.VB_DisturbServer(action_data.m_Player);
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);

		VB_RiftHeart heart = VB_RiftHeart.Cast(action_data.m_Target.GetObject());
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

		VB_RiftHeart heart = VB_RiftHeart.Cast(action_data.m_Target.GetObject());
		if (heart)
			heart.VB_DisturbServer(action_data.m_Player, false);
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);

		VB_RiftHeart heart = VB_RiftHeart.Cast(action_data.m_Target.GetObject());
		if (!heart || !heart.VB_TakeSporesServer())
			return;

		VB_RiftItemsServer.VB_DropAtFeet(action_data.m_Player, SPORES_TYPE);
		if (action_data.m_MainItem)
			action_data.m_MainItem.AddQuantity(-1);
	}
}

modded class VB_ActionBreakRiftRoot
{
	// сервер: клиентская часть проверила ночь, здесь — есть ли рядом живой корень
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!super.ActionCondition(player, target, item))
			return false;

		return VB_FindRootServer(player.GetPosition()) != null;
	}

	// Пульса на сервере нет: ищем корень гнезда среди объектов рядом. Отломанные и спящие (ближайшее сердце
	// вырезано, VB_IsDormantServer) не годятся: иначе изменённый клиент отламывал корни мёртвого гнезда (№39).
	static VB_RiftBase VB_FindRootServer(vector pos)
	{
		array<Object> objs = new array<Object>;
		g_Game.GetObjectsAtPosition(pos, ROOT_DIST + 0.5, objs, null);
		VB_RiftBase best;
		VB_RiftBase root;
		float bestSq = (ROOT_DIST + 0.5) * (ROOT_DIST + 0.5);
		float dSq;
		foreach (Object o : objs)
		{
			root = VB_RiftBase.Cast(o);
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

		VB_RiftBase root = VB_FindRootServer(action_data.m_Player.GetPosition());
		if (!root)
			return;

		root.VB_BreakServer();
		VB_RiftItemsServer.VB_NoiseAt(root.GetPosition(), NOISE, NOISE_TIME);
		VB_RiftItemsServer.VB_DropAtFeet(action_data.m_Player, ROOT_TYPE);
		if (action_data.m_MainItem)
			action_data.m_MainItem.AddHealth("", "", -TOOL_DAMAGE);
	}
}
