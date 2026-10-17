// INT_UIFramework - one-time registration of the atlas and the widget styles.
// The imageset MUST be registered before the styles that reference it, or the
// client logs "wrong ImageSet" and crashes on draw (proven in INT_BaseProtection,
// INT_BP_Menu.c:41-50). Once per session; client only.
class INT_UIF_Assets
{
	static bool s_Loaded;

	static void EnsureLoaded()
	{
		if (s_Loaded)
			return;
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;

		s_Loaded = true;
		LoadWidgetImageSet(INT_UIF_IMAGESET_FILE);
		LoadWidgetStyles(INT_UIF_STYLES_FILE);
		INT_UIF_Log.Info("assets registered");
	}
}
