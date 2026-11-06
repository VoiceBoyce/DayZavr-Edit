class RF_NICKNAMES_CL_HeadLabelHud: Managed {
	
	// MARK: - Private Properties --------------------------------------------------------------------------

	private Widget layoutRoot;
	private TextWidget labelTextWidget;
	private PlayerBase player;

	private bool isShown = false; // что сейчас показано, чтобы не трогать виджет каждый кадр
	private string shownName = "";

	private const float VIEW_DISTANCE = 9.0; // 9 метров
	private int HEAD_Y_OFFSET = 2.5; // поднять ник относительно земли на 2.5 метра
	private int HEAD_X_OFFSET = 0; // сместить ник в сторону

	// MARK: - Init --------------------------------------------------------------------------

	void RF_NICKNAMES_CL_HeadLabelHud() {
		setupWidgets();
		layoutRoot.Show(false);
		// RF_NICKNAMES_Log().Info(string.Format("init %1", this));
	}

	void ~RF_NICKNAMES_CL_HeadLabelHud() {
		// RF_NICKNAMES_Log().Info(string.Format("~%1", this));
		layoutRoot.Unlink();
	}

	void ConfigureWith(PlayerBase player) {
        if (!player) return;
		this.player = player;
	}

	void OnUpdate() {
		this.updatePosition();	
	}

	// MARK: - Private Methods --------------------------------------------------------------------------

	private void setupWidgets() {
		this.layoutRoot = GetGame().GetWorkspace().CreateWidgets(RF_NICKNAMES_Constant.HEAD_LABEL_HUD, null);
        this.labelTextWidget = TextWidget.Cast(layoutRoot.FindAnyWidget("PlayerNameTextWidget"));
	}

	private void setVisible(bool visible) {
		if (isShown == visible) return;
		isShown = visible;
		layoutRoot.Show(visible);
	}

	private void updatePosition() {
		if (!layoutRoot || !player) return;
		auto identity = player.GetIdentity();

		if (!identity || !GetGame() || !GetGame().GetPlayer() || isPlayerEqualClient()) {
			setVisible(false);
			return;
		}

		// int headIdx = this.player.GetBoneIndexByName("Head");
		// vector playerPosition = this.player.GetBonePositionWS(headIdx);
		vector playerPosition = this.player.GetPosition();

		playerPosition[1] = playerPosition[1] + HEAD_Y_OFFSET;
		playerPosition[2] = playerPosition[2] + HEAD_X_OFFSET;

		vector currentPosition = GetGame().GetCurrentCameraPosition();
		float distance = Math.Ceil(vector.Distance(playerPosition, currentPosition));

		// Сначала дешёвые проверки: большинство игроков дальше 9 метров,
		// и для них луч и работа с виджетом не нужны
		if (distance > VIEW_DISTANCE || GetGame().GetUIManager().GetMenu() || !canShow(playerPosition)) {
			setVisible(false);
			return;
		}

		string playerName = identity.GetName();
		if (playerName != shownName) {
			shownName = playerName;
			this.labelTextWidget.SetText(playerName);
		}

		vector screenPosition = GetGame().GetScreenPos(playerPosition);
		float x = Math.Ceil(screenPosition[0]);
        float y = Math.Ceil(screenPosition[1]);

		layoutRoot.SetPos(x, y);
		setVisible(true);
	}

	private bool isPlayerEqualClient() {
		if (!player) return false;
		auto identity = player.GetIdentity();
		auto clientPlayer = GetGame().GetPlayer();
		if (!clientPlayer) return false;
		auto clientIdentity = clientPlayer.GetIdentity();
		return identity == clientIdentity;
	}

	private bool canShow(vector position) {
		vector relativePos = GetGame().GetScreenPosRelative(position);
		if (relativePos[0] >= 1 || relativePos[0] == 0 || relativePos[1] >= 1 || relativePos[1] == 0 || relativePos[2] < 0) return false;

		vector from = GetGame().GetCurrentCameraPosition();
		vector to = player.GetPosition();
		vector contactPosition;
		vector contactDir = vector.Zero;
		set<Object> hitObjects = new set<Object>();
		int contactComponent;

		DayZPhysics.RaycastRV(from, to, contactPosition, contactDir, contactComponent, hitObjects, player, GetGame().GetPlayer(), true, false, ObjIntersectView);

		if (hitObjects.Count() >= 1) {
			if (hitObjects[0] != this.player) return false;
		}

		return true;
	}
}

typedef array<autoptr RF_NICKNAMES_CL_HeadLabelHud> RF_NICKNAMES_CL_HeadLabelHudsArray;
typedef map<string, autoptr RF_NICKNAMES_CL_HeadLabelHud> RF_NICKNAMES_CL_HeadLabelHudsMap;



// class ActionConsume: ActionContinuousBase {
// 	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item) {	
// 		if (!super.ActionCondition(player, target, item)) return false;
		
// 		auto mask = player	
// 	}
// };