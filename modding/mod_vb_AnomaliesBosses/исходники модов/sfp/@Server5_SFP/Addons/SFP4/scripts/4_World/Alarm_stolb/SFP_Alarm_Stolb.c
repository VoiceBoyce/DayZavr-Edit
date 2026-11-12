//==============================================================================
// SFP_Alarm_Stolb (House)
// - Player action: toggle alarm (on/off)
// - Plays configured soundset from memory point "sound_point"
// - Auto-off after configured duration (default 120s)
//==============================================================================

class SFP_Alarm_Stolb extends House
{
	protected const string SELECTION_TUMBLER_ON = "tumbler_on";
	protected const string SELECTION_TUMBLER_OFF = "tumbler_off";

	// Net-synced
	protected bool m_IsAlarmOn;
	protected int m_SoundType;

	// Server-only
	protected int m_AutoOffSeconds;
	protected ref Timer m_AutoOffTimer;

	// Client-only
	protected EffectSound m_AlarmSound;

	void SFP_Alarm_Stolb()
	{
		m_IsAlarmOn = false;
		m_SoundType = 3;
		m_AutoOffSeconds = 120;

		RegisterNetSyncVariableBool("m_IsAlarmOn");
		RegisterNetSyncVariableInt("m_SoundType", 1, 4);
	}

	override void EEInit()
	{
		super.EEInit();

		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			// Register this pole in JSON (and load its settings)
			SFP_AlarmStolbConfigManager mgr = SFP_AlarmStolbConfigManager.GetInstance();
			SFP_AlarmStolbPoleConfig cfg = mgr.GetOrCreateByPosition(GetPosition());

			int st = mgr.SanitizeSoundType(cfg.SoundType);
			int off = mgr.SanitizeAutoOffSeconds(cfg.AutoOffSeconds);

			bool cfg_dirty = false;
			if (cfg.SoundType != st)
			{
				cfg.SoundType = st;
				cfg_dirty = true;
			}
			if (cfg.AutoOffSeconds != off)
			{
				cfg.AutoOffSeconds = off;
				cfg_dirty = true;
			}

			if (cfg_dirty)
				mgr.Save();

			SetAlarmConfig(st, off);
			SetSynchDirty();
		}

		// Client-side init (sound + visuals)
		UpdateAlarmSound();
		UpdateTumblerVisual();
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		UpdateAlarmSound();
		UpdateTumblerVisual();
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);

		if (m_AutoOffTimer)
			m_AutoOffTimer.Stop();

		StopAlarmSound();
	}

	//---------------------------------------------------------------------------
	// ACTIONS
	//---------------------------------------------------------------------------
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionSFP_AlarmStolbToggle);
	}

	//---------------------------------------------------------------------------
	// STATE
	//---------------------------------------------------------------------------
	bool IsAlarmOn()
	{
		return m_IsAlarmOn;
	}

	int GetSoundType()
	{
		return m_SoundType;
	}

	int GetAutoOffSeconds()
	{
		return m_AutoOffSeconds;
	}

	void SetAlarmConfig(int soundType, int autoOffSeconds)
	{
		m_SoundType = Math.Clamp(soundType, 1, 4);
		m_AutoOffSeconds = autoOffSeconds;

		if (m_AutoOffSeconds <= 0)
			m_AutoOffSeconds = 120;
	}

	// Called from action (server/SP)
	void ToggleAlarmServer()
	{
		if (!(GetGame().IsServer() || !GetGame().IsMultiplayer()))
			return;

		if (m_IsAlarmOn)
			TurnOffServer();
		else
			TurnOnServer();
	}

	protected void TurnOnServer()
	{
		if (m_IsAlarmOn)
			return;

		m_IsAlarmOn = true;
		SetSynchDirty();

		// Auto-off timer
		if (!m_AutoOffTimer)
			m_AutoOffTimer = new Timer(CALL_CATEGORY_SYSTEM);

		m_AutoOffTimer.Stop();
		m_AutoOffTimer.Run(m_AutoOffSeconds, this, "AutoOff", NULL, false);

		// SP / listen-server: update local audio/visual immediately
		UpdateAlarmSound();
		UpdateTumblerVisual();
	}

	protected void TurnOffServer()
	{
		if (!m_IsAlarmOn)
			return;

		m_IsAlarmOn = false;
		SetSynchDirty();

		if (m_AutoOffTimer)
			m_AutoOffTimer.Stop();

		// SP / listen-server: update local audio/visual immediately
		UpdateAlarmSound();
		UpdateTumblerVisual();
	}

	void AutoOff()
	{
		// Timer callback (server/SP)
		TurnOffServer();
	}

	//---------------------------------------------------------------------------
	// SOUNDS (client/SP)
	//---------------------------------------------------------------------------
	protected void UpdateAlarmSound()
	{
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;

		if (m_IsAlarmOn)
			StartAlarmSound();
		else
			StopAlarmSound();
	}

	protected void StartAlarmSound()
	{
		if (GetGame().IsDedicatedServer())
			return;

		// Safety: ensure no orphaned loop keeps playing (engine can keep a registered env sound alive)
		ForceStopAndDestroyAlarmSound();

		string soundSet = GetSoundSetByType(m_SoundType);
		if (soundSet == string.Empty)
			return;

		// plays from memory point inside model
		PlaySoundSetAtMemoryPointLoopedSafe(m_AlarmSound, soundSet, "sound_point", 0, 0);
		if (m_AlarmSound)
		{
			// Match vanilla PlaySoundSet behaviour (avoids leaked EffectSound refs)
			m_AlarmSound.SetAutodestroy(true);
		}
	}

	protected void StopAlarmSound()
	{
		if (GetGame().IsDedicatedServer())
			return;

		ForceStopAndDestroyAlarmSound();
	}

	// Hard stop + unregister (prevents looped env sounds from lingering)
	protected void ForceStopAndDestroyAlarmSound()
	{
		if (m_AlarmSound)
		{
			m_AlarmSound.SoundStop();
			SEffectManager.DestroyEffect(m_AlarmSound);
			m_AlarmSound = null;
		}
	}

//---------------------------------------------------------------------------
// VISUALS (client/SP) - tumbler selections
// - When alarm OFF: show tumbler_off, hide tumbler_on
// - When alarm ON : show tumbler_on,  hide tumbler_off
//---------------------------------------------------------------------------
protected void UpdateTumblerVisual()
{
	if (!GetGame() || GetGame().IsDedicatedServer())
		return;

	int idxOn = GetHiddenSelectionIndex(SELECTION_TUMBLER_ON);
	int idxOff = GetHiddenSelectionIndex(SELECTION_TUMBLER_OFF);

	if (idxOn < 0 || idxOff < 0)
		return;

	if (m_IsAlarmOn)
	{
		ShowSelectionTexture(idxOn);
		HideSelectionTexture(idxOff);
	}
	else
	{
		ShowSelectionTexture(idxOff);
		HideSelectionTexture(idxOn);
	}
}

protected void ShowSelectionTexture(int selectionIdx)
{
	if (selectionIdx < 0)
		return;

	TStringArray textures = GetHiddenSelectionsData().GetHiddenSelectionsTextures();
	if (textures && selectionIdx < textures.Count())
	{
		// Restore default texture from CfgVehicles hiddenSelectionsTextures
		SetObjectTexture(selectionIdx, textures[selectionIdx]);
	}
}

protected void HideSelectionTexture(int selectionIdx)
{
	if (selectionIdx < 0)
		return;

	// Vanilla way to hide selection: set empty texture string
	SetObjectTexture(selectionIdx, "");
}

	protected string GetSoundSetByType(int type)
	{
		switch (type)
		{
			case 1:
				return "SFP_Alarm_Sound_Type1_Soundset";
			case 2:
				return "SFP_Alarm_Sound_Type2_Soundset";
			case 3:
				return "SFP_Alarm_Sound_Type3_Soundset";
			case 4:
				return "SFP_Alarm_Sound_Type4_Soundset";
		}

		return "SFP_Alarm_Sound_Type3_Soundset";
	}
};
