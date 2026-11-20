// Курсор действий: под названием устройства базы — состояние сети и вложенное (VB_EB_Info.c).
// Свой блок строк (gui/layouts/eb_cursor_info.layout) вставлен в ванильный GridSpacer курсора сразу после
// строки «item». Строки обновляются вместе с содержимым курсора (PrepareCursorContent, полный проход), но
// собираются заново не чаще раза в VB_EB_REBUILD_MS или при смене устройства.

modded class ActionTargetsCursor
{
	protected static const string VB_EB_LAYOUT = "VB_EchoBase/gui/layouts/eb_cursor_info.layout";
	protected static const int VB_EB_ROWS = 16;		// столько строк в разметке

	protected Widget m_VB_EB_Info;
	protected ref array<Widget> m_VB_EB_Rows = new array<Widget>;
	protected ref array<TextWidget> m_VB_EB_Texts = new array<TextWidget>;
	protected ref array<string> m_VB_EB_Shown = new array<string>;	// что сейчас написано (не трогать без изменений)
	protected ref VB_EB_Lines m_VB_EB_Lines = new VB_EB_Lines;
	protected VB_EB_Device m_VB_EB_LastDev;		// для кого собраны строки
	protected int m_VB_EB_LastBuild = -100000;	// когда, мс
	protected static const int VB_EB_REBUILD_MS = 250;	// строки — не каждый кадр: цифры сервера меняются раз в секунду

	override protected void OnWidgetScriptInit(Widget w)
	{
		super.OnWidgetScriptInit(w);
		VB_EB_InitInfo();
	}

	protected void VB_EB_InitInfo()
	{
		if (!m_Root || !GetGame() || !GetGame().GetWorkspace())
			return;
		m_VB_EB_Info = GetGame().GetWorkspace().CreateWidgets(VB_EB_LAYOUT);
		if (!m_VB_EB_Info)
			return;
		// AddChildAfter ставит новый виджет на место указанного, то есть перед ним (проверено на стенде 11-20:
		// с «item» блок встал над названием) — поэтому указываем следующую строку, «interact»
		GridSpacerWidget grid = GridSpacerWidget.Cast(m_Root);
		Widget next = m_Root.FindAnyWidget("interact");
		if (grid && next && next.GetParent() == m_Root)
			grid.AddChildAfter(m_VB_EB_Info, next, false);
		else
			m_Root.AddChild(m_VB_EB_Info, false);
		for (int i = 0; i < VB_EB_ROWS; i++)
		{
			Widget row = m_VB_EB_Info.FindAnyWidget("vb_eb_row" + i);
			TextWidget txt = TextWidget.Cast(m_VB_EB_Info.FindAnyWidget("vb_eb_txt" + i));
			if (!row || !txt)
				break;
			m_VB_EB_Rows.Insert(row);
			m_VB_EB_Texts.Insert(txt);
			m_VB_EB_Shown.Insert("");
		}
		m_VB_EB_Info.Show(false);
	}

	override protected void PrepareCursorContent()
	{
		super.PrepareCursorContent();
		VB_EB_UpdateInfo();
	}

	// устройство базы под курсором: поставленное, не голограмма установки
	protected VB_EB_Device VB_EB_Target()
	{
		Object o = m_DisplayInteractTarget;
		if (!o && m_Target)
			o = m_Target.GetObject();
		VB_EB_Device d = VB_EB_Device.Cast(o);
		if (!d && m_Target)
			d = VB_EB_Device.Cast(m_Target.GetParent());
		if (!d || !d.VB_IsPlaced())
			return null;
		return d;
	}

	protected void VB_EB_UpdateInfo()
	{
		if (!m_VB_EB_Info)
			return;
		VB_EB_Device d = VB_EB_Target();
		if (!d)
		{
			if (m_VB_EB_Info.IsVisible())
				m_VB_EB_Info.Show(false);
			return;
		}
		d.VB_RequestInfo();
		int now = GetGame().GetTime();
		if (d == m_VB_EB_LastDev && now - m_VB_EB_LastBuild < VB_EB_REBUILD_MS && m_VB_EB_Info.IsVisible())
			return;
		m_VB_EB_LastDev = d;
		m_VB_EB_LastBuild = now;
		d.VB_CursorLines(m_VB_EB_Lines);
		int n = Math.Min(m_VB_EB_Lines.Count(), m_VB_EB_Rows.Count());
		for (int i = 0; i < m_VB_EB_Rows.Count(); i++)
		{
			Widget row = m_VB_EB_Rows[i];
			if (i >= n)
			{
				if (row.IsVisible())
					row.Show(false);
				continue;
			}
			string text = m_VB_EB_Lines.m_Text[i];
			TextWidget txt = m_VB_EB_Texts[i];
			if (m_VB_EB_Shown[i] != text)
			{
				txt.SetText(text);
				m_VB_EB_Shown[i] = text;
			}
			txt.SetColor(m_VB_EB_Lines.m_Color[i]);
			if (!row.IsVisible())
				row.Show(true);
		}
		bool show = n > 0;
		if (m_VB_EB_Info.IsVisible() != show)
			m_VB_EB_Info.Show(show);
	}
};
