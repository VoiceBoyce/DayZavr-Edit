class RF_ESCAPE_FROM_DAYZ_Utils : Managed {

	private static DayZPlayer previewPlayerRandom;
    private static const vector PLAYERS_PREVIEW_POSITION = Vector(6336.373535, 301.045776, 7676.859863);
	private static string currentPlayerPreviewType = "SurvivorM_Oliver";

	static DayZPlayer GetRandomPlayerPreview(string playerType) {
		if (currentPlayerPreviewType != playerType) {
			currentPlayerPreviewType = playerType;
		}
		if (!RF_ESCAPE_FROM_DAYZ_Utils.previewPlayerRandom) {
			// PlayerIdentity identity = GetGame().GetPlayer().GetIdentity();
			// Print(identity);
			RF_ESCAPE_FROM_DAYZ_Utils.previewPlayerRandom = DayZPlayer.Cast(GetGame().CreatePlayer(null, currentPlayerPreviewType, PLAYERS_PREVIEW_POSITION, 0, "NONE"));
			// auto pp = GetGame().CreateObject(currentPlayerPreviewType, PLAYERS_PREVIEW_POSITION, true);
			// Print(pp);

			// RF_ESCAPE_FROM_DAYZ_Utils.previewPlayerRandom = DayZPlayer.Cast(pp);

			Print(RF_ESCAPE_FROM_DAYZ_Utils.previewPlayerRandom);

			auto player = PlayerBase.Cast(RF_ESCAPE_FROM_DAYZ_Utils.previewPlayerRandom);
			// player.GetStatWater().Set(3000);
			// player.GetStatEnergy().Set(3000);
			// player.SetModifiers(false);
			// player.SetAllowDamage(false);
			// RF_ESCAPE_FROM_DAYZ_Utils.previewPlayerRandom.SetCanBeDestroyed(false);
			// player.DisableSimulation(true);
		}
		return RF_ESCAPE_FROM_DAYZ_Utils.previewPlayerRandom;
	}

	static DayZPlayer GetRandomPlayerPreviewTest(string playerType) {
		return DayZPlayer.Cast(GetGame().CreatePlayer(null, currentPlayerPreviewType, PLAYERS_PREVIEW_POSITION, 0, "NONE"));
	}

	static void ResetRandomPlayerPreview() {
		RF_ESCAPE_FROM_DAYZ_Utils.previewPlayerRandom = null;
	}

    static bool DoesPlayerHaveBackpack(PlayerBase player) {
		auto handsItem = player.GetItemInHands();
		auto backPackInHands = Backpack_Base.Cast(handsItem);
		if (backPackInHands) return true;

		auto backpackOnBack = player.GetItemOnSlot("BACK");
		if (Backpack_Base.Cast(backpackOnBack)) return true;

		return false;
	}

	static void LockControls() {
        RF_ESCAPE_FROM_DAYZ_Log().Info("Отключаем управление");
        GetGame().GetMission().PlayerControlDisable(INPUT_EXCLUDE_ALL);
        GetGame().GetUIManager().ShowUICursor(true);
        GetGame().GetMission().GetHud().Show(false);
    }

    static void UnlockControls() {
        RF_ESCAPE_FROM_DAYZ_Log().Info("Включаем управление");
        GetGame().GetMission().PlayerControlEnable(false);
        GetGame().GetInput().ResetGameFocus();
        GetGame().GetUIManager().ShowUICursor(false);
        GetGame().GetMission().GetHud().Show(true);
    }
}