// SFP_Cig_Sys
// Cigarette pack base item (20 pcs)

class SFP_SigaPack_Base : Inventory_Base
{
	static const string TEX_INVISIBLE = "#(argb,8,8,3)color(0,0,0,0,ca)";

	// Visual count of cigarettes in the pack (0..20), server authoritative
	protected int m_SFP_PackVisualCount;

	// cached original textures for hidden selections (if available)
	protected bool m_SFP_TexCached;
	protected ref map<string, string> m_SFP_OrigTexBySel;
	protected ref array<string> m_SFP_SelNames;

	void SFP_SigaPack_Base()
	{
		RegisterNetSyncVariableInt("m_SFP_PackVisualCount", 0, 20);
		m_SFP_OrigTexBySel = new map<string, string>;
		m_SFP_SelNames = new array<string>;
		SFP_BuildSelectionList();
	}

	// Ensure pack visuals are applied after model is ready (client-side)
	protected void SFP_RequestPackVisualRefresh()
	{
		if (GetGame().IsDedicatedServer())
			return;

		// Best-effort count before net-sync arrives (purely visual)
		m_SFP_PackVisualCount = SFP_GetCountFromQuantity(GetQuantity());

		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(SFP_RefreshPackVisuals_Client, 0, false);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(SFP_RefreshPackVisuals_Client, 50, false);
	}

	protected void SFP_RefreshPackVisuals_Client()
	{
		if (GetGame().IsDedicatedServer())
			return;
		SFP_CacheOriginalTextures();
		SFP_ApplyPackVisuals();
	}

	override void EEInit()
	{
		super.EEInit();

		if (GetGame().IsServer())
			SFP_UpdateVisualCount_Server(true);

		if (!GetGame().IsDedicatedServer())
			SFP_RequestPackVisualRefresh();
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;

		if (GetGame().IsServer())
			SFP_UpdateVisualCount_Server(true);
		else
			SFP_RequestPackVisualRefresh();

		return true;
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();

		if (GetGame().IsDedicatedServer())
			return;

		SFP_RequestPackVisualRefresh();
	}

	override void OnQuantityChanged(float delta)
	{
		super.OnQuantityChanged(delta);

		if (GetGame().IsServer())
			SFP_UpdateVisualCount_Server();

		if (!GetGame().IsDedicatedServer())
			SFP_RequestPackVisualRefresh();
	}

	// -----------------------
	// Actions
	// -----------------------
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionSFP_TakeCigaretteFromPack);
	}

	

	// -----------------------
	// Drag&drop "stack" support:
	// - Drag FULL cigarette (100) onto a pack -> pack +1, cigarette removed
	// Works through vanilla COMBINE_QUANTITY2 flow (CombineItemsClient -> CombineItems on server)
	// -----------------------
	override bool CanBeCombined(EntityAI other_item, bool reservation_check = true, bool stack_max_limit = false)
	{
		// Allow combining a FULL single cigarette into a non-full pack
		if (other_item && other_item.IsKindOf("SFP_SigaPack_Single"))
		{
			ItemBase cig = ItemBase.Cast(other_item);
			if (!cig)
				return false;

			// Only whole cigarettes
			if (Math.Round(cig.GetQuantity()) < Math.Round(cig.GetQuantityMax()))
				return false;

			// Only if pack has space
			if (GetQuantity() >= GetQuantityMax())
				return false;

			return true;
		}

		return super.CanBeCombined(other_item, reservation_check, stack_max_limit);
	}

	override void CombineItems(ItemBase other_item, bool use_stack_max = true)
	{
		// Special case: put FULL single cigarette into pack via drag&drop
		if (other_item && other_item.IsKindOf("SFP_SigaPack_Single"))
		{
			ItemBase cig = other_item;

			// Safety checks (must match CanBeCombined)
			if (Math.Round(cig.GetQuantity()) < Math.Round(cig.GetQuantityMax()))
				return;

			if (GetQuantity() >= GetQuantityMax())
				return;

			// Server authoritative
			if (GetGame().IsServer() || !GetGame().IsMultiplayer())
			{
				AddQuantity(1);
				SFP_UpdateVisualCount_Server(true);

				// Remove the cigarette completely
				cig.DeleteSafe();
			}

			return;
		}

		super.CombineItems(other_item, use_stack_max);
	}
// -----------------------
	// Server: compute & sync visual count
	// -----------------------
	protected int SFP_GetCountFromQuantity(float q)
	{
		int iq = Math.Round(q);
		if (iq < 0)
			return 0;
		if (iq > 20)
			return 20;
		return iq;
	}

	void SFP_UpdateVisualCount_Server(bool force = false)
	{
		if (!GetGame().IsServer())
			return;

		int newCount = SFP_GetCountFromQuantity(GetQuantity());
		if (force || newCount != m_SFP_PackVisualCount)
		{
			m_SFP_PackVisualCount = newCount;
			SetSynchDirty();
		}
	}

	// -----------------------
	// Visuals
	// -----------------------
	protected void SFP_BuildSelectionList()
	{
		m_SFP_SelNames.Clear();

		// primary spelling used in your models
		for (int i = 1; i <= 20; i++)
		{
			m_SFP_SelNames.Insert(string.Format("ciggarette_%1", i));
		}

		// common fallback spelling
		for (int j = 1; j <= 20; j++)
		{
			m_SFP_SelNames.Insert(string.Format("cigarette_%1", j));
		}
	}

	protected void SFP_CacheOriginalTextures()
	{
		if (m_SFP_TexCached)
			return;

		foreach (string selName : m_SFP_SelNames)
		{
			if (!m_SFP_OrigTexBySel.Contains(selName))
				m_SFP_OrigTexBySel.Set(selName, SFP_GetOriginalTexture(selName));
		}

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
			// hiddenSelection -> hide/show via texture override ONLY
			if (active)
			{
				string tex = string.Empty;
				if (m_SFP_OrigTexBySel && m_SFP_OrigTexBySel.Contains(sel_name))
					tex = m_SFP_OrigTexBySel.Get(sel_name);

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

	protected void SFP_ApplyPackVisuals()
	{
		SFP_CacheOriginalTextures();

		// Hide everything first
		foreach (string s : m_SFP_SelNames)
		{
			SFP_SetSelectionState(s, false);
		}

		int count = m_SFP_PackVisualCount;
		if (count <= 0)
			return;

		// Show ALL cigarettes up to count (1..count visible, count+1..20 hidden)
		for (int i = 1; i <= count; i++)
		{
			string selPrimary = string.Format("ciggarette_%1", i);
			string selAlt     = string.Format("cigarette_%1", i);
			SFP_SetSelectionState(selPrimary, true);
			SFP_SetSelectionState(selAlt, true);
		}
	}
};
class SFP_SigaPack_SouzRED: SFP_SigaPack_Base {};
class SFP_SigaPack_SouzBlue: SFP_SigaPack_Base {};
class SFP_SigaPack_Camel: SFP_SigaPack_Base {};
class SFP_SigaPack_Magna: SFP_SigaPack_Base {};
class SFP_SigaPack_Prima: SFP_SigaPack_Base {};
class SFP_SigaPack_WinstonRed: SFP_SigaPack_Base {};
class SFP_SigaPack_GoldYava: SFP_SigaPack_Base {};