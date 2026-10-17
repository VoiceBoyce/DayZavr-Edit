class DRL_CraftProcessData : Managed
{
    static const int SERIALIZER_VERSION = 4;

    string m_ProcessId;
    int m_RecipeId;
    string m_ResultClass;
    int m_CraftCount;
    string m_PlayerSteamId;
    string m_PlayerName;
    bool m_IsWorkbenchCraft;
    int m_WorkbenchNetLow;
    int m_WorkbenchNetHigh;
    vector m_WorkbenchPos;
    string m_WorkbenchClass;
    vector m_CustomSpawnPos;
    float m_TotalDurationSec;
    int m_StartTimeEpoch;
    ref array<int> m_AttachedRecipeIds;
    ref map<int, string> m_AttachedVariants;
    ref map<int, string> m_SelectedVariations;

    int m_QueuePosition;
    int m_ProcessStatus;
    ref array<int> m_LockedEntityNetLows;
    ref array<int> m_LockedEntityNetHighs;
    bool m_HasPlayerIngredients;

    void DRL_CraftProcessData()
    {
        m_AttachedRecipeIds = new array<int>();
        m_AttachedVariants = new map<int, string>();
        m_SelectedVariations = new map<int, string>();
        m_CustomSpawnPos = "0 0 0";
        m_QueuePosition = 0;
        m_ProcessStatus = DRL_CraftProcessStatus.RUNNING;
        m_LockedEntityNetLows = new array<int>();
        m_LockedEntityNetHighs = new array<int>();
        m_HasPlayerIngredients = false;
    }

    string GetBinFileName()
    {
        if (m_IsWorkbenchCraft)
        {
            int posX = Math.Round(m_WorkbenchPos[0]);
            int posZ = Math.Round(m_WorkbenchPos[2]);
            return "wb_" + posX.ToString() + "_" + posZ.ToString() + "_" + m_ProcessId + ".bin";
        }

        string steamId = m_PlayerSteamId;
        if (steamId == "")
        {
            steamId = "offline_player";
        }
        return "p_" + steamId + "_" + m_ProcessId + ".bin";
    }

    float GetElapsedSec()
    {
        if (m_ProcessStatus == DRL_CraftProcessStatus.QUEUED || m_StartTimeEpoch <= 0)
        {
            return 0.0;
        }

        int now = GetGame().GetTime() / 1000;
        int elapsed = now - m_StartTimeEpoch;
        if (elapsed < 0)
        {
            elapsed = 0;
        }
        return elapsed;
    }

    float GetRemainingSec()
    {
        if (m_ProcessStatus == DRL_CraftProcessStatus.QUEUED)
        {
            return m_TotalDurationSec;
        }

        float rem = m_TotalDurationSec - GetElapsedSec();
        if (rem < 0.0)
        {
            rem = 0.0;
        }
        return rem;
    }

    void OnWrite(ParamsWriteContext ctx)
    {
        ctx.Write(SERIALIZER_VERSION);
        ctx.Write(m_ProcessId);
        ctx.Write(m_RecipeId);
        ctx.Write(m_ResultClass);
        ctx.Write(m_CraftCount);
        ctx.Write(m_PlayerSteamId);
        ctx.Write(m_PlayerName);
        ctx.Write(m_IsWorkbenchCraft);
        ctx.Write(m_WorkbenchNetLow);
        ctx.Write(m_WorkbenchNetHigh);
        ctx.Write(m_WorkbenchPos);
        ctx.Write(m_WorkbenchClass);
        ctx.Write(m_CustomSpawnPos);
        ctx.Write(m_TotalDurationSec);
        ctx.Write(m_StartTimeEpoch);
        ctx.Write(m_AttachedRecipeIds);

        int attVarCount = 0;
        if (m_AttachedVariants)
        {
            attVarCount = m_AttachedVariants.Count();
        }
        ctx.Write(attVarCount);
        for (int aIdx = 0; aIdx < attVarCount; aIdx++)
        {
            ctx.Write(m_AttachedVariants.GetKey(aIdx));
            ctx.Write(m_AttachedVariants.GetElement(aIdx));
        }

        int varCount = 0;
        if (m_SelectedVariations)
        {
            varCount = m_SelectedVariations.Count();
        }
        ctx.Write(varCount);
        for (int i = 0; i < varCount; i++)
        {
            ctx.Write(m_SelectedVariations.GetKey(i));
            ctx.Write(m_SelectedVariations.GetElement(i));
        }

        ctx.Write(m_QueuePosition);
        ctx.Write(m_ProcessStatus);
        ctx.Write(m_LockedEntityNetLows);
        ctx.Write(m_LockedEntityNetHighs);
    }

    bool OnRead(ParamsReadContext ctx)
    {
        int version;
        if (!ctx.Read(version))
        {
            return false;
        }

        if (!ctx.Read(m_ProcessId))
        {
            return false;
        }
        if (!ctx.Read(m_RecipeId))
        {
            return false;
        }
        if (!ctx.Read(m_ResultClass))
        {
            return false;
        }
        if (!ctx.Read(m_CraftCount))
        {
            return false;
        }
        if (!ctx.Read(m_PlayerSteamId))
        {
            return false;
        }
        if (!ctx.Read(m_PlayerName))
        {
            return false;
        }
        if (!ctx.Read(m_IsWorkbenchCraft))
        {
            return false;
        }
        if (!ctx.Read(m_WorkbenchNetLow))
        {
            return false;
        }
        if (!ctx.Read(m_WorkbenchNetHigh))
        {
            return false;
        }
        if (!ctx.Read(m_WorkbenchPos))
        {
            return false;
        }
        if (!ctx.Read(m_WorkbenchClass))
        {
            return false;
        }
        if (version >= 2)
        {
            if (!ctx.Read(m_CustomSpawnPos))
            {
                m_CustomSpawnPos = "0 0 0";
            }
        }
        else
        {
            m_CustomSpawnPos = "0 0 0";
        }

        if (!ctx.Read(m_TotalDurationSec))
        {
            return false;
        }
        if (!ctx.Read(m_StartTimeEpoch))
        {
            return false;
        }
        if (!ctx.Read(m_AttachedRecipeIds))
        {
            return false;
        }

        if (version >= 4)
        {
            int attVarCount;
            if (ctx.Read(attVarCount))
            {
                m_AttachedVariants.Clear();
                for (int av = 0; av < attVarCount; av++)
                {
                    int avKey;
                    string avVal;
                    if (ctx.Read(avKey) && ctx.Read(avVal))
                    {
                        m_AttachedVariants.Set(avKey, avVal);
                    }
                }
            }
        }
        else
        {
            m_AttachedVariants.Clear();
        }

        int varCount;
        if (ctx.Read(varCount))
        {
            m_SelectedVariations.Clear();
            for (int i = 0; i < varCount; i++)
            {
                int k;
                string v;
                if (ctx.Read(k) && ctx.Read(v))
                {
                    m_SelectedVariations.Set(k, v);
                }
            }
        }

        if (version >= 3)
        {
            if (!ctx.Read(m_QueuePosition))
            {
                m_QueuePosition = 0;
            }
            if (!ctx.Read(m_ProcessStatus))
            {
                m_ProcessStatus = DRL_CraftProcessStatus.RUNNING;
            }
            if (!ctx.Read(m_LockedEntityNetLows))
            {
                m_LockedEntityNetLows = new array<int>();
            }
            if (!ctx.Read(m_LockedEntityNetHighs))
            {
                m_LockedEntityNetHighs = new array<int>();
            }
        }
        else
        {
            m_QueuePosition = 0;
            m_ProcessStatus = DRL_CraftProcessStatus.RUNNING;
            m_LockedEntityNetLows = new array<int>();
            m_LockedEntityNetHighs = new array<int>();
        }

        return true;
    }
}

class DRL_CraftProcessSyncInfo : Managed
{
    string m_ProcessId;
    int m_RecipeId;
    string m_ResultClass;
    int m_CraftCount;
    float m_TotalDurationSec;
    float m_RemainingTimeSec;
    bool m_IsWorkbenchCraft;
    int m_WorkbenchNetLow;
    int m_WorkbenchNetHigh;
    int m_QueuePosition;
    int m_ProcessStatus;

    void DRL_CraftProcessSyncInfo(DRL_CraftProcessData data = null)
    {
        if (data)
        {
            m_ProcessId = data.m_ProcessId;
            m_RecipeId = data.m_RecipeId;
            m_ResultClass = data.m_ResultClass;
            m_CraftCount = data.m_CraftCount;
            m_TotalDurationSec = data.m_TotalDurationSec;
            m_RemainingTimeSec = data.GetRemainingSec();
            m_IsWorkbenchCraft = data.m_IsWorkbenchCraft;
            m_WorkbenchNetLow = data.m_WorkbenchNetLow;
            m_WorkbenchNetHigh = data.m_WorkbenchNetHigh;
            m_QueuePosition = data.m_QueuePosition;
            m_ProcessStatus = data.m_ProcessStatus;
        }
    }

    bool IsQueued()
    {
        return m_ProcessStatus == DRL_CraftProcessStatus.QUEUED;
    }
}

class DRL_CraftProcessBridge
{
    protected static ref array<ref DRL_CraftProcessSyncInfo> s_ActiveProcesses = new array<ref DRL_CraftProcessSyncInfo>();

    static ref ScriptInvoker s_OnProcessesSync = new ScriptInvoker();
    static ref ScriptInvoker s_OnProcessStarted = new ScriptInvoker();
    static ref ScriptInvoker s_OnProcessFinished = new ScriptInvoker();
    static ref ScriptInvoker s_OnNotification = new ScriptInvoker();

    static void SetActiveProcesses(array<ref DRL_CraftProcessSyncInfo> list)
    {
        if (!s_ActiveProcesses)
        {
            s_ActiveProcesses = new array<ref DRL_CraftProcessSyncInfo>();
        }
        s_ActiveProcesses.Clear();
        if (list)
        {
            for (int i = 0; i < list.Count(); i++)
            {
                s_ActiveProcesses.Insert(list.Get(i));
            }
        }
    }

    static array<ref DRL_CraftProcessSyncInfo> GetActiveProcesses()
    {
        if (!s_ActiveProcesses)
        {
            s_ActiveProcesses = new array<ref DRL_CraftProcessSyncInfo>();
        }
        return s_ActiveProcesses;
    }
}


