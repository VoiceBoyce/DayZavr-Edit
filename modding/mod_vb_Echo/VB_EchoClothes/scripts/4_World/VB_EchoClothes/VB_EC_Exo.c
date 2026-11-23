// Экзоскелет Эхо (VB_EchoClothes). Этап 2 (11-23, ТЗ_VB_EchoClothes.md раздел 6): бонусы скриптом по схеме сетов одежды
// DayZavr (vb_bonuses.c у сетов), на шаг сильнее. Множители — config.cpp, class CfgVB_EchoClothes Exo.
// Вещи комплекта — с меткой vbExo=1 (7 штук). Сервопривод (vbExoServo) в комплект не входит — отдельный модуль на
// энергии Эхо, его бонусы — этап 4. С бонусами сетов не складывается: у сетов своя метка vbSet и свои слоты.

class VB_EC_Exo
{
	static float s_ContentsWeight = 0.4;
	static float s_SetOwnWeight = 0.4;
	static float s_FallHeight = 0.5;
	static float s_SetHungerThirst = 0.6;
	static float s_SetBlood = 1.75;
	static float s_SetHealth = 1.75;
	static float s_SetStamina = 0.6;
	protected static ref TStringArray s_SetSlots;

	// числа из конфига (один раз); нет ключа — остаётся значение по умолчанию выше
	static void Load()
	{
		if (s_SetSlots)
			return;
		s_SetSlots = new TStringArray;
		string p = "CfgVB_EchoClothes Exo ";
		GetGame().ConfigGetTextArray(p + "setSlots", s_SetSlots);
		s_ContentsWeight = VB_ReadFloat(p + "contentsWeight", s_ContentsWeight);
		s_SetOwnWeight = VB_ReadFloat(p + "setOwnWeight", s_SetOwnWeight);
		s_FallHeight = VB_ReadFloat(p + "fallHeight", s_FallHeight);
		s_SetHungerThirst = VB_ReadFloat(p + "setHungerThirst", s_SetHungerThirst);
		s_SetBlood = VB_ReadFloat(p + "setBlood", s_SetBlood);
		s_SetHealth = VB_ReadFloat(p + "setHealth", s_SetHealth);
		s_SetStamina = VB_ReadFloat(p + "setStamina", s_SetStamina);
	}

	protected static float VB_ReadFloat(string path, float def)
	{
		if (!GetGame().ConfigIsExisting(path))
			return def;
		return GetGame().ConfigGetFloat(path);
	}

	static bool IsExo(EntityAI item)
	{
		return item && item.ConfigGetBool("vbExo");
	}

	// полный комплект: во всех слотах комплекта — вещи экзо (excluded — вещь, которую как раз снимают)
	static bool IsSetComplete(PlayerBase player, EntityAI excluded = null)
	{
		if (!player)
			return false;
		Load();
		if (s_SetSlots.Count() == 0)
			return false;
		foreach (string slot : s_SetSlots)
		{
			EntityAI item = player.FindAttachmentBySlotName(slot);
			if (!item || item == excluded || !IsExo(item))
				return false;
		}
		return true;
	}

	// комплект собрался или распался — пересчитать вес надетых вещей экзо (кэш веса движка)
	static void MarkWeights(PlayerBase player)
	{
		if (!player)
			return;
		Load();
		foreach (string slot : s_SetSlots)
		{
			EntityAI item = player.FindAttachmentBySlotName(slot);
			if (item && IsExo(item))
				item.SetWeightDirty();
		}
	}
};

// Каркас: карманы вещи экзо весят s_ContentsWeight; в полном комплекте и сама надетая вещь — s_SetOwnWeight
modded class Clothing
{
	protected int m_VB_EC_Exo = -1;		// кэш метки vbExo (-1 — ещё не читали)

	bool VB_EC_IsExo()
	{
		if (m_VB_EC_Exo < 0)
		{
			m_VB_EC_Exo = 0;
			if (VB_EC_Exo.IsExo(this))
				m_VB_EC_Exo = 1;
		}
		return m_VB_EC_Exo == 1;
	}

	override protected float GetWeightSpecialized(bool forceRecalc = false)
	{
		float weight = super.GetWeightSpecialized(forceRecalc);
		if (!VB_EC_IsExo())
			return weight;
		VB_EC_Exo.Load();
		float contents = GetInventoryAndCargoWeight(forceRecalc);
		float own = weight - contents;
		PlayerBase wearer = PlayerBase.Cast(GetHierarchyParent());
		if (wearer && wearer.VB_EC_ExoSetActive())
			own = own * VB_EC_Exo.s_SetOwnWeight;
		return own + contents * VB_EC_Exo.s_ContentsWeight;
	}
};

// Мягкое приземление: ботинки экзо уменьшают высоту падения
modded class DayZPlayerImplementFallDamage
{
	override void HandleFallDamage(FallDamageData pData)
	{
		PlayerBase player = PlayerBase.Cast(m_Player);
		if (pData && player && VB_EC_Exo.IsExo(player.FindAttachmentBySlotName("Feet")))
		{
			VB_EC_Exo.Load();
			float mul = VB_EC_Exo.s_FallHeight;
			// полный экзоскелет (комплект + работающий сервопривод) — ещё мягче
			if (player.VB_EC_ExoSetActive() && VB_EC_Servo.Active(player))
			{
				VB_EC_Servo.Load();
				mul = VB_EC_Servo.s_FallFull;
			}
			pData.m_Height = pData.m_Height * mul;
		}
		super.HandleFallDamage(pData);
	}
};

// Полный комплект: флаг пересчитывается при надевании и снятии вещей (сервер и клиент)
modded class PlayerBase
{
	protected bool m_VB_EC_ExoSet;

	bool VB_EC_ExoSetActive()
	{
		return m_VB_EC_ExoSet;
	}

	protected void VB_EC_UpdateExoSet(EntityAI excluded)
	{
		bool was = m_VB_EC_ExoSet;
		m_VB_EC_ExoSet = VB_EC_Exo.IsSetComplete(this, excluded);
		if (was == m_VB_EC_ExoSet)
			return;
		VB_EC_Exo.MarkWeights(this);
#ifdef DIAG_DEVELOPER
		// стенд: видно, что комплект собрался/распался и как изменился вес (на сервере — без клиента)
		if (GetGame().IsDedicatedServer())
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_EC_LogSet, 500, false);
#endif
	}

#ifdef DIAG_DEVELOPER
	protected void VB_EC_LogSet()
	{
		float kg = GetWeightEx() / 1000;
		Print("[VB_EC] комплект экзо: " + m_VB_EC_ExoSet + ", вес " + kg + " кг, " + GetType());
	}
#endif

	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);
		VB_EC_UpdateExoSet(null);
	}

	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);
		VB_EC_UpdateExoSet(item);
		if (item && VB_EC_Exo.IsExo(item))
			item.SetWeightDirty();		// снятая вещь снова весит полностью
	}
};

modded class HungerMdfr
{
	override void OnTick(PlayerBase player, float deltaT)
	{
		if (player && player.VB_EC_ExoSetActive())
			deltaT = deltaT * VB_EC_Exo.s_SetHungerThirst;
		super.OnTick(player, deltaT);
	}
};

modded class ThirstMdfr
{
	override void OnTick(PlayerBase player, float deltaT)
	{
		if (player && player.VB_EC_ExoSetActive())
			deltaT = deltaT * VB_EC_Exo.s_SetHungerThirst;
		super.OnTick(player, deltaT);
	}
};

modded class BloodRegenMdfr
{
	override void OnTick(PlayerBase player, float deltaT)
	{
		if (player && player.VB_EC_ExoSetActive())
			deltaT = deltaT * VB_EC_Exo.s_SetBlood;
		super.OnTick(player, deltaT);
	}
};

modded class HealthRegenMdfr
{
	override void OnTick(PlayerBase player, float deltaT)
	{
		if (player && player.VB_EC_ExoSetActive())
			deltaT = deltaT * VB_EC_Exo.s_SetHealth;
		super.OnTick(player, deltaT);
	}
};

// Полный комплект: газ не вызывает химических ожогов ни на одной части тела (дыхание закрывает маска)
modded class BleedingSourcesManagerServer
{
	override bool AttemptAddBleedingSourceDirectly(int bit, eBleedingSourceType type = eBleedingSourceType.NORMAL, int context = 0)
	{
		if (type == eBleedingSourceType.CONTAMINATED && m_Player && m_Player.VB_EC_ExoSetActive())
			return false;
		return super.AttemptAddBleedingSourceDirectly(bit, type, context);
	}
};

// Полный комплект и работающий сервопривод: расход выносливости меньше (множители складываются: 0,6 × 0,8)
modded class StaminaHandler
{
	override protected void ProcessStaminaChanges(float deltaT, bool isServerOrSingleplayer)
	{
		if (m_Player)
		{
			float mul = 1;
			if (m_Player.VB_EC_ExoSetActive())
				mul = mul * VB_EC_Exo.s_SetStamina;
			if (VB_EC_Servo.Active(m_Player))
			{
				VB_EC_Servo.Load();
				mul = mul * VB_EC_Servo.s_StaminaUse;
			}
			if (mul < 1)
			{
				if (m_StaminaDelta < 0)
					m_StaminaDelta = m_StaminaDelta * mul;
				m_StaminaDepletion = m_StaminaDepletion * mul;
			}
		}
		super.ProcessStaminaChanges(deltaT, isServerOrSingleplayer);
	}
};

// ------------------------------------------------------------------ шлем
// (был IMP_Helmet_RoninHeavy_ColorBase): как MICH 2001 — ночной прицел и фонарь на шлеме (их включают ванильные
// действия по этому классу); тёмное забрало — затемнение экрана, как у спортивных очков
class VB_EC_Exo_Helmet_Base extends Mich2001Helmet
{
	// стёкла (выделение glass, этап 3б): горят светом сервопривода на этом бойце; без энергии — обычное стекло
	static const string GLASS_TEX = "VB_EchoClothes\\exo_set\\helmet\\ronin_light\\data\\glass_ca.paa";
	static const string GLASS_MAT = "dz\\weapons\\attachments\\optics\\data\\lensglass_clearer.rvmat";
	protected int m_VB_GlowShown = -1;

	override int GetGlassesEffectID()
	{
		return PPERequesterBank.REQ_GLASSESSPORTBLACK;
	}

	void VB_SetGlow(int kind)
	{
		if (GetGame().IsDedicatedServer() || kind == m_VB_GlowShown)
			return;
		m_VB_GlowShown = kind;
		VB_EC_Glow.Apply(this, "glass", kind, GLASS_TEX, GLASS_MAT);
	}

	protected void VB_SyncGlow()
	{
		int kind = 0;
		PlayerBase p = PlayerBase.Cast(GetHierarchyParent());
		if (p)
			kind = p.VB_EC_ServoGlowKind();
		VB_SetGlow(kind);
	}

	override void EEInit()
	{
		super.EEInit();
		VB_SyncGlow();
	}

	override void OnWasAttached(EntityAI parent, int slot_id)
	{
		super.OnWasAttached(parent, slot_id);
		VB_SyncGlow();
		PlayerBase p = PlayerBase.Cast(parent);
		if (p)
			p.VB_EC_UpdateMaskPlate(true);			// маска экзо под шлемом — пластину черепа спрятать
	}

	override void OnWasDetached(EntityAI parent, int slot_id)
	{
		super.OnWasDetached(parent, slot_id);
		VB_SetGlow(0);
		PlayerBase p = PlayerBase.Cast(parent);
		if (p)
			p.VB_EC_UpdateMaskPlate(false);
	}

	override void EEHealthLevelChanged(int oldLevel, int newLevel, string zone)
	{
		super.EEHealthLevelChanged(oldLevel, newLevel, zone);
		int kind = m_VB_GlowShown;
		m_VB_GlowShown = -1;
		VB_SetGlow(Math.Max(kind, 0));
	}
};

// хват шлема в руках — как у шлема «Горка» (так было в IMP)
modded class ModItemRegisterCallbacks
{
	override void RegisterOneHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
	{
		super.RegisterOneHanded(pType, pBehavior);
		pType.AddItemInHandsProfileIK("VB_EC_Exo_Helmet_Base", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/gorka_helmet.anm");
	}
};
