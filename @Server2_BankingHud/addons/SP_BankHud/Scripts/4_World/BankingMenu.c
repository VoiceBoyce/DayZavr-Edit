modded class DC_BankingMenu
{
	override Widget Init()
    {
		
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("SP_BankHud/layouts/BankirMenu.layout");
        m_ButtonDeposit = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonDeposit"));
		m_ButtonWithdraw = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonWithdraw"));
		m_ButtonClose = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonClose"));
		m_TextMaxOwnedAmount = TextWidget.Cast(layoutRoot.FindAnyWidget("TextMaxOwnedAmount"));
		m_TextOwnedAmount = TextWidget.Cast(layoutRoot.FindAnyWidget("TextOwnedAmount"));
		m_TextOnPlayerAmount = TextWidget.Cast(layoutRoot.FindAnyWidget("TextOnPlayerAmount"));
		m_PanelAmountInput = Widget.Cast(layoutRoot.FindAnyWidget("PanelAmountInput"));
		m_TextAmountInput = TextWidget.Cast(layoutRoot.FindAnyWidget("TextAmountInput"));
		m_EditBoxAmount = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("EditBoxAmountInput"));
		m_MultilineTextMessage = MultilineTextWidget.Cast(layoutRoot.FindAnyWidget("MultilineTextMessage"));
		m_PanelMessage = Widget.Cast(layoutRoot.FindAnyWidget("PanelMessage"));
		
		m_TextOwnedAmount.SetText("0");
		m_TextMaxOwnedAmount.SetText("0");
		m_TextOnPlayerAmount.SetText("0");
		m_TextAmountInput.SetText("0");
		m_EditBoxAmount.SetText("0");
		layoutRoot.Show(false);
		
        return layoutRoot;
    }
}