class DRL_CraftProcessEntryWidget
{
    protected Widget m_Root;
    protected ButtonWidget m_Button;
    protected ImageWidget m_Icon;
    protected ItemPreviewWidget m_Preview;
    protected ProgressBarWidget m_ProgressBar;
    protected ButtonWidget m_CancelButton;
    protected Widget m_QueuePanel;

    protected ref DRL_CraftProcessSyncInfo m_SyncInfo;
    protected float m_LocalElapsed;
    protected float m_TotalDuration;
    protected EntityAI m_PreviewEntity;
    protected DRL_CraftMenuUI m_Menu;

    void DRL_CraftProcessEntryWidget(Widget parent, DRL_CraftProcessSyncInfo info, DRL_CraftMenuUI menu)
    {
        m_SyncInfo = info;
        m_Menu = menu;

        if (!parent || !info)
        {
            return;
        }

        m_TotalDuration = info.m_TotalDurationSec;
        if (m_TotalDuration <= 0.0)
        {
            m_TotalDuration = 1.0;
        }

        m_LocalElapsed = m_TotalDuration - info.m_RemainingTimeSec;
        if (m_LocalElapsed < 0.0)
        {
            m_LocalElapsed = 0.0;
        }

        m_Root = GetGame().GetWorkspace().CreateWidgets("DRL_CraftMenu/UI/layout/CraftProcess.layout", parent);
        if (!m_Root)
        {
            return;
        }

        m_Root.SetUserData(this);

        m_Button = ButtonWidget.Cast(m_Root.FindAnyWidget("CraftProcessButton"));
        if (!m_Button)
        {
            m_Button = ButtonWidget.Cast(m_Root);
        }
        if (m_Button)
        {
            m_Button.SetUserData(this);
        }

        m_CancelButton = ButtonWidget.Cast(m_Root.FindAnyWidget("CancelButton"));
        if (m_CancelButton)
        {
            m_CancelButton.SetUserData(this);
            m_CancelButton.Show(false);
        }

        m_QueuePanel = m_Root.FindAnyWidget("QueuePanel");

        m_Icon = ImageWidget.Cast(m_Root.FindAnyWidget("RecipesImagePreview"));
        if (!m_Icon)
        {
            m_Icon = ImageWidget.Cast(m_Root.FindAnyWidget("CraftProcessIconImage"));
        }

        m_Preview = ItemPreviewWidget.Cast(m_Root.FindAnyWidget("RecipesItemPreview"));
        if (!m_Preview)
        {
            m_Preview = ItemPreviewWidget.Cast(m_Root.FindAnyWidget("CraftProcessItemPreview"));
        }

        m_ProgressBar = ProgressBarWidget.Cast(m_Root.FindAnyWidget("CraftProcessProgressBar"));

        SetupVisuals();
        UpdateProgress();
    }

    void ~DRL_CraftProcessEntryWidget()
    {
        Destroy();
    }

    void Destroy()
    {
        if (m_Preview)
        {
            m_Preview.SetItem(null);
            m_Preview = null;
        }

        if (m_Root)
        {
            m_Root.Unlink();
            m_Root = null;
        }
    }

    protected void SetupVisuals()
    {
        if (!m_SyncInfo)
        {
            return;
        }

        DRL_Craft_Config cfg = null;
        if (m_Menu)
        {
            cfg = m_Menu.GetCraft_Config();
        }
        else
        {
            cfg = g_Craft_Config;
        }

        DRL_Craft_Recipe r = null;
        if (cfg)
        {
            r = cfg.FindById(m_SyncInfo.m_RecipeId);
        }

        bool iconLoaded = false;
        if (r && r.CraftIcon != "" && m_Icon)
        {
            m_Icon.LoadImageFile(0, DRL_CraftUIHelper.ResolveImagePath(r.CraftIcon));
            m_Icon.Show(true);
            iconLoaded = true;
        }
        else if (m_Icon)
        {
            m_Icon.Show(false);
        }

        if (!iconLoaded && m_Preview && m_SyncInfo.m_ResultClass != "")
        {
            if (m_Menu)
            {
                m_PreviewEntity = m_Menu.GetPreviewEntity(m_SyncInfo.m_ResultClass);
            }
            if (m_PreviewEntity)
            {
                m_Preview.SetItem(m_PreviewEntity);
                int vIdx = m_PreviewEntity.GetViewIndex();
                if (vIdx < 0)
                {
                    vIdx = 0;
                }
                m_Preview.SetView(vIdx);
                m_Preview.SetModelPosition(Vector(0, 0, 0));
                m_Preview.SetModelOrientation(Vector(0, 0, 0));
                m_Preview.Show(true);
            }
        }

        if (m_QueuePanel)
        {
            m_QueuePanel.Show(IsQueued());
        }

        if (m_ProgressBar)
        {
            m_ProgressBar.Show(!IsQueued());
        }
    }

    void UpdateTick(float dt)
    {
        if (IsQueued())
        {
            if (m_ProgressBar)
            {
                m_ProgressBar.SetCurrent(0);
                m_ProgressBar.Show(false);
            }
            if (m_QueuePanel)
            {
                m_QueuePanel.Show(true);
            }
            return;
        }

        if (m_QueuePanel)
        {
            m_QueuePanel.Show(false);
        }
        if (m_ProgressBar)
        {
            m_ProgressBar.Show(true);
        }

        m_LocalElapsed += dt;
        if (m_LocalElapsed > m_TotalDuration)
        {
            m_LocalElapsed = m_TotalDuration;
        }
        UpdateProgress();
    }

    void SetCancelButtonVisible(bool show)
    {
        if (m_CancelButton)
        {
            m_CancelButton.Show(show);
        }
    }

    ButtonWidget GetCancelButton()
    {
        return m_CancelButton;
    }

    bool IsQueued()
    {
        if (m_SyncInfo)
        {
            return m_SyncInfo.IsQueued();
        }
        return false;
    }

    protected void UpdateProgress()
    {
        if (!m_ProgressBar || m_TotalDuration <= 0.0)
        {
            return;
        }

        float pct = (m_LocalElapsed / m_TotalDuration) * 100.0;
        int clamped = Math.Clamp(Math.Round(pct), 0, 100);
        m_ProgressBar.SetCurrent(clamped);
    }

    int GetCurrentProgressPercent()
    {
        if (m_TotalDuration <= 0.0)
        {
            return 0;
        }

        float pct = (m_LocalElapsed / m_TotalDuration) * 100.0;
        return Math.Clamp(Math.Round(pct), 0, 100);
    }

    bool IsFinished()
    {
        return m_LocalElapsed >= m_TotalDuration;
    }

    string GetProcessId()
    {
        if (m_SyncInfo)
        {
            return m_SyncInfo.m_ProcessId;
        }
        return "";
    }

    int GetRecipeId()
    {
        if (m_SyncInfo)
        {
            return m_SyncInfo.m_RecipeId;
        }
        return -1;
    }

    DRL_CraftProcessSyncInfo GetSyncInfo()
    {
        return m_SyncInfo;
    }

    Widget GetRoot()
    {
        return m_Root;
    }
}
