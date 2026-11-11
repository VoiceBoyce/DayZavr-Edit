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

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(VB_ActionCutRiftHeart);
	}
}

modded class ItemBase
{
	// всё, чем можно свежевать, годится и чтобы вырезать сердце Изнанки
	override void AddAction(typename actionName)
	{
		super.AddAction(actionName);
		if (actionName == ActionSkinning)
			super.AddAction(VB_ActionCutRiftHeart);
	}
}
