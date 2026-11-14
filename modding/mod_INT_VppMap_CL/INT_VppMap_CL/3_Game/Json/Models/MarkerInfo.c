class MarkerInfo {
    private string M_MARKER_NAME;
    private string M_ICON_PATH;
    private vector M_COLOR;
    private vector M_POSITION;
    private bool   M_ISACTIVE;
    private bool   M_IS_3D_ACTIVE;

    [NonSerialized()]
    private bool   m_3DForcedDisabled;

    //INT (2026-11-14): the player's own marker shows no name next to its 3D icon (create / edit card «Название в 3D»).
    //Last member, false = show the name: an old MapMarkersCache.json / VPPMapConfig.json without the key and every
    //constructor caller (RF bridge, DeathBox, VPPMapItemRequirements, the RF_MAP import) mean "show". Both sides of
    //the VPP RPCs run this class (the client PBO is loaded by the server too), so the RPC layout stays the same.
    private bool   INT_HideName3D;

    //INT (2026-11-14): id of a SERVER marker (VPPMapConfig.json), the address of an in-game admin edit / delete.
    //0 = none; the server gives every marker of its list an id before it sends the list (INT_VPP_SrvMarkers).
    //Personal markers keep 0. Missing in old files = 0.
    private int    INT_SrvId;

    void MarkerInfo(string name, string path, vector color, vector pos, bool isActive = true, bool is3DActive = false ) {
        M_MARKER_NAME  = name;
        M_ICON_PATH	   = path;
        M_COLOR		   = color;
        M_POSITION	   = pos;
        M_ISACTIVE     = isActive;
        M_IS_3D_ACTIVE = is3DActive;
    }

    void SetName(string newName) {
        M_MARKER_NAME = newName;
    }

    void SetIconPath(string newIcon) {
        M_ICON_PATH = newIcon;
    }

    void SetColor(vector newColor) {
        M_COLOR = newColor;
    }

    void SetPosition(vector newPos) {
        M_POSITION = newPos;
    }

    void SetActive(bool state) {
        M_ISACTIVE = state;
    }

    void Set3DActive(bool state) {
        M_IS_3D_ACTIVE = state;
    }

    void Set3DForcedDisabled(bool state){
        m_3DForcedDisabled = state;
    }

    string GetName() {
        return M_MARKER_NAME;
    }

    string GetIconPath() {
        return M_ICON_PATH;
    }

    vector GetColor() {
        return M_COLOR;
    }

    vector GetPosition() {
        return M_POSITION;
    }

    bool IsActive() {
        return M_ISACTIVE;
    }

    bool Is3DActive() {
        return M_IS_3D_ACTIVE;
    }

    bool Is3DForcedDisabled(){
        return m_3DForcedDisabled;
    }

    //INT (2026-11-14)
    void INT_SetName3DHidden(bool hide) {
        INT_HideName3D = hide;
    }

    bool INT_IsName3DHidden() {
        return INT_HideName3D;
    }

    void INT_SetSrvId(int id) {
        INT_SrvId = id;
    }

    int INT_GetSrvId() {
        return INT_SrvId;
    }
};
