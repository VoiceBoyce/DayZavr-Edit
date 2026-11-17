// VB_MiningResources: классы предметов и объектов (общая часть клиента и сервера).
// Логика перенесена 1:1 из BRDK_GoldRush. Серверная часть (результаты, пресс, плавка, настройки)
// лежит в VB_MiningResources_Server. Старые классы BRDK_* в конфиге наследуют эти.

// ---------------------------------------------------------------- жила
class VB_MR_Vein_Base extends House
{
	// время добычи из настроек, сервер присылает его клиенту
	float m_VB_MR_MineTime;

	void VB_MR_Vein_Base()
	{
		RegisterNetSyncVariableFloat("m_VB_MR_MineTime");
	}
}

// ---------------------------------------------------------------- ресурсы
class VB_MR_Nugget_Base extends ItemBase {}		// самородок металла: плавится в чаше в слиток
class VB_MR_Sand_Base extends ItemBase {}		// песок металла: плавится в чаше в самородок
class VB_MR_StoneNugget_Base extends ItemBase {}	// кусок камня: прессуется в брикет
class VB_MR_StoneSand_Base extends ItemBase {}	// песок камня: прессуется в кусок
class VB_MR_MetalBar_Base extends ItemBase {}	// слиток
class VB_MR_StoneBar_Base extends ItemBase {}	// брикет (угольный горит в костре)

// ---------------------------------------------------------------- инструмент добычи и сковорода
modded class ItemBase
{
	// предмет из списка MineTools настроек, сервер присылает признак клиенту
	bool m_VB_MR_IsMineTool;

	override void InitItemVariables()
	{
		super.InitItemVariables();
		RegisterNetSyncVariableBool("m_VB_MR_IsMineTool");
	}

	override void SetActions()
	{
		super.SetActions();
		AddAction(VB_MR_ActionMine);
	}
}

modded class FryingPan
{
	// время промывки из настроек, сервер присылает его клиенту
	float m_VB_MR_PanTime;

	override void InitItemVariables()
	{
		super.InitItemVariables();
		RegisterNetSyncVariableFloat("m_VB_MR_PanTime");
	}

	override void SetActions()
	{
		super.SetActions();
		AddAction(VB_MR_ActionPan);
	}
}

// ---------------------------------------------------------------- плавильная чаша
// Своего скриптового класса у чаши нет: костёр пускает на треногу только предмет с типом ровно Cauldron
// (Fireplace.c сравнивает item.Type()). Поэтому чаша — это Cauldron, а признак берётся из конфига.
modded class Cauldron
{
	protected bool m_VB_MR_IsMeltingBowl;

	void Cauldron()
	{
		m_VB_MR_IsMeltingBowl = IsKindOf("VB_MR_MeltingBowl");
	}

	bool VB_MR_IsMeltingBowl()
	{
		return m_VB_MR_IsMeltingBowl;
	}

	// в хранилище чаши кладётся только металл
	override bool CanReceiveItemIntoCargo(EntityAI item)
	{
		if (!super.CanReceiveItemIntoCargo(item))
			return false;

		if (!m_VB_MR_IsMeltingBowl)
			return true;

		return item.IsKindOf("VB_MR_Nugget_Base") || item.IsKindOf("VB_MR_Sand_Base") || item.IsKindOf("VB_MR_MetalBar_Base");
	}
}

// ---------------------------------------------------------------- набор пресса
// Голограмму пресса задаёт параметр projectionTypename в конфиге набора
class VB_MR_Press_Kit extends ItemBase
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionTogglePlaceObject);
		AddAction(ActionPlaceObject);
	}

	override bool IsBasebuildingKit()
	{
		return true;
	}

	override bool IsDeployable()
	{
		return true;
	}

	override void OnPlacementComplete(Man player, vector position = "0 0 0", vector orientation = "0 0 0")
	{
		super.OnPlacementComplete(player, position, orientation);

		if (GetGame().IsServer())
		{
			EntityAI press = EntityAI.Cast(GetGame().CreateObjectEx("VB_MR_Press", position, ECE_PLACE_ON_SURFACE));
			if (press)
			{
				press.SetPosition(position);
				press.SetOrientation(orientation);
			}
			Delete();
		}
	}
}

// ---------------------------------------------------------------- пресс
class VB_MR_Press extends Container_Base
{
	// пресс работает: инвентарь закрыт, пока не выйдет результат
	protected bool m_VB_MR_IsPressing;

	void VB_MR_Press()
	{
		RegisterNetSyncVariableBool("m_VB_MR_IsPressing");
	}

	override bool IsInventoryVisible()
	{
		if (!super.IsInventoryVisible())
			return false;

		return !m_VB_MR_IsPressing;
	}

	// в слоте полный стак целого камня или песка
	bool VB_MR_HasFullStack()
	{
		ItemBase stone = ItemBase.Cast(FindAttachmentBySlotName("BRDK_STONES"));
		if (!stone || stone.IsRuined())
			return false;

		return stone.GetQuantity() == stone.GetQuantityMax();
	}

	override bool CanPutInCargo(EntityAI parent)
	{
		return false;
	}

	override bool CanPutIntoHands(EntityAI parent)
	{
		return false;
	}

	override void SetActions()
	{
		super.SetActions();
		AddAction(VB_MR_ActionPress);
	}
}

modded class Pliers
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(VB_MR_ActionDismantlePress);
	}
}

// ---------------------------------------------------------------- брикет в костре
// Брикеты горят как топливо в слоте дров (энергия 500, как в старом моде)
modded class FireplaceBase
{
	void FireplaceBase()
	{
		if (m_FireConsumableTypes && !m_FireConsumableTypes.Contains(VB_MR_StoneBar_Base))
		{
			m_FireConsumableTypes.Insert(VB_MR_StoneBar_Base, new FireConsumableType(VB_MR_StoneBar_Base, 500, false, "Firewood"));
		}
	}
}

// ---------------------------------------------------------------- как предметы держатся в руках
modded class ModItemRegisterCallbacks
{
	override void RegisterTwoHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
	{
		super.RegisterTwoHanded(pType, pBehavior);
		pType.AddItemInHandsProfileIK("VB_MR_MetalBar_Base", "dz/anims/workspaces/player/player_main/player_main_2h.asi", pBehavior, "dz/anims/anm/player/ik/two_handed/stone.anm");
		pType.AddItemInHandsProfileIK("VB_MR_MeltingBowl", "dz/anims/workspaces/player/player_main/props/player_main_2h_pot.asi", pBehavior, "dz/anims/anm/player/ik/two_handed/Cauldron.anm");
	}

	override void RegisterOneHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
	{
		super.RegisterOneHanded(pType, pBehavior);
		pType.AddItemInHandsProfileIK("VB_MR_StoneBar_Base", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/meat_steak.anm");
		pType.AddItemInHandsProfileIK("VB_MR_Nugget_Base", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/SmallStone.anm");
		pType.AddItemInHandsProfileIK("VB_MR_StoneNugget_Base", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/SmallStone.anm");
		pType.AddItemInHandsProfileIK("VB_MR_Sand_Base", "dz/anims/workspaces/player/player_main/props/player_main_1h_compass.asi", pBehavior, "dz/anims/anm/player/ik/gear/compass.anm");
		pType.AddItemInHandsProfileIK("VB_MR_StoneSand_Base", "dz/anims/workspaces/player/player_main/props/player_main_1h_compass.asi", pBehavior, "dz/anims/anm/player/ik/gear/compass.anm");
	}

	override void RegisterHeavy(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
	{
		super.RegisterHeavy(pType, pBehavior);
		pType.AddItemInHandsProfileIK("VB_MR_Press_Kit", "dz/anims/workspaces/player/player_main/player_main_heavy.asi", pBehavior, "dz/anims/anm/player/ik/heavy/wooden_crate.anm");
	}
}
