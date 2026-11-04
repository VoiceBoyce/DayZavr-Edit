modded class MissionGameplay {

    // MARK: - Private Properties

    private const int RF_ESCAPE_FROM_DAYZ_CL_initDelay = 3;

    void ~MissionGameplay() {
        Print("~MissionGameplay");
        RF_ESCAPE_FROM_DAYZ_Utils.ResetRandomPlayerPreview();
    }

    // MARK: - Override Methods

    override void OnInit() {
        super.OnInit();
        RF_ESCAPE_FROM_DAYZ_CL_Global.InitializeUnits();

        // TODO: Сделать проверки, что если игрок заходит и буквально сразу прыгает в пекло (крашнуло или еще чего)
        // Чтобы не возникло проблем с отображением. (возможно показать голого персонажа instead)
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(RF_ESCAPE_FROM_DAYZ_CL_InitDelayed, RF_ESCAPE_FROM_DAYZ_CL_initDelay * 1000, false);
        Print("wtf fetchServerConfiguration OnInit");
    }

    override void OnPlayerRespawned(Man player) {
        super.OnPlayerRespawned(player);
        if (!RF_ESCAPE_FROM_DAYZ_CL_Global.rpc) return;
            // RF_ESCAPE_FROM_DAYZ_SE_Global.rpc.Send("fetchServerConfiguration");
        RF_ESCAPE_FROM_DAYZ_CL_Global.rpc.Send("fetchPlayerStatistics");
    }

    // MARK: - Private Methods

    void RF_ESCAPE_FROM_DAYZ_CL_InitDelayed() {
        RF_ESCAPE_FROM_DAYZ_CL_Global.rpc.Send("fetchServerConfiguration");
        RF_ESCAPE_FROM_DAYZ_CL_Global.rpc.Send("fetchPlayerStatistics");
    }
}
