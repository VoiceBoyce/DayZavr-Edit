modded class ActionTurnOnWhileInHands {

    // MARK: - Override methods

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item) {
        if (!super.ActionCondition(player, target, item)) return false;
        if (item.GetType() != "RF_ESCAPE_FROM_DAYZ_Roadflare") return true;
        // RF_ESCAPE_FROM_DAYZ_Log().Info(string.Format("Is under roof: %1. Is sound inside building: %2", MiscGameplayFunctions.IsUnderRoof(player), player.IsSoundInsideBuilding()));
        return !player.IsSoundInsideBuilding();
    }
}