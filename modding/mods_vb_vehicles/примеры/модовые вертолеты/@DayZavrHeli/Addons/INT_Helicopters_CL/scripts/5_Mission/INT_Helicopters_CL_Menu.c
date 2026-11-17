//DayZavr: the helicopter login notice window ("DAYZAVR | HELICOPTERS", text, "Понятно")
class INT_HELI_NoticeMenu extends UIScriptedMenu
{
	protected ButtonWidget m_INT_OkButton;

	override Widget Init()
	{
		layoutRoot = g_Game.GetWorkspace().CreateWidgets("INT_Helicopters_CL/gui/int_heli_notice.layout");
		m_INT_OkButton = ButtonWidget.Cast(layoutRoot.FindAnyWidget("INT_HELI_OkButton"));
		if (m_INT_OkButton)
			m_INT_OkButton.SetText("ПОНЯТНО");

		TextWidget calc_title = TextWidget.Cast(layoutRoot.FindAnyWidget("INT_HELI_Title"));
		if (calc_title)
			calc_title.SetText("DAYZAVR | HELICOPTERS");
		MultilineTextWidget calc_body = MultilineTextWidget.Cast(layoutRoot.FindAnyWidget("INT_HELI_Body"));
		if (calc_body)
			calc_body.SetText(INT_HELI_Notice.INT_TextIntro());
		MultilineTextWidget calc_heli = MultilineTextWidget.Cast(layoutRoot.FindAnyWidget("INT_HELI_HeliLine"));
		if (calc_heli)
			calc_heli.SetText(INT_HELI_Notice.INT_TextHeli());

		//coordinates row: white pin, red when the helicopter is ruined; no row (and a shorter window) when it is gone
		TextWidget calc_coords = TextWidget.Cast(layoutRoot.FindAnyWidget("INT_HELI_Coords"));
		Widget calc_pin = layoutRoot.FindAnyWidget("INT_HELI_Pin");
		bool calc_has_coords = INT_HELI_Notice.s_INT_Status != INT_HELI_Notice.INT_STATUS_NOT_FOUND;
		if (calc_coords)
		{
			calc_coords.SetText(INT_HELI_Notice.INT_TextCoords());
			calc_coords.Show(calc_has_coords);
		}
		if (calc_pin)
		{
			calc_pin.Show(calc_has_coords);
			if (INT_HELI_Notice.s_INT_Status == INT_HELI_Notice.INT_STATUS_RUINED)
				calc_pin.SetColor(ARGB(255, 232, 77, 61));
		}
		Widget calc_window = layoutRoot.FindAnyWidget("INT_HELI_Window");
		if (calc_window && !calc_has_coords)
			calc_window.SetSize(560, 242);
		return layoutRoot;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w == m_INT_OkButton)
		{
			Close();
			return true;
		}
		return super.OnClick(w, x, y, button);
	}

	override void Update(float timeslice)
	{
		super.Update(timeslice);
		if (g_Game.GetInput().LocalPress("UAUIBack", false))
			Close();
	}
}

modded class MissionGameplay
{
	//the menu object must be held by script: ShowScriptedMenu does not keep it alive
	protected ref INT_HELI_NoticeMenu m_INT_HELI_NoticeMenu;

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);

		//show the notice once the player is in the game for a moment and no other menu is open
		if (!INT_HELI_Notice.s_INT_Pending)
			return;
		if (g_Game.GetTickTime() - INT_HELI_Notice.s_INT_ReceivedAt < 1.0)
			return;
		PlayerBase calc_player = PlayerBase.Cast(g_Game.GetPlayer());
		if (!calc_player || !calc_player.IsAlive() || g_Game.GetUIManager().GetMenu())
			return;

		INT_HELI_Notice.s_INT_Pending = false;
		m_INT_HELI_NoticeMenu = new INT_HELI_NoticeMenu;
		g_Game.GetUIManager().ShowScriptedMenu(m_INT_HELI_NoticeMenu, null);
	}
}
