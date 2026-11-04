class RF_ESCAPE_FROM_DAYZ_CL_Global: Managed {

    // MARK: - Static Properties

    static ref RF_ESCAPE_FROM_DAYZ_CL_KeyHandler keyHandler;

    static ref RF_ESCAPE_FROM_DAYZ_CL_RPCInstance rpc;

    static ref RF_ESCAPE_FROM_DAYZ_CL_SirenService sirenService;

    static ref RF_ESCAPE_FROM_DAYZ_CL_AirstrikeService airstrikeServer;

    static ref RF_ESCAPE_FROM_DAYZ_CL_Storage storage;

    static ref RF_ESCAPE_FROM_DAYZ_CL_TeleportationService teleportationService;

    static ref RF_ESCAPE_FROM_DAYZ_CL_EyesAnimationService eyesAnimationService;

    static ref RF_ESCAPE_FROM_DAYZ_CL_SpawnPresetLootService spawnPresetLootService;

    // MARK: - Static

    static void InitializeUnits() {
        rpc = new RF_ESCAPE_FROM_DAYZ_CL_RPCInstance();
        keyHandler = new RF_ESCAPE_FROM_DAYZ_CL_KeyHandler();
        sirenService = new RF_ESCAPE_FROM_DAYZ_CL_SirenService();
        airstrikeServer = new RF_ESCAPE_FROM_DAYZ_CL_AirstrikeService();
        storage = new RF_ESCAPE_FROM_DAYZ_CL_Storage();
        teleportationService = new RF_ESCAPE_FROM_DAYZ_CL_TeleportationService();
        eyesAnimationService = new RF_ESCAPE_FROM_DAYZ_CL_EyesAnimationService();
        spawnPresetLootService = new RF_ESCAPE_FROM_DAYZ_CL_SpawnPresetLootService();
    }
}
