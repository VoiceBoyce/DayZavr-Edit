modded class TraderNotification
{

    Widget Init(Widget root_widget, string message, float time, bool isExitSafezoneMsg = false)
    {
        m_Parent = root_widget;
        layoutRoot = GetGame().GetWorkspace().CreateWidgets("SP_TraderStalkerHud/layouts/TraderNotification.layout", m_Parent);
        m_Message = RichTextWidget.Cast(layoutRoot.FindAnyWidget("text_message") );
        m_Timer = time;
        if (isExitSafezoneMsg)
        {
            message = GetExitSafeZoneMsg();
            m_IsExitSafezoneMsg = true;
        }
        m_Message.SetText(message);
        layoutRoot.Show(true);
        return layoutRoot;
    }
};