// ===================== Рунные алтари =====================
// Состояние «алтарь копит энергию» синхронизируется с клиентом: действие «Добывать» скрывается,
// а алтарь меняет материал на неактивный (берётся из класса <тип>_NoActive, он есть в конфиге для каждого алтаря).
class MSFZ_RuneGathering_Base extends BuildingSuper
{
    protected bool m_MSFZ_IsRuneAlreadyGathed = false;
    // Сколько минут алтарь копит энергию после добычи
    protected int MSFZ_RuneGatheringCooldown = 30;

    void MSFZ_RuneGathering_Base()
    {
        RegisterNetSyncVariableBool("m_MSFZ_IsRuneAlreadyGathed");
    }

    override void EEInit()
    {
        super.EEInit();
        MSFZ_UpdateLook();
    }

    override void OnVariablesSynchronized()
    {
        super.OnVariablesSynchronized();
        MSFZ_UpdateLook();
    }

    bool MSFZ_RuneGatheredCheck(PlayerBase player)
    {
        return m_MSFZ_IsRuneAlreadyGathed;
    }

    int MSFZ_Timeoutlimit()
    {
        return MSFZ_RuneGatheringCooldown;
    }

    // Сервер: алтарь опустошён, через MSFZ_RuneGatheringCooldown минут восстановится
    void MSFZ_RuneGatheringTimeout()
    {
        m_MSFZ_IsRuneAlreadyGathed = true;
        SetSynchDirty();
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(MSFZ_RuneGatheringReGen, MSFZ_RuneGatheringCooldown * 60 * 1000, false);
    }

    void MSFZ_RuneGatheringReGen()
    {
        m_MSFZ_IsRuneAlreadyGathed = false;
        SetSynchDirty();
    }

    // Клиент: активный или неактивный материал
    protected void MSFZ_UpdateLook()
    {
        if (GetGame().IsDedicatedServer())
            return;

        string cfgClass = GetType();
        if (m_MSFZ_IsRuneAlreadyGathed)
            cfgClass = cfgClass + "_NoActive";

        TStringArray materials = new TStringArray;
        GetGame().ConfigGetTextArray("CfgVehicles " + cfgClass + " hiddenSelectionsMaterials", materials);
        for (int i = 0; i < materials.Count(); i++)
        {
            SetObjectMaterial(i, materials[i]);
        }
    }

    string GetDropItemClassName(string targetClassName)
    {
        switch (targetClassName)
        {
            case "MSFZ_Obelisk":
                return "MSFZ_RuneStone_Day";
            case "msfz_stone_1":
                return "MSFZ_RuneStone_FireSmall";
            case "msfz_stone_2":
                return "MSFZ_RuneStone_Fire";
            case "msfz_stone_altar_1":
                return "MSFZ_RuneStone_Ice";
            case "msfz_stone_altar_2":
                return "MSFZ_RuneStone_IceSmall";
            case "msfz_stone_altar_3":
                return "MSFZ_RuneStone_StormSmall";
            case "msfz_stone_altar_4":
                return "MSFZ_RuneStone_Storm";
        }
        return "";
    }
}

class MSFZ_Obelisk : MSFZ_RuneGathering_Base {};
class msfz_stone_1 : MSFZ_RuneGathering_Base {};
class msfz_stone_2 : MSFZ_RuneGathering_Base {};
class msfz_stone_altar_1 : MSFZ_RuneGathering_Base {};
class msfz_stone_altar_2 : MSFZ_RuneGathering_Base {};
class msfz_stone_altar_3 : MSFZ_RuneGathering_Base {};
class msfz_stone_altar_4 : MSFZ_RuneGathering_Base {};

// ===================== Порталы =====================
// Раньше позиции хранились в двух статических переменных: работала только одна пара порталов
// (все входы вели к последнему загруженному выходу). Теперь:
//  - вход (MSFZ_Portal) ведёт к ближайшему выходу (MSFZ_Portal_Exit);
//  - выход возвращает к тому входу, через который игрок пришёл (сервер помнит до рестарта),
//    иначе — к ближайшему входу.
class MSFZ_PortalRegistry
{
    static ref array<MSFZ_Portal_Base> s_Entries = new array<MSFZ_Portal_Base>;
    static ref array<MSFZ_Portal_Base> s_Exits = new array<MSFZ_Portal_Base>;
    static ref map<string, vector> s_ReturnPoints = new map<string, vector>; // сервер: SteamID -> позиция входа

    static void Register(MSFZ_Portal_Base portal)
    {
        array<MSFZ_Portal_Base> list = GetList(portal);
        if (list.Find(portal) == -1)
            list.Insert(portal);
    }

    static void Unregister(MSFZ_Portal_Base portal)
    {
        array<MSFZ_Portal_Base> list = GetList(portal);
        int idx = list.Find(portal);
        if (idx != -1)
            list.Remove(idx);
    }

    static array<MSFZ_Portal_Base> GetList(MSFZ_Portal_Base portal)
    {
        if (portal.MSFZ_IsExit())
            return s_Exits;
        return s_Entries;
    }

    // Ближайший портал из списка; vector.Zero, если порталов нет
    static vector Nearest(array<MSFZ_Portal_Base> list, vector from)
    {
        vector best = vector.Zero;
        float bestDist = -1;
        foreach (MSFZ_Portal_Base p : list)
        {
            if (!p)
                continue;
            float d = vector.DistanceSq(from, p.GetPosition());
            if (bestDist < 0 || d < bestDist)
            {
                bestDist = d;
                best = p.GetPosition();
            }
        }
        return best;
    }
}

class MSFZ_Portal_Base : House
{
    bool MSFZ_IsExit()
    {
        return false;
    }

    override void EEInit()
    {
        super.EEInit();
        MSFZ_PortalRegistry.Register(this);
    }

    override void EEDelete(EntityAI parent)
    {
        MSFZ_PortalRegistry.Unregister(this);
        super.EEDelete(parent);
    }

    override void SetActions()
    {
        super.SetActions();
        AddAction(ActionTeleportToObelisk);
    }

    // Куда перенесёт игрока; vector.Zero — некуда (парного портала нет)
    vector MSFZ_GetDestination(PlayerBase player)
    {
        return vector.Zero;
    }
}

class MSFZ_Portal : MSFZ_Portal_Base
{
    override vector MSFZ_GetDestination(PlayerBase player)
    {
        return MSFZ_PortalRegistry.Nearest(MSFZ_PortalRegistry.s_Exits, GetPosition());
    }
}

class MSFZ_Portal_Exit : MSFZ_Portal_Base
{
    override bool MSFZ_IsExit()
    {
        return true;
    }

    override vector MSFZ_GetDestination(PlayerBase player)
    {
        if (player && player.GetIdentity())
        {
            vector back;
            if (MSFZ_PortalRegistry.s_ReturnPoints.Find(player.GetIdentity().GetPlainId(), back))
                return back;
        }
        return MSFZ_PortalRegistry.Nearest(MSFZ_PortalRegistry.s_Entries, GetPosition());
    }
}
