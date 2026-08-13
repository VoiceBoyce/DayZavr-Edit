class Action_MSFZ_MineRockCB : ActionContinuousBaseCB
{
    private const float TIME_BETWEEN_MATERIAL_DROPS = 8;

    override void CreateActionComponent()
    {
        m_ActionData.m_ActionComponent = new CAContinuousTime(TIME_BETWEEN_MATERIAL_DROPS);
    }
};

class Action_MSFZ_MineRock: ActionContinuousBase
{
	string m_message_fail = "#STR_MSFZ_minefiasko";
	string m_message_timeout = "#STR_MSFZ_minefiaskotime";
	string m_message_gototimeout = "#STR_MSFZ_minefiaskotime";
	string m_message_gathered_success = "#STR_MSFZ_minegood";
    void Action_MSFZ_MineRock()
    {
        m_CallbackClass = Action_MSFZ_MineRockCB;
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_MINEROCK;
        m_FullBody = true;
        m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;
        m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
        m_Text = "#STR_MSFZ_mine";
    }

    override void CreateConditionComponents()  
    {        
        m_ConditionTarget = new CCTCursor(UAMaxDistances.DEFAULT);
        m_ConditionItem = new CCINonRuined;
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {    
        //Action not allowed if player has broken legs
        if (player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
            return false;
        
        Object targetObject = target.GetObject();
        if (targetObject && targetObject.IsInherited(MSFZ_RuneGathering_Base))
        {
            MSFZ_RuneGathering_Base rune = MSFZ_RuneGathering_Base.Cast(targetObject);
            if(!rune.MSFZ_RuneGatheredCheck(player))
            {
                return true;
            }
			else
			{
				SendMessageToChat(player, m_message_fail);
			}
        }
        
        return false;
    }

    override string GetSoundCategory(ActionData action_data)
    {
        ItemBase item = action_data.m_MainItem;
        if (item.IsKindOf("MSFZ_Magic_Kirka"))
        {
            return "MSFZ_Magic_Kirka_Mine_Sound";
        }
        return "";
    }
	
	void SendMessageToChat(PlayerBase player, string message)
	{
		Param1<string> msgParam = new Param1<string>(message);
		GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, msgParam, true, player.GetIdentity());
	}
};

modded class ActionConstructor
{
    override void RegisterActions(TTypenameArray actions)
    {
        super.RegisterActions(actions);
        actions.Insert(Action_MSFZ_MineRock);
		actions.Insert(ActionTeleportToObelisk);	
    }
};