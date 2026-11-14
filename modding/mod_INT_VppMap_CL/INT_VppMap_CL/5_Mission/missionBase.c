modded class MissionBase {
    //INT (2026-11-12): a dedicated server started WITHOUT the server part (-serverMod=@INT_VppMap_SE) says so once.
    //The CfgMods name INT_VppMap_SE is a #define only where that PBO is loaded; nothing of it is named here.
    override void OnMissionStart() {
        super.OnMissionStart();
#ifndef INT_VppMap_SE
        if (GetGame() && GetGame().IsDedicatedServer())
            Print("[INT_VPP] WARNING: @INT_VppMap_SE is not loaded - server markers, zones, admins and file migration are OFF. Start the server with -serverMod=@INT_VppMap_SE");
#endif
    }

    override UIScriptedMenu CreateScriptedMenu(int id) {
        UIScriptedMenu menu = NULL;
        menu = super.CreateScriptedMenu(id);
        if (!menu) {
            switch (id) {
            case VPP_MENU_MAP:
                menu = new VPPMapMenu;
                break;
            }
            if (menu) {
                menu.SetID(id);
            }
        }
        return menu;
    }
}