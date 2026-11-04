class RF_ESCAPE_FROM_DAYZ_TeleportTimerHud: Managed {

    private autoptr TextWidget timerValueTextWidget;
    private autoptr TextWidget timerMessageTextWidget;
    private autoptr Widget layoutRoot;

    private float timerTime;

    // MARK: - Init

    void RF_ESCAPE_FROM_DAYZ_TeleportTimerHud() {
        setupWidgets();
    }

    void ~RF_ESCAPE_FROM_DAYZ_TeleportTimerHud() {
        RF_ESCAPE_FROM_DAYZ_Log().Info("~RF_ESCAPE_FROM_DAYZ_TeleportTimerHud");
        if (layoutRoot) {
		    layoutRoot.Unlink();
        }

        auto game = GetDayZGame();
		if (!game) return
		game.Unsubscribe(this, "OnUpdate");
    }

    void SetTimerTime(float value) {
        timerTime = value;
        string valueString = value.ToString();
        int pos = valueString.IndexOf(".");
        valueString = valueString.Substring(0, pos + 2);
        this.timerValueTextWidget.SetText(string.Format("%1", valueString));
    }

    void StartTimer() {
        auto game = GetDayZGame();
		if (!game) return;
		game.Subscribe(this, "OnUpdate");
    }

    private void OnUpdate(bool doSim, float timeslice) {
        if (timerTime <= 0) {
            layoutRoot.Show(false);

            auto game = GetDayZGame();
            if (!game) return;
            game.Unsubscribe(this, "OnUpdate");
            return;
        }
        
        SetTimerTime(timerTime);
        timerTime -= timeslice;
    }
    
    // MARK: - Private Methods

	private void setupWidgets() {
		layoutRoot = GetGame().GetWorkspace().CreateWidgets(RF_ESCAPE_FROM_DAYZ_Constant.TELEPORTATION_TIMER_HUD);
		timerValueTextWidget = TextWidget.Cast(layoutRoot.FindAnyWidget("TimerValueTextWidget"));
		timerMessageTextWidget = TextWidget.Cast(layoutRoot.FindAnyWidget("TimerMessageTextWidget"));
	}
}