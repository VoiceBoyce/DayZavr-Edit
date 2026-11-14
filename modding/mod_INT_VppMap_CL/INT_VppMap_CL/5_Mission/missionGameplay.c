// VanillaPPMap (our VPP fork) - map lifecycle (MAP agent, «Экспедиция» B, 2026-11-12).
// Kept for the integrations (BUILD_PLAN §1): field `instance`, LockControls / UnlockControls, CanOpenMap, the four
// RPC handlers (MarkerCheck, OwnPositionMarkerDisabled, SessionMarkers, ForceMapItem) and K -> TempToggleAll3dMarkers.
// New lifecycle: the map window exists only while it is open. INT_VPP_OpenMap = EnterScriptedMenu(VPP_MENU_MAP)
// (Init builds everything); INT_VPP_CloseMap = INT_VPP_Teardown (every tree / timer / subscription) -> the menu is
// hidden -> its layoutRoot unlinked -> instance = null. M and Esc go through them; a close made by someone else
// (VPPMapItemRequirements' OpenMap copy hides the menu, UIManager.CloseAll) is noticed in OnUpdate and finished
// the same way.
modded class MissionGameplay {
    private ref VPPMapMenu instance;
    //INT (review #1): a menu opened over the map (vanilla warning / note, a UIF window, the pause menu) owns the
    //cursor, the game focus and the "menu" exclude while it is up: our controls go back only once no menu is up
    protected bool m_INT_VPP_PendingUnlock;
    //INT: our LockControls is in force (until UnlockControls). `instance` alone cannot say it: when the engine
    //destroys the menu (UIManager.CloseAll, CloseMenu by a UIF window, someone's menu.Close()) the ref goes null
    //before OnUpdate sees it - measured 2026-11-12: the controls then stayed locked ("menu" exclude) for good
    protected bool m_INT_VPP_Locked;

    void MissionGameplay() {
        GetRPCManager().AddRPC( "RPC_MissionGameplay", "MarkerCheck", this, SingleplayerExecutionType.Server );
        GetRPCManager().AddRPC( "RPC_MissionGameplay", "OwnPositionMarkerDisabled", this, SingleplayerExecutionType.Server );
        GetRPCManager().AddRPC( "RPC_MissionGameplay", "SessionMarkers", this, SingleplayerExecutionType.Server );
        GetRPCManager().AddRPC( "RPC_MissionGameplay", "ForceMapItem", this, SingleplayerExecutionType.Server );

        GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(this.GetMarkerData, 5000, false);
    }

    //INT fix: the 3D markers live on DayZGame; without this they stayed alive (ticking every frame) after leaving the server
    override void OnMissionFinish() {
        //INT: the open map goes first (its trees, timers and subscriptions), while the mission still exists
        INT_VPP_CloseMap();
        m_INT_VPP_PendingUnlock = false;
        m_INT_VPP_Locked = false;
        if (GetGame())
            GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(this.GetMarkerData);
        super.OnMissionFinish();
        g_Game.SetClient3dMarkers(NULL);
        g_Game.SetServer3dMarkers(NULL);
    }

    void GetMarkerData()
    {
        GetRPCManager().SendRPC( "RPC_MissionServer", "MarkerCheck", NULL, true);
        GetRPCManager().SendRPC( "RPC_MissionServer", "OwnPositionMarkerDisabled", NULL, true);
        GetRPCManager().SendRPC( "RPC_MissionServer", "SessionMarkers", NULL, true);
        GetRPCManager().SendRPC( "RPC_MissionServer", "ForceMapItem", NULL, true);
    }

    void OwnPositionMarkerDisabled( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target ) {
        if (type == CallType.Client) {
            Param1<bool> data;
            if ( !ctx.Read( data ) ) return;
            g_Game.SetOwnPositionMarkerDisabled(data.param1);
        }
    }

    void MarkerCheck( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target ) {
        if (type == CallType.Client) {
            Param1<bool> data;
            if ( !ctx.Read( data ) ) return;
            g_Game.SetUse3DMarkers(data.param1);
        }
    }

    void SessionMarkers( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target ) {
        if (type == CallType.Client) {
            Param1<ref array<ref MarkerInfo>> data;
            if ( !ctx.Read( data ) ) return;
            //INT (2026-11-14): the mode keys follow server markers an admin renamed / moved (key = name + position)
            INT_VPP_MigrateModeKeys(g_Game.GetServerMarkers(), data.param1);
            g_Game.SetServerMarkers(data.param1);
        }
    }

    //INT (2026-11-14): old list -> new list by INT_SrvId; a changed key moves the player's mode to the new key
    void INT_VPP_MigrateModeKeys(array<ref MarkerInfo> oldList, array<ref MarkerInfo> newList) {
        if (!oldList || !newList)
            return;
        INT_VPP_Prefs prefs = INT_VPP_Prefs.INT_Peek();
        if (!prefs)
            return;
        string server = g_Game.GetConnectAddressPort();
        map<int, string> oldKeys = new map<int, string>();
        foreach (MarkerInfo o : oldList) {
            if (!o || o.INT_GetSrvId() <= 0)
                continue;
            string ok = INT_VPP_Prefs.INT_ServerKey(server, o);
            oldKeys.Set(o.INT_GetSrvId(), ok);
        }
        bool moved = false;
        foreach (MarkerInfo n : newList) {
            if (!n || n.INT_GetSrvId() <= 0)
                continue;
            string was;
            if (!oldKeys.Find(n.INT_GetSrvId(), was))
                continue;
            string now = INT_VPP_Prefs.INT_ServerKey(server, n);
            if (now == was)
                continue;
            if (prefs.INT_MoveMarkerMode(was, now))
                moved = true;
        }
        if (moved)
            prefs.INT_Save();
    }

    void ForceMapItem(CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target){
        if (type == CallType.Client) {
            Param1<bool> data;
            if ( !ctx.Read( data ) ) return;
            g_Game.SetForcePhysicalMap(data.param1);
        }
    }

    //Checks if server config forces player to have a physical map item to open the map, returns true if player has map, returns true if option is not forced, returns false if option is forced and player has not physical map in inventory
    bool CanOpenMap(){
        if (!g_Game.ForcedPhysicalMap())
            return true;

        if (g_Game.ForcedPhysicalMap()){
            //Check Inventory for map item
            array<EntityAI> itemsArray = new array<EntityAI>;
            PlayerBase pb;

            if (Class.CastTo(pb,GetGame().GetPlayer())){
                pb.GetInventory().EnumerateInventory(InventoryTraversalType.INORDER, itemsArray); //INORDER,PREORDER

                for (int i = 0; i < itemsArray.Count(); i++){
                    if (itemsArray[i] != null){
                        if (itemsArray[i].IsInherited(ChernarusMap) || itemsArray[i].IsInherited(ItemMap) || itemsArray[i].IsKindOf("ChernarusMap"))
                            return true;
                    }
                }
            }
        }
        return false;
    }

    // ===================================================================== INT (MAP): lifecycle
    // Test hooks (BUILD_PLAN §8) and the only way the map opens / closes.
    void INT_VPP_OpenMap() {
        if (!GetGame() || GetGame().IsDedicatedServer())
            return;
        if (instance)
            return;
        UIManager um = GetGame().GetUIManager();
        if (!um)
            return;
        if (um.GetMenu() != NULL)
            return;
        if (!INT_VPP_PlayerCanUseMap())
            return;
        if (!CanOpenMap())
            return;
        // no menu is up: a pending unlock is replaced by this lock
        m_INT_VPP_PendingUnlock = false;
        LockControls();
        UIScriptedMenu made = um.EnterScriptedMenu(VPP_MENU_MAP, null);
        VPPMapMenu menu = VPPMapMenu.Cast(made);
        if (!menu) {
            UnlockControls();
            return;
        }
        instance = menu;
        menu.SetMenuOpen(true);
        INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.OPEN);
    }

    void INT_VPP_CloseMap() {
        VPPMapMenu menu = instance;
        if (!menu)
            return;
        menu.SetMenuOpen(false);
        menu.INT_VPP_Teardown();
        instance = null;
        UIManager um = null;
        if (GetGame())
            um = GetGame().GetUIManager();
        //INT (review #9): Close() only while the map is the current menu (it destroys the menu; the destructor unlinks
        //layoutRoot). Hidden / closed by someone else: its AfterHide finishes it. The parent of a menu opened over
        //it: its OnShow finishes it when that menu closes - a Close() now would take the other menu down too.
        if (um && um.GetMenu() == menu)
            menu.INT_VPP_CloseSelf();
        //INT (review #1): another menu still up = it owns the cursor / focus / excludes: unlock when it is gone
        UIScriptedMenu top = null;
        if (um)
            top = um.GetMenu();
        if (top) {
            m_INT_VPP_PendingUnlock = true;
            return;
        }
        UnlockControls();
    }

    // a map is not read while dead / unconscious (review #13) - and never without a player
    protected bool INT_VPP_PlayerCanUseMap() {
        if (!GetGame())
            return false;
        PlayerBase pb = PlayerBase.Cast(GetGame().GetPlayer());
        if (!pb)
            return false;
        if (!pb.IsAlive())
            return false;
        if (pb.IsUnconscious())
            return false;
        return true;
    }

    // TEST: controls are waiting for the menu over the map to close
    bool INT_VPP_IsUnlockPending() {
        return m_INT_VPP_PendingUnlock;
    }

    bool INT_VPP_IsMapOpen() {
        if (instance)
            return true;
        return false;
    }

    override void OnUpdate(float timeslice) {
        super.OnUpdate(timeslice);
        if (!GetGame() || GetGame().IsDedicatedServer())
            return;
        Input input = GetGame().GetInput();
        if (!input)
            return;

        //INT: a close made by someone else (HideScriptedMenu / Close() / CloseAll / a menu opened over the map: the
        //menu tore itself down in OnHide) is finished here: instance dropped, controls given back (now or later)
        UIManager um = GetGame().GetUIManager();
        if (instance) {
            bool gone = false;
            if (!instance.IsMenuOpen())
                gone = true;
            if (!instance.INT_IsAlive())
                gone = true;
            if (um) {
                UIScriptedMenu found = um.FindMenu(VPP_MENU_MAP);
                if (!found)
                    gone = true;
            }
            //INT (review #13): the map closes when the player dies or falls unconscious
            if (!INT_VPP_PlayerCanUseMap())
                gone = true;
            if (gone)
                INT_VPP_CloseMap();
        }
        //INT: the engine destroyed the map under us (instance already null): finish like any other close
        if (!instance && m_INT_VPP_Locked)
            m_INT_VPP_PendingUnlock = true;
        //INT (review #1): the menu that was opened over the map is gone too: now the controls come back
        if (m_INT_VPP_PendingUnlock && !instance) {
            UIScriptedMenu top = null;
            if (um)
                top = um.GetMenu();
            if (!top)
                UnlockControls();
        }

        if (input.LocalPress("UAUIBack", false)) {
            if (instance && instance.IsMenuOpen()) {
                //INT: popover / confirm -> editor / zone editor / ruler -> card -> the map
                bool used = instance.INT_VPP_HandleBack();
                if (!used && !g_Game.IsKeyboardBusy()) {
                    INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CLOSE);
                    INT_VPP_CloseMap();
                }
            }
        }

        if ( input.LocalPress("UAOpenVPPMap", false) ) {
            if (instance) {
                if (!g_Game.IsKeyboardBusy()) {
                    INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CLOSE);
                    INT_VPP_CloseMap();
                }
            } else {
                INT_VPP_OpenMap();
            }
        }

        if ( input.LocalPress("UAToggleVPPMap3DMarkers", false) ) {
            if (!g_Game.IsKeyboardBusy()) {
                g_Game.TempToggleAll3dMarkers();
                if (instance)
                    INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.TOGGLE);
            }
        }
    }

    private void LockControls() {
        if (!GetGame() || !GetGame().GetMission())
            return;
        GetGame().GetMission().PlayerControlDisable(INPUT_EXCLUDE_ALL);
        m_INT_VPP_Locked = true;
        if (GetGame().GetUIManager())
            GetGame().GetUIManager().ShowUICursor( true );
        Hud hud = GetGame().GetMission().GetHud();
        if (hud)
            hud.Show( false );
    }

    private void UnlockControls() {
        m_INT_VPP_PendingUnlock = false;
        m_INT_VPP_Locked = false;
        if (!GetGame() || !GetGame().GetMission())
            return;
        GetGame().GetMission().PlayerControlEnable(false);
        if (GetGame().GetInput())
            GetGame().GetInput().ResetGameFocus();
        if (GetGame().GetUIManager())
            GetGame().GetUIManager().ShowUICursor( false );
        //INT (review #1): the HUD is never forced on for a dead / unconscious player (the death screen and the
        //unconscious screen own it)
        Hud hud = GetGame().GetMission().GetHud();
        if (hud && INT_VPP_PlayerCanUseMap())
            hud.Show( true );
    }
}
