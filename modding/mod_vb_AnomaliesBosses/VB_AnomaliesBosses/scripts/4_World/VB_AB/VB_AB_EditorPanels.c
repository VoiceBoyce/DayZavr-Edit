// Панели редактора точек по бокам экрана (раскладка как у редактора гнёзд Изнанки), открыты всё время,
// пока редактор включён. Слева — точки (ближние сверху) и боссы из bosses.json с фильтром по модели и палитре,
// справа — выбранная точка, босс на ней и статус. Середина экрана свободна для расстановки.
// Подсказок клавиш нет (просьба пользователя), все строки — stringtable (tools/strings/editor.json).

class VB_AB_EditorPanels
{
	static const float PANEL_W = 400;
	static const float TOP = 70;
	static const float EDGE = 16;
	static const float PANEL_H = 950;
	static const float REFILL_MOVE = 15.0;	// м: игрок отошёл — пересчитать расстояния в списке точек
	static const int SORT = 900;			// панели выше HUD; диалог подтверждения поднимается выше них
	static const float BH = 34;				// высота кнопки

	protected ref INT_UIF_Bag m_VB_Bag;
	protected Widget m_VB_Left;
	protected Widget m_VB_Right;

	// слева
	protected INT_UIF_List m_VB_PointList;
	protected INT_UIF_Dropdown m_VB_ModelDrop;
	protected INT_UIF_Dropdown m_VB_PalDrop;
	protected INT_UIF_List m_VB_BossList;
	protected ref TStringArray m_VB_Models = new TStringArray;		// токены фильтра (индекс 0 — «все»)
	protected ref TStringArray m_VB_Palettes = new TStringArray;
	protected string m_VB_FilterModel;
	protected string m_VB_FilterPal;

	// справа: точка
	protected TextWidget m_VB_PtTitle;
	protected TextWidget m_VB_PtSub;
	protected INT_UIF_List m_VB_PtBosses;
	protected INT_UIF_Button m_VB_BtnBossAdd;
	protected INT_UIF_Button m_VB_BtnBossDel;
	protected INT_UIF_Toggle m_VB_Enabled;
	protected INT_UIF_Stepper m_VB_Weight;
	protected INT_UIF_Button m_VB_BtnTp;
	protected INT_UIF_Button m_VB_BtnDelete;

	// справа: босс на точке
	protected TextWidget m_VB_LiveLine;
	protected TextWidget m_VB_FieldLine;
	protected INT_UIF_Button m_VB_BtnSummon;
	protected INT_UIF_Button m_VB_BtnDespawn;
	protected INT_UIF_Button m_VB_BtnOpen;
	protected INT_UIF_Button m_VB_BtnReroll;
	protected INT_UIF_Toggle m_VB_Radii;
	protected INT_UIF_Stepper m_VB_ActiveStep;	// точек за рестарт (ActivePointsPerRestart в settings.json)
	protected TextWidget m_VB_Rules;

	// статус
	protected TextWidget m_VB_StMode;
	protected TextWidget m_VB_StPlace;
	protected TextWidget m_VB_StTarget;
	protected TextWidget m_VB_StMsg;

	protected int m_VB_SeenVersion = -1;
	protected int m_VB_SeenCatalog = -1;
	protected int m_VB_SeenLive = -1;
	protected int m_VB_SeenListLive = -1;
	protected int m_VB_SeenPoint = -1;
	protected string m_VB_SeenBoss;
	protected string m_VB_SeenPick;
	protected string m_VB_SeenCounts;	// числа «(N)» у боссов: на скольких точках стоит
	protected vector m_VB_FillPos;
	protected bool m_VB_Filling;

	static string Tr(string key)
	{
		return VB_AB_EdText.Tr(key);
	}

	void VB_Build()
	{
		m_VB_Bag = new INT_UIF_Bag();
		float canvasW = INT_UIF_Scale.ScreenW() / INT_UIF_Scale.Factor();
		float w = PANEL_W;

		// --- слева: точки и боссы ---
		m_VB_Left = INT_UIF_Draw.Frame(null, EDGE, TOP, w, PANEL_H, "VB_ABEditorLeft");
		if (!m_VB_Left)
			return;
		m_VB_Left.SetSort(SORT);

		INT_UIF_Card points = INT_UIF_Card.Create(m_VB_Bag, m_VB_Left, 0, 0, w, 430, Tr("STR_VB_AB_Ed_CapPoints"), "pin", "");
		m_VB_PointList = INT_UIF_List.Create(m_VB_Bag, points.GetContent(), 0, 0, points.GetContentW(), points.GetContentH(), Tr("STR_VB_AB_Ed_NoPoints"));
		m_VB_PointList.SetSelectable(true);	// у списков фреймворка выбор по клику по умолчанию выключен
		m_VB_PointList.OnSelected().Insert(OnVBPointSelected);

		INT_UIF_Card bosses = INT_UIF_Card.Create(m_VB_Bag, m_VB_Left, 0, 442, w, PANEL_H - 442, Tr("STR_VB_AB_Ed_CapBosses"), "users", "");
		Widget bc = bosses.GetContent();
		float bw = bosses.GetContentW();
		float hw = (bw - 8) * 0.5;
		array<string> none = new array<string>;
		m_VB_ModelDrop = INT_UIF_Dropdown.Create(m_VB_Bag, bc, 0, 0, hw, none, -1, Tr("STR_VB_AB_Ed_AllModels"), 36);
		m_VB_ModelDrop.OnSelected().Insert(OnVBModelPicked);
		m_VB_PalDrop = INT_UIF_Dropdown.Create(m_VB_Bag, bc, hw + 8, 0, hw, none, -1, Tr("STR_VB_AB_Ed_AllPalettes"), 36);
		m_VB_PalDrop.OnSelected().Insert(OnVBPalettePicked);
		m_VB_BossList = INT_UIF_List.Create(m_VB_Bag, bc, 0, 46, bw, bosses.GetContentH() - 46, Tr("STR_VB_AB_Ed_NoBosses"));
		m_VB_BossList.SetSelectable(true);
		m_VB_BossList.OnSelected().Insert(OnVBBossPicked);

		// --- справа: точка, босс, статус ---
		m_VB_Right = INT_UIF_Draw.Frame(null, canvasW - EDGE - w, TOP, w, PANEL_H, "VB_ABEditorRight");
		if (!m_VB_Right)
			return;
		m_VB_Right.SetSort(SORT);

		float half;
		float y;
		INT_UIF_Card pt = INT_UIF_Card.Create(m_VB_Bag, m_VB_Right, 0, 0, w, 426, Tr("STR_VB_AB_Ed_CapPoint"), "target", "");
		Widget c = pt.GetContent();
		float cw = pt.GetContentW();
		half = (cw - 8) * 0.5;
		m_VB_PtTitle = INT_UIF_Draw.Text(c, 0, 0, cw, 26, "", 18, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT));
		if (m_VB_PtTitle)
			m_VB_PtTitle.SetBold(true);
		m_VB_PtSub = INT_UIF_Draw.Text(c, 0, 28, cw, 20, "", 13, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_MUTED));
		y = 54;
		m_VB_PtBosses = INT_UIF_List.Create(m_VB_Bag, c, 0, y, cw, 180, Tr("STR_VB_AB_Ed_PointNoBosses"));
		m_VB_PtBosses.SetSelectable(true);
		m_VB_PtBosses.OnSelected().Insert(OnVBPointBossSelected);
		y += 188;
		m_VB_BtnBossAdd = INT_UIF_Button.Create(m_VB_Bag, c, 0, y, half, BH, Tr("STR_VB_AB_Ed_BossAdd"), INT_UIF_Variant.SECONDARY, "plus");
		m_VB_BtnBossAdd.OnClick().Insert(OnVBBossAdd);
		m_VB_BtnBossDel = INT_UIF_Button.Create(m_VB_Bag, c, half + 8, y, half, BH, Tr("STR_VB_AB_Ed_BossRemove"), INT_UIF_Variant.SECONDARY, "minus");
		m_VB_BtnBossDel.OnClick().Insert(OnVBBossDel);
		y += BH + 8;
		m_VB_Enabled = INT_UIF_Toggle.Create(m_VB_Bag, c, 0, y, 170, Tr("STR_VB_AB_Ed_Enabled"), true, "", BH);
		m_VB_Enabled.OnChanged().Insert(OnVBEnabledChanged);
		INT_UIF_Draw.Text(c, 180, y, 46, BH, Tr("STR_VB_AB_Ed_Weight"), 14, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT), INT_UIF_Align.RIGHT);
		m_VB_Weight = INT_UIF_Stepper.Create(m_VB_Bag, c, 232, y, cw - 232, 1, 100, 1, 1, "", BH);
		m_VB_Weight.OnChanged().Insert(OnVBWeightChanged);
		y += BH + 8;
		m_VB_BtnTp = INT_UIF_Button.Create(m_VB_Bag, c, 0, y, half, BH, Tr("STR_VB_AB_Ed_Teleport"), INT_UIF_Variant.SECONDARY, "pin");
		m_VB_BtnTp.OnClick().Insert(OnVBTeleport);
		m_VB_BtnDelete = INT_UIF_Button.Create(m_VB_Bag, c, half + 8, y, half, BH, Tr("STR_VB_AB_Ed_DeletePoint"), INT_UIF_Variant.DANGER, "trash");
		m_VB_BtnDelete.OnClick().Insert(OnVBDelete);

		INT_UIF_Card boss = INT_UIF_Card.Create(m_VB_Bag, m_VB_Right, 0, 438, w, 356, Tr("STR_VB_AB_Ed_CapBoss"), "bolt", "");
		c = boss.GetContent();
		cw = boss.GetContentW();
		half = (cw - 8) * 0.5;
		m_VB_LiveLine = INT_UIF_Draw.Text(c, 0, 0, cw, 20, "", 14, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT));
		m_VB_FieldLine = INT_UIF_Draw.Text(c, 0, 22, cw, 20, "", 13, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_MUTED));
		y = 50;
		m_VB_BtnSummon = INT_UIF_Button.Create(m_VB_Bag, c, 0, y, half, BH, Tr("STR_VB_AB_Ed_Summon"), INT_UIF_Variant.PRIMARY, "");
		m_VB_BtnSummon.OnClick().Insert(OnVBSummon);
		m_VB_BtnDespawn = INT_UIF_Button.Create(m_VB_Bag, c, half + 8, y, half, BH, Tr("STR_VB_AB_Ed_Despawn"), INT_UIF_Variant.DANGER, "");
		m_VB_BtnDespawn.OnClick().Insert(OnVBDespawn);
		y += BH + 8;
		m_VB_BtnOpen = INT_UIF_Button.Create(m_VB_Bag, c, 0, y, half, BH, Tr("STR_VB_AB_Ed_FieldOpen"), INT_UIF_Variant.SECONDARY, "");
		m_VB_BtnOpen.OnClick().Insert(OnVBFieldOpen);
		m_VB_BtnReroll = INT_UIF_Button.Create(m_VB_Bag, c, half + 8, y, half, BH, Tr("STR_VB_AB_Ed_FieldReroll"), INT_UIF_Variant.WARNING, "");
		m_VB_BtnReroll.OnClick().Insert(OnVBFieldReroll);
		y += BH + 8;
		m_VB_Radii = INT_UIF_Toggle.Create(m_VB_Bag, c, 0, y, cw, Tr("STR_VB_AB_Ed_Radii"), false, "", 32);
		m_VB_Radii.OnChanged().Insert(OnVBRadiiChanged);
		y += 42;
		INT_UIF_Divider.Create(m_VB_Bag, c, 0, y, cw);
		y += 10;
		INT_UIF_Draw.Text(c, 0, y, cw - 150, BH, Tr("STR_VB_AB_Ed_PerRestart"), 14, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT));
		m_VB_ActiveStep = INT_UIF_Stepper.Create(m_VB_Bag, c, cw - 140, y, 140, 1, 100, Math.Max(1, VB_AB_EditorClient.s_ActivePerRestart), 1, "", BH);
		m_VB_ActiveStep.OnChanged().Insert(OnVBActiveChanged);
		y += BH + 6;
		m_VB_Rules = INT_UIF_Draw.Text(c, 0, y, cw, 18, "", 12, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_FAINT));
		y += 26;
		INT_UIF_Button reload = INT_UIF_Button.Create(m_VB_Bag, c, 0, y, half, BH, Tr("STR_VB_AB_Ed_Reload"), INT_UIF_Variant.SECONDARY, "");
		reload.OnClick().Insert(OnVBReload);
		INT_UIF_Button quit = INT_UIF_Button.Create(m_VB_Bag, c, half + 8, y, half, BH, Tr("STR_VB_AB_Ed_Quit"), INT_UIF_Variant.GHOST, "logout");
		quit.OnClick().Insert(OnVBQuit);

		INT_UIF_Card status = INT_UIF_Card.Create(m_VB_Bag, m_VB_Right, 0, 806, w, PANEL_H - 806, Tr("STR_VB_AB_Ed_CapStatus"), "info", "");
		Widget s = status.GetContent();
		float sw = status.GetContentW();
		m_VB_StMode = INT_UIF_Draw.Text(s, 0, 0, sw, 18, "", 12, INT_UIF.Color(INT_UIF_ThemeKeys.TEAL));
		m_VB_StPlace = INT_UIF_Draw.Text(s, 0, 20, sw, 20, "", 13, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT));
		m_VB_StTarget = INT_UIF_Draw.Text(s, 0, 41, sw, 18, "", 12, INT_UIF.Color(INT_UIF_ThemeKeys.AMBER_TEXT));
		m_VB_StMsg = INT_UIF_Draw.Text(s, 0, 60, sw, 18, "", 12, INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_FAINT));

		VB_FillAll();
	}

	void VB_Destroy()
	{
		m_VB_Bag = null;	// сначала компоненты (они сами снимают свои виджеты), потом корни
		if (m_VB_Left)
			m_VB_Left.Unlink();
		if (m_VB_Right)
			m_VB_Right.Unlink();
		m_VB_Left = null;
		m_VB_Right = null;
	}

	void VB_Show(bool show)
	{
		if (m_VB_Left)
			m_VB_Left.Show(show);
		if (m_VB_Right)
			m_VB_Right.Show(show);
	}

	bool VB_Contains(int x, int y)
	{
		if (INT_UIF_Widgets.Contains(m_VB_Left, x, y) || INT_UIF_Widgets.Contains(m_VB_Right, x, y))
			return true;
		// раскрытый список фильтра может выйти за панель
		if (m_VB_ModelDrop && m_VB_ModelDrop.IsOpen() && INT_UIF_Widgets.Contains(m_VB_ModelDrop.GetPopup(), x, y))
			return true;
		if (m_VB_PalDrop && m_VB_PalDrop.IsOpen() && INT_UIF_Widgets.Contains(m_VB_PalDrop.GetPopup(), x, y))
			return true;
		return false;
	}

	void VB_Tick(VB_AB_Editor ed)
	{
		vector ppos = vector.Zero;
		if (g_Game.GetPlayer())
			ppos = g_Game.GetPlayer().GetPosition();

		// список точек перерисовывается (и теряет прокрутку) только когда сменились точки или их состояние
		if (m_VB_SeenVersion != VB_AB_EditorClient.s_Version)
			VB_FillAll();
		else if (m_VB_SeenListLive != VB_AB_EditorClient.s_LiveListVersion || m_VB_SeenPoint != ed.m_VB_CurPoint || VB_AB_Editor.VB_DistXZ(ppos, m_VB_FillPos) > REFILL_MOVE)
			VB_FillPoints();
		else if (m_VB_SeenLive != VB_AB_EditorClient.s_LiveVersion || m_VB_SeenBoss != ed.m_VB_CurBoss)
			VB_FillCurrent();
		if (m_VB_SeenPick != ed.m_VB_Pick)
			VB_SyncPick(ed);
		if (m_VB_Radii && m_VB_Radii.IsOn() != ed.m_VB_ShowRadii)
			m_VB_Radii.SetOn(ed.m_VB_ShowRadii, false);
		VB_UpdateStatus(ed);
	}

	// --- заполнение -------------------------------------------------------------------------------

	protected void VB_FillAll()
	{
		m_VB_SeenVersion = VB_AB_EditorClient.s_Version;
		if (m_VB_SeenCatalog != VB_AB_EditorClient.s_CatalogVersion)
		{
			m_VB_SeenCatalog = VB_AB_EditorClient.s_CatalogVersion;
			VB_FillFilters();
			VB_FillBosses();
		}
		else if (m_VB_SeenCounts != VB_CountsSig(VB_BossCounts()))
		{
			VB_FillBosses();	// поставили/удалили точку или босса в ней — обновить числа в скобках
		}
		VB_FillPoints();
		if (m_VB_ActiveStep)
			m_VB_ActiveStep.SetValue(Math.Max(1, VB_AB_EditorClient.s_ActivePerRestart), false);
	}

	// фильтр: модели и палитры, которые есть в каталоге (по порядку появления)
	protected void VB_FillFilters()
	{
		if (!m_VB_ModelDrop || !m_VB_PalDrop)
			return;

		m_VB_Models.Clear();
		m_VB_Palettes.Clear();
		m_VB_Models.Insert("");
		m_VB_Palettes.Insert("");
		foreach (VB_AB_EdBoss b : VB_AB_EditorClient.s_Bosses)
		{
			if (b.Model != "" && m_VB_Models.Find(b.Model) < 0)
				m_VB_Models.Insert(b.Model);
			if (b.Palette != "" && m_VB_Palettes.Find(b.Palette) < 0)
				m_VB_Palettes.Insert(b.Palette);
		}

		array<string> names = new array<string>;
		names.Insert(Tr("STR_VB_AB_Ed_AllModels"));
		for (int i = 1; i < m_VB_Models.Count(); i++)
		{
			names.Insert(VB_AB_EdText.ModelName(m_VB_Models[i]));
		}
		m_VB_ModelDrop.SetOptions(names);
		int mi = m_VB_Models.Find(m_VB_FilterModel);
		if (mi < 0)
		{
			mi = 0;
			m_VB_FilterModel = "";
		}
		m_VB_ModelDrop.SetSelected(mi, false);

		array<string> pals = new array<string>;
		pals.Insert(Tr("STR_VB_AB_Ed_AllPalettes"));
		for (int k = 1; k < m_VB_Palettes.Count(); k++)
		{
			pals.Insert(VB_AB_EdText.PaletteName(m_VB_Palettes[k]));
		}
		m_VB_PalDrop.SetOptions(pals);
		int pi = m_VB_Palettes.Find(m_VB_FilterPal);
		if (pi < 0)
		{
			pi = 0;
			m_VB_FilterPal = "";
		}
		m_VB_PalDrop.SetSelected(pi, false);
	}

	// автотест: фильтр по токенам (* — все)
	void VB_SetFilter(string model, string palette)
	{
		m_VB_FilterModel = model;
		if (model == "*")
			m_VB_FilterModel = "";
		m_VB_FilterPal = palette;
		if (palette == "*")
			m_VB_FilterPal = "";
		VB_FillFilters();
		VB_FillBosses();
	}

	protected void VB_FillBosses()
	{
		if (!m_VB_BossList)
			return;

		VB_AB_Editor ed = VB_AB_Editor.VB_Get();
		m_VB_SeenPick = ed.m_VB_Pick;
		map<string, int> counts = VB_BossCounts();
		m_VB_SeenCounts = VB_CountsSig(counts);
		m_VB_Filling = true;
		m_VB_BossList.Clear();
		VB_AB_EdBoss b;
		int r;
		int n;
		string title;
		for (int i = 0; i < VB_AB_EditorClient.s_Bosses.Count(); i++)
		{
			b = VB_AB_EditorClient.s_Bosses[i];
			if (m_VB_FilterModel != "" && b.Model != m_VB_FilterModel)
				continue;
			if (m_VB_FilterPal != "" && b.Palette != m_VB_FilterPal)
				continue;
			// «Болотный Энт (2)» — на скольких точках уже стоит этот босс
			n = 0;
			counts.Find(b.Id, n);
			title = b.Name + " (" + n.ToString() + ")";
			r = m_VB_BossList.AddRow(title, VB_BossSub(b), i);
			m_VB_BossList.SetRowIcon(r, "star", VB_TierHue(b.Tier));
			if (b.Id == ed.m_VB_Pick)
				m_VB_BossList.Select(r, false);
		}
		m_VB_Filling = false;
	}

	// сколько точек с каждым боссом (id → число точек; босс дважды в одной точке считается один раз)
	static map<string, int> VB_BossCounts()
	{
		map<string, int> counts = new map<string, int>;
		TStringArray seen = new TStringArray;
		int n;
		foreach (VB_AB_EdPoint p : VB_AB_EditorClient.s_Points)
		{
			seen.Clear();
			foreach (string id : p.Bosses)
			{
				if (seen.Find(id) >= 0)
					continue;
				seen.Insert(id);
				n = 0;
				counts.Find(id, n);
				counts.Set(id, n + 1);
			}
		}
		return counts;
	}

	// подпись чисел по порядку каталога: сменилась — список боссов перерисовать
	static string VB_CountsSig(map<string, int> counts)
	{
		string sig = "";
		int n;
		foreach (VB_AB_EdBoss b : VB_AB_EditorClient.s_Bosses)
		{
			n = 0;
			counts.Find(b.Id, n);
			sig += n.ToString() + ",";
		}
		return sig;
	}

	// выбор босса сбросился после установки — снять выделение в списке
	protected void VB_SyncPick(VB_AB_Editor ed)
	{
		VB_FillBosses();
	}

	static string VB_BossSub(VB_AB_EdBoss b)
	{
		return string.Format(Tr("STR_VB_AB_Ed_BossRow"), VB_AB_EdText.ModelName(b.Model), VB_AB_EdText.PaletteName(b.Palette),
			b.Tier, Math.Round(b.Radius));
	}

	static string VB_TierHue(int tier)
	{
		if (tier >= 3)
			return INT_UIF_ThemeKeys.RED;
		if (tier == 2)
			return INT_UIF_ThemeKeys.AMBER;
		return INT_UIF_ThemeKeys.TEAL;
	}

	protected void VB_FillPoints()
	{
		if (!m_VB_PointList)
			return;

		VB_AB_Editor ed = VB_AB_Editor.VB_Get();
		m_VB_SeenListLive = VB_AB_EditorClient.s_LiveListVersion;
		m_VB_SeenPoint = ed.m_VB_CurPoint;

		vector ppos = vector.Zero;
		if (g_Game.GetPlayer())
			ppos = g_Game.GetPlayer().GetPosition();
		m_VB_FillPos = ppos;

		// ближние сверху
		array<VB_AB_EdPoint> sorted = new array<VB_AB_EdPoint>;
		foreach (VB_AB_EdPoint any : VB_AB_EditorClient.s_Points)
		{
			sorted.Insert(any);
		}
		int count = sorted.Count();
		int bi;
		float bd;
		float d;
		for (int i = 0; i < count; i++)
		{
			bi = i;
			bd = VB_AB_Editor.VB_DistXZ(sorted[i].Position, ppos);
			for (int j = i + 1; j < count; j++)
			{
				d = VB_AB_Editor.VB_DistXZ(sorted[j].Position, ppos);
				if (d < bd)
				{
					bd = d;
					bi = j;
				}
			}
			if (bi != i)
				sorted.SwapItems(i, bi);
		}

		m_VB_Filling = true;
		m_VB_PointList.Clear();
		int r;
		string title;
		string sub;
		string subFmt = Tr("STR_VB_AB_Ed_RowSub");
		string moreFmt = Tr("STR_VB_AB_Ed_RowMore");
		int distM;
		string weightText;
		foreach (VB_AB_EdPoint p : sorted)
		{
			title = "#" + p.Id.ToString();
			if (p.Bosses.Count() > 0)
				title += " · " + VB_AB_EditorClient.VB_BossName(p.Bosses[0]);
			if (p.Bosses.Count() > 1)
				title += string.Format(moreFmt, p.Bosses.Count() - 1);
			// сначала в локальные: Math.Round в аргументе затирался вызовом Math.Round внутри VB_Num,
			// и вместо расстояния выводился вес (баг №6: у всех точек «1 м»)
			distM = Math.Round(VB_AB_Editor.VB_DistXZ(p.Position, ppos));
			weightText = VB_Num(p.Weight);
			sub = string.Format(subFmt, distM, weightText);
			r = m_VB_PointList.AddRow(title, sub, p.Id);
			VB_PointRowLook(r, p);
			if (p.Id == ed.m_VB_CurPoint)
				m_VB_PointList.Select(r, false);
		}
		m_VB_Filling = false;

		VB_FillCurrent();
	}

	// значок и плашка строки точки по её состоянию
	protected void VB_PointRowLook(int r, VB_AB_EdPoint p)
	{
		VB_AB_EdLive lv = VB_AB_EditorClient.VB_FindLive(p.Id);
		if (!VB_HasKnownBoss(p))
		{
			m_VB_PointList.SetRowIcon(r, "warning", INT_UIF_ThemeKeys.AMBER);
			m_VB_PointList.SetRowBadge(r, Tr("STR_VB_AB_Ed_BadgeNoBoss"), INT_UIF_Variant.WARNING);
			return;
		}
		if (lv && (lv.Kind == VB_AB_EditorNet.LIVE_AWAKE || lv.Kind == VB_AB_EditorNet.LIVE_FIELD))
		{
			m_VB_PointList.SetRowIcon(r, "target", INT_UIF_ThemeKeys.RED);
			if (lv.Kind == VB_AB_EditorNet.LIVE_FIELD)
				m_VB_PointList.SetRowBadge(r, Tr("STR_VB_AB_Ed_BadgeField"), INT_UIF_Variant.DANGER);
			else
				m_VB_PointList.SetRowBadge(r, Tr("STR_VB_AB_Ed_BadgeAwake"), INT_UIF_Variant.DANGER);
			return;
		}
		if (p.Disabled != 0)
		{
			m_VB_PointList.SetRowIcon(r, "target", INT_UIF_ThemeKeys.TEXT_FAINT);
			m_VB_PointList.SetRowBadge(r, Tr("STR_VB_AB_Ed_BadgeOff"), INT_UIF_Variant.SECONDARY);
			return;
		}
		if (lv && (lv.Kind == VB_AB_EditorNet.LIVE_DONE || lv.Kind == VB_AB_EditorNet.LIVE_RESPAWN))
		{
			m_VB_PointList.SetRowIcon(r, "target", INT_UIF_ThemeKeys.VIOLET);
			m_VB_PointList.SetRowBadge(r, Tr("STR_VB_AB_Ed_BadgeDone"), INT_UIF_Variant.SECONDARY);
			return;
		}
		if (lv)
		{
			m_VB_PointList.SetRowIcon(r, "target", INT_UIF_ThemeKeys.GREEN);
			m_VB_PointList.SetRowBadge(r, Tr("STR_VB_AB_Ed_BadgeLive"), INT_UIF_Variant.SUCCESS);
			return;
		}
		m_VB_PointList.SetRowIcon(r, "target", INT_UIF_ThemeKeys.TEXT_MUTED);
	}

	static bool VB_HasKnownBoss(VB_AB_EdPoint p)
	{
		foreach (string id : p.Bosses)
		{
			if (VB_AB_EditorClient.VB_FindBoss(id))
				return true;
		}
		return false;
	}

	// число без лишних нулей: 1, 2.5
	static string VB_Num(float v)
	{
		float r = Math.Round(v * 10) / 10;
		int whole = Math.Round(r);
		if (r == whole)
			return whole.ToString();
		return r.ToString();
	}

	protected void VB_FillCurrent()
	{
		if (!m_VB_PtTitle || !m_VB_LiveLine)
			return;		// правая панель не построилась
		VB_AB_Editor ed = VB_AB_Editor.VB_Get();
		m_VB_SeenBoss = ed.m_VB_CurBoss;
		m_VB_SeenLive = VB_AB_EditorClient.s_LiveVersion;
		VB_AB_EdPoint p = VB_AB_EditorClient.VB_FindPoint(ed.m_VB_CurPoint);
		bool has = p != null;
		m_VB_BtnBossAdd.SetEnabled(has);
		m_VB_BtnTp.SetEnabled(has);
		m_VB_BtnDelete.SetEnabled(has);
		m_VB_Enabled.SetEnabled(has);
		m_VB_Weight.SetEnabled(has);

		m_VB_Filling = true;
		m_VB_PtBosses.Clear();
		m_VB_Filling = false;
		if (!has)
		{
			m_VB_PtTitle.SetText(Tr("STR_VB_AB_Ed_NoPointTitle"));
			m_VB_PtSub.SetText(Tr("STR_VB_AB_Ed_NoPointSub"));
			m_VB_BtnBossDel.SetEnabled(false);
			m_VB_LiveLine.SetText("");
			m_VB_FieldLine.SetText("");
			m_VB_BtnSummon.SetEnabled(false);
			m_VB_BtnDespawn.SetEnabled(false);
			m_VB_BtnOpen.SetEnabled(false);
			m_VB_BtnReroll.SetEnabled(false);
			VB_FillRules();
			return;
		}

		// точка
		m_VB_PtTitle.SetText(string.Format(Tr("STR_VB_AB_Ed_PointTitle"), p.Id));
		vector ppos = vector.Zero;
		if (g_Game.GetPlayer())
			ppos = g_Game.GetPlayer().GetPosition();
		// каждое число в свою переменную до Format (три Math.Round в одном списке аргументов затирают друг друга)
		string subFmt = Tr("STR_VB_AB_Ed_PointSub");
		int distM = Math.Round(VB_AB_Editor.VB_DistXZ(p.Position, ppos));
		int px = Math.Round(p.Position[0]);
		int pz = Math.Round(p.Position[2]);
		string sub = string.Format(subFmt, distM, px, pz);
		m_VB_PtSub.SetText(sub);
		m_VB_Enabled.SetOn(p.Disabled == 0, false, false);
		m_VB_Weight.SetValue(p.Weight, false);

		// её боссы
		m_VB_Filling = true;
		int r;
		bool curFound = false;
		VB_AB_EdBoss b;
		for (int i = 0; i < p.Bosses.Count(); i++)
		{
			b = VB_AB_EditorClient.VB_FindBoss(p.Bosses[i]);
			if (b)
			{
				r = m_VB_PtBosses.AddRow(b.Name, VB_BossSub(b), i);
				m_VB_PtBosses.SetRowIcon(r, "star", VB_TierHue(b.Tier));
			}
			else
			{
				r = m_VB_PtBosses.AddRow(p.Bosses[i], "", i);
				m_VB_PtBosses.SetRowIcon(r, "warning", INT_UIF_ThemeKeys.AMBER);
				m_VB_PtBosses.SetRowBadge(r, Tr("STR_VB_AB_Ed_BadgeMissing"), INT_UIF_Variant.WARNING);
			}
			if (p.Bosses[i] == ed.m_VB_CurBoss)
			{
				m_VB_PtBosses.Select(r, false);
				curFound = true;
			}
		}
		m_VB_Filling = false;
		if (!curFound)
			ed.m_VB_CurBoss = "";
		m_VB_SeenBoss = ed.m_VB_CurBoss;
		m_VB_BtnBossDel.SetEnabled(ed.m_VB_CurBoss != "");

		// босс на точке
		VB_AB_EdLive lv = VB_AB_EditorClient.VB_FindLive(p.Id);
		bool alive = lv && (lv.Kind == VB_AB_EditorNet.LIVE_AWAKE || lv.Kind == VB_AB_EditorNet.LIVE_FIELD);
		bool field = lv && lv.Kind == VB_AB_EditorNet.LIVE_FIELD;
		bool known = VB_HasKnownBoss(p);
		string liveText = Tr("STR_VB_AB_Ed_LiveNone");
		string fieldText = "";
		if (lv)
		{
			if (alive)
			{
				string awakeFmt = Tr("STR_VB_AB_Ed_LiveAwake");
				string awakeName = VB_AB_EditorClient.VB_BossName(lv.BossId);
				int hpPct = Math.Round(lv.Hp * 100);
				liveText = string.Format(awakeFmt, awakeName, hpPct);
				if (field)
				{
					string fieldFmt = Tr("STR_VB_AB_Ed_FieldOpenLine");
					string anomText = VB_AB_EdText.Count(lv.Anomalies, "STR_VB_AB_Ed_Anom1", "STR_VB_AB_Ed_Anom2", "STR_VB_AB_Ed_Anom5");
					fieldText = string.Format(fieldFmt, anomText);
				}
				else
					fieldText = Tr("STR_VB_AB_Ed_FieldClosedLine");
			}
			else if (lv.Kind == VB_AB_EditorNet.LIVE_DONE)
				liveText = Tr("STR_VB_AB_Ed_LiveDone");
			else if (lv.Kind == VB_AB_EditorNet.LIVE_RESPAWN)
				liveText = string.Format(Tr("STR_VB_AB_Ed_LiveRespawn"), Math.Ceil(lv.RespawnSec / 60.0));
			else
				liveText = Tr("STR_VB_AB_Ed_LiveWait");
		}
		m_VB_LiveLine.SetText(liveText);
		m_VB_FieldLine.SetText(fieldText);
		m_VB_BtnSummon.SetEnabled(known && !alive);
		m_VB_BtnDespawn.SetEnabled(alive);
		m_VB_BtnOpen.SetEnabled(known && !field);
		m_VB_BtnReroll.SetEnabled(alive);
		VB_FillRules();
	}

	protected void VB_FillRules()
	{
		int inRestart = VB_AB_EditorClient.s_Live.Count();
		int awake = 0;
		foreach (int pid, VB_AB_EdLive lv : VB_AB_EditorClient.s_Live)
		{
			if (lv.Kind == VB_AB_EditorNet.LIVE_AWAKE || lv.Kind == VB_AB_EditorNet.LIVE_FIELD)
				awake++;
		}
		if (m_VB_Rules)
			m_VB_Rules.SetText(string.Format(Tr("STR_VB_AB_Ed_Rules"), inRestart, awake, VB_AB_EditorClient.s_MaxAwake, Math.Round(VB_AB_EditorClient.s_MinDist)));
	}

	protected void VB_UpdateStatus(VB_AB_Editor ed)
	{
		string modeLine = Tr("STR_VB_AB_Ed_ModeUi");
		if (ed.m_VB_Control)
			modeLine = Tr("STR_VB_AB_Ed_ModeControl");

		string placeLine = Tr("STR_VB_AB_Ed_PlaceNone");
		if (ed.m_VB_Pick != "")
		{
			placeLine = string.Format(Tr("STR_VB_AB_Ed_PlacePick"), VB_AB_EditorClient.VB_BossName(ed.m_VB_Pick), VB_Num(ed.m_VB_Height));
			if (!ed.VB_GhostOk() && ed.VB_GhostNote() != "")
				placeLine += " · " + ed.VB_GhostNote();
		}
		else if (ed.m_VB_Height != 0)
		{
			placeLine += " · " + string.Format(Tr("STR_VB_AB_Ed_HeightOnly"), VB_Num(ed.m_VB_Height));
		}

		string targetLine = "";
		if (ed.VB_TargetId() != 0)
			targetLine = string.Format(Tr("STR_VB_AB_Ed_Target"), ed.VB_TargetId());

		string msgLine = "";
		if (g_Game.GetTime() < ed.m_VB_MsgUntil)
			msgLine = ed.m_VB_Msg;

		if (m_VB_StMode)
		{
			m_VB_StMode.SetText(modeLine);
			m_VB_StPlace.SetText(placeLine);
			m_VB_StTarget.SetText(targetLine);
			m_VB_StMsg.SetText(msgLine);
		}
	}

	// --- события ----------------------------------------------------------------------------------

	// клик по точке — выделить и перенестись к ней (ТЗ 6)
	protected void OnVBPointSelected(int row, INT_UIF_List list)
	{
		if (m_VB_Filling || row < 0)
			return;
		VB_AB_Editor ed = VB_AB_Editor.VB_Get();
		ed.VB_Select(list.GetData(row), true);
		m_VB_SeenPoint = ed.m_VB_CurPoint;
		VB_FillCurrent();
	}

	protected void OnVBModelPicked(int index, string text, INT_UIF_Dropdown dd)
	{
		if (index < 0 || index >= m_VB_Models.Count())
			return;
		m_VB_FilterModel = m_VB_Models[index];
		VB_FillBosses();
	}

	protected void OnVBPalettePicked(int index, string text, INT_UIF_Dropdown dd)
	{
		if (index < 0 || index >= m_VB_Palettes.Count())
			return;
		m_VB_FilterPal = m_VB_Palettes[index];
		VB_FillBosses();
	}

	// босс слева — что ставить (призрак под курсором) или кого добавить в точку
	protected void OnVBBossPicked(int row, INT_UIF_List list)
	{
		if (m_VB_Filling || row < 0)
			return;
		int i = list.GetData(row);
		if (i < 0 || i >= VB_AB_EditorClient.s_Bosses.Count())
			return;
		VB_AB_Editor ed = VB_AB_Editor.VB_Get();
		ed.m_VB_Pick = VB_AB_EditorClient.s_Bosses[i].Id;
		m_VB_SeenPick = ed.m_VB_Pick;
	}

	protected void OnVBPointBossSelected(int row, INT_UIF_List list)
	{
		if (m_VB_Filling || row < 0)
			return;
		VB_AB_Editor ed = VB_AB_Editor.VB_Get();
		VB_AB_EdPoint p = VB_AB_EditorClient.VB_FindPoint(ed.m_VB_CurPoint);
		int i = list.GetData(row);
		if (!p || i < 0 || i >= p.Bosses.Count())
			return;
		ed.m_VB_CurBoss = p.Bosses[i];
		m_VB_SeenBoss = ed.m_VB_CurBoss;
		m_VB_BtnBossDel.SetEnabled(true);
	}

	protected void OnVBBossAdd(INT_UIF_Button b)
	{
		VB_AB_Editor.VB_Get().VB_AddPickedBoss();
	}

	protected void OnVBBossDel(INT_UIF_Button b)
	{
		VB_AB_Editor ed = VB_AB_Editor.VB_Get();
		ed.VB_RemoveBoss(ed.m_VB_CurBoss);
	}

	protected void OnVBEnabledChanged(bool on, INT_UIF_Toggle t)
	{
		VB_AB_Editor.VB_Get().VB_SetDisabled(!on);
	}

	protected void OnVBWeightChanged(float value, INT_UIF_Stepper stepper)
	{
		VB_AB_Editor.VB_Get().VB_SetWeight(value);
	}

	protected void OnVBTeleport(INT_UIF_Button b)
	{
		VB_AB_Editor ed = VB_AB_Editor.VB_Get();
		ed.VB_Select(ed.m_VB_CurPoint, true);
	}

	protected void OnVBDelete(INT_UIF_Button b)
	{
		VB_AB_Editor ed = VB_AB_Editor.VB_Get();
		if (ed.m_VB_CurPoint == 0)
			return;
		INT_UIF_Dialog d = INT_UIF_Dialog.Confirm(string.Format(Tr("STR_VB_AB_Ed_DelTitle"), ed.m_VB_CurPoint), Tr("STR_VB_AB_Ed_DelText"), Tr("STR_VB_AB_Ed_DelYes"), Tr("STR_VB_AB_Ed_DelNo"), INT_UIF_Variant.DANGER);
		if (d)
			d.OnResult().Insert(OnVBDeleteConfirm);
	}

	protected void OnVBDeleteConfirm(int result, INT_UIF_Dialog dlg)
	{
		if (result == INT_UIF_Dialog.RESULT_YES)
		{
			VB_AB_Editor ed = VB_AB_Editor.VB_Get();
			ed.VB_DeletePoint(ed.m_VB_CurPoint);
		}
	}

	protected void OnVBSummon(INT_UIF_Button b)
	{
		VB_AB_Editor.VB_Get().VB_Summon();
	}

	protected void OnVBDespawn(INT_UIF_Button b)
	{
		VB_AB_Editor.VB_Get().VB_Despawn();
	}

	protected void OnVBFieldOpen(INT_UIF_Button b)
	{
		VB_AB_Editor.VB_Get().VB_FieldOpen();
	}

	protected void OnVBFieldReroll(INT_UIF_Button b)
	{
		VB_AB_Editor.VB_Get().VB_FieldReroll();
	}

	protected void OnVBRadiiChanged(bool on, INT_UIF_Toggle t)
	{
		VB_AB_Editor.VB_Get().m_VB_ShowRadii = on;
	}

	// число точек за рестарт: сразу на сервер (settings.json), действует со следующего рестарта
	protected void OnVBActiveChanged(float value, INT_UIF_Stepper stepper)
	{
		VB_AB_Editor.VB_Get().VB_SetActivePerRestart(Math.Round(value));
	}

	protected void OnVBReload(INT_UIF_Button b)
	{
		VB_AB_Editor.VB_Get().VB_Reload();
	}

	protected void OnVBQuit(INT_UIF_Button b)
	{
		VB_AB_Editor.VB_Get().VB_Deactivate();
	}
}
