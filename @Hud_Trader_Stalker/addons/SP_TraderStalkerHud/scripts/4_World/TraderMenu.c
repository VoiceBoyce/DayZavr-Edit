modded class TraderMenu extends UIScriptedMenu
{
	const string m_QuantString = "";

    override Widget Init()
    {
		m_Player = PlayerBase.Cast(GetGame().GetPlayer());
		layoutRoot = GetGame().GetWorkspace().CreateWidgets( "SP_TraderStalkerHud/layouts/TraderMenu.layout" );

        m_BtnBuy = ButtonWidget.Cast( layoutRoot.FindAnyWidget( "btn_buy" ) );
		m_BtnSell = ButtonWidget.Cast( layoutRoot.FindAnyWidget( "btn_sell" ) );
		m_BtnCancel = ButtonWidget.Cast( layoutRoot.FindAnyWidget( "btn_cancel" ) );
		m_ListboxItems = TextListboxWidget.Cast(layoutRoot.FindAnyWidget("txtlist_items") );
		m_Saldo = TextWidget.Cast(layoutRoot.FindAnyWidget("text_saldo") );
		m_SaldoValue = TextWidget.Cast(layoutRoot.FindAnyWidget("text_saldoValue") );
		m_TraderName = TextWidget.Cast(layoutRoot.FindAnyWidget("title_text") );
		m_XComboboxCategorys = XComboBoxWidget.Cast( layoutRoot.FindAnyWidget( "xcombobox_categorys" ) );
		m_ItemDescription = MultilineTextWidget.Cast( layoutRoot.FindAnyWidget( "ItemDescWidget" ) );
		m_ItemWeight = TextWidget.Cast(layoutRoot.FindAnyWidget("ItemWeight") );
		m_ItemQuantity = TextWidget.Cast(layoutRoot.FindAnyWidget("ItemQuantity"));
		m_SearchBox    = EditBoxWidget.Cast( layoutRoot.FindAnyWidget( "SearchBox" ) );
		m_SellablesCheckbox = CheckBoxWidget.Cast(layoutRoot.FindAnyWidget( "SellablesCheckBox" ) );
		m_SellablesCheckbox.SetChecked(false);
        return layoutRoot;
    }
};