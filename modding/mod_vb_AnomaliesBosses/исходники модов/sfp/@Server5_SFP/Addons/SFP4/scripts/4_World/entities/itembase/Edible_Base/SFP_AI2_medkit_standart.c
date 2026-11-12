// AI2 Medkit item behaviour
// Classname must match CfgVehicles class name: "SFP_AI2_medkit_standart"
//
// Lid control for model selections/hiddenSelections:
// - lid_closed visible by default (inventory/hands)
// - lid_open visible only while action is running
//
// NOTE:
// Many mods accidentally mismatch the selection name casing/spaces between model.cfg and config.cpp.
// This script resolves indices in a tolerant way (case-insensitive + substring fallback).

class SFP_ai2medkit_Base : Edible_Base
{
	// replicated so clients can apply visuals too
	protected bool m_AI2_LidOpen = false;

	// cache
	protected bool   m_AI2_LidCacheReady = false;
	protected int    m_AI2_LidOpenIdx = -1;
	protected int    m_AI2_LidClosedIdx = -1;
	protected string m_AI2_LidOpenTex = "";
	protected string m_AI2_LidOpenMat = "";
	protected string m_AI2_LidClosedTex = "";
	protected string m_AI2_LidClosedMat = "";

	// procedural fully transparent texture (no extra files needed)
	static const string AI2_TEX_INVISIBLE = "#(argb,8,8,3)color(0,0,0,0,ca)";

	void SFP_ai2medkit_Base()
	{
		RegisterNetSyncVariableBool("m_AI2_LidOpen");
	}

	override void SetActions()
	{
		super.SetActions();

		// Optional: allow force-consuming (feeding) like other tablets
		AddAction(ActionForceConsumeSingle);

		// Custom action: works with masks + uses tablet animation
		AddAction(ActionAI2UseMedkit);
	}

	override void OnConsume(float amount, PlayerBase consumer)
	{
		super.OnConsume(amount, consumer);

		if (!GetGame().IsServer() || !consumer)
			return;

		// 1) Painkiller effect (same behaviour as PainkillerTablets, timer reset)
		ModifiersManager mm = consumer.GetModifiersManager();
		if (mm)
		{
			if (mm.IsModifierActive(eModifiers.MDF_PAINKILLERS))
				mm.DeactivateModifier(eModifiers.MDF_PAINKILLERS);

			mm.ActivateModifier(eModifiers.MDF_PAINKILLERS);
		}

		// 2) Healing: +20 instantly, then +2/sec for 5 seconds
		AI2_Medkit_ApplyHealing(consumer, amount);
	}

	// ------------------------------------------------------------
	// Lid visuals

	override void EEInit()
	{
		super.EEInit();

		AI2_InitLidCache();

		// Default closed on spawn
		if (GetGame().IsServer())
		{
			m_AI2_LidOpen = false;
			SetSynchDirty();
		}

		AI2_ApplyLidVisual(false);
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();

		AI2_InitLidCache();
		AI2_ApplyLidVisual(m_AI2_LidOpen);
	}

	override void OnItemLocationChanged(EntityAI old_owner, EntityAI new_owner)
	{
		super.OnItemLocationChanged(old_owner, new_owner);

		// When moved around (hands/inventory/ground), force closed
		if (GetGame().IsServer())
		{
			if (m_AI2_LidOpen)
			{
				m_AI2_LidOpen = false;
				SetSynchDirty();
			}
		}

		AI2_ApplyLidVisual(false);
	}

	// Called from action on SERVER (authoritative + sync)
	void AI2_SetLidOpen_Server(bool open)
	{
		if (!GetGame().IsServer())
			return;

		AI2_InitLidCache();

		if (m_AI2_LidOpen == open)
			return;

		m_AI2_LidOpen = open;
		SetSynchDirty();

		AI2_ApplyLidVisual(open);
	}

	// Called from action on CLIENT (instant local feedback; server will sync for everyone)
	void AI2_SetLidOpen_Local(bool open)
	{
		AI2_InitLidCache();
		AI2_ApplyLidVisual(open);
	}

	// Tolerant index resolver (case-insensitive + substring fallback)
	protected int AI2_FindHiddenSelectionIndexSmart(string key)
	{
		// 1) engine direct
		int idx = GetHiddenSelectionIndex(key);
		if (idx >= 0)
			return idx;

		// 2) config-based scan
		HiddenSelectionsData hsd = GetHiddenSelectionsData();
		if (!hsd)
			return -1;

		string keyL = key;
		keyL.ToLower();

		// exact, case-insensitive
		for (int i = 0; i < hsd.m_HiddenSelections.Count(); i++)
		{
			string s = hsd.m_HiddenSelections[i];
			string sL = s;
			sL.ToLower();
			if (sL == keyL)
				return i;
		}

		// substring (handles suffix/prefix like "lid_open_0")
		for (i = 0; i < hsd.m_HiddenSelections.Count(); i++)
		{
			s = hsd.m_HiddenSelections[i];
			sL = s;
			sL.ToLower();
			if (sL.Contains(keyL))
				return i;
		}

		// heuristic: lid + open/close
		if (keyL.Contains("open"))
		{
			for (i = 0; i < hsd.m_HiddenSelections.Count(); i++)
			{
				s = hsd.m_HiddenSelections[i];
				sL = s;
				sL.ToLower();
				if (sL.Contains("lid") && sL.Contains("open"))
					return i;
			}
		}
		if (keyL.Contains("close"))
		{
			for (i = 0; i < hsd.m_HiddenSelections.Count(); i++)
			{
				s = hsd.m_HiddenSelections[i];
				sL = s;
				sL.ToLower();
				if (sL.Contains("lid") && (sL.Contains("close") || sL.Contains("closed")))
					return i;
			}
		}

		return -1;
	}

	protected void AI2_InitLidCache()
	{
		if (m_AI2_LidCacheReady)
			return;

		m_AI2_LidOpenIdx   = AI2_FindHiddenSelectionIndexSmart("lid_open");
		m_AI2_LidClosedIdx = AI2_FindHiddenSelectionIndexSmart("lid_closed");

		// Helpful debug (goes to script log)
		SFP_Log.Debug(string.Format("[AI2_Medkit] %1 resolved idx: open=%2 closed=%3", this, m_AI2_LidOpenIdx, m_AI2_LidClosedIdx));

		// Cache current visuals (before we start hiding anything)
		if (m_AI2_LidOpenIdx >= 0)
		{
			m_AI2_LidOpenTex = GetObjectTexture(m_AI2_LidOpenIdx);
			m_AI2_LidOpenMat = GetObjectMaterial(m_AI2_LidOpenIdx);
		}

		if (m_AI2_LidClosedIdx >= 0)
		{
			m_AI2_LidClosedTex = GetObjectTexture(m_AI2_LidClosedIdx);
			m_AI2_LidClosedMat = GetObjectMaterial(m_AI2_LidClosedIdx);
		}

		// Fallback to config arrays if engine returns empty strings
		HiddenSelectionsData hsd = GetHiddenSelectionsData();
		if (hsd)
		{
			TStringArray texs = hsd.GetHiddenSelectionsTextures();
			TStringArray mats = hsd.GetHiddenSelectionsMaterials();

			if (m_AI2_LidOpenIdx >= 0)
			{
				if (m_AI2_LidOpenTex == "" && texs && m_AI2_LidOpenIdx < texs.Count())
					m_AI2_LidOpenTex = texs[m_AI2_LidOpenIdx];
				if (m_AI2_LidOpenMat == "" && mats && m_AI2_LidOpenIdx < mats.Count())
					m_AI2_LidOpenMat = mats[m_AI2_LidOpenIdx];
			}

			if (m_AI2_LidClosedIdx >= 0)
			{
				if (m_AI2_LidClosedTex == "" && texs && m_AI2_LidClosedIdx < texs.Count())
					m_AI2_LidClosedTex = texs[m_AI2_LidClosedIdx];
				if (m_AI2_LidClosedMat == "" && mats && m_AI2_LidClosedIdx < mats.Count())
					m_AI2_LidClosedMat = mats[m_AI2_LidClosedIdx];
			}

			// Also print the selection list once (helps if open name differs)
			string list = "";
			for (int j = 0; j < hsd.m_HiddenSelections.Count(); j++)
			{
				list = list + string.Format("%1:%2 ", j, hsd.m_HiddenSelections[j]);
			}
			SFP_Log.Debug(string.Format("[AI2_Medkit] hiddenSelections: %1", list));
		}

		SFP_Log.Debug(string.Format("[AI2_Medkit] cached open(tex='%1' mat='%2') closed(tex='%3' mat='%4')", m_AI2_LidOpenTex, m_AI2_LidOpenMat, m_AI2_LidClosedTex, m_AI2_LidClosedMat));

		m_AI2_LidCacheReady = true;
	}

	protected void AI2_SetIdxVisibleSafe(int idx, bool visible, string tex, string mat)
	{
		if (idx < 0)
			return;

		if (visible)
		{
			if (tex != "")
				SetObjectTexture(idx, tex);
			if (mat != "")
				SetObjectMaterial(idx, mat);
		}
		else
		{
			// Transparent procedural texture; this works on most DayZ materials that use texture alpha
			SetObjectTexture(idx, AI2_TEX_INVISIBLE);
		}
	}

	protected void AI2_ApplyLidVisual(bool open)
	{
		// Index-based swap (hiddenSelections)
		if (open)
		{
			AI2_SetIdxVisibleSafe(m_AI2_LidOpenIdx, true,  m_AI2_LidOpenTex,   m_AI2_LidOpenMat);
			AI2_SetIdxVisibleSafe(m_AI2_LidClosedIdx, false, "", "");
		}
		else
		{
			AI2_SetIdxVisibleSafe(m_AI2_LidOpenIdx, false, "", "");
			AI2_SetIdxVisibleSafe(m_AI2_LidClosedIdx, true,  m_AI2_LidClosedTex, m_AI2_LidClosedMat);
		}

		// Extra fallback paths (in case model is wired via hide animations / named selections)
		// These calls are safe no-ops if not supported by the model.
		if (open)
		{
			HideSelection("lid_closed");
			ShowSelection("lid_open");
			SetAnimationPhase("lid_closed", 1);
			SetAnimationPhase("lid_open", 0);
		}
		else
		{
			HideSelection("lid_open");
			ShowSelection("lid_closed");
			SetAnimationPhase("lid_open", 1);
			SetAnimationPhase("lid_closed", 0);
		}
	}
}
class SFP_AI2_medkit_standart : SFP_ai2medkit_Base {}

class SFP_AI2_medkit_Yello : SFP_AI2_medkit_standart {}
class SFP_AI2_medkit_Blue  : SFP_AI2_medkit_standart {}
class SFP_AI2_medkit_Green : SFP_AI2_medkit_standart {}
class SFP_AI2_medkit_Red   : SFP_AI2_medkit_standart {}