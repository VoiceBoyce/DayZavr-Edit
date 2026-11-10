class BanditAI_Base extends InfectedBanditBase
{
	const int SIMPLE_SELECTION_MELEE_RIFLE = 0;
	const int SIMPLE_SELECTION_MELEE_MELEE = 1;
	const int SIMPLE_SELECTION_SHOULDER_RIFLE = 2;
	const int SIMPLE_SELECTION_SHOULDER_MELEE = 3;

	protected static const ref array<string> BANDIT_BASE_SLOTS =
		{
			"BanditHead",
			"BanditBody",
			"BanditLegs",
			"BanditHands",
			"BanditFeet"};

	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);

		ItemBase itemIB = ItemBase.Cast(item);
		SwitchItemSelectionTexture(item, slot_name);
		Param1<BanditAI_Base> p = new Param1<BanditAI_Base>(this);
		item.SwitchItemSelectionTextureEx(EItemManipulationContext.ATTACHING, p);
		UpdateShoulderProxyVisibility(item, slot_name);

		CoverNakedSlots(slot_name);

		// HideHairSelections(itemIB,true);

		AdjustBandana(item, slot_name);
	}

	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);

		ItemBase item_base = ItemBase.Cast(item);
		SwitchItemSelectionTexture(item, slot_name);
		item.SwitchItemSelectionTextureEx(EItemManipulationContext.DETACHING);

		CoverNakedSlots(slot_name, false);

		// HideHairSelections(item_base,false);
	}

	protected void CoverNakedSlots(string slotName, bool cover = true)
	{
		string slotNameInQuestion;
		switch (slotName)
		{
		case "Feet":
			slotNameInQuestion = "BanditFeet";
			break;

		case "Gloves":
			slotNameInQuestion = "BanditHands";
			break;

		case "Headgear":
			// To do, hide head selections.
			break;

		case "Mask":
			// To do, hide beard.
			break;

		case "Body":
			slotNameInQuestion = "BanditBody";
			break;

		case "Legs":
			slotNameInQuestion = "BanditLegs";
			break;

		case "BanditBody":
			if (FindAttachmentBySlotName("Body"))
				slotNameInQuestion = "BanditBody";
			break;

		case "BanditLegs":
			if (FindAttachmentBySlotName("Legs"))
				slotNameInQuestion = "BanditLegs";
			break;

		case "BanditHands":
			if (FindAttachmentBySlotName("Gloves"))
				slotNameInQuestion = "BanditHands";
			break;

		case "BanditFeet":
			if (FindAttachmentBySlotName("Feet"))
				slotNameInQuestion = "BanditFeet";
			break;

		default:
			break;
		}

		if (slotNameInQuestion == string.Empty)
			return;

		EntityAI item;
		if (Class.CastTo(item, FindAttachmentBySlotName(slotNameInQuestion)))
			item.SetInvisible(cover);
	}

	void UpdateShoulderProxyVisibility(EntityAI item, string slot_name)
	{
		// string slot = slot_name;
		// bool boo;
		// boo = item.IsWeapon();

		// if (slot == "Melee")
		// {
		// SetSimpleHiddenSelectionState(SIMPLE_SELECTION_MELEE_RIFLE,boo);
		// SetSimpleHiddenSelectionState(SIMPLE_SELECTION_MELEE_MELEE,!boo);
		// }
		// else if (slot == "Shoulder")
		// {
		// SetSimpleHiddenSelectionState(SIMPLE_SELECTION_SHOULDER_RIFLE,boo);
		// SetSimpleHiddenSelectionState(SIMPLE_SELECTION_SHOULDER_MELEE,!boo);
		// }
	}

	override void SwitchItemSelectionTexture(EntityAI item, string slot_name)
	{
		super.SwitchItemSelectionTexture(item, slot_name);

		// shirt + armband
		EntityAI armband = FindAttachmentBySlotName("Armband");
		if (slot_name == "Body" && armband)
		{
			Param1<BanditAI_Base> p = new Param1<BanditAI_Base>(this);
			armband.SwitchItemSelectionTextureEx(EItemManipulationContext.UPDATE, p);
		}
	}

	void AdjustBandana(EntityAI item, string slot_name)
	{
		if (Bandana_ColorBase.Cast(item))
		{
			if (slot_name == "Headgear")
			{
				item.SetSimpleHiddenSelectionState(0, 1);
				item.SetSimpleHiddenSelectionState(1, 0);
			}
			else if (slot_name == "Mask")
			{
				item.SetSimpleHiddenSelectionState(0, 0);
				item.SetSimpleHiddenSelectionState(1, 1);
			}
		}
	}

	override void EEInit()
	{
		super.EEInit();

		if (GetGame().IsServer())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Call(CreateBody);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Call(CreateHead);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Call(WearClothing);
		}
	}

	void CreateBody()
	{
		if (!FindAttachmentBySlotName("BanditBody"))
			GetInventory().CreateAttachment("BanditBody_Base");

		if (!FindAttachmentBySlotName("BanditHands"))
			GetInventory().CreateAttachment("BanditHands_Base");

		if (!FindAttachmentBySlotName("BanditLegs"))
			GetInventory().CreateAttachment("BanditLegs_Base");

		if (!FindAttachmentBySlotName("BanditFeet"))
			GetInventory().CreateAttachment("BanditFeet_Base");
	}

	void CreateHead()
	{
		if (!FindAttachmentBySlotName("BanditHead"))
			GetInventory().CreateAttachment(GetGame().ConfigGetTextOut("CfgVehicles " + ClassName() + " Head"));
	}

	void WearClothing()
	{
		int j;
		for (int i = 0; i < m_Loadout.Count(); i++)
		{
			EntityAI cloth = GetInventory().CreateAttachment(m_Loadout.Get(i));
			// cloth.GetInventory().CreateInInventory(
		}
		EntityAI batoh = FindAttachmentBySlotName("Back");
		EntityAI pants = FindAttachmentBySlotName("Legs");
		ItemBase item;
		if (batoh && m_Loot)
		{

			for (j = 0; j < m_Loot.Count(); j++)
			{
				// Print(m_Loot.Get(j));
				item = ItemBase.Cast(batoh.GetInventory().CreateInInventory(m_Loot.Get(j)));
			}
		}
		else if (pants && m_Loot)
		{
			for (j = 0; j < m_Loot.Count(); j++)
			{
				// Print(m_Loot.Get(j));
				item = ItemBase.Cast(pants.GetInventory().CreateInInventory(m_Loot.Get(j)));
			}
		}
		/*
		GetInventory().CreateAttachment("CombatBoots_Black");
		GetInventory().CreateAttachment("HunterPants_Brown");
		GetInventory().CreateAttachment("HuntingJacket_Brown");
		GetInventory().CreateAttachment("HuntingBag");
		*/
	}

	void InitialSetup(TStringArray clothing)
	{
		m_Loadout = clothing;
	}

	void SetupLoot(TStringArray loot)
	{
		m_Loot = loot;
	}
}

class BanditAI_MaleBase : BanditAI_Base {}
class BanditAI_FemaleBase : BanditAI_Base {}

class BanditAI_Adam : BanditAI_MaleBase {}
class BanditAI_Boris : BanditAI_MaleBase {}
class BanditAI_Cyril : BanditAI_MaleBase {}
class BanditAI_Denis : BanditAI_MaleBase {}
class BanditAI_Elias : BanditAI_MaleBase {}
class BanditAI_Francis : BanditAI_MaleBase {}
class BanditAI_Guo : BanditAI_MaleBase {}
class BanditAI_Hassan : BanditAI_MaleBase {}
class BanditAI_Indar : BanditAI_MaleBase {}
class BanditAI_Jose : BanditAI_MaleBase {}
class BanditAI_Kaito : BanditAI_MaleBase {}
class BanditAI_Lewis : BanditAI_MaleBase {}
class BanditAI_Manua : BanditAI_MaleBase {}
class BanditAI_Niki : BanditAI_MaleBase {}
class BanditAI_Oliver : BanditAI_MaleBase {}
class BanditAI_Peter : BanditAI_MaleBase {}
class BanditAI_Quinn : BanditAI_MaleBase {}
class BanditAI_Rolf : BanditAI_MaleBase {}
class BanditAI_Seth : BanditAI_MaleBase {}
class BanditAI_Taiki : BanditAI_MaleBase {}

class BanditAI_Eva : BanditAI_FemaleBase {}
class BanditAI_Frida : BanditAI_FemaleBase {}
class BanditAI_Gabi : BanditAI_FemaleBase {}
class BanditAI_Helga : BanditAI_FemaleBase {}
class BanditAI_Irena : BanditAI_FemaleBase {}
class BanditAI_Judy : BanditAI_FemaleBase {}
class BanditAI_Keiko : BanditAI_FemaleBase {}
class BanditAI_Linda : BanditAI_FemaleBase {}
class BanditAI_Maria : BanditAI_FemaleBase {}
class BanditAI_Naomi : BanditAI_FemaleBase {}
class BanditAI_Baty : BanditAI_FemaleBase {}

class BanditHead_Base : Inventory_Base
{
	override void SwitchItemSelectionTextureEx(EItemManipulationContext context, Param par = null)
	{
		super.SwitchItemSelectionTextureEx(context, par);

		Param1<BanditAI_Base> data = Param1<BanditAI_Base>.Cast(par);

		if (!data)
			return;

		BanditAI_Base bandit = data.param1;
		string tone_mat, tone_texture;

		int personalityIndex = GetHiddenSelectionIndex("personality");
		if (personalityIndex >= 0)
		{
			int bodyPersonalityIndex = bandit.GetHiddenSelectionIndex("personality");
			tone_mat = bandit.GetHiddenSelectionsMaterials().Get(bodyPersonalityIndex);
			tone_texture = bandit.GetHiddenSelectionsTextures().Get(bodyPersonalityIndex);

			SetObjectMaterial(personalityIndex, tone_mat);
			SetObjectTexture(personalityIndex, tone_texture);
		}

		int hairIndex = GetHiddenSelectionIndex("hair");
		if (hairIndex >= 0)
		{
			int bodyHairIndex = bandit.GetHiddenSelectionIndex("hair");
			tone_mat = bandit.GetHiddenSelectionsMaterials().Get(bodyHairIndex);
			tone_texture = bandit.GetHiddenSelectionsTextures().Get(bodyHairIndex);

			SetObjectMaterial(hairIndex, tone_mat);
			SetObjectTexture(hairIndex, tone_texture);
		}

		int beardIndex = GetHiddenSelectionIndex("beard");
		if (beardIndex >= 0)
		{
			int bodyBeardIndex = bandit.GetHiddenSelectionIndex("beard");
			tone_mat = bandit.GetHiddenSelectionsMaterials().Get(bodyBeardIndex);
			tone_texture = bandit.GetHiddenSelectionsTextures().Get(bodyBeardIndex);

			SetObjectMaterial(beardIndex, tone_mat);
			SetObjectTexture(beardIndex, tone_texture);
		}
	}
}