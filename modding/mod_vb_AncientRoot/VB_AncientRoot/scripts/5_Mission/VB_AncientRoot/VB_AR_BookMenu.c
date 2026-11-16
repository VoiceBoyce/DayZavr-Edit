// Окно «Книги древних» (ТЗ_сервер_18.md, ветка 2в; дизайн — на усмотрение, решение пользователя 2026-11-16). Только клиент.
// Разворот 1240 x 820 (единицы 1/1080 высоты экрана): переплет из коры, две страницы пергамента, сгиб, справа закладки
// глав. Листать — стрелки-листья в нижних углах, колесо мыши над книгой, закладки. Закрыть — Esc или клик мимо книги;
// окно закрывается само, если книги больше нет в руках. Пока открыто — худ спрятан, фон размыт, управление заблокировано.
// Картинки — живые модели предметов: ItemPreviewWidget над локальными копиями (создаются далеко от игрока, высоко над
// землей, и удаляются при закрытии), медленно вращаются. Страница выше расположения игрока — руническая вязь и подсказка,
// с какой ступени она прочтется. Тексты — 4_World/VB_AR_BookText.c, разметка — tools/gen_ar_book_layout.py,
// текстуры — tools/gen_ar_book.py (gui/book), ветвь и руна — те же, что в худе.

class VB_AR_BookSide
{
	Widget Root;
	TextWidget Title;
	ImageWidget Orn;
	ItemPreviewWidget Pic;
	RichTextWidget Body;
	ImageWidget NoteBg;
	RichTextWidget Note;
	TextWidget Num;
	ImageWidget Runes;
	RichTextWidget Lock;
	Widget Ent1;
	Widget Ent2;
	ItemPreviewWidget EntPic1;
	ItemPreviewWidget EntPic2;
	TextWidget EntName1;
	TextWidget EntName2;
	RichTextWidget EntText1;
	RichTextWidget EntText2;

	bool VB_Bind(Widget root, string s)
	{
		Root = root.FindAnyWidget("VB_AR_BookSide" + s);
		Title = TextWidget.Cast(root.FindAnyWidget("VB_AR_BookTitle" + s));
		Orn = ImageWidget.Cast(root.FindAnyWidget("VB_AR_BookOrn" + s));
		Pic = ItemPreviewWidget.Cast(root.FindAnyWidget("VB_AR_BookPic" + s));
		Body = RichTextWidget.Cast(root.FindAnyWidget("VB_AR_BookBody" + s));
		NoteBg = ImageWidget.Cast(root.FindAnyWidget("VB_AR_BookNoteBg" + s));
		Note = RichTextWidget.Cast(root.FindAnyWidget("VB_AR_BookNote" + s));
		Num = TextWidget.Cast(root.FindAnyWidget("VB_AR_BookNum" + s));
		Runes = ImageWidget.Cast(root.FindAnyWidget("VB_AR_BookRunes" + s));
		Lock = RichTextWidget.Cast(root.FindAnyWidget("VB_AR_BookLock" + s));
		Ent1 = root.FindAnyWidget("VB_AR_BookEnt1" + s);
		Ent2 = root.FindAnyWidget("VB_AR_BookEnt2" + s);
		EntPic1 = ItemPreviewWidget.Cast(root.FindAnyWidget("VB_AR_BookEntPic1" + s));
		EntPic2 = ItemPreviewWidget.Cast(root.FindAnyWidget("VB_AR_BookEntPic2" + s));
		EntName1 = TextWidget.Cast(root.FindAnyWidget("VB_AR_BookEntName1" + s));
		EntName2 = TextWidget.Cast(root.FindAnyWidget("VB_AR_BookEntName2" + s));
		EntText1 = RichTextWidget.Cast(root.FindAnyWidget("VB_AR_BookEntText1" + s));
		EntText2 = RichTextWidget.Cast(root.FindAnyWidget("VB_AR_BookEntText2" + s));
		if (!Root || !Title || !Orn || !Pic || !Body || !NoteBg || !Note || !Num || !Runes || !Lock)
			return false;
		return Ent1 && Ent2 && EntPic1 && EntPic2 && EntName1 && EntName2 && EntText1 && EntText2;
	}

	void VB_HideAll()
	{
		Title.Show(false);
		Orn.Show(false);
		Pic.Show(false);
		Body.Show(false);
		NoteBg.Show(false);
		Note.Show(false);
		Runes.Show(false);
		Lock.Show(false);
		Ent1.Show(false);
		Ent2.Show(false);
	}
}

class VB_AR_BookMenu : UIScriptedMenu
{
	static const string LAYOUT = "VB_AncientRoot/gui/layouts/vb_ar_book.layout";
	static const string TEX_COVER = "VB_AncientRoot/gui/book/vb_ar_book_cover_ca.paa";
	static const string TEX_PAGE = "VB_AncientRoot/gui/book/vb_ar_book_page_ca.paa";
	static const string TEX_SPINE = "VB_AncientRoot/gui/book/vb_ar_book_spine_ca.paa";
	static const string TEX_TAB = "VB_AncientRoot/gui/book/vb_ar_book_tab_ca.paa";
	static const string TEX_ARROW = "VB_AncientRoot/gui/book/vb_ar_book_arrow_ca.paa";
	static const string TEX_RUNES = "VB_AncientRoot/gui/book/vb_ar_book_runes_ca.paa";
	static const string TEX_ORN = "VB_AncientRoot/gui/book/vb_ar_book_orn_ca.paa";
	static const string TEX_WHITE = "VB_AncientRoot/gui/book/vb_ar_book_white_ca.paa";

	static const int TABS = 10;					// столько закладок в разметке
	static const float TAB_X = 1226;			// закладки торчат из-под обложки справа
	static const float TAB_X_ACTIVE = 1240;		// закладка открытой главы выдвинута
	static const float TAB_Y0 = 66;
	static const float TAB_STEP = 70;
	static const float SPIN_SPEED = 22;			// градусов в секунду — вращение моделей на страницах
	static const float BODY_TOP = 124;			// текст без картинки
	static const float BODY_TOP_PIC = 288;		// текст под картинкой
	static const float BODY_TOP_NOTITLE = 60;
	static const float BODY_BOTTOM = 582;		// над заметкой на полях (RichText сам ужимает шрифт, если текст не влез)
	static const float BODY_BOTTOM_FREE = 680;	// без заметки
	static const float YOU_BODY_TOP = 364;
	static const float BRANCH_W = 400;
	static const float BUD = 22;

	static const int INK = 0xFF3A2614;			// чернила
	static const int INK_TITLE = 0xFF4A2A10;
	static const int INK_NOTE = 0xFF6E3416;
	static const int INK_FAINT = 0xFF7A6650;
	static const int TAB_TEXT = 0xFFF4E9CF;
	static const int NOTE_BG = 0x146E3416;		// едва заметная подложка заметки
	static const int DIM = 0x99000000;

	protected ref array<ref VB_AR_BookPage> m_VB_Pages;
	protected ref array<ref VB_AR_BookChapter> m_VB_Chapters;
	protected ref VB_AR_BookSide m_VB_L;
	protected ref VB_AR_BookSide m_VB_R;
	protected ref map<string, EntityAI> m_VB_Items;			// локальные копии предметов для картинок
	protected ref array<ItemPreviewWidget> m_VB_Spin;		// видимые картинки (вращаются)
	protected ref map<int, bool> m_VB_Overflow;				// о переполнении страницы пишем в лог один раз
	protected ref array<ButtonWidget> m_VB_Tabs;
	protected ref array<ImageWidget> m_VB_TabImgs;
	protected ref array<TextWidget> m_VB_TabTexts;

	protected Widget m_VB_Book;
	protected ImageWidget m_VB_Dim;
	protected ImageWidget m_VB_Cover;
	protected ImageWidget m_VB_PageL;
	protected ImageWidget m_VB_PageR;
	protected ImageWidget m_VB_Spine;
	protected ButtonWidget m_VB_Prev;
	protected ButtonWidget m_VB_Next;
	protected ImageWidget m_VB_PrevImg;
	protected ImageWidget m_VB_NextImg;

	// «Ты и лес»
	protected Widget m_VB_You;
	protected ImageWidget m_VB_YouDisc;
	protected ImageWidget m_VB_YouRune;
	protected ImageWidget m_VB_YouBranch;
	protected ImageWidget m_VB_YouThorn;
	protected ImageWidget m_VB_YouBloom;
	protected ImageWidget m_VB_YouBud;
	protected TextWidget m_VB_YouTier;
	protected TextWidget m_VB_YouValue;

	protected static int s_VB_LastSpread;		// книга открывается там, где ее закрыли
	protected int m_VB_Spread;
	protected float m_VB_Yaw;
	protected int m_VB_SeenChange;
	protected bool m_VB_Ready;
	protected bool m_VB_Shown;

	void VB_AR_BookMenu()
	{
		m_VB_Pages = new array<ref VB_AR_BookPage>;
		m_VB_Chapters = new array<ref VB_AR_BookChapter>;
		m_VB_L = new VB_AR_BookSide();
		m_VB_R = new VB_AR_BookSide();
		m_VB_Items = new map<string, EntityAI>;
		m_VB_Spin = new array<ItemPreviewWidget>;
		m_VB_Overflow = new map<int, bool>;
		m_VB_Tabs = new array<ButtonWidget>;
		m_VB_TabImgs = new array<ImageWidget>;
		m_VB_TabTexts = new array<TextWidget>;
	}

	void ~VB_AR_BookMenu()
	{
		VB_DeleteItems();
		VB_RestoreScreen();
	}

	override Widget Init()
	{
		layoutRoot = g_Game.GetWorkspace().CreateWidgets(LAYOUT);
		if (!layoutRoot)
			return null;
		m_VB_Book = layoutRoot.FindAnyWidget("VB_AR_Book");
		m_VB_Dim = ImageWidget.Cast(layoutRoot.FindAnyWidget("VB_AR_BookDim"));
		m_VB_Cover = ImageWidget.Cast(layoutRoot.FindAnyWidget("VB_AR_BookCover"));
		m_VB_PageL = ImageWidget.Cast(layoutRoot.FindAnyWidget("VB_AR_BookPageL"));
		m_VB_PageR = ImageWidget.Cast(layoutRoot.FindAnyWidget("VB_AR_BookPageR"));
		m_VB_Spine = ImageWidget.Cast(layoutRoot.FindAnyWidget("VB_AR_BookSpine"));
		m_VB_Prev = ButtonWidget.Cast(layoutRoot.FindAnyWidget("VB_AR_BookPrev"));
		m_VB_Next = ButtonWidget.Cast(layoutRoot.FindAnyWidget("VB_AR_BookNext"));
		m_VB_PrevImg = ImageWidget.Cast(layoutRoot.FindAnyWidget("VB_AR_BookPrevImg"));
		m_VB_NextImg = ImageWidget.Cast(layoutRoot.FindAnyWidget("VB_AR_BookNextImg"));
		m_VB_You = layoutRoot.FindAnyWidget("VB_AR_BookYou");
		m_VB_YouDisc = ImageWidget.Cast(layoutRoot.FindAnyWidget("VB_AR_BookYouDisc"));
		m_VB_YouRune = ImageWidget.Cast(layoutRoot.FindAnyWidget("VB_AR_BookYouRune"));
		m_VB_YouBranch = ImageWidget.Cast(layoutRoot.FindAnyWidget("VB_AR_BookYouBranch"));
		m_VB_YouThorn = ImageWidget.Cast(layoutRoot.FindAnyWidget("VB_AR_BookYouThorn"));
		m_VB_YouBloom = ImageWidget.Cast(layoutRoot.FindAnyWidget("VB_AR_BookYouBloom"));
		m_VB_YouBud = ImageWidget.Cast(layoutRoot.FindAnyWidget("VB_AR_BookYouBud"));
		m_VB_YouTier = TextWidget.Cast(layoutRoot.FindAnyWidget("VB_AR_BookYouTier"));
		m_VB_YouValue = TextWidget.Cast(layoutRoot.FindAnyWidget("VB_AR_BookYouValue"));
		for (int i = 0; i < TABS; i++)
		{
			m_VB_Tabs.Insert(ButtonWidget.Cast(layoutRoot.FindAnyWidget("VB_AR_BookTab" + i.ToString())));
			m_VB_TabImgs.Insert(ImageWidget.Cast(layoutRoot.FindAnyWidget("VB_AR_BookTabImg" + i.ToString())));
			m_VB_TabTexts.Insert(TextWidget.Cast(layoutRoot.FindAnyWidget("VB_AR_BookTabText" + i.ToString())));
		}

		m_VB_Ready = VB_CheckWidgets();
		if (!m_VB_Ready)
		{
			Print("[VB_AncientRoot] книга: в разметке нет нужных виджетов, окно не открыто");
			return layoutRoot;
		}

		m_VB_Dim.LoadImageFile(0, TEX_WHITE);
		m_VB_Dim.SetColor(DIM);
		m_VB_Cover.LoadImageFile(0, TEX_COVER);
		m_VB_PageL.LoadImageFile(0, TEX_PAGE);
		m_VB_PageR.LoadImageFile(0, TEX_PAGE);
		m_VB_Spine.LoadImageFile(0, TEX_SPINE);
		m_VB_PrevImg.LoadImageFile(0, TEX_ARROW);
		m_VB_NextImg.LoadImageFile(0, TEX_ARROW);
		m_VB_PrevImg.SetColor(0xFF6E5A2E);
		m_VB_NextImg.SetColor(0xFF6E5A2E);
		VB_SetupSide(m_VB_L);
		VB_SetupSide(m_VB_R);

		m_VB_YouDisc.LoadImageFile(0, VB_AR_FavorHud.TEX_DISC);
		m_VB_YouDisc.SetColor(0xD8000000);
		m_VB_YouRune.LoadImageFile(0, VB_AR_FavorHud.TEX_RUNE);
		m_VB_YouBranch.LoadImageFile(0, VB_AR_FavorHud.TEX_BRANCH);
		m_VB_YouThorn.LoadImageFile(0, VB_AR_FavorHud.TEX_THORN);
		m_VB_YouBloom.LoadImageFile(0, VB_AR_FavorHud.TEX_BLOOM);
		m_VB_YouBud.LoadImageFile(0, VB_AR_FavorHud.TEX_BUD);
		m_VB_YouThorn.LoadMaskTexture(VB_AR_FavorHud.MASK_LEFT);
		m_VB_YouBloom.LoadMaskTexture(VB_AR_FavorHud.MASK_RIGHT);
		m_VB_YouThorn.SetMaskTransitionWidth(0.04);
		m_VB_YouBloom.SetMaskTransitionWidth(0.04);
		m_VB_YouValue.SetColor(INK_NOTE);

		VB_AR_BookText.VB_Build(m_VB_Pages, m_VB_Chapters);
		for (int t = 0; t < TABS; t++)
		{
			bool has = t < m_VB_Chapters.Count();
			m_VB_Tabs[t].Show(has);
			if (!has)
				continue;
			m_VB_TabImgs[t].LoadImageFile(0, TEX_TAB);
			m_VB_TabImgs[t].SetColor(m_VB_Chapters[t].Color);
			m_VB_TabTexts[t].SetText(m_VB_Chapters[t].Tab);
			m_VB_TabTexts[t].SetColor(TAB_TEXT);
			m_VB_TabTexts[t].SetShadow(1, 0xFF000000, 0.6, 1, 1);
		}

		m_VB_Spread = Math.Clamp(s_VB_LastSpread, 0, VB_SpreadCount() - 1);
		VB_ShowSpread(m_VB_Spread);
		return layoutRoot;
	}

	protected bool VB_CheckWidgets()
	{
		if (!m_VB_Book || !m_VB_Dim || !m_VB_Cover || !m_VB_PageL || !m_VB_PageR || !m_VB_Spine)
			return false;
		if (!m_VB_Prev || !m_VB_Next || !m_VB_PrevImg || !m_VB_NextImg)
			return false;
		if (!m_VB_You || !m_VB_YouDisc || !m_VB_YouRune || !m_VB_YouBranch || !m_VB_YouThorn || !m_VB_YouBloom)
			return false;
		if (!m_VB_YouBud || !m_VB_YouTier || !m_VB_YouValue)
			return false;
		for (int i = 0; i < TABS; i++)
		{
			if (!m_VB_Tabs[i] || !m_VB_TabImgs[i] || !m_VB_TabTexts[i])
				return false;
		}
		return m_VB_L.VB_Bind(layoutRoot, "L") && m_VB_R.VB_Bind(layoutRoot, "R");
	}

	protected void VB_SetupSide(VB_AR_BookSide s)
	{
		s.Orn.LoadImageFile(0, TEX_ORN);
		s.Orn.SetColor(0xC05A3A1A);
		s.Runes.LoadImageFile(0, TEX_RUNES);
		s.Runes.SetColor(0xB04A2A10);
		s.NoteBg.LoadImageFile(0, TEX_WHITE);
		s.NoteBg.SetColor(NOTE_BG);
		s.Title.SetColor(INK_TITLE);
		s.Body.SetColor(INK);
		s.Note.SetColor(INK_NOTE);
		s.Num.SetColor(INK_FAINT);
		s.Lock.SetColor(INK_NOTE);
		s.EntName1.SetColor(INK_TITLE);
		s.EntName2.SetColor(INK_TITLE);
		s.EntText1.SetColor(INK);
		s.EntText2.SetColor(INK);
	}

	// ------------------------------------------------------------------ показ и скрытие
	override void OnShow()
	{
		super.OnShow();
		if (m_VB_Shown)
			return;
		m_VB_Shown = true;
		Mission mission = g_Game.GetMission();
		if (mission)
		{
			mission.AddActiveInputExcludes({"menu"});
			if (mission.GetHud())
			{
				mission.GetHud().ShowHudUI(false);
				mission.GetHud().ShowQuickbarUI(false);
			}
		}
		PPERequesterBank.GetRequester(PPERequester_InventoryBlur).Start();
	}

	override void OnHide()
	{
		super.OnHide();
		s_VB_LastSpread = m_VB_Spread;
		VB_DeleteItems();
		VB_RestoreScreen();
	}

	protected void VB_RestoreScreen()
	{
		if (!m_VB_Shown || !g_Game)
			return;
		m_VB_Shown = false;
		Mission mission = g_Game.GetMission();
		if (mission)
		{
			mission.RemoveActiveInputExcludes({"menu"}, true);
			if (mission.GetHud())
			{
				mission.GetHud().ShowHudUI(true);
				mission.GetHud().ShowQuickbarUI(true);
			}
		}
		PPERequesterBank.GetRequester(PPERequester_InventoryBlur).Stop();
	}

	override void Update(float timeslice)
	{
		super.Update(timeslice);
		if (!m_VB_Ready)
		{
			Close();
			return;
		}
		if (GetUApi().GetInputByID(UAUIBack).LocalPress())
		{
			GetUApi().SupressNextFrame(true);
			Close();
			return;
		}
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		if (!player || !player.IsAlive() || player.IsUnconscious() || !VB_AR_Book.Cast(player.GetItemInHands()))
		{
			Close();
			return;
		}

		// модели на страницах медленно вращаются
		m_VB_Yaw += SPIN_SPEED * timeslice;
		if (m_VB_Yaw > 360)
			m_VB_Yaw -= 360;
		foreach (ItemPreviewWidget w : m_VB_Spin)
		{
			if (w)
				w.SetModelOrientation(Vector(10, m_VB_Yaw, 0));
		}

		// расположение изменилось при открытой книге — перерисовать разворот (страница «Ты и лес», замки)
		if (VB_AR_Favor.s_VB_ChangedAt != m_VB_SeenChange)
			VB_ShowSpread(m_VB_Spread);
	}

	// ------------------------------------------------------------------ ввод
	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (!m_VB_Ready)
			return false;
		if (w == m_VB_Prev)
		{
			VB_Turn(-1);
			return true;
		}
		if (w == m_VB_Next)
		{
			VB_Turn(1);
			return true;
		}
		for (int i = 0; i < m_VB_Chapters.Count() && i < TABS; i++)
		{
			if (w == m_VB_Tabs[i])
			{
				VB_ShowSpread(m_VB_Chapters[i].Page / 2);
				return true;
			}
		}
		return false;
	}

	override bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		if (m_VB_Ready && w == m_VB_Dim && button == MouseState.LEFT)
		{
			Close();
			return true;
		}
		return false;
	}

	override bool OnMouseWheel(Widget w, int x, int y, int wheel)
	{
		if (!m_VB_Ready || w == m_VB_Dim)
			return false;
		if (wheel < 0)
			VB_Turn(1);
		else if (wheel > 0)
			VB_Turn(-1);
		return true;
	}

	protected void VB_Turn(int dir)
	{
		int next = Math.Clamp(m_VB_Spread + dir, 0, VB_SpreadCount() - 1);
		if (next == m_VB_Spread)
			return;
		VB_ShowSpread(next);
		VB_PageSound();
	}

	// шорох листа (ванильный звук бумаги)
	protected void VB_PageSound()
	{
		if (!g_Game.GetPlayer())
			return;
		EffectSound snd = SEffectManager.PlaySound("pickUpPaper_SoundSet", g_Game.GetPlayer().GetPosition());
		if (snd)
			snd.SetAutodestroy(true);
	}

	protected int VB_SpreadCount()
	{
		return Math.Max(1, (m_VB_Pages.Count() + 1) / 2);
	}

	// ------------------------------------------------------------------ разворот
	protected void VB_ShowSpread(int spread)
	{
		m_VB_Spread = spread;
		m_VB_SeenChange = VB_AR_Favor.s_VB_ChangedAt;
		m_VB_Spin.Clear();
		int tier = VB_AR_Favor.VB_MyTier();

		m_VB_You.Show(false);
		VB_FillSide(m_VB_L, spread * 2, tier);
		VB_FillSide(m_VB_R, spread * 2 + 1, tier);

		m_VB_Prev.Show(spread > 0);
		m_VB_Next.Show(spread < VB_SpreadCount() - 1);

		// закладка открытой главы выдвинута и ярче
		int active = 0;
		for (int c = 0; c < m_VB_Chapters.Count(); c++)
		{
			if (m_VB_Chapters[c].Page <= spread * 2 + 1)
				active = c;
		}
		for (int t = 0; t < m_VB_Chapters.Count() && t < TABS; t++)
		{
			float tx = TAB_X;
			float alpha = 0.82;
			if (t == active)
			{
				tx = TAB_X_ACTIVE;
				alpha = 1;
			}
			m_VB_Tabs[t].SetPos(tx, TAB_Y0 + t * TAB_STEP);
			m_VB_TabImgs[t].SetAlpha(alpha);
		}
	}

	protected void VB_FillSide(VB_AR_BookSide s, int index, int tier)
	{
		s.VB_HideAll();
		if (index < 0 || index >= m_VB_Pages.Count())
		{
			s.Num.SetText("");
			return;
		}
		VB_AR_BookPage p = m_VB_Pages[index];
		int number = index + 1;
		s.Num.SetText(number.ToString());

		bool hasTitle = p.Title != "";
		if (hasTitle)
		{
			s.Title.SetText(p.Title);
			s.Title.Show(true);
			s.Orn.Show(true);
		}

		// страница еще закрыта: вязь рун и что нужно, чтобы прочесть
		if (tier < p.Need)
		{
			s.Runes.Show(true);
			s.Lock.SetText(VB_LockText(p.Need, tier));
			s.Lock.Show(true);
			return;
		}

		if (p.Kind == VB_AR_BookPage.KIND_YOU)
		{
			VB_FillYou(s, tier);
			return;
		}

		if (p.Kind == VB_AR_BookPage.KIND_ITEMS)
		{
			VB_FillEntry(s.Ent1, s.EntPic1, s.EntName1, s.EntText1, p.Pic1, p.Name1, p.Text1);
			VB_FillEntry(s.Ent2, s.EntPic2, s.EntName2, s.EntText2, p.Pic2, p.Name2, p.Text2);
			return;
		}

		float top = BODY_TOP;
		if (!hasTitle)
			top = BODY_TOP_NOTITLE;
		if (p.Pic != "" && VB_SetPicture(s.Pic, p.Pic))
			top = BODY_TOP_PIC;
		float bottom = BODY_BOTTOM_FREE;
		if (p.Note != "")
		{
			bottom = BODY_BOTTOM;
			s.Note.SetText(p.Note);
			s.Note.Show(true);
			s.NoteBg.Show(true);
		}
		VB_PlaceBody(s.Body, top, bottom, p.Body, index);
	}

	protected void VB_PlaceBody(RichTextWidget body, float top, float bottom, string text, int index)
	{
		float x, y, w, h;
		body.GetPos(x, y);
		body.GetSize(w, h);
		body.SetPos(x, top);
		body.SetSize(w, bottom - top);
		body.SetText(text);
		body.Show(true);
		body.Update();
		VB_CheckFit(body, index);
	}

	// разработчику: текст не влез в страницу — строка в лог клиента (один раз за открытие книги). На стенде 2026-11-16
	// не сработала ни разу: RichText сам ужимает шрифт, когда текст не влезает, — длину страниц проверять снимком.
	protected void VB_CheckFit(RichTextWidget body, int index)
	{
		float sw, sh;
		body.GetScreenSize(sw, sh);
		float need = body.GetContentHeight();
		if (sh > 0 && need > sh + 2 && !m_VB_Overflow.Contains(index))
		{
			m_VB_Overflow.Set(index, true);
			int number = index + 1;
			Print("[VB_AncientRoot] книга: стр. " + number.ToString() + " — текст не влезает (" + need.ToString() + " из " + sh.ToString() + ")");
		}
	}

	protected void VB_FillEntry(Widget frame, ItemPreviewWidget pic, TextWidget name, RichTextWidget text, string type, string title, string body)
	{
		if (title == "")
			return;
		frame.Show(true);
		name.SetText(title);
		text.SetText(body);
		VB_SetPicture(pic, type);
	}

	protected void VB_FillYou(VB_AR_BookSide s, int tier)
	{
		m_VB_You.Show(true);
		int color = VB_AR_FavorHud.VB_TierColor(tier);
		m_VB_YouRune.SetColor(color);
		m_VB_YouBud.SetColor(color);
		m_VB_YouTier.SetText(VB_AR_Favor.VB_TierName(tier));
		m_VB_YouTier.SetColor(VB_InkOfTier(tier));

		float v = Math.Clamp(VB_AR_Favor.s_VB_Value, -100, 100);
		m_VB_YouBloom.SetMaskProgress(Math.Max(0, v) / 100);
		m_VB_YouThorn.SetMaskProgress(Math.Max(0, -v) / 100);
		float pos01 = (v + 100) / 200;
		m_VB_YouBud.SetPos(pos01 * BRANCH_W - BUD / 2, 13);
		int shown = Math.Round(v);
		string val = shown.ToString();
		if (shown > 0)
			val = "+" + val;
		if (VB_AR_Favor.s_VB_Known)
			m_VB_YouValue.SetText("Расположение леса: " + val);
		else
			m_VB_YouValue.SetText("Лес еще не знает тебя");

		// что открыто и что дальше
		int open = 0;
		foreach (VB_AR_BookPage p : m_VB_Pages)
		{
			if (tier >= p.Need)
				open++;
		}
		int total = m_VB_Pages.Count();
		string txt = VB_AR_BookText.VB_TierSpeech(tier) + "\n";
		txt += "Открыто страниц: " + open.ToString() + " из " + total.ToString() + ".";
		int nextTier = VB_AR_Favor.TIER_FRIEND;
		if (tier < VB_AR_Favor.TIER_OWN)
			nextTier = VB_AR_Favor.TIER_OWN;
		if (tier < VB_AR_Favor.TIER_FRIEND)
		{
			string titles = "";
			foreach (VB_AR_BookPage q : m_VB_Pages)
			{
				if (q.Need > tier && q.Need <= nextTier && q.Title != "")
				{
					if (titles != "")
						titles += ", ";
					titles += "«" + q.Title + "»";
				}
			}
			int needValue = VB_AR_BookText.VB_NeedValue(nextTier);
			string needWord = VB_AR_BookText.VB_NeedWord(nextTier);
			txt += " Станешь " + needWord + " (от +" + needValue.ToString() + ")";
			if (titles != "")
				txt += " — проступит " + titles;
			txt += ".";
		}
		else
		{
			txt += " Тебе открыта вся книга.";
		}
		if (tier <= VB_AR_Favor.TIER_ENEMY)
			txt += "\nПуть назад описан в главе «Гнев леса».";
		VB_PlaceBody(s.Body, YOU_BODY_TOP, BODY_BOTTOM_FREE, txt, 1);
	}

	protected string VB_LockText(int need, int tier)
	{
		int v = Math.Round(VB_AR_Favor.s_VB_Value);
		string val = v.ToString();
		if (v > 0)
			val = "+" + val;
		int needValue = VB_AR_BookText.VB_NeedValue(need);
		string needWord = VB_AR_BookText.VB_NeedWord(need);
		string tierName = VB_AR_Favor.VB_TierName(tier);
		string t = "Эти строки написаны языком корней.\nОни проступят, когда лес назовет тебя " + needWord;
		t += " — расположение +" + needValue.ToString() + " и выше.\n";
		t += "Сейчас ты для леса — " + tierName + " (" + val + ").";
		return t;
	}

	// темные «чернильные» оттенки цвета ступени — светлые цвета худа плохо читаются на пергаменте
	protected int VB_InkOfTier(int tier)
	{
		switch (tier)
		{
			case VB_AR_Favor.TIER_FRIEND: return 0xFFA06E0E;
			case VB_AR_Favor.TIER_OWN: return 0xFF3E7024;
			case VB_AR_Favor.TIER_ENEMY: return 0xFFA4501A;
			case VB_AR_Favor.TIER_CURSED: return 0xFF8E1C14;
		}
		return 0xFF5E5040;
	}

	// ------------------------------------------------------------------ картинки: локальные копии предметов
	protected bool VB_SetPicture(ItemPreviewWidget w, string type)
	{
		EntityAI item = VB_GetItem(type);
		if (!item)
			return false;
		w.SetItem(item);
		w.SetView(item.GetViewIndex());
		w.SetModelOrientation(Vector(10, m_VB_Yaw, 0));
		w.Show(true);
		m_VB_Spin.Insert(w);
		return true;
	}

	protected EntityAI VB_GetItem(string type)
	{
		EntityAI item;
		if (m_VB_Items.Find(type, item) && item)
			return item;
		// далеко и высоко: копию не видно и не слышно (у сердцевины есть стук)
		vector pos = Vector(50, 2500, 50);
		int flags = ECE_LOCAL | ECE_NOLIFETIME | ECE_KEEPHEIGHT | ECE_NOSURFACEALIGN;
		item = EntityAI.Cast(g_Game.CreateObjectEx(type, pos, flags));
		if (!item)
		{
			Print("[VB_AncientRoot] книга: не создать картинку " + type);
			return null;
		}
		m_VB_Items.Set(type, item);
		return item;
	}

	protected void VB_DeleteItems()
	{
		if (!m_VB_Items)
			return;
		if (m_VB_Spin)
			m_VB_Spin.Clear();
		if (m_VB_L && m_VB_L.Pic)
			VB_ClearPreviews(m_VB_L);
		if (m_VB_R && m_VB_R.Pic)
			VB_ClearPreviews(m_VB_R);
		foreach (string type, EntityAI item : m_VB_Items)
		{
			if (item && g_Game)
				g_Game.ObjectDelete(item);
		}
		m_VB_Items.Clear();
	}

	protected void VB_ClearPreviews(VB_AR_BookSide s)
	{
		s.Pic.SetItem(null);
		s.EntPic1.SetItem(null);
		s.EntPic2.SetItem(null);
	}
}

modded class MissionGameplay
{
	override UIScriptedMenu CreateScriptedMenu(int id)
	{
		UIScriptedMenu menu = super.CreateScriptedMenu(id);
		if (!menu && id == VB_AR_Book.MENU_ID)
		{
			menu = new VB_AR_BookMenu();
			menu.SetID(id);
		}
		return menu;
	}
}
