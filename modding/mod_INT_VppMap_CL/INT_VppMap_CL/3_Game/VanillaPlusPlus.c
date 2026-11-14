modded class DayZGame {
    /* Global Vars [These are always controled even when player leaves session.] */
    private bool m_IsKeyboardBusy;
    private bool m_CanUse3DMarkers = true;
    private bool m_OwnPositionMarkerDisabled = false;
    private ref ClientMarkersCache m_ClientMarkersCache;
    private ref array<ref VPP3DMarker> m_TempHidden3dMarkers;
    private ref array<ref VPP3DMarker> m_Client3dMarkers;
    private ref array<ref VPP3DMarker> m_Server3dMarkers; //Static 3Dmarkers we get from server upon connect.
    private ref array<ref MarkerInfo> m_ServerMarkers; //Static markers we get from server upon connect.

    private bool m_Use3dMarkersSet;
    private bool m_ServerMarkersSet;
    private bool m_ForcePhysicalMap;

    //INT (HUD3D, additive): the markers that came from our own SetServerMarkers (anything else in the server
    //array = RF bridge / other mods → layer «События»), a revision the 3D manager watches and the signature of
    //the server array (the manager polls it once a second: RF inserts at any time, no map needs to be open)
    private ref array<ref MarkerInfo> m_INT_VPP_OwnServerMarkers;
    private int m_INT_VPP_3DRev;
    private int m_INT_VPP_SrvSig;
    private bool m_INT_VPP_SrvSigValid;
    //INT (review #8): MapMarkersCache.json is read once per session (in-session saves go through the same
    //in-memory cache; a reload per map open only threw the MarkerInfo objects away)
    private bool m_INT_VPP_CacheLoaded;
    private int m_INT_VPP_CacheLoads;

    void DayZGame() {
        Print("DZM_Projects Map! by DaOne");
        m_ClientMarkersCache = new ClientMarkersCache();
        m_Client3dMarkers = new array<ref VPP3DMarker>;
        m_Server3dMarkers = new array<ref VPP3DMarker>;
        m_ServerMarkers = new array<ref MarkerInfo>;
    }

    bool IsKeyboardBusy() {
        return m_IsKeyboardBusy;
    }

    void SetKeyboardBusy(bool isKeyboardBusy) {
        m_IsKeyboardBusy = isKeyboardBusy;
    }

    ref array<ref MarkerInfo> GetServerMarkers() {
        return m_ServerMarkers;
    }

    void SetServerMarkers(ref array<ref MarkerInfo> staticMarkers) {
        m_ServerMarkers = staticMarkers;
        //INT (HUD3D): remember which markers are ours before anyone inserts into the array
        INT_VPP_RememberOwn(staticMarkers);
        foreach(ref MarkerInfo mkr: m_ServerMarkers)
        {
            mkr.Set3DForcedDisabled(!mkr.Is3DActive());
        }
        m_ServerMarkersSet = true;
        Display3dMarkers();
    }

    bool CanUse3DMarkers() {
        return m_CanUse3DMarkers;
    }

    void SetUse3DMarkers(bool canUse3DMarkers) {
        m_CanUse3DMarkers = canUse3DMarkers;
        m_Use3dMarkersSet = true;
        Display3dMarkers();
    }

    bool OwnPositionMarkerDisabled() {
        return m_OwnPositionMarkerDisabled;
    }

    void SetOwnPositionMarkerDisabled(bool ownPositionMarkerDisabled) {
        m_OwnPositionMarkerDisabled = ownPositionMarkerDisabled;
    }

    void SetForcePhysicalMap(bool state){
        m_ForcePhysicalMap = state;
    }

    bool ForcedPhysicalMap(){
        return m_ForcePhysicalMap;
    }

    void Display3dMarkers() {
        if (m_ServerMarkersSet && m_Use3dMarkersSet) {
            ClientMarkersCache cache = GetClientMarkersCache();
            if (cache) {
                //INT: the saved json once per session (raw edits made between sessions), not on every call
                INT_VPP_LoadCacheOnce();
                array<ref MarkerInfo> clientMarkers = cache.GetCachedArray();
                //INT (2026-11-13): this server's own personal list too (VPP's per-server markers, e.g. markers
                //imported from RF_MAP), like the map's DisplayClientMarkers - before, they had 3D only while the map
                //had been opened and lost it again at the next call here
                array<ref MarkerInfo> perServer = null;
                string intAddr = GetConnectAddressPort();
                if (intAddr != "" && !IsDedicatedServer())
                    perServer = cache.GetCachedArray(intAddr);
                //INT (review #6): the 3D marker of every MarkerInfo is reused / updated in place (no delete-all /
                //rebuild-all: no blink, no tree churn); «Мои» tagged with their colour, own server → «Сервер»,
                //RF / other → «События»
                INT_VPP_SyncClient3d(clientMarkers, perServer);
                INT_VPP_RebuildServer3d();
            }
        }
    }

    // ===================================================================== INT (HUD3D, additive)
    // true = this MarkerInfo came from our own SetServerMarkers; false = RF bridge / other mod → «События»
    bool INT_VPP_IsOwnServerMarker(MarkerInfo m) {
        if (!m || !m_INT_VPP_OwnServerMarkers)
            return false;
        if (m_INT_VPP_OwnServerMarkers.Find(m) >= 0)
            return true;
        return false;
    }

    // INT_VPP_LAYER_SERVER for our own server markers, INT_VPP_LAYER_EVENTS for everything else
    int INT_VPP_ServerLayerOf(MarkerInfo m) {
        if (INT_VPP_IsOwnServerMarker(m))
            return INT_VPP_LAYER_SERVER;
        return INT_VPP_LAYER_EVENTS;
    }

    // a VPP3DMarker for a MarkerInfo, tagged with its layer (and its colour for «Мои»); MAP may use it too
    VPP3DMarker INT_VPP_Make3D(MarkerInfo m, int layer, bool visible) {
        if (!m)
            return null;
        string nm = m.GetName();
        string ic = m.GetIconPath();
        vector pos = m.GetPosition();
        VPP3DMarker mk = new VPP3DMarker(nm, ic, pos, visible, layer);
        if (layer == INT_VPP_LAYER_MINE) {
            int tint = INT_VPP_ColorOf(m);
            mk.INT_SetTint(tint);
        }
        //INT (2026-11-14): «Название в 3D» of an own / server marker
        if (layer == INT_VPP_LAYER_MINE || layer == INT_VPP_LAYER_SERVER) {
            bool hideName = m.INT_IsName3DHidden();
            mk.INT_SetNameHidden(hideName);
        }
        return mk;
    }

    // marker colour (vector 0..255) as opaque ARGB
    static int INT_VPP_ColorOf(MarkerInfo m) {
        if (!m)
            return 0xFFFFFFFF;
        vector c = m.GetColor();
        int r = Math.Round(c[0]);
        int g = Math.Round(c[1]);
        int b = Math.Round(c[2]);
        r = Math.Clamp(r, 0, 255);
        g = Math.Clamp(g, 0, 255);
        b = Math.Clamp(b, 0, 255);
        return ARGB(255, r, g, b);
    }

    // +1 whenever the 3D arrays were rebuilt or K changed (the 3D manager re-culls at once)
    int INT_VPP_3DRevision() {
        return m_INT_VPP_3DRev;
    }

    // K state: true while TempToggleAll3dMarkers keeps everything hidden
    bool INT_VPP_Is3DTempHidden() {
        if (m_TempHidden3dMarkers)
            return true;
        return false;
    }

    // layer of an UNTAGGED marker of the server 3D array (old callers that use the 4-param constructor):
    // looks its MarkerInfo up by name + position once; -1 when not found
    int INT_VPP_ResolveServerLayer(VPP3DMarker mk) {
        if (!mk)
            return -1;
        array<ref MarkerInfo> list = GetServerMarkers();
        if (!list)
            return -1;
        string nm = mk.INT_GetName();
        vector pos = mk.Get3DMarkerPosition();
        int n = list.Count();
        for (int i = 0; i < n; i++) {
            MarkerInfo m = list.Get(i);
            if (!m)
                continue;
            vector mp = m.GetPosition();
            if (vector.DistanceSq(mp, pos) > 0.01)
                continue;
            string mn = m.GetName();
            if (mn != nm)
                continue;
            return INT_VPP_ServerLayerOf(m);
        }
        return -1;
    }

    // the 3D manager calls it once a second: the RF bridge inserts into / removes from GetServerMarkers() at any
    // time; a changed signature (count, names, icons, positions, flags) rebuilds the server 3D markers.
    // true = rebuilt
    bool INT_VPP_CheckServerMarkers() {
        if (!m_ServerMarkersSet || !m_Use3dMarkersSet)
            return false;
        int sig = INT_VPP_ServerSignature();
        if (m_INT_VPP_SrvSigValid && sig == m_INT_VPP_SrvSig)
            return false;
        INT_VPP_RebuildServer3d();
        return true;
    }

    // the server half of Display3dMarkers (same filter, tagged); the marker of a MarkerInfo is reused and updated
    // in place (an RF marker moved by SetPosition moves its live tree), only markers that left the list go
    void INT_VPP_RebuildServer3d() {
        array<ref VPP3DMarker> old = m_Server3dMarkers;
        array<ref VPP3DMarker> made = new array<ref VPP3DMarker>;
        array<ref MarkerInfo> list = GetServerMarkers();
        INT_VPP_Collect3d(list, old, made, false);
        m_Server3dMarkers = made;
        INT_VPP_Drop3d(old);
        INT_VPP_NoteServerMarkers();
    }

    // personal markers (+ this server's own list; null = none): reused / updated in place like the server ones;
    // nothing at all while the server forbids 3D markers
    void INT_VPP_SyncClient3d(array<ref MarkerInfo> mine, array<ref MarkerInfo> perServer) {
        array<ref VPP3DMarker> old = m_Client3dMarkers;
        array<ref VPP3DMarker> made = new array<ref VPP3DMarker>;
        if (CanUse3DMarkers()) {
            INT_VPP_Collect3d(mine, old, made, true);
            INT_VPP_Collect3d(perServer, old, made, true);
        }
        m_Client3dMarkers = made;
        INT_VPP_Drop3d(old);
    }

    // MapMarkersCache.json: once per session (EndSession re-arms it). Everything in-session goes through the
    // same in-memory ClientMarkersCache, so its arrays (and the MarkerInfo objects the 3D markers point at) stay.
    void INT_VPP_LoadCacheOnce() {
        if (m_INT_VPP_CacheLoaded)
            return;
        ClientMarkersCache cache = GetClientMarkersCache();
        if (!cache)
            return;
        cache.LoadCache();
        m_INT_VPP_CacheLoaded = true;
        m_INT_VPP_CacheLoads++;
    }

    // self-test: json reads by INT_VPP_LoadCacheOnce since game start
    int INT_VPP_CacheLoads() {
        return m_INT_VPP_CacheLoads;
    }

    // mission finish / server switch (review #4): nothing of the old server survives into the next session - its
    // markers (own + RF events), flags, K state, signature; the marker json is read again at the next session
    void INT_VPP_EndSession() {
        m_TempHidden3dMarkers = null;
        m_INT_VPP_SrvSigValid = false;
        m_INT_VPP_3DRev++;
        m_ServerMarkers = new array<ref MarkerInfo>;
        m_INT_VPP_OwnServerMarkers = null;
        m_ServerMarkersSet = false;
        m_Use3dMarkersSet = false;
        m_ForcePhysicalMap = false;
        m_OwnPositionMarkerDisabled = false;
        m_CanUse3DMarkers = true;
        m_IsKeyboardBusy = false;
        m_INT_VPP_CacheLoaded = false;
    }

    // reuses (updated in place) or makes the 3D marker of every MarkerInfo of `list` into `made`; a reused one is
    // taken out of `old` (set to null there). Client lists: «Мои», visible = Is3DActive; server list: 3D-active
    // and not forced off, own = «Сервер», other = «События».
    protected void INT_VPP_Collect3d(array<ref MarkerInfo> list, array<ref VPP3DMarker> old, array<ref VPP3DMarker> made, bool client) {
        if (!list)
            return;
        int cursor = 0;
        int n = list.Count();
        for (int i = 0; i < n; i++) {
            MarkerInfo m = list.Get(i);
            if (!m)
                continue;
            int layer = INT_VPP_LAYER_MINE;
            bool visible = true;
            if (client) {
                visible = m.Is3DActive();
            } else {
                //INT (2026-11-12): the player's mode of the marker (Client.json), capped by the server's 3D flags
                int smode = INT_VPP_SrvMode(m);
                if (smode != INT_VPP_MODE_3D)
                    continue;
                layer = INT_VPP_ServerLayerOf(m);
            }
            int at = INT_VPP_Find3d(old, m, cursor);
            if (at >= 0) {
                VPP3DMarker kept = old.Get(at);
                made.Insert(kept);
                old.Set(at, null);
                cursor = at + 1;
                INT_VPP_Refresh3d(kept, m, layer, visible);
                continue;
            }
            VPP3DMarker mk = INT_VPP_Make3D(m, layer, visible);
            if (!mk)
                continue;
            mk.INT_SetSource(m);
            made.Insert(mk);
        }
    }

    // index of the marker made for `m` in `old` (the lists usually keep their order: the cursor hits first), -1
    protected int INT_VPP_Find3d(array<ref VPP3DMarker> old, MarkerInfo m, int cursor) {
        if (!old)
            return -1;
        int n = old.Count();
        if (cursor >= 0 && cursor < n) {
            VPP3DMarker c = old.Get(cursor);
            if (c && c.INT_GetSource() == m)
                return cursor;
        }
        for (int j = 0; j < n; j++) {
            VPP3DMarker o = old.Get(j);
            if (o && o.INT_GetSource() == m)
                return j;
        }
        return -1;
    }

    protected void INT_VPP_Refresh3d(VPP3DMarker mk, MarkerInfo m, int layer, bool visible) {
        int tint = 0;
        if (layer == INT_VPP_LAYER_MINE)
            tint = INT_VPP_ColorOf(m);
        string nm = m.GetName();
        string ic = m.GetIconPath();
        vector pos = m.GetPosition();
        mk.INT_Update(nm, ic, pos, layer, tint);
        //INT (2026-11-14): «Название в 3D» of an own marker or of a server marker (an admin's choice in
        //VPPMapConfig.json); events never hide their names. A change rebuilds the tree.
        bool hideName = false;
        if (layer == INT_VPP_LAYER_MINE || layer == INT_VPP_LAYER_SERVER)
            hideName = m.INT_IsName3DHidden();
        mk.INT_SetNameHidden(hideName);
        // K hides everything: a hidden marker stays hidden (K restores it) unless it should not be shown at all
        bool inK = false;
        if (m_TempHidden3dMarkers && m_TempHidden3dMarkers.Find(mk) >= 0)
            inK = true;
        if (inK) {
            if (!visible)
                m_TempHidden3dMarkers.RemoveItem(mk);
            return;
        }
        if (mk.IsMarkerVisible() != visible)
            mk.SetMarkerVisible(visible);
    }

    // markers that left the lists: their trees go now, the K list forgets them, the refs drop with the array
    // (no `delete`: the K list may still hold the same object - review #14)
    protected void INT_VPP_Drop3d(array<ref VPP3DMarker> old) {
        if (!old)
            return;
        int n = old.Count();
        for (int i = 0; i < n; i++) {
            VPP3DMarker o = old.Get(i);
            if (!o)
                continue;
            o.INT_DropTree();
            if (m_TempHidden3dMarkers)
                m_TempHidden3dMarkers.RemoveItem(o);
        }
        old.Clear();
    }

    // ---- map mode of a server / event marker (chips of «МЕТКИ НА КАРТЕ», 2026-11-12) ----
    // 3D allowed: the server allows 3D markers at all and this marker's own 3D flag is on
    bool INT_VPP_SrvCan3D(MarkerInfo m) {
        if (!m || !CanUse3DMarkers())
            return false;
        if (!m.Is3DActive() || m.Is3DForcedDisabled())
            return false;
        return true;
    }

    // the key of its override in Client.json (own server marker: name + position; event: name + icon)
    string INT_VPP_SrvModeKey(MarkerInfo m) {
        string server = GetConnectAddressPort();
        if (INT_VPP_IsOwnServerMarker(m))
            return INT_VPP_Prefs.INT_ServerKey(server, m);
        return INT_VPP_Prefs.INT_EventKey(server, m);
    }

    // effective mode: no override = the server's flags (3D if allowed, else 2D if on the map, else hidden);
    // an override is capped by the same flags
    int INT_VPP_SrvMode(MarkerInfo m) {
        if (!m)
            return INT_VPP_MODE_HIDDEN;
        bool can3 = INT_VPP_SrvCan3D(m);
        bool can2 = m.IsActive();
        int mode = INT_VPP_MODE_HIDDEN;
        if (can3)
            mode = INT_VPP_MODE_3D;
        else if (can2)
            mode = INT_VPP_MODE_2D;
        if (!GetGame() || GetGame().IsDedicatedServer())
            return mode;
        // the mission's prefs (created at mission start; none = no overrides)
        INT_VPP_Prefs pr = INT_VPP_Prefs.INT_Peek();
        if (!pr || pr.INT_MarkerModeCount() == 0)
            return mode;
        string key = INT_VPP_SrvModeKey(m);
        int ov = pr.INT_GetMarkerMode(key);
        if (ov < 0)
            return mode;
        int capped = INT_VPP_Prefs.INT_CapMode(ov, can3, can2);
        return capped;
    }

    protected void INT_VPP_RememberOwn(array<ref MarkerInfo> list) {
        m_INT_VPP_OwnServerMarkers = new array<ref MarkerInfo>;
        if (!list)
            return;
        int n = list.Count();
        for (int i = 0; i < n; i++) {
            MarkerInfo m = list.Get(i);
            if (m)
                m_INT_VPP_OwnServerMarkers.Insert(m);
        }
    }

    protected void INT_VPP_NoteServerMarkers() {
        m_INT_VPP_SrvSig = INT_VPP_ServerSignature();
        m_INT_VPP_SrvSigValid = true;
        m_INT_VPP_3DRev++;
    }

    protected int INT_VPP_ServerSignature() {
        array<ref MarkerInfo> list = GetServerMarkers();
        if (!list)
            return 0;
        int n = list.Count();
        int sig = n * 7919 + 17;
        for (int i = 0; i < n; i++) {
            MarkerInfo m = list.Get(i);
            if (!m) {
                sig = sig * 31 + 1;
                continue;
            }
            string nm = m.GetName();
            int hn = nm.Hash();
            string ic = m.GetIconPath();
            int hi = ic.Hash();
            vector p = m.GetPosition();
            int px = Math.Round(p[0]);
            int pz = Math.Round(p[2]);
            int fl = 0;
            if (m.Is3DActive())
                fl = fl + 1;
            if (m.IsActive())
                fl = fl + 2;
            sig = sig * 31 + hn;
            sig = sig * 31 + hi;
            sig = sig * 31 + px;
            sig = sig * 31 + pz;
            sig = sig * 31 + fl;
        }
        return sig;
    }

    /*
    	Return all active 3d markers.
    */
    ref array<ref VPP3DMarker> GetClient3dMarkers() {
        return m_Client3dMarkers;
    }

    void SetClient3dMarkers(ref array<ref VPP3DMarker> vpp3dMarkers) {
        if (!vpp3dMarkers) {
            // given array is null, just clear the existing one then
            //INT (review #14): no `delete` (the K list may hold the same objects): trees now, refs with the array
            INT_VPP_Drop3d(m_Client3dMarkers);
            m_Client3dMarkers = new array<ref VPP3DMarker>;
        } else {
            m_Client3dMarkers = vpp3dMarkers;
        }
    }

    void AddClient3dMarker(ref VPP3DMarker marker3d) {
        if (marker3d && m_Client3dMarkers) {
            m_Client3dMarkers.Insert(marker3d);
        }
    }

    /*
    	Return global 3d markers
    */
    ref array<ref VPP3DMarker> GetServer3dMarkers() {
        return m_Server3dMarkers;
    }

    /*
    	For Global 3dMarkers
    */
    void SetServer3dMarkers(ref array<ref VPP3DMarker> markerArray) {
        if (markerArray != NULL) {
            m_Server3dMarkers = markerArray;
        } else {
            //INT (review #14): no `delete` (see SetClient3dMarkers)
            INT_VPP_Drop3d(m_Server3dMarkers);
            m_Server3dMarkers = new array<ref VPP3DMarker>;
        }
    }

    void ShowAllHidden3dMarkers() {
        if (m_TempHidden3dMarkers) {
            foreach(ref VPP3DMarker marker3dToHide : m_TempHidden3dMarkers) {
                if (marker3dToHide) {
                    marker3dToHide.SetMarkerVisible(true);
                }
            }
            m_TempHidden3dMarkers = null;
            //INT (HUD3D): re-cull at once
            m_INT_VPP_3DRev++;
        }
    }

    void HideAll3dMarkers() {
        if (!m_TempHidden3dMarkers) {
            m_TempHidden3dMarkers = new array<ref VPP3DMarker>;
            foreach(ref VPP3DMarker servermarker3d : m_Server3dMarkers) {
                if (servermarker3d.IsMarkerVisible()) {
                    m_TempHidden3dMarkers.Insert(servermarker3d);
                }
            }
            foreach(ref VPP3DMarker clientmarker3d : m_Client3dMarkers) {
                if (clientmarker3d.IsMarkerVisible()) {
                    m_TempHidden3dMarkers.Insert(clientmarker3d);
                }
            }
            foreach(ref VPP3DMarker marker3dToHide : m_TempHidden3dMarkers) {
                marker3dToHide.SetMarkerVisible(false);
            }
            //INT (HUD3D): SetMarkerVisible(false) already dropped the trees; the beacon follows on the next frame
            m_INT_VPP_3DRev++;
        }
    }

    void TempToggleAll3dMarkers() {
        if (m_TempHidden3dMarkers) {
            ShowAllHidden3dMarkers();
        } else {
            HideAll3dMarkers();
        }
    }

    /*
    	Returns Map Cache Manager instance ( it is auto inits @  DayZGame() )
    */
    ref ClientMarkersCache GetClientMarkersCache() {
        return m_ClientMarkersCache;
    }

    string GetConnectAddressPort() {
        return m_ConnectAddress + m_ConnectPort;
    }
}