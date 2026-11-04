// DayZavr: скриптовые баффы предметов set_cod2_vb. Файл сгенерирован, правки вносить через генератор.
// Каждая копия в 12 модах проверяет только свой сет (vbSet = "set_cod2"), поэтому эффекты не складываются.

class VBC_set_cod2
{
	static const string SET = "set_cod2";
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
		TStringArray slots = {"Mask", "Body", "Vest", "Hips", "Legs", "Feet"};
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
		if (VBC_set_cod2.IsMine(this))
			weight -= GetInventoryAndCargoWeight(forceRecalc) * (1 - VBC_set_cod2.WEIGHT_CONTENTS);
		return weight;
	}
};

// Мягкое приземление: обувь _vb уменьшает эффективную высоту падения
modded class DayZPlayerImplementFallDamage
{
	override void HandleFallDamage(FallDamageData pData)
	{
		PlayerBase player = PlayerBase.Cast(m_Player);
		if (pData && player && VBC_set_cod2.IsMine(player.FindAttachmentBySlotName("Feet")))
			pData.m_Height = pData.m_Height * VBC_set_cod2.FALL_HEIGHT;
		super.HandleFallDamage(pData);
	}
};

// Сетовый бонус: флаг пересчитывается при надевании и снятии вещей (сервер и клиент)
modded class PlayerBase
{
	protected bool m_VBC_set_cod2_Set;

	bool VBC_set_cod2_SetActive()
	{
		return m_VBC_set_cod2_Set;
	}

	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);
		m_VBC_set_cod2_Set = VBC_set_cod2.IsSetComplete(this);
	}

	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);
		m_VBC_set_cod2_Set = VBC_set_cod2.IsSetComplete(this, item);
	}
};

modded class HungerMdfr
{
	override void OnTick(PlayerBase player, float deltaT)
	{
		if (player && player.VBC_set_cod2_SetActive())
			deltaT = deltaT * VBC_set_cod2.SET_HUNGER_THIRST;
		super.OnTick(player, deltaT);
	}
};

modded class ThirstMdfr
{
	override void OnTick(PlayerBase player, float deltaT)
	{
		if (player && player.VBC_set_cod2_SetActive())
			deltaT = deltaT * VBC_set_cod2.SET_HUNGER_THIRST;
		super.OnTick(player, deltaT);
	}
};

modded class BloodRegenMdfr
{
	override void OnTick(PlayerBase player, float deltaT)
	{
		if (player && player.VBC_set_cod2_SetActive())
			deltaT = deltaT * VBC_set_cod2.SET_BLOOD;
		super.OnTick(player, deltaT);
	}
};

modded class HealthRegenMdfr
{
	override void OnTick(PlayerBase player, float deltaT)
	{
		if (player && player.VBC_set_cod2_SetActive())
			deltaT = deltaT * VBC_set_cod2.SET_HEALTH;
		super.OnTick(player, deltaT);
	}
};

// Сетовый бонус: в полном сете _vb газ не вызывает химических ожогов ни на одной части тела
// (закрывает и те части, для которых в сете нет вещи: шея, руки, голова под шлемом без маски)
modded class BleedingSourcesManagerServer
{
	override bool AttemptAddBleedingSourceDirectly(int bit, eBleedingSourceType type = eBleedingSourceType.NORMAL, int context = 0)
	{
		if (type == eBleedingSourceType.CONTAMINATED && m_Player && m_Player.VBC_set_cod2_SetActive())
			return false;
		return super.AttemptAddBleedingSourceDirectly(bit, type, context);
	}
};

modded class StaminaHandler
{
	override protected void ProcessStaminaChanges(float deltaT, bool isServerOrSingleplayer)
	{
		if (m_Player && m_Player.VBC_set_cod2_SetActive())
		{
			if (m_StaminaDelta < 0)
				m_StaminaDelta = m_StaminaDelta * VBC_set_cod2.SET_STAMINA;
			m_StaminaDepletion = m_StaminaDepletion * VBC_set_cod2.SET_STAMINA;
		}
		super.ProcessStaminaChanges(deltaT, isServerOrSingleplayer);
	}
};
