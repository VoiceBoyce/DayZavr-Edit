// ============================================================================
// VanillaPPMap (our VPP fork) - the map window, «Экспедиция» concept B «ОБЗОР» (MAP agent, phase 2, 2026-11-12).
// Integration contract (BUILD_PLAN §1, byte-compatible): class VPPMapMenu, VPP_MENU_MAP, field m_MapWidget,
// GetLayoutRoot() holds ONE MapWidget "Map_Widget" (full screen, priority 100: PartyMode / PvEZ / Quests put their
// layers inside it), MapDoubleClick / DisplayPlayerPosition / DisplayClientMarkers / DisplayServerMarkers /
// IsMenuOpen / SetMenuOpen keep their signatures and meaning (VPPMapItemRequirements overrides the first two and
// calls super; our own double-click marker and own-position refresh go through them). The old VPP methods used by
// the unused list / edit classes stay (RemoveMarkerExact, EditMarker*, ShowDialog, HideDialog, VectorToARGB).
// Lifecycle: a fresh Init() per open builds everything; INT_VPP_Teardown (from INT_VPP_CloseMap, OnHide or the
// destructor, runs once) unlinks every tree, drops every subscription; nothing survives a close.
// Notes for the lead:
//  - layoutRoot is unlinked by INT_VPP_ReleaseRoot after HideScriptedMenu (or in the destructor), like RF's menus.
//  - a menu opened ON TOP of the map hides it (OnHide -> teardown); its OnShow then closes it (never frozen).
// 2026-11-13: admin text labels - tab «05 Надписи» (admins) = INT_VPP_LabelEditor at the zone editor's place; the
// labels themselves are drawn by INT_VPP_MapView («Зоны» layer); delete through the same confirm band.
// ============================================================================

class VPPMapMenu extends UIScriptedMenu {

    // ---- kept VPP members ----
    private ref array<ref MarkerInfo> m_ClientMarkers = new array<ref MarkerInfo>();
    private ref array<ref MarkerInfo> m_ServerMarkers = new array<ref MarkerInfo>();
    private ref array<ref MarkerInfo> m_CustomServerMarkers = new array<ref MarkerInfo>();
    private bool m_IsMenuOpen;
    private string m_AddressPort;

    MapWidget m_MapWidget;

    private ref MarkerInfo m_LastEditedMarker;
    private string m_LastEditedMarkerName;
    private int m_MarkerSuffix = 1;

    // ---- INT (MAP) ----
    static const float INT_ME_TICK = 0.25;
    static const float INT_ROUTE_NAME_R = 50;
    static const float INT_CLICK_PX = 5;
    static const int INT_BACK_GAP_MS = 250;
    static const int INT_TAB_MAP = 0;
    static const int INT_TAB_MARKS = 1;
    static const int INT_TAB_ROUTE = 2;
    static const int INT_TAB_ZONES = 3;
    static const int INT_TAB_LABELS = 4;
    static const int INT_CONFIRM_MARKER = 1;
    static const int INT_CONFIRM_ZONE = 2;
    static const int INT_CONFIRM_LABEL = 3;
    // a server marker (map admins, 2026-11-14)
    static const int INT_CONFIRM_SRVMARKER = 4;
    static const string INT_TXT_POINT = "Точка";
    static const string INT_TXT_ROUTE_FULL = "Маршрут: предел точек";
    static const string INT_TXT_RULER_FULL = "Линейка: предел точек";
    static const string INT_TXT_ROUTE_OFF = "Слой «Маршрут» выключен";

    // view memory across opens (this session)
    protected static vector s_INT_LastPos;
    protected static float s_INT_LastScale;
    protected static bool s_INT_HasLast;
    protected static int s_INT_LastColor = 0xFF56E07E;
    protected static string s_INT_LastIcon;

    protected bool m_INT_Built;
    protected bool m_INT_TornDown;
    protected bool m_INT_CloseIssued;
    protected bool m_INT_StatRoot;
    protected int m_INT_Frames;

    protected Widget m_INT_Screen;
    protected float m_INT_ScrW;
    protected float m_INT_ScrH;
    protected float m_INT_F = 1;
    protected Widget m_INT_ConfirmHost;
    protected ref array<Widget> m_INT_Graveyard;

    protected ref INT_VPP_MapView m_INT_View;
    protected ref INT_VPP_Islands m_INT_Isl;
    protected ref INT_VPP_MarkerCard m_INT_Card;
    protected ref INT_VPP_EditorCard m_INT_Editor;
    protected ref INT_VPP_RouteRibbon m_INT_Ribbon;
    protected ref INT_VPP_RulerCard m_INT_RulerCard;
    protected ref INT_VPP_ZoneEditor m_INT_ZoneEd;
    protected ref INT_VPP_LabelEditor m_INT_LabelEd;
    protected ref INT_VPPX_Confirm m_INT_Confirm;

    protected ref array<ref INT_VPP_MapItem> m_INT_Items;
    protected ref MarkerInfo m_INT_CardInfo;
    protected ref MarkerInfo m_INT_EditInfo;
    protected int m_INT_EditKind;
    protected vector m_INT_EditorPos;
    protected int m_INT_Tab;
    protected bool m_INT_Admin;

    // confirm band
    protected int m_INT_ConfirmKind;
    protected ref MarkerInfo m_INT_ConfirmInfo;
    protected int m_INT_ConfirmZoneId;
    protected int m_INT_ConfirmLabelId;
    protected int m_INT_ConfirmSrvId;
    // the last server-marker answer seen (a refusal shows a notice)
    protected int m_INT_SeenSrvResults;
    protected string m_INT_ConfirmLabelText;
    protected int m_INT_ConfirmLabelSize;
    protected string m_INT_ConfirmIcon;
    protected int m_INT_ConfirmColor;

    // ruler
    protected bool m_INT_RulerOn;
    protected ref array<vector> m_INT_RulerPts;

    // input
    protected bool m_INT_LmbWas;
    protected bool m_INT_RmbWas;
    protected int m_INT_PressX;
    protected int m_INT_PressY;
    protected bool m_INT_PressOnMap;
    protected bool m_INT_PressOnUi;
    protected int m_INT_RPressX;
    protected int m_INT_RPressY;
    protected bool m_INT_RPressOnMap;
    protected int m_INT_LastMouseX = -1;
    protected int m_INT_LastMouseY = -1;
    protected bool m_INT_SpaceWas;
    protected bool m_INT_RWas;
    protected bool m_INT_EnterWas;
    protected bool m_INT_EscWas;
    protected int m_INT_LastBackMs = -100000;

    // view guard (the engine MapWidget takes LMB under our islands: memory dayz-mapwidget-native-drag)
    protected vector m_INT_ViewPos;
    protected float m_INT_ViewScale;
    protected bool m_INT_HaveView;
    protected float m_INT_Guard;
    static int s_INT_Drifts;

    // me
    protected float m_INT_MeTick;
    protected vector m_INT_LastMePos;
    protected float m_INT_LastHeading = -1000;
    protected bool m_INT_MeShown;
    protected bool m_INT_Temp3D;
    protected int m_INT_SrvCount = -2;

    // cursor text
    protected int m_INT_CurWX = -1000000;
    protected int m_INT_CurWZ = -1000000;

    // dirty flags (invokers / actions -> processed in Update, never inside a click)
    protected bool m_INT_DirtyIslands;
    protected bool m_INT_DirtyRibbon;
    protected bool m_INT_DirtyList;
    protected bool m_INT_DirtyAdmin;
    protected bool m_INT_DirtyRuler;
    protected bool m_INT_CardsDirty;
    protected int m_INT_LastPlaced = -1;
    protected bool m_INT_LastZEdit;
    protected bool m_INT_LastLEdit;
    // review #5: g_Game.INT_VPP_3DRevision() seen last (RF markers moved in place on an open map)
    protected int m_INT_SeenRev3D = -1;
    // sound: the marker under the cursor last frame (weak; HOVER on change)
    protected INT_VPP_MapItem m_INT_HovLast;
    // per-change scratch (no allocation per repaint)
    protected ref array<int> m_INT_ScrCounts;
    // per-click scratch: override keys of the server / event markers a chip changed
    protected ref array<string> m_INT_ScrKeys;
    protected bool m_INT_AfterHideQueued;

    void VPPMapMenu()
    {
        m_INT_Graveyard = new array<Widget>();
        m_INT_Items = new array<ref INT_VPP_MapItem>();
        m_INT_RulerPts = new array<vector>();
        m_INT_ScrCounts = new array<int>();
        m_INT_ScrKeys = new array<string>();
    }

    // review #9: the layout root is unlinked only here (never while the engine may still own a hidden menu), and a
    // queued INT_VPP_AfterHide never runs on a dead menu
    void ~VPPMapMenu() {
        if (!GetGame())
            return;
        GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(INT_VPP_AfterHide);
        INT_VPP_Teardown();
        INT_VPP_ReleaseRoot();
    }

    override Widget Init() {
        m_AddressPort = g_Game.GetConnectAddressPort();
        if (!GetGame().GetWorkspace())
            return null;
        layoutRoot = GetGame().GetWorkspace().CreateWidgets( "VanillaPPMap/GUI/Layouts/VPPMapMenu.layout" );
        if (!layoutRoot)
            return null;
        INT_VPP_Stat.INT_Up(0);
        m_INT_StatRoot = true;
        m_MapWidget = MapWidget.Cast( layoutRoot.FindAnyWidget( "Map_Widget" ) );
        if (m_MapWidget && WidgetEventHandler.GetInstance())
            WidgetEventHandler.GetInstance().RegisterOnDoubleClick( m_MapWidget, this, "MapDoubleClick" );
        INT_BuildAll();
        return layoutRoot;
    }

    override void OnShow()
    {
        super.OnShow();
        // shown again after a close (a menu opened on top hid us): finish the close instead of a dead window
        if (m_INT_TornDown)
            INT_VPP_CloseSelf();
    }

    override void OnHide()
    {
        super.OnHide();
        // closed by anyone (our M / Esc, HideScriptedMenu, Close(), CloseAll): everything of the window goes
        INT_VPP_Teardown();
        // HideScriptedMenu only HIDES: the engine keeps the menu object for a later ShowScriptedMenu (VPP's old
        // keep-one-instance design). Measured 2026-11-12: every open left one hidden menu alive. Next frame we
        // close ourselves for real, unless a child menu on top is what hid us (then OnShow closes us later).
        if (!m_INT_CloseIssued && GetGame())
            INT_VPP_QueueAfterHide(0);
    }

    protected void INT_VPP_QueueAfterHide(int delayMs)
    {
        if (m_INT_AfterHideQueued || !GetGame())
            return;
        m_INT_AfterHideQueued = true;
        GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(INT_VPP_AfterHide, delayMs, false);
    }

    void INT_VPP_AfterHide()
    {
        m_INT_AfterHideQueued = false;
        if (m_INT_CloseIssued)
            return;
        if (IsVisible())
            return;
        if (GetSubMenu())
            return;
        // review #1: another menu is current (UIF CloseMenu + EnterScriptedMenu, the pause menu after CloseAll):
        // never Close() while it is up - look again later; our trees are already gone
        UIManager um = null;
        if (GetGame())
            um = GetGame().GetUIManager();
        if (um)
        {
            UIScriptedMenu top = um.GetMenu();
            if (top && top != this)
            {
                INT_VPP_QueueAfterHide(250);
                return;
            }
        }
        INT_VPP_CloseSelf();
    }

    // the only place that calls the native Close() (it destroys the menu; once only)
    void INT_VPP_CloseSelf()
    {
        if (m_INT_CloseIssued)
            return;
        m_INT_CloseIssued = true;
        Close();
    }

    // ===================================================================== kept VPP API
    bool IsMenuOpen() {
        return m_IsMenuOpen;
    }

    void SetMenuOpen(bool isMenuOpen) {
        m_IsMenuOpen = isMenuOpen;
    }

    void ReloadMarkers() {
        ClientMarkersCache cache = g_Game.GetClientMarkersCache();
        if (cache) {
            //INT (review #8): the saved json once per session (raw edits made between sessions); in-session saves
            //are already in the same in-memory cache
            g_Game.INT_VPP_LoadCacheOnce();
            m_ClientMarkers = cache.GetCachedArray();
            m_CustomServerMarkers = cache.GetCachedArray(m_AddressPort);
            DisplayClientMarkers();
            DisplayServerMarkers();
        }
    }

    /*
    	Personal markers + "my server markers" (MapMarkersCache): 2D overlay + client 3D markers, VPP order.
    */
    void DisplayClientMarkers() {
        ClientMarkersCache cache = g_Game.GetClientMarkersCache();
        if (cache) {
            m_ClientMarkers = cache.GetCachedArray();
            m_CustomServerMarkers = cache.GetCachedArray(m_AddressPort);
        }
        DisplayPlayerPosition();
        //INT (review #6): client 3D markers reused / updated in place per MarkerInfo (was: delete all + rebuild)
        array<ref MarkerInfo> perServer = null;
        if (m_AddressPort != "")
            perServer = m_CustomServerMarkers;
        g_Game.INT_VPP_SyncClient3d(m_ClientMarkers, perServer);
        INT_Refresh3D();
        INT_RebuildItems();
    }

    void DisplayServerMarkers() {
        m_ServerMarkers = g_Game.GetServerMarkers();
        int srvCount = -1;
        if (m_ServerMarkers)
            srvCount = m_ServerMarkers.Count();
        m_INT_SrvCount = srvCount;
        //INT (review #6): server 3D markers reused / updated in place per MarkerInfo (own → «Сервер», RF → «События»)
        if (m_ServerMarkers != NULL)
            g_Game.INT_VPP_RebuildServer3d();
        m_INT_SeenRev3D = g_Game.INT_VPP_3DRevision();
        INT_Refresh3D();
        INT_RebuildItems();
    }

    // Own position overlay. VPPMapItemRequirements sets OwnPositionMarkerDisabled first, then calls this: the flag
    // is read here, after that.
    void DisplayPlayerPosition() {
        if (!GetGame())
            return;
        Man player = GetGame().GetPlayer();
        if (!player)
            return;
        bool shown = true;
        if (g_Game.OwnPositionMarkerDisabled())
            shown = false;
        vector pos = player.GetPosition();
        float heading = INT_Heading(player);
        m_INT_LastMePos = pos;
        m_INT_LastHeading = heading;
        m_INT_MeShown = shown;
        if (m_INT_View) {
            m_INT_View.INT_SetMe(shown, pos, heading);
            m_INT_View.INT_UpdateDistances(pos);
        }
        if (m_INT_Isl) {
            string co = INT_VPP_Fmt.INT_Coords(pos);
            string sq = INT_VPP_Fmt.INT_Square(pos);
            m_INT_Isl.INT_SetMeCoords(shown, co, sq);
        }
        if (m_INT_Ribbon)
            m_INT_Ribbon.INT_UpdateMe(pos, shown);
        m_INT_DirtyList = true;
    }

    //Returns world coords of where player clicks on map
    vector GetMapClickPos() {
        int mx;
        int my;
        GetMousePos(mx, my);
        if (m_INT_View)
            return m_INT_View.INT_ScreenToWorld(mx, my);
        if (!m_MapWidget)
            return vector.Zero;
        vector sp = Vector(mx, my, 0);
        vector w = m_MapWidget.ScreenToMap(sp);
        w[1] = 0;
        return w;
    }

    // 2xLMB on the map: the editor card for a NEW marker at that point (VPPMapItemRequirements: pen gate -> super)
    void MapDoubleClick(Widget w, int x, int y, int button) {
        if (button != MouseState.LEFT)
            return;
        if (!m_INT_Built || m_INT_TornDown)
            return;
        if (m_INT_RulerOn)
            return;
        if (m_INT_ZoneEd && m_INT_ZoneEd.INT_IsEditing())
            return;
        if (m_INT_LabelEd && m_INT_LabelEd.INT_IsEditing())
            return;
        if (m_INT_Confirm && m_INT_Confirm.IsOpen())
            return;
        vector pos = GetMapClickPos();
        INT_OpenEditorNew(pos);
    }

    /*
    	Remove marker by using exact position
    */
    void RemoveMarkerExact(vector pos, bool isCustomServer) {
        string addressPort = "";
        if (isCustomServer) {
            addressPort = m_AddressPort;
        }
        g_Game.GetClientMarkersCache().RemoveCachedMarker(pos, addressPort);
        g_Game.GetClientMarkersCache().SaveCache(); //Save directly to json
        //Redraw markers
        ReloadMarkers();
    }

    /*
    	Remove marker by using relative position
    */
    void RemoveMarker(vector relativePos) {
        float minDistance = 86;
        float distance;
        MarkerInfo nearestMarker;
        bool isCustomServerMarker = false;
        foreach (MarkerInfo clientMarker : m_ClientMarkers) {
            distance = vector.Distance(relativePos, clientMarker.GetPosition());
            if (distance < minDistance) {
                minDistance = distance;
                nearestMarker = clientMarker;
            }
        }
        foreach (MarkerInfo marker : m_CustomServerMarkers) {
            distance = vector.Distance(relativePos, marker.GetPosition());
            if (distance < minDistance) {
                minDistance = distance;
                nearestMarker = marker;
                isCustomServerMarker = true;
            }
        }
        if (nearestMarker) {
            RemoveMarkerExact(nearestMarker.GetPosition(), isCustomServerMarker);
        }
    }

    static int VectorToARGB(vector color) {
        float R;
        float G;
        float B;
        R = color[0];
        G = color[1];
        B = color[2];
        return ARGB(255,R,G,B);
    }

    void EditMarkerVisibility(array<int> indexes, bool show2D, bool show3D, bool isClientMarker, bool isCustomServerMarker) {
        array<ref MarkerInfo> markers;
        foreach (int index : indexes) {
            if (isClientMarker) {
                markers = m_ClientMarkers;
            } else if (isCustomServerMarker) {
                markers = m_CustomServerMarkers;
            } else {
                markers = m_ServerMarkers;
            }
            MarkerInfo marker = markers.Get(index);
            marker.SetActive(show2D);
            marker.Set3DActive(show3D);
        }

        if (isClientMarker || isCustomServerMarker) {
            string addressPort = "";
            if (isCustomServerMarker) {
                addressPort = m_AddressPort;
            }
            g_Game.GetClientMarkersCache().UpdateCachedMarkerInfos(markers, addressPort);
            g_Game.GetClientMarkersCache().SaveCache(); //Save directly to json
        }
        ReloadMarkers(); //Reload all markers
    }

    void EditMarkerAttributes(int index, string name, string markerSuffix, string iconPath, vector color, bool wasCustomServer, bool isCustomServer) {
        array<ref MarkerInfo> markers;
        if (wasCustomServer) {
            markers = m_CustomServerMarkers;
        } else {
            markers = m_ClientMarkers;
        }
        m_LastEditedMarker = markers.Get(index);
        m_LastEditedMarker.SetName(name);
        m_LastEditedMarker.SetIconPath(iconPath);
        m_LastEditedMarker.SetColor(color);
        string markerName = name;
        if (markerSuffix) {
            markerName.Replace(markerSuffix, "");
        }
        if (!m_LastEditedMarkerName || m_LastEditedMarkerName != markerName) {
            m_MarkerSuffix = 1;
            m_LastEditedMarkerName = markerName;
        }

        if (wasCustomServer && !isCustomServer) {
            g_Game.GetClientMarkersCache().RemoveCachedMarker(m_LastEditedMarker.GetPosition(), m_AddressPort);
            m_ClientMarkers.Insert(m_LastEditedMarker);
            g_Game.GetClientMarkersCache().UpdateCachedMarkerInfos(m_ClientMarkers);
        } else if (!wasCustomServer && isCustomServer) {
            g_Game.GetClientMarkersCache().RemoveCachedMarker(m_LastEditedMarker.GetPosition());
            m_CustomServerMarkers.Insert(m_LastEditedMarker);
            g_Game.GetClientMarkersCache().UpdateCachedMarkerInfos(m_CustomServerMarkers, m_AddressPort);
        } else if (isCustomServer && wasCustomServer) {
            g_Game.GetClientMarkersCache().UpdateCachedMarkerInfos(m_CustomServerMarkers, m_AddressPort);
        } else if (!isCustomServer && !wasCustomServer) {
            g_Game.GetClientMarkersCache().UpdateCachedMarkerInfos(m_ClientMarkers);
        }
        g_Game.GetClientMarkersCache().SaveCache(); //Save directly to json
        ReloadMarkers(); //Reload all markers
    }

    // old VPP entry: now opens our editor card for that marker
    void ShowDialog(int markerIndex, bool isCustomServer, bool isNewMarker, string markerSuffix = "") {
        array<ref MarkerInfo> markers = m_ClientMarkers;
        if (isCustomServer)
            markers = m_CustomServerMarkers;
        if (!markers || markerIndex < 0 || markerIndex >= markers.Count())
            return;
        MarkerInfo info = markers.Get(markerIndex);
        INT_VPP_MapItem it = INT_FindItem(info);
        if (it)
            INT_OpenEditorFor(it);
    }

    void HideDialog() {
        INT_CancelEditor();
    }

    // ===================================================================== INT: lifecycle
    protected void INT_BuildAll()
    {
        if (m_INT_Built)
            return;
        m_INT_F = INT_UIF_Scale.Factor();
        if (m_INT_F <= 0)
            m_INT_F = 1;
        int sw = INT_UIF_Scale.ScreenW();
        int sh = INT_UIF_Scale.ScreenH();
        m_INT_ScrW = sw / m_INT_F;
        m_INT_ScrH = sh / m_INT_F;
        if (m_INT_ScrW < 1920)
            m_INT_ScrW = 1920;
        if (m_INT_ScrH < 1080)
            m_INT_ScrH = 1080;
        m_INT_Screen = INT_UIF_Draw.Frame(layoutRoot, 0, 0, m_INT_ScrW, m_INT_ScrH, "IntVppScreen");
        if (!m_INT_Screen)
            return;
        m_INT_View = new INT_VPP_MapView();
        m_INT_View.INT_Bind(m_MapWidget, layoutRoot);
        INT_VPP_Client cl = INT_VPP_Client.Get();
        m_INT_Admin = cl.IsAdmin();
        cl.OnZones.Insert(INT_OnZonesEvt);
        cl.OnSettings.Insert(INT_OnSettingsEvt);
        cl.m_INT_OnLabels.Insert(INT_OnLabelsEvt);
        // review #10: a player made map admin (or removed) mid-session learns it on the next open (≤ 1 per 30 s)
        cl.RequestRefresh();
        INT_VPP_Route route = INT_VPP_Route.INT_Get();
        if (route && route.m_INT_OnChanged)
            route.m_INT_OnChanged.Insert(INT_OnRouteEvt);
        INT_VPP_Prefs prefs = INT_VPP_Prefs.INT_Get();
        if (prefs && prefs.m_INT_OnChanged)
            prefs.m_INT_OnChanged.Insert(INT_OnPrefsEvt);
        m_INT_Isl = new INT_VPP_Islands();
        m_INT_Isl.INT_Build(m_INT_Screen, m_INT_ScrW, m_INT_ScrH, this, m_INT_Admin);
        float hx = (m_INT_ScrW - 1920) * 0.5;
        float hy = (m_INT_ScrH - 1080) * 0.5;
        m_INT_ConfirmHost = INT_UIF_Draw.Frame(m_INT_Screen, hx, hy, 1920, 1080, "IntVppConfirmHost");
        INT_VPP_Ui.INT_Sort(m_INT_Screen);
        if (s_INT_LastIcon == "")
            s_INT_LastIcon = INT_VPP_Icons.INT_DefaultPath();
        m_INT_Built = true;
        m_INT_Tab = INT_TAB_MAP;
        ReloadMarkers();
        m_INT_DirtyIslands = true;
        INT_ApplyMode();
    }

    // Unlinks every tree the window made (script trees; layoutRoot: INT_VPP_ReleaseRoot). Runs once.
    void INT_VPP_Teardown()
    {
        if (m_INT_TornDown)
            return;
        m_INT_TornDown = true;
        m_IsMenuOpen = false;
        if (!GetGame())
            return;
        g_Game.SetKeyboardBusy(false);
        if (m_MapWidget && m_INT_Built)
        {
            s_INT_LastPos = m_MapWidget.GetMapPos();
            s_INT_LastScale = m_MapWidget.GetScale();
            s_INT_HasLast = true;
        }
        // subscriptions (the singletons may already be shut down: Peek)
        INT_VPP_Client cl = INT_VPP_Client.Peek();
        if (cl)
        {
            if (cl.OnZones)
                cl.OnZones.Remove(INT_OnZonesEvt);
            if (cl.OnSettings)
                cl.OnSettings.Remove(INT_OnSettingsEvt);
            if (cl.m_INT_OnLabels)
                cl.m_INT_OnLabels.Remove(INT_OnLabelsEvt);
        }
        INT_VPP_Route route = INT_VPP_Route.INT_Peek();
        if (route && route.m_INT_OnChanged)
            route.m_INT_OnChanged.Remove(INT_OnRouteEvt);
        INT_VPP_Prefs prefs = INT_VPP_Prefs.INT_Peek();
        if (prefs && prefs.m_INT_OnChanged)
            prefs.m_INT_OnChanged.Remove(INT_OnPrefsEvt);
        // the confirm band: its closed band is unlinked by its Tick / destructor -> before the host goes
        if (m_INT_Confirm)
        {
            m_INT_Confirm.OnResult().Clear();
            m_INT_Confirm.OnPreview().Clear();
            if (m_INT_Confirm.IsOpen())
                m_INT_Confirm.Cancel();
            m_INT_Confirm.Tick(0);
            m_INT_Confirm = null;
        }
        // cards / editors (their roots go to the graveyard), islands, then the graveyard itself
        if (m_INT_Card)
            m_INT_Card.INT_Close();
        m_INT_Card = null;
        if (m_INT_Editor)
            m_INT_Editor.INT_Close();
        m_INT_Editor = null;
        if (m_INT_Ribbon)
            m_INT_Ribbon.INT_Close();
        m_INT_Ribbon = null;
        if (m_INT_RulerCard)
            m_INT_RulerCard.INT_Close();
        m_INT_RulerCard = null;
        if (m_INT_ZoneEd)
            m_INT_ZoneEd.INT_Destroy();
        m_INT_ZoneEd = null;
        if (m_INT_LabelEd)
            m_INT_LabelEd.INT_Destroy();
        m_INT_LabelEd = null;
        if (m_INT_Isl)
            m_INT_Isl.INT_Destroy();
        m_INT_Isl = null;
        INT_FlushGraveyard();
        // map-side trees (inside Map_Widget)
        if (m_INT_View)
            m_INT_View.INT_Destroy();
        m_INT_View = null;
        if (m_MapWidget && WidgetEventHandler.GetInstance())
            WidgetEventHandler.GetInstance().UnregisterWidget(m_MapWidget);
        if (m_INT_Screen)
            m_INT_Screen.Unlink();
        m_INT_Screen = null;
        m_INT_ConfirmHost = null;
        INT_VPPX_TextFit.Clear();
        INT_VPPX_Look.ForgetDeadTwins();
        INT_VPPX_Fx.ForgetDeadHomes();
        m_INT_Items.Clear();
        m_INT_RulerPts.Clear();
        m_INT_CardInfo = null;
        m_INT_EditInfo = null;
        m_INT_ConfirmInfo = null;
        m_INT_RulerOn = false;
    }

    // After the menu was hidden: the layout itself.
    void INT_VPP_ReleaseRoot()
    {
        m_MapWidget = null;
        if (layoutRoot)
        {
            layoutRoot.Unlink();
            layoutRoot = null;
        }
        if (m_INT_StatRoot)
        {
            m_INT_StatRoot = false;
            INT_VPP_Stat.INT_Down(0);
        }
    }

    // false once the window tore itself down (closed by anyone)
    bool INT_IsAlive()
    {
        if (m_INT_TornDown)
            return false;
        return true;
    }

    void INT_Bury(Widget w)
    {
        if (!w)
            return;
        w.Show(false);
        m_INT_Graveyard.Insert(w);
    }

    protected void INT_FlushGraveyard()
    {
        int n = m_INT_Graveyard.Count();
        if (n == 0)
            return;
        for (int i = 0; i < n; i++)
        {
            Widget w = m_INT_Graveyard.Get(i);
            if (w)
                w.Unlink();
        }
        m_INT_Graveyard.Clear();
    }

    // ===================================================================== INT: per frame
    override void Update(float timeslice) {
        super.Update(timeslice);
        if (!m_INT_Built || m_INT_TornDown)
            return;
        INT_FlushGraveyard();
        if (m_INT_Confirm)
            m_INT_Confirm.Tick(timeslice);
        INT_PollSrvMarkerResult();
        if (m_INT_Isl)
        {
            m_INT_Isl.INT_Tick(timeslice);
            // a panel was dragged / put back (2026-11-14): the pins of the cards follow
            if (m_INT_Isl.INT_TakePanelMoved())
                m_INT_CardsDirty = true;
        }
        INT_KeepConfirmTop();
        if (m_INT_ZoneEd)
            m_INT_ZoneEd.INT_Tick(timeslice);
        if (m_INT_LabelEd)
            m_INT_LabelEd.INT_Tick(timeslice);
        m_INT_Frames++;
        if (m_INT_Frames == 2)
        {
            INT_FirstView();
            // tab switches animate from now on (2026-11-14); the tab the map opened with was shown at once
            m_INT_TabFxOn = true;
            if (m_INT_Isl)
                m_INT_Isl.INT_SetTabAnim(true);
        }
        bool typing = INT_IsTyping();
        g_Game.SetKeyboardBusy(typing);
        INT_RawEsc(typing);
        int mx;
        int my;
        GetMousePos(mx, my);
        bool overMap = INT_IsOverMap();
        INT_Mouse(mx, my, overMap);
        if (!typing)
            INT_Keys();
        INT_ProcessDirty();
        INT_TickMe(timeslice);
        INT_ViewGuard(timeslice);
        if (m_INT_View)
        {
            m_INT_View.INT_SetCursor(mx, my, overMap);
            m_INT_View.INT_Tick(timeslice);
        }
        INT_PlaceCards();
        INT_CursorText(mx, my, overMap);
    }

    protected bool INT_IsTyping()
    {
        Widget f = GetFocus();
        if (!f)
            return false;
        EditBoxWidget eb = EditBoxWidget.Cast(f);
        if (eb)
            return true;
        return false;
    }

    protected void INT_BlurAll()
    {
        if (m_INT_Editor)
            m_INT_Editor.INT_Blur();
        if (m_INT_ZoneEd)
            m_INT_ZoneEd.INT_Blur();
        if (m_INT_LabelEd)
            m_INT_LabelEd.INT_Blur();
        if (INT_IsTyping())
            SetFocus(null);
        g_Game.SetKeyboardBusy(false);
    }

    // a focused edit box does not always pass UAUIBack on: the raw key leaves the field (first Esc)
    protected void INT_RawEsc(bool typing)
    {
        bool esc = false;
        if (KeyState(KeyCode.KC_ESCAPE) != 0)
            esc = true;
        if (esc && !m_INT_EscWas && typing)
        {
            INT_BlurAll();
            m_INT_LastBackMs = GetGame().GetTime();
        }
        m_INT_EscWas = esc;
    }

    // UAUIBack (MissionGameplay): popover / confirm -> editor / zone editor / ruler -> card -> false (= close the map)
    bool INT_VPP_HandleBack()
    {
        if (!m_INT_Built || m_INT_TornDown)
            return false;
        int now = GetGame().GetTime();
        if (now - m_INT_LastBackMs < INT_BACK_GAP_MS)
            return true;
        m_INT_LastBackMs = now;
        if (INT_IsTyping())
        {
            INT_BlurAll();
            return true;
        }
        if (m_INT_Confirm && m_INT_Confirm.IsOpen())
        {
            m_INT_Confirm.Cancel();
            return true;
        }
        if (m_INT_Isl && m_INT_Isl.INT_IsPopOpen())
        {
            INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CLOSE);
            m_INT_Isl.INT_ClosePop();
            return true;
        }
        if (m_INT_Editor)
        {
            INT_CancelEditor();
            return true;
        }
        if (m_INT_ZoneEd)
        {
            INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CLOSE);
            if (!m_INT_ZoneEd.INT_Back())
                INT_CloseZoneEditor();
            return true;
        }
        if (m_INT_LabelEd)
        {
            INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CLOSE);
            if (!m_INT_LabelEd.INT_Back())
                INT_CloseLabelEditor();
            return true;
        }
        if (m_INT_RulerOn)
        {
            INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CLOSE);
            INT_SetRulerOn(false);
            return true;
        }
        if (m_INT_Card)
        {
            INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CLOSE);
            INT_CloseCard();
            return true;
        }
        if (m_INT_Isl && m_INT_Isl.INT_IsListOpen())
        {
            INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CLOSE);
            INT_SelectTab(INT_TAB_MAP);
            return true;
        }
        return false;
    }

    protected bool INT_IsOverMap()
    {
        if (!m_MapWidget)
            return false;
        Widget w = GetWidgetUnderCursor();
        while (w)
        {
            if (w == m_MapWidget)
                return true;
            w = w.GetParent();
        }
        return false;
    }

    protected void INT_Mouse(int mx, int my, bool overMap)
    {
        int lmbState = GetMouseState(MouseState.LEFT) & MB_PRESSED_MASK;
        int rmbState = GetMouseState(MouseState.RIGHT) & MB_PRESSED_MASK;
        bool lmb = false;
        if (lmbState != 0)
            lmb = true;
        bool rmb = false;
        if (rmbState != 0)
            rmb = true;
        // left: press / release edges (a click = release over the map within a few px of the press)
        if (lmb && !m_INT_LmbWas)
        {
            m_INT_PressX = mx;
            m_INT_PressY = my;
            m_INT_PressOnMap = overMap;
            m_INT_PressOnUi = false;
            if (overMap)
            {
                m_INT_Guard = 0;
            }
            else
            {
                m_INT_PressOnUi = true;
                INT_RememberView();
            }
        }
        if (!lmb && m_INT_LmbWas)
        {
            if (m_INT_PressOnMap && overMap)
            {
                int dx = mx - m_INT_PressX;
                int dy = my - m_INT_PressY;
                float lim = INT_CLICK_PX * m_INT_F;
                if (dx * dx + dy * dy <= lim * lim)
                    INT_OnMapLeftClick(mx, my);
            }
            if (m_INT_PressOnUi)
                m_INT_Guard = 0.4;
            m_INT_PressOnUi = false;
            m_INT_PressOnMap = false;
        }
        m_INT_LmbWas = lmb;
        // right
        if (rmb && !m_INT_RmbWas)
        {
            m_INT_RPressX = mx;
            m_INT_RPressY = my;
            m_INT_RPressOnMap = overMap;
        }
        if (!rmb && m_INT_RmbWas)
        {
            if (m_INT_RPressOnMap && overMap)
            {
                int rdx = mx - m_INT_RPressX;
                int rdy = my - m_INT_RPressY;
                float rlim = 8 * m_INT_F;
                if (rdx * rdx + rdy * rdy <= rlim * rlim)
                    INT_OnMapRightClick(mx, my);
            }
            m_INT_RPressOnMap = false;
        }
        m_INT_RmbWas = rmb;
        // hover: only when the cursor moved
        if (mx != m_INT_LastMouseX || my != m_INT_LastMouseY)
        {
            m_INT_LastMouseX = mx;
            m_INT_LastMouseY = my;
            INT_VPP_MapItem hov = null;
            if (overMap && m_INT_View)
                hov = m_INT_View.INT_HitTest(mx, my);
            if (m_INT_View)
                m_INT_View.INT_SetHover(hov);
            // a new marker under the cursor: a very quiet hover tick (once per marker, not per pixel)
            if (hov && hov != m_INT_HovLast)
                INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.HOVER);
            m_INT_HovLast = hov;
        }
    }

    protected void INT_OnMapLeftClick(int mx, int my)
    {
        if (!m_INT_View)
            return;
        vector w = m_INT_View.INT_ScreenToWorld(mx, my);
        if (m_INT_Isl && m_INT_Isl.INT_IsPopOpen())
            m_INT_Isl.INT_ClosePop();
        if (m_INT_ZoneEd && m_INT_ZoneEd.INT_IsDrawing())
        {
            if (m_INT_ZoneEd.INT_OnMapClick(w, mx, my))
                return;
        }
        // label editor: the place of the edited label, or (list mode) a label of the map to edit
        if (m_INT_LabelEd)
        {
            if (m_INT_LabelEd.INT_OnMapClick(w, mx, my))
                return;
        }
        if (m_INT_RulerOn)
        {
            INT_RulerAdd(w);
            return;
        }
        // a server marker under edit moves to the clicked point (2026-11-14, like the labels)
        if (m_INT_Editor && m_INT_Editor.INT_IsServer())
        {
            m_INT_EditorPos = w;
            m_INT_Editor.INT_SetPos(w);
            if (m_INT_View)
            {
                string gic = m_INT_Editor.INT_GetIconPath();
                int gcol = m_INT_Editor.INT_GetColor();
                m_INT_View.INT_SetGhost(true, w, gic, gcol);
            }
            m_INT_CardsDirty = true;
            INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.TICK);
            return;
        }
        if (m_INT_Editor)
            return;
        INT_VPP_MapItem it = m_INT_View.INT_HitTest(mx, my);
        if (it)
        {
            INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.SELECT);
            INT_OpenCard(it);
        }
        else
        {
            if (m_INT_Card)
                INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CLOSE);
            INT_CloseCard();
        }
    }

    protected void INT_OnMapRightClick(int mx, int my)
    {
        if (!m_INT_View)
            return;
        if (m_INT_ZoneEd && m_INT_ZoneEd.INT_IsEditing())
        {
            m_INT_ZoneEd.INT_OnMapRmb();
            return;
        }
        if (m_INT_LabelEd && m_INT_LabelEd.INT_OnMapRmb())
            return;
        if (m_INT_RulerOn)
        {
            int rn = m_INT_RulerPts.Count();
            if (rn > 0)
            {
                INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.TICK);
                m_INT_RulerPts.Remove(rn - 1);
                INT_RulerChanged();
            }
            return;
        }
        if (m_INT_Editor || (m_INT_Confirm && m_INT_Confirm.IsOpen()))
            return;
        vector w = m_INT_View.INT_ScreenToWorld(mx, my);
        INT_AddRoutePoint(w, "");
    }

    protected void INT_Keys()
    {
        bool space = false;
        if (KeyState(KeyCode.KC_SPACE) != 0)
            space = true;
        bool rk = false;
        if (KeyState(KeyCode.KC_R) != 0)
            rk = true;
        bool enter = false;
        if (KeyState(KeyCode.KC_RETURN) != 0 || KeyState(KeyCode.KC_NUMPADENTER) != 0)
            enter = true;
        if (space && !m_INT_SpaceWas)
            INT_CenterOnMe();
        if (rk && !m_INT_RWas)
        {
            bool next = true;
            if (m_INT_RulerOn)
                next = false;
            INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.TOGGLE);
            INT_SetRulerOn(next);
        }
        if (enter && !m_INT_EnterWas)
        {
            if (m_INT_Confirm && m_INT_Confirm.IsOpen())
                m_INT_Confirm.Confirm();
            else if (m_INT_Editor)
                INT_EditorSave();
            else if (m_INT_LabelEd && m_INT_LabelEd.INT_IsEditing())
                m_INT_LabelEd.INT_SaveFromKey();
        }
        m_INT_SpaceWas = space;
        m_INT_RWas = rk;
        m_INT_EnterWas = enter;
        // Ctrl + C (2026-11-14): once per press, never while a text field has the keyboard (INT_Keys runs only then)
        bool ctrl = false;
        if (KeyState(KeyCode.KC_LCONTROL) != 0 || KeyState(KeyCode.KC_RCONTROL) != 0)
            ctrl = true;
        bool ck = false;
        if (KeyState(KeyCode.KC_C) != 0)
            ck = true;
        bool copy = false;
        if (ctrl && ck)
            copy = true;
        if (copy && !m_INT_CopyWas)
            INT_CopyCursor();
        m_INT_CopyWas = copy;
    }

    // Ctrl + C on the map (2026-11-14): the world point under the cursor to the clipboard as «X: 13239 Y: 5931»
    // (integer metres, X = world X, Y = world Z: the numbers of «КУРСОР» without the thousands space). Over a panel
    // (not the map) nothing happens: the cursor line shows the last map point, but a copy needs the cursor on the map.
    protected bool m_INT_CopyWas;

    string INT_CopyText(vector w)
    {
        int x = Math.Round(w[0]);
        int z = Math.Round(w[2]);
        return "X: " + x.ToString() + " Y: " + z.ToString();
    }

    protected void INT_CopyCursor()
    {
        if (!m_INT_View || !INT_IsOverMap())
            return;
        int mx;
        int my;
        GetMousePos(mx, my);
        vector w = m_INT_View.INT_ScreenToWorld(mx, my);
        string s = INT_CopyText(w);
        GetGame().CopyToClipboard(s);
        INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CLICK);
        if (m_INT_Isl)
            m_INT_Isl.INT_FlashCopied(s);
        s_INT_LastCopied = s;
    }

    // TEST: the last string Ctrl + C put on the clipboard
    static string s_INT_LastCopied;

    protected void INT_TickMe(float dt)
    {
        m_INT_MeTick = m_INT_MeTick + dt;
        if (m_INT_MeTick < INT_ME_TICK)
            return;
        m_INT_MeTick = 0;
        Man player = GetGame().GetPlayer();
        if (player)
        {
            vector pos = player.GetPosition();
            float moved = INT_VPP_Fmt.INT_Dist2D(pos, m_INT_LastMePos);
            if (moved > 2)
            {
                DisplayPlayerPosition();
            }
            else
            {
                float h = INT_Heading(player);
                float dh = Math.AbsFloat(h - m_INT_LastHeading);
                if (dh > 2 && m_INT_View)
                {
                    m_INT_LastHeading = h;
                    m_INT_View.INT_SetHeading(h);
                }
            }
        }
        bool hidden = g_Game.INT_VPP_Is3DTempHidden();
        if (hidden != m_INT_Temp3D)
        {
            m_INT_Temp3D = hidden;
            m_INT_DirtyIslands = true;
        }
        // RF bridge / other mods insert into GetServerMarkers() directly (no SetServerMarkers): follow the list
        array<ref MarkerInfo> srv = g_Game.GetServerMarkers();
        int srvCount = -1;
        if (srv)
            srvCount = srv.Count();
        bool srvChanged = false;
        if (srvCount != m_INT_SrvCount)
            srvChanged = true;
        if (srv != m_ServerMarkers)
            srvChanged = true;
        // review #5: an RF marker moved / renamed IN PLACE changes neither the count nor the array: the 3D manager's
        // signature check (once a second) rebuilds the server 3D markers and bumps the revision - follow it
        int rev = g_Game.INT_VPP_3DRevision();
        if (rev != m_INT_SeenRev3D)
            srvChanged = true;
        if (srvChanged)
        {
            m_INT_SrvCount = srvCount;
            DisplayServerMarkers();
        }
    }

    protected float INT_Heading(Man player)
    {
        if (!player)
            return 0;
        vector ypr = player.GetYawPitchRoll();
        float angle = Math.Round(ypr[0]);
        if (angle < 0)
            angle = angle + 360;
        return angle;
    }

    protected void INT_CursorText(int mx, int my, bool overMap)
    {
        if (!m_INT_Isl || !m_INT_View || !overMap)
            return;
        vector w = m_INT_View.INT_ScreenToWorld(mx, my);
        int wx = Math.Round(w[0]);
        int wz = Math.Round(w[2]);
        if (wx == m_INT_CurWX && wz == m_INT_CurWZ)
            return;
        m_INT_CurWX = wx;
        m_INT_CurWZ = wz;
        string t = INT_VPP_Fmt.INT_Coords(w);
        if (m_INT_MeShown)
        {
            float d = INT_VPP_Fmt.INT_Dist2D(m_INT_LastMePos, w);
            string ds = INT_VPP_Fmt.INT_Dist(d);
            t = t + "   ·   " + ds + " " + INT_VPP_Islands.TXT_FROM_YOU;
        }
        m_INT_Isl.INT_SetCursorText(t);
    }

    // ===================================================================== INT: view (kept in script)
    protected void INT_RememberView()
    {
        if (!m_MapWidget)
            return;
        m_INT_ViewPos = m_MapWidget.GetMapPos();
        m_INT_ViewScale = m_MapWidget.GetScale();
        m_INT_HaveView = true;
    }

    protected void INT_MapTo(vector pos)
    {
        if (!m_MapWidget)
            return;
        m_MapWidget.SetMapPos(pos);
        INT_RememberView();
        m_INT_Guard = 0.5;
    }

    protected void INT_MapScale(float s)
    {
        if (!m_MapWidget)
            return;
        m_MapWidget.SetScale(s);
        INT_RememberView();
        m_INT_Guard = 0.5;
    }

    // after a press on our islands (and right after our own view changes) the widget's drift is set back
    protected void INT_ViewGuard(float dt)
    {
        if (!m_MapWidget || !m_INT_HaveView)
            return;
        bool hold = false;
        if (m_INT_LmbWas && m_INT_PressOnUi)
            hold = true;
        if (!hold && m_INT_Guard <= 0)
            return;
        if (m_INT_Guard > 0)
            m_INT_Guard = m_INT_Guard - dt;
        vector pos = m_MapWidget.GetMapPos();
        float sc = m_MapWidget.GetScale();
        bool moved = false;
        if (Math.AbsFloat(pos[0] - m_INT_ViewPos[0]) > 0.01 || Math.AbsFloat(pos[2] - m_INT_ViewPos[2]) > 0.01)
            moved = true;
        bool zoomed = false;
        if (Math.AbsFloat(sc - m_INT_ViewScale) > m_INT_ViewScale * 0.0001)
            zoomed = true;
        if (!moved && !zoomed)
            return;
        if (zoomed)
            m_MapWidget.SetScale(m_INT_ViewScale);
        m_MapWidget.SetMapPos(m_INT_ViewPos);
        s_INT_Drifts++;
    }

    protected void INT_FirstView()
    {
        if (!m_MapWidget)
            return;
        if (s_INT_HasLast)
        {
            INT_MapScale(s_INT_LastScale);
            INT_MapTo(s_INT_LastPos);
            return;
        }
        if (m_INT_MeShown)
        {
            INT_MapScale(0.3);
            INT_MapTo(m_INT_LastMePos);
        }
    }

    void INT_CenterOn(vector pos)
    {
        INT_MapTo(pos);
    }

    // TEST: a view (centre + scale) set from code
    void INT_SetView(vector pos, float scale)
    {
        INT_MapScale(scale);
        INT_MapTo(pos);
    }

    void INT_CenterOnMe()
    {
        if (!m_INT_MeShown)
        {
            INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.DENY);
            return;
        }
        Man player = GetGame().GetPlayer();
        if (!player)
            return;
        vector pos = player.GetPosition();
        INT_MapTo(pos);
        INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.TICK);
    }

    // ===================================================================== INT: markers -> items
    protected void INT_Refresh3D()
    {
        INT_VPP_3D m = INT_VPP_3D.INT_Peek();
        if (m)
            m.INT_Refresh();
    }

    protected INT_VPP_MapItem INT_MakeItem(MarkerInfo m, int kind, int layer)
    {
        INT_VPP_MapItem it = new INT_VPP_MapItem();
        it.m_INT_Kind = kind;
        it.m_INT_Layer = layer;
        it.m_INT_Info = m;
        it.m_INT_Name = m.GetName();
        it.m_INT_Pos = m.GetPosition();
        vector c = m.GetColor();
        it.m_INT_Color = INT_VPP_Fmt.INT_VecToArgb(c);
        string rawPath = m.GetIconPath();
        //INT (2026-11-13): empty or not loadable (RF configs naming textures no mod has) -> our icon by name / words;
        //a path that loads is drawn as it is (INT_VPP_IconFallback)
        string nmIcon = m.GetName();
        string path = INT_VPP_IconFallback.INT_Resolve(rawPath, nmIcon);
        it.m_INT_IconPath = path;
        bool own = INT_VPP_Icons.INT_IsOwnPath(path);
        it.m_INT_OwnIcon = own;
        bool act = m.IsActive();
        it.m_INT_Visible = act;
        bool act3 = m.Is3DActive();
        it.m_INT_3D = act3;
        INT_ReadItemMode(it);
        return it;
    }

    // ---- map mode per marker (chips of «МЕТКИ НА КАРТЕ», 2026-11-12) ----
    // «Мои»: the marker's own flags (= the editor's 2D / 2D+3D / hidden), 3D capped by the server's global switch;
    // «Сервер» / «События»: DayZGame.INT_VPP_SrvMode (the player's override in Client.json, capped by the server)
    protected void INT_ReadItemMode(INT_VPP_MapItem it)
    {
        MarkerInfo m = it.m_INT_Info;
        if (!m)
            return;
        if (it.INT_IsMine())
        {
            bool can3m = g_Game.CanUse3DMarkers();
            int mm = INT_VPP_MODE_HIDDEN;
            if (m.IsActive())
            {
                mm = INT_VPP_MODE_2D;
                if (m.Is3DActive())
                    mm = INT_VPP_MODE_3D;
            }
            it.m_INT_Can3 = can3m;
            it.m_INT_Can2 = true;
            it.m_INT_Mode = INT_VPP_Prefs.INT_CapMode(mm, can3m, true);
            return;
        }
        bool can3 = g_Game.INT_VPP_SrvCan3D(m);
        bool can2 = m.IsActive();
        int sm = g_Game.INT_VPP_SrvMode(m);
        it.m_INT_Can3 = can3;
        it.m_INT_Can2 = can2;
        it.m_INT_Mode = sm;
        bool vis = false;
        if (sm != INT_VPP_MODE_HIDDEN && can2)
            vis = true;
        it.m_INT_Visible = vis;
        bool v3 = false;
        if (sm == INT_VPP_MODE_3D)
            v3 = true;
        it.m_INT_3D = v3;
    }

    // one marker to `mode` (capped by what the server allows): flags / override in memory + its 2D want flag;
    // the caller commits once (INT_CommitModes). «Мои» flags are written to the MarkerInfo; server / event keys
    // are collected into `srvKeys` (the override stores the player's wish, the cap applies on read)
    protected void INT_PutItemMode(INT_VPP_MapItem it, int mode, array<string> srvKeys)
    {
        MarkerInfo m = it.m_INT_Info;
        if (!m)
            return;
        int eff = INT_VPP_Prefs.INT_CapMode(mode, it.m_INT_Can3, it.m_INT_Can2);
        if (it.INT_IsMine())
        {
            bool act = false;
            if (eff != INT_VPP_MODE_HIDDEN)
                act = true;
            bool act3 = false;
            if (eff == INT_VPP_MODE_3D)
                act3 = true;
            m.SetActive(act);
            m.Set3DActive(act3);
            it.m_INT_Visible = act;
            it.m_INT_3D = act3;
        }
        else
        {
            string key = g_Game.INT_VPP_SrvModeKey(m);
            srvKeys.Insert(key);
            bool vis = false;
            if (eff != INT_VPP_MODE_HIDDEN && it.m_INT_Can2)
                vis = true;
            it.m_INT_Visible = vis;
            bool v3 = false;
            if (eff == INT_VPP_MODE_3D)
                v3 = true;
            it.m_INT_3D = v3;
        }
        it.m_INT_Mode = eff;
        if (m_INT_View)
            m_INT_View.INT_RefreshItem(it);
    }

    // one save per kind, one 3D sync per kind (never per marker)
    protected void INT_CommitModes(bool mine, array<string> srvKeys, int srvMode)
    {
        if (mine)
        {
            INT_PersistClient();
            array<ref MarkerInfo> perServer = null;
            if (m_AddressPort != "")
                perServer = m_CustomServerMarkers;
            g_Game.INT_VPP_SyncClient3d(m_ClientMarkers, perServer);
        }
        if (srvKeys && srvKeys.Count() > 0)
        {
            INT_VPP_Prefs prefs = INT_VPP_Prefs.INT_Get();
            if (prefs)
            {
                prefs.INT_SetMarkerModes(srvKeys, srvMode);
                if (prefs.INT_IsDirty())
                    prefs.INT_Save();
            }
            g_Game.INT_VPP_RebuildServer3d();
            // our own change: the open map must not take the 3D revision as an outside change (full rebuild)
            m_INT_SeenRev3D = g_Game.INT_VPP_3DRevision();
        }
        INT_Refresh3D();
    }

    // row chip: the next mode of this marker; returns false when nothing can change (only hidden allowed)
    bool INT_CycleRowMode(INT_VPP_ListRow r)
    {
        if (!r || !r.m_INT_Item)
            return false;
        INT_VPP_MapItem it = r.m_INT_Item;
        int next = INT_VPP_Prefs.INT_NextMode(it.m_INT_Mode, it.m_INT_Can3, it.m_INT_Can2);
        if (next == it.m_INT_Mode)
            return false;
        m_INT_ScrKeys.Clear();
        INT_PutItemMode(it, next, m_INT_ScrKeys);
        bool mine = it.INT_IsMine();
        INT_CommitModes(mine, m_INT_ScrKeys, next);
        INT_SyncRowMode(r);
        return true;
    }

    // header chip: every marker of the group to the next group mode (mixed → 3D, or 2D without any 3D allowed)
    bool INT_CycleGroupMode(INT_VPP_ListRow h, array<ref INT_VPP_ListRow> rows)
    {
        if (!h)
            return false;
        int layer = h.m_INT_Layer;
        bool can3 = false;
        bool can2 = false;
        int n = 0;
        foreach (INT_VPP_MapItem c : m_INT_Items)
        {
            if (!c || c.m_INT_Layer != layer)
                continue;
            n++;
            if (c.m_INT_Can3)
                can3 = true;
            if (c.m_INT_Can2)
                can2 = true;
        }
        if (n == 0)
            return false;
        // where the group stands: its mode, or the mode it was set to as far as the server allows (a 3D-less
        // marker stays 2D after «3D»: shown mixed, but the cycle goes on from 3D); really mixed = -1 → 3D first
        int cur = INT_GroupCapMode(layer);
        int next = INT_VPP_Prefs.INT_NextMode(cur, can3, can2);
        // a mode that would change nothing (every marker already there after the caps): one more step
        if (INT_GroupAt(layer, next))
            next = INT_VPP_Prefs.INT_NextMode(next, can3, can2);
        m_INT_ScrKeys.Clear();
        bool mine = false;
        foreach (INT_VPP_MapItem it : m_INT_Items)
        {
            if (!it || it.m_INT_Layer != layer)
                continue;
            if (it.INT_IsMine())
                mine = true;
            INT_PutItemMode(it, next, m_INT_ScrKeys);
        }
        INT_CommitModes(mine, m_INT_ScrKeys, next);
        if (rows)
        {
            foreach (INT_VPP_ListRow r : rows)
            {
                if (r)
                    INT_SyncRowMode(r);
            }
        }
        return true;
    }

    // true = every marker of the layer is at `mode` as far as the server allows it
    protected bool INT_GroupAt(int layer, int mode)
    {
        foreach (INT_VPP_MapItem it : m_INT_Items)
        {
            if (!it || it.m_INT_Layer != layer)
                continue;
            int capped = INT_VPP_Prefs.INT_CapMode(mode, it.m_INT_Can3, it.m_INT_Can2);
            if (capped != it.m_INT_Mode)
                return false;
        }
        return true;
    }

    // the mode the group was set to (2D and × first: an all-2D group of 3D-less markers is «2D», not «3D»), -1 = mixed
    protected int INT_GroupCapMode(int layer)
    {
        if (INT_GroupAt(layer, INT_VPP_MODE_2D))
            return INT_VPP_MODE_2D;
        if (INT_GroupAt(layer, INT_VPP_MODE_HIDDEN))
            return INT_VPP_MODE_HIDDEN;
        if (INT_GroupAt(layer, INT_VPP_MODE_3D))
            return INT_VPP_MODE_3D;
        return -1;
    }

    // the mode all markers of a layer share, -1 = mixed (or none)
    int INT_GroupMode(int layer)
    {
        int mode = -2;
        foreach (INT_VPP_MapItem it : m_INT_Items)
        {
            if (!it || it.m_INT_Layer != layer)
                continue;
            if (mode == -2)
                mode = it.m_INT_Mode;
            else if (mode != it.m_INT_Mode)
                return -1;
        }
        if (mode == -2)
            return -1;
        return mode;
    }

    // a list row follows its marker (item row) / its group (header row)
    void INT_SyncRowMode(INT_VPP_ListRow r)
    {
        if (!r)
            return;
        if (r.m_INT_Type == INT_VPP_ListRow.TYPE_HEADER)
        {
            if (r.m_INT_HasMode)
                r.m_INT_Mode = INT_GroupMode(r.m_INT_Layer);
            return;
        }
        INT_VPP_MapItem it = r.m_INT_Item;
        if (!it)
            return;
        r.m_INT_Mode = it.m_INT_Mode;
        r.m_INT_Can3 = it.m_INT_Can3;
        r.m_INT_Can2 = it.m_INT_Can2;
        INT_VPP_Prefs prefs = INT_VPP_Prefs.INT_Get();
        bool layerOn = true;
        if (prefs)
            layerOn = prefs.INT_IsLayerOn(it.m_INT_Layer);
        bool dim = false;
        if (!it.m_INT_Visible || !layerOn)
            dim = true;
        r.m_INT_Dim = dim;
    }

    // TEST: the map item of a marker (null when not on the map)
    INT_VPP_MapItem INT_TestItemOf(MarkerInfo info)
    {
        return INT_FindItem(info);
    }

    protected void INT_RebuildItems()
    {
        m_INT_Items.Clear();
        if (m_ClientMarkers)
        {
            foreach (MarkerInfo a : m_ClientMarkers)
            {
                if (a)
                    m_INT_Items.Insert(INT_MakeItem(a, INT_VPP_MapItem.KIND_MINE, INT_VPP_LAYER_MINE));
            }
        }
        if (m_CustomServerMarkers && m_AddressPort != "")
        {
            foreach (MarkerInfo b : m_CustomServerMarkers)
            {
                if (b)
                    m_INT_Items.Insert(INT_MakeItem(b, INT_VPP_MapItem.KIND_MINE_SRV, INT_VPP_LAYER_MINE));
            }
        }
        if (m_ServerMarkers)
        {
            foreach (MarkerInfo s : m_ServerMarkers)
            {
                if (!s)
                    continue;
                bool own = g_Game.INT_VPP_IsOwnServerMarker(s);
                if (own)
                    m_INT_Items.Insert(INT_MakeItem(s, INT_VPP_MapItem.KIND_SERVER, INT_VPP_LAYER_SERVER));
                else
                    m_INT_Items.Insert(INT_MakeItem(s, INT_VPP_MapItem.KIND_EVENT, INT_VPP_LAYER_EVENTS));
            }
        }
        if (m_INT_View)
        {
            m_INT_View.INT_SetItems(m_INT_Items);
            if (m_INT_MeShown)
                m_INT_View.INT_UpdateDistances(m_INT_LastMePos);
        }
        // the open card follows its marker (or closes when it is gone)
        if (m_INT_Card && m_INT_CardInfo)
        {
            INT_VPP_MapItem again = INT_FindItem(m_INT_CardInfo);
            if (again)
                INT_OpenCard(again);
            else
                INT_CloseCard();
        }
        m_INT_DirtyIslands = true;
        m_INT_DirtyList = true;
    }

    protected INT_VPP_MapItem INT_FindItem(MarkerInfo info)
    {
        if (!info)
            return null;
        foreach (INT_VPP_MapItem it : m_INT_Items)
        {
            if (it && it.m_INT_Info == info)
                return it;
        }
        return null;
    }

    // ===================================================================== INT: islands / modes
    protected void INT_ProcessDirty()
    {
        if (m_INT_DirtyAdmin)
        {
            m_INT_DirtyAdmin = false;
            INT_VPP_Client cl = INT_VPP_Client.Peek();
            bool admin = false;
            if (cl)
                admin = cl.IsAdmin();
            if (admin != m_INT_Admin)
            {
                m_INT_Admin = admin;
                if (m_INT_Isl)
                    m_INT_Isl.INT_SetAdmin(admin);
                if (!admin && m_INT_ZoneEd)
                    INT_CloseZoneEditor();
                if (!admin && m_INT_LabelEd)
                    INT_CloseLabelEditor();
                INT_PaintTab();
                m_INT_DirtyIslands = true;
            }
        }
        if (m_INT_DirtyRibbon)
        {
            m_INT_DirtyRibbon = false;
            if (m_INT_Ribbon)
                INT_BuildRibbon();
        }
        if (m_INT_DirtyRuler)
        {
            m_INT_DirtyRuler = false;
            INT_BuildRulerCard();
        }
        if (m_INT_DirtyIslands)
        {
            m_INT_DirtyIslands = false;
            INT_PaintIslands();
        }
        // the keys strip follows the zone editor's list / edit mode
        bool zEdit = false;
        if (m_INT_ZoneEd && m_INT_ZoneEd.INT_IsEditing())
            zEdit = true;
        if (zEdit != m_INT_LastZEdit)
        {
            m_INT_LastZEdit = zEdit;
            INT_ApplyMode();
        }
        bool lEdit = false;
        if (m_INT_LabelEd && m_INT_LabelEd.INT_IsEditing())
            lEdit = true;
        if (lEdit != m_INT_LastLEdit)
        {
            m_INT_LastLEdit = lEdit;
            INT_ApplyMode();
        }
        if (m_INT_DirtyList)
        {
            m_INT_DirtyList = false;
            if (m_INT_Isl && m_INT_Isl.INT_IsListOpen())
                INT_FillList();
        }
    }

    protected void INT_PaintIslands()
    {
        if (!m_INT_Isl)
            return;
        array<int> counts = m_INT_ScrCounts;
        counts.Clear();
        int nServer = 0;
        int nMine = 0;
        int nEvents = 0;
        foreach (INT_VPP_MapItem it : m_INT_Items)
        {
            if (!it)
                continue;
            if (it.m_INT_Layer == INT_VPP_LAYER_SERVER)
                nServer++;
            else if (it.m_INT_Layer == INT_VPP_LAYER_MINE)
                nMine++;
            else if (it.m_INT_Layer == INT_VPP_LAYER_EVENTS)
                nEvents++;
        }
        int nZones = 0;
        INT_VPP_Client cl = INT_VPP_Client.Peek();
        if (cl)
            nZones = cl.ZonesCount();
        int nRoute = 0;
        INT_VPP_Route route = INT_VPP_Route.INT_Peek();
        if (route)
            nRoute = route.INT_Count();
        counts.Insert(nServer);
        counts.Insert(nMine);
        counts.Insert(nEvents);
        counts.Insert(nZones);
        counts.Insert(nRoute);
        m_INT_Isl.INT_PaintLayers(counts, m_INT_Temp3D);
        m_INT_Isl.INT_SetTabCount(INT_TAB_MARKS, nMine);
        m_INT_Isl.INT_SetTabCount(INT_TAB_ROUTE, nRoute);
        int all = m_INT_Items.Count();
        m_INT_Isl.INT_SetListCount(all);
    }

    protected void INT_PaintTab()
    {
        if (!m_INT_Isl)
            return;
        int shown = m_INT_Tab;
        if (m_INT_Editor)
            shown = INT_TAB_MARKS;
        m_INT_Isl.INT_SetTab(shown);
    }

    // which islands show, which keys strip
    protected void INT_ApplyMode()
    {
        if (!m_INT_Isl)
            return;
        bool editor = false;
        bool editorLeft = false;
        if (m_INT_Editor)
        {
            editor = true;
            editorLeft = m_INT_Editor.INT_IsLeft();
        }
        bool ribbon = false;
        if (m_INT_Ribbon)
            ribbon = true;
        bool coords = true;
        if (editor || ribbon)
            coords = false;
        bool drawer = true;
        if (editor || ribbon)
            drawer = false;
        bool layers = true;
        if (editorLeft)
            layers = false;
        m_INT_Isl.INT_ShowIslands(coords, drawer, layers);
        int keys = INT_VPP_Islands.KEYS_MAP;
        if (editor)
            keys = INT_VPP_Islands.KEYS_EDITOR;
        else if (m_INT_RulerOn)
            keys = INT_VPP_Islands.KEYS_RULER;
        else if (m_INT_ZoneEd && m_INT_ZoneEd.INT_IsEditing())
            keys = INT_VPP_Islands.KEYS_ZONES;
        else if (m_INT_LabelEd && m_INT_LabelEd.INT_IsEditing())
            keys = INT_VPP_Islands.INT_KEYS_LABELS;
        else if (ribbon)
            keys = INT_VPP_Islands.KEYS_NONE;
        m_INT_Isl.INT_SetKeys(keys);
        INT_PaintTab();
    }

    // tabs: Карта / Метки (list) / Маршрут (ribbon) / Зоны (admin); the active one clicked again -> Карта
    // tab switch entrance of the new tab's panel (2026-11-14); nothing while the map is still opening
    protected bool m_INT_TabFxOn;

    protected void INT_TabPanelIn(Widget root)
    {
        if (!root || !m_INT_TabFxOn)
            return;
        INT_VPP_TabFx.INT_In(root, 0.1);
    }

    void INT_SelectTab(int idx)
    {
        if (!m_INT_Built || m_INT_TornDown)
            return;
        int target = idx;
        if (target == m_INT_Tab && target != INT_TAB_MAP && !m_INT_Editor)
            target = INT_TAB_MAP;
        if (target == INT_TAB_ZONES && !m_INT_Admin)
            return;
        if (target == INT_TAB_LABELS && !m_INT_Admin)
            return;
        if (m_INT_Editor)
            INT_CloseEditor();
        int prevTab = m_INT_Tab;
        m_INT_Tab = target;
        // list
        bool listOn = false;
        if (target == INT_TAB_MARKS)
            listOn = true;
        if (m_INT_Isl)
        {
            if (listOn)
            {
                bool listWas = m_INT_Isl.INT_IsListOpen();
                array<ref INT_VPP_ListRow> rows = new array<ref INT_VPP_ListRow>();
                INT_BuildRows(rows);
                m_INT_Isl.INT_SetListOpen(true, rows);
                if (!listWas)
                    INT_TabPanelIn(m_INT_Isl.INT_GetListRoot());
            }
            else if (m_INT_Isl.INT_IsListOpen())
            {
                m_INT_Isl.INT_SetListOpen(false, null);
            }
        }
        // ribbon
        if (target == INT_TAB_ROUTE)
        {
            INT_BuildRibbon();
            if (prevTab != INT_TAB_ROUTE && m_INT_Ribbon)
                INT_TabPanelIn(m_INT_Ribbon.INT_GetRoot());
        }
        else if (m_INT_Ribbon)
        {
            m_INT_Ribbon.INT_Close();
            m_INT_Ribbon = null;
        }
        // zones
        if (target == INT_TAB_ZONES)
        {
            if (!m_INT_ZoneEd)
            {
                if (m_INT_RulerOn)
                    INT_SetRulerOn(false);
                m_INT_ZoneEd = new INT_VPP_ZoneEditor();
                m_INT_ZoneEd.INT_Build(m_INT_Screen, m_INT_ScrW, m_INT_ScrH, this, m_INT_View);
                if (m_INT_Isl)
                    m_INT_Isl.INT_RegisterDrag(INT_VPP_Islands.INT_PAN_ZONES, m_INT_ZoneEd.INT_GetRoot(), m_INT_ZoneEd.INT_GetBlock());
                INT_TabPanelIn(m_INT_ZoneEd.INT_GetRoot());
            }
        }
        else if (m_INT_ZoneEd)
        {
            m_INT_ZoneEd.INT_Destroy();
            m_INT_ZoneEd = null;
        }
        // labels (admin, 2026-11-13)
        if (target == INT_TAB_LABELS)
        {
            if (!m_INT_LabelEd)
            {
                if (m_INT_RulerOn)
                    INT_SetRulerOn(false);
                m_INT_LabelEd = new INT_VPP_LabelEditor();
                m_INT_LabelEd.INT_Build(m_INT_Screen, m_INT_ScrW, m_INT_ScrH, this, m_INT_View);
                if (m_INT_Isl)
                    m_INT_Isl.INT_RegisterDrag(INT_VPP_Islands.INT_PAN_LABELS, m_INT_LabelEd.INT_GetRoot(), m_INT_LabelEd.INT_GetBlock());
                INT_TabPanelIn(m_INT_LabelEd.INT_GetRoot());
            }
        }
        else if (m_INT_LabelEd)
        {
            m_INT_LabelEd.INT_Destroy();
            m_INT_LabelEd = null;
        }
        INT_ApplyMode();
    }

    // ---- layer toggles (islands) ----
    void INT_ToggleLayer(int layer, bool on)
    {
        INT_VPP_Prefs prefs = INT_VPP_Prefs.INT_Get();
        if (!prefs)
            return;
        prefs.INT_SetLayerOn(layer, on);
        prefs.INT_Save();
        INT_Refresh3D();
        m_INT_DirtyIslands = true;
    }

    void INT_Set3DMaster(bool on)
    {
        INT_VPP_Prefs prefs = INT_VPP_Prefs.INT_Get();
        if (!prefs)
            return;
        prefs.INT_Set3DOn(on);
        prefs.INT_Save();
        INT_Refresh3D();
        m_INT_DirtyIslands = true;
    }

    void INT_SetDist3D(int layer, float m)
    {
        INT_VPP_Prefs prefs = INT_VPP_Prefs.INT_Get();
        if (!prefs)
            return;
        prefs.INT_SetDist3D(layer, m);
        prefs.INT_Save();
        INT_Refresh3D();
    }

    // size / transparency of everything drawn in 3D (popover «ВИД 3D-МЕТОК»); saved at once like the distances
    // «Названия» / «Расстояние» of «ВИД 3D-МЕТОК» (2026-11-14): name / metres text next to every 3D marker
    void INT_Set3DTexts(bool names, bool dist)
    {
        INT_VPP_Prefs prefs = INT_VPP_Prefs.INT_Get();
        if (!prefs)
            return;
        prefs.INT_Set3DNamesOn(names);
        prefs.INT_Set3DDistOn(dist);
        if (prefs.INT_IsDirty())
            prefs.INT_Save();
        INT_Refresh3D();
    }

    // «Сбросить» of «ВИД 3D-МЕТОК»: size, transparency, names and metres back to the defaults
    void INT_Reset3DLook()
    {
        INT_VPP_Prefs prefs = INT_VPP_Prefs.INT_Get();
        if (!prefs)
            return;
        prefs.INT_Reset3DLook();
        if (prefs.INT_IsDirty())
            prefs.INT_Save();
        INT_Refresh3D();
    }

    void INT_Set3DLook(int scalePct, int fadePct)
    {
        INT_VPP_Prefs prefs = INT_VPP_Prefs.INT_Get();
        if (!prefs)
            return;
        prefs.INT_Set3DLook(scalePct, fadePct);
        if (prefs.INT_IsDirty())
            prefs.INT_Save();
        INT_Refresh3D();
    }

    // ---- the list (tab «Метки») ----
    protected void INT_FillList()
    {
        if (!m_INT_Isl)
            return;
        array<ref INT_VPP_ListRow> rows = new array<ref INT_VPP_ListRow>();
        INT_BuildRows(rows);
        m_INT_Isl.INT_SetRows(rows);
    }

    protected INT_VPP_ListRow INT_HeaderRow(array<ref INT_VPP_ListRow> rows, string title, int count, int color)
    {
        INT_VPP_ListRow h = new INT_VPP_ListRow();
        h.m_INT_Type = INT_VPP_ListRow.TYPE_HEADER;
        h.m_INT_Text = title;
        h.m_INT_Sub = INT_VPPX_Text.Pad2(count);
        h.m_INT_Color = color;
        h.m_INT_Layer = -1;
        rows.Insert(h);
        return h;
    }

    protected string INT_DistSub(vector p)
    {
        if (!m_INT_MeShown)
            return "";
        float d = INT_VPP_Fmt.INT_Dist2D(m_INT_LastMePos, p);
        return INT_VPP_Fmt.INT_Dist(d);
    }

    protected void INT_ItemRows(array<ref INT_VPP_ListRow> rows, int layer, string title, int color)
    {
        int n = 0;
        foreach (INT_VPP_MapItem c : m_INT_Items)
        {
            if (c && c.m_INT_Layer == layer)
                n++;
        }
        if (n == 0)
            return;
        // the group's chip (2026-11-12): every marker of the layer at once
        INT_VPP_ListRow head = INT_HeaderRow(rows, title, n, color);
        head.m_INT_Layer = layer;
        head.m_INT_HasMode = true;
        head.m_INT_Mode = INT_GroupMode(layer);
        INT_VPP_Prefs prefs = INT_VPP_Prefs.INT_Get();
        bool layerOn = true;
        if (prefs)
            layerOn = prefs.INT_IsLayerOn(layer);
        foreach (INT_VPP_MapItem it : m_INT_Items)
        {
            if (!it || it.m_INT_Layer != layer)
                continue;
            INT_VPP_ListRow r = new INT_VPP_ListRow();
            r.m_INT_Type = INT_VPP_ListRow.TYPE_ITEM;
            r.m_INT_Text = it.m_INT_Name;
            r.m_INT_Sub = INT_DistSub(it.m_INT_Pos);
            r.m_INT_Color = it.m_INT_Color;
            r.m_INT_Icon = it.m_INT_IconPath;
            bool dim = false;
            if (!it.m_INT_Visible || !layerOn)
                dim = true;
            r.m_INT_Dim = dim;
            r.m_INT_Item = it;
            r.m_INT_Pos = it.m_INT_Pos;
            r.m_INT_Layer = layer;
            r.m_INT_HasMode = true;
            r.m_INT_Mode = it.m_INT_Mode;
            r.m_INT_Can3 = it.m_INT_Can3;
            r.m_INT_Can2 = it.m_INT_Can2;
            rows.Insert(r);
        }
    }

    protected void INT_BuildRows(array<ref INT_VPP_ListRow> rows)
    {
        INT_ItemRows(rows, INT_VPP_LAYER_SERVER, "Сервер", 0xFFF3EFE8);
        INT_ItemRows(rows, INT_VPP_LAYER_MINE, "Мои", 0xFF56E07E);
        INT_ItemRows(rows, INT_VPP_LAYER_EVENTS, "События", 0xFFFFC246);
        INT_VPP_Client cl = INT_VPP_Client.Peek();
        array<ref INT_VPP_Zone> zones = null;
        if (cl)
            zones = cl.GetZones();
        if (zones && zones.Count() > 0)
        {
            INT_HeaderRow(rows, "Зоны", zones.Count(), 0xFFCFE35A);
            foreach (INT_VPP_Zone z : zones)
            {
                if (!z)
                    continue;
                INT_VPP_ListRow zr = new INT_VPP_ListRow();
                zr.m_INT_Type = INT_VPP_ListRow.TYPE_ZONE;
                zr.m_INT_Text = z.name;
                vector zc = Vector(z.cx, 0, z.cz);
                zr.m_INT_Sub = INT_DistSub(zc);
                zr.m_INT_Color = INT_VPP_Validate.ForceAlpha(z.color);
                zr.m_INT_ZoneId = z.id;
                zr.m_INT_Pos = zc;
                rows.Insert(zr);
            }
        }
        INT_VPP_Route route = INT_VPP_Route.INT_Peek();
        if (route && route.INT_Count() > 0)
        {
            int rn = route.INT_Count();
            INT_HeaderRow(rows, "Маршрут", rn, INT_VPPX_Look.ORANGE);
            int nextIdx = route.INT_NextIndex();
            for (int i = 0; i < rn; i++)
            {
                INT_VPP_RoutePoint rp = route.INT_At(i);
                if (!rp)
                    continue;
                INT_VPP_ListRow pr = new INT_VPP_ListRow();
                pr.m_INT_Type = INT_VPP_ListRow.TYPE_ROUTE;
                int num = route.INT_NumberOf(i);
                string pad = INT_VPPX_Text.Pad2(num);
                pr.m_INT_Text = pad + " · " + rp.m_INT_Name;
                pr.m_INT_Sub = INT_DistSub(rp.m_INT_Pos);
                pr.m_INT_Color = INT_VPPX_Look.ORANGE;
                bool passed = false;
                if (i < nextIdx)
                    passed = true;
                pr.m_INT_Dim = passed;
                pr.m_INT_RouteIdx = i;
                pr.m_INT_Pos = rp.m_INT_Pos;
                rows.Insert(pr);
            }
        }
    }

    void INT_OnListRow(INT_VPP_ListRow r)
    {
        if (!r)
            return;
        INT_MapTo(r.m_INT_Pos);
        if (r.m_INT_Type == INT_VPP_ListRow.TYPE_ITEM && r.m_INT_Item)
            INT_OpenCard(r.m_INT_Item);
    }

    // ===================================================================== INT: marker card
    protected void INT_OpenCard(INT_VPP_MapItem it)
    {
        if (!it || !m_INT_Screen)
            return;
        // never over the confirm band
        if (INT_ConfirmUp())
            return;
        if (m_INT_Card)
            m_INT_Card.INT_Close();
        m_INT_Card = new INT_VPP_MarkerCard();
        m_INT_Card.INT_Build(m_INT_Screen, m_INT_ScrW, m_INT_ScrH, this, it, m_INT_LastMePos, m_INT_MeShown);
        m_INT_CardInfo = it.m_INT_Info;
        if (m_INT_View)
            m_INT_View.INT_SetSelected(it);
        m_INT_CardsDirty = true;
    }

    protected void INT_CloseCard()
    {
        if (m_INT_Card)
            m_INT_Card.INT_Close();
        m_INT_Card = null;
        m_INT_CardInfo = null;
        if (m_INT_View)
            m_INT_View.INT_SetSelected(null);
    }

    void INT_CardEdit(INT_VPP_MapItem it)
    {
        if (!it)
            return;
        INT_CloseCard();
        INT_OpenEditorFor(it);
    }

    void INT_CardRoute(INT_VPP_MapItem it)
    {
        if (!it)
            return;
        vector pos = it.m_INT_Pos;
        string nm = it.m_INT_Name;
        INT_CloseCard();
        if (INT_AddRoutePoint(pos, nm))
            INT_SelectTab(INT_TAB_ROUTE);
    }

    void INT_CardDelete(INT_VPP_MapItem it)
    {
        if (!it)
            return;
        if (it.m_INT_Kind == INT_VPP_MapItem.KIND_SERVER && m_INT_Admin)
        {
            INT_AskSrvMarkerDelete(it.m_INT_Info, it.m_INT_Name, it.m_INT_IconPath, it.m_INT_Color);
            return;
        }
        if (!it.INT_IsMine())
            return;
        INT_AskDeleteMarker(it.m_INT_Info, it.m_INT_Name, it.m_INT_IconPath, it.m_INT_Color);
    }

    // map admin (the last INT_Settings): the cards of server markers get «Изменить» / «×» (2026-11-14)
    bool INT_IsAdmin()
    {
        return m_INT_Admin;
    }

    // server marker delete (2026-11-14): the usual red band, then INT_SrvMarkerDelete
    protected void INT_AskSrvMarkerDelete(MarkerInfo info, string nm, string iconPath, int color)
    {
        if (!info || info.INT_GetSrvId() <= 0 || !INT_EnsureConfirm())
            return;
        m_INT_ConfirmKind = INT_CONFIRM_SRVMARKER;
        m_INT_ConfirmSrvId = info.INT_GetSrvId();
        m_INT_ConfirmIcon = iconPath;
        m_INT_ConfirmColor = color;
        m_INT_Confirm.SetAccent(INT_VPPX_Look.RED);
        m_INT_Confirm.SetKind("метка сервера");
        m_INT_Confirm.Open(m_INT_ConfirmHost, "Удалить метку сервера", nm, "Метка исчезнет у всех игроков сервера.", "Удалить", "Отмена");
        INT_RaiseConfirm();
        INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.WARN);
    }

    // places every pinned card when the projection changed (never per frame otherwise)
    protected void INT_PlaceCards()
    {
        if (!m_INT_View)
            return;
        int pc = m_INT_View.INT_ProjectCount();
        if (pc == m_INT_LastPlaced && !m_INT_CardsDirty)
            return;
        m_INT_LastPlaced = pc;
        m_INT_CardsDirty = false;
        float f = m_INT_View.INT_Factor();
        float ax;
        float ay;
        bool on;
        if (m_INT_Card)
        {
            INT_VPP_MapItem it = m_INT_Card.INT_GetItem();
            if (it)
            {
                on = m_INT_View.INT_ToScreen(it.m_INT_Pos, ax, ay);
                m_INT_Card.INT_Place(ax, ay, on, f);
            }
        }
        if (m_INT_Editor)
        {
            on = m_INT_View.INT_ToScreen(m_INT_EditorPos, ax, ay);
            m_INT_Editor.INT_Place(ax, ay, on, f);
        }
        if (m_INT_Ribbon)
        {
            INT_VPP_Route route = INT_VPP_Route.INT_Peek();
            if (route)
            {
                int ni = route.INT_NextIndex();
                if (ni >= 0)
                {
                    INT_VPP_RoutePoint rp = route.INT_At(ni);
                    if (rp)
                    {
                        on = m_INT_View.INT_ToScreen(rp.m_INT_Pos, ax, ay);
                        m_INT_Ribbon.INT_PlaceNext(ax, ay, on, f);
                    }
                }
            }
        }
        if (m_INT_RulerCard)
        {
            int rc = m_INT_RulerPts.Count();
            if (rc > 0)
            {
                on = m_INT_View.INT_ToScreen(m_INT_RulerPts.Get(rc - 1), ax, ay);
                m_INT_RulerCard.INT_Place(ax, ay, on, f);
            }
        }
    }

    // ===================================================================== INT: editor
    protected bool INT_EditorOnLeft(vector pos)
    {
        if (!m_INT_View)
            return false;
        float ax;
        float ay;
        m_INT_View.INT_ToScreen(pos, ax, ay);
        float gx = ax / m_INT_View.INT_Factor();
        if (gx > m_INT_ScrW - 48 - INT_VPP_EditorCard.CARD_W - 40)
            return true;
        return false;
    }

    protected void INT_OpenEditorNew(vector pos)
    {
        if (!m_INT_Screen)
            return;
        INT_CloseCard();
        if (m_INT_Editor)
            INT_CloseEditor();
        if (m_INT_Tab != INT_TAB_MAP)
            INT_SelectTab(INT_TAB_MAP);
        if (m_INT_RulerOn)
            INT_SetRulerOn(false);
        m_INT_EditInfo = null;
        m_INT_EditKind = INT_VPP_MapItem.KIND_MINE;
        m_INT_EditorPos = pos;
        bool left = INT_EditorOnLeft(pos);
        m_INT_Editor = new INT_VPP_EditorCard();
        // a map admin chooses «Моя / Серверная» on the card (2026-11-14)
        m_INT_Editor.INT_SetServerOptions(false, m_INT_Admin);
        m_INT_Editor.INT_Build(m_INT_Screen, m_INT_ScrW, m_INT_ScrH, this, null, pos, "", s_INT_LastColor, s_INT_LastIcon, 1, false, m_INT_LastMePos, m_INT_MeShown, left);
        if (m_INT_Isl)
            m_INT_Isl.INT_RegisterDrag(INT_VPP_Islands.INT_PAN_MARKER, m_INT_Editor.INT_GetRoot(), m_INT_Editor.INT_GetBlock());
        if (m_INT_View)
            m_INT_View.INT_SetGhost(true, pos, s_INT_LastIcon, s_INT_LastColor);
        m_INT_Editor.INT_FocusName();
        m_INT_CardsDirty = true;
        INT_ApplyMode();
        INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.OPEN);
    }

    protected void INT_OpenEditorFor(INT_VPP_MapItem it)
    {
        if (!it || !m_INT_Screen)
            return;
        // own markers, and (2026-11-14) the server's markers for a map admin
        bool srvItem = false;
        if (it.m_INT_Kind == INT_VPP_MapItem.KIND_SERVER && m_INT_Admin && it.m_INT_Info && it.m_INT_Info.INT_GetSrvId() > 0)
            srvItem = true;
        if (!it.INT_IsMine() && !srvItem)
            return;
        INT_CloseCard();
        if (m_INT_Editor)
            INT_CloseEditor();
        if (m_INT_Tab != INT_TAB_MAP)
            INT_SelectTab(INT_TAB_MAP);
        if (m_INT_RulerOn)
            INT_SetRulerOn(false);
        m_INT_EditInfo = it.m_INT_Info;
        m_INT_EditKind = it.m_INT_Kind;
        m_INT_EditorPos = it.m_INT_Pos;
        int vis = 0;
        if (!it.m_INT_Visible)
            vis = 2;
        else if (it.m_INT_3D)
            vis = 1;
        bool only = false;
        if (it.m_INT_Kind == INT_VPP_MapItem.KIND_MINE_SRV)
            only = true;
        bool left = INT_EditorOnLeft(it.m_INT_Pos);
        m_INT_Editor = new INT_VPP_EditorCard();
        m_INT_Editor.INT_SetServerOptions(srvItem, false);
        m_INT_Editor.INT_Build(m_INT_Screen, m_INT_ScrW, m_INT_ScrH, this, it, it.m_INT_Pos, it.m_INT_Name, it.m_INT_Color, it.m_INT_IconPath, vis, only, m_INT_LastMePos, m_INT_MeShown, left);
        if (m_INT_Isl)
            m_INT_Isl.INT_RegisterDrag(INT_VPP_Islands.INT_PAN_MARKER, m_INT_Editor.INT_GetRoot(), m_INT_Editor.INT_GetBlock());
        if (m_INT_View)
            m_INT_View.INT_SetSelected(it);
        m_INT_CardsDirty = true;
        INT_ApplyMode();
        INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.OPEN);
    }

    protected void INT_CloseEditor()
    {
        if (m_INT_Editor)
            m_INT_Editor.INT_Close();
        m_INT_Editor = null;
        m_INT_EditInfo = null;
        if (m_INT_View)
        {
            m_INT_View.INT_SetGhost(false, vector.Zero, "", 0);
            m_INT_View.INT_SetSelected(null);
        }
        g_Game.SetKeyboardBusy(false);
        INT_ApplyMode();
    }

    void INT_CancelEditor()
    {
        if (!m_INT_Editor)
            return;
        INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CLOSE);
        INT_CloseEditor();
    }

    // persist exactly like VPP: UpdateCachedMarkerInfos (+ the per-server list) and SaveCache
    protected void INT_PersistClient()
    {
        ClientMarkersCache cache = g_Game.GetClientMarkersCache();
        if (!cache)
            return;
        cache.UpdateCachedMarkerInfos(m_ClientMarkers);
        if (m_AddressPort != "")
            cache.UpdateCachedMarkerInfos(m_CustomServerMarkers, m_AddressPort);
        cache.SaveCache();
    }

    void INT_EditorSave()
    {
        if (!m_INT_Editor)
            return;
        string nm = m_INT_Editor.INT_GetName();
        int col = m_INT_Editor.INT_GetColor();
        string icon = m_INT_Editor.INT_GetIconPath();
        int vis = m_INT_Editor.INT_GetVis();
        bool only = m_INT_Editor.INT_GetOnlyServer();
        if (m_AddressPort == "")
            only = false;
        //INT (2026-11-14): «Название в 3D» of the card -> MarkerInfo (false = the name is hidden next to the 3D icon)
        bool name3d = m_INT_Editor.INT_GetName3D();
        bool hideName3d = false;
        if (!name3d)
            hideName3d = true;
        //INT (2026-11-14): a server marker is not saved here: the server checks, writes VPPMapConfig.json and sends
        //the new list to everybody (a refusal comes back as a notice)
        if (m_INT_Editor.INT_IsServer())
        {
            INT_EditorSaveServer(nm, icon, col, vis, hideName3d);
            return;
        }
        vector colV = INT_VPP_Fmt.INT_ArgbToVec(col);
        bool active = true;
        if (vis == 2)
            active = false;
        bool act3d = false;
        if (vis == 1)
            act3d = true;
        if (!m_ClientMarkers || !m_CustomServerMarkers)
        {
            INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.ERROR);
            return;
        }
        MarkerInfo info = m_INT_EditInfo;
        if (info)
        {
            info.SetName(nm);
            info.SetIconPath(icon);
            info.SetColor(colV);
            info.SetActive(active);
            info.Set3DActive(act3d);
            info.INT_SetName3DHidden(hideName3d);
            bool wasSrv = false;
            if (m_INT_EditKind == INT_VPP_MapItem.KIND_MINE_SRV)
                wasSrv = true;
            if (wasSrv != only)
            {
                if (only)
                {
                    m_ClientMarkers.RemoveItem(info);
                    m_CustomServerMarkers.Insert(info);
                }
                else
                {
                    m_CustomServerMarkers.RemoveItem(info);
                    m_ClientMarkers.Insert(info);
                }
            }
        }
        else
        {
            vector pos = m_INT_EditorPos;
            MarkerInfo made = new MarkerInfo(nm, icon, colV, pos, active, act3d);
            made.INT_SetName3DHidden(hideName3d);
            if (only)
                m_CustomServerMarkers.Insert(made);
            else
                m_ClientMarkers.Insert(made);
        }
        s_INT_LastColor = col;
        s_INT_LastIcon = icon;
        INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CONFIRM);
        INT_PersistClient();
        INT_CloseEditor();
        DisplayClientMarkers();
    }

    protected void INT_EditorSaveServer(string nm, string icon, int col, int vis, bool hideName3d)
    {
        INT_VPP_Client cl = INT_VPP_Client.Peek();
        if (!cl)
        {
            INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.ERROR);
            return;
        }
        vector colV = INT_VPP_Fmt.INT_ArgbToVec(col);
        bool active = true;
        if (vis == 2)
            active = false;
        bool act3d = false;
        if (vis == 1)
            act3d = true;
        vector pos = m_INT_Editor.INT_GetPos();
        MarkerInfo req = new MarkerInfo(nm, icon, colV, pos, active, act3d);
        req.INT_SetName3DHidden(hideName3d);
        int sid = 0;
        if (m_INT_EditInfo)
            sid = m_INT_EditInfo.INT_GetSrvId();
        req.INT_SetSrvId(sid);
        bool sent = cl.INT_RequestSrvMarkerSave(req);
        if (!sent)
        {
            INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.ERROR);
            if (m_INT_Isl)
                m_INT_Isl.INT_ShowNotice("Метка сервера: " + cl.INT_SrvMarkerError());
            return;
        }
        s_INT_LastColor = col;
        s_INT_LastIcon = icon;
        INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CONFIRM);
        INT_CloseEditor();
    }

    // a refused server-marker request: one notice (2026-11-14)
    protected void INT_PollSrvMarkerResult()
    {
        INT_VPP_Client cl = INT_VPP_Client.Peek();
        if (!cl)
            return;
        int rev = cl.INT_SrvMarkerResultsRevision();
        if (rev == m_INT_SeenSrvResults)
            return;
        m_INT_SeenSrvResults = rev;
        if (cl.INT_SrvMarkerResultOk())
            return;
        INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.ERROR);
        if (m_INT_Isl)
            m_INT_Isl.INT_ShowNotice("Метка сервера: " + cl.INT_SrvMarkerError());
    }

    void INT_EditorDelete()
    {
        if (!m_INT_Editor || !m_INT_EditInfo)
            return;
        if (m_INT_Editor.INT_IsServer())
        {
            string snm = m_INT_EditInfo.GetName();
            string sic = m_INT_Editor.INT_GetIconPath();
            int scol = m_INT_Editor.INT_GetColor();
            INT_AskSrvMarkerDelete(m_INT_EditInfo, snm, sic, scol);
            return;
        }
        string nm = m_INT_EditInfo.GetName();
        string icon = m_INT_Editor.INT_GetIconPath();
        int col = m_INT_Editor.INT_GetColor();
        INT_AskDeleteMarker(m_INT_EditInfo, nm, icon, col);
    }

    protected void INT_DeleteMarkerInfo(MarkerInfo info)
    {
        if (!info)
            return;
        if (m_ClientMarkers)
            m_ClientMarkers.RemoveItem(info);
        if (m_CustomServerMarkers)
            m_CustomServerMarkers.RemoveItem(info);
        INT_PersistClient();
        if (m_INT_CardInfo == info)
            INT_CloseCard();
        if (m_INT_Editor && m_INT_EditInfo == info)
            INT_CloseEditor();
        DisplayClientMarkers();
    }

    // ===================================================================== INT: confirm band
    protected bool INT_EnsureConfirm()
    {
        if (!m_INT_ConfirmHost)
            return false;
        if (!m_INT_Confirm)
        {
            m_INT_Confirm = new INT_VPPX_Confirm();
            m_INT_Confirm.OnResult().Insert(INT_OnConfirm);
            m_INT_Confirm.OnPreview().Insert(INT_OnConfirmPreview);
        }
        if (m_INT_Confirm.IsOpen())
            return false;
        INT_BlurAll();
        // modal = nothing on top (owner's bug 2026-11-12: the marker card stayed over the confirm band): the card
        // and its pin line go, the 3D popover closes; INT_OpenCard refuses while the band is up and
        // INT_KeepConfirmTop raises the band again if anything else is built after it
        INT_CloseCard();
        if (m_INT_Isl)
            m_INT_Isl.INT_ClosePop();
        return true;
    }

    protected bool INT_ConfirmUp()
    {
        if (m_INT_Confirm && m_INT_Confirm.IsOpen())
            return true;
        return false;
    }

    // the band's own sort is above every other island / card (300): measured 2026-11-12, re-sorting it at the same
    // value does not lift it over a sibling built later. Set when the band opens; per frame only one int compare.
    static const int INT_CONFIRM_SORT = 320;

    protected void INT_RaiseConfirm()
    {
        if (m_INT_ConfirmHost)
            INT_VPPX_Look.SortTree(m_INT_ConfirmHost, INT_CONFIRM_SORT);
    }

    protected void INT_KeepConfirmTop()
    {
        if (!m_INT_ConfirmHost || !INT_ConfirmUp())
            return;
        if (m_INT_ConfirmHost.GetSort() != INT_CONFIRM_SORT)
            INT_RaiseConfirm();
    }

    // TEST: confirm band / marker card state and hooks
    bool INT_TestConfirmOpen()
    {
        return INT_ConfirmUp();
    }

    // the band's sort is above every other child of the screen frame (or equal and it is the last one)
    bool INT_TestConfirmOnTop()
    {
        if (!m_INT_Screen || !m_INT_ConfirmHost)
            return false;
        int hs = m_INT_ConfirmHost.GetSort();
        bool after = false;
        Widget c = m_INT_Screen.GetChildren();
        while (c)
        {
            if (c == m_INT_ConfirmHost)
            {
                after = true;
            }
            else
            {
                int cs = c.GetSort();
                if (cs > hs)
                    return false;
                if (cs == hs && after)
                    return false;
            }
            c = c.GetSibling();
        }
        return true;
    }

    bool INT_TestHasCard()
    {
        if (m_INT_Card)
            return true;
        return false;
    }

    void INT_TestOpenCardFor(MarkerInfo info)
    {
        INT_VPP_MapItem it = INT_FindItem(info);
        if (it)
            INT_OpenCard(it);
    }

    void INT_TestAskDelete(MarkerInfo info)
    {
        INT_VPP_MapItem it = INT_FindItem(info);
        if (!it)
            return;
        INT_AskDeleteMarker(info, it.m_INT_Name, it.m_INT_IconPath, it.m_INT_Color);
    }

    void INT_TestAnswer(bool yes)
    {
        if (!INT_ConfirmUp())
            return;
        if (yes)
            m_INT_Confirm.Confirm();
        else
            m_INT_Confirm.Cancel();
    }

    // a personal marker made the way the editor saves one (one save, 2D + 3D lists synced)
    MarkerInfo INT_TestAddMine(string nm, vector pos)
    {
        if (!m_ClientMarkers)
            return null;
        vector col = "255 200 40";
        MarkerInfo made = new MarkerInfo(nm, INT_VPP_Icons.INT_DefaultPath(), col, pos, true, true);
        m_ClientMarkers.Insert(made);
        INT_PersistClient();
        DisplayClientMarkers();
        return made;
    }

    protected void INT_AskDeleteMarker(MarkerInfo info, string nm, string iconPath, int color)
    {
        if (!info || !INT_EnsureConfirm())
            return;
        m_INT_ConfirmKind = INT_CONFIRM_MARKER;
        m_INT_ConfirmInfo = info;
        m_INT_ConfirmIcon = iconPath;
        m_INT_ConfirmColor = color;
        m_INT_Confirm.SetAccent(INT_VPPX_Look.RED);
        m_INT_Confirm.SetKind("метка");
        m_INT_Confirm.Open(m_INT_ConfirmHost, "Удалить метку", nm, "Метка исчезнет с карты и из мира. Это нельзя отменить.", "Удалить", "Отмена");
        INT_RaiseConfirm();
        INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.WARN);
    }

    void INT_AskZoneDelete(int id, string nm)
    {
        if (id <= 0 || !INT_EnsureConfirm())
            return;
        m_INT_ConfirmKind = INT_CONFIRM_ZONE;
        m_INT_ConfirmZoneId = id;
        m_INT_ConfirmIcon = "";
        m_INT_ConfirmColor = INT_VPPX_Look.ORANGE;
        INT_VPP_Client cl = INT_VPP_Client.Peek();
        if (cl)
        {
            INT_VPP_Zone z = cl.FindZone(id);
            if (z)
                m_INT_ConfirmColor = INT_VPP_Validate.ForceAlpha(z.color);
        }
        m_INT_Confirm.SetAccent(INT_VPPX_Look.RED);
        m_INT_Confirm.SetKind("зона");
        m_INT_Confirm.Open(m_INT_ConfirmHost, "Удалить зону", nm, "Зона исчезнет у всех игроков сервера.", "Удалить", "Отмена");
        INT_RaiseConfirm();
        INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.WARN);
    }

    // admin text label (2026-11-13): the same red band as zones; the preview shows the text in its colour
    void INT_AskLabelDelete(int id, string text)
    {
        if (id <= 0 || !INT_EnsureConfirm())
            return;
        m_INT_ConfirmKind = INT_CONFIRM_LABEL;
        m_INT_ConfirmLabelId = id;
        m_INT_ConfirmIcon = "";
        m_INT_ConfirmColor = INT_VPPX_Look.WHITE;
        m_INT_ConfirmLabelText = text;
        m_INT_ConfirmLabelSize = INT_VPP_Net.INT_LABEL_DEF_SIZE;
        INT_VPP_Client cl = INT_VPP_Client.Peek();
        if (cl)
        {
            INT_VPP_Label l = cl.INT_FindLabel(id);
            if (l)
            {
                m_INT_ConfirmColor = INT_VPP_Validate.ForceAlpha(l.INT_Color);
                m_INT_ConfirmLabelSize = l.INT_Size;
            }
        }
        string nm = INT_VPPX_Look.FitText(text, 18, false, 520);
        m_INT_Confirm.SetAccent(INT_VPPX_Look.RED);
        m_INT_Confirm.SetKind("надпись");
        m_INT_Confirm.Open(m_INT_ConfirmHost, "Удалить надпись", nm, "Надпись исчезнет с карты у всех игроков сервера.", "Удалить", "Отмена");
        INT_RaiseConfirm();
        INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.WARN);
    }

    void INT_OnConfirmPreview(Widget clip, INT_VPPX_Confirm b)
    {
        if (!clip)
            return;
        float cx = INT_VPPX_Confirm.CLIP_W * 0.5;
        float cy = INT_VPPX_Confirm.CLIP_H * 0.5;
        if (m_INT_ConfirmKind == INT_CONFIRM_LABEL)
        {
            float ps = m_INT_ConfirmLabelSize;
            if (ps > 40)
                ps = 40;
            if (ps < 18)
                ps = 18;
            string pt = INT_VPPX_Look.FitText(m_INT_ConfirmLabelText, ps, false, INT_VPPX_Confirm.CLIP_W - 40);
            int pc = m_INT_ConfirmColor;
            bool pbig = false;
            if (ps >= 40)
                pbig = true;
            TextWidget pw = INT_UIF_Draw.Text(clip, 0, cy - ps, INT_VPPX_Confirm.CLIP_W, ps * 2, pt, ps, pc, INT_UIF_Align.CENTER, pbig);
            if (pw)
                pw.SetOutline(1, 0xFF0B0B0B);
            return;
        }
        if (m_INT_ConfirmKind == INT_CONFIRM_MARKER && m_INT_ConfirmIcon != "")
        {
            INT_UIF_Draw.Texture(clip, cx - 48, cy - 48, 96, 96, m_INT_ConfirmIcon, m_INT_ConfirmColor);
            return;
        }
        INT_VPPX_Look.Diamond(clip, cx, cy, 80, false, m_INT_ConfirmColor);
    }

    void INT_OnConfirm(int result, INT_VPPX_Confirm b)
    {
        int kind = m_INT_ConfirmKind;
        m_INT_ConfirmKind = 0;
        if (result != INT_VPPX_Confirm.RESULT_YES)
        {
            INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CLOSE);
            m_INT_ConfirmInfo = null;
            return;
        }
        if (kind == INT_CONFIRM_MARKER)
        {
            MarkerInfo info = m_INT_ConfirmInfo;
            m_INT_ConfirmInfo = null;
            INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CONFIRM);
            INT_DeleteMarkerInfo(info);
        }
        else if (kind == INT_CONFIRM_ZONE)
        {
            // the server's answer plays CONFIRM / ERROR (zone editor, OnResult)
            INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CLICK);
            if (m_INT_ZoneEd)
                m_INT_ZoneEd.INT_SendDelete(m_INT_ConfirmZoneId);
        }
        else if (kind == INT_CONFIRM_LABEL)
        {
            // the server's answer plays CONFIRM / ERROR (label editor, INT_LabelResult)
            INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CLICK);
            if (m_INT_LabelEd)
                m_INT_LabelEd.INT_SendDelete(m_INT_ConfirmLabelId);
        }
        else if (kind == INT_CONFIRM_SRVMARKER)
        {
            INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CLICK);
            INT_VPP_Client dcl = INT_VPP_Client.Peek();
            bool sentDel = false;
            if (dcl)
                sentDel = dcl.INT_RequestSrvMarkerDelete(m_INT_ConfirmSrvId);
            if (!sentDel && m_INT_Isl && dcl)
                m_INT_Isl.INT_ShowNotice("Метка сервера: " + dcl.INT_SrvMarkerError());
            if (m_INT_Editor && m_INT_Editor.INT_IsServer())
                INT_CloseEditor();
        }
    }

    // ===================================================================== INT: route
    // RMB / «Маршрут сюда»: name = given, else the nearest marker within 50 m, else «Точка N»
    protected bool INT_AddRoutePoint(vector pos, string nm)
    {
        INT_VPP_Route route = INT_VPP_Route.INT_Get();
        if (!route)
            return false;
        string pointName = nm;
        if (pointName == "")
        {
            float best = INT_ROUTE_NAME_R;
            foreach (INT_VPP_MapItem it : m_INT_Items)
            {
                if (!it)
                    continue;
                float d = INT_VPP_Fmt.INT_Dist2D(it.m_INT_Pos, pos);
                if (d <= best)
                {
                    best = d;
                    pointName = it.m_INT_Name;
                }
            }
        }
        if (pointName == "")
        {
            int cnt = route.INT_Count();
            int num = route.INT_NumberOf(cnt);
            pointName = INT_TXT_POINT + " " + num.ToString();
        }
        bool ok = route.INT_Add(pos, pointName);
        if (!ok)
        {
            INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.DENY);
            if (m_INT_Isl)
                m_INT_Isl.INT_ShowNotice(INT_TXT_ROUTE_FULL);
            return false;
        }
        INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CONFIRM);
        INT_VPP_Prefs prefs = INT_VPP_Prefs.INT_Get();
        if (prefs && !prefs.INT_IsLayerOn(INT_VPP_LAYER_ROUTE) && m_INT_Isl)
            m_INT_Isl.INT_ShowNotice(INT_TXT_ROUTE_OFF);
        return true;
    }

    protected void INT_BuildRibbon()
    {
        if (!m_INT_Screen)
            return;
        if (m_INT_Ribbon)
            m_INT_Ribbon.INT_Close();
        m_INT_Ribbon = new INT_VPP_RouteRibbon();
        m_INT_Ribbon.INT_Build(m_INT_Screen, m_INT_ScrW, m_INT_ScrH, this, m_INT_LastMePos, m_INT_MeShown);
        if (m_INT_Isl)
            m_INT_Isl.INT_RegisterDrag(INT_VPP_Islands.INT_PAN_ROUTE, m_INT_Ribbon.INT_GetRoot(), m_INT_Ribbon.INT_GetBlock());
        m_INT_CardsDirty = true;
    }

    void INT_RouteRemove(int i)
    {
        INT_VPP_Route route = INT_VPP_Route.INT_Get();
        if (!route)
            return;
        INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CLICK);
        route.INT_RemoveAt(i);
    }

    void INT_RouteClear()
    {
        INT_VPP_Route route = INT_VPP_Route.INT_Get();
        if (!route)
            return;
        if (route.INT_Count() > 0)
            INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.WARN);
        route.INT_Clear();
    }

    // review #8: one in-place reverse of the points still to walk (one save, one event, numbers kept in order)
    void INT_RouteReverse()
    {
        INT_VPP_Route route = INT_VPP_Route.INT_Get();
        if (!route)
            return;
        if (!route.INT_ReverseRemaining())
        {
            INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.DENY);
            return;
        }
        INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.CLICK);
    }

    void INT_RouteAutoRemove(bool on)
    {
        INT_VPP_Route route = INT_VPP_Route.INT_Get();
        if (!route)
            return;
        INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.TOGGLE);
        route.INT_SetAutoRemove(on);
    }

    // ===================================================================== INT: ruler
    protected int INT_RulerMax()
    {
        int v = INT_VPP_Net.DEF_RULER_MAX_POINTS;
        INT_VPP_Client cl = INT_VPP_Client.Peek();
        if (cl)
        {
            INT_VPP_ClientSettings st = cl.GetSettings();
            if (st)
                v = st.RulerMaxPoints;
        }
        return INT_VPP_Validate.ClampInt(v, 2, INT_VPP_Net.HARD_MAX_RULER_POINTS);
    }

    protected void INT_RulerAdd(vector w)
    {
        int maxR = INT_RulerMax();
        if (m_INT_RulerPts.Count() >= maxR)
        {
            INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.DENY);
            if (m_INT_Isl)
                m_INT_Isl.INT_ShowNotice(INT_TXT_RULER_FULL);
            return;
        }
        INT_VPPX_Sfx.Play(INT_VPPX_SfxKind.TICK);
        m_INT_RulerPts.Insert(w);
        INT_RulerChanged();
    }

    protected void INT_RulerChanged()
    {
        if (m_INT_View)
            m_INT_View.INT_SetRuler(m_INT_RulerOn, m_INT_RulerPts);
        m_INT_DirtyRuler = true;
    }

    protected void INT_BuildRulerCard()
    {
        if (m_INT_RulerCard)
            m_INT_RulerCard.INT_Close();
        m_INT_RulerCard = null;
        if (!m_INT_RulerOn || !m_INT_Screen)
            return;
        m_INT_RulerCard = new INT_VPP_RulerCard();
        int maxR = INT_RulerMax();
        m_INT_RulerCard.INT_Build(m_INT_Screen, m_INT_ScrW, m_INT_ScrH, this, m_INT_RulerPts, maxR);
        m_INT_CardsDirty = true;
    }

    // ===================================================================== INT: zone editor
    void INT_CloseZoneEditor()
    {
        if (!m_INT_ZoneEd)
            return;
        if (m_INT_Tab == INT_TAB_ZONES)
        {
            INT_SelectTab(INT_TAB_MAP);
            return;
        }
        m_INT_ZoneEd.INT_Destroy();
        m_INT_ZoneEd = null;
        INT_ApplyMode();
    }

    // ===================================================================== INT: label editor (2026-11-13)
    void INT_CloseLabelEditor()
    {
        if (!m_INT_LabelEd)
            return;
        if (m_INT_Tab == INT_TAB_LABELS)
        {
            INT_SelectTab(INT_TAB_MAP);
            return;
        }
        m_INT_LabelEd.INT_Destroy();
        m_INT_LabelEd = null;
        INT_ApplyMode();
    }

    // ===================================================================== INT: invokers (marks only)
    void INT_OnLabelsEvt()
    {
        if (m_INT_TornDown)
            return;
        if (m_INT_View)
            m_INT_View.INT_MarkLabels();
        if (m_INT_LabelEd)
            m_INT_LabelEd.INT_OnLabels();
    }

    void INT_OnZonesEvt()
    {
        if (m_INT_TornDown)
            return;
        if (m_INT_View)
            m_INT_View.INT_MarkZones();
        if (m_INT_ZoneEd)
            m_INT_ZoneEd.INT_OnZones();
        m_INT_DirtyIslands = true;
        m_INT_DirtyList = true;
    }

    void INT_OnSettingsEvt()
    {
        if (m_INT_TornDown)
            return;
        m_INT_DirtyAdmin = true;
        m_INT_DirtyIslands = true;
    }

    void INT_OnRouteEvt()
    {
        if (m_INT_TornDown)
            return;
        if (m_INT_View)
            m_INT_View.INT_MarkRoute();
        m_INT_DirtyRibbon = true;
        m_INT_DirtyIslands = true;
        m_INT_DirtyList = true;
        m_INT_CardsDirty = true;
    }

    void INT_OnPrefsEvt()
    {
        if (m_INT_TornDown)
            return;
        if (m_INT_View)
            m_INT_View.INT_MarkLayers();
        // the label preview follows the «Зоны» layer (gone when off, back when on)
        if (m_INT_LabelEd)
            m_INT_LabelEd.INT_PushDraft();
        m_INT_DirtyIslands = true;
        m_INT_DirtyList = true;
    }

    // ===================================================================== INT: test hooks (BUILD_PLAN §8)
    void INT_OpenEditorAt(vector worldPos)
    {
        if (!m_INT_Built || m_INT_TornDown)
            return;
        vector p = worldPos;
        p[1] = 0;
        INT_OpenEditorNew(p);
    }

    void INT_OpenZoneEditor()
    {
        if (!m_INT_Built || m_INT_TornDown)
            return;
        INT_VPP_Client cl = INT_VPP_Client.Peek();
        if (!cl || !cl.IsAdmin())
            return;
        m_INT_Admin = true;
        if (m_INT_Isl)
            m_INT_Isl.INT_SetAdmin(true);
        if (m_INT_Tab != INT_TAB_ZONES)
            INT_SelectTab(INT_TAB_ZONES);
    }

    // TEST + BUILD_PLAN §8 (2026-11-13): the admin label editor (tab «05 Надписи»); nothing for a non-admin
    void INT_OpenLabelEditor()
    {
        if (!m_INT_Built || m_INT_TornDown)
            return;
        INT_VPP_Client cl = INT_VPP_Client.Peek();
        if (!cl || !cl.IsAdmin())
            return;
        m_INT_Admin = true;
        if (m_INT_Isl)
            m_INT_Isl.INT_SetAdmin(true);
        if (m_INT_Tab != INT_TAB_LABELS)
            INT_SelectTab(INT_TAB_LABELS);
    }

    bool INT_IsLabelEditorOpen()
    {
        if (m_INT_LabelEd)
            return true;
        return false;
    }

    // TEST: the open label editor (null when closed)
    INT_VPP_LabelEditor INT_GetLabelEditor()
    {
        return m_INT_LabelEd;
    }

    void INT_SetRulerOn(bool on)
    {
        if (!m_INT_Built || m_INT_TornDown)
            return;
        if (on == m_INT_RulerOn)
            return;
        m_INT_RulerOn = on;
        m_INT_RulerPts.Clear();
        if (on)
        {
            INT_CloseCard();
            if (m_INT_Editor)
                INT_CloseEditor();
            if (m_INT_ZoneEd)
                INT_CloseZoneEditor();
            if (m_INT_LabelEd)
                INT_CloseLabelEditor();
        }
        INT_RulerChanged();
        INT_ApplyMode();
    }

    bool INT_IsRulerOn()
    {
        return m_INT_RulerOn;
    }

    bool INT_IsEditorOpen()
    {
        if (m_INT_Editor)
            return true;
        return false;
    }

    bool INT_IsZoneEditorOpen()
    {
        if (m_INT_ZoneEd)
            return true;
        return false;
    }

    INT_VPP_MapView INT_GetView()
    {
        return m_INT_View;
    }

    // TEST: the open zone editor (null when closed)
    INT_VPP_ZoneEditor INT_GetZoneEditor()
    {
        return m_INT_ZoneEd;
    }
};
