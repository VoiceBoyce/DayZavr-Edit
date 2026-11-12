// ============================
//   PLAY CASSETTE
// ============================
class ActionSFP_PlayCassette : ActionSingleUseBase
{
    void ActionSFP_PlayCassette()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_ITEM_ON;
        m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
        m_Text       = "#STR_Play_cassette";
    }

    override void CreateConditionComponents()
    {
        m_ConditionItem   = new CCINonRuined();
        m_ConditionTarget = new CCTNone;
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        SFP_Radio radio;
        if (!Class.CastTo(radio, item))
            return false;

        if (!radio.SFP_HasCassette())
            return false;

        if (!radio.GetCompEM() || !radio.GetCompEM().IsWorking())
            return false;

        if (radio.SFP_IsCassetteEnabled()) // уже играет
            return false;

        return true;
    }

    override void OnExecuteServer(ActionData action_data)
    {
        SFP_Radio radio;
        if (Class.CastTo(radio, action_data.m_MainItem))
        {
            radio.SFP_SetCassetteEnabled(true);
        }
    }
}

// ============================
//   STOP CASSETTE
// ============================
class ActionSFP_StopCassette : ActionSingleUseBase
{
    void ActionSFP_StopCassette()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_ITEM_OFF;
        m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
        m_Text       = "#STR_Stop_cassette";
    }

    override void CreateConditionComponents()
    {
        m_ConditionItem   = new CCINonRuined();
        m_ConditionTarget = new CCTNone;
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        SFP_Radio radio;
        if (!Class.CastTo(radio, item))
            return false;

        if (!radio.SFP_HasCassette())
            return false;

        if (!radio.GetCompEM() || !radio.GetCompEM().IsWorking())
            return false;

        if (!radio.SFP_IsCassetteEnabled()) // уже остановлено
            return false;

        return true;
    }

    override void OnExecuteServer(ActionData action_data)
    {
        SFP_Radio radio;
        if (Class.CastTo(radio, action_data.m_MainItem))
        {
            radio.SFP_SetCassetteEnabled(false);
        }
    }
}

// ============================
//   VOLUME UP
// ============================
class ActionSFP_CassetteVolumeUp : ActionSingleUseBase
{
    void ActionSFP_CassetteVolumeUp()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_ITEM_ON;
        m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
        m_Text       = "#STR_Volume_up";
    }

    override void CreateConditionComponents()
    {
        m_ConditionItem   = new CCINonRuined();
        m_ConditionTarget = new CCTNone;
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        SFP_Radio radio;
        if (!Class.CastTo(radio, item))
            return false;

        if (!radio.SFP_HasCassette())
            return false;

        if (!radio.GetCompEM() || !radio.GetCompEM().IsWorking())
            return false;

        if (!radio.SFP_CanIncreaseVolume())
            return false;

        return true;
    }

    override void OnExecuteServer(ActionData action_data)
    {
        SFP_Radio radio;
        if (Class.CastTo(radio, action_data.m_MainItem))
        {
            radio.SFP_ChangeCassetteVolume(0.3); // шаг +0.3

            int percent = Math.Round(radio.SFP_GetCassetteVolume() * 100.0);
            PlayerBase player = PlayerBase.Cast(action_data.m_Player);
            if (player)
            {
                player.MessageStatus("Volume- " + percent.ToString() + "%");
            }
        }
    }
}

// ============================
//   VOLUME DOWN
// ============================
class ActionSFP_CassetteVolumeDown : ActionSingleUseBase
{
    void ActionSFP_CassetteVolumeDown()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_ITEM_OFF;
        m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
        m_Text       = "#STR_Volume_down";
    }

    override void CreateConditionComponents()
    {
        m_ConditionItem   = new CCINonRuined();
        m_ConditionTarget = new CCTNone;
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        SFP_Radio radio;
        if (!Class.CastTo(radio, item))
            return false;

        if (!radio.SFP_HasCassette())
            return false;

        if (!radio.GetCompEM() || !radio.GetCompEM().IsWorking())
            return false;

        if (!radio.SFP_CanDecreaseVolume())
            return false;

        return true;
    }

    override void OnExecuteServer(ActionData action_data)
    {
        SFP_Radio radio;
        if (Class.CastTo(radio, action_data.m_MainItem))
        {
            radio.SFP_ChangeCassetteVolume(-0.3); // шаг -0.3

            int percent = Math.Round(radio.SFP_GetCassetteVolume() * 100.0);
            PlayerBase player = PlayerBase.Cast(action_data.m_Player);
            if (player)
            {
                player.MessageStatus("Volume- " + percent.ToString() + "%");
            }
        }
    }
}

// ============================
//   NEXT TRACK
// ============================
class ActionSFP_CassetteNextTrack : ActionSingleUseBase
{
    void ActionSFP_CassetteNextTrack()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_ITEM_ON;
        m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
        m_Text       = "#STR_NextTrack";
    }

    override void CreateConditionComponents()
    {
        m_ConditionItem   = new CCINonRuined();
        m_ConditionTarget = new CCTNone;
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        SFP_Radio radio;
        if (!Class.CastTo(radio, item))
            return false;

        if (!radio.SFP_HasCassette())
            return false;

        // радио должно быть с батарейкой
        if (!radio.GetCompEM() || !radio.GetCompEM().IsWorking())
            return false;

        SFP_Cassette_Base cassette = radio.FindInsertedCassette();
        if (!cassette)
            return false;

        return true;
    }

    override void OnExecuteServer(ActionData action_data)
    {
        SFP_Radio radio;
        if (Class.CastTo(radio, action_data.m_MainItem))
        {
            SFP_Cassette_Base cassette = radio.FindInsertedCassette();
            if (cassette)
            {
				if (cassette.SFP_GetTrackCount() <= 1)
					return;

                bool wasEnabled = radio.SFP_IsCassetteEnabled();

                cassette.SFP_NextTrack();

                if (wasEnabled)
                {
                    radio.SFP_MarkTrackChanged();
                }

                PlayerBase player = PlayerBase.Cast(action_data.m_Player);
                if (player)
                {
                    int idx   = cassette.SFP_GetTrackIndex() + 1; // в UI считаем с 1
                    int count = cassette.SFP_GetTrackCount();
                    string name = cassette.SFP_GetCurrentTrackName();

                    string msg = "Track- " + idx.ToString() + "/" + count.ToString();
                    if (name != string.Empty)
                        msg = msg + ": " + name;

                    player.MessageStatus(msg);
                }
            }
        }
    }
}

// ============================
//   PREVIOUS TRACK
// ============================
class ActionSFP_CassettePrevTrack : ActionSingleUseBase
{
    void ActionSFP_CassettePrevTrack()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_ITEM_OFF;
        m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
        m_Text       = "#STR_Previoustrack";
    }

    override void CreateConditionComponents()
    {
        m_ConditionItem   = new CCINonRuined();
        m_ConditionTarget = new CCTNone;
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        SFP_Radio radio;
        if (!Class.CastTo(radio, item))
            return false;

        if (!radio.SFP_HasCassette())
            return false;

        if (!radio.GetCompEM() || !radio.GetCompEM().IsWorking())
            return false;

        SFP_Cassette_Base cassette = radio.FindInsertedCassette();
        if (!cassette)
            return false;

        return true;
    }

    override void OnExecuteServer(ActionData action_data)
    {
        SFP_Radio radio;
        if (Class.CastTo(radio, action_data.m_MainItem))
        {
            SFP_Cassette_Base cassette = radio.FindInsertedCassette();
            if (cassette)
            {
				if (cassette.SFP_GetTrackCount() <= 1)
					return;

                bool wasEnabled = radio.SFP_IsCassetteEnabled();

                cassette.SFP_PrevTrack();

                if (wasEnabled)
                {
                    radio.SFP_MarkTrackChanged();
                }

                PlayerBase player = PlayerBase.Cast(action_data.m_Player);
                if (player)
                {
                    int idx   = cassette.SFP_GetTrackIndex() + 1;
                    int count = cassette.SFP_GetTrackCount();
                    string name = cassette.SFP_GetCurrentTrackName();

                    string msg = "Track- " + idx.ToString() + "/" + count.ToString();
                    if (name != string.Empty)
                        msg = msg + ": " + name;

                    player.MessageStatus(msg);
                }
            }
        }
    }
}
