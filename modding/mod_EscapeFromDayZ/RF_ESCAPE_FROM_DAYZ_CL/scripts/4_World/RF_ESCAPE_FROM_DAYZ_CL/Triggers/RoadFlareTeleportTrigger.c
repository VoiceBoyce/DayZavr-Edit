class RF_ESCAPE_FROM_DAYZ_RoadFlareTeleportTrigger : Trigger {

	private autoptr RF_ESCAPE_FROM_DAYZ_TeleportTimerHud teleportTimerHud;
	private bool RF_ESCAPE_FROM_DAYZ_SE_isTeleportTimerHudShown;
	private bool RF_ESCAPE_FROM_DAYZ_CL_isActive;
	private int RF_ESCAPE_FROM_DAYZ_CL_extractionTime;

	// MARK: - Init --------------------------------------------------------------------------

	void RF_ESCAPE_FROM_DAYZ_RoadFlareTeleportTrigger() {
		RF_ESCAPE_FROM_DAYZ_Log().Info(string.Format("Roadflare teleport zone inited"));
		RegisterNetSyncVariableInt("RF_ESCAPE_FROM_DAYZ_CL_extractionTime");
		RegisterNetSyncVariableBool("RF_ESCAPE_FROM_DAYZ_CL_isActive");
	}

	void ~RF_ESCAPE_FROM_DAYZ_RoadFlareTeleportTrigger() {
		RF_ESCAPE_FROM_DAYZ_Log().Info(string.Format("~ %1", this));
	}

	// MARK: - Override Methods --------------------------------------------------------------------------

	override protected bool CanAddObjectAsInsider(Object object) {
		if (!object.IsInherited(SurvivorBase)) return false;
		return true;
	}

	override void OnLeaveClientEvent(TriggerInsider insider) {
		super.OnLeaveClientEvent(insider);

		auto player = PlayerBase.Cast(GetGame().GetPlayer());
		PlayerBase playerInsider = PlayerBase.Cast(insider.GetObject());
		if (player != playerInsider) return;

		this.RF_ESCAPE_FROM_DAYZ_CL_closeTimerHud();
	}

	override void OnStayClientEvent(TriggerInsider insider, float deltaTime) {
		super.OnStayClientEvent(insider, deltaTime);		
		auto player = PlayerBase.Cast(GetGame().GetPlayer());
		PlayerBase playerInsider = PlayerBase.Cast(insider.GetObject());
		if (player != playerInsider) return;

		// // Закрываем инвентарь
		// player.CloseInventoryMenu();
		if (this.RF_ESCAPE_FROM_DAYZ_SE_isTeleportTimerHudShown) return;
		if (!this.RF_ESCAPE_FROM_DAYZ_CL_isActive) return;
		
		teleportTimerHud = new RF_ESCAPE_FROM_DAYZ_TeleportTimerHud();
		teleportTimerHud.SetTimerTime(RF_ESCAPE_FROM_DAYZ_CL_extractionTime);
		teleportTimerHud.StartTimer();
		this.RF_ESCAPE_FROM_DAYZ_SE_isTeleportTimerHudShown = true;
	}

	override void OnVariablesSynchronized() {
		super.OnVariablesSynchronized();
		if (RF_ESCAPE_FROM_DAYZ_CL_isActive) return;
		this.RF_ESCAPE_FROM_DAYZ_CL_closeTimerHud();
	}

	private void RF_ESCAPE_FROM_DAYZ_CL_closeTimerHud() {
		if (!this.RF_ESCAPE_FROM_DAYZ_SE_isTeleportTimerHudShown) return;
		this.teleportTimerHud = NULL;
		this.RF_ESCAPE_FROM_DAYZ_SE_isTeleportTimerHudShown = false;
	}
}