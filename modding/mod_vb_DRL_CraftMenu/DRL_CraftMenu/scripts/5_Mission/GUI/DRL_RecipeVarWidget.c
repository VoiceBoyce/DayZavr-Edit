class DRL_RecipeVarWidget
{
	protected Widget 			m_Root;
	protected ButtonWidget 		m_Btn;
	protected Widget 			m_PanelItem;
	protected Widget 			m_PanelBg;
	protected ImageWidget 		m_Icon;
	protected ItemPreviewWidget m_Preview;
	protected ButtonWidget 		m_InfoBtn;
	protected ImageWidget 		m_InfoIcon;
	protected CheckBoxWidget 	m_CheckBox;
	protected ImageWidget 		m_LockIcon;
	protected Widget 			m_VarSelectPanel;
	protected GridSpacerWidget 	m_VarGridSpacer;
	protected ref array<ButtonWidget> m_VariantButtons;
	protected ref array<Widget>       m_VariantPanels;
	protected ref array<TextWidget>   m_VariantTexts;
	protected ref array<string>       m_AvailableVariants;
	protected int                     m_SelectedVariantIdx = 0;

	protected Widget 			m_QuantityNumberPanel;
	protected ButtonWidget 		m_MinusQuantityBtn;
	protected ButtonWidget 		m_PlusQuantityBtn;
	protected TextWidget 		m_QuantityNumberText;
	protected int 				m_Quantity = 1;
	static const int 			MAX_ATTACHED_QUANTITY = 10;

	protected DRL_Craft_Recipe 	m_Recipe;
	protected DRL_CraftMenuUI 	m_Menu;
	protected bool 				m_IsAvailable;
	protected bool 				m_IsChecked;
	protected bool 				m_IsRecipeAllowed = true;
	protected bool 				m_IsLearned = true;
	protected int 				m_DefaultItemPanelColor;

	static const int COLOR_CHECKBOX_OK = 0xFF50C850;
	static const int COLOR_CHECKBOX_LOCK = 0xFFC83C3C;
	static const int COLOR_PANEL_OK = 0x1E78C850;
	static const int COLOR_PANEL_LOCK = 0x1EC83C3C;

	Widget Init(Widget parent, DRL_Craft_Recipe recipe, DRL_CraftMenuUI menu)
	{
		m_Recipe = recipe;
		m_Menu = menu;

		m_Root = g_Game.GetWorkspace().CreateWidgets("DRL_CraftMenu/UI/layout/RecipeVar.layout", parent);
		if (!m_Root)
		{
			return null;
		}

		m_Btn = ButtonWidget.Cast(m_Root);
		m_PanelItem = m_Root.FindAnyWidget("RecipesDopItemPanelWidget");
		if (m_PanelItem)
		{
			m_DefaultItemPanelColor = m_PanelItem.GetColor();
		}
		m_PanelBg = m_Root.FindAnyWidget("RecipesDopPanelW");
		m_Icon = ImageWidget.Cast(m_Root.FindAnyWidget("RecipesImagePreview"));
		m_Preview = ItemPreviewWidget.Cast(m_Root.FindAnyWidget("RecipesItemPreview"));
		m_InfoBtn = ButtonWidget.Cast(m_Root.FindAnyWidget("InfoRecipesDopButton"));
		m_InfoIcon = ImageWidget.Cast(m_Root.FindAnyWidget("InfoRecipesDopIcon"));
		m_CheckBox = CheckBoxWidget.Cast(m_Root.FindAnyWidget("RecipesDopCheckBox"));
		m_LockIcon = ImageWidget.Cast(m_Root.FindAnyWidget("RecipesDopLockIcon"));

		m_VarSelectPanel = m_Root.FindAnyWidget("VarSelectCraftPanel");
		m_VarGridSpacer  = GridSpacerWidget.Cast(m_Root.FindAnyWidget("VarSelectCraftGridSpacer"));

		m_QuantityNumberPanel = m_Root.FindAnyWidget("QuantityNumberPanel");
		m_MinusQuantityBtn    = ButtonWidget.Cast(m_Root.FindAnyWidget("MinusQuantityNumberButton"));
		m_PlusQuantityBtn     = ButtonWidget.Cast(m_Root.FindAnyWidget("PlusQuantityNumberButton"));
		m_QuantityNumberText  = TextWidget.Cast(m_Root.FindAnyWidget("QuantityNumberText"));

		if (m_QuantityNumberPanel)
		{
			m_QuantityNumberPanel.Show(false);
		}
		if (m_QuantityNumberText)
		{
			m_QuantityNumberText.SetText("1");
		}

		InitVariants();
		SetupVisuals();

		m_Root.SetUserData(m_Recipe);
		if (m_Btn)
		{
			m_Btn.SetUserData(m_Recipe);
		}
		if (m_InfoBtn)
		{
			m_InfoBtn.SetUserData(m_Recipe);
		}
		if (m_CheckBox)
		{
			m_CheckBox.SetUserData(m_Recipe);
		}
		if (m_MinusQuantityBtn)
		{
			m_MinusQuantityBtn.SetUserData(m_Recipe);
		}
		if (m_PlusQuantityBtn)
		{
			m_PlusQuantityBtn.SetUserData(m_Recipe);
		}

		return m_Root;
	}

	void InitVariants()
	{
		m_AvailableVariants = new array<string>();
		m_VariantButtons = new array<ButtonWidget>();
		m_VariantPanels = new array<Widget>();
		m_VariantTexts = new array<TextWidget>();
		m_SelectedVariantIdx = 0;

		if (!m_Recipe)
		{
			if (m_VarSelectPanel) m_VarSelectPanel.Show(false);
			return;
		}

		if (m_Recipe.ResultClasses && m_Recipe.ResultClasses.Count() > 1)
		{
			for (int v = 0; v < m_Recipe.ResultClasses.Count(); v++)
			{
				m_AvailableVariants.Insert(m_Recipe.ResultClasses[v]);
			}
		}
		else if (m_Recipe.Result != "")
		{
			m_AvailableVariants.Insert(m_Recipe.Result);
		}

		if (m_AvailableVariants.Count() <= 1)
		{
			if (m_VarSelectPanel)
			{
				m_VarSelectPanel.Show(false);
			}
			return;
		}

		if (m_VarSelectPanel)
		{
			m_VarSelectPanel.Show(true);
		}

		if (m_VarGridSpacer)
		{
			DRL_CraftUIHelper.ClearChildren(m_VarGridSpacer);
			for (int i = 0; i < m_AvailableVariants.Count(); i++)
			{
				Widget varRoot = g_Game.GetWorkspace().CreateWidgets("DRL_CraftMenu/UI/layout/VarSelectCraft.layout", m_VarGridSpacer);
				if (!varRoot) continue;

				ButtonWidget btn = ButtonWidget.Cast(varRoot.FindAnyWidget("VarSelectCraftButton"));
				if (!btn) btn = ButtonWidget.Cast(varRoot);
				Widget p = varRoot.FindAnyWidget("VarSelectCraftP");
				TextWidget numText = TextWidget.Cast(varRoot.FindAnyWidget("VarSelectCraftNumber"));

				if (btn)
				{
					btn.SetUserID(i);
					btn.SetUserData(m_Recipe);
					m_VariantButtons.Insert(btn);
				}
				if (p)
				{
					m_VariantPanels.Insert(p);
				}
				if (numText)
				{
					numText.SetText((i + 1).ToString());
					m_VariantTexts.Insert(numText);
				}
			}
			m_VarGridSpacer.Update();
		}

		UpdateVariantVisuals();
	}

	void UpdateVariantVisuals()
	{
		if (!m_VariantPanels) return;
		for (int i = 0; i < m_VariantPanels.Count(); i++)
		{
			Widget p = m_VariantPanels[i];
			if (!p) continue;
			if (i == m_SelectedVariantIdx)
			{
				p.SetColor(ARGB(255, 235, 140, 50));
			}
			else
			{
				p.SetColor(ARGB(60, 192, 189, 189));
			}
		}
	}

	void SelectVariant(int idx)
	{
		if (!m_AvailableVariants || idx < 0 || idx >= m_AvailableVariants.Count())
			return;

		m_SelectedVariantIdx = idx;
		UpdateVariantVisuals();
		UpdatePreviewForSelectedVariant();
	}

	string GetSelectedResultClass()
	{
		if (m_AvailableVariants && m_SelectedVariantIdx >= 0 && m_SelectedVariantIdx < m_AvailableVariants.Count())
		{
			return m_AvailableVariants[m_SelectedVariantIdx];
		}
		if (m_Recipe)
		{
			if (m_Recipe.PreviewClassname != "") return m_Recipe.PreviewClassname;
			return m_Recipe.Result;
		}
		return "";
	}

	bool IsVariantButton(Widget w, out int outIdx)
	{
		outIdx = -1;
		if (!w || !m_VariantButtons) return false;
		for (int i = 0; i < m_VariantButtons.Count(); i++)
		{
			ButtonWidget btn = m_VariantButtons[i];
			if (btn && (w == btn || (m_VariantPanels && i < m_VariantPanels.Count() && w == m_VariantPanels[i]) || (m_VariantTexts && i < m_VariantTexts.Count() && w == m_VariantTexts[i])))
			{
				outIdx = i;
				return true;
			}
		}
		return false;
	}

	array<ButtonWidget> GetVariantButtons()
	{
		return m_VariantButtons;
	}

	void UpdatePreviewForSelectedVariant()
	{
		if (m_Preview && m_Menu)
		{
			string cls = GetSelectedResultClass();
			if (cls == "") return;

			EntityAI e = m_Menu.GetPreviewEntity(cls);
			if (e)
			{
				m_Preview.SetItem(e);
				m_Preview.SetView(Math.Max(0, e.GetViewIndex()));
				m_Preview.SetModelPosition(Vector(0, 0, 0));
				m_Preview.SetModelOrientation(Vector(0, 0, 0));
				m_Preview.Show(true);
				m_Menu.AddAttachedPreview(m_Preview);
			}
			else
			{
				m_Preview.SetItem(null);
			}
		}
	}

	void SetupVisuals()
	{
		if (!m_Recipe)
		{
			return;
		}

		if (m_Recipe.CraftIcon != "")
		{
			if (m_Icon)
			{
				m_Icon.LoadImageFile(0, DRL_CraftMenuUI.ResolveImagePath(m_Recipe.CraftIcon));
				m_Icon.Show(true);
			}
			if (m_Preview)
			{
				m_Preview.SetItem(null);
				m_Preview.Show(false);
			}
		}
		else
		{
			if (m_Icon)
			{
				m_Icon.Show(false);
			}
			UpdatePreviewForSelectedVariant();
		}
	}

	void SetRecipeAllowed(bool allowed)
	{
		m_IsRecipeAllowed = allowed;
		if (m_InfoIcon)
		{
			if (m_IsRecipeAllowed)
			{
				m_InfoIcon.SetColor(ARGB(255, 255, 255, 255));
			}
			else
			{
				m_InfoIcon.SetColor(ARGB(255, 200, 60, 60));
			}
		}
		if (m_InfoBtn)
		{
			m_InfoBtn.Enable(m_IsRecipeAllowed);
		}
	}

	bool IsRecipeAllowed()
	{
		return m_IsRecipeAllowed;
	}

	void SetLearned(bool learned)
	{
		m_IsLearned = learned;
		if (!m_IsLearned)
		{
			if (m_PanelItem)
			{
				m_PanelItem.SetColor(ARGB(255, 90, 42, 42));
			}
			if (m_CheckBox)
			{
				m_CheckBox.Show(false);
				m_CheckBox.Enable(false);
			}
			if (m_LockIcon)
			{
				m_LockIcon.LoadImageFile(0, "DRL_CraftMenu/UI/data/lock1.edds");
				m_LockIcon.Show(true);
			}
		}
		else
		{
			if (m_PanelItem)
			{
				m_PanelItem.SetColor(m_DefaultItemPanelColor);
			}
			if (m_CheckBox)
			{
				m_CheckBox.Show(true);
				m_CheckBox.Enable(true);
			}
			if (m_LockIcon)
			{
				m_LockIcon.Show(false);
			}
		}
	}

	bool IsLearned()
	{
		return m_IsLearned;
	}

	void SetSlotAvailable(bool available)
	{
		m_IsAvailable = available;

		if (!m_IsLearned)
		{
			if (m_CheckBox)
			{
				m_CheckBox.Show(false);
				m_CheckBox.Enable(false);
			}
			if (m_LockIcon)
			{
				m_LockIcon.Show(true);
			}
			if (m_PanelItem)
			{
				m_PanelItem.SetColor(ARGB(60, 90, 42, 42));
			}
			return;
		}

		if (m_LockIcon)
		{
			m_LockIcon.Show(false);
		}

		if (m_CheckBox)
		{
			m_CheckBox.Show(true);
			m_CheckBox.Enable(true);
			if (m_IsAvailable || m_IsChecked)
			{
				m_CheckBox.SetColor(COLOR_CHECKBOX_OK);
			}
			else
			{
				m_CheckBox.SetColor(COLOR_CHECKBOX_LOCK);
			}
		}

		if (m_PanelBg)
		{
			if (m_IsAvailable || m_IsChecked)
			{
				m_PanelBg.SetColor(COLOR_PANEL_OK);
			}
			else
			{
				m_PanelBg.SetColor(COLOR_PANEL_LOCK);
			}
		}
	}

	void SetChecked(bool checked)
	{
		m_IsChecked = checked;
		if (m_CheckBox)
		{
			m_CheckBox.SetChecked(checked);
		}

		if (m_QuantityNumberPanel)
		{
			m_QuantityNumberPanel.Show(checked);
		}

		if (!checked)
		{
			m_Quantity = 1;
			if (m_QuantityNumberText)
			{
				m_QuantityNumberText.SetText("1");
			}
		}
	}

	bool ChangeQuantity(int delta)
	{
		int newQty = m_Quantity + delta;
		if (newQty < 1)
		{
			newQty = 1;
		}
		if (newQty > MAX_ATTACHED_QUANTITY)
		{
			newQty = MAX_ATTACHED_QUANTITY;
		}

		if (newQty != m_Quantity)
		{
			m_Quantity = newQty;
			if (m_QuantityNumberText)
			{
				m_QuantityNumberText.SetText(m_Quantity.ToString());
			}
			return true;
		}
		return false;
	}

	int GetQuantity()
	{
		return m_Quantity;
	}

	void SetQuantity(int qty)
	{
		if (qty < 1)
		{
			qty = 1;
		}
		if (qty > MAX_ATTACHED_QUANTITY)
		{
			qty = MAX_ATTACHED_QUANTITY;
		}
		m_Quantity = qty;
		if (m_QuantityNumberText)
		{
			m_QuantityNumberText.SetText(m_Quantity.ToString());
		}
	}

	bool IsQuantityButton(Widget w, out int delta)
	{
		delta = 0;
		if (!w)
		{
			return false;
		}
		Widget cur = w;
		while (cur && cur != m_Root)
		{
			if (cur == m_MinusQuantityBtn || cur.GetName() == "MinusQuantityNumberButton")
			{
				delta = -1;
				return true;
			}
			if (cur == m_PlusQuantityBtn || cur.GetName() == "PlusQuantityNumberButton")
			{
				delta = 1;
				return true;
			}
			cur = cur.GetParent();
		}
		return false;
	}

	ButtonWidget GetMinusQuantityButton()
	{
		return m_MinusQuantityBtn;
	}

	ButtonWidget GetPlusQuantityButton()
	{
		return m_PlusQuantityBtn;
	}

	bool IsChecked()
	{
		return m_IsChecked;
	}

	bool IsSlotAvailable()
	{
		return m_IsAvailable;
	}

	DRL_Craft_Recipe GetRecipe()
	{
		return m_Recipe;
	}

	Widget GetRoot()
	{
		return m_Root;
	}

	ButtonWidget GetButton()
	{
		return m_Btn;
	}

	ButtonWidget GetInfoButton()
	{
		return m_InfoBtn;
	}

	CheckBoxWidget GetCheckBox()
	{
		return m_CheckBox;
	}

	void ~DRL_RecipeVarWidget()
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
		if (m_LockIcon)
		{
			m_LockIcon = null;
		}
		if (m_InfoIcon)
		{
			m_InfoIcon = null;
		}
		if (m_PanelItem)
		{
			m_PanelItem = null;
		}
		if (m_VarSelectPanel)
		{
			m_VarSelectPanel = null;
		}
		if (m_VarGridSpacer)
		{
			DRL_CraftUIHelper.ClearChildren(m_VarGridSpacer);
			m_VarGridSpacer = null;
		}
		if (m_VariantButtons)
		{
			m_VariantButtons.Clear();
			m_VariantButtons = null;
		}
		if (m_VariantPanels)
		{
			m_VariantPanels.Clear();
			m_VariantPanels = null;
		}
		if (m_VariantTexts)
		{
			m_VariantTexts.Clear();
			m_VariantTexts = null;
		}
		if (m_AvailableVariants)
		{
			m_AvailableVariants.Clear();
			m_AvailableVariants = null;
		}
		if (m_QuantityNumberPanel)
		{
			m_QuantityNumberPanel = null;
		}
		if (m_MinusQuantityBtn)
		{
			m_MinusQuantityBtn = null;
		}
		if (m_PlusQuantityBtn)
		{
			m_PlusQuantityBtn = null;
		}
		if (m_QuantityNumberText)
		{
			m_QuantityNumberText = null;
		}
		if (m_Root)
		{
			m_Root.Unlink();
			m_Root = null;
		}
	}
}
