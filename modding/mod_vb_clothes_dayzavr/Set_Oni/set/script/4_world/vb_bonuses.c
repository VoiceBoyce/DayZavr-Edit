// DayZavr: скриптовые баффы предметов Set_Oni_vb. Файл сгенерирован, правки вносить через генератор.
// Каждая копия в 12 модах проверяет только свой сет (vbSet = "Set_Oni"), поэтому эффекты не складываются.

class VBC_Set_Oni
{
	static const string SET = "Set_Oni";
	static const float WEIGHT_CONTENTS = 0.5;
	static const float FALL_HEIGHT = 0.6;
	static const float SET_HUNGER_THIRST = 0.7;
	static const float SET_BLOOD = 1.5;
	static const float SET_HEALTH = 1.5;
	static const float SET_STAMINA = 0.75;

	static bool IsMine(EntityAI item)
	{
		return item && item.ConfigGetString("vbSet") == SET;
	}

	// полный сет: во всех слотах сета надеты вещи _vb этого сета
	static bool IsSetComplete(PlayerBase player, EntityAI excluded = null)
	{
		if (!player)
			return false;
		TStringArray slots = {"Mask", "Body", "Vest", "Hips", "Legs", "Gloves", "Feet"};
		foreach (string slot : slots)
		{
			EntityAI item = player.FindAttachmentBySlotName(slot);
			if (!item || item == excluded || !IsMine(item))
				return false;
		}
		return true;
	}
};

// Облегчённая переноска: содержимое одежды _vb весит меньше
modded class Clothing
{
	override protected float GetWeightSpecialized(bool forceRecalc = false)
	{
		float weight = super.GetWeightSpecialized(forceRecalc);
		if (VBC_Set_Oni.IsMine(this))
			weight -= GetInventoryAndCargoWeight(forceRecalc) * (1 - VBC_Set_Oni.WEIGHT_CONTENTS);
		return weight;
	}
};

// Мягкое приземление: обувь _vb уменьшает эффективную высоту падения
modded class DayZPlayerImplementFallDamage
{
	override void HandleFallDamage(FallDamageData pData)
	{
		PlayerBase player = PlayerBase.Cast(m_Player);
		if (pData && player && VBC_Set_Oni.IsMine(player.FindAttachmentBySlotName("Feet")))
			pData.m_Height = pData.m_Height * VBC_Set_Oni.FALL_HEIGHT;
		super.HandleFallDamage(pData);
	}
};

// Сетовый бонус: флаг пересчитывается при надевании и снятии вещей (сервер и клиент)
modded class PlayerBase
{
	protected bool m_VBC_Set_Oni_Set;

	bool VBC_Set_Oni_SetActive()
	{
		return m_VBC_Set_Oni_Set;
	}

	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);
		m_VBC_Set_Oni_Set = VBC_Set_Oni.IsSetComplete(this);
	}

	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);
		m_VBC_Set_Oni_Set = VBC_Set_Oni.IsSetComplete(this, item);
	}
};

modded class HungerMdfr
{
	override void OnTick(PlayerBase player, float deltaT)
	{
		if (player && player.VBC_Set_Oni_SetActive())
			deltaT = deltaT * VBC_Set_Oni.SET_HUNGER_THIRST;
		super.OnTick(player, deltaT);
	}
};

modded class ThirstMdfr
{
	override void OnTick(PlayerBase player, float deltaT)
	{
		if (player && player.VBC_Set_Oni_SetActive())
			deltaT = deltaT * VBC_Set_Oni.SET_HUNGER_THIRST;
		super.OnTick(player, deltaT);
	}
};

modded class BloodRegenMdfr
{
	override void OnTick(PlayerBase player, float deltaT)
	{
		if (player && player.VBC_Set_Oni_SetActive())
			deltaT = deltaT * VBC_Set_Oni.SET_BLOOD;
		super.OnTick(player, deltaT);
	}
};

modded class HealthRegenMdfr
{
	override void OnTick(PlayerBase player, float deltaT)
	{
		if (player && player.VBC_Set_Oni_SetActive())
			deltaT = deltaT * VBC_Set_Oni.SET_HEALTH;
		super.OnTick(player, deltaT);
	}
};

// Сетовый бонус: в полном сете _vb газ не вызывает химических ожогов ни на одной части тела
// (закрывает и те части, для которых в сете нет вещи: шея, руки, голова под шлемом без маски)
modded class BleedingSourcesManagerServer
{
	override bool AttemptAddBleedingSourceDirectly(int bit, eBleedingSourceType type = eBleedingSourceType.NORMAL, int context = 0)
	{
		if (type == eBleedingSourceType.CONTAMINATED && m_Player && m_Player.VBC_Set_Oni_SetActive())
			return false;
		return super.AttemptAddBleedingSourceDirectly(bit, type, context);
	}
};

modded class StaminaHandler
{
	override protected void ProcessStaminaChanges(float deltaT, bool isServerOrSingleplayer)
	{
		if (m_Player && m_Player.VBC_Set_Oni_SetActive())
		{
			if (m_StaminaDelta < 0)
				m_StaminaDelta = m_StaminaDelta * VBC_Set_Oni.SET_STAMINA;
			m_StaminaDepletion = m_StaminaDepletion * VBC_Set_Oni.SET_STAMINA;
		}
		super.ProcessStaminaChanges(deltaT, isServerOrSingleplayer);
	}
};
