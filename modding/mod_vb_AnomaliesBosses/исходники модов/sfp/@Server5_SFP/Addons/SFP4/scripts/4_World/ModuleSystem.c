class SFP_Module_SystemBase extends Clothing
{
	// ---------- CONSTANTS ----------

	// ---------- OPTIONAL FEATURES ----------
	static const bool SMART_BATTERY_DRAIN_ENABLED = true;  // доп. расход батарейки в зависимости от модулей/активности
	static const bool MODULE_WEAR_ENABLED         = true;  // износ модулей после N срабатываний (требует ремонта)

	// Smart drain tuning (в единицах EnergyManager, масштабируется от базового energyUsage предмета)
	static const float SMART_DRAIN_MULT_PER_INSTALLED_MODULE = 0.25; // +25% от базового usage за каждый установленный модуль
	static const float SMART_DRAIN_MULT_PER_ACTIVE_EFFECT    = 0.50; // +50% от базового usage за активный эффект (бафф/модификатор)
	static const float SMART_DRAIN_MIN_EXTRA_PER_TICK        = 0.0;  // минимальный доп. расход за тик (0 = выключено)

	// Module wear tuning
	static const int   MODULE_WEAR_TRIGGERS_PER_DAMAGE = 5;    // после скольких срабатываний наносить урон модулю
	static const float MODULE_WEAR_DAMAGE_VALUE        = 5.0;  // сколько урона по хп модуля за "пакет" срабатываний


	// Health module
	static const float HEALTH_TRIGGER_THRESHOLD   = 40.0;       // старт баффа, если HP < 40
	static const float HEALTH_TARGET              = 100.0;      // лечим до 100
	static const float HEALTH_REGEN_PER_SEC       = 10.0;       // +10 HP/сек
	static const int   HEALTH_BUFF_DURATION_MS    = 10 * 1000;  // примерно хватает с запасом
	static const int   HEALTH_COOLDOWN_MS         = 60 * 1000;  // 60 секунд отката

	// Blood module

	// Anti-bleed (встроено в модуль крови)
	static const int   ANTIBLEED_COOLDOWN_MS          = 120 * 1000; // 2 минуты откат на остановку кровотечений
	// Disease cure (встроено в модуль крови)
	static const bool  DISEASE_CURE_ENABLED     = true;          // true/false - лечить болезни (агенты/инфекции)
	static const int   DISEASE_CURE_COOLDOWN_MS = 120 * 1000;     // 2 минуты откат

	static const float BLOOD_TRIGGER_THRESHOLD    = 3000.0;     // старт баффа, если кровь < 3000
	static const float BLOOD_TARGET               = 5000.0;     // регеним до 5000
	static const float BLOOD_REGEN_PER_SEC        = 100.0;      // +100 крови/сек
	static const int   BLOOD_BUFF_DURATION_MS     = 60 * 1000;  // до 5000 за 50 сек, берём с запасом
	static const int   BLOOD_COOLDOWN_MS          = 70 * 1000;  // 70 секунд отката

	// Stamina module
	static const int   STAMINA_BUFF_DURATION_MS   = 30 * 1000;  // 30 секунд баффа
	static const int   STAMINA_COOLDOWN_MS        = 60 * 1000;  // 60 секунд отката
	static const float STAMINA_REGEN_PER_SEC      = 50.0;       // быстрая регенерация стамины

	// Bone fix module
	static const int   BONEFIX_COOLDOWN_MS        = 2 * 60 * 1000;  // 2 минуты отката
	// Shock stabilizer (встроено в BoneFix)
	static const float SHOCK_TRIGGER_THRESHOLD = 25.0;      // если Shock < 25
	static const float SHOCK_TARGET            = 80.0;      // поднимаем Shock до 80
	static const int   SHOCK_COOLDOWN_MS        = 60 * 1000; // 60 секунд отката


	// ---------- FIELDS ----------
	protected PlayerBase   m_Player;
	protected bool         m_UpdateActive;
	protected bool         m_SystemOn;

	// таймеры баффов / откатов
	protected int          m_HealthBuffEndTime;
	protected int          m_HealthCooldownEndTime;

	protected int          m_BloodBuffEndTime;
	protected int          m_BloodCooldownEndTime;

	protected int          m_StaminaBuffEndTime;
	protected int          m_StaminaCooldownEndTime;
	protected bool         m_StaminaModifierActive;


	protected int          m_BoneFixCooldownEndTime;
	protected int          m_AntiBleedCooldownEndTime;
	protected int          m_DiseaseCureCooldownEndTime;
	protected int          m_ShockCooldownEndTime;

	protected int          m_HealthTriggerCount;
	protected int          m_BloodTriggerCount;
	protected int          m_StaminaTriggerCount;
	protected int          m_BoneFixTriggerCount;

	// ---------- CTOR / DTOR ----------
	void SFP_Module_SystemBase()
	{
		m_Player                 = null;
		m_UpdateActive           = false;
		m_SystemOn               = false;

		m_HealthBuffEndTime      = 0;
		m_HealthCooldownEndTime  = 0;

		m_BloodBuffEndTime       = 0;
		m_BloodCooldownEndTime   = 0;

		m_StaminaBuffEndTime     = 0;
		m_StaminaCooldownEndTime = 0;
		m_StaminaModifierActive  = false;

		m_BoneFixCooldownEndTime = 0;
		m_AntiBleedCooldownEndTime = 0;
		m_DiseaseCureCooldownEndTime = 0;
		m_ShockCooldownEndTime = 0;
	}

	void ~SFP_Module_SystemBase()
	{
		Deactivate();
		ResetStaminaModifier();
	}

	// ---------- ACTIONS ----------
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionTurnOnWhileInHands);
		AddAction(ActionTurnOffWhileInHands);
	}

	// ---------- ATTACH / DETACH ----------
	override void OnWasAttached(EntityAI parent, int slot_id)
	{
		super.OnWasAttached(parent, slot_id);

		PlayerBase player = PlayerBase.Cast(parent);
		if (player)
		{
			m_Player = player;
			Activate();
			SFP_Log.Debug("[SFP_ModuleSystem] OnWasAttached: player=" + player.ToString());
		}
	}

	override void OnWasDetached(EntityAI parent, int slot_id)
	{
		super.OnWasDetached(parent, slot_id);

		PlayerBase player = PlayerBase.Cast(parent);
		if (player && player == m_Player)
		{
			SFP_Log.Debug("[SFP_ModuleSystem] OnWasDetached");
			ResetStaminaModifier();
			m_Player = null;
			Deactivate();
		}
	}


	// ---------- INVENTORY EVENTS ----------
	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);

		// Welcome sound when CPU is inserted (local player only)
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;

		PlayerBase owner = PlayerBase.Cast(GetHierarchyRootPlayer());
		if (!owner || !owner.IsControlledPlayer())
			return;

		// Play insertion sounds only when the system is powered (battery installed + switched on)
		if (!m_SystemOn)
			return;

		// For non-CPU modules, also require CPU installed (otherwise the system won't function anyway)
		if (slot_name != "SFP_ModuleSystemSlot_CPU" && !HasCPUModule())
			return;


		EffectSound snd;

		// CPU (welcome)
		if (slot_name == "SFP_ModuleSystemSlot_CPU" && SFP_ModuleSystem_CPU.Cast(item))
		{
			owner.PlaySoundSet(snd, "SFP_moduleSystem_Welcome_Soundset", 0, 0);
		}
		// Other modules: sound once on insertion (local player only)
		else if (slot_name == "SFP_ModuleSystemSlot_Health" && SFP_ModuleSystem_Health.Cast(item))
		{
			owner.PlaySoundSet(snd, "SFP_moduleSystem_SystemHealth_Soundset", 0, 0);
		}
		else if (slot_name == "SFP_ModuleSystemSlot_Blood" && SFP_ModuleSystem_Blood.Cast(item))
		{
			owner.PlaySoundSet(snd, "SFP_moduleSystem_SystemBlood_Soundset", 0, 0);
		}
		else if (slot_name == "SFP_ModuleSystemSlot_Stamina" && SFP_ModuleSystem_Stamina.Cast(item))
		{
			owner.PlaySoundSet(snd, "SFP_moduleSystem_SystemStamina_Soundset", 0, 0);
		}
		else if (slot_name == "SFP_ModuleSystemSlot_boneFix" && SFP_ModuleSystem_BoneFix.Cast(item))
		{
			owner.PlaySoundSet(snd, "SFP_moduleSystem_SystemFixBones_Soundset", 0, 0);
		}

	}
	// ---------- ENERGY MANAGER HOOKS ----------
	override void OnWorkStart()
	{
		super.OnWorkStart();
		m_SystemOn = true;
		SFP_Log.Debug("[SFP_ModuleSystem] OnWorkStart (system ON)");
	}

	override void OnWorkStop()
	{
		super.OnWorkStop();
		m_SystemOn = false;
		SFP_Log.Debug("[SFP_ModuleSystem] OnWorkStop (system OFF)");

		// сбросим модификаторы
		ResetStaminaModifier();

		// при выключении просто “обнуляем” активные баффы (кулдауны остаются)
		m_HealthBuffEndTime  = 0;
		m_BloodBuffEndTime   = 0;
		m_StaminaBuffEndTime = 0;
	}

	// ---------- UPDATE TICK ----------
	void Activate()
	{
		if (!m_UpdateActive && GetGame())
		{
			m_UpdateActive = true;
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.ModuleSystemUpdate, 1000, true);
			SFP_Log.Debug("[SFP_ModuleSystem] Activate tick");
		}
	}

	void Deactivate()
	{
		if (m_UpdateActive && GetGame())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(this.ModuleSystemUpdate);
			m_UpdateActive = false;
			SFP_Log.Debug("[SFP_ModuleSystem] Deactivate tick");
		}
	}

	void ModuleSystemUpdate()
	{
		if (!m_Player || !m_Player.IsAlive())
		{
			ResetStaminaModifier();
			Deactivate();
			return;
		}

		// система должна быть включена (экшен + EnergyManager)
		if (!m_SystemOn)
		{
			ResetStaminaModifier();
			return;
		}

		// требуется установленный процессор (CPU). Без него модули не работают даже при батарейке.
		if (!HasCPUModule())
		{
			// если CPU вынули во время работы — останавливаем активные эффекты
			ResetStaminaModifier();
			m_HealthBuffEndTime  = 0;
			m_BloodBuffEndTime   = 0;
			m_StaminaBuffEndTime = 0;
			return;
		}


		int now = GetGame().GetTime();
		ApplySmartBatteryDrain(now);

		UpdateHealthModule();
		UpdateBloodModule();
		UpdateStaminaModule();
		UpdateBoneFixModule();
	}

	

	// ---------- HELPERS ----------
	protected bool IsModuleUsable(EntityAI att)
	{
		ItemBase ib = ItemBase.Cast(att);
		if (!ib)
			return false;

		// ruined modules should not function
		if (ib.IsRuined())
			return false;

		return true;
	}

	protected ItemBase GetModuleBySlot(string slot_name)
	{
		return ItemBase.Cast(FindAttachmentBySlotName(slot_name));
	}

	protected void ApplyModuleWear(string slot_name, inout int trigger_counter)
	{
		if (!MODULE_WEAR_ENABLED)
			return;

		trigger_counter++;

		if (trigger_counter < MODULE_WEAR_TRIGGERS_PER_DAMAGE)
			return;

		trigger_counter = 0;

		if (!GetGame() || !GetGame().IsServer())
			return;

		ItemBase module = GetModuleBySlot(slot_name);
		if (!module)
			return;

		// наносим урон модулю; если его нужно ремонтировать — игрок это увидит по состоянию
		module.DecreaseHealth("", "", MODULE_WEAR_DAMAGE_VALUE);
	}

	protected void ApplySmartBatteryDrain(int now)
	{
		if (!SMART_BATTERY_DRAIN_ENABLED)
			return;

		if (!GetGame() || !GetGame().IsServer())
			return;

		ComponentEnergyManager em = GetCompEM();
		if (!em)
			return;

		float baseUsage = em.GetEnergyUsage();
		if (baseUsage <= 0)
		{
			// если в конфиге usage = 0, можно включить минимальный расход через SMART_DRAIN_MIN_EXTRA_PER_TICK
			if (SMART_DRAIN_MIN_EXTRA_PER_TICK > 0)
				em.ConsumeEnergy(SMART_DRAIN_MIN_EXTRA_PER_TICK);
			return;
		}

		int installed = 0;
		if (HasCPUModule())        installed++;
		if (HasHealthModule())     installed++;
		if (HasBloodModule())      installed++;
		if (HasStaminaModule())    installed++;
		if (HasBoneFixModule())    installed++;

		int active = 0;
		if (now < m_HealthBuffEndTime)   active++;
		if (now < m_BloodBuffEndTime)    active++;
		if (m_StaminaModifierActive)     active++;

		// BoneFix не является "постоянным баффом", поэтому тут не считаем его активным.
		float mult = (installed * SMART_DRAIN_MULT_PER_INSTALLED_MODULE) + (active * SMART_DRAIN_MULT_PER_ACTIVE_EFFECT);
		float extra = baseUsage * mult;

		if (extra > 0)
			em.ConsumeEnergy(extra);
	}

// ---------- MODULE CHECKS ----------

	protected bool HasCPUModule()
	{
		EntityAI att = FindAttachmentBySlotName("SFP_ModuleSystemSlot_CPU");
		SFP_ModuleSystem_CPU mod = SFP_ModuleSystem_CPU.Cast(att);
		if (!mod)
			return false;
		return IsModuleUsable(mod);
	}
	protected bool HasHealthModule()
	{
		EntityAI att = FindAttachmentBySlotName("SFP_ModuleSystemSlot_Health");
		SFP_ModuleSystem_Health mod = SFP_ModuleSystem_Health.Cast(att);
		if (!mod)
			return false;
		return IsModuleUsable(mod);
	}

	protected bool HasBloodModule()
	{
		EntityAI att = FindAttachmentBySlotName("SFP_ModuleSystemSlot_Blood");
		SFP_ModuleSystem_Blood mod = SFP_ModuleSystem_Blood.Cast(att);
		if (!mod)
			return false;
		return IsModuleUsable(mod);
	}

	protected bool HasStaminaModule()
	{
		EntityAI att = FindAttachmentBySlotName("SFP_ModuleSystemSlot_Stamina");
		SFP_ModuleSystem_Stamina mod = SFP_ModuleSystem_Stamina.Cast(att);
		if (!mod)
			return false;
		return IsModuleUsable(mod);
	}

	protected bool HasBoneFixModule()
	{
		EntityAI att = FindAttachmentBySlotName("SFP_ModuleSystemSlot_boneFix");
		SFP_ModuleSystem_BoneFix mod = SFP_ModuleSystem_BoneFix.Cast(att);
		if (!mod)
			return false;
		return IsModuleUsable(mod);
	}

// ---------- DISEASE CURE HELPERS ----------
protected bool HasAnyDiseaseActive()
{
	if (!m_Player)
		return false;

	// Любые активные агенты (болезни/инфекции)
	if (m_Player.GetAgents() > 0)
		return true;

	ModifiersManager mm = m_Player.GetModifiersManager();
	if (!mm)
		return false;

	// Проверяем ключевые модификаторы болезней/инфекций
	if (mm.IsModifierActive(eModifiers.MDF_COMMON_COLD)) return true;
	if (mm.IsModifierActive(eModifiers.MDF_INFLUENZA)) return true;
	if (mm.IsModifierActive(eModifiers.MDF_CHOLERA)) return true;
	if (mm.IsModifierActive(eModifiers.MDF_SALMONELLA)) return true;
	if (mm.IsModifierActive(eModifiers.MDF_PNEUMONIA)) return true;

	if (mm.IsModifierActive(eModifiers.MDF_WOUND_INFECTION1)) return true;
	if (mm.IsModifierActive(eModifiers.MDF_WOUND_INFECTION2)) return true;

	if (mm.IsModifierActive(eModifiers.MDF_POISONING)) return true;
	if (mm.IsModifierActive(eModifiers.MDF_HEMOLYTIC_REACTION)) return true;
	if (mm.IsModifierActive(eModifiers.MDF_TOXICITY)) return true;
	if (mm.IsModifierActive(eModifiers.MDF_BRAIN)) return true;
	if (mm.IsModifierActive(eModifiers.MDF_FEVER)) return true;

	if (mm.IsModifierActive(eModifiers.MDF_CONTAMINATION1)) return true;
	if (mm.IsModifierActive(eModifiers.MDF_CONTAMINATION2)) return true;
	if (mm.IsModifierActive(eModifiers.MDF_CONTAMINATION3)) return true;

	if (mm.IsModifierActive(eModifiers.MDF_HEAVYMETAL1)) return true;
	if (mm.IsModifierActive(eModifiers.MDF_HEAVYMETAL2)) return true;
	if (mm.IsModifierActive(eModifiers.MDF_HEAVYMETAL3)) return true;

	return false;
}

protected void CureAllDiseasesServer()
{
	if (!GetGame() || !GetGame().IsServer() || !m_Player)
		return;

	// 1) Убираем всех агентов
	m_Player.RemoveAllAgents();

	// 2) Снимаем модификаторы болезней/инфекций (на всякий случай)
	ModifiersManager mm = m_Player.GetModifiersManager();
	if (!mm)
		return;

	mm.DeactivateModifier(eModifiers.MDF_COMMON_COLD);
	mm.DeactivateModifier(eModifiers.MDF_INFLUENZA);
	mm.DeactivateModifier(eModifiers.MDF_CHOLERA);
	mm.DeactivateModifier(eModifiers.MDF_SALMONELLA);
	mm.DeactivateModifier(eModifiers.MDF_PNEUMONIA);

	mm.DeactivateModifier(eModifiers.MDF_WOUND_INFECTION1);
	mm.DeactivateModifier(eModifiers.MDF_WOUND_INFECTION2);

	mm.DeactivateModifier(eModifiers.MDF_POISONING);
	mm.DeactivateModifier(eModifiers.MDF_HEMOLYTIC_REACTION);
	mm.DeactivateModifier(eModifiers.MDF_TOXICITY);
	mm.DeactivateModifier(eModifiers.MDF_BRAIN);
	mm.DeactivateModifier(eModifiers.MDF_FEVER);

	mm.DeactivateModifier(eModifiers.MDF_CONTAMINATION1);
	mm.DeactivateModifier(eModifiers.MDF_CONTAMINATION2);
	mm.DeactivateModifier(eModifiers.MDF_CONTAMINATION3);

	mm.DeactivateModifier(eModifiers.MDF_HEAVYMETAL1);
	mm.DeactivateModifier(eModifiers.MDF_HEAVYMETAL2);
	mm.DeactivateModifier(eModifiers.MDF_HEAVYMETAL3);
}

	// ---------- HEALTH MODULE (как стамина: бафф + откат + звук 1 раз) ----------
	protected void UpdateHealthModule()
	{
		if (!HasHealthModule())
		{
			// если модуль вынули — просто сбросим активный бафф
			m_HealthBuffEndTime = 0;
			return;
		}

		float health    = m_Player.GetHealth("", "Health");
		float maxHealth = m_Player.GetMaxHealth("", "Health");

		int now = GetGame().GetTime();
		bool buffActive     = (now < m_HealthBuffEndTime);
		bool cooldownActive = (now < m_HealthCooldownEndTime);

		// старт баффа: HP < порога, бафф не активен, кулдауна нет
		if (!buffActive && !cooldownActive && health < HEALTH_TRIGGER_THRESHOLD)
		{
			// задаём время конца баффа и конца отката
			m_HealthBuffEndTime     = now + HEALTH_BUFF_DURATION_MS;
			m_HealthCooldownEndTime = now + HEALTH_BUFF_DURATION_MS + HEALTH_COOLDOWN_MS;

			SFP_Log.Debug("[SFP_ModuleSystem] HEALTH BUFF START");

			// износ модуля (по количеству срабатываний)
			ApplyModuleWear("SFP_ModuleSystemSlot_Health", m_HealthTriggerCount);

			// звук перенесён на установку модуля в слот

			buffActive = true; // сразу считаем, что бафф активен
		}

		// пока бафф активен – лечим
		if (buffActive)
		{
			float target = Math.Min(HEALTH_TARGET, maxHealth);

			if (GetGame().IsServer())
			{
				float newHealth = health + HEALTH_REGEN_PER_SEC;
				if (newHealth > target)
					newHealth = target;

				m_Player.SetHealth("", "Health", newHealth);
			}

			// если уже достигли цели раньше времени – досрочно заканчиваем бафф
			if (health >= target)
			{
				m_HealthBuffEndTime = now; // на следующем тике buffActive станет false
			}
		}
	}

	// ---------- BLOOD MODULE (как стамина: бафф + откат + звук 1 раз) ----------
	protected void UpdateBloodModule()
	{
		if (!HasBloodModule())
		{
			m_BloodBuffEndTime = 0;
			return;
		}

		float blood    = m_Player.GetHealth("", "Blood");
		float maxBlood = m_Player.GetMaxHealth("", "Blood");

		int now = GetGame().GetTime();
		bool buffActive     = (now < m_BloodBuffEndTime);
		bool cooldownActive = (now < m_BloodCooldownEndTime);

		// Anti-bleed: если есть кровотечения — останавливаем (с откатом)
		if (GetGame().IsServer())
		{
			int bleedingBits = m_Player.GetBleedingBits();
			if (bleedingBits != 0 && now >= m_AntiBleedCooldownEndTime)
			{
				BleedingSourcesManagerServer bm = m_Player.GetBleedingManagerServer();
				if (bm)
				{
					bm.RemoveAllSources();
					m_AntiBleedCooldownEndTime = now + ANTIBLEED_COOLDOWN_MS;

					SFP_Log.Debug("[SFP_ModuleSystem] BLOOD MODULE: anti-bleed (removed all bleeding sources), cooldown 2 min");

					// износ модуля крови за срабатывание anti-bleed
					ApplyModuleWear("SFP_ModuleSystemSlot_Blood", m_BloodTriggerCount);
				}
			}
		}


		// Disease cure: лечим все активные болезни/инфекции (с откатом)
		if (DISEASE_CURE_ENABLED && GetGame().IsServer())
		{
			if (now >= m_DiseaseCureCooldownEndTime && HasAnyDiseaseActive())
			{
				CureAllDiseasesServer();
				m_DiseaseCureCooldownEndTime = now + DISEASE_CURE_COOLDOWN_MS;

				SFP_Log.Debug("[SFP_ModuleSystem] BLOOD MODULE: disease cure (removed agents + deactivated disease modifiers), cooldown 2 min");

				// износ модуля крови за срабатывание лечения болезней
				ApplyModuleWear("SFP_ModuleSystemSlot_Blood", m_BloodTriggerCount);
			}
		}


		// старт баффа: кровь < порога, бафф не активен, кулдауна нет
		if (!buffActive && !cooldownActive && blood < BLOOD_TRIGGER_THRESHOLD)
		{
			m_BloodBuffEndTime     = now + BLOOD_BUFF_DURATION_MS;
			m_BloodCooldownEndTime = now + BLOOD_BUFF_DURATION_MS + BLOOD_COOLDOWN_MS;

			SFP_Log.Debug("[SFP_ModuleSystem] BLOOD BUFF START");

			// износ модуля (по количеству срабатываний)
			ApplyModuleWear("SFP_ModuleSystemSlot_Blood", m_BloodTriggerCount);

			// звук перенесён на установку модуля в слот

			buffActive = true;
		}

		if (buffActive)
		{
			float target = Math.Min(BLOOD_TARGET, maxBlood);

			if (GetGame().IsServer())
			{
				float newBlood = blood + BLOOD_REGEN_PER_SEC;
				if (newBlood > target)
					newBlood = target;

				m_Player.SetHealth("", "Blood", newBlood);
			}

			// досрочно завершаем бафф, если уже дорегенили
			if (blood >= target)
			{
				m_BloodBuffEndTime = now;
			}
		}
	}

	// ---------- STAMINA MODULE (как у тебя работал) ----------
	protected void UpdateStaminaModule()
	{
		if (!HasStaminaModule())
		{
			ResetStaminaModifier();
			return;
		}

		StaminaHandler staminaHandler = m_Player.GetStaminaHandler();
		if (!staminaHandler)
		{
			ResetStaminaModifier();
			return;
		}

		int  now            = GetGame().GetTime();
		bool buffActive     = (now < m_StaminaBuffEndTime);
		bool cooldownActive = (now < m_StaminaCooldownEndTime);

		float stamina = staminaHandler.GetStamina();

		// старт баффа: стамина = 0, нет активного баффа и кулдауна
		if (!buffActive && !cooldownActive && stamina <= 0.1)
		{
			m_StaminaBuffEndTime     = now + STAMINA_BUFF_DURATION_MS;
			m_StaminaCooldownEndTime = now + STAMINA_BUFF_DURATION_MS + STAMINA_COOLDOWN_MS;

			SFP_Log.Debug("[SFP_ModuleSystem] STAMINA BUFF START");

			// износ модуля (по количеству срабатываний)
			ApplyModuleWear("SFP_ModuleSystemSlot_Stamina", m_StaminaTriggerCount);

			if (GetGame().IsServer())
			{
				staminaHandler.ActivateDepletionModifier(EStaminaMultiplierTypes.EPINEPHRINE);
				m_StaminaModifierActive = true;
			}

			// звук перенесён на установку модуля в слот

			buffActive = true;
		}

		buffActive = (now < m_StaminaBuffEndTime);

		// пока бафф активен – быстро регеним стамину
		if (buffActive)
		{
			if (GetGame().IsServer())
			{
				float maxStamina = CfgGameplayHandler.GetStaminaMax();
				float newStamina = stamina + STAMINA_REGEN_PER_SEC;
				if (newStamina > maxStamina)
					newStamina = maxStamina;

				staminaHandler.SetStamina(newStamina);
			}
		}
		else
		{
			// бафф закончился – модификатор отключаем, ждём только кулдаун по времени
			ResetStaminaModifier();
		}
	}


	// ---------- BONE FIX MODULE (лечит перелом ног) ----------
	protected void UpdateBoneFixModule()
	{
		if (!HasBoneFixModule())
			return;

		// Зависимости: батарейка + включение (m_SystemOn) обеспечиваются EnergyManager,
		// CPU проверяется в ModuleSystemUpdate() перед вызовом модулей.

		if (!GetGame() || !m_Player)
			return;

		int now = GetGame().GetTime();

		// Shock stabilizer: если Shock сильно упал — поднимаем (отдельный откат)
		if (GetGame().IsServer() && now >= m_ShockCooldownEndTime)
		{
			float shock = m_Player.GetHealth("", "Shock");
			if (shock < SHOCK_TRIGGER_THRESHOLD)
			{
				float maxShock = m_Player.GetMaxHealth("", "Shock");
				float targetShock = SHOCK_TARGET;
				if (targetShock > maxShock)
					targetShock = maxShock;

				m_Player.SetHealth("", "Shock", targetShock);
				m_ShockCooldownEndTime = now + SHOCK_COOLDOWN_MS;

				SFP_Log.Debug("[SFP_ModuleSystem] BONEFIX MODULE: SHOCK STABILIZED");
				ApplyModuleWear("SFP_ModuleSystemSlot_boneFix", m_BoneFixTriggerCount);
			}
		}

		if (now < m_BoneFixCooldownEndTime)
			return; // откат после последнего лечения

		if (!GetGame().IsServer())
			return;

		ModifiersManager modifiers = m_Player.GetModifiersManager();
		bool brokenMdfrActive = false;
		if (modifiers)
			brokenMdfrActive = modifiers.IsModifierActive(eModifiers.MDF_BROKEN_LEGS);

		// Ванильный перелом ног держится модификатором BrokenLegsMdfr и снимается,
		// когда здоровье ног восстановлено. Поэтому лечим НЕ только флаг перелома,
		// но и восстанавливаем здоровье ног/ступней.
		if (brokenMdfrActive || m_Player.GetBrokenLegs() != eBrokenLegs.NO_BROKEN_LEGS)
		{
			float max;

			max = m_Player.GetMaxHealth("RightLeg", "Health");  m_Player.SetHealth("RightLeg",  "Health", max);
			max = m_Player.GetMaxHealth("LeftLeg",  "Health");  m_Player.SetHealth("LeftLeg",   "Health", max);
			max = m_Player.GetMaxHealth("RightFoot","Health");  m_Player.SetHealth("RightFoot", "Health", max);
			max = m_Player.GetMaxHealth("LeftFoot", "Health");  m_Player.SetHealth("LeftFoot",  "Health", max);

			// Снимаем модификатор перелома (он же уберёт нотификатор и сплинт при необходимости)
			if (modifiers && brokenMdfrActive)
				modifiers.DeactivateModifier(eModifiers.MDF_BROKEN_LEGS);

			// На всякий случай убираем состояние перелома сразу
			m_Player.SetBrokenLegs(eBrokenLegs.NO_BROKEN_LEGS);

			m_BoneFixCooldownEndTime = now + BONEFIX_COOLDOWN_MS;
			SFP_Log.Debug("[SFP_ModuleSystem] BONE FIX: healed broken legs (restored leg health, cooldown 2 min)");
			// износ модуля boneFix за реальное лечение перелома
			ApplyModuleWear("SFP_ModuleSystemSlot_boneFix", m_BoneFixTriggerCount);
		}
	}
	// ---------- STAMINA MODIFIER RESET ----------
	protected void ResetStaminaModifier()
	{
		if (m_StaminaModifierActive && GetGame() && GetGame().IsServer() && m_Player)
		{
			StaminaHandler staminaHandler = m_Player.GetStaminaHandler();
			if (staminaHandler)
			{
				staminaHandler.DeactivateDepletionModifier(EStaminaMultiplierTypes.EPINEPHRINE);
			}
		}

		m_StaminaModifierActive = false;
	}
}