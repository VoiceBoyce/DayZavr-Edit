class SM_Mercenary_Jacket_ColorBase extends Top_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}

	void HideSelectionModel(string selectionName, bool hide = false)
    {
        TStringArray selectionNames = new TStringArray;
        ConfigGetTextArray("simpleHiddenSelections",selectionNames);
        int selectionId = selectionNames.Find(selectionName);
        SetSimpleHiddenSelectionState(selectionId, hide);
    }
	
	override void EEItemAttached(EntityAI item, string slot_name)
	{	
		super.EEItemAttached(item,slot_name);

		if ( slot_name == "Gloves" )
        {
            HideSelectionModel("hide_gloves", 0);
        }
	}
	
	override void EEItemDetached(EntityAI item, string slot_name)
	{	
		super.EEItemDetached(item,slot_name);

		if ( slot_name == "Gloves" )
        {
          HideSelectionModel("hide_gloves", 1);
        }
	}
};

class SM_Mercenary_Jacket_Black extends SM_Mercenary_Jacket_ColorBase {};
class SM_Mercenary_Jacket_White extends SM_Mercenary_Jacket_ColorBase {};
class SM_Mercenary_Jacket_Yellow extends SM_Mercenary_Jacket_ColorBase {};

class SM_Mercenary_Jacket_NO_EXO_ColorBase extends Top_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}

	void HideSelectionModel(string selectionName, bool hide = false)
    {
        TStringArray selectionNames = new TStringArray;
        ConfigGetTextArray("simpleHiddenSelections",selectionNames);
        int selectionId = selectionNames.Find(selectionName);
        SetSimpleHiddenSelectionState(selectionId, hide);
    }
	
	override void EEItemAttached(EntityAI item, string slot_name)
	{	
		super.EEItemAttached(item,slot_name);

		if ( slot_name == "Gloves" )
        {
            HideSelectionModel("hide_gloves", 0);
        }
	}
	
	override void EEItemDetached(EntityAI item, string slot_name)
	{	
		super.EEItemDetached(item,slot_name);

		if ( slot_name == "Gloves" )
        {
          HideSelectionModel("hide_gloves", 1);
        }
	}
};

class SM_Mercenary_Jacket_NO_EXO_Black extends SM_Mercenary_Jacket_ColorBase {};
class SM_Mercenary_Jacket_NO_EXO_White extends SM_Mercenary_Jacket_ColorBase {};
class SM_Mercenary_Jacket_NO_EXO_Yellow extends SM_Mercenary_Jacket_ColorBase {};