modded class Action_MSFZ_MineRock
{
    override void OnFinishProgressServer(ActionData action_data)
    {
        super.OnFinishProgressServer(action_data);

        Object targetObject = action_data.m_Target.GetObject();
        MSFZ_RuneGathering_Base rune = MSFZ_RuneGathering_Base.Cast(targetObject);
        rune.MSFZ_RuneGatheringTimeout();

        string targetClassName = targetObject.GetType();
        string m_dropClassName = rune.GetDropItemClassName(targetClassName);
		string m_message_timeouttime = rune.MSFZ_Timeoutlimit().ToString();

        // Get the player's position
        vector playerPosition = action_data.m_Player.GetPosition();

        // Spawn the item at the player's position
        Object DroppedRune = GetGame().CreateObject(m_dropClassName, playerPosition, false, false, true);
		

        // Send message to player
        SendMessageToChat(action_data.m_Player, m_message_gathered_success + " " + DroppedRune.GetDisplayName());
		SendMessageToChat(action_data.m_Player, m_message_gototimeout);
    }
};