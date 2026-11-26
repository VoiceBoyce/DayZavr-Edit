// VB_MiningResources: классы предметов и объектов (общая часть клиента и сервера).
// Серверная часть (результаты, пресс, плавка, настройки) лежит в VB_MiningResources_Server.
// Старые классы BRDK_* в конфиге наследуют эти.

// Состояние станка (пресс, чаша), его видит игрок в подсказке
const int VB_MR_STATION_IDLE = 0;
const int VB_MR_STATION_WORK = 1;

// набор пресса ставится не дальше этого от игрока, м (голограмма ванили — до 6 м)
const float VB_MR_KIT_MAX_DISTANCE = 7.0;

// ---------------------------------------------------------------- жила
// Время добычи берётся из данных сервера (VB_MR_Shared), своих полей у жилы нет
class VB_MR_Vein_Base extends House
{
}

// ---------------------------------------------------------------- ресурсы
class VB_MR_Nugget_Base extends ItemBase {}		// самородок металла: плавится в чаше в слиток
class VB_MR_Sand_Base extends ItemBase {}		// песок металла: плавится в чаше в самородок
class VB_MR_StoneNugget_Base extends ItemBase {}	// кусок камня: прессуется в брикет
class VB_MR_StoneSand_Base extends ItemBase {}	// песок камня: прессуется в кусок
class VB_MR_MetalBar_Base extends ItemBase {}	// слиток
class VB_MR_StoneBar_Base extends ItemBase {}	// брикет (угольный горит в костре)

// ---------------------------------------------------------------- инструмент добычи и сковорода
// Действие есть у любого предмета, а можно ли им добывать — решает список инструментов с сервера
modded class ItemBase
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(VB_MR_ActionMine);
	}
}

modded class FryingPan
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(VB_MR_ActionPan);
	}
}

// ---------------------------------------------------------------- плавильная чаша
// Своего скриптового класса у чаши нет: костёр пускает на треногу только предмет с типом ровно Cauldron
// (Fireplace.c сравнивает item.Type(), E83). Поэтому чаша — это Cauldron, а признак берётся из конфига.
modded class Cauldron
{
	protected bool m_VB_MR_IsMeltingBowl;
	protected int m_VB_MR_State;			// VB_MR_STATION_*
	protected float m_VB_MR_Done;			// сколько уже плавится порция, с
	protected float m_VB_MR_Total;			// сколько всего плавить порцию, с

	void Cauldron()
	{
		m_VB_MR_IsMeltingBowl = IsKindOf("VB_MR_MeltingBowl");
		if (m_VB_MR_IsMeltingBowl)
		{
			RegisterNetSyncVariableInt("m_VB_MR_State", 0, 1);
			RegisterNetSyncVariableFloat("m_VB_MR_Done", 0, 100000, 1);
			RegisterNetSyncVariableFloat("m_VB_MR_Total", 0, 100000, 1);
		}
	}

	bool VB_MR_IsMeltingBowl()
	{
		return m_VB_MR_IsMeltingBowl;
	}

	int VB_MR_GetState()
	{
		return m_VB_MR_State;
	}

	float VB_MR_GetDone()
	{
		return m_VB_MR_Done;
	}

	float VB_MR_GetTotal()
	{
		return m_VB_MR_Total;
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

	// подсказка о плавке раньше ванильных действий: иначе её закрывает «Взять» (видна только прокруткой)
	override void SetActions()
	{
		AddAction(VB_MR_ActionStationInfo);
		super.SetActions();
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

	// позицию присылает игрок; ванильная голограмма уходит не дальше 6 м — дальше значит подмена (сквозь стену, на другой конец карты)
	override bool CanBePlaced(Man player, vector position)
	{
		if (!super.CanBePlaced(player, position))
			return false;
		if (!player)
			return true;

		return vector.DistanceSq(player.GetPosition(), position) <= VB_MR_KIT_MAX_DISTANCE * VB_MR_KIT_MAX_DISTANCE;
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
			// набор удалит сама ваниль в конце действия (ActionDeployObject.OnEndServer), как у забора
			HideAllSelections();
		}
	}
}

// ---------------------------------------------------------------- пресс
class VB_MR_Press extends Container_Base
{
	protected int m_VB_MR_State;			// VB_MR_STATION_*: работает — инвентарь закрыт
	protected float m_VB_MR_Done;
	protected float m_VB_MR_Total;

	// только у игрока: звук и дым, пока пресс работает
	protected EffectSound m_VB_MR_Sound;
	protected Particle m_VB_MR_Smoke;

	void VB_MR_Press()
	{
		RegisterNetSyncVariableInt("m_VB_MR_State", 0, 1);
		RegisterNetSyncVariableFloat("m_VB_MR_Done", 0, 100000, 1);
		RegisterNetSyncVariableFloat("m_VB_MR_Total", 0, 100000, 1);
	}

	int VB_MR_GetState()
	{
		return m_VB_MR_State;
	}

	float VB_MR_GetDone()
	{
		return m_VB_MR_Done;
	}

	float VB_MR_GetTotal()
	{
		return m_VB_MR_Total;
	}

	ItemBase VB_MR_GetStone()
	{
		return ItemBase.Cast(FindAttachmentBySlotName("BRDK_STONES"));
	}

	// в слоте целый камень или песок, которого хватает на порцию
	bool VB_MR_CanStart()
	{
		if (m_VB_MR_State != VB_MR_STATION_IDLE)
			return false;

		ItemBase stone = VB_MR_GetStone();
		if (!stone || stone.IsRuined())
			return false;

		VB_MR_RecipeInfo recipe = VB_MR_Shared.FindRecipe(stone);
		return recipe && stone.GetQuantity() >= recipe.Portion;
	}

	override bool IsInventoryVisible()
	{
		if (!super.IsInventoryVisible())
			return false;

		return m_VB_MR_State == VB_MR_STATION_IDLE;
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
		AddAction(VB_MR_ActionStationInfo);
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		if (!GetGame().IsDedicatedServer())
			VB_MR_UpdateEffects();
	}

	protected void VB_MR_UpdateEffects()
	{
		if (m_VB_MR_State == VB_MR_STATION_WORK)
		{
			if (!m_VB_MR_Sound)
				PlaySoundSetLoop(m_VB_MR_Sound, "powerGeneratorLoop_SoundSet", 0.5, 0.5);
			if (!m_VB_MR_Smoke)
				// дым генератора (у smoking_barrel в 1.30 нет материала — частица не рисуется)
				m_VB_MR_Smoke = ParticleManager.GetInstance().PlayOnObject(ParticleList.POWER_GENERATOR_SMOKE, this, "0 1.1 0");
		}
		else
		{
			VB_MR_StopEffects();
		}
	}

	protected void VB_MR_StopEffects()
	{
		if (m_VB_MR_Sound)
			StopSoundSet(m_VB_MR_Sound);
		if (m_VB_MR_Smoke)
		{
			m_VB_MR_Smoke.StopParticle();
			m_VB_MR_Smoke = null;
		}
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		if (!GetGame().IsDedicatedServer())
			VB_MR_StopEffects();
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

	// Тяжёлый набор пресса - не через RegisterHeavy (№ 90, 2026-11-26): у модов машин эта цепочка длиннее предела
	// стека Enforce (Stack overflow), её конец не вызывается. RegisterCustom короткая; «тяжёлое» поведение - как у
	// ванили (heavyItemBehaviour.SetHeavyItems()); свой предмет - до super.
	override void RegisterCustom(DayZPlayerType pType)
	{
		DayzPlayerItemBehaviorCfg heavy = new DayzPlayerItemBehaviorCfg;
		heavy.SetHeavyItems();
		pType.AddItemInHandsProfileIK("VB_MR_Press_Kit", "dz/anims/workspaces/player/player_main/player_main_heavy.asi", heavy, "dz/anims/anm/player/ik/heavy/wooden_crate.anm");
		super.RegisterCustom(pType);
	}
}
