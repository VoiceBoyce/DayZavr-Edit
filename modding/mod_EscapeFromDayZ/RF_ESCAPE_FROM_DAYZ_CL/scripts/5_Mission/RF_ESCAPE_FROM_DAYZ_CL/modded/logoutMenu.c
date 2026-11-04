modded class LogoutMenu {

    override void UpdateInfo() {
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player.IsRestrained() || player.IsUnconscious()) {
			m_DescriptionText.SetText("#layout_logout_dialog_note_killed");
		} else {
			m_DescriptionText.SetText("#Logout_LeaveRaidWarning");
		}
	}
}