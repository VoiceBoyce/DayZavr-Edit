// «Вырезать сердце» Изнанки: долгое действие присев, любым инструментом со свежеванием
// (ножи, топоры, тесаки, пилы, штыки), только ночью. Как только начал — Изнанка в 50 м бьётся чаще, вылезает волна
// зомби (VB_RiftHorde). Сердце гаснет до рестарта, вырезавший получает «Сердце Изнанки»,
// у всех игроков бьёт красная молния (VB_RiftStrike).

class VB_ActionCutRiftHeartCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(VB_ActionCutRiftHeart.CUT_TIME);
	}
}

class VB_ActionCutRiftHeart : ActionContinuousBase
{
	static const float CUT_TIME = 15.0;		// секунд держать
	static const float TOOL_DAMAGE = 15.0;	// урон инструменту за одно сердце

	void VB_ActionCutRiftHeart()
	{
		m_CallbackClass = VB_ActionCutRiftHeartCB;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_ANIMALSKINNING;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_FullBody = true;
		m_Text = "Вырезать сердце";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined();
		m_ConditionTarget = new CCTCursor(UAMaxDistances.DEFAULT);
	}

	// только ночью, когда Изнанка горит в полную силу (днём и в сумерках сердце спит — резать нечего)
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		VB_RiftHeart heart = VB_RiftHeart.Cast(target.GetObject());
		if (!heart || heart.VB_IsDead())
			return false;

		return VB_RiftPulse.VB_NightFactor() >= VB_RiftPulse.NIGHT_FULL_ABOVE;
	}

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

// «Собрать споры»: тряпка в руках, у живого сердца ночью. Тряпка уходит (из пачки — одна), игрок получает
// «Споры Изнанки». У сердца SPORES_MAX порций за рестарт. Сердце просыпается и шумит, но волны зомби нет.

class VB_ActionCollectSporesCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(VB_ActionCollectSpores.COLLECT_TIME);
	}
}

class VB_ActionCollectSpores : ActionContinuousBase
{
	static const float COLLECT_TIME = 6.0;
	static const string SPORES_TYPE = "VB_RiftSpores";

	void VB_ActionCollectSpores()
	{
		m_CallbackClass = VB_ActionCollectSporesCB;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_FullBody = true;
		m_Text = "Собрать споры";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined();
		m_ConditionTarget = new CCTCursor(UAMaxDistances.DEFAULT);
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		VB_RiftHeart heart = VB_RiftHeart.Cast(target.GetObject());
		if (!heart || heart.VB_IsDead() || heart.VB_SporesLeft() <= 0)
			return false;

		return VB_RiftPulse.VB_NightFactor() >= VB_RiftPulse.NIGHT_FULL_ABOVE;
	}

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

		VB_RiftItems.VB_DropAtFeet(action_data.m_Player, SPORES_TYPE);
		if (action_data.m_MainItem)
			action_data.m_MainItem.AddQuantity(-1);
	}
}

// «Отломить корень»: нож (всё, чем свежуют) в руках, рядом живой корень гнезда (VB_RiftRoots_*), ночью.
// Цель — ближайший корень в ROOT_DIST от игрока, не прицел: «ручка» корня у самой земли, в неё трудно навести.
// Корень гаснет до рестарта (второй раз с него не взять), игрок получает «Корень Изнанки», рядом шум.

class VB_ActionBreakRiftRootCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(VB_ActionBreakRiftRoot.BREAK_TIME);
	}
}

class VB_ActionBreakRiftRoot : ActionContinuousBase
{
	static const float BREAK_TIME = 8.0;
	static const float ROOT_DIST = 3.0;		// м от игрока до места, где корень выходит из земли
	static const float TOOL_DAMAGE = 4.0;
	static const string ROOT_TYPE = "VB_RiftRootPiece";
	static const string NOISE = "VB_RiftChunkNoise";	// тихий треск: зомби рядом проверят, деревня не сбегается
	static const float NOISE_TIME = 6.0;

	void VB_ActionBreakRiftRoot()
	{
		m_CallbackClass = VB_ActionBreakRiftRootCB;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_MEDIUM;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_ANIMALSKINNING;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_FullBody = true;
		m_Text = "Отломить корень";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined();
		m_ConditionTarget = new CCTNone();
	}

	override bool HasTarget()
	{
		return false;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (VB_RiftPulse.VB_NightFactor() < VB_RiftPulse.NIGHT_FULL_ABOVE)
			return false;

		if (g_Game.IsDedicatedServer())
			return VB_FindRootServer(player.GetPosition()) != null;

		return VB_RiftPulse.VB_NearestLiveRoot(player.GetPosition(), ROOT_DIST) != null;
	}

	// сервер: пульса тут нет, ищем корень среди объектов рядом (связь с сердцем знает только клиент)
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
			if (!root || !root.VB_IsGroundRoot() || root.VB_IsBroken())
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
		VB_RiftItems.VB_NoiseAt(root.GetPosition(), NOISE, NOISE_TIME);
		VB_RiftItems.VB_DropAtFeet(action_data.m_Player, ROOT_TYPE);
		if (action_data.m_MainItem)
			action_data.m_MainItem.AddHealth("", "", -TOOL_DAMAGE);
	}
}

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(VB_ActionCutRiftHeart);
		actions.Insert(VB_ActionCollectSpores);
		actions.Insert(VB_ActionBreakRiftRoot);
	}
}

modded class ItemBase
{
	// всё, чем можно свежевать, годится, чтобы вырезать сердце и отломить корень Изнанки
	override void AddAction(typename actionName)
	{
		super.AddAction(actionName);
		if (actionName == ActionSkinning)
		{
			super.AddAction(VB_ActionCutRiftHeart);
			super.AddAction(VB_ActionBreakRiftRoot);
		}
	}
}

modded class Rag
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(VB_ActionCollectSpores);
	}
}
