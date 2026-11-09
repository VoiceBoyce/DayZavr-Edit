modded class Action_MSFZ_MineRock
{
    override void OnFinishProgressServer(ActionData action_data)
    {
        super.OnFinishProgressServer(action_data);

        PlayerBase player = action_data.m_Player;
        MSFZ_RuneGathering_Base rune = MSFZ_RuneGathering_Base.Cast(action_data.m_Target.GetObject());
        if (!rune || !player)
            return;

        // двое добывали одновременно — руна достаётся тому, кто закончил первым
        if (rune.MSFZ_RuneGatheredCheck(player))
        {
            SendMessageToChat(player, m_message_fail);
            return;
        }

        string dropClassName = rune.GetDropItemClassName(rune.GetType());
        if (dropClassName == "")
            return;

        rune.MSFZ_RuneGatheringTimeout();

        // руна сразу в инвентарь, если нет места — на землю под ноги
        EntityAI droppedRune = player.GetInventory().CreateInInventory(dropClassName);
        if (!droppedRune)
            droppedRune = EntityAI.Cast(GetGame().CreateObjectEx(dropClassName, player.GetPosition(), ECE_PLACE_ON_SURFACE));

        if (droppedRune)
            SendMessageToChat(player, m_message_gathered_success + " " + droppedRune.GetDisplayName());
		SendMessageToChat(player, m_message_gototimeout);
    }
};
