// Склад клана в окне банкомата INT_Banking.
//
// У игрока в клане в шапке окна переключатель «Личный счёт | Клан [ТЕГ]». В режиме клана:
//   слева клановая карта (цвет, эмблема, тег) и сводка склада вместо таймеров банка;
//   справа свои пять страниц на те же F1..F5: обзор склада, внести наличные, снять наличные,
//   перевод между личным счётом и складом, история склада.
// Без клана окно банкомата как было. Операции выполняет сервер планшета (VB_TabletBank): право ранга
// «Работать со складом», банкомат банка рядом, суммы. Ответ приходит RESULT планшета, баланс склада
// и история следом (CLAN, CLAN_BANK). После перевода личный счёт обновляется запросом STATE к банку.
// В личной истории счёта переводы с кланом подписаны «Перевод в клан» / «Перевод из клана».

const int VB_BANK_DEP = 1;
const int VB_BANK_WD = 2;
const int VB_BANK_IN = 3;	// со счёта на склад
const int VB_BANK_OUT = 4;	// со склада на счёт
const float VB_BANK_TIMEOUT = 10.0;

modded class INT_Bank_Menu
{
	protected bool m_VbOffered;		// игрок в клане: переключатель и страницы клана построены
	protected bool m_VbClan;		// включён счёт клана
	protected INT_Bank_UIButton m_VbSegMe;
	protected INT_Bank_UIButton m_VbSegClan;
	protected Widget m_VbCard;
	protected TextWidget m_VbCardBal;
	protected TextWidget m_VbCardAccess;
	protected Widget m_VbBox;
	protected TextWidget m_VbBoxTax;
	protected TextWidget m_VbBoxDays;
	protected TextWidget m_VbBoxAccess;
	protected ref array<Widget> m_VbPages;
	protected ref array<Widget> m_VbLocks;

	// обзор
	protected TextWidget m_VbOvBal;
	protected TextWidget m_VbOvCur;
	protected TextWidget m_VbOvLimit;
	protected INT_UIF_ProgressBar m_VbOvBar;
	protected TextWidget m_VbOvCash;
	protected TextWidget m_VbOvRoom;
	protected TextWidget m_VbOvDays;
	protected INT_UIF_List m_VbOvRecent;

	// внести
	protected ref INT_Bank_UIAmount m_VbDepAmount;
	protected ref INT_Bank_UIKeypad m_VbDepKeys;
	protected TextWidget m_VbDepCash;
	protected TextWidget m_VbDepNow;
	protected TextWidget m_VbDepAfter;
	protected INT_Bank_UIButton m_VbDepGo;
	protected INT_Bank_UIButton m_VbDepAll;

	// снять
	protected ref INT_Bank_UIAmount m_VbWdAmount;
	protected ref INT_Bank_UIKeypad m_VbWdKeys;
	protected TextWidget m_VbWdNow;
	protected TextWidget m_VbWdAfter;
	protected RichTextWidget m_VbWdNotes;
	protected INT_Bank_UIButton m_VbWdGo;

	// перевод
	protected ref INT_Bank_UIAmount m_VbTrAmount;
	protected ref INT_Bank_UIKeypad m_VbTrKeys;
	protected INT_Bank_UIButton m_VbTrIn;
	protected INT_Bank_UIButton m_VbTrOut;
	protected INT_Bank_UIButton m_VbTrGo;
	protected TextWidget m_VbTrAcc;
	protected TextWidget m_VbTrStore;
	protected TextWidget m_VbTrAccAfter;
	protected TextWidget m_VbTrStoreAfter;
	protected bool m_VbTrToMe;		// false: со счёта на склад, true: со склада на счёт

	// история
	protected INT_UIF_List m_VbHistList;
	protected TextWidget m_VbHistCount;
	protected string m_VbHistSig = "-";

	// операция
	protected bool m_VbBusy;
	protected float m_VbBusyLeft;
	protected bool m_VbSegOff;			// кнопка «Клан» выключена: клан пропал, пока окно открыто
	protected int m_VbOpKind;
	protected int m_VbOpAmount;
	protected int m_VbResultSeen;
	protected int m_VbDataSeen = -1;
	protected int m_VbStateSeq;
	protected float m_VbQuiet;		// пауза после отказа сервера: зажатый Enter не шлёт запрос повторно
	protected bool m_VbF6Was;
	protected bool m_VbF9Was;

	// =================================================================================================
	// Построение
	// =================================================================================================
	override void OnBuildHeader(Widget center, Widget right)
	{
		super.OnBuildHeader(center, right);
		VB_Clan c = VB_TabletClient.Get().Clan;
		if (!center || !c)
			return;
		m_VbOffered = true;
		float slot = GetHeaderSlotW();
		float sw = 196;
		float pw = sw * 2 + 16;
		Widget pad = INT_UIF_Draw.Frame(center, (slot - pw) * 0.5, 14, pw, 48, "VbSwitch");
		// Цвета в локальных: два вызова ARGB в одних аргументах затирают друг друга (Hair тоже зовёт ARGB).
		int padFill = ARGB(150, 10, 8, 6);
		int padRim = INT_Bank_UILook.Hair();
		INT_UIF_Surface.Box(pad, 0, 0, pw, 48, 12, padFill, padRim);
		m_VbSegMe = INT_Bank_UIButton.CreateFx(m_Bag, pad, 4, 4, sw, 40, "#STR_VB_BANK_ME", INT_UIF_Variant.PRIMARY, "user");
		m_VbSegMe.SetSound(INT_Bank_Sfx.TAB);
		m_VbSegMe.OnClick().Insert(VbOnSegMe);
		string label = VBT.T("#STR_VB_BANK_CLAN") + " [" + c.Tag + "]";
		m_VbSegClan = INT_Bank_UIButton.CreateFx(m_Bag, pad, sw + 12, 4, sw, 40, label, INT_UIF_Variant.GHOST, "shield");
		m_VbSegClan.SetSound(INT_Bank_Sfx.TAB);
		m_VbSegClan.OnClick().Insert(VbOnSegClan);
		VbPaintSwitch();
	}

	override void OnBuild(Widget body)
	{
		super.OnBuild(body);
		if (!m_VbOffered)
			return;
		m_VbPages = new array<Widget>();
		m_VbLocks = new array<Widget>();
		float bh = GetBodyH();
		VbBuildCard(m_Left, LEFT_W, 214);
		VbBuildBox(m_Left, 0, 538, LEFT_W, bh - 538);
		float rw = GetBodyW() - LEFT_W - 20;
		float pw = rw - PAGE_PAD * 2;
		float ph = bh - PAGE_PAD * 2;
		VbBuildOverview(VbNewPage(pw, ph), pw, ph);
		VbBuildDeposit(VbNewPage(pw, ph), pw, ph);
		VbBuildWithdraw(VbNewPage(pw, ph), pw, ph);
		VbBuildTransfer(VbNewPage(pw, ph), pw, ph);
		VbBuildHistory(VbNewPage(pw, ph), pw, ph);
		VbHidePages();
	}

	protected Widget VbNewPage(float w, float h)
	{
		Widget p = INT_UIF_Draw.Frame(m_PageHost, 0, 0, w, h, "VbBankPage");
		p.Show(false);
		m_VbPages.Insert(p);
		return p;
	}

	// Клановая карта на месте личной: цвет клана, эмблема, тег, доступ и баланс склада.
	protected void VbBuildCard(Widget parent, float w, float h)
	{
		VB_Clan c = VB_TabletClient.Get().Clan;
		int col = c.Color;
		int white = ARGB(255, 255, 255, 255);
		int light = INT_UIF_Color.Lerp(col, white, 0.55);
		m_VbCard = INT_UIF_Draw.Frame(parent, 0, 0, w, h, "VbClanCard");
		INT_UIF_Surface.Shadow(m_VbCard, -22, -10, w + 44, h + 40, ARGB(190, 0, 0, 0));
		INT_UIF_Surface.Glow(m_VbCard, -16, -14, w + 32, h + 28, INT_UIF_Color.WithAlpha(col, 46));
		int deep = INT_UIF_Color.Shade(col, -0.76);
		int cardFill = INT_UIF_Color.WithAlpha(deep, 255);
		int cardRim = INT_UIF_Color.WithAlpha(col, 170);
		INT_UIF_Surface.Box(m_VbCard, 0, 0, w, h, 14, cardFill, cardRim);
		Widget clip = INT_UIF_Draw.Clip(m_VbCard, 2, 2, w - 4, h - 4, "VbCardClip");
		float cw = w - 4;
		float ch = h - 4;
		INT_UIF_Draw.Image(clip, 0, 0, cw, ch, "iuf_radial", INT_UIF_Color.WithAlpha(col, 110));
		INT_UIF_Draw.Image(clip, 0, 0, cw, ch * 0.6, "iuf_grad_v", ARGB(50, 255, 255, 255));
		INT_UIF_Draw.Texture(clip, 0, 0, cw, ch, INT_BANK_TEX_AURORA, INT_UIF_Color.WithAlpha(col, 90));
		INT_UIF_Draw.Texture(clip, 0, 0, cw, ch, INT_BANK_TEX_STRIPES, ARGB(22, 255, 255, 255));
		INT_UIF_Draw.Line(clip, 0, ch - 46, cw, 46, ARGB(80, 0, 0, 0));

		TextWidget brand = INT_UIF_Draw.Text(clip, 22, 16, 220, 22, "DAYZAVR", 17, ARGB(255, 255, 244, 230));
		if (brand)
			brand.SetBold(true);
		INT_UIF_Draw.Text(clip, 22, 37, 220, 14, INT_Bank_UILook.Spaced("#STR_VB_BANK_CLAN"), 9, ARGB(220, 255, 240, 220));
		TextWidget tag = INT_UIF_Draw.Text(clip, cw - 152, 16, 130, 20, "[" + c.Tag + "]", 14, light, INT_UIF_Align.RIGHT);
		if (tag)
			tag.SetBold(true);

		Widget tile = INT_UIF_Draw.Frame(clip, 20, 60, 64, 64, "VbEmblemTile");
		int tileFill = ARGB(150, 0, 0, 0);
		int tileRim = INT_UIF_Color.WithAlpha(col, 140);
		INT_UIF_Surface.Box(tile, 0, 0, 64, 64, 12, tileFill, tileRim);
		VBK.Emblem(tile, 8, 8, 48, c.EmbShape, c.EmbSymbol, col);
		TextWidget title = INT_UIF_Draw.Text(clip, 98, 66, cw - 120, 26, VBT.Fit(c.Title, cw - 120, 18), 18, ARGB(255, 255, 244, 230));
		if (title)
			title.SetBold(true);
		int members = c.Members.Count();
		string sub = INT_Bank_UILook.Fmt("#STR_VB_BANK_CARD_SUB", c.Level.ToString(), members.ToString());
		INT_UIF_Draw.Text(clip, 98, 94, cw - 120, 18, sub, 12, ARGB(220, 236, 226, 210));

		INT_UIF_Draw.Text(clip, 24, ch - 40, 160, 12, "#STR_VB_BANK_CARD_ACCESS", 8, ARGB(200, 236, 222, 200));
		m_VbCardAccess = INT_UIF_Draw.Text(clip, 24, ch - 27, cw * 0.45, 18, "", 13, ARGB(255, 255, 244, 230));
		if (m_VbCardAccess)
			m_VbCardAccess.SetBold(true);
		INT_UIF_Draw.Text(clip, cw * 0.5, ch - 40, cw * 0.5 - 24, 12, "#STR_VB_BANK_CARD_STORE", 8, ARGB(200, 236, 222, 200), INT_UIF_Align.RIGHT);
		m_VbCardBal = INT_UIF_Draw.Text(clip, cw * 0.4, ch - 27, cw * 0.6 - 24, 18, "", 13, ARGB(255, 255, 244, 230), INT_UIF_Align.RIGHT);
		if (m_VbCardBal)
			m_VbCardBal.SetBold(true);
		m_VbCard.Show(false);
	}

	// Сводка склада на месте таймеров банка: у склада своих пауз нет.
	protected void VbBuildBox(Widget parent, float x, float y, float w, float h)
	{
		m_VbBox = INT_UIF_Draw.Frame(parent, x, y, w, h, "VbStoreBox");
		int boxFill = INT_Bank_UILook.Glass();
		int boxRim = INT_Bank_UILook.Hair();
		INT_UIF_Surface.Box(m_VbBox, 0, 0, w, h, 12, boxFill, boxRim);
		INT_UIF_Draw.Caption(m_VbBox, 18, 14, w - 36, "#STR_VB_BANK_BOX", "shield");
		m_VbBoxTax = CdRow(m_VbBox, 18, 46, w - 36, "#STR_VB_BANK_TAX_DAY");
		m_VbBoxDays = CdRow(m_VbBox, 18, 72, w - 36, "#STR_VB_BANK_TAX_LEFT");
		m_VbBoxAccess = CdRow(m_VbBox, 18, 98, w - 36, "#STR_VB_BANK_ACCESS");
		m_VbBox.Show(false);
	}

	protected void VbBuildOverview(Widget p, float w, float h)
	{
		INT_UIF_Draw.Caption(p, 0, 0, w, "#STR_VB_BANK_OV_TITLE", "shield");
		m_VbOvBal = INT_UIF_Draw.Text(p, 0, 26, w - 160, 70, "0", 60, INT_Bank_UILook.Ink(), INT_UIF_Align.LEFT, true);
		m_VbOvCur = INT_UIF_Draw.Text(p, 200, 56, 140, 30, "#STR_INT_BANK_CUR", 22, INT_Bank_UILook.Brass());
		m_VbOvLimit = INT_UIF_Draw.Text(p, 0, 104, w, 18, "", 12, INT_Bank_UILook.Faint());
		m_VbOvBar = INT_UIF_ProgressBar.Create(m_Bag, p, 0, 126, w, 6, 0, INT_UIF_ThemeKeys.TEAL);

		float tw = (w - 32) / 3;
		m_VbOvCash = Tile(p, 0, 152, tw, "#STR_INT_BANK_OV_CASH", "wallet");
		m_VbOvRoom = Tile(p, tw + 16, 152, tw, "#STR_VB_BANK_OV_ROOM", "target");
		m_VbOvDays = Tile(p, (tw + 16) * 2, 152, tw, "#STR_VB_BANK_OV_TAX", "clock");

		float bw = (w - 36) / 4;
		INT_Bank_UIButton b1 = INT_Bank_UIButton.CreateFx(m_Bag, p, 0, 262, bw, 50, "#STR_INT_BANK_NAV_DEPOSIT", INT_UIF_Variant.PRIMARY, "plus");
		b1.SetInkLabel(true);
		b1.OnClick().Insert(OnQuickDeposit);
		INT_Bank_UIButton b2 = INT_Bank_UIButton.CreateFx(m_Bag, p, bw + 12, 262, bw, 50, "#STR_INT_BANK_NAV_WITHDRAW", INT_UIF_Variant.SECONDARY, "wallet");
		b2.OnClick().Insert(OnQuickWithdraw);
		INT_Bank_UIButton b3 = INT_Bank_UIButton.CreateFx(m_Bag, p, (bw + 12) * 2, 262, bw, 50, "#STR_INT_BANK_NAV_TRANSFER", INT_UIF_Variant.SECONDARY, "swap");
		b3.OnClick().Insert(OnQuickTransfer);
		INT_Bank_UIButton b4 = INT_Bank_UIButton.CreateFx(m_Bag, p, (bw + 12) * 3, 262, bw, 50, "#STR_INT_BANK_DEP_ALL", INT_UIF_Variant.SECONDARY, "check");
		b4.OnClick().Insert(VbOnDepositAll);

		INT_UIF_Draw.Caption(p, 0, 340, w, "#STR_VB_BANK_OV_RECENT", "clock");
		m_VbOvRecent = INT_UIF_List.Create(m_Bag, p, 0, 368, w, h - 368, "#STR_VB_BANK_HIST_EMPTY");
		m_VbOvRecent.SetSelectable(false);
	}

	protected void VbBuildDeposit(Widget p, float w, float h)
	{
		float lw = KeypadX(w) - 28;
		INT_UIF_Draw.Caption(p, 0, 0, lw, "#STR_VB_BANK_DEP_TITLE", "plus");
		m_VbDepAmount = new INT_Bank_UIAmount();
		m_VbDepAmount.Build(p, 0, 30, lw, 140, "#STR_INT_BANK_AMOUNT_DEPOSIT");
		QuickRow(p, 0, 186, lw, "vbdep");
		m_VbDepCash = InfoRow(p, 0, 282, lw, "#STR_INT_BANK_INFO_CASH");
		m_VbDepNow = InfoRow(p, 0, 318, lw, "#STR_VB_BANK_INFO_STORE");
		m_VbDepAfter = InfoRow(p, 0, 354, lw, "#STR_VB_BANK_INFO_STORE_AFTER");
		INT_UIF_Draw.MText(p, 0, 396, lw, 54, "#STR_VB_BANK_DEP_NOTE", 12, INT_Bank_UILook.Faint());
		float by = h - 54;
		float half = (lw - 12) / 2;
		m_VbDepAll = INT_Bank_UIButton.CreateFx(m_Bag, p, 0, by, half, 54, "#STR_INT_BANK_DEP_ALL", INT_UIF_Variant.SECONDARY, "check");
		m_VbDepAll.OnClick().Insert(VbOnDepositAll);
		m_VbDepGo = INT_Bank_UIButton.CreateFx(m_Bag, p, half + 12, by, half, 54, "#STR_INT_BANK_DEP_GO", INT_UIF_Variant.PRIMARY, "plus");
		m_VbDepGo.SetInkLabel(true);
		m_VbDepGo.OnClick().Insert(VbOnGo);
		m_VbDepKeys = BuildKeypad(p, w, 30);
		VbBuildLock(p, w, h);
	}

	protected void VbBuildWithdraw(Widget p, float w, float h)
	{
		float lw = KeypadX(w) - 28;
		INT_UIF_Draw.Caption(p, 0, 0, lw, "#STR_VB_BANK_WD_TITLE", "wallet");
		m_VbWdAmount = new INT_Bank_UIAmount();
		m_VbWdAmount.Build(p, 0, 30, lw, 140, "#STR_INT_BANK_AMOUNT_WITHDRAW");
		QuickRow(p, 0, 186, lw, "vbwd");
		m_VbWdNow = InfoRow(p, 0, 282, lw, "#STR_VB_BANK_INFO_STORE");
		m_VbWdAfter = InfoRow(p, 0, 318, lw, "#STR_VB_BANK_INFO_STORE_LEFT");
		INT_UIF_Draw.Text(p, 0, 358, lw, 18, "#STR_INT_BANK_WD_NOTES", 11, INT_Bank_UILook.Faint());
		m_VbWdNotes = INT_UIF_Draw.Rich(p, 0, 378, lw, 44, "", 13, INT_Bank_UILook.Sand(), INT_UIF_Align.LEFT);
		float by = h - 54;
		m_VbWdGo = INT_Bank_UIButton.CreateFx(m_Bag, p, 0, by, lw, 54, "#STR_INT_BANK_WD_GO", INT_UIF_Variant.PRIMARY, "wallet");
		m_VbWdGo.SetInkLabel(true);
		m_VbWdGo.OnClick().Insert(VbOnGo);
		m_VbWdKeys = BuildKeypad(p, w, 30);
		VbBuildLock(p, w, h);
	}

	protected void VbBuildTransfer(Widget p, float w, float h)
	{
		float lw = KeypadX(w) - 28;
		INT_UIF_Draw.Caption(p, 0, 0, lw, "#STR_VB_BANK_TR_TITLE", "swap");
		float half = (lw - 12) / 2;
		m_VbTrIn = INT_Bank_UIButton.CreateFx(m_Bag, p, 0, 28, half, 44, "#STR_VB_BANK_TR_IN", INT_UIF_Variant.PRIMARY, "chevron_down");
		m_VbTrIn.SetSound(INT_Bank_Sfx.SELECT);
		m_VbTrIn.OnClick().Insert(VbOnTrIn);
		m_VbTrOut = INT_Bank_UIButton.CreateFx(m_Bag, p, half + 12, 28, half, 44, "#STR_VB_BANK_TR_OUT", INT_UIF_Variant.GHOST, "chevron_up");
		m_VbTrOut.SetSound(INT_Bank_Sfx.SELECT);
		m_VbTrOut.OnClick().Insert(VbOnTrOut);
		m_VbTrAmount = new INT_Bank_UIAmount();
		m_VbTrAmount.Build(p, 0, 86, lw, 132, "#STR_INT_BANK_AMOUNT_TRANSFER");
		QuickRow(p, 0, 230, lw, "vbtr");
		m_VbTrAcc = InfoRow(p, 0, 324, lw, "#STR_VB_BANK_INFO_ACC");
		m_VbTrStore = InfoRow(p, 0, 360, lw, "#STR_VB_BANK_INFO_STORE");
		m_VbTrAccAfter = InfoRow(p, 0, 396, lw, "#STR_VB_BANK_INFO_ACC_AFTER");
		m_VbTrStoreAfter = InfoRow(p, 0, 432, lw, "#STR_VB_BANK_INFO_STORE_AFTER");
		float by = h - 54;
		m_VbTrGo = INT_Bank_UIButton.CreateFx(m_Bag, p, 0, by, lw, 54, "#STR_VB_BANK_TR_GO", INT_UIF_Variant.PRIMARY, "swap");
		m_VbTrGo.SetInkLabel(true);
		m_VbTrGo.OnClick().Insert(VbOnGo);
		m_VbTrKeys = BuildKeypad(p, w, 30);
		VbBuildLock(p, w, h);
		VbPaintDir();
	}

	protected void VbBuildHistory(Widget p, float w, float h)
	{
		INT_UIF_Draw.Caption(p, 0, 0, w * 0.6, "#STR_VB_BANK_HIST_TITLE", "list");
		m_VbHistCount = INT_UIF_Draw.Text(p, w * 0.4, 0, w * 0.6, 20, "", 12, INT_Bank_UILook.Faint(), INT_UIF_Align.RIGHT);
		m_VbHistList = INT_UIF_List.Create(m_Bag, p, 0, 30, w, h - 30, "#STR_VB_BANK_HIST_EMPTY");
		m_VbHistList.SetSelectable(false);
	}

	// Без права «Работать со складом» страница закрыта табличкой: смотреть можно обзор и историю.
	protected void VbBuildLock(Widget p, float w, float h)
	{
		Widget veil = INT_UIF_Draw.Frame(p, 0, 0, w, h, "VbLock");
		int veilFill = ARGB(232, 14, 11, 8);
		int veilRim = INT_Bank_UILook.Hair();
		INT_UIF_Surface.Box(veil, 0, 0, w, h, 14, veilFill, veilRim);
		INT_UIF_Draw.Hit(veil, 0, 0, w, h, null);
		INT_UIF_Draw.IconTile(veil, w * 0.5 - 28, h * 0.5 - 92, 56, "lock", INT_Bank_UILook.Brass(), 14);
		TextWidget head = INT_UIF_Draw.Text(veil, 0, h * 0.5 - 20, w, 30, "#STR_VB_BANK_LOCK_TITLE", 20, INT_Bank_UILook.Ink(), INT_UIF_Align.CENTER);
		if (head)
			head.SetBold(true);
		INT_UIF_Draw.MText(veil, w * 0.2, h * 0.5 + 18, w * 0.6, 60, "#STR_VB_BANK_LOCK", 13, INT_Bank_UILook.Muted());
		veil.Show(false);
		m_VbLocks.Insert(veil);
	}

	// =================================================================================================
	// Переключатель счёта
	// =================================================================================================
	void VbOnSegMe(INT_UIF_Button b)
	{
		VbSetMode(false);
	}

	void VbOnSegClan(INT_UIF_Button b)
	{
		VbSetMode(true);
	}

	protected void VbPaintSwitch()
	{
		VbSeg(m_VbSegMe, !m_VbClan);
		VbSeg(m_VbSegClan, m_VbClan);
	}

	protected void VbSeg(INT_Bank_UIButton b, bool on)
	{
		if (!b)
			return;
		if (on)
			b.SetVariant(INT_UIF_Variant.PRIMARY);
		else
			b.SetVariant(INT_UIF_Variant.GHOST);
		b.SetInkLabel(on);
		// Подпись кнопки это RichText: новый цвет виден только после повторной записи текста.
		b.SetText(b.GetText());
	}

	protected void VbSetMode(bool clan)
	{
		if (clan == m_VbClan || !m_VbOffered || !m_VbPages)
			return;
		if (IsBusy() || HasOverlay())
			return;
		if (clan && !VB_TabletClient.Get().Clan)
			return;
		m_VbClan = clan;
		VbPaintSwitch();
		// Reveal поднимает карту от текущего места: сначала карта ставится на своё, иначе
		// быстрые переключения подряд уводят её вниз.
		Widget own = null;
		if (m_Card)
			own = m_Card.GetRoot();
		if (own)
		{
			INT_UIF_Draw.Place(own, 0, 0, LEFT_W, 214);
			own.Show(!clan);
		}
		if (m_VbCard)
		{
			INT_UIF_Draw.Place(m_VbCard, 0, 0, LEFT_W, 214);
			m_VbCard.Show(clan);
		}
		if (clan && m_VbCard)
			INT_UIF_Animator.Reveal(m_VbCard, 0.02, 18, 0.45);
		if (!clan && m_Card)
			m_Card.Reveal(0.02);
		Widget cd = null;
		if (m_CdDep)
			cd = m_CdDep.GetParent();
		if (cd)
			cd.Show(!clan);
		if (m_VbBox)
			m_VbBox.Show(clan);
		INT_Bank_UISfx.Play(INT_Bank_Sfx.CARD);
		if (clan)
		{
			VB_TabletClient.Get().Request(VB_TabletOp.CLAN_BANK);
			VbFillHistory(true);
		}
		int page = m_Page;
		if (page < 0)
			page = PAGE_OVERVIEW;
		m_Page = -1;
		ShowPage(page, true);
	}

	protected void VbHidePages()
	{
		if (m_VbPages)
		{
			foreach (Widget p : m_VbPages)
			{
				if (p)
					p.Show(false);
			}
		}
		if (m_VbDepAmount)
			m_VbDepAmount.SetActive(false);
		if (m_VbWdAmount)
			m_VbWdAmount.SetActive(false);
		if (m_VbTrAmount)
			m_VbTrAmount.SetActive(false);
	}

	// =================================================================================================
	// Страницы и ввод: в режиме клана свои, иначе банка
	// =================================================================================================
	override protected void ShowPage(int index, bool animate)
	{
		if (!m_VbClan || !m_VbPages)
		{
			// Пока F5 зажата, банк каждый кадр открывает «Историю» заново (на этой странице он сам чистит
			// память клавиш) и шлёт второй запрос. Сервер отвечает на него «слишком часто», ответ на первый
			// уже не принимается, и список остаётся пустым. Пока ответ не пришёл, повтор не нужен.
			if (index == m_Page && index == PAGE_HISTORY && m_HistSeq != 0)
				return;
			VbHidePages();
			super.ShowPage(index, animate);
			return;
		}
		if (index < 0 || index >= m_VbPages.Count())
			return;
		int prev = m_Page;
		m_Page = index;
		foreach (Widget own : m_Pages)
		{
			if (own)
				own.Show(false);
		}
		int n = m_VbPages.Count();
		for (int i = 0; i < n; i++)
		{
			Widget p = m_VbPages.Get(i);
			if (p)
				p.Show(i == index);
		}
		Widget cur = m_VbPages.Get(index);
		if (animate && cur && prev != index)
		{
			INT_UIF_Draw.Move(cur, 0, 0);
			INT_UIF_Animator.Reveal(cur, 0, 12, 0.3);
		}
		if (m_DepAmount)
			m_DepAmount.SetActive(false);
		if (m_WdAmount)
			m_WdAmount.SetActive(false);
		if (m_TrAmount)
			m_TrAmount.SetActive(false);
		if (m_VbDepAmount)
			m_VbDepAmount.SetActive(index == PAGE_DEPOSIT);
		if (m_VbWdAmount)
			m_VbWdAmount.SetActive(index == PAGE_WITHDRAW);
		if (m_VbTrAmount)
			m_VbTrAmount.SetActive(index == PAGE_TRANSFER);
		if (index == PAGE_WITHDRAW)
			VbNotes();
		if (index == PAGE_HISTORY && prev != index)
			VB_TabletClient.Get().Request(VB_TabletOp.CLAN_BANK);
		SetFocus(null);
		RefreshAll();
	}

	protected bool VbCanUse()
	{
		return VB_TabletClient.Get().Can(VB_ClanPerm.BANK);
	}

	override protected INT_Bank_UIAmount ActiveAmount()
	{
		if (!m_VbClan)
			return super.ActiveAmount();
		if (!VbCanUse())
			return null;
		if (m_Page == PAGE_DEPOSIT)
			return m_VbDepAmount;
		if (m_Page == PAGE_WITHDRAW)
			return m_VbWdAmount;
		if (m_Page == PAGE_TRANSFER)
			return m_VbTrAmount;
		return null;
	}

	override protected INT_Bank_UIKeypad ActiveKeys()
	{
		if (!m_VbClan)
			return super.ActiveKeys();
		if (!VbCanUse())
			return null;
		if (m_Page == PAGE_DEPOSIT)
			return m_VbDepKeys;
		if (m_Page == PAGE_WITHDRAW)
			return m_VbWdKeys;
		if (m_Page == PAGE_TRANSFER)
			return m_VbTrKeys;
		return null;
	}

	protected int VbStore()
	{
		VB_Clan c = VB_TabletClient.Get().Clan;
		if (!c)
			return 0;
		return c.Bank;
	}

	// Свой минимум целых: Min из Math считает во float, а суммы счёта бывают больше 16 млн и теряют точность.
	protected int VbMin(int a, int b)
	{
		if (a < b)
			return a;
		return b;
	}

	// Сколько ещё поместится на склад.
	protected int VbRoom()
	{
		int room = VB_TabletClient.Get().BankMax - VbStore();
		if (room < 0)
			room = 0;
		return room;
	}

	// Сколько ещё поместится на личный счёт.
	protected int VbAccRoom()
	{
		INT_Bank_State st = St();
		if (st.MaxBalance <= 0)
			return 2000000000;
		int room = st.MaxBalance - st.Balance;
		if (room < 0)
			room = 0;
		return room;
	}

	override protected int PageMax()
	{
		if (!m_VbClan)
			return super.PageMax();
		int v = 0;
		if (m_Page == PAGE_DEPOSIT)
			v = VbMin(Cash(), VbRoom());
		else if (m_Page == PAGE_WITHDRAW)
			v = VbStore();
		else if (m_Page == PAGE_TRANSFER)
		{
			if (m_VbTrToMe)
				v = VbMin(VbStore(), VbAccRoom());
			else
				v = VbMin(St().Balance, VbRoom());
		}
		if (v < 0)
			v = 0;
		return v;
	}

	override protected void AmountChanged()
	{
		if (!m_VbClan)
		{
			super.AmountChanged();
			return;
		}
		if (m_Page == PAGE_WITHDRAW)
			VbNotes();
		RefreshAll();
	}

	override protected void SubmitPage()
	{
		if (!m_VbClan)
		{
			super.SubmitPage();
			return;
		}
		INT_Bank_UISfx.Play(INT_Bank_Sfx.CLICK);
		VbSubmit();
	}

	override protected bool IsBusy()
	{
		return super.IsBusy() || m_VbBusy;
	}

	// Купюры, которыми выдаст снятие: тот же список, что у банка.
	protected void VbNotes()
	{
		if (!m_VbWdNotes || !m_VbWdAmount)
			return;
		int amount = m_VbWdAmount.Get();
		if (amount <= 0)
		{
			m_VbWdNotes.SetText("-");
			return;
		}
		array<string> classes = new array<string>();
		array<int> pieces = new array<int>();
		bool ok = INT_Bank_Client.Get().PreviewNotes(amount, classes, pieces);
		if (!ok || classes.Count() == 0)
		{
			string bad = INT_Bank_UILook.Tr("#STR_INT_BANK_ERR_NOT_PAYABLE");
			m_VbWdNotes.SetText(bad);
			return;
		}
		string txt = "";
		int n = classes.Count();
		for (int i = 0; i < n; i++)
		{
			string nm = INT_Bank_UIState.NoteName(classes.Get(i));
			int pc = pieces.Get(i);
			if (txt != "")
				txt = txt + "   ";
			txt = txt + nm + " x" + pc.ToString();
		}
		m_VbWdNotes.SetText(txt);
	}

	// =================================================================================================
	// Операции
	// =================================================================================================
	void VbOnGo(INT_UIF_Button b)
	{
		VbSubmit();
	}

	void VbOnTrIn(INT_UIF_Button b)
	{
		if (!m_VbTrToMe)
			return;
		m_VbTrToMe = false;
		VbPaintDir();
		if (m_VbTrAmount)
			m_VbTrAmount.Clear();
		RefreshAll();
	}

	void VbOnTrOut(INT_UIF_Button b)
	{
		if (m_VbTrToMe)
			return;
		m_VbTrToMe = true;
		VbPaintDir();
		if (m_VbTrAmount)
			m_VbTrAmount.Clear();
		RefreshAll();
	}

	protected void VbPaintDir()
	{
		VbSeg(m_VbTrIn, !m_VbTrToMe);
		VbSeg(m_VbTrOut, m_VbTrToMe);
	}

	// «Внести всё»: всё, что при себе, сколько поместится на склад.
	void VbOnDepositAll(INT_UIF_Button b)
	{
		if (!m_Ready || IsBusy() || m_VbQuiet > 0)
			return;
		if (!VbCanUse())
		{
			VbError(VB_TabletResult.DENIED);
			return;
		}
		int cash = Cash();
		if (cash <= 0)
		{
			ShowError(INT_Bank_Status.NO_CASH, 0);
			return;
		}
		int amount = VbMin(cash, VbRoom());
		if (amount <= 0)
		{
			VbError(VB_TabletResult.BANK_FULL);
			return;
		}
		VbStart(VB_TabletOp.ATM_DEPOSIT, VB_BANK_DEP, amount);
	}

	protected void VbSubmit()
	{
		if (!m_Ready || IsBusy() || m_VbQuiet > 0 || !VB_TabletClient.Get().Clan)
			return;
		if (!VbCanUse())
		{
			VbError(VB_TabletResult.DENIED);
			return;
		}
		INT_Bank_UIAmount a = ActiveAmount();
		if (!a)
			return;
		int amount = a.Get();
		if (amount <= 0)
		{
			a.Shake();
			return;
		}
		int store = VbStore();
		if (m_Page == PAGE_DEPOSIT)
		{
			int cash = Cash();
			if (amount > cash)
			{
				a.Shake();
				ShowError(INT_Bank_Status.NO_CASH, cash);
				return;
			}
			if (amount > VbRoom())
			{
				a.Shake();
				VbError(VB_TabletResult.BANK_FULL);
				return;
			}
			VbStart(VB_TabletOp.ATM_DEPOSIT, VB_BANK_DEP, amount);
			return;
		}
		if (m_Page == PAGE_WITHDRAW)
		{
			if (amount > store)
			{
				a.Shake();
				VbError(VB_TabletResult.NO_FUNDS);
				return;
			}
			array<string> classes = new array<string>();
			array<int> pieces = new array<int>();
			if (!INT_Bank_Client.Get().PreviewNotes(amount, classes, pieces))
			{
				a.Shake();
				ShowError(INT_Bank_Status.NOT_PAYABLE, 0);
				return;
			}
			VbStart(VB_TabletOp.ATM_WITHDRAW, VB_BANK_WD, amount);
			return;
		}
		if (m_Page != PAGE_TRANSFER)
			return;
		if (m_VbTrToMe)
		{
			if (amount > store)
			{
				a.Shake();
				VbError(VB_TabletResult.NO_FUNDS);
				return;
			}
			if (amount > VbAccRoom())
			{
				a.Shake();
				VbError(VB_TabletResult.ACC_FULL);
				return;
			}
			VbStart(VB_TabletOp.ATM_TO_ACCOUNT, VB_BANK_OUT, amount);
			return;
		}
		if (amount > St().Balance)
		{
			a.Shake();
			VbError(VB_TabletResult.ACC_NO_FUNDS);
			return;
		}
		if (amount > VbRoom())
		{
			a.Shake();
			VbError(VB_TabletResult.BANK_FULL);
			return;
		}
		VbStart(VB_TabletOp.ATM_FROM_ACCOUNT, VB_BANK_IN, amount);
	}

	protected void VbStart(int op, int kind, int amount)
	{
		VB_TabletClient client = VB_TabletClient.Get();
		m_VbResultSeen = client.ResultVersion;
		m_VbOpKind = kind;
		m_VbOpAmount = amount;
		m_VbBusy = true;
		m_VbBusyLeft = VB_BANK_TIMEOUT;
		ScriptRPC rpc = client.Begin(op);
		rpc.Write(amount);
		client.Post(rpc);
		ShowBusy(true);
		RefreshAll();
	}

	protected void VbError(int code)
	{
		INT_Bank_UISfx.Play(INT_Bank_Sfx.ERROR);
		string text = VBT.T(VBK.ResultText(code));
		int kind = INT_UIF_ToastKind.ERROR;
		if (code == VB_TabletResult.RATE)
			kind = INT_UIF_ToastKind.WARNING;
		INT_UIF_Toast.Show(text, kind, 4.5);
	}

	protected void VbOnResult(int code)
	{
		if (code == VB_TabletResult.NO_ROOM)
		{
			INT_ShowNoRoom();
			return;
		}
		if (code != VB_TabletResult.OK)
		{
			// Пока шёл запрос, банк чистил память клавиш: Enter, который ещё зажат, сработал бы снова.
			m_VbQuiet = 0.6;
			VbError(code);
			return;
		}
		VB_Clan c = VB_TabletClient.Get().Clan;
		string tag = "";
		if (c)
			tag = "[" + c.Tag + "]";
		string text = INT_Bank_UILook.Fmt("#STR_VB_BANK_RES_TEXT", tag);
		string title = "";
		int bankOp = INT_Bank_Op.TRANSFER;
		if (m_VbOpKind == VB_BANK_DEP)
		{
			INT_Bank_UISfx.Play(INT_Bank_Sfx.CASH_IN);
			title = "#STR_VB_BANK_RES_DEP";
			bankOp = INT_Bank_Op.DEPOSIT;
			if (m_VbDepAmount)
				m_VbDepAmount.Clear();
		}
		else if (m_VbOpKind == VB_BANK_WD)
		{
			INT_Bank_UISfx.Play(INT_Bank_Sfx.CASH_OUT);
			title = "#STR_VB_BANK_RES_WD";
			bankOp = INT_Bank_Op.WITHDRAW;
			if (m_VbWdAmount)
				m_VbWdAmount.Clear();
			VbNotes();
		}
		else
		{
			title = "#STR_VB_BANK_RES_IN";
			if (m_VbOpKind == VB_BANK_OUT)
				title = "#STR_VB_BANK_RES_OUT";
			if (m_VbTrAmount)
				m_VbTrAmount.Clear();
			// Личный счёт изменил сервер планшета: состояние банка запрашивается заново.
			m_VbStateSeq = INT_Bank_Client.Get().Send(INT_Bank_Op.STATE);
		}
		INT_Bank_UISfx.Play(INT_Bank_Sfx.CONFIRM);
		ShowResult(bankOp, INT_Bank_UILook.Tr(title), m_VbOpAmount, text, true);
	}

	// =================================================================================================
	// Кадр: ответ сервера планшета, обновление данных клана
	// =================================================================================================
	override void Update(float timeslice)
	{
		super.Update(timeslice);
		if (!m_VbOffered)
			return;
#ifdef DIAG_DEVELOPER
		// Только DayZDiag, для проверки на стенде клавиатурой: F6 счёт, F9 направление перевода.
		// Своя память нажатия: банк чистит свою (m_KeyWas) каждый кадр на страницах без клавиатуры.
		bool f6 = KeyState(KeyCode.KC_F6) != 0;
		bool f9 = KeyState(KeyCode.KC_F9) != 0;
		if (!HasModal() && !HasOverlay())
		{
			if (f6 && !m_VbF6Was)
				VbSetMode(!m_VbClan);
			if (f9 && !m_VbF9Was && m_VbClan)
			{
				if (m_VbTrToMe)
					VbOnTrIn(null);
				else
					VbOnTrOut(null);
			}
		}
		m_VbF6Was = f6;
		m_VbF9Was = f9;
#endif
		VB_TabletClient client = VB_TabletClient.Get();
		if (m_VbQuiet > 0)
			m_VbQuiet = m_VbQuiet - timeslice;
		if (m_VbBusy)
		{
			if (client.ResultVersion != m_VbResultSeen)
			{
				m_VbBusy = false;
				ShowBusy(false);
				VbOnResult(client.LastResult);
				RefreshAll();
			}
			else
			{
				m_VbBusyLeft = m_VbBusyLeft - timeslice;
				if (m_VbBusyLeft <= 0)
				{
					m_VbBusy = false;
					ShowBusy(false);
					ShowError(INT_Bank_Status.TIMEOUT, 0);
					RefreshAll();
				}
			}
		}
		if (m_VbStateSeq != 0 && !INT_Bank_Client.Get().IsPending(m_VbStateSeq))
		{
			m_VbStateSeq = 0;
			CountBalance(St().Balance);
			RequestRecent();
			RefreshAll();
		}
		// Игрока исключили или клан распущен, пока окно открыто: обратно на личный счёт.
		// Во время операции или окна поверх переключиться нельзя: проверка повторяется каждый кадр.
		if (!client.Clan)
		{
			if (m_VbClan && !IsBusy() && !HasOverlay())
				VbSetMode(false);
			if (m_VbSegClan && !m_VbSegOff)
			{
				m_VbSegClan.SetEnabled(false);
				m_VbSegOff = true;
			}
		}
		if (client.DataVersion != m_VbDataSeen)
		{
			m_VbDataSeen = client.DataVersion;
			VbFillHistory(false);
			RefreshAll();
		}
	}

	override void OnReady()
	{
		super.OnReady();
		if (m_VbOffered)
			VB_TabletClient.Get().Request(VB_TabletOp.CLAN_BANK);
	}

	// =================================================================================================
	// Надписи из последних данных
	// =================================================================================================
	override protected void RefreshAll()
	{
		super.RefreshAll();
		VbRefresh();
	}

	protected void VbRefresh()
	{
		if (!m_VbOffered || !m_VbPages)
			return;
		VB_TabletClient client = VB_TabletClient.Get();
		VB_Clan c = client.Clan;
		if (!c)
			return;
		INT_Bank_State st = St();
		int store = c.Bank;
		int most = client.BankMax;
		int room = VbRoom();
		int cash = Cash();
		bool can = VbCanUse();
		bool busy = IsBusy() || !m_Ready;
		string sStore = INT_Bank_UILook.Money(store);
		string sCash = INT_Bank_UILook.Money(cash);
		string access = "#STR_VB_BANK_ACCESS_VIEW";
		if (can)
			access = "#STR_VB_BANK_ACCESS_FULL";

		// карта и сводка
		SetText(m_VbCardBal, sStore);
		SetText(m_VbCardAccess, INT_Bank_UILook.Tr(access));
		SetText(m_VbBoxTax, INT_Bank_UILook.Money(client.MyTax()));
		SetText(m_VbBoxDays, VbDaysText(c, client));
		SetText(m_VbBoxAccess, INT_Bank_UILook.Tr(access));
		if (m_VbBoxAccess)
		{
			if (can)
				m_VbBoxAccess.SetColor(INT_Bank_UILook.Sage());
			else
				m_VbBoxAccess.SetColor(ARGB(255, 226, 158, 72));
		}
		if (m_VbBoxDays)
		{
			if (VbDays(c, client) <= 3)
				m_VbBoxDays.SetColor(INT_Bank_UILook.Brick());
			else
				m_VbBoxDays.SetColor(INT_Bank_UILook.Sage());
		}

		// обзор
		if (m_VbOvBal)
		{
			string big = INT_Bank_Util.Money(store);
			m_VbOvBal.SetText(big);
			float tw = INT_Bank_UIMeasure.TextW(m_VbOvBal, big, 60);
			if (m_VbOvCur)
				INT_UIF_Draw.Place(m_VbOvCur, tw + 14, 56, 140, 30);
		}
		SetText(m_VbOvLimit, INT_Bank_UILook.Fmt("#STR_VB_BANK_OV_LIMIT", INT_Bank_UILook.Money(most)));
		if (m_VbOvBar)
		{
			float frac = 0;
			if (most > 0)
				frac = store / (float)most;
			m_VbOvBar.SetValue(Math.Clamp(frac, 0, 1));
		}
		SetText(m_VbOvCash, INT_Bank_Util.Money(cash));
		SetText(m_VbOvRoom, INT_Bank_Util.Money(room));
		SetText(m_VbOvDays, VbDaysText(c, client));

		// внести
		int dep = 0;
		if (m_VbDepAmount)
			dep = m_VbDepAmount.Get();
		SetText(m_VbDepCash, sCash);
		SetText(m_VbDepNow, sStore);
		SetText(m_VbDepAfter, INT_Bank_UILook.Money(store + dep));
		if (m_VbDepAmount)
		{
			string dh = INT_Bank_UILook.Fmt("#STR_INT_BANK_DEP_HINT", sCash);
			int dhc = INT_Bank_UILook.Muted();
			if (dep > cash)
			{
				dh = INT_Bank_UILook.Fmt("#STR_INT_BANK_ERR_NO_CASH", sCash);
				dhc = INT_Bank_UILook.Brick();
			}
			else if (dep > room)
			{
				dh = INT_Bank_UILook.Fmt("#STR_VB_BANK_HINT_ROOM", INT_Bank_UILook.Money(room));
				dhc = INT_Bank_UILook.Brick();
			}
			m_VbDepAmount.SetHint(dh, dhc);
		}
		VbButton(m_VbDepGo, !busy && can);
		VbButton(m_VbDepAll, !busy && can);

		// снять
		int wd = 0;
		if (m_VbWdAmount)
			wd = m_VbWdAmount.Get();
		SetText(m_VbWdNow, sStore);
		SetText(m_VbWdAfter, INT_Bank_UILook.Money(store - wd));
		if (m_VbWdAfter)
		{
			if (store - wd < 0)
				m_VbWdAfter.SetColor(INT_Bank_UILook.Brick());
			else
				m_VbWdAfter.SetColor(INT_Bank_UILook.Ink());
		}
		if (m_VbWdAmount)
		{
			string wh = INT_Bank_UILook.Fmt("#STR_VB_BANK_HINT_STORE", sStore);
			int whc = INT_Bank_UILook.Muted();
			if (wd > store)
			{
				wh = INT_Bank_UILook.Fmt("#STR_VB_BANK_HINT_NO_STORE", sStore);
				whc = INT_Bank_UILook.Brick();
			}
			m_VbWdAmount.SetHint(wh, whc);
		}
		VbButton(m_VbWdGo, !busy && can);

		// перевод
		int tr = 0;
		if (m_VbTrAmount)
			tr = m_VbTrAmount.Get();
		int acc = st.Balance;
		int accAfter = acc - tr;
		int storeAfter = store + tr;
		if (m_VbTrToMe)
		{
			accAfter = acc + tr;
			storeAfter = store - tr;
		}
		SetText(m_VbTrAcc, INT_Bank_UILook.Money(acc));
		SetText(m_VbTrStore, sStore);
		SetText(m_VbTrAccAfter, INT_Bank_UILook.Money(accAfter));
		SetText(m_VbTrStoreAfter, INT_Bank_UILook.Money(storeAfter));
		VbAfterColor(m_VbTrAccAfter, accAfter);
		VbAfterColor(m_VbTrStoreAfter, storeAfter);
		if (m_VbTrAmount)
		{
			string th = INT_Bank_UILook.Fmt("#STR_INT_BANK_WD_HINT", INT_Bank_UILook.Money(acc));
			if (m_VbTrToMe)
				th = INT_Bank_UILook.Fmt("#STR_VB_BANK_HINT_STORE", sStore);
			int thc = INT_Bank_UILook.Muted();
			if (accAfter < 0 || storeAfter < 0)
				thc = INT_Bank_UILook.Brick();
			else if (!m_VbTrToMe && tr > room)
			{
				th = INT_Bank_UILook.Fmt("#STR_VB_BANK_HINT_ROOM", INT_Bank_UILook.Money(room));
				thc = INT_Bank_UILook.Brick();
			}
			m_VbTrAmount.SetHint(th, thc);
		}
		VbButton(m_VbTrGo, !busy && can);
		VbButton(m_VbTrIn, !busy);
		VbButton(m_VbTrOut, !busy);

		// замки
		foreach (Widget veil : m_VbLocks)
		{
			if (veil)
				veil.Show(!can);
		}
	}

	protected void VbButton(INT_Bank_UIButton b, bool on)
	{
		if (b)
			b.SetEnabled(on);
	}

	protected void VbAfterColor(TextWidget w, int value)
	{
		if (!w)
			return;
		if (value < 0)
			w.SetColor(INT_Bank_UILook.Brick());
		else
			w.SetColor(INT_Bank_UILook.Ink());
	}

	// Дней до роспуска за неуплату: дни без налога плюс склад, делённый на налог.
	protected int VbDays(VB_Clan c, VB_TabletClient client)
	{
		int tax = client.TaxOf(c.Level);
		if (tax <= 0)
			return 9999;
		return c.TaxFree + c.Bank / tax;
	}

	protected string VbDaysText(VB_Clan c, VB_TabletClient client)
	{
		if (client.TaxOf(c.Level) <= 0)
			return "-";
		int days = VbDays(c, client);
		return INT_Bank_UILook.Fmt("#STR_VB_BANK_DAYS", days.ToString());
	}

	// =================================================================================================
	// История склада: последние в обзоре, вся на F5. Список пересобирается, только если он изменился.
	// =================================================================================================
	protected void VbFillHistory(bool force)
	{
		if (!m_VbHistList || !m_VbOvRecent)
			return;
		array<ref VB_ClanBankRow> rows = VB_TabletClient.Get().History;
		int n = rows.Count();
		string sig = n.ToString();
		if (n > 0)
		{
			VB_ClanBankRow top = rows.Get(0);
			sig = sig + "|" + top.Time + "|" + top.Sum.ToString() + "|" + top.Who;
		}
		if (!force && sig == m_VbHistSig)
			return;
		m_VbHistSig = sig;
		VbFillList(m_VbOvRecent, rows, 4);
		VbFillList(m_VbHistList, rows, n);
		if (m_VbHistCount)
			m_VbHistCount.SetText(INT_Bank_UILook.Fmt("#STR_INT_BANK_HIST_COUNT", n.ToString()));
	}

	protected void VbFillList(INT_UIF_List list, array<ref VB_ClanBankRow> rows, int limit)
	{
		list.Clear();
		int n = VbMin(limit, rows.Count());
		for (int i = 0; i < n; i++)
		{
			VB_ClanBankRow row = rows.Get(i);
			string who = row.Who;
			if (who == "")
				who = VBT.T("#STR_VB_TAB_C_SYSTEM");
			string sub = row.Time + "  ·  " + who;
			int idx = list.AddRow(VbKindText(row.Kind), sub, i);
			bool plus = row.Sum > 0;
			string hue = INT_UIF_ThemeKeys.RED;
			string icon = "chevron_up";
			string sign = "- ";
			int col = INT_Bank_UILook.Brick();
			if (plus)
			{
				hue = INT_UIF_ThemeKeys.GREEN;
				icon = "chevron_down";
				sign = "+ ";
				col = INT_Bank_UILook.Sage();
			}
			if (row.Kind == VB_ClanEvent.FROM_ACCOUNT || row.Kind == VB_ClanEvent.TO_ACCOUNT)
				icon = "swap";
			else if (row.Kind == VB_ClanEvent.TAX)
				icon = "clock";
			else if (row.Kind == VB_ClanEvent.CONTRACT || row.Kind == VB_ClanEvent.WEEK_PRIZE)
				icon = "star";
			list.SetRowIcon(idx, icon, hue);
			int sum = Math.AbsInt(row.Sum);
			list.SetRowRight(idx, sign + INT_Bank_UILook.Money(sum), col);
		}
		list.Reveal(0.05);
	}

	protected string VbKindText(int kind)
	{
		if (kind == VB_ClanEvent.DEPOSIT)
			return "#STR_VB_TAB_C_OP_DEPOSIT";
		if (kind == VB_ClanEvent.WITHDRAW)
			return "#STR_VB_TAB_C_OP_WITHDRAW";
		if (kind == VB_ClanEvent.FROM_ACCOUNT)
			return "#STR_VB_TAB_C_OP_FROM_ACC";
		if (kind == VB_ClanEvent.TO_ACCOUNT)
			return "#STR_VB_TAB_C_OP_TO_ACC";
		if (kind == VB_ClanEvent.TAX)
			return "#STR_VB_TAB_C_OP_TAX";
		if (kind == VB_ClanEvent.CONTRACT)
			return "#STR_VB_TAB_C_OP_CONTRACT";
		if (kind == VB_ClanEvent.WEEK_PRIZE)
			return "#STR_VB_TAB_C_OP_PRIZE";
		return "#STR_VB_TAB_C_OP_SETTINGS";
	}

	// =================================================================================================
	// Личная история счёта: переводы с кланом подписаны по-своему (банк пишет их как оплату на бирже)
	// =================================================================================================
	override protected void ApplyHistory(INT_Bank_Reply r, INT_UIF_List list)
	{
		if (!list || r.Status != INT_Bank_Status.OK)
			return;
		list.Clear();
		int limit = r.Rows.Count();
		if (list == m_OvRecent && limit > 4)
			limit = 4;
		int mark = VB_BANK_PEER.Length();
		for (int i = 0; i < limit; i++)
		{
			INT_Bank_HistRow row = r.Rows.Get(i);
			string title = HistTitle(row.Type);
			bool income = INT_Bank_Util.HistIsIncome(row.Type);
			string peer = row.PeerName;
			if (peer == "")
				peer = row.PeerId;
			bool clanRow = row.PeerId.IndexOf(VB_BANK_PEER) == 0;
			if (clanRow)
			{
				string tag = row.PeerId.Substring(mark, row.PeerId.Length() - mark);
				peer = VBT.T("#STR_VB_BANK_CLAN") + " [" + tag + "]";
				title = "#STR_VB_BANK_H_TO_CLAN";
				if (income)
					title = "#STR_VB_BANK_H_FROM_CLAN";
			}
			string sub = row.Time;
			if (peer != "")
				sub = sub + "  ·  " + peer;
			int idx = list.AddRow(title, sub, i);
			string hue = INT_UIF_ThemeKeys.RED;
			string icon = "chevron_up";
			string sign = "- ";
			int col = INT_Bank_UILook.Brick();
			if (income)
			{
				hue = INT_UIF_ThemeKeys.GREEN;
				icon = "chevron_down";
				sign = "+ ";
				col = INT_Bank_UILook.Sage();
			}
			if (clanRow || row.Type == INT_Bank_Hist.TRANSFER_IN || row.Type == INT_Bank_Hist.TRANSFER_OUT)
				icon = "swap";
			if (row.Type == INT_Bank_Hist.BBA_IN || row.Type == INT_Bank_Hist.BBA_OUT)
				icon = "shield";
			if (row.Type == INT_Bank_Hist.QST_IN)
				icon = "star";
			list.SetRowIcon(idx, icon, hue);
			string money = INT_Bank_UILook.Money(row.Amount);
			list.SetRowRight(idx, sign + money, col);
		}
		list.Reveal(0.05);
	}
}
