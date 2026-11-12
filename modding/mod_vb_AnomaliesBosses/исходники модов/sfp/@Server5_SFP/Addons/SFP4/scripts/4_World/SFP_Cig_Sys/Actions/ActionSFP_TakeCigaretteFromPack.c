// SFP_Cig_Sys
// Action: open cigarette pack and take 1 cigarette.
// Result (server-authoritative):
// - Pack in hands is replaced by a single cigarette in hands
// - New pack (with -1 quantity) is spawned into inventory (or dropped if inventory full)

class ActionSFP_TakeCigaretteFromPackCB : ActionContinuousBaseCB
{
	// Faster open/unpack
	static const float TIME_OPEN_PACK = 1.3;

	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(TIME_OPEN_PACK);
	}
};

// Replace pack in hands with a single cigarette, and spawn a reduced pack into inventory/ground.
class SFP_PackToSingleLambda : ReplaceItemWithNewLambdaBase
{
	PlayerBase m_Player;
	string m_PackType;
	int m_PackNewQty;
	float m_PackHealth;
	float m_PackWet;

	// NOTE: Do NOT call ReplaceItemWithNewLambdaBase constructor explicitly.
	// DayZ inventory code initializes the lambda when it is executed.
	void SFP_PackToSingleLambda(EntityAI old_item, string new_item_type, PlayerBase player, string packType, int packNewQty, float packHealth, float packWet)
	{
		m_Player = player;
		m_PackType = packType;
		m_PackNewQty = packNewQty;
		m_PackHealth = packHealth;
		m_PackWet = packWet;
	}

	override void OnSuccess(EntityAI new_item)
	{
		super.OnSuccess(new_item);

		if (!m_Player)
			return;

		// Ensure cigarette starts full and has correct visual stage immediately
		SFP_SigaPack_Single cig;
		if (Class.CastTo(cig, new_item) && cig.HasQuantity())
		{
			cig.SetQuantity(100, false, false);
			cig.SFP_UpdateVisualStage_Server(true);
			cig.SetSmokingState(false);
		}
		else
		{
			ItemBase cigBase = ItemBase.Cast(new_item);
			if (cigBase && cigBase.HasQuantity())
				cigBase.SetQuantity(100, false, false);
		}

		// If pack became empty after taking one, do not spawn a new pack
		if (m_PackNewQty <= 0)
			return;

		EntityAI newPackEnt;
		InventoryLocation il = new InventoryLocation;
		if (m_Player.GetInventory().FindFirstFreeLocationForNewEntity(m_PackType, FindInventoryLocationType.CARGO | FindInventoryLocationType.ATTACHMENT, il))
		{
			newPackEnt = GameInventory.LocationCreateEntity(il, m_PackType, ECE_IN_INVENTORY, RF_DEFAULT);
		}

		if (!newPackEnt)
		{
			// Inventory full -> drop near the player
			newPackEnt = EntityAI.Cast(GetGame().CreateObjectEx(m_PackType, m_Player.GetPosition(), ECE_PLACE_ON_SURFACE));
		}

		ItemBase newPack = ItemBase.Cast(newPackEnt);
		if (newPack)
		{
			newPack.SetHealth("", "", m_PackHealth);
			newPack.SetWet(m_PackWet);
			newPack.SetQuantity(m_PackNewQty);
		}
	}
};

class ActionSFP_TakeCigaretteFromPack : ActionContinuousBase
{
	void ActionSFP_TakeCigaretteFromPack()
	{
		m_CallbackClass = ActionSFP_TakeCigaretteFromPackCB;

		// Vanilla open item animation
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENITEM;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_OPENITEM;

		m_FullBody = false;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		m_Text = "#STR_SFPUnpackSigaPack";
	}

	override typename GetInputType()
	{
		// Like vanilla unpack/open actions: hold LMB
		return ContinuousDefaultActionInput;
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNone;
	}

	override bool HasTarget()
	{
		return false;
	}

	override bool HasProneException()
	{
		return true;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		SFP_SigaPack_Base pack;
		if (!Class.CastTo(pack, item))
			return false;

		if (!player)
			return false;

		// Must be in hands
		if (player.GetItemInHands() != pack)
			return false;

		if (!pack.HasQuantity())
			return false;

		if (pack.GetQuantity() <= 0)
			return false;

		// Allow replacing item in hands
		if (!player.GetHumanInventory() || !player.GetHumanInventory().CanRemoveEntityInHands())
			return false;

		return true;
	}

	override bool ActionConditionContinue(ActionData action_data)
	{
		if (!action_data || !action_data.m_Player)
			return false;

		SFP_SigaPack_Base pack;
		if (!Class.CastTo(pack, action_data.m_MainItem))
			return false;

		// While action is running the pack is still in hands (swap happens only on finish)
		if (action_data.m_Player.GetItemInHands() != pack)
			return false;

		return pack.HasQuantity() && pack.GetQuantity() > 0;
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		if (!action_data || !action_data.m_Player)
			return;

		PlayerBase player = action_data.m_Player;
		SFP_SigaPack_Base pack;
		if (!Class.CastTo(pack, action_data.m_MainItem))
			return;

		if (player.GetItemInHands() != pack)
			return;

		int oldQty = pack.GetQuantity();
		if (oldQty <= 0)
			return;

		int newQty = oldQty - 1;
		if (newQty < 0)
			newQty = 0;

		string packType = pack.GetType();
		float packHealth = pack.GetHealth("", "");
		float packWet = pack.GetWet();

		SFP_PackToSingleLambda lambda = new SFP_PackToSingleLambda(pack, "SFP_SigaPack_Single", player, packType, newQty, packHealth, packWet);
		bool ok = player.ServerReplaceItemInHandsWithNew(lambda);
		if (!ok)
		{
			player.GetActionManager().Interrupt();
		}
	}
};
