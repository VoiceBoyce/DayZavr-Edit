class DRL_Craft_WB : ItemBase
{
    override void SetActions()
    {
        super.SetActions();
		
        AddAction(ActionOpenDRLWorkbench);
        AddAction(ActionDeconstructDRLWorkbench);
    }

    override bool CanPutInCargo (EntityAI parent)
	{
		return false;
	}

    override bool CanPutIntoHands (EntityAI parent)
	{	
		return false;
	}
	
	bool NeedsScrewdriverForDeconstruct()
    {
        return true;
    }
};

class DRL_Craft_Workbench extends DRL_Craft_WB
{
	override bool NeedsScrewdriverForDeconstruct()
    {
        return false;
    }
};

class DRL_Craft_Garden extends DRL_Craft_WB
{
	override bool NeedsScrewdriverForDeconstruct()
    {
        return false;
    }
};

class DRL_Craft_Cook extends DRL_Craft_WB
{
	override bool NeedsScrewdriverForDeconstruct()
    {
        return false;
    }
};

class DRL_Craft_Build extends DRL_Craft_WB
{
	override bool NeedsScrewdriverForDeconstruct()
    {
        return false;
    }
};

class DRL_Craft_Welding_Machine extends DRL_Craft_WB
{
	override bool CanPutIntoHands (EntityAI parent)
	{	
		return true;
	}
};

class DRL_Craft_Circular extends DRL_Craft_WB
{
	override bool CanPutIntoHands (EntityAI parent)
	{	
		return true;
	}
};

class DRL_Craft_NoteBook extends DRL_Craft_WB
{
	override bool CanPutIntoHands (EntityAI parent)
	{	
		return true;
	}
};

class DRL_Craft_Microscope extends DRL_Craft_WB
{
	override bool CanPutIntoHands (EntityAI parent)
	{	
		return true;
	}
};

class DRL_Craft_Microwave extends DRL_Craft_WB
{
	override bool CanPutIntoHands (EntityAI parent)
	{	
		return true;
	}
};

class DRL_Craft_Sewing extends DRL_Craft_WB
{
	override bool CanPutIntoHands (EntityAI parent)
	{	
		return true;
	}
};

class DRL_Craft_WB1: DRL_Craft_WB{};
class DRL_Craft_WB2: DRL_Craft_WB{};
class DRL_Craft_WB3: DRL_Craft_WB{};
class DRL_Craft_WB4: DRL_Craft_WB{};
class DRL_Craft_WB5: DRL_Craft_WB{};
class DRL_Craft_WB6: DRL_Craft_WB{};
class DRL_Craft_WB7: DRL_Craft_WB{};
class DRL_Craft_WB8: DRL_Craft_WB{};
class DRL_Craft_WB9: DRL_Craft_WB{};


modded class OH_workbench
{
    override void SetActions()
    {
        super.SetActions();
		
        AddAction(ActionOpenDRLWorkbench);
    }
};

modded class SRS_Toolsbench
{
    override void SetActions()
    {
        super.SetActions();
		
        AddAction(ActionOpenDRLWorkbench);
    }
};

modded class SRS_Toolsbench_Red
{
    override void SetActions()
    {
        super.SetActions();
		
        AddAction(ActionOpenDRLWorkbench);
    }
};

modded class SRS_Toolsbench_Blue
{
    override void SetActions()
    {
        super.SetActions();
		
        AddAction(ActionOpenDRLWorkbench);
    }
};