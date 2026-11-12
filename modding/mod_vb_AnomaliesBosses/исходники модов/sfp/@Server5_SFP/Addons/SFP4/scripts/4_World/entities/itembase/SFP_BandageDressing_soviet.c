class SFP_BandageDressing_soviet extends Inventory_Base
{
	protected bool m_PackHidden;

	// Indices for different hiding systems (depends on how the model/config is set up)
	protected int m_PackSimpleSelIdx;
	protected int m_PackHiddenSelIdx;
	protected string m_PackOrigTexture;

	void SFP_BandageDressing_soviet()
	{
		// Sync pack state to clients
		RegisterNetSyncVariableBool("m_PackHidden");
		m_PackHidden = false;

		m_PackSimpleSelIdx = -1;
		m_PackHiddenSelIdx = -1;
		m_PackOrigTexture = string.Empty;
	}

	override void EEInit()
	{
		super.EEInit();

		CachePackSelections();

		// Force default VISIBLE on init (server authoritative)
		if (GetGame() && GetGame().IsServer())
		{
			m_PackHidden = false;
			SetSynchDirty();
		}

		ApplyPackVisual();
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;

		CachePackSelections();

		// After restart/load keep it VISIBLE
		if (GetGame() && GetGame().IsServer())
		{
			m_PackHidden = false;
			SetSynchDirty();
		}

		ApplyPackVisual();
		return true;
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		CachePackSelections();
		ApplyPackVisual();
	}

	protected void CachePackSelections()
	{
		m_PackSimpleSelIdx = -1;
		m_PackHiddenSelIdx = -1;
		m_PackOrigTexture = string.Empty;

		// 1) simpleHiddenSelections (Bandana/Shemag style)
		if (ConfigIsExisting("simpleHiddenSelections"))
		{
			array<string> simpleSels = new array<string>;
			ConfigGetTextArray("simpleHiddenSelections", simpleSels);

			for (int i = 0; i < simpleSels.Count(); i++)
			{
				if (simpleSels[i] == "pack_hide")
				{
					m_PackSimpleSelIdx = i;
					break;
				}
			}
		}

		// 2) hiddenSelections index (SetObjectTexture) + (sometimes) HideSelection/ShowSelection
		m_PackHiddenSelIdx = GetHiddenSelectionIndex("pack_hide");
		if (m_PackHiddenSelIdx >= 0)
		{
			array<string> tex = GetHiddenSelectionsTextures();
			if (tex && m_PackHiddenSelIdx < tex.Count())
				m_PackOrigTexture = tex.Get(m_PackHiddenSelIdx);
		}
	}

	protected void ApplyPackVisual()
	{
		// AI2-style approach: server sync var + OnVariablesSynchronized drives visuals
		// Primary: HideSelection/ShowSelection (requires pack_hide to be configured as hiddenSelection/section)
		// Fallback: simpleHiddenSelections
		// Last resort: swap texture to a fully transparent procedural texture (no rvmat swap)

		if (m_PackHidden)
		{
			// Animation source (if your model uses it)
			SetAnimationPhase("pack_hide", 1.0);

			// Section hide (AI2 style)
			HideSelection("pack_hide");

			// simpleHiddenSelections (true = hidden)
			if (m_PackSimpleSelIdx >= 0)
				SetSimpleHiddenSelectionState(m_PackSimpleSelIdx, true);

			// Transparent texture fallback (won't turn white like empty string)
			if (m_PackHiddenSelIdx >= 0)
				SetObjectTexture(m_PackHiddenSelIdx, "#(argb,8,8,3)color(1,1,1,0,ca)");
		}
		else
		{
			SetAnimationPhase("pack_hide", 0.0);

			ShowSelection("pack_hide");

			if (m_PackSimpleSelIdx >= 0)
				SetSimpleHiddenSelectionState(m_PackSimpleSelIdx, false);

			if (m_PackHiddenSelIdx >= 0 && m_PackOrigTexture != string.Empty)
				SetObjectTexture(m_PackHiddenSelIdx, m_PackOrigTexture);
		}
	}

	void SetPackHidden(bool hidden)
	{
		if (m_PackHidden == hidden)
			return;

		m_PackHidden = hidden;
		ApplyPackVisual();

		if (GetGame() && GetGame().IsServer())
			SetSynchDirty();
	}

//================================================================
	// IGNITION ACTIONS (vanilla-like)
	//================================================================
	override bool HasFlammableMaterial()
	{
		return true;
	}

	override bool CanBeIgnitedBy(EntityAI igniter = NULL)
	{
		if (GetHierarchyParent())
			return false;

		return true;
	}

	override bool CanIgniteItem(EntityAI ignite_target = NULL)
	{
		return false;
	}

	override void OnIgnitedTarget(EntityAI target_item)
	{
	}

	override void OnIgnitedThis(EntityAI fire_source)
	{
		Fireplace.IgniteEntityAsFireplace(this, fire_source);
	}

	override bool IsThisIgnitionSuccessful(EntityAI item_source = NULL)
	{
		return Fireplace.CanIgniteEntityAsFireplace(this);
	}

	override void SetActions()
	{
		super.SetActions();

		AddAction(ActionSFPBandageSelfRun);

		// keep useful vanilla actions from BandageDressing
		AddAction(ActionCreateIndoorFireplace);
		AddAction(ActionCreateIndoorOven);
		AddAction(ActionAttach);
		AddAction(ActionDetach);
	}

	override float GetBandagingEffectivity()
	{
		return 2.0;
	}

	override bool CanBeDisinfected()
	{
		return true;
	}

	// No wound infection from closing bleeding sources with this item
	override float GetInfectionChance(int system = 0, Param param = null)
	{
		return 0.0;
	}

	override void OnCombine(ItemBase other_item)
	{
		super.OnCombine(other_item);

		// keep cleanness state consistent when combining stacks
		if (m_Cleanness == 1 && other_item && other_item.m_Cleanness == 0)
			SetCleanness(0);
	}
};
