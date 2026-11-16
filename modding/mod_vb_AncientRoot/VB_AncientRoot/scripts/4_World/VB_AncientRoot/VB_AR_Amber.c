// Янтарь (ТЗ_сервер_18.md п. 2б.5) — клиентская часть: действие «Расколоть».
//   Окаменевшая сердцевина (VB_AR_HeartDead) → несколько кусков янтаря, иногда с вкраплением.
//   Янтарь с вкраплением (VB_AR_AmberInclusion) → руна или кристалл MSFZ.
// Как: положить на землю, взять в руки молоток, топор, кирку или булаву, присесть рядом.
// Что выпадает и сколько — сервер (VB_AncientRoot_Server, VB_AR_AmberServer.c). Простой янтарь (дар рощи) не колется.

class VB_AR_AmberSplit
{
	static const string DEAD_CORE = "VB_AR_HeartDead";
	static const string INCLUSION = "VB_AR_AmberInclusion";

	// чем колоть: ударный или рубящий инструмент (ваниль + молот и кирка MSFZ)
	static bool VB_IsTool(EntityAI item)
	{
		if (!item)
			return false;
		if (item.IsKindOf("Hammer") || item.IsKindOf("SledgeHammer") || item.IsKindOf("MeatTenderizer") || item.IsKindOf("Mace"))
			return true;
		if (item.IsKindOf("Hatchet") || item.IsKindOf("WoodAxe") || item.IsKindOf("FirefighterAxe"))
			return true;
		if (item.IsKindOf("Pickaxe") || item.IsKindOf("Iceaxe") || item.IsKindOf("Crowbar"))
			return true;
		return item.IsKindOf("MSFZ_Magic_Hammer") || item.IsKindOf("MSFZ_Magic_Kirka");
	}

	// что колоть: окаменевшая сердцевина или янтарь с вкраплением, лежит на земле (не в инвентаре и не в контейнере)
	static bool VB_CanSplit(Object o)
	{
		ItemBase it = ItemBase.Cast(o);
		if (!it || it.GetHierarchyParent())
			return false;
		return it.IsKindOf(DEAD_CORE) || it.IsKindOf(INCLUSION);
	}
}

class VB_ActionSplitAmberCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(VB_ActionSplitAmber.SPLIT_TIME);
	}
}

class VB_ActionSplitAmber : ActionContinuousBase
{
	static const float SPLIT_TIME = 5.0;

	void VB_ActionSplitAmber()
	{
		m_CallbackClass = VB_ActionSplitAmberCB;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_LOW;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_FullBody = true;
		m_Text = "Расколоть";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined();
		m_ConditionTarget = new CCTNonRuined(UAMaxDistances.DEFAULT);
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!target || !VB_AR_AmberSplit.VB_IsTool(item))
			return false;
		return VB_AR_AmberSplit.VB_CanSplit(target.GetObject());
	}
}

modded class ItemBase
{
	// инструментов много и они ванильные — действие у всех предметов, отбор в условии (как у подношения)
	override void SetActions()
	{
		super.SetActions();
		AddAction(VB_ActionSplitAmber);
	}
}

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(VB_ActionSplitAmber);
	}
}
