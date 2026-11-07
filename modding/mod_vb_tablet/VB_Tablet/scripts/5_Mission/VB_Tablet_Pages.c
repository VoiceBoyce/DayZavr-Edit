// Экран планшета: рисует себя в рабочей области 1172x720 и получает такт, пока открыт.
class VB_TabletPage extends Managed
{
	protected VB_TabletMenu m_Menu;
	protected Widget m_Root;
	protected int m_HitNo;

	void Attach(VB_TabletMenu menu, Widget root)
	{
		m_Menu = menu;
		m_Root = root;
	}

	void Build();
	void Tick(float dt);
	void Leave();

	// Пришли новые данные. false: экран перестроится целиком. Экраны с полями ввода
	// возвращают true и обновляют только то, что нужно, иначе текст пропадёт.
	bool OnData()
	{
		return false;
	}

	void OnInput(VBT_Input field);
	void OnSubmit(VBT_Input field);
	void OnArea(VBT_Area field);
	void OnPick(VBT_Dropdown list, int index);

	// Ответ диалога: yes и введённый текст.
	void OnAnswer(int tag, bool yes, string text);

	// Ответ сервера на действие, тост меню уже показало.
	void OnResult(int code);

	bool OnHit(Widget w)
	{
		return false;
	}

	bool OnHover(Widget w, bool enter)
	{
		return false;
	}

	protected Widget HitAt(Widget parent, float x, float y, float w, float h)
	{
		Widget hit = INT_UIF_Draw.Hit(parent, x, y, w, h, m_Menu.GetHitHandler());
		m_HitNo++;
		hit.SetName("VBT_Hit_" + m_HitNo.ToString());
		return hit;
	}
}

// Заглушка для экранов, которые ещё не перенесены из макета.
class VB_TabletPageStub extends VB_TabletPage
{
	protected string m_Icon;

	void VB_TabletPageStub(string icon)
	{
		m_Icon = icon;
	}

	override void Build()
	{
		INT_UIF_Draw.Texture(m_Root, 556, 300, 60, 60, VB_TABLET_ICON + m_Icon, VB_C_DIM2);
		INT_UIF_Draw.Text(m_Root, 0, 376, 1172, 32, "#STR_VB_TAB_SOON", 17, VB_C_DIM, INT_UIF_Align.CENTER);
	}
}
