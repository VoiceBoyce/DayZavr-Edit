// Круговое меню камеры транспорта: зажать клавишу (по умолчанию Alt+V), мышью выбрать сторону, отпустить.
// Это оверлей на виджетах INT_UIFramework, а не окно: игра не на паузе, машиной можно управлять.
// Движение мыши читаем из инпутов напрямую, курсора нет. Отпустили в центре — выбор отменён.

class VB_VehicleCameraRadial
{
	protected static const string INPUT_MENU = "UAVBCamMenu";
	protected static const string TEX_DIR = "VB_VehicleCamera/gui/textures/";

	protected static const float SIZE = 560;			// размер меню в дизайн-пикселях
	protected static const float HALF = 280;
	protected static const float HUB_SIZE = 184;
	protected static const float LABEL_X = 175;			// расстояние подписи сектора от центра
	protected static const float POINTER_R = 116;		// радиус точки-указателя
	protected static const float POINTER_SIZE = 12;
	protected static const float PILL_Y = 574;			// верх плашки с описанием, от верха меню
	protected static const float PILL_H = 38;

	protected static const float CURSOR_GAIN = 700;		// дизайн-пикселей на радиан движения мыши
	protected static const float CURSOR_MAX = 120;
	protected static const float DEADZONE = 40;
	protected static const float ZOOM_SHOW_TIME = 1.2;	// сколько секунд центр показывает дальность
	protected static const float FADE_IN_TIME = 0.16;	// меню должно появляться быстро, оно нужно на ходу
	protected static const float FADE_OUT_TIME = 0.1;
	protected static const float REVEAL_RISE = 8;

	protected static const int SECTOR_FILL = ARGB(240, 21, 25, 29);
	protected static const int SECTOR_HOVER = ARGB(245, 30, 49, 48);

	protected Widget m_Root;
	protected Widget m_Fading;		// корень закрытого меню, пока он затухает
	protected float m_FadeLeft;
	protected ImageWidget m_FillLeft;
	protected ImageWidget m_FillRight;
	protected ImageWidget m_ArcLeft;
	protected ImageWidget m_ArcRight;
	protected ImageWidget m_Pointer;
	protected TextWidget m_NameLeft;
	protected TextWidget m_NameRight;
	protected TextWidget m_StateLeft;
	protected TextWidget m_StateRight;
	protected TextWidget m_HubCaption;
	protected TextWidget m_HubName;
	protected TextWidget m_PillText;
	protected ref INT_UIF_Surface m_Pill;

	protected float m_CursorX;
	protected float m_CursorY;
	protected int m_Hover;			// -1 нет, иначе VB_VehicleCameraStore.MODE_*
	protected int m_ZoomTick;
	protected float m_ZoomShow;		// сколько ещё секунд показывать дальность
	protected string m_PillShown;

	void ~VB_VehicleCameraRadial()
	{
		Destroy();
	}

	void OnUpdate(float dt)
	{
		VB_VehicleCameraStore.Tick(dt);
		TickFade(dt);

		bool canShow = CanShow();

		// V в ванилле переключает вид: пока зажат Alt в транспорте, гасим это нажатие.
		if (canShow && GetUApi().GetInputByID(UALookAround).LocalValue() > 0)
			GetUApi().GetInputByID(UAPersonView).Supress();

		bool held = false;
		UAInput input = GetUApi().GetInputByName(INPUT_MENU);
		if (input && canShow)
			held = input.LocalValue() > 0;

		if (held && !m_Root)
			Open();

		if (!m_Root)
			return;

		if (!held)
		{
			// Если меню закрылось не по отпусканию клавиши (вышли из машины, открыли окно), выбор не применяем.
			Close(canShow);
			return;
		}

		UpdateCursor(dt);
		Refresh(dt);
	}

	protected bool CanShow()
	{
		if (g_Game.GetUIManager().GetMenu())
			return false;

		return VB_VehicleCameraRig.IsVehicleCameraActive() && VB_VehicleCameraRig.GetLocalSeat() != null;
	}

	protected void Open()
	{
		float designW = INT_UIF_Scale.ScreenW() / INT_UIF_Scale.Factor();
		float designH = INT_UIF_Scale.ScreenH() / INT_UIF_Scale.Factor();

		m_Root = INT_UIF_Draw.Frame(null, (designW - SIZE) * 0.5, (designH - SIZE) * 0.5, SIZE, SIZE, "VBCamRadial");
		if (!m_Root)
			return;

		m_Root.SetSort(INT_UIF_SORT_POPUP);
		m_CursorX = 0;
		m_CursorY = 0;
		m_Hover = -1;
		m_ZoomShow = 0;
		m_ZoomTick = VB_VehicleCameraRig.GetZoomTick();
		m_PillShown = "";

		Build();
		Refresh(0);
		VB_VehicleCameraRig.SetMenuOpen(true);
		INT_UIF_Animator.Reveal(m_Root, 0, REVEAL_RISE, FADE_IN_TIME);
	}

	protected void Build()
	{
		INT_UIF_ThemeManager theme = INT_UIF_ThemeManager.Get();
		int teal = theme.Color(INT_UIF_ThemeKeys.TEAL);
		int text = theme.Color(INT_UIF_ThemeKeys.TEXT);
		int faint = theme.Color(INT_UIF_ThemeKeys.TEXT_FAINT);
		int muted = theme.Color(INT_UIF_ThemeKeys.TEXT_MUTED);

		INT_UIF_Draw.Image(m_Root, -280, -280, 1120, 1120, "iuf_radial", ARGB(150, 5, 7, 10));

		m_FillLeft = INT_UIF_Draw.Texture(m_Root, 0, 0, HALF, SIZE, TEX_DIR + "vb_cam_sector_l_ca.paa", SECTOR_FILL);
		m_FillRight = INT_UIF_Draw.Texture(m_Root, HALF, 0, HALF, SIZE, TEX_DIR + "vb_cam_sector_r_ca.paa", SECTOR_FILL);
		m_ArcLeft = INT_UIF_Draw.Texture(m_Root, 0, 0, HALF, SIZE, TEX_DIR + "vb_cam_arc_l_ca.paa", teal);
		m_ArcRight = INT_UIF_Draw.Texture(m_Root, HALF, 0, HALF, SIZE, TEX_DIR + "vb_cam_arc_r_ca.paa", teal);

		INT_UIF_Draw.Icon(m_Root, HALF - LABEL_X - 17, 232, 34, "lock", muted);
		INT_UIF_Draw.Icon(m_Root, HALF + LABEL_X - 17, 232, 34, "eye", muted);
		m_NameLeft = INT_UIF_Draw.Text(m_Root, HALF - LABEL_X - 82, 274, 164, 26, "#STR_VB_VC_VANILLA", 19, muted, INT_UIF_Align.CENTER);
		m_NameRight = INT_UIF_Draw.Text(m_Root, HALF + LABEL_X - 82, 274, 164, 26, "#STR_VB_VC_FREE", 19, muted, INT_UIF_Align.CENTER);
		m_StateLeft = INT_UIF_Draw.Text(m_Root, HALF - LABEL_X - 82, 304, 164, 18, "", 12, teal, INT_UIF_Align.CENTER);
		m_StateRight = INT_UIF_Draw.Text(m_Root, HALF + LABEL_X - 82, 304, 164, 18, "", 12, teal, INT_UIF_Align.CENTER);

		m_Pointer = INT_UIF_Draw.Image(m_Root, 0, 0, POINTER_SIZE, POINTER_SIZE, "iuf_circle16", teal);

		float hubXY = HALF - HUB_SIZE * 0.5;
		INT_UIF_Draw.Texture(m_Root, hubXY, hubXY, HUB_SIZE, HUB_SIZE, TEX_DIR + "vb_cam_hub_ca.paa", ARGB(255, 255, 255, 255));
		m_HubCaption = INT_UIF_Draw.Text(m_Root, hubXY, HALF - 34, HUB_SIZE, 18, "", 12, faint, INT_UIF_Align.CENTER);
		m_HubName = INT_UIF_Draw.Text(m_Root, hubXY, HALF - 12, HUB_SIZE, 34, "", 21, text, INT_UIF_Align.CENTER);
	}

	protected void UpdateCursor(float dt)
	{
		float right = GetUApi().GetInputByID(UAAimRight).LocalValue() - GetUApi().GetInputByID(UAAimLeft).LocalValue();
		float up = GetUApi().GetInputByID(UAAimUp).LocalValue() - GetUApi().GetInputByID(UAAimDown).LocalValue();

		m_CursorX += right * dt * CURSOR_GAIN;
		m_CursorY -= up * dt * CURSOR_GAIN;

		float length = Math.Sqrt(m_CursorX * m_CursorX + m_CursorY * m_CursorY);
		if (length > CURSOR_MAX)
		{
			m_CursorX = m_CursorX / length * CURSOR_MAX;
			m_CursorY = m_CursorY / length * CURSOR_MAX;
		}

		// Секторов два, поэтому выбор решает только сторона. Глядя на мёртвую зону по длине, а не по X,
		// случайный уход мыши вверх-вниз не меняет выбор.
		m_Hover = -1;
		if (length > DEADZONE)
		{
			if (m_CursorX < 0)
				m_Hover = VB_VehicleCameraStore.MODE_VANILLA;
			else
				m_Hover = VB_VehicleCameraStore.MODE_FREE;
		}

		m_Pointer.Show(m_Hover >= 0);
		if (m_Hover >= 0 && length > 0.01)
		{
			float x = HALF + m_CursorX / length * POINTER_R - POINTER_SIZE * 0.5;
			float y = HALF + m_CursorY / length * POINTER_R - POINTER_SIZE * 0.5;
			INT_UIF_Draw.Move(m_Pointer, x, y);
		}
	}

	protected void Refresh(float dt)
	{
		VB_VehicleCameraSeat seat = VB_VehicleCameraRig.GetLocalSeat();
		if (!seat)
			return;

		bool hoverVanilla = m_Hover == VB_VehicleCameraStore.MODE_VANILLA;
		bool hoverFree = m_Hover == VB_VehicleCameraStore.MODE_FREE;

		INT_UIF_ThemeManager theme = INT_UIF_ThemeManager.Get();
		int text = theme.Color(INT_UIF_ThemeKeys.TEXT);
		int muted = theme.Color(INT_UIF_ThemeKeys.TEXT_MUTED);

		PaintSector(m_FillLeft, m_ArcLeft, m_NameLeft, hoverVanilla, seat.Mode == VB_VehicleCameraStore.MODE_VANILLA, text, muted);
		PaintSector(m_FillRight, m_ArcRight, m_NameRight, hoverFree, seat.Mode == VB_VehicleCameraStore.MODE_FREE, text, muted);

		string selected = Widget.TranslateString("#STR_VB_VC_SELECTED");
		if (seat.Mode == VB_VehicleCameraStore.MODE_VANILLA)
		{
			m_StateLeft.SetText(selected);
			m_StateRight.SetText("");
		}
		else
		{
			m_StateLeft.SetText("");
			m_StateRight.SetText(selected);
		}

		int tick = VB_VehicleCameraRig.GetZoomTick();
		if (tick != m_ZoomTick)
		{
			m_ZoomTick = tick;
			m_ZoomShow = ZOOM_SHOW_TIME;
		}
		m_ZoomShow = Math.Max(0, m_ZoomShow - dt);

		if (m_ZoomShow > 0)
		{
			m_HubCaption.SetText(Widget.TranslateString("#STR_VB_VC_DISTANCE"));
			m_HubName.SetText(FormatDistance(seat.Distance));
			m_HubName.SetColor(theme.Color(INT_UIF_ThemeKeys.TEAL));
		}
		else
		{
			int shown = seat.Mode;
			if (m_Hover >= 0)
				shown = m_Hover;

			m_HubCaption.SetText(Widget.TranslateString("#STR_VB_VC_CAMERA"));
			m_HubName.SetText(Widget.TranslateString(ModeNameKey(shown)));
			m_HubName.SetColor(text);
		}

		SetPill(PillKey(seat));
	}

	protected void PaintSector(ImageWidget fill, ImageWidget arc, TextWidget name, bool hover, bool current, int text, int muted)
	{
		if (hover)
			fill.SetColor(SECTOR_HOVER);
		else
			fill.SetColor(SECTOR_FILL);

		arc.Show(hover);

		if (hover || current)
			name.SetColor(text);
		else
			name.SetColor(muted);
	}

	protected string PillKey(VB_VehicleCameraSeat seat)
	{
		if (m_Hover == VB_VehicleCameraStore.MODE_VANILLA)
			return "#STR_VB_VC_VANILLA_DESC";

		if (m_Hover == VB_VehicleCameraStore.MODE_FREE)
			return "#STR_VB_VC_FREE_DESC";

		return "#STR_VB_VC_HINT_WHEEL";
	}

	protected void SetPill(string key)
	{
		if (key == m_PillShown)
			return;

		m_PillShown = key;

		// Ширину плашки считаем заново, поэтому пересоздаём её вместе с текстом.
		if (m_Pill)
		{
			m_Pill.m_Root.Unlink();
			m_Pill = null;
		}

		if (m_PillText)
		{
			m_PillText.Unlink();
			m_PillText = null;
		}

		if (key == "")
			return;

		INT_UIF_ThemeManager theme = INT_UIF_ThemeManager.Get();
		float width = INT_UIF_Draw.TextWidth(key, 15) + 36;
		float x = HALF - width * 0.5;
		m_Pill = INT_UIF_Surface.Box(m_Root, x, PILL_Y, width, PILL_H, 8, theme.Color(INT_UIF_ThemeKeys.WINDOW), theme.Color(INT_UIF_ThemeKeys.WINDOW_BORDER));
		m_PillText = INT_UIF_Draw.Text(m_Root, x, PILL_Y, width, PILL_H, Widget.TranslateString(key), 15, theme.Color(INT_UIF_ThemeKeys.TEXT_MUTED), INT_UIF_Align.CENTER);
	}

	protected string ModeNameKey(int mode)
	{
		if (mode == VB_VehicleCameraStore.MODE_VANILLA)
			return "#STR_VB_VC_VANILLA";

		return "#STR_VB_VC_FREE";
	}

	protected string FormatDistance(float distance)
	{
		if (distance <= 0)
			return Widget.TranslateString("#STR_VB_VC_DISTANCE_DEFAULT");

		string value = distance.ToString();
		value.Replace(".", ",");
		return "+" + value + " " + Widget.TranslateString("#STR_VB_VC_METERS");
	}

	protected void Close(bool apply)
	{
		VB_VehicleCameraSeat seat = VB_VehicleCameraRig.GetLocalSeat();

		if (apply && seat && m_Hover >= 0 && m_Hover != seat.Mode)
		{
			seat.Mode = m_Hover;
			VB_VehicleCameraStore.Save();
			INT_UIF_Toast.Show(Widget.TranslateString("#STR_VB_VC_TOAST") + ": " + Widget.TranslateString(ModeNameKey(seat.Mode)), INT_UIF_ToastKind.INFO, 2.0);
		}

		VB_VehicleCameraRig.SetMenuOpen(false);

		// Корень уходит на затухание и удаляется в TickFade; меню можно сразу открыть заново.
		DropFading();
		m_Fading = m_Root;
		m_FadeLeft = FADE_OUT_TIME;
		INT_UIF_Animator.FadeOut(m_Fading, FADE_OUT_TIME, false);

		m_Root = null;
		m_Pill = null;
		m_PillText = null;
	}

	protected void TickFade(float dt)
	{
		if (!m_Fading)
			return;

		m_FadeLeft -= dt;
		if (m_FadeLeft <= 0)
			DropFading();
	}

	protected void DropFading()
	{
		if (!m_Fading)
			return;

		INT_UIF_Animator.Get().Cancel(m_Fading);
		m_Fading.Unlink();
		m_Fading = null;
	}

	protected void Destroy()
	{
		VB_VehicleCameraRig.SetMenuOpen(false);
		DropFading();

		m_Pill = null;
		m_PillText = null;
		if (m_Root)
		{
			INT_UIF_Animator.Get().Cancel(m_Root);
			m_Root.Unlink();
			m_Root = null;
		}
	}
}

modded class MissionGameplay
{
	protected ref VB_VehicleCameraRadial m_VBCameraRadial;

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);

		if (!m_VBCameraRadial)
			m_VBCameraRadial = new VB_VehicleCameraRadial();

		m_VBCameraRadial.OnUpdate(timeslice);
	}

	override void OnMissionFinish()
	{
		m_VBCameraRadial = null;
		VB_VehicleCameraStore.Flush();

		super.OnMissionFinish();
	}
}
