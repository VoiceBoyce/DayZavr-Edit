// ============================================================================
// DRL_CraftMenu - общие детали нового интерфейса на INT_UIFramework.
//
// Стиль повторяет INT_Trader: тёмная тема фреймворка, плитки, чипы,
// 3D-превью на «сцене». Все размеры — дизайн-пиксели холста 1920x1080.
// ============================================================================

// ID окон мода (те же, что у старых меню, — их открывают действия и F9).
const int DRL_UI_MENU_CRAFT = 9501;
const int DRL_UI_MENU_WORKBENCH = 9502;
const int DRL_UI_MENU_LEARNING = 9503;

// Изучен ли рецепт на клиенте (перенесено из старого DRL_CraftMenuUI.DRL_ClientHasLearnedRecipe):
// при выключенном изучении или рецепте без изучения — «изучен».
class DRL_LearnState
{
	static bool HasLearned(int recipeId)
	{
		DRL_Craft_Config cfg = g_Craft_Config;
		if (!cfg || !cfg.IsLearningEnabled())
			return true;
		DRL_Learning_Config lcfg = GetLearning_Config();
		if (!lcfg || !lcfg.RecipeRequiresLearning(recipeId))
			return true;
		return DRL_LearningBridge.HasLearnedRecipe(recipeId);
	}
}

// Приоритеты отрисовки: 3D-модель рисуется поверх любого 2D-виджета с
// приоритетом не выше своего, поэтому чипы поверх превью берут больший.
// ItemPreviewWidget с приоритетом > 256 не рисуется вовсе (замер INT_Trader).
const int DRL_UI_SORT_PREVIEW = 10;
const int DRL_UI_SORT_OVER_MODEL = 100;

class DRL_UIText
{
	// Перевод ключа и подстановка %1, %2.
	static string Fmt(string key, string a = "", string b = "")
	{
		string s = key;
		if (key.IndexOf("#") == 0)
			s = Widget.TranslateString(key);
		s.Replace("%1", a);
		s.Replace("%2", b);
		return s;
	}

	static string Tr(string key)
	{
		if (key.IndexOf("#") == 0)
			return Widget.TranslateString(key);
		return key;
	}

	// Время крафта коротко: «20 с», «2 мин 5 с».
	static string Time(int secs)
	{
		if (secs < 0)
			secs = 0;
		int mm = secs / 60;
		int ss = secs - mm * 60;
		if (mm > 0 && ss > 0)
			return Fmt("#STR_DRL_UI_TIME_MS", mm.ToString(), ss.ToString());
		if (mm > 0)
			return Fmt("#STR_DRL_UI_TIME_M", mm.ToString());
		return Fmt("#STR_DRL_UI_TIME_S", ss.ToString());
	}

	// «Подпись: значение»; у строк DRL в конце подписи бывает пробел.
	static string Label(string key, string value)
	{
		string k = Tr(key);
		k = k.Trim();
		return k + " " + value;
	}

	static string Pct(float frac)
	{
		frac = Math.Clamp(frac, 0, 1);
		int p = Math.Round(frac * 100.0);
		return p.ToString() + "%";
	}

	// = DRL_CraftMenuUI.DRL_FormatStateFromDamageFrac
	static string StateFromDamage(float dmgFrac)
	{
		if (dmgFrac <= 0.0)
			return Tr("#STR_DRL_UNTOUCHED");
		if (dmgFrac >= 1.0)
			return Tr("#STR_DRL_BROKEN");
		int p = Math.Round(dmgFrac * 100.0);
		string s = Tr("#STR_DRL_DMG_PERCENT");
		s = s.Trim();
		return s + " " + p.ToString() + "%";
	}

	// = DRL_CraftMenuUI.DRL_TextFromMinHealthFrac (уровень = 3 - MinHealthFrac)
	static string MinHealth(int minHF)
	{
		int lvl = Math.Clamp(3 - minHF, 0, 3);
		if (lvl == 0)
			return Tr("#STR_DRL_UNTOUCHED");
		if (lvl == 1)
			return Tr("#STR_DRL_WORN");
		if (lvl == 2)
			return Tr("#STR_DRL_DAMAGED");
		return Tr("#STR_DRL_BADLY_DAMAGED");
	}

	static string DisplayName(string cls)
	{
		string dn = "";
		if (cls != "" && DRL_CraftUIHelper.GetDisplayNameForClass(cls, dn) && dn != "")
			return dn;
		return cls;
	}
}

class DRL_UIAnim
{
	// INT_UIF_Animator.Reveal считает «домом» текущую позицию виджета; при
	// повторном вызове посреди анимации дом уехал бы. Сначала ставим на место.
	static void Reveal(Widget w, float homeX, float homeY, float delay, float dy = 10, float duration = 0.3)
	{
		if (!w)
			return;
		INT_UIF_Animator anim = INT_UIF_Animator.Peek();
		if (anim)
			anim.Cancel(w);
		INT_UIF_Draw.Move(w, homeX, homeY);
		INT_UIF_Animator.Reveal(w, delay, dy, duration);
	}

	// Перенос виджета из пула на новое место без «хвоста» старой анимации.
	static void MoveTo(Widget w, float x, float y)
	{
		if (!w)
			return;
		INT_UIF_Animator anim = INT_UIF_Animator.Peek();
		if (anim)
			anim.Cancel(w);
		w.SetAlpha(1);
		INT_UIF_Draw.Move(w, x, y);
	}
}

// Один ItemPreviewWidget на весь родитель + локальный объект, который он
// показывает. Объект создаётся тем же хелпером, что и в старом меню
// (подбор «играбельного» класса, сборка оружия).
class DRL_UIItemView extends Managed
{
	protected ItemPreviewWidget m_Widget;
	protected EntityAI m_Entity;
	protected string m_Class;
	protected vector m_Orientation;
	protected float m_Zoom = 1;

	void ~DRL_UIItemView()
	{
		Clear();
	}

	bool Create(Widget parent, bool ignorePointer)
	{
		if (!parent || !GetGame() || !GetGame().GetWorkspace())
			return false;
		int flags = WidgetFlags.VISIBLE;
		if (ignorePointer)
			flags = flags | WidgetFlags.IGNOREPOINTER;
		m_Widget = ItemPreviewWidget.Cast(GetGame().GetWorkspace().CreateWidget(ItemPreviewWidgetTypeID, 0, 0, 1, 1, flags, ARGB(255, 255, 255, 255), DRL_UI_SORT_PREVIEW, parent));
		if (!m_Widget)
			return false;
		m_Widget.SetPos(0, 0);
		m_Widget.SetSize(1, 1);
		return true;
	}

	EntityAI GetEntity()
	{
		return m_Entity;
	}

	string GetClass()
	{
		return m_Class;
	}

	// Показать класс; новый объект создаётся только при смене класса.
	bool SetClass(string cls)
	{
		if (cls == m_Class && (m_Entity || cls == ""))
			return m_Entity != null;
		Clear();
		m_Class = cls;
		if (cls == "" || !m_Widget)
			return false;
		EntityAI ent = null;
		DRL_CraftPreviewHelper.CreatePreviewObject(cls, ent);
		m_Entity = ent;
		if (!m_Entity)
		{
			m_Widget.Show(false);
			return false;
		}
		m_Widget.SetItem(m_Entity);
		int view = m_Entity.GetViewIndex();
		if (view < 0)
			view = 0;
		m_Widget.SetView(view);
		m_Widget.SetModelPosition(Vector(0, 0, 0));
		m_Widget.SetModelOrientation(m_Orientation);
		m_Widget.Show(true);
		return true;
	}

	// Собрать объект заново с навесным (= RebuildMainPreviewWithAttachments):
	// тот же класс переиспользуется, навесное снимается и ставится заново.
	bool Build(string cls, array<string> attachments)
	{
		if (!m_Widget)
			return false;
		if (cls != m_Class)
			Clear();
		m_Class = cls;
		if (cls == "")
			return false;
		EntityAI ent = m_Entity;
		DRL_CraftPreviewHelper.CreatePreviewObject(cls, ent);
		m_Entity = ent;
		if (!m_Entity)
		{
			m_Widget.Show(false);
			return false;
		}
		bool isTransport = m_Entity.IsInherited(Transport);
		if (!isTransport && attachments && m_Entity.GetInventory())
		{
			foreach (string att : attachments)
			{
				DRL_CraftPreviewHelper.SpawnAttachment(m_Entity, att);
			}
		}
		Weapon_Base gun = Weapon_Base.Cast(m_Entity);
		if (gun)
			gun.ShowMagazine();
		m_Widget.SetItem(null);
		m_Widget.SetItem(m_Entity);
		int view = m_Entity.GetViewIndex();
		if (view < 0)
			view = 0;
		m_Widget.SetView(view);
		m_Widget.SetModelPosition(Vector(0, 0, 0));
		m_Widget.SetModelOrientation(m_Orientation);
		m_Widget.Show(true);
		return true;
	}

	void Clear()
	{
		if (m_Widget)
			m_Widget.SetItem(null);
		if (m_Entity && GetGame())
			GetGame().ObjectDelete(m_Entity);
		m_Entity = null;
		m_Class = "";
	}

	void Show(bool show)
	{
		if (m_Widget)
			m_Widget.Show(show && m_Entity != null);
	}

	void SetOrientation(vector o)
	{
		m_Orientation = o;
		if (m_Widget && m_Entity)
			m_Widget.SetModelOrientation(o);
	}

	vector GetOrientation()
	{
		return m_Orientation;
	}

	// Зум 1 = вписать в рамку; рамка-родитель обрезает лишнее.
	void SetZoom(float zoom)
	{
		m_Zoom = zoom;
		if (!m_Widget)
			return;
		m_Widget.SetSize(zoom, zoom);
		m_Widget.SetPos((1 - zoom) * 0.5, (1 - zoom) * 0.5);
	}

	float GetZoom()
	{
		return m_Zoom;
	}
}

// Большое превью панели деталей: подсветка, «пол», медленное вращение.
// Тянуть — вращать, колесо — зум, двойной клик — сброс.
class DRL_UIStage extends ScriptedWidgetEventHandler
{
	static const float SPIN_SPEED = 22;
	static const float IDLE_RESUME = 2.5;

	protected Widget m_Root;
	protected Widget m_Clip;
	protected ButtonWidget m_Hit;
	protected ImageWidget m_Glow;
	protected ImageWidget m_Backdrop;
	protected Widget m_Missing;
	protected Widget m_HintRow;
	protected ref DRL_UIItemView m_View;
	protected float m_W;
	protected float m_H;
	protected bool m_Dragging;
	protected int m_DragX;
	protected int m_DragY;
	protected vector m_DragStart;
	protected float m_Idle;
	protected int m_HideMask;

	void Build(Widget parent, float x, float y, float w, float h)
	{
		m_W = w;
		m_H = h;
		m_Root = INT_UIF_Draw.Frame(parent, x, y, w, h, "DRL_Stage");
		int tileFill = INT_UIF.Color(INT_UIF_ThemeKeys.TILE);
		int tileBorder = INT_UIF.Color(INT_UIF_ThemeKeys.TILE_BORDER);
		INT_UIF_Surface.Box(m_Root, 0, 0, w, h, 12, tileFill, tileBorder);
		// Фон категории/верстака из конфига — приглушённой подложкой.
		m_Backdrop = INT_UIF_Draw.Image(m_Root, 1, 1, w - 2, h - 2, "iuf_white", ARGB(46, 255, 255, 255));
		m_Backdrop.Show(false);
		INT_UIF_Draw.Image(m_Root, 0, 0, w, h * 0.6, "iuf_grad_v", ARGB(14, 255, 255, 255));
		m_Glow = INT_UIF_Draw.Image(m_Root, w * 0.5 - 150, h * 0.5 - 130, 300, 260, "iuf_radial", ARGB(40, 245, 165, 36));
		INT_UIF_Draw.Image(m_Root, w * 0.5 - 130, h - 58, 260, 40, "iuf_radial", ARGB(120, 0, 0, 0));

		m_Clip = INT_UIF_Draw.Clip(m_Root, 10, 10, w - 20, h - 40, "DRL_StageClip");
		m_View = new DRL_UIItemView();
		m_View.Create(m_Clip, true);
		m_View.SetOrientation(Vector(-8, 30, 0));

		int faint = INT_UIF.Color(INT_UIF_ThemeKeys.TEXT_FAINT);
		m_Missing = INT_UIF_Draw.Frame(m_Root, 0, 0, w, h, "DRL_StageMissing");
		INT_UIF_Draw.Icon(m_Missing, w * 0.5 - 18, h * 0.5 - 30, 36, "info", faint);
		INT_UIF_Draw.Text(m_Missing, 0, h * 0.5 + 12, w, 20, "#STR_DRL_UI_NO_PREVIEW", 13, faint, INT_UIF_Align.CENTER);
		m_Missing.Show(false);

		m_HintRow = INT_UIF_Draw.Frame(m_Root, 14, h - 26, w - 28, 18, "DRL_StageHint");
		INT_UIF_Draw.Icon(m_HintRow, 0, 2, 14, "refresh", faint);
		INT_UIF_Draw.Text(m_HintRow, 20, 0, w - 50, 18, "#STR_DRL_UI_PV_HINT", 11, faint);
		m_HintRow.Show(false);

		m_Hit = INT_UIF_Draw.Hit(m_Root, 0, 0, w, h, this);
		m_Hit.SetName("DRL_StageHit");
	}

	Widget GetRoot()
	{
		return m_Root;
	}

	EntityAI GetEntity()
	{
		if (!m_View)
			return null;
		return m_View.GetEntity();
	}

	void SetAccent(int color)
	{
		if (m_Glow)
			m_Glow.SetColor(INT_UIF_Color.WithAlpha(color, 44));
	}

	void SetBackdrop(string path)
	{
		if (!m_Backdrop)
			return;
		if (path == "")
		{
			m_Backdrop.Show(false);
			return;
		}
		m_Backdrop.LoadImageFile(0, path);
		m_Backdrop.SetImage(0);
		m_Backdrop.Show(true);
	}

	void SetClass(string cls)
	{
		if (!m_View)
			return;
		if (cls != m_View.GetClass())
		{
			m_View.SetOrientation(Vector(-8, 30, 0));
			m_View.SetZoom(1);
			m_Idle = 0;
		}
		bool ok = m_View.SetClass(cls);
		if (m_HideMask != 0)
			m_View.Show(false);
		m_Missing.Show(cls != "" && !ok);
		m_HintRow.Show(ok);
		if (ok && m_Clip)
			DRL_UIAnim.Reveal(m_Clip, 10, 10, 0, 8, 0.28);
	}

	// Главное превью с выбранным доп. навесным; объект пересобирается всегда.
	void SetBuild(string cls, array<string> attachments)
	{
		if (!m_View)
			return;
		bool changed = cls != m_View.GetClass();
		if (changed)
		{
			m_View.SetOrientation(Vector(-8, 30, 0));
			m_View.SetZoom(1);
			m_Idle = 0;
		}
		bool ok = m_View.Build(cls, attachments);
		if (m_HideMask != 0)
			m_View.Show(false);
		m_Missing.Show(cls != "" && !ok);
		m_HintRow.Show(ok);
		if (ok && changed && m_Clip)
			DRL_UIAnim.Reveal(m_Clip, 10, 10, 0, 8, 0.28);
	}

	// Оверлеи (выпадающие панели, диалоги) прячут модель, пока перекрывают её:
	// 3D рисуется поверх 2D. У каждой причины свой бит.
	static const int HIDE_POPOVER = 1;
	static const int HIDE_DIALOG = 2;
	static const int HIDE_HOVERCARD = 4;

	void SetHidden(int reason, bool hidden)
	{
		if (hidden)
			m_HideMask = m_HideMask | reason;
		else
			m_HideMask = m_HideMask & ~reason;
		if (m_View)
			m_View.Show(m_HideMask == 0);
	}

	void Clear()
	{
		if (m_View)
			m_View.Clear();
		if (m_Missing)
			m_Missing.Show(false);
		if (m_HintRow)
			m_HintRow.Show(false);
	}

	void ResetView()
	{
		if (!m_View)
			return;
		m_View.SetOrientation(Vector(-8, 30, 0));
		m_View.SetZoom(1);
		m_Idle = 0;
	}

	void Tick(float dt)
	{
		if (!m_View || !m_View.GetEntity())
			return;
		if (m_Dragging)
		{
			if (!(GetMouseState(MouseState.LEFT) & MB_PRESSED_MASK))
			{
				m_Dragging = false;
				m_Idle = IDLE_RESUME;
				return;
			}
			int mx;
			int my;
			GetMousePos(mx, my);
			vector o = m_DragStart;
			float pitch = o[0] + (my - m_DragY) * 0.45;
			o[0] = Math.Clamp(pitch, -70, 70);
			o[1] = o[1] + (mx - m_DragX) * 0.55;
			m_View.SetOrientation(o);
			return;
		}
		if (m_Idle > 0)
		{
			m_Idle = m_Idle - dt;
			return;
		}
		vector spin = m_View.GetOrientation();
		spin[1] = spin[1] + SPIN_SPEED * dt;
		if (spin[1] > 360)
			spin[1] = spin[1] - 360;
		m_View.SetOrientation(spin);
	}

	override bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		if (w != m_Hit || button != MouseState.LEFT)
			return false;
		m_Dragging = true;
		GetMousePos(m_DragX, m_DragY);
		m_DragStart = m_View.GetOrientation();
		return true;
	}

	override bool OnDoubleClick(Widget w, int x, int y, int button)
	{
		if (w != m_Hit)
			return false;
		ResetView();
		return true;
	}

	override bool OnMouseWheel(Widget w, int x, int y, int wheel)
	{
		if (w != m_Hit || !m_View)
			return false;
		float z = m_View.GetZoom() + wheel * 0.12;
		m_View.SetZoom(Math.Clamp(z, 0.6, 2.4));
		m_Idle = IDLE_RESUME;
		return true;
	}
}

// Маленький чип: пилюля + иконка + текст. Ширина по тексту.
class DRL_UIChip extends Managed
{
	Widget m_Root;
	ref INT_UIF_Surface m_Bg;
	ImageWidget m_Icon;
	TextWidget m_Text;
	float m_H;

	void Build(Widget parent, float x, float y, float h, string name)
	{
		m_H = h;
		m_Root = INT_UIF_Draw.Frame(parent, x, y, 60, h, name);
		m_Bg = INT_UIF_Surface.Pill(m_Root, 0, 0, 60, h, 0, 0);
		m_Icon = INT_UIF_Draw.Icon(m_Root, 8, (h - 11) * 0.5, 11, "check", 0);
		m_Text = INT_UIF_Draw.Text(m_Root, 24, 0, 40, h, "", 11, 0);
	}

	// Возвращает ширину чипа.
	float Set(string text, string icon, int hue)
	{
		string t = DRL_UIText.Tr(text);
		float tx = 9;
		if (icon != "")
			tx = 24;
		float w = tx + INT_UIF_Draw.TextWidth(t, 11) * 1.08 + 12;
		INT_UIF_Draw.Resize(m_Root, w, m_H);
		INT_UIF_Draw.Resize(m_Bg.GetRoot(), w, m_H);
		m_Bg.SetFill(INT_UIF_Color.WithAlpha(hue, 40));
		m_Bg.SetBorder(INT_UIF_Color.WithAlpha(hue, 100));
		m_Icon.Show(icon != "");
		if (icon != "")
		{
			INT_UIF_Widgets.SetIcon(m_Icon, icon);
			m_Icon.SetColor(hue);
		}
		INT_UIF_Draw.Place(m_Text, tx, 0, w - tx, m_H);
		m_Text.SetText(t);
		m_Text.SetColor(hue);
		return w;
	}

	void Show(bool show)
	{
		if (m_Root)
			m_Root.Show(show);
	}

	// Приоритет ставится всему поддереву: у дочерних виджетов он свой,
	// и 3D-модель рисуется поверх каждого, у кого он не выше её собственного.
	void SetSort(int sort)
	{
		DRL_UIKitUtil.SetSortDeep(m_Root, sort);
	}
}

class DRL_UIKitUtil
{
	// dayz-mcp ui_click ищет обработчик через GetScript/GetUserData виджета и
	// его родителей, а SetHandler не видит. Хит-зоне даём уникальное имя и
	// ссылку на обработчик — тогда клик доходит до него напрямую.
	static void ExposeHit(Widget hit, string name, Managed handler)
	{
		if (!hit)
			return;
		hit.SetName(name);
		if (handler)
			hit.SetUserData(handler);
	}

	// Хит-зона компонента фреймворка — первый виджет «Hit» в его дереве
	// (в поверхностях тоже есть ButtonWidget, поэтому ищем по имени).
	// У степпера две кнопки: повторный вызов находит следующую.
	static void Expose(INT_UIF_Component c, string name)
	{
		if (!c || !c.GetRoot())
			return;
		ExposeHit(c.GetRoot().FindAnyWidget("Hit"), name, c);
	}

	static void SetSortDeep(Widget w, int sort)
	{
		if (!w)
			return;
		w.SetSort(sort);
		Widget c = w.GetChildren();
		while (c)
		{
			SetSortDeep(c, sort);
			c = c.GetSibling();
		}
	}
}
