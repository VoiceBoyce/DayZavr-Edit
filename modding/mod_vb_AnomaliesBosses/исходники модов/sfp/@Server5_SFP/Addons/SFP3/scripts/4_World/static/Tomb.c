/*class SFP_Static_TombBase : ItemBase
{
	ref array<string> m_RandomLoot = {
		"AKM",
		"GlassBottle",
		"SVD",
		"AmmoBox_762x39_20Rnd",
		"Canteen"
	};

	override bool CanPutIntoHands(EntityAI parent)
	{
		return false; // запрет взять в руки
	}

	override bool CanReceiveItemIntoCargo(EntityAI item)
	{
		if (IsOpen())
			return super.CanReceiveItemIntoCargo(item);
		return false;
	}

	// <<< ВАЖНО >>>
	// Не вызываем super.SetActions(), чтобы убрать стандартное "взять"
	override void SetActions()
	{
		AddAction(ActionInspectGrave);
	}
};


// ---------------------------------------------
// Новое действие: "Осмотреть"
// ---------------------------------------------
class ActionInspectGrave : ActionContinuousBase
{
	void ActionInspectGrave()
	{
		m_CallbackClass = ActionInspectGraveCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_FullBody = true;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_LOW;
		m_Text = "Осмотреть";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTObject(UAMaxDistances.DEFAULT);
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		return SFP_Static_TombBase.Cast(target.GetObject()) != null;
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		PlayerBase player = action_data.m_Player;
		SFP_Static_TombBase tomb = SFP_Static_TombBase.Cast(action_data.m_Target.GetObject());
		if (!tomb) return;

		int randomIndex = Math.RandomInt(0, tomb.m_RandomLoot.Count());
		string lootType = tomb.m_RandomLoot.Get(randomIndex);

		vector pos = player.GetPosition();
		GetGame().CreateObjectEx(lootType, pos, ECE_PLACE_ON_SURFACE);
		player.MessageStatus("Вы нашли что-то в могиле...");
	}
};

class ActionInspectGraveCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(10.0); // 10 секунд
	}
};


// ---------------------------------------------
// Регистрация классов крестов
// ---------------------------------------------
class SFP_Static_tomb_01 : SFP_Static_TombBase {};
class SFP_Static_tomb_02 : SFP_Static_TombBase {};
class SFP_Static_tomb_03 : SFP_Static_TombBase {};
class SFP_Static_tomb_04 : SFP_Static_TombBase {};
class SFP_Static_tomb_05 : SFP_Static_TombBase {};
class SFP_Static_tomb_06 : SFP_Static_TombBase {};

*/

class SFP_Static_TombBase : ItemBase
{
	
	override bool CanPutIntoHands( EntityAI parent )
	{
		if ( !super.CanPutIntoHands( parent ))
		return false;
		return false;
	}
	override bool CanReceiveItemIntoCargo( EntityAI item )
	{
		if ( IsOpen() )
		return super.CanReceiveItemIntoCargo( item );
		return false;
	}
	
	//----------------------------------------------------------
};

class SFP_Static_tomb_01 : SFP_Static_TombBase {};
class SFP_Static_tomb_02 : SFP_Static_TombBase {};
class SFP_Static_tomb_03 : SFP_Static_TombBase {};
class SFP_Static_tomb_04 : SFP_Static_TombBase {};
class SFP_Static_tomb_05 : SFP_Static_TombBase {};
class SFP_Static_tomb_06 : SFP_Static_TombBase {};










