class SFP_Item_scanner_mutantType2 extends Inventory_Base
{
	// ---- Modes ----
	static const int SFP_MODE_STANDARD = 0;
	static const int SFP_MODE_BOOST    = 1;

	static const float SFP_RADIUS_STANDARD = 20.0;
	static const float SFP_RADIUS_BOOST    = 40.0;

	// Burn ("выжигатель")
	static const float SFP_BURN_RADIUS = 50.0;
	static const float SFP_BURN_REQUIRED = 0.80; // 80%
	static const string SFP_SOUND_BURN = "SFP_Scaner_work_03_Soundset";

	// Повышенный расход энергии в усиленном режиме (умножитель от базового из config.cpp)
	static const float SFP_BOOST_ENERGY_MULT = 2.5;

	// “паника/разворот” (без телепорта)
	static const int   SFP_REPEL_TICK_MS = 120;  // ms
	static const float SFP_REPEL_BUFFER  = 2.5;  // не дёргать у границы

	static const float SFP_STEP_MIN_STD  = 0.30;
	static const float SFP_STEP_MAX_STD  = 0.95;
	static const float SFP_STEP_MIN_BST  = 0.40;
	static const float SFP_STEP_MAX_BST  = 1.25;

	// Soundsets for repel modes
	static const string SFP_SOUND_STANDARD = "SFP_Scaner_work_01_Soundset";
	static const string SFP_SOUND_BOOST    = "SFP_Scaner_work_02_Soundset";

	// ----- LED system (like detonator: texture color strings) -----
	static const string SFP_COLOR_LED_OFF    = "#(argb,8,8,3)color(0,0,0,1.0,co)";
	static const string SFP_COLOR_LED_RED    = "#(argb,8,8,3)color(1,0,0,1.0,co)";
	static const string SFP_COLOR_LED_GREEN  = "#(argb,8,8,3)color(0,1,0,1.0,co)";
	static const string SFP_COLOR_LED_YELLOW = "#(argb,8,8,3)color(1,1,0,1.0,co)";

	static const string SFP_SEL_LED_ENERGY   = "led_energy";
	static const bool SFP_ENABLE_LEDS = false; // disable LED system (no server load)
	static const string SFP_SEL_LED_WORK     = "led_work";
	static const string SFP_SEL_LED_KILLALL  = "led_kill_all";

	// Power source slot (CarBattery)
	static const string SFP_SLOT_POWER = "CarBattery";

	// Antenna animation source
	static const string SFP_ANIM_ANTENNA = "antenna";
	static const int SFP_ANTENNA_TICK_MS = 16; // ~60 FPS (client-side) // 20 FPS
	static const float SFP_ANTENNA_RPS = 0.01;  // revolutions per second



	// Blink (kill-ready LED)
	static const int SFP_KILL_BLINK_MS = 500;
	static const int SFP_VISUAL_TICK_MS = 250; // client refresh interval
	static const bool SFP_ENABLE_KILL_BLINK = false; // MP-safe: disable blinking
	protected bool m_SFP_KillBlinkState;

	// Antenna spin runtime
	protected bool m_SFP_AntennaSpinning;
	protected float m_SFP_AntennaPhase;
	protected int m_SFP_AntennaLastMs;
	protected bool m_SFP_BlinkActive;


	// Synced mode for clients (sound switching)
	protected int m_SFP_Mode = SFP_MODE_STANDARD;
	protected int m_SFP_ClientLastMode = -1;

	// Synced LED states (0/1)
	protected int m_SFP_LED_Energy = 0;
	protected int m_SFP_LED_Work   = 0;
	protected int m_SFP_LED_Kill   = 0;

	// One-shot burn sound event counter (net sync)
	protected int m_SFP_BurnEvent = 0;
	protected int m_SFP_BurnEvent_ClientLast = 0;

	// Net: working state (for MP LED reliability)
	protected int m_SFP_WorkingNet = 0;

	protected float m_SFP_BaseEnergyUsage = -1.0;

	protected ref EffectSound m_SFP_LoopSound;

	protected ref array<Object> m_SFP_NearObjects;
	protected ref array<CargoBase> m_SFP_NearCargos;

	void SFP_Item_scanner_mutantType2()
	{
		m_SFP_NearObjects = new array<Object>;
		m_SFP_NearCargos = new array<CargoBase>;

		RegisterNetSyncVariableInt("m_SFP_Mode", 0, 1);
		RegisterNetSyncVariableInt("m_SFP_LED_Energy", 0, 1);
		RegisterNetSyncVariableInt("m_SFP_LED_Work", 0, 1);
		RegisterNetSyncVariableInt("m_SFP_LED_Kill", 0, 1);
		RegisterNetSyncVariableInt("m_SFP_BurnEvent");
		RegisterNetSyncVariableInt("m_SFP_WorkingNet", 0, 1);

		// Ensure LEDs are in a known state on spawn (SP/client visuals)
		if (SFP_ENABLE_LEDS) SFP_ApplyLEDs();
	}

	override void EEInit()
	{
		super.EEInit();

		SFP_CacheBaseEnergyUsage();

		// Server calculates and syncs states; SP also applies visuals locally
		if (SFP_ENABLE_LEDS) SFP_UpdateLEDStates(true);

	
		SFP_StopAntennaSpin();


		if (!SFP_ENABLE_LEDS) SFP_ForceLEDOff();
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();

		// Apply LED textures on clients
		if (GetGame().IsClient())
		{
			if (SFP_ENABLE_LEDS) SFP_ApplyLEDs();
		}

		// Burn sound: server bumps counter, clients play one-shot
		if (GetGame().IsClient())
		{
			if (m_SFP_BurnEvent != m_SFP_BurnEvent_ClientLast)
			{
				m_SFP_BurnEvent_ClientLast = m_SFP_BurnEvent;
				SEffectManager.PlaySoundOnObject(SFP_SOUND_BURN, this);
			}
		}

		// If mode changed while device working -> restart loop sound with correct soundset
		if (GetGame().IsClient())
		{
			if (m_SFP_ClientLastMode != m_SFP_Mode)
			{
				m_SFP_ClientLastMode = m_SFP_Mode;
				if (GetCompEM() && GetCompEM().IsWorking())
					SFP_RestartLoopSound();
			}
		}

		// Antenna spin: client-side cosmetic based on synced working flag
		if (GetGame().IsClient())
		{
			if (m_SFP_WorkingNet == 1)
				SFP_StartAntennaSpin();
			else
				SFP_StopAntennaSpin();
		}
	}

	// Vanilla On/Off + our mode toggle + burn action
	override void SetActions()
	{
		super.SetActions();

		AddAction(ActionTurnOnWhileOnGround);
		AddAction(ActionTurnOffWhileOnGround);

		AddAction(ActionSFP_ScannerMutantToggleMode);
		AddAction(ActionSFP_ScannerMutantBurnKillAll);
	}

	// ---------- API for actions ----------
	int SFP_GetMode()
	{
		return m_SFP_Mode;
	}

	void SFP_SetMode(int newMode)
	{
		if (newMode != SFP_MODE_STANDARD && newMode != SFP_MODE_BOOST)
			return;

		if (m_SFP_Mode == newMode)
			return;

		m_SFP_Mode = newMode;
		SetSynchDirty();

		// Apply immediately if working
		if (GetCompEM() && GetCompEM().IsWorking())
		{
			SFP_ApplyModeEnergyUsage();

			// SP / local client: restart sound immediately
			if (!GetGame().IsServer() || !GetGame().IsMultiplayer())
				SFP_RestartLoopSound();
		}
	}

	bool SFP_IsBurnReady()
	{
		CarBattery cb = SFP_GetCarBattery();
		if (!cb)
			return false;

		return SFP_GetBatteryLevel0To1() >= SFP_BURN_REQUIRED;
	}

	protected bool SFP_IsPredatorType(string typeName)
	{
		// Covers vanilla and most modded names that include these tokens
		if (typeName.Contains("Bear") || typeName.Contains("bear") || typeName.Contains("Ursus") || typeName.Contains("ursus"))
			return true;

		if (typeName.Contains("Wolf") || typeName.Contains("wolf") || typeName.Contains("Lupus") || typeName.Contains("lupus"))
			return true;

		return false;
	}


	void SFP_DoBurnKillAll()
	{
		if (!GetGame().IsServer())
			return;

		if (!SFP_IsBurnReady())
			return;

		// Ensure device is off
		if (GetCompEM() && GetCompEM().IsWorking())
			GetCompEM().SwitchOff();

		m_SFP_WorkingNet = 0;
		SetSynchDirty();

		// Burn sound event: dedicated server has no audio, clients play via net sync
		m_SFP_BurnEvent = m_SFP_BurnEvent + 1;
		SetSynchDirty();

		// Singleplayer/local: play immediately
		if (!GetGame().IsMultiplayer())
			SEffectManager.PlaySoundOnObject(SFP_SOUND_BURN, this);

		// Kill infected + predators in radius
		vector center = GetPosition();

		m_SFP_NearObjects.Clear();
		m_SFP_NearCargos.Clear();
		GetGame().GetObjectsAtPosition3D(center, SFP_BURN_RADIUS, m_SFP_NearObjects, m_SFP_NearCargos);

		for (int i = 0; i < m_SFP_NearObjects.Count(); i++)
		{
			Object obj = m_SFP_NearObjects[i];
			if (!obj)
				continue;

			// 1) Infected
			ZombieBase zed = ZombieBase.Cast(obj);
			if (zed)
			{
				if (zed.IsAlive())
					zed.SetHealth("", "", 0);

				continue;
			}

			// 2) Predators (bear/wolf) and modded creatures with their AI (name contains tokens)
			DayZCreatureAI creature = DayZCreatureAI.Cast(obj);
			if (!creature)
				continue;

			if (!creature.IsAlive())
				continue;

			if (!SFP_IsPredatorType(obj.GetType()))
				continue;

			creature.SetHealth("", "", 0);
		}

		// Drain & destroy the car battery
		CarBattery cb = SFP_GetCarBattery();
		if (cb)
		{
			if (cb.HasEnergyManager() && cb.GetCompEM())
				cb.GetCompEM().SetEnergy(0);

			if (cb.HasQuantity())
				cb.SetQuantity(0);

			cb.SetHealth("", "", 0);
		}

		if (SFP_ENABLE_LEDS) SFP_UpdateLEDStates(true);
	}

	// ---------- Energy / attachment hooks for LEDs ----------
	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);
		if (SFP_ENABLE_LEDS) SFP_UpdateLEDStates(true);

	}

	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);
		if (SFP_ENABLE_LEDS) SFP_UpdateLEDStates(true);

	}

	override void OnEnergyAdded()
	{
		super.OnEnergyAdded();
		if (SFP_ENABLE_LEDS) SFP_UpdateLEDStates(true);

	}

	override void OnEnergyConsumed()
	{
		super.OnEnergyConsumed();
		if (SFP_ENABLE_LEDS) SFP_UpdateLEDStates(false);
	}

	// ---------- EnergyManager hooks ----------
	override void OnWorkStart()
	{
		super.OnWorkStart();

		if (GetGame().IsServer())
		{
			m_SFP_WorkingNet = 1;
			SetSynchDirty();
		}

		// On enable: always start in standard mode (server authority)
		if (GetGame().IsServer())
		{
			m_SFP_Mode = SFP_MODE_STANDARD;
			SetSynchDirty();
		}

		SFP_CacheBaseEnergyUsage();
		SFP_ApplyModeEnergyUsage();

		// Sound: client (and SP)
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			SFP_RestartLoopSound();
		}

		

		// Antenna spin on all machines when working
		SFP_StartAntennaSpin();
// Server: start repel loop
		if (GetGame().IsServer())
		{
			if (!SFP_IsOnGround())
			{
				if (GetCompEM()) GetCompEM().SwitchOff();
				return;
			}

			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(SFP_RepelTick);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SFP_RepelTick, SFP_REPEL_TICK_MS, true);
		}

		if (SFP_ENABLE_LEDS) SFP_UpdateLEDStates(true);

	}

	override void OnWorkStop()
	{
		super.OnWorkStop();

		// Stop sound on client
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			SFP_StopLoopSound();
		}

		

		// Stop antenna spin
		SFP_StopAntennaSpin();
// Server: stop repel loop + restore base energy usage
		if (GetGame().IsServer())
		{
			m_SFP_WorkingNet = 0;
			SetSynchDirty();

			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(SFP_RepelTick);

			if (GetCompEM() && m_SFP_BaseEnergyUsage > 0)
				GetCompEM().SetEnergyUsage(m_SFP_BaseEnergyUsage);
		}

		if (SFP_ENABLE_LEDS) SFP_UpdateLEDStates(true);

	}

	// If moved into inventory/hands/cargo -> force off (server)
	override void OnInventoryEnter(Man player)
	{
		super.OnInventoryEnter(player);

		if (!GetGame().IsServer())
			return;

		if (GetCompEM() && GetCompEM().IsWorking())
			GetCompEM().SwitchOff();

		if (SFP_ENABLE_LEDS) SFP_UpdateLEDStates(true);

	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);

		if (GetGame().IsServer())
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(SFP_RepelTick);

		SFP_StopLoopSound();

		// Stop client blink timer
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(SFP_BlinkKillLED);

		// Stop antenna tick
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(SFP_AntennaSpinTick);

	}

	// ---------- Helpers ----------
	protected bool SFP_IsOnGround()
	{
		InventoryLocation loc = new InventoryLocation;
		if (GetInventory().GetCurrentInventoryLocation(loc))
			return loc.GetType() == InventoryLocationType.GROUND;

		return GetHierarchyParent() == null;
	}

	protected float SFP_GetCurrentRadius()
	{
		if (m_SFP_Mode == SFP_MODE_BOOST)
			return SFP_RADIUS_BOOST;

		return SFP_RADIUS_STANDARD;
	}

	protected float SFP_GetStepMin()
	{
		if (m_SFP_Mode == SFP_MODE_BOOST)
			return SFP_STEP_MIN_BST;

		return SFP_STEP_MIN_STD;
	}

	protected float SFP_GetStepMax()
	{
		if (m_SFP_Mode == SFP_MODE_BOOST)
			return SFP_STEP_MAX_BST;

		return SFP_STEP_MAX_STD;
	}

	protected string SFP_GetSoundset()
	{
		if (m_SFP_Mode == SFP_MODE_BOOST)
			return SFP_SOUND_BOOST;

		return SFP_SOUND_STANDARD;
	}

	protected void SFP_CacheBaseEnergyUsage()
	{
		if (m_SFP_BaseEnergyUsage > 0)
			return;

		if (GetCompEM())
			m_SFP_BaseEnergyUsage = GetCompEM().GetEnergyUsage();
	}

	protected void SFP_ApplyModeEnergyUsage()
	{
		if (!GetCompEM())
			return;

		SFP_CacheBaseEnergyUsage();
		if (m_SFP_BaseEnergyUsage <= 0)
			return;

		if (m_SFP_Mode == SFP_MODE_BOOST)
			GetCompEM().SetEnergyUsage(m_SFP_BaseEnergyUsage * SFP_BOOST_ENERGY_MULT);
		else
			GetCompEM().SetEnergyUsage(m_SFP_BaseEnergyUsage);
	}

	protected void SFP_RestartLoopSound()
	{
		SFP_StopLoopSound();
		SFP_StartLoopSound();
	}

	protected void SFP_StartLoopSound()
	{
		if (m_SFP_LoopSound)
			return;

		PlaySoundSetLoop(m_SFP_LoopSound, SFP_GetSoundset(), 0.0, 0.0);
	}

	protected void SFP_StopLoopSound()
	{
		if (!m_SFP_LoopSound)
			return;

		m_SFP_LoopSound.SoundStop();
		m_SFP_LoopSound = null;
	}

	// ----- LED logic -----

	protected CarBattery SFP_GetCarBattery()
	{
		// Preferred: attachment in slot "CarBattery"
		int slotId = InventorySlots.GetSlotIdFromString(SFP_SLOT_POWER);
		if (slotId >= 0)
		{
			EntityAI att = GetInventory().FindAttachment(slotId);
			CarBattery cb = CarBattery.Cast(att);
			if (cb)
				return cb;
		}

		// Fallback: whatever EnergyManager considers the energy source
		if (GetCompEM())
			return CarBattery.Cast(GetCompEM().GetEnergySource());

		return null;
	}

	protected float SFP_GetBatteryLevel0To1()
	{
		CarBattery cb = SFP_GetCarBattery();
		if (!cb)
			return 0.0;

		// CarBattery has EnergyManager; use it as the source of truth
		if (cb.HasEnergyManager() && cb.GetCompEM())
			return cb.GetCompEM().GetEnergy0To1();

		// Fallback for weird configs
		if (cb.HasQuantity())
			return cb.GetQuantityNormalized();

		return 0.0;
	}

protected bool SFP_HasNonEmptyPower()
	{
		CarBattery cb = SFP_GetCarBattery();
		if (!cb)
			return false;

		return SFP_GetBatteryLevel0To1() > 0;
	}

	protected void SFP_UpdateLEDStates(bool forced)
	{
		if (!SFP_ENABLE_LEDS)
			return;

		// CLIENT DOES NOT COMPUTE LED STATES IN MP (prevents flicker)
		if (GetGame().IsMultiplayer() && !GetGame().IsServer())
			return;

		int newEnergy = 0;
		int newWork = 0;
		int newKill = 0;

		if (SFP_HasNonEmptyPower())
			newEnergy = 1;

		if (GetGame().IsServer())
		{
			if (m_SFP_WorkingNet == 1)
				newWork = 1;
		}
		else
		{
			if (GetCompEM() && GetCompEM().IsWorking())
				newWork = 1;
		}

		if (SFP_IsBurnReady())
			newKill = 1;

		bool changed = false;

		if (forced || newEnergy != m_SFP_LED_Energy)
		{
			m_SFP_LED_Energy = newEnergy;
			changed = true;
		}

		if (forced || newWork != m_SFP_LED_Work)
		{
			m_SFP_LED_Work = newWork;
			changed = true;
		}

		if (forced || newKill != m_SFP_LED_Kill)
		{
			m_SFP_LED_Kill = newKill;
			changed = true;
		}

		if (changed)
		{
			SetSynchDirty();

			// SP/local visuals
			if (!GetGame().IsMultiplayer() || GetGame().IsClient())
				if (SFP_ENABLE_LEDS) SFP_ApplyLEDs();
		}
	}

	protected void SFP_SetLED(string selectionName, bool lit, string litColor)
	{
		if (!SFP_ENABLE_LEDS)
			return;

		int idx = GetHiddenSelectionIndex(selectionName);
		if (idx < 0)
			return;

		if (lit)
			SetObjectTexture(idx, litColor);
		else
			SetObjectTexture(idx, SFP_COLOR_LED_OFF);
	}

	// When LEDs are disabled, we still force all LED selections OFF (model defaults can appear lit in MP)
	protected void SFP_ForceLEDOff()
	{
		int idx;

		idx = GetHiddenSelectionIndex(SFP_SEL_LED_ENERGY);
		if (idx >= 0)
			SetObjectTexture(idx, SFP_COLOR_LED_OFF);

		idx = GetHiddenSelectionIndex(SFP_SEL_LED_WORK);
		if (idx >= 0)
			SetObjectTexture(idx, SFP_COLOR_LED_OFF);

		idx = GetHiddenSelectionIndex(SFP_SEL_LED_KILLALL);
		if (idx >= 0)
			SetObjectTexture(idx, SFP_COLOR_LED_OFF);
	}


	protected void SFP_ApplyLEDs()
	{
		if (!SFP_ENABLE_LEDS)
			return;

		// Apply only from synced states (stable in MP)
		SFP_SetLED(SFP_SEL_LED_ENERGY, m_SFP_LED_Energy == 1, SFP_COLOR_LED_RED);
		SFP_SetLED(SFP_SEL_LED_WORK, m_SFP_WorkingNet == 1, SFP_COLOR_LED_GREEN);
		SFP_SetLED(SFP_SEL_LED_KILLALL, m_SFP_LED_Kill == 1, SFP_COLOR_LED_YELLOW);

		// Blinking disabled (MP stability). Keep function as no-op.
		SFP_UpdateKillBlink();
	}
protected void SFP_UpdateKillBlink()
	{
		if (!SFP_ENABLE_LEDS)
			return;

		// Disabled: MP-safe solid yellow.
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(SFP_BlinkKillLED);
		m_SFP_BlinkActive = false;
	}

	void SFP_BlinkKillLED()
	{
		if (!SFP_ENABLE_LEDS)
			return;

		// Stop if no longer ready
		if (m_SFP_LED_Kill != 1)
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(SFP_BlinkKillLED);

		// Stop antenna tick
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(SFP_AntennaSpinTick);
			m_SFP_BlinkActive = false;
			m_SFP_KillBlinkState = false;
			SFP_SetLED(SFP_SEL_LED_KILLALL, false, SFP_COLOR_LED_YELLOW);
			return;
		}

		// Toggle between yellow and off
		m_SFP_KillBlinkState = !m_SFP_KillBlinkState;
		SFP_SetLED(SFP_SEL_LED_KILLALL, m_SFP_KillBlinkState, SFP_COLOR_LED_YELLOW);
	}


	// ----- Antenna animation (looping phase 0..1) -----
	protected void SFP_StartAntennaSpin()
	{
		// Client-side cosmetic in MP (dedicated server shouldn't drive animation)
		if (GetGame().IsMultiplayer() && !GetGame().IsClient())
			return;

		if (m_SFP_AntennaSpinning)
			return;

		m_SFP_AntennaSpinning = true;

		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(SFP_AntennaSpinTick);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SFP_AntennaSpinTick, SFP_ANTENNA_TICK_MS, true);
	}

	protected void SFP_StopAntennaSpin()
	{
		if (GetGame().IsMultiplayer() && !GetGame().IsClient())
			return;

		m_SFP_AntennaSpinning = false;
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(SFP_AntennaSpinTick);

		// park at 0
		m_SFP_AntennaPhase = 0.0;
		SetAnimationPhase(SFP_ANIM_ANTENNA, m_SFP_AntennaPhase);

		if (!SFP_ENABLE_LEDS)
			SFP_ForceLEDOff();


		// Some models reset hiddenSelections on anim update in MP; re-apply LED textures locally
		if (GetGame().IsClient() || !GetGame().IsMultiplayer())
			if (SFP_ENABLE_LEDS) SFP_ApplyLEDs();
	}

	void SFP_AntennaSpinTick()
	{
		if (GetGame().IsMultiplayer() && !GetGame().IsClient())
			return;

		// Client-side cosmetic in MP
		if (!m_SFP_AntennaSpinning)
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(SFP_AntennaSpinTick);
			return;
		}

		if (!GetCompEM() || !GetCompEM().IsWorking())
		{
			SFP_StopAntennaSpin();
			return;
		}

		int now = GetGame().GetTime();
		float dt = (now - m_SFP_AntennaLastMs) / 1000.0;
		m_SFP_AntennaLastMs = now;

		// Clamp dt to avoid jumps on lag spikes
		if (dt < 0) dt = 0;
		if (dt > 0.2) dt = 0.2;

		float step = SFP_ANTENNA_RPS * dt; // 1.0 phase = 1 revolution

		m_SFP_AntennaPhase = m_SFP_AntennaPhase + step;
		while (m_SFP_AntennaPhase >= 1.0)
			m_SFP_AntennaPhase = m_SFP_AntennaPhase - 1.0;

		SetAnimationPhase(SFP_ANIM_ANTENNA, m_SFP_AntennaPhase);

		// Some models reset hiddenSelections on anim update in MP; re-apply LED textures locally
		if (GetGame().IsClient() || !GetGame().IsMultiplayer())
			if (SFP_ENABLE_LEDS) SFP_ApplyLEDs();
	}

// ---------- Repel tick (server) ----------
	void SFP_RepelTick()
	{
		if (!GetGame().IsServer())
			return;

		if (!GetCompEM() || !GetCompEM().IsWorking())
			return;

		if (!SFP_IsOnGround())
		{
			GetCompEM().SwitchOff();
			return;
		}

		vector center = GetPosition();
		float radius = SFP_GetCurrentRadius();

		m_SFP_NearObjects.Clear();
		m_SFP_NearCargos.Clear();
		GetGame().GetObjectsAtPosition3D(center, radius, m_SFP_NearObjects, m_SFP_NearCargos);

		for (int i = 0; i < m_SFP_NearObjects.Count(); i++)
		{
			Object obj = m_SFP_NearObjects[i];
			if (!obj)
				continue;

			DayZCreatureAI creature = DayZCreatureAI.Cast(obj);
			if (!creature)
				continue;

			if (!creature.IsAlive())
				continue;

			vector pos = obj.GetPosition();

			vector dir = pos - center;
			dir[1] = 0;

			float dist = dir.Length();
			if (dist < 0.01)
			{
				dir = Vector(1, 0, 0);
				dist = 1.0;
			}

			if (dist < (radius - SFP_REPEL_BUFFER))
			{
				dir.Normalize();

				float depth = (radius - dist) / radius;
				if (depth < 0.0) depth = 0.0;
				if (depth > 1.0) depth = 1.0;

				float step = SFP_GetStepMin() + (SFP_GetStepMax() - SFP_GetStepMin()) * depth;

				vector newPos = pos + dir * step;
				newPos[1] = GetGame().SurfaceY(newPos[0], newPos[2]);

				// 1.30 port: never push the AI into water, and never drop a creature that stands on a floor or a roof
				// down to the terrain under it (SurfaceY ignores buildings)
				if (SFP_Safety.IsWater(newPos))
					continue;
				if (pos[1] - newPos[1] > 1.0)
					continue;

				obj.SetDirection(dir);
				obj.SetPosition(newPos);
			}
		}

		// Keep LEDs updated as energy drains while working (kill-ready can drop below 80%)
		if (SFP_ENABLE_LEDS) SFP_UpdateLEDStates(false);
	}
};
