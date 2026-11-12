// SFP_Cig_Sys
// Single cigarette item (quantity 0..100)

class SFP_SigaPack_Single : Inventory_Base
{
	protected ParticleSource m_SmokeParticle;
	protected bool m_SFP_IsSmoking;
	// 0=100, 1=95, 2=70, 3=60, 4=50, 5=35, 6=20
	protected int m_SFP_VisualStage;

	static const string MEM_SMOKE_POINT = "smoke_point";
	static const string TEX_INVISIBLE = "#(argb,8,8,3)color(0,0,0,0,ca)";

	// Selection names (primary + common fallback spelling)
	static const string SEL_100 = "ciggarette_100";
	static const string SEL_95  = "ciggarette_95";
	static const string SEL_70  = "ciggarette_70";
	static const string SEL_60  = "ciggarette_60";
	static const string SEL_50  = "ciggarette_50";
	static const string SEL_35  = "ciggarette_35";
	static const string SEL_20  = "ciggarette_20";

	static const string SEL_100_ALT = "cigarette_100";
	static const string SEL_95_ALT  = "cigarette_95";
	static const string SEL_70_ALT  = "cigarette_70";
	static const string SEL_60_ALT  = "cigarette_60";
	static const string SEL_50_ALT  = "cigarette_50";
	static const string SEL_35_ALT  = "cigarette_35";
	static const string SEL_20_ALT  = "cigarette_20";

	// cached original textures for hidden selections (if available)
	protected bool m_SFP_TexCached;
	protected string m_TexSel100;
	protected string m_TexSel95;
	protected string m_TexSel70;
	protected string m_TexSel60;
	protected string m_TexSel50;
	protected string m_TexSel35;
	protected string m_TexSel20;
	protected string m_TexSel100A;
	protected string m_TexSel95A;
	protected string m_TexSel70A;
	protected string m_TexSel60A;
	protected string m_TexSel50A;
	protected string m_TexSel35A;
	protected string m_TexSel20A;

	void SFP_SigaPack_Single()
	{
		RegisterNetSyncVariableBool("m_SFP_IsSmoking");
		RegisterNetSyncVariableInt("m_SFP_VisualStage", 0, 6);
	}

	// Ensure visuals are applied after model is ready (client-side)
	protected void SFP_RequestVisualRefresh()
	{
		if (GetGame().IsDedicatedServer())
			return;

		// Best-effort stage before net-sync arrives (purely visual)
		m_SFP_VisualStage = SFP_GetStageFromQuantity(GetQuantity());

		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(SFP_RefreshVisuals_Client, 0, false);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(SFP_RefreshVisuals_Client, 50, false);
	}

	protected void SFP_RefreshVisuals_Client()
	{
		if (GetGame().IsDedicatedServer())
			return;
		SFP_CacheOriginalTextures();
		ApplyVisualStage();
	}

	override void EEInit()
	{
		super.EEInit();

		if (GetGame().IsServer())
			SFP_UpdateVisualStage_Server(true);

		if (!GetGame().IsDedicatedServer())
			SFP_RequestVisualRefresh();
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;

		// After persistence load, quantity is now valid -> update stage
		if (GetGame().IsServer())
			SFP_UpdateVisualStage_Server(true);
		else
			SFP_RequestVisualRefresh();

		return true;
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();

		if (GetGame().IsDedicatedServer())
			return;

		// smoke
		if (m_SFP_IsSmoking)
			StartSmokingParticle();
		else
			StopSmokingParticle();

		// visuals (delay to ensure model selections are ready)
		SFP_RequestVisualRefresh();
	}

	override void OnQuantityChanged(float delta)
	{
		super.OnQuantityChanged(delta);

		if (GetGame().IsServer())
			SFP_UpdateVisualStage_Server();
		else
			SFP_RequestVisualRefresh();
	}

	override void EEItemLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
	{
		super.EEItemLocationChanged(oldLoc, newLoc);

		if (!GetGame().IsDedicatedServer())
		{
			if (newLoc.GetType() != InventoryLocationType.HANDS)
				StopSmokingParticle();
		}
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		if (!GetGame().IsDedicatedServer())
			StopSmokingParticle();
	}

	// Prevent stacking/splitting
	override bool CanBeSplit()
	{
		return false;
	}

	override bool CanBeCombined(EntityAI other_item, bool reservation_check = true, bool stack_max_limit = false)
	{
		return false;
	}

	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionSFP_SmokeCigarette);
	}

	// -----------------------
	// Smoking state + particle
	// -----------------------
	void SetSmokingState(bool state)
	{
		if (!GetGame().IsServer())
			return;
		m_SFP_IsSmoking = state;
		SetSynchDirty();
	}

	void StartSmokingParticle()
	{
		if (GetGame().IsDedicatedServer())
			return;
		if (m_SmokeParticle)
			return;

		vector localPos = GetMemoryPointPos(MEM_SMOKE_POINT);
		m_SmokeParticle = ParticleManager.GetInstance().PlayOnObject(ParticleList.SFP_SMOKE_SIGA_PART, this, localPos);
	}

	void StopSmokingParticle()
	{
		if (m_SmokeParticle)
		{
			m_SmokeParticle.Stop();
			m_SmokeParticle = null;
		}
	}

	override void DeleteSafe()
	{
		if (GetGame() && GetGame().IsServer())
			GetGame().ObjectDelete(this);
	}

	// -----------------------
	// Visual stage (server authoritative)
	// -----------------------
	protected int SFP_GetStageFromQuantity(float q)
	{
		// 100  -> SEL_100
		// 95   -> SEL_95
		// 70   -> SEL_70
		// 60   -> SEL_60
		// 50   -> SEL_50
		// 35   -> SEL_35
		// 20   -> SEL_20
		if (q > 95)
			return 0;
		if (q > 70)
			return 1;
		if (q > 60)
			return 2;
		if (q > 50)
			return 3;
		if (q > 35)
			return 4;
		if (q > 20)
			return 5;
		return 6;
	}

	void SFP_UpdateVisualStage_Server(bool force = false)
	{
		if (!GetGame().IsServer())
			return;

		int newStage = SFP_GetStageFromQuantity(GetQuantity());
		if (force || newStage != m_SFP_VisualStage)
		{
			m_SFP_VisualStage = newStage;
			SetSynchDirty();
		}
	}

	// -----------------------
	// Visuals
	// -----------------------
	protected void SFP_CacheOriginalTextures()
	{
		if (m_SFP_TexCached)
			return;

		m_TexSel100  = SFP_GetOriginalTexture(SEL_100);
		m_TexSel95   = SFP_GetOriginalTexture(SEL_95);
		m_TexSel70   = SFP_GetOriginalTexture(SEL_70);
		m_TexSel60   = SFP_GetOriginalTexture(SEL_60);
		m_TexSel50   = SFP_GetOriginalTexture(SEL_50);
		m_TexSel35   = SFP_GetOriginalTexture(SEL_35);
		m_TexSel20   = SFP_GetOriginalTexture(SEL_20);
		m_TexSel100A = SFP_GetOriginalTexture(SEL_100_ALT);
		m_TexSel95A  = SFP_GetOriginalTexture(SEL_95_ALT);
		m_TexSel70A  = SFP_GetOriginalTexture(SEL_70_ALT);
		m_TexSel60A  = SFP_GetOriginalTexture(SEL_60_ALT);
		m_TexSel50A  = SFP_GetOriginalTexture(SEL_50_ALT);
		m_TexSel35A  = SFP_GetOriginalTexture(SEL_35_ALT);
		m_TexSel20A  = SFP_GetOriginalTexture(SEL_20_ALT);

		m_SFP_TexCached = true;
	}

	protected string SFP_GetOriginalTexture(string sel_name)
	{
		int idx = GetHiddenSelectionIndex(sel_name);
		if (idx < 0)
			return string.Empty;

		// try config first
		string cfg = "CfgVehicles " + GetType();
		array<string> sels = new array<string>;
		array<string> texs = new array<string>;
		if (GetGame().ConfigIsExisting(cfg + " hiddenSelections") && GetGame().ConfigIsExisting(cfg + " hiddenSelectionsTextures"))
		{
			GetGame().ConfigGetTextArray(cfg + " hiddenSelections", sels);
			GetGame().ConfigGetTextArray(cfg + " hiddenSelectionsTextures", texs);
			int cfgIdx = sels.Find(sel_name);
			if (cfgIdx > -1 && cfgIdx < texs.Count())
				return texs.Get(cfgIdx);
		}

		// fallback: current runtime texture
		return GetObjectTexture(idx);
	}

	protected void SFP_SetSelectionState(string sel_name, bool active)
	{
		int idx = GetHiddenSelectionIndex(sel_name);
		if (idx >= 0)
		{
			// This is a hiddenSelection -> hide/show by texture override ONLY.
			if (active)
			{
				string tex;
				switch (sel_name)
				{
					case SEL_100:     tex = m_TexSel100;  break;
					case SEL_95:      tex = m_TexSel95;   break;
					case SEL_70:      tex = m_TexSel70;   break;
					case SEL_60:      tex = m_TexSel60;   break;
					case SEL_50:      tex = m_TexSel50;   break;
					case SEL_35:      tex = m_TexSel35;   break;
					case SEL_20:      tex = m_TexSel20;   break;
					case SEL_100_ALT: tex = m_TexSel100A; break;
					case SEL_95_ALT:  tex = m_TexSel95A;  break;
					case SEL_70_ALT:  tex = m_TexSel70A;  break;
					case SEL_60_ALT:  tex = m_TexSel60A;  break;
					case SEL_50_ALT:  tex = m_TexSel50A;  break;
					case SEL_35_ALT:  tex = m_TexSel35A;  break;
					case SEL_20_ALT:  tex = m_TexSel20A;  break;
				}

				// If we don't know the original texture, clearing the override is safer than keeping invisible
				if (tex != string.Empty)
					SetObjectTexture(idx, tex);
				else
					SetObjectTexture(idx, "");
			}
			else
			{
				SetObjectTexture(idx, TEX_INVISIBLE);
			}
			return;
		}

		// Not a hiddenSelection -> hide/show by selection toggle
		if (active)
			ShowSelection(sel_name);
		else
			HideSelection(sel_name);
	}

	protected void ApplyVisualStage()
	{
		SFP_CacheOriginalTextures();

		// decide which selection should be active
		string activeA;
		string activeB;
		switch (m_SFP_VisualStage)
		{
			case 0: activeA = SEL_100; activeB = SEL_100_ALT; break;
			case 1: activeA = SEL_95;  activeB = SEL_95_ALT;  break;
			case 2: activeA = SEL_70;  activeB = SEL_70_ALT;  break;
			case 3: activeA = SEL_60;  activeB = SEL_60_ALT;  break;
			case 4: activeA = SEL_50;  activeB = SEL_50_ALT;  break;
			case 5: activeA = SEL_35;  activeB = SEL_35_ALT;  break;
			default: activeA = SEL_20; activeB = SEL_20_ALT;  break;
		}

		// Apply state to all known names
		SFP_SetSelectionState(SEL_100,     (SEL_100     == activeA) || (SEL_100     == activeB));
		SFP_SetSelectionState(SEL_100_ALT, (SEL_100_ALT == activeA) || (SEL_100_ALT == activeB));

		SFP_SetSelectionState(SEL_95,      (SEL_95      == activeA) || (SEL_95      == activeB));
		SFP_SetSelectionState(SEL_95_ALT,  (SEL_95_ALT  == activeA) || (SEL_95_ALT  == activeB));

		SFP_SetSelectionState(SEL_70,      (SEL_70      == activeA) || (SEL_70      == activeB));
		SFP_SetSelectionState(SEL_70_ALT,  (SEL_70_ALT  == activeA) || (SEL_70_ALT  == activeB));
		SFP_SetSelectionState(SEL_60,      (SEL_60      == activeA) || (SEL_60      == activeB));
		SFP_SetSelectionState(SEL_60_ALT,  (SEL_60_ALT  == activeA) || (SEL_60_ALT  == activeB));
		SFP_SetSelectionState(SEL_50,      (SEL_50      == activeA) || (SEL_50      == activeB));
		SFP_SetSelectionState(SEL_50_ALT,  (SEL_50_ALT  == activeA) || (SEL_50_ALT  == activeB));
		SFP_SetSelectionState(SEL_35,      (SEL_35      == activeA) || (SEL_35      == activeB));
		SFP_SetSelectionState(SEL_35_ALT,  (SEL_35_ALT  == activeA) || (SEL_35_ALT  == activeB));
		SFP_SetSelectionState(SEL_20,      (SEL_20      == activeA) || (SEL_20      == activeB));
		SFP_SetSelectionState(SEL_20_ALT,  (SEL_20_ALT  == activeA) || (SEL_20_ALT  == activeB));
	}
};
