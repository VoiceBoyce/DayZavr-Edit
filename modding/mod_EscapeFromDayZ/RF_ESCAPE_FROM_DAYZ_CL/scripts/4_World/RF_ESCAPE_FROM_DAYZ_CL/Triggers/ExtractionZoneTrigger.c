class RF_ESCAPE_FROM_DAYZ_ExtractionZoneTrigger : Trigger {

	private autoptr RF_ESCAPE_FROM_DAYZ_TeleportTimerHud teleportTimerHud;
	private bool RF_ESCAPE_FROM_DAYZ_SE_isTeleportTimerHudShown;

	private int RF_ESCAPE_FROM_DAYZ_CL_extractionTime;
	private bool RF_ESCAPE_FROM_DAYZ_CL_requireBackpack;
	private bool RF_ESCAPE_FROM_DAYZ_CL_isActive;
	private bool RF_ESCAPE_FROM_DAYZ_CL_isEntrance;

	// MARK: - Init --------------------------------------------------------------------------

	void RF_ESCAPE_FROM_DAYZ_ExtractionZoneTrigger() {
		int mask = GetEventMask();
		mask &= ~EntityEvent.INIT;
		// SetEventMask(mask);
		// Print(mask);
		RF_ESCAPE_FROM_DAYZ_Log().Info(string.Format("Teleport zone inited"));
		RegisterNetSyncVariableInt("RF_ESCAPE_FROM_DAYZ_CL_extractionTime");
		RegisterNetSyncVariableBool("RF_ESCAPE_FROM_DAYZ_CL_isActive");
		RegisterNetSyncVariableBool("RF_ESCAPE_FROM_DAYZ_CL_requireBackpack");
		RegisterNetSyncVariableBool("RF_ESCAPE_FROM_DAYZ_CL_isEntrance");
	}

	void ~RF_ESCAPE_FROM_DAYZ_ExtractionZoneTrigger() {
		RF_ESCAPE_FROM_DAYZ_Log().Info(string.Format("~ %1", this));
	}

	// MARK: - Override Methods --------------------------------------------------------------------------

	override protected bool CanAddObjectAsInsider(Object object) {
		if (!object.IsInherited(EntityAI)) return false;
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
		
		if (this.RF_ESCAPE_FROM_DAYZ_CL_requireBackpack && RF_ESCAPE_FROM_DAYZ_Utils.DoesPlayerHaveBackpack(player)) {
			this.RF_ESCAPE_FROM_DAYZ_CL_closeTimerHud();
			return;
		}
		// // Закрываем инвентарь
		// player.CloseInventoryMenu();
		if (this.RF_ESCAPE_FROM_DAYZ_SE_isTeleportTimerHudShown) return;
		if (!this.RF_ESCAPE_FROM_DAYZ_CL_isActive) return;
		if (this.RF_ESCAPE_FROM_DAYZ_CL_isEntrance && (player.RF_ESCAPE_FROM_DAYZ_GetPlayerPresetChoice() == RF_ESCAPE_FROM_DAYZ_PlayerPresetChoiceEnum.NOT_CHOOSEN)) return;
		if (this.RF_ESCAPE_FROM_DAYZ_CL_isEntrance && (player.RF_ESCAPE_FROM_DAYZ_GetPlayerPresetChoice() == RF_ESCAPE_FROM_DAYZ_PlayerPresetChoiceEnum.PENDING)) return;
		
		teleportTimerHud = new RF_ESCAPE_FROM_DAYZ_TeleportTimerHud();
		teleportTimerHud.SetTimerTime(RF_ESCAPE_FROM_DAYZ_CL_extractionTime);
		teleportTimerHud.StartTimer();
		this.RF_ESCAPE_FROM_DAYZ_SE_isTeleportTimerHudShown = true;
	}

	private void RF_ESCAPE_FROM_DAYZ_CL_closeTimerHud() {
		if (!this.RF_ESCAPE_FROM_DAYZ_SE_isTeleportTimerHudShown) return;
		this.teleportTimerHud = NULL;
		this.RF_ESCAPE_FROM_DAYZ_SE_isTeleportTimerHudShown = false;
	}
}