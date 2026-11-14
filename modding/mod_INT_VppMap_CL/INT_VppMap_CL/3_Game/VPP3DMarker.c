// VPP 3D marker — INT «Экспедиция» B rework (HUD3D, 2026-11-12).
// Contract kept (RF bridge, DeathBox, VPPMapItemRequirements): class name, the 4-param constructor,
// SetMarkerVisible / IsMarkerVisible / Get3DMarkerPosition, DoUpdate (now a no-op) and the static helpers.
// New: optional 5th constructor parameter `intLayer` (INT_VPP_LAYER_*, -1 = not tagged → server layer; the
// manager resolves untagged markers of the server array to «События» when they are not our own).
// The constructor creates NO widgets any more: INT_VPP_3D (5_Mission, ONE loop for all markers) builds the
// tree lazily when the marker comes into range (INT_BuildTree) and unlinks it when it leaves / its layer or
// the master switch goes off / K hides it (INT_DropTree). SetMarkerVisible(false) drops the tree at once.
// The destructor unlinks a tree if any. Tree kind 6 in INT_VPP_Stat.
class VPP3DMarker extends UIScriptedMenu {
    private string           m_MarkerName;
    private string           m_iconPath;
    private	vector	         m_MarkerPositon;
    private vector           m_MarkerPosPLANE;
    private bool             m_IsMarkerVisible;

    // ---- INT (HUD3D) ----
    static const int INT_COL_AMBER = 0xFFFFC246;       // «События» accent (design X.W)
    static const int INT_COL_SUB = 0xFFE5E0D8;         // metres under the name
    static const int INT_COL_EVENT_SUB = 0xFFFFE1A0;
    static const int INT_COL_OUTLINE = 0xFF0B0B0B;     // 1 px text outline over the world
    static const int INT_COL_PAD = 0xB00B0B0B;         // soft dark pad behind the icon
    static const float INT_ROOT_W = 290;               // design px (1920x1080 canvas, scaled by INT_UIF)
    static const float INT_ROOT_H = 44;
    static const float INT_CX = 22;                    // icon centre inside the root = the projected point
    static const float INT_CY = 22;
    static const float INT_TEXT_X = 48;
    static const float INT_NAME_ROOM = 230;
    static const float INT_EDGE = 64;                  // screen px a marker may stick out before it is hidden

    protected int            m_INT_Layer;              // raw tag, -1 = untagged
    protected int            m_INT_Tint;               // 0 = layer colour
    protected int            m_INT_TreeLayer;          // layer the live tree was built for
    protected bool           m_INT_HasTree;
    protected Widget         m_INT_Root;               // weak (Widget is Managed)
    protected TextWidget     m_INT_DistText;
    protected TextWidget     m_INT_NameText;
    protected float          m_INT_LabelW;             // screen px of the name / metres block (de-clutter)
    protected bool           m_INT_LabelOn;
    protected float          m_INT_LastDist;
    protected int            m_INT_Seen;
    protected int            m_INT_LastKey;
    protected bool           m_INT_Shown;
    protected int            m_INT_LastPx;
    protected int            m_INT_LastPy;
    protected float          m_INT_AnchorX;
    protected float          m_INT_AnchorY;
    protected bool           m_INT_Resolved;
    // the MarkerInfo this marker shows (strong: DayZGame reuses ONE marker per MarkerInfo and compares by identity)
    protected ref MarkerInfo m_INT_Src;
    // 3D look of the live tree (INT_VPP_Prefs size, 2026-11-12): every design size / offset x m_INT_K
    protected float          m_INT_K = 1.0;
    protected float          m_INT_Edge = 64;          // INT_EDGE x m_INT_K (screen px)
    // texts never below this share of the design size (pad / diamond / icon still follow k)
    static const float INT_TEXT_FLOOR = 0.75;
    // label rect offsets from the root corner (screen px) and the name's design size of the live tree
    protected float          m_INT_LabelX = 48;
    protected float          m_INT_LabelY1 = 3;
    protected float          m_INT_LabelY2 = 40;
    protected float          m_INT_NameSize = 16;
    // 2026-11-14: this marker's own «Название в 3D» (own markers only; read from its MarkerInfo) and whether the live
    // tree has any text at all (no texts = nothing for the label de-clutter)
    protected bool           m_INT_NameHidden;
    protected bool           m_INT_HasLabel = true;

    // outputs of INT_DrawLook for its caller (weak; read right after the call, then cleared)
    static TextWidget        s_INT_OutName;
    static TextWidget        s_INT_OutDist;
    static float             s_INT_OutLabelW;
    static float             s_INT_OutLabelX;
    static float             s_INT_OutLabelY1;
    static float             s_INT_OutLabelY2;
    static float             s_INT_OutNameSize;
    static bool              s_INT_OutHasLabel;

    void VPP3DMarker( string MarkerName, string iconPath, vector MarkerPos, bool visible, int intLayer = -1 ) {
        m_MarkerPosPLANE   = MarkerPos;
        m_MarkerName  	   = MarkerName;
        m_iconPath 	       = iconPath;
        vector SnappedPos  = SnapToGround( Vector(MarkerPos[0], MarkerPos[1], MarkerPos[2]) );
        m_MarkerPositon    = Vector(SnappedPos[0], SnappedPos[1] + 1, SnappedPos[2]);
        m_IsMarkerVisible  = visible;

        m_INT_Layer        = intLayer;
        m_INT_Tint         = 0;
        m_INT_TreeLayer    = -1;
        m_INT_LastKey      = -1;
        m_INT_LastPx       = -100000;
        m_INT_LastPy       = -100000;
        if (intLayer >= 0)
            m_INT_Resolved = true;
    }

    void ~VPP3DMarker() {
        INT_DropTree();
    }

    float CalcDistance() {
        if (!GetGame() || !GetGame().GetPlayer())
            return 0;
        return vector.Distance( GetGame().GetPlayer().GetPosition(), m_MarkerPositon );
    }

    vector Get3DMarkerPosition() {
        return m_MarkerPosPLANE;
    }

    // Kept for old callers: drawing is done by INT_VPP_3D (one loop for all markers), nothing to do here.
    void DoUpdate() {
    }

    bool IsMarkerVisible() {
        return m_IsMarkerVisible;
    }

    void SetMarkerVisible(bool visible) {
        /*if (!g_Game.CanUse3DMarkers() && visible)
        {
            m_IsMarkerVisible = false;
            return;
        }*/
        m_IsMarkerVisible = visible;
        //INT: hidden = no widget tree (K hides all through here)
        if (!visible)
            INT_DropTree();
    }

    // ------------------------------------------------------------------ INT accessors
    // raw tag: -1 = untagged
    int INT_GetRawLayer() {
        return m_INT_Layer;
    }

    // effective layer: untagged = server
    int INT_GetLayer() {
        if (m_INT_Layer < 0)
            return INT_VPP_LAYER_SERVER;
        return m_INT_Layer;
    }

    // tag a marker (Display3dMarkers / the manager's resolution of untagged ones)
    void INT_SetLayer(int layer) {
        m_INT_Layer = layer;
        m_INT_Resolved = true;
        if (m_INT_HasTree && m_INT_TreeLayer != INT_GetLayer())
            INT_DropTree();
    }

    // true once the layer is known for sure (tagged at creation or resolved by the manager)
    bool INT_IsResolved() {
        return m_INT_Resolved;
    }

    void INT_MarkResolved() {
        m_INT_Resolved = true;
    }

    // icon colour for «Мои» (ARGB; 0 = layer colour)
    void INT_SetTint(int argb) {
        if (m_INT_Tint == argb)
            return;
        m_INT_Tint = argb;
        if (m_INT_HasTree)
            INT_DropTree();
    }

    int INT_GetTint() {
        return m_INT_Tint;
    }

    string INT_GetName() {
        return m_MarkerName;
    }

    // ------------------------------------------------------------------ INT reuse (review #6)
    void INT_SetSource(MarkerInfo m) {
        m_INT_Src = m;
    }

    MarkerInfo INT_GetSource() {
        return m_INT_Src;
    }

    // own marker's «Название в 3D» off (2026-11-14): a change drops the live tree (rebuilt lazily without the name)
    void INT_SetNameHidden(bool hide) {
        if (hide == m_INT_NameHidden)
            return;
        m_INT_NameHidden = hide;
        if (m_INT_HasTree)
            INT_DropTree();
    }

    bool INT_IsNameHidden() {
        return m_INT_NameHidden;
    }

    // false = the live tree has neither a name nor metres (both switched off): no label rect for the de-clutter
    bool INT_HasLabel() {
        return m_INT_HasLabel;
    }

    // In-place update (DayZGame reuses the marker of a MarkerInfo): a new position moves the live tree, only a new
    // look (name, icon, layer, tint) drops it (the manager rebuilds it in range). true = something changed.
    bool INT_Update(string name, string icon, vector pos, int layer, int tint) {
        bool look = false;
        if (name != m_MarkerName || icon != m_iconPath)
            look = true;
        if (layer != m_INT_Layer || tint != m_INT_Tint)
            look = true;
        m_MarkerName = name;
        m_iconPath = icon;
        m_INT_Layer = layer;
        if (layer >= 0)
            m_INT_Resolved = true;
        m_INT_Tint = tint;
        bool moved = false;
        if (vector.DistanceSq(pos, m_MarkerPosPLANE) > 0.0001)
            moved = true;
        if (moved) {
            m_MarkerPosPLANE = pos;
            vector snapped = SnapToGround(Vector(pos[0], pos[1], pos[2]));
            m_MarkerPositon = Vector(snapped[0], snapped[1] + 1, snapped[2]);
            m_INT_LastKey = -1;
        }
        if (look && m_INT_HasTree)
            INT_DropTree();
        if (look || moved)
            return true;
        return false;
    }

    string INT_GetIconPath() {
        return m_iconPath;
    }

    // the point the marker is drawn at (ground + 1 m, VPP's snap)
    vector INT_GetDrawPos() {
        return m_MarkerPositon;
    }

    void INT_MarkSeen(int pass) {
        m_INT_Seen = pass;
    }

    int INT_SeenPass() {
        return m_INT_Seen;
    }

    bool INT_HasTree() {
        // the engine removed the widgets under us (mission end): fix the books
        if (m_INT_HasTree && !m_INT_Root)
            INT_DropTree();
        return m_INT_HasTree;
    }

    // ------------------------------------------------------------------ INT tree
    void INT_DropTree() {
        if (!m_INT_HasTree)
            return;
        Widget root = m_INT_Root;
        if (root)
            root.Unlink();
        m_INT_Root = null;
        m_INT_DistText = null;
        m_INT_NameText = null;
        m_INT_LabelOn = false;
        m_INT_HasTree = false;
        m_INT_TreeLayer = -1;
        m_INT_Shown = false;
        m_INT_LastKey = -1;
        m_INT_LastPx = -100000;
        m_INT_LastPy = -100000;
        INT_VPP_Stat.INT_Down(INT_VPP_Stat.INT_KIND_3D);
    }

    // «Экспедиция» look: dark pad + (server / events) outline diamond + icon, name and metres to the right.
    // Size and transparency from INT_VPP_Prefs (2026-11-12; 100 % / 0 % = the look before). A change of either
    // makes INT_VPP_3D drop every tree; this rebuilds them through the normal lazy path.
    void INT_BuildTree(int layer) {
        if (m_INT_HasTree)
            INT_DropTree();
        if (!GetGame() || GetGame().IsDedicatedServer() || !GetGame().GetWorkspace())
            return;
        float k = 1.0;
        float alphaK = 1.0;
        bool showName = true;
        bool showDist = true;
        INT_VPP_Prefs pr = INT_VPP_Prefs.INT_Get();
        if (pr) {
            k = pr.INT_3DScaleK();
            alphaK = pr.INT_3DAlphaK();
            showName = pr.INT_Is3DNamesOn();
            showDist = pr.INT_Is3DDistOn();
        }
        // names (2026-11-14): «Названия» of «ВИД 3D-МЕТОК» for every marker, AND the marker's own «Название в 3D»
        // (own markers; server markers when the admin turned it off; never events)
        if ((layer == INT_VPP_LAYER_MINE || layer == INT_VPP_LAYER_SERVER) && m_INT_NameHidden)
            showName = false;
        Widget root = VPP3DMarker.INT_DrawLook(null, 0, 0, k, alphaK, layer, m_iconPath, m_INT_Tint, m_MarkerName, "0 м", "INT_VPP_3DMarker", showName, showDist);
        bool hasLabel = s_INT_OutHasLabel;
        TextWidget nameText = s_INT_OutName;
        TextWidget distText = s_INT_OutDist;
        float labelW = s_INT_OutLabelW;
        float labelX = s_INT_OutLabelX;
        float labelY1 = s_INT_OutLabelY1;
        float labelY2 = s_INT_OutLabelY2;
        float nameSize = s_INT_OutNameSize;
        s_INT_OutName = null;
        s_INT_OutDist = null;
        if (!root)
            return;
        m_INT_LabelX = labelX;
        m_INT_LabelY1 = labelY1;
        m_INT_LabelY2 = labelY2;
        m_INT_NameSize = nameSize;
        m_INT_Root = root;
        m_INT_HasTree = true;
        m_INT_TreeLayer = layer;
        INT_VPP_Stat.INT_Up(INT_VPP_Stat.INT_KIND_3D);
        root.Show(false);
        m_INT_Shown = false;
        m_INT_LastKey = -1;
        m_INT_LastPx = -100000;
        m_INT_LastPy = -100000;
        m_INT_K = k;
        m_INT_Edge = INT_EDGE * k;
        m_INT_AnchorX = INT_UIF_Draw.S(INT_CX * k);
        m_INT_AnchorY = INT_UIF_Draw.S(INT_CY * k);
        m_INT_DistText = distText;
        m_INT_NameText = nameText;
        m_INT_LabelW = labelW;
        m_INT_LabelOn = true;
        m_INT_HasLabel = hasLabel;
    }

    // The look of one 3D marker at size k (1 = design) and alpha k (1 = as designed), drawn into `parent` at (x, y)
    // design px (parent null = a top-level root on the workspace). Shared by the world markers and the preview in
    // the map's 3D popover. Outputs: s_INT_OutName / s_INT_OutDist (texts) and s_INT_OutLabelW (screen px).
    // showName / showDist (2026-11-14, «Названия» / «Расстояние»): without the name the metres take its place next to
    // the icon (centred on it); without both only the pad / diamond / icon (s_INT_OutHasLabel false).
    static Widget INT_DrawLook(Widget parent, float x, float y, float k, float alphaK, int layer, string iconPath, int tint, string name, string dist, string rootName, bool showName = true, bool showDist = true) {
        s_INT_OutName = null;
        s_INT_OutDist = null;
        s_INT_OutLabelW = 0;
        s_INT_OutHasLabel = false;
        Widget root = INT_UIF_Draw.Frame(parent, x, y, INT_ROOT_W * k, INT_ROOT_H * k, rootName);
        if (!root)
            return null;
        int accent = INT_VPPX_Look.WHITE;
        int nameColor = INT_VPPX_Look.WHITE;
        int subColor = INT_COL_SUB;
        if (layer == INT_VPP_LAYER_EVENTS) {
            accent = INT_COL_AMBER;
            nameColor = INT_COL_AMBER;
            subColor = INT_COL_EVENT_SUB;
        } else if (layer == INT_VPP_LAYER_MINE) {
            if (tint != 0)
                accent = tint;
        }

        //INT (2026-11-13): empty or not loadable (RF configs naming textures no mod has) -> our icon by name / words;
        //a path that loads is drawn as it is (INT_VPP_IconFallback)
        string icon = INT_VPP_IconFallback.INT_Resolve(iconPath, name);
        bool ownIcon = INT_VPP_Icons.INT_IsOwnPath(icon);
        int iconColor = accent;
        // legacy / foreign textures are drawn as they are (server / events); «Мои» keep their colour like the 2D map
        if (!ownIcon && layer != INT_VPP_LAYER_MINE)
            iconColor = 0xFFFFFFFF;

        float cx = INT_CX * k;
        float cy = INT_CY * k;
        float padHalf = 21 * k;
        float padSide = 42 * k;
        string pad = INT_VPP_Icons.INT_UI_MARKBACK;
        INT_UIF_Draw.Texture(root, cx - padHalf, cy - padHalf, padSide, padSide, pad, INT_COL_PAD);
        float iconSize = 24;
        if (layer == INT_VPP_LAYER_EVENTS) {
            INT_VPPX_Look.Diamond(root, cx, cy, 21 * k, false, accent);
            iconSize = 16;
        } else if (layer != INT_VPP_LAYER_MINE) {
            INT_VPPX_Look.Diamond(root, cx, cy, 24 * k, false, accent);
            iconSize = 18;
        }
        iconSize = iconSize * k;
        float half = iconSize * 0.5;
        INT_UIF_Draw.Texture(root, cx - half, cy - half, iconSize, iconSize, icon, iconColor);

        // text floor (coordinator 2026-11-12): the texts never go below INT_TEXT_FLOOR of the design size (8 px names at
        // 50 % were unreadable); the pad / diamond / icon keep k. At k >= the floor nothing changes (kt = k, the old
        // formulas). Below it the texts start at the icon's right edge + a gap and are centred on the icon.
        float kt = k;
        if (kt < INT_TEXT_FLOOR)
            kt = INT_TEXT_FLOOR;
        float textX = INT_TEXT_X * k;
        float nameY = 3 * k;
        float distY = 23 * k;
        float labelY2 = 40 * k;
        if (kt > k) {
            textX = (INT_CX + 21) * k + 5 * kt;
            nameY = cy - 19 * kt;
            distY = cy + kt;
            labelY2 = cy + 18 * kt;
        }
        // the name is fitted at the design size: the room scales with the text, so the cut is the same at any size
        string shown = INT_VPPX_Look.FitText(name, 16, false, INT_NAME_ROOM);
        float nameRoomW = (INT_NAME_ROOM + 10) * kt;
        float nameSize = 16 * kt;
        TextWidget nameText = null;
        if (showName) {
            nameText = INT_UIF_Draw.Text(root, textX, nameY, nameRoomW, 20 * kt, shown, nameSize, nameColor, INT_UIF_Align.LEFT);
            if (nameText)
                nameText.SetOutline(1, INT_COL_OUTLINE);
        }
        TextWidget distText = null;
        float labelY1 = nameY;
        if (showDist) {
            // the metres alone sit level with the icon's centre
            if (!showName) {
                distY = cy - 9 * kt;
                labelY1 = distY;
                labelY2 = distY + 17 * kt;
            }
            distText = INT_UIF_Draw.Text(root, textX, distY, 160 * kt, 17 * kt, dist, 14 * kt, subColor, INT_UIF_Align.LEFT);
            if (distText)
                distText.SetOutline(1, INT_COL_OUTLINE);
        } else if (showName) {
            labelY2 = nameY + 20 * kt;
        }
        float nameW = 60;
        if (showName)
            nameW = INT_VPPX_Look.Measure(shown, 16);
        if (nameW < 60)
            nameW = 60;
        if (showName || showDist)
            s_INT_OutHasLabel = true;
        s_INT_OutLabelW = INT_UIF_Draw.S(nameW * kt);
        // the de-clutter rect = what is drawn (offsets from the root's corner, screen px)
        s_INT_OutLabelX = INT_UIF_Draw.S(textX);
        s_INT_OutLabelY1 = INT_UIF_Draw.S(labelY1);
        s_INT_OutLabelY2 = INT_UIF_Draw.S(labelY2);
        s_INT_OutNameSize = nameSize;
        s_INT_OutName = nameText;
        s_INT_OutDist = distText;
        // transparency: every widget's own alpha (a frame's alpha does not reach its children)
        INT_VPPX_Look.INT_FadeTree(root, alphaK);
        return root;
    }

    // TEST / tools: the live tree's root (null without a tree) and its size factor
    Widget INT_GetRoot() {
        return m_INT_Root;
    }

    float INT_GetScaleK() {
        return m_INT_K;
    }

    // ------------------------------------------------------------------ INT label de-clutter (INT_VPP_3D, 4 Hz)
    bool INT_IsShownNow() {
        if (m_INT_HasTree && m_INT_Shown)
            return true;
        return false;
    }

    float INT_LastDist() {
        return m_INT_LastDist;
    }

    // screen rect of the name + metres block (x1, y1, x2, y2), from the last placed position
    void INT_LabelRect(out float x1, out float y1, out float x2, out float y2) {
        x1 = m_INT_LastPx + m_INT_LabelX;
        y1 = m_INT_LastPy + m_INT_LabelY1;
        x2 = x1 + m_INT_LabelW;
        y2 = m_INT_LastPy + m_INT_LabelY2;
    }

    // TEST: design size of the name text of the live tree; the name / metres text widgets
    float INT_GetNameSize() {
        return m_INT_NameSize;
    }

    TextWidget INT_GetNameText() {
        return m_INT_NameText;
    }

    bool INT_IsLabelOn() {
        return m_INT_LabelOn;
    }

    // the icon stays; only the name and the metres hide
    void INT_SetLabelOn(bool on) {
        if (m_INT_LabelOn == on)
            return;
        m_INT_LabelOn = on;
        if (m_INT_NameText)
            m_INT_NameText.Show(on);
        if (m_INT_DistText)
            m_INT_DistText.Show(on);
    }

    // hide the tree without dropping it (another menu is on top)
    void INT_Hide() {
        Widget root = m_INT_Root;
        if (root && m_INT_Shown) {
            root.Show(false);
            m_INT_Shown = false;
        }
    }

    // per frame, only for markers that have a tree (the manager culls by distance first)
    void INT_Frame(vector playerPos, float screenW, float screenH) {
        if (!m_INT_HasTree)
            return;
        Widget root = m_INT_Root;
        if (!root)
            return;
        vector sp = GetGame().GetScreenPos(m_MarkerPositon);
        bool on = true;
        if (sp[2] <= 0) {
            on = false;
        } else if (sp[0] < -m_INT_Edge || sp[0] > screenW + m_INT_Edge) {
            on = false;
        } else if (sp[1] < -m_INT_Edge || sp[1] > screenH + m_INT_Edge) {
            on = false;
        }
        if (on != m_INT_Shown) {
            root.Show(on);
            m_INT_Shown = on;
        }
        if (!on)
            return;
        int px = Math.Round(sp[0] - m_INT_AnchorX);
        int py = Math.Round(sp[1] - m_INT_AnchorY);
        if (px != m_INT_LastPx || py != m_INT_LastPy) {
            root.SetPos(px, py);
            m_INT_LastPx = px;
            m_INT_LastPy = py;
        }
        float d = INT_VPP_Metres.INT_Dist2D(playerPos, m_MarkerPositon);
        m_INT_LastDist = d;
        int key = INT_VPP_Metres.INT_MetresKey(d);
        if (key != m_INT_LastKey) {
            m_INT_LastKey = key;
            if (m_INT_DistText)
                m_INT_DistText.SetText(INT_VPP_Metres.INT_MetresText(key));
        }
    }


    /**************************************
    /********** Utility methods /**********
    /**************************************

    /*
    	Convert string to vector format
    */
    static vector StringToVector(string str) {
        vector oV = "0 0 0";
        TStringArray strs = new TStringArray;
        str.Replace("<","");
        str.Replace(">","");
        str.Replace(","," ");

        str.Split( " ", strs );

        oV = Vector(strs.Get(0).ToFloat(),strs.Get(1).ToFloat(),strs.Get(2).ToFloat());
        return oV;
    }

    /*
    	Convert vector to string format
    */
    static string VectorToString(vector vctr) {
        string oStr;
        oStr = vctr[0].ToString();
        oStr += "," + vctr[1].ToString();
        oStr += "," + vctr[2].ToString();
        return oStr;
    }

    static vector SnapToGround(vector pos) {
        float pos_x = pos[0];
        float pos_z = pos[2];
        float pos_y = GetGame().SurfaceY(pos_x, pos_z);
        vector tmp_pos = Vector(pos_x, pos_y, pos_z);
        tmp_pos[1] = tmp_pos[1] + pos[1];

        return tmp_pos;
    }
}
