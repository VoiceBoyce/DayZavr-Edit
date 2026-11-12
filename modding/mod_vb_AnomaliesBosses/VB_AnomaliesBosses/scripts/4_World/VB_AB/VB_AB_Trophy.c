// «Вырезать трофей» (ТЗ 2.7): по образцу вырезания сердца Изнанки (VB_ActionCutRiftHeart).
// Резать можно всем, чем в ванили разделывают туши (у предмета есть ActionSkinning), сидя.
// Время — Trophy.CutTimeSec босса из bosses.json (синхронизируется на клиент, чтобы полоска совпала с сервером).
// Одно тело — одна разделка: трофеи в инвентарь или на землю рядом, тело исчезает, руки в крови, инструмент
// теряет Trophy.ToolDamage здоровья. Сама выдача — VB_AB_BossBase.VB_CarveServer.

class VB_AB_ActionCutTrophyCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		float t = VB_AB_ActionCutTrophy.DEFAULT_TIME;
		VB_AB_BossBase boss = VB_AB_BossBase.Cast(m_ActionData.m_Target.GetObject());
		if (boss && boss.VB_GetCutTime() > 0)
			t = boss.VB_GetCutTime();
		m_ActionData.m_ActionComponent = new CAContinuousTime(t);
	}
};

class VB_AB_ActionCutTrophy : ActionContinuousBase
{
	static const float DEFAULT_TIME = 15.0;

	void VB_AB_ActionCutTrophy()
	{
		m_CallbackClass = VB_AB_ActionCutTrophyCB;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_ANIMALSKINNING;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_FullBody = true;
		m_Text = "#STR_VB_AB_ActionCutTrophy";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined();
		m_ConditionTarget = new CCTCursorNoRuinCheck(UAMaxDistances.DEFAULT);	// мёртвое тело «разрушено»
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		VB_AB_BossBase boss = VB_AB_BossBase.Cast(target.GetObject());
		return boss && !boss.IsAlive() && !boss.VB_IsCarved();
	}

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

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(VB_AB_ActionCutTrophy);
	}
};

modded class ItemBase
{
	// всё, чем можно свежевать, годится и для трофея (тот же приём, что у Изнанки)
	override void AddAction(typename actionName)
	{
		super.AddAction(actionName);
		if (actionName == ActionSkinning)
			super.AddAction(VB_AB_ActionCutTrophy);
	}
};
