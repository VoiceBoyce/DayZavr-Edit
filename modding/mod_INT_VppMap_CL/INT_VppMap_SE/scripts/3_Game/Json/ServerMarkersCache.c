//INT (2026-11-12): server only (PBO INT_VppMap_SE). The file lives in $profile:INT_Mods/INT_VppMap/ - moved once from
//$profile:VPPMapConfig.json by INT_VPP_SE_Migrate before this first read (its old place only for a session whose
//move failed). Format and behaviour unchanged. VPP's SAVE_PATH const is gone (nothing read it; it named a fixed path
//while a failed move reads the old one): INT_Path() is the file in use.
//Every read goes through INT_VPP_SE_Migrate.INT_GuardMarkers first: VPP's reader throws on bad JSON (which used to
//abort MissionServer's constructor), so a bad file is set aside as .broken-<stamp> and VPP writes its defaults.
class ServerMarkersCache extends JsonCacheManager<VPPMapConfig> {
    protected string m_INT_Path;

    void ServerMarkersCache() {
        string intPath = INT_VPP_SE_Migrate.INT_MarkersPath();
        m_INT_Path = intPath;
        Init(intPath, new VPPMapConfig);
    }

    string INT_Path() {
        return m_INT_Path;
    }

    override void LoadCache() {
        bool safe = INT_VPP_SE_Migrate.INT_GuardMarkers(m_INT_Path);
        if (!safe)
            return;
        super.LoadCache();
    }

    override void LoadDefaults() {
        VPPMapConfig.Cast(m_MarkersCache).SetUse3DMarkers(true);
        VPPMapConfig.Cast(m_MarkersCache).SetOwnPositionMarkerDisabled(false);
        VPPMapConfig.Cast(m_MarkersCache).SetForceToUseMapItem(false);
        array<ref MarkerInfo> arr = new array<ref MarkerInfo>;
        arr.Insert( new MarkerInfo("Bohemia Interactive","VanillaPPMap\\GUI\\Textures\\CustomMapIcons\\waypointeditor_CA.paa",Vector(255,255,255),Vector(7500,0,7500),true,true));
        arr.Insert( new MarkerInfo("Loot Stash","VanillaPPMap\\GUI\\Textures\\CustomMapIcons\\waypointeditor_CA.paa",Vector(255,255,255),Vector(7400,0,7200),true,true));
        m_MarkersCache.SetCachedArray(arr);
        SaveCache();
        LoadCache();
    }

    bool CanUse3DMarkers() {
        LoadCache();
        return VPPMapConfig.Cast(m_MarkersCache).CanUse3DMarkers();
    }

    bool OwnPositionMarkerDisabled() {
        LoadCache();
        return VPPMapConfig.Cast(m_MarkersCache).OwnPositionMarkerDisabled();
    }

    bool ForceToUseMapItem(){
        LoadCache();
        return VPPMapConfig.Cast(m_MarkersCache).ForceToUseMapItem();
    }
};