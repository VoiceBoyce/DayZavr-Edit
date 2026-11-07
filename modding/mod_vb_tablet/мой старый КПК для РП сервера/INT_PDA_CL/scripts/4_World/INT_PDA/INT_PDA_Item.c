// INT_PDA — the PDA item (spec.md §4). Battery: vanilla energy manager (config), on/off: vanilla actions in hands,
// «Заглянуть в КПК»: INT_PDA_ActionOpen (client-local). The screen selection (back_face) shows the ON texture while
// the device works. The PDA remembers its owner (first account that logged in with it): a looted PDA shows
// «КПК сталкера <позывной>» to anybody else (owner 2026-10-22). Only a hash of the owner is synced; the callsign
// comes from the server on request.

class INT_PDA extends ItemBase
{
	static const int STORE_VERSION = 1;
	static const int SEL_SCREEN = 1;			// index of "back_face" in hiddenSelections
	static const string SLOT_GPS = "INT_PDA_Module_GPS";
	static const string SLOT_MAP = "INT_PDA_Module_MAP";
	static const string SLOT_CHAT = "INT_PDA_Module_Chat";
	static const string SLOT_GP = "INT_PDA_Module_GP";
	static const string TEX_ON = "INT_PDA_Data\\data\\pda\\back_face\\pristine\\pda_pristine_on_co.paa";
	static const string MAT_ON = "INT_PDA_Data\\data\\pda\\back_face\\pristine\\pda_pristine_on.rvmat";
	static const string TEX_OFF = "INT_PDA_Data\\data\\pda\\back_face\\pristine\\pda_pristine_co.paa";
	static const string MAT_OFF = "INT_PDA_Data\\data\\pda\\back_face\\pristine\\pda_pristine.rvmat";

	protected string m_OwnerSteam;				// server only (persistent)
	protected int m_OwnerHash;					// synced: 0 = nobody logged in with this PDA yet

	void INT_PDA()
	{
		RegisterNetSyncVariableInt("m_OwnerHash");
	}

	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionTurnOnWhileInHands);
		AddAction(INT_PDA_ActionOpen);
		AddAction(ActionTurnOffWhileInHands);
	}

	bool IsPdaOn()
	{
		return GetCompEM() && GetCompEM().IsWorking();
	}

	bool IsWorkingPda()
	{
		return !IsRuined() && GetCompEM() && GetCompEM().IsWorking();
	}

	bool HasModule(string slot)
	{
		return FindAttachmentBySlotName(slot) != null;
	}

	bool HasGps()
	{
		return HasModule(SLOT_GPS);
	}

	bool HasMap()
	{
		return HasModule(SLOT_MAP);
	}

	bool HasChat()
	{
		return HasModule(SLOT_CHAT);
	}

	bool HasGp()
	{
		return HasModule(SLOT_GP);
	}

	// Battery charge 0..1. The battery's energy itself is not synced to clients; the 9V battery mirrors it into its
	// item quantity (convertEnergyToQuantity), which is synced — the vanilla inventory bar reads the same value.
	float BatteryLevel()
	{
		ItemBase bat;
		if (GetCompEM())
			bat = ItemBase.Cast(GetCompEM().GetEnergySource());
		if (!bat)
			bat = ItemBase.Cast(FindAttachmentBySlotName("BatteryD"));
		if (!bat)
			return 0;
		if (GetGame().IsDedicatedServer() && bat.GetCompEM())
			return bat.GetCompEM().GetEnergy0To1();
		float mx = bat.GetQuantityMax();
		if (mx <= 0)
			return 0;
		return Math.Clamp(bat.GetQuantity() / mx, 0, 1);
	}

	// --- owner ------------------------------------------------------------------------------------------------------
	int OwnerHash()
	{
		return m_OwnerHash;
	}

	string OwnerSteam()
	{
		return m_OwnerSteam;
	}

	bool IsOwnedBy(string steam)
	{
		return m_OwnerHash == 0 || m_OwnerHash == steam.Hash();
	}

	// Server: the first account that logs in with this PDA owns it.
	void SetOwnerServer(string steam)
	{
		if (!GetGame() || !GetGame().IsDedicatedServer())
			return;
		m_OwnerSteam = steam;
		m_OwnerHash = 0;
		if (steam != "")
			m_OwnerHash = steam.Hash();
		SetSynchDirty();
	}

	// --- screen -----------------------------------------------------------------------------------------------------
	override void OnWorkStart()
	{
		super.OnWorkStart();
		UpdateScreen();
		if (!GetGame().IsDedicatedServer())
			INT_PDA_Events.PdaChanged(this);
	}

	override void OnWorkStop()
	{
		super.OnWorkStop();
		UpdateScreen();
		if (GetGame().IsDedicatedServer())
			INT_PDA_Events.ServerWorkStop(this);
		else
			INT_PDA_Events.PdaChanged(this);
	}

	override void EEHealthLevelChanged(int oldLevel, int newLevel, string zone)
	{
		super.EEHealthLevelChanged(oldLevel, newLevel, zone);
		UpdateScreen();
	}

	override void EEInit()
	{
		super.EEInit();
		UpdateScreen();
	}

	protected void UpdateScreen()
	{
		if (IsRuined())
			return;
		if (IsPdaOn())
		{
			SetObjectTexture(SEL_SCREEN, TEX_ON);
			SetObjectMaterial(SEL_SCREEN, MAT_ON);
		}
		else
		{
			SetObjectTexture(SEL_SCREEN, TEX_OFF);
			SetObjectMaterial(SEL_SCREEN, MAT_OFF);
		}
	}

	// --- world beep -------------------------------------------------------------------------------------------------
	// Server: an incoming private message beeps the receiver's PDA. The RPC goes to the clients that have this item in
	// their network bubble; each of them plays the 3D beep (15 m). The owner hears the PDA's own 2D sound instead.
	void BeepServer()
	{
		if (!GetGame().IsDedicatedServer())
			return;
		ScriptRPC rpc = new ScriptRPC();
		rpc.Send(this, INT_PDA_Const.RPC_BEEP, true, null);
	}

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		if (rpc_type != INT_PDA_Const.RPC_BEEP || GetGame().IsDedicatedServer())
			return;
		if (GetHierarchyRootPlayer() && GetHierarchyRootPlayer() == GetGame().GetPlayer())
			return;
		EffectSound snd = SEffectManager.PlaySoundOnObject("INT_PDA_BeepWorld_SoundSet", this);
		if (snd)
			snd.SetAutodestroy(true);
	}

	// --- client notifications ---------------------------------------------------------------------------------------
	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);
		if (!GetGame().IsDedicatedServer())
			INT_PDA_Events.PdaChanged(this);
	}

	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);
		if (!GetGame().IsDedicatedServer())
			INT_PDA_Events.PdaChanged(this);
	}

	override void EEItemLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
	{
		super.EEItemLocationChanged(oldLoc, newLoc);
		if (!GetGame().IsDedicatedServer())
			INT_PDA_Events.PdaChanged(this);
	}

	// --- persistence ------------------------------------------------------------------------------------------------
	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		ctx.Write(STORE_VERSION);
		ctx.Write(m_OwnerSteam);
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;
		int ver;
		string owner;
		if (!ctx.Read(ver) || ver < 1 || !ctx.Read(owner))
			return false;
		m_OwnerSteam = owner;
		m_OwnerHash = 0;
		if (owner != "")
			m_OwnerHash = owner.Hash();
		return true;
	}

	override void AfterStoreLoad()
	{
		super.AfterStoreLoad();
		SetSynchDirty();
	}
}
