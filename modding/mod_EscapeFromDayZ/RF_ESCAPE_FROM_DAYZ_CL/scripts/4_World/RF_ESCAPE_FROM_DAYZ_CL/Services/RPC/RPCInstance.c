class RF_ESCAPE_FROM_DAYZ_CL_RPCInstance: RF_RPCInstance {

    // MARK: - Override Properties


    override int GetRPCType() { return RF_ESCAPE_FROM_DAYZ_Constant.RPC_TYPE; }

    override string GetModName() { return RF_ESCAPE_FROM_DAYZ_Constant.MOD_NAME; }

    // MARK: - Override

    override void Handle(PlayerIdentity sender, string key, ParamsReadContext ctx) {
        super.Handle(sender, key, ctx);
        switch (key) {
            case "didFetchStorage": {
                handleDidFetchStorage(sender, ctx);
                break;
            }
            case "didReceiveKeyString": {
                handleDidReceiveKeyString(sender, ctx);
                break;
            }
            case "didReceiveKeyInt": {
                handleDidReceiveKeyInt(sender, ctx);
                break;
            }
            case "didReceivePlaySiren": {
                handleDidReceivePlaySiren(sender, ctx);
                break;
            }
            case "didCreateAirstrike": {
                handleDidCreateAirstrike(sender, ctx);
            }
            case "playerDidStartTeleportation": {
                handlePlayerDidStartTeleportation(sender, ctx);
            }
            case "didFetchPlayerStatistics": {
                handleDidFetchPlayerStatistics(sender, ctx);
            }
            case "playerDidEnterEntranceTeleportationZone": {
                handlePlayerDidEnterEntranceTeleportationZone(sender, ctx);
            }
        }
    }

    // MARK: - Handlers

    private void handleDidReceiveKeyString(PlayerIdentity sender, ParamsReadContext ctx) {
        string object;
        ctx.Read(object);
        if (!object) return;

        RF_ESCAPE_FROM_DAYZ_Log().Info(object);
    }

    private void handleDidReceiveKeyInt(PlayerIdentity sender, ParamsReadContext ctx) {
        int object;
        ctx.Read(object);
        if (!object) return;
        RF_ESCAPE_FROM_DAYZ_Log().Info(object.ToString());
    }
    
    private void handleDidReceivePlaySiren(PlayerIdentity sender, ParamsReadContext ctx) {
        RF_ESCAPE_FROM_DAYZ_CL_SirenDTO object;
        ctx.Read(object);
        if (!object) return;
        RF_ESCAPE_FROM_DAYZ_CL_Global.sirenService.ConfigureWith(object);
    }

    private void handleDidCreateAirstrike(PlayerIdentity sender, ParamsReadContext ctx) {
        RF_ESCAPE_FROM_DAYZ_AirstrikeDTO object;
        ctx.Read(object);
        if (!object) return;
        // Refactor that shit
        RF_ESCAPE_FROM_DAYZ_CL_Global.airstrikeServer.ConfigureWith(object);
        RF_ESCAPE_FROM_DAYZ_CL_Global.airstrikeServer.StartAirstrikeDelayed(object);
    }

    private void handlePlayerDidStartTeleportation(PlayerIdentity sender, ParamsReadContext ctx) {
        RF_ESCAPE_FROM_DAYZ_ExtractionTeleportationDto object;
        ctx.Read(object);
        if (!object) return;
        // PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
        RF_ESCAPE_FROM_DAYZ_CL_Global.teleportationService.StartTeleportationAnimation(object);
    }

    private void handleDidFetchPlayerStatistics(PlayerIdentity sender, ParamsReadContext ctx) {
        RF_ESCAPE_FROM_DAYZ_PlayerStatistics object;
        ctx.Read(object);
        if (!object) return;
        RF_ESCAPE_FROM_DAYZ_CL_Global.storage.playerStatistics = object;
    }

    private void handlePlayerDidEnterEntranceTeleportationZone(PlayerIdentity sender, ParamsReadContext ctx) {
        RF_ESCAPE_FROM_DAYZ_Log().Info(string.Format("handlePlayerDidEnterEntranceTeleportationZone"));
        RF_ESCAPE_FROM_DAYZ_RaidEntrancePreloadMenuDto object;
        ctx.Read(object);
        if (!object) return;
        RF_ESCAPE_FROM_DAYZ_CL_Global.storage.presetId = object.presetId;
        RF_ESCAPE_FROM_DAYZ_CL_Global.teleportationService.PlayerDidEnterTeleportationZone(sender, object.presetId);
    }

     private void handleDidFetchStorage(PlayerIdentity sender, ParamsReadContext ctx) {
        autoptr RF_ESCAPE_FROM_DAYZ_CL_Storage object;
        ctx.Read(object);
        if (!object) return;
        RF_ESCAPE_FROM_DAYZ_Log().Info(string.Format("Конфигурация сервера была успешно получена"));
        RF_ESCAPE_FROM_DAYZ_CL_Global.storage = object;
        RF_ESCAPE_FROM_DAYZ_CL_Global.storage.SetupLootPresets();
    }
}

class RF_ESCAPE_FROM_DAYZ_CL_SirenDTO: Managed {
    autoptr TVectorArray positions = {};
}

class RF_ESCAPE_FROM_DAYZ_ExtractionTeleportationDto: Managed {
    string soundSet;
    int soundLength;
    vector loadingPosition;
    int teleportationType;
}