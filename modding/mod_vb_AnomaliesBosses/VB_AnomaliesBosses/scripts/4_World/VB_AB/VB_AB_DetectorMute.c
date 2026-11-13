// Тихий режим детектора (ТЗ 8.20): «Без звука» — писк, щелчки и звук находки выключены, экран и лампа работают,
// поэтому звук не выдаёт игрока. «Со звуком» — обратно. Детектор в руках, включён он или нет — не важно.
// Флаг m_VB_Mute синхронизируется (VB_AB_DetectorBase), в сохранение не пишется: после рестарта снова со звуком.

class VB_AB_ActionDetectorMuteBase extends ActionSingleUseBase
{
	void VB_AB_ActionDetectorMuteBase()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_ITEM_ON;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_ITEM_ON;
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

	override bool HasProneException()
	{
		return true;
	}

	// какое состояние ставит действие
	protected bool VB_MuteTo()
	{
		return true;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		VB_AB_DetectorBase d = VB_AB_DetectorBase.Cast(item);
		return d && d.VB_IsMuted() != VB_MuteTo();
	}

	override void OnExecuteServer(ActionData action_data)
	{
		VB_AB_DetectorBase d = VB_AB_DetectorBase.Cast(action_data.m_MainItem);
		if (d)
			d.VB_SetMuted(VB_MuteTo());
	}
};

class VB_AB_ActionDetectorMute extends VB_AB_ActionDetectorMuteBase
{
	void VB_AB_ActionDetectorMute()
	{
		m_Text = "#STR_VB_AB_ActionMute";
	}
};

class VB_AB_ActionDetectorUnmute extends VB_AB_ActionDetectorMuteBase
{
	void VB_AB_ActionDetectorUnmute()
	{
		m_Text = "#STR_VB_AB_ActionUnmute";
	}

	override protected bool VB_MuteTo()
	{
		return false;
	}
};

// Без регистрации здесь AddAction в SetActions молча не работает (действие не создаётся).
modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(VB_AB_ActionDetectorMute);
		actions.Insert(VB_AB_ActionDetectorUnmute);
	}
};
