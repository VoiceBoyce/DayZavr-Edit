// INT Quests (client) — class picker of the admin windows (design B: a paper card pinned over the window). The admin
// finds a config class by its class name or its display name (Cyrillic too, case-insensitive), filtered by kind:
// items, creatures to kill, survivor models for givers, static objects, clothing. The class index is built ONCE per
// game session, lazily on the first Open and time-sliced in Tick (INDEX_BUDGET config entries per frame) over
// CfgVehicles, CfgWeapons and CfgMagazines, the way vanilla PluginRecipesManager walks the roots (scope +
// ConfigGetFullPath, pluginrecipesmanager.c:202-211). Results show in a pooled INT_QST_VList (a fixed set of rows); a
// class from a mod that the admin's client does not load can still be entered as typed.

// The class index (static: one per game session, shared by every picker).
class INT_QST_PickIndex
{
	static const int F_ITEM = 1;		// inventory item (Inventory_Base, weapons, magazines / ammo)
	static const int F_CREATURE = 2;	// kill target
	static const int F_NPC = 4;		// survivor model for a giver
	static const int F_OBJECT = 8;		// static object (House / HouseNoDestruct)
	static const int F_CLOTHING = 16;
	static const int F_PLAYER = 32;		// the "any player" entry (SurvivorBase)
	static const int F_ANIMAL = 64;
	static const int F_ZOMBIE = 128;

	static const int R_VEHICLES = 0;
	static const int R_WEAPONS = 1;
	static const int R_MAGAZINES = 2;

	static ref array<string> s_Class = new array<string>();
	static ref array<string> s_Disp = new array<string>();
	static ref array<string> s_ClassLow = new array<string>();
	static ref array<string> s_DispLow = new array<string>();
	static ref array<int> s_Flags = new array<int>();
	static ref array<int> s_Root = new array<int>();
	static ref array<int> s_Order = new array<int>();		// entries sorted by class name (filled at the end)
	static bool s_Started;
	static bool s_Done;
	static int s_Revision;						// bumps when entries are added (open pickers refilter)

	protected static ref array<string> s_SortKeys = new array<string>();	// "classlow <entry>"
	protected static ref TStringArray s_Path = new TStringArray();
	protected static int s_Stage;		// 0 = walking the roots, 1 = reading the sorted order, 2 = done
	protected static int s_RootIdx;
	protected static int s_Child;
	protected static int s_ChildCount;
	protected static int s_Total;
	protected static int s_Seen;
	protected static int s_ParsePos;

	// case folding (ToLower may be ASCII-only: Cyrillic capitals are replaced pairwise)
	protected static ref array<string> s_Up;
	protected static ref array<string> s_Lo;
	protected static int s_CyrMode = -1;	// -1 unknown, 0 replace needed, 1 ToLower already folds Cyrillic

	static string RootPath(int root)
	{
		if (root == R_WEAPONS)
			return "CfgWeapons";
		if (root == R_MAGAZINES)
			return "CfgMagazines";
		return "CfgVehicles";
	}

	static int Size()
	{
		return s_Class.Count();
	}

	// 0..100
	static int Percent()
	{
		if (s_Done)
			return 100;
		if (s_Stage == 1)
		{
			int keys = s_SortKeys.Count();
			if (keys <= 0)
				return 99;
			return 95 + s_ParsePos * 4 / keys;
		}
		if (s_Total <= 0)
			return 0;
		return s_Seen * 95 / s_Total;
	}

	// Lower case for search keys: ToLower, then the Cyrillic capitals when the engine left them.
	static string Fold(string s)
	{
		string r = s + "";
		r.ToLower();
		if (r.Length() == r.LengthUtf8() || r.Length() > 2000)
			return r;
		if (s_CyrMode < 0)
			INT_QST_PickIndex.InitFold();
		if (s_CyrMode == 1)
			return r;
		int n = s_Up.Count();
		for (int i = 0; i < n; i++)
		{
			string up = s_Up[i];
			string lo = s_Lo[i];
			r.Replace(up, lo);
		}
		return r;
	}

	protected static void InitFold()
	{
		s_Up = new array<string>();
		s_Lo = new array<string>();
		string ups = "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ" + "";
		string los = "абвгдеёжзийклмнопрстуфхцчшщъыьэюя" + "";
		int n = ups.LengthUtf8();
		for (int i = 0; i < n; i++)
		{
			string cu = ups.SubstringUtf8(i, 1);
			string cl = los.SubstringUtf8(i, 1);
			s_Up.Insert(cu);
			s_Lo.Insert(cl);
		}
		string probe = "ЖЁ" + "";
		probe.ToLower();
		s_CyrMode = 0;
		if (probe == "жё")
			s_CyrMode = 1;
	}

	// One slice of the build: `budget` config entries (0 = only start it).
	static void Step(int budget)
	{
		if (s_Done)
			return;
		if (!s_Started)
			INT_QST_PickIndex.Begin();
		int left = budget;
		string rootPath;
		while (left > 0 && s_Stage == 0)
		{
			if (s_Child >= s_ChildCount)
			{
				s_RootIdx++;
				if (s_RootIdx > R_MAGAZINES)
				{
					INT_QST_PickIndex.EndRoots();
					break;
				}
				rootPath = INT_QST_PickIndex.RootPath(s_RootIdx);
				s_ChildCount = GetGame().ConfigGetChildrenCount(rootPath);
				s_Child = 0;
				continue;
			}
			INT_QST_PickIndex.AddChild(s_RootIdx, s_Child);
			s_Child++;
			s_Seen++;
			left--;
		}
		if (s_Stage == 1 && budget > 0)
			INT_QST_PickIndex.ParseOrder(budget * 10);
	}

	protected static void Begin()
	{
		s_Started = true;
		s_Stage = 0;
		s_RootIdx = R_VEHICLES;
		s_Child = 0;
		s_Seen = 0;
		string pv = INT_QST_PickIndex.RootPath(R_VEHICLES);
		string pw = INT_QST_PickIndex.RootPath(R_WEAPONS);
		string pm = INT_QST_PickIndex.RootPath(R_MAGAZINES);
		s_ChildCount = GetGame().ConfigGetChildrenCount(pv);
		int nw = GetGame().ConfigGetChildrenCount(pw);
		int nm = GetGame().ConfigGetChildrenCount(pm);
		s_Total = s_ChildCount + nw + nm;
		// "any player": kills are matched with IsKindOf(victim, Target), every survivor model is a SurvivorBase
		string player = INT_QST_Look.Tr("#STR_INT_QST_PK_PLAYER");
		int pf = F_CREATURE | F_PLAYER;
		INT_QST_PickIndex.Put("SurvivorBase", player, pf, R_VEHICLES, true);
	}

	protected static void AddChild(int root, int child)
	{
		string rp = INT_QST_PickIndex.RootPath(root);
		string cls = "";
		GetGame().ConfigGetChildName(rp, child, cls);
		if (cls == "")
			return;
		string cp = rp + " " + cls;
		int scope = GetGame().ConfigGetInt(cp + " scope");
		if (scope < 1)
			return;
		int flags = 0;
		if (root == R_VEHICLES)
		{
			TStringArray path = s_Path;
			path.Clear();
			GetGame().ConfigGetFullPath(cp, path);
			flags = INT_QST_PickIndex.VehicleFlags(path, scope);
		}
		else if (scope == 2)
		{
			flags = F_ITEM;
		}
		if (flags == 0)
			return;
		string disp = "";
		if (flags != F_OBJECT)
		{
			GetGame().ConfigGetText(cp + " displayName", disp);
			disp.TrimInPlace();
			if (disp.IndexOf("#") == 0 || disp.IndexOf("$") == 0)
				disp = Widget.TranslateString(disp);
		}
		INT_QST_PickIndex.Put(cls, disp, flags, root, false);
	}

	// Kind flags of a CfgVehicles class from its inheritance chain (lower-case compare: config names ignore case).
	protected static int VehicleFlags(TStringArray path, int scope)
	{
		bool inv = false;
		bool clo = false;
		bool ani = false;
		bool zom = false;
		bool sur = false;
		bool house = false;
		int n = path.Count();
		for (int i = 0; i < n; i++)
		{
			string low = INT_QST_Util.Lower(path[i]);
			if (low == "inventory_base")
				inv = true;
			else if (low == "clothing_base")
				clo = true;
			else if (low == "animalbase")
				ani = true;
			else if (low == "zombiebase")
				zom = true;
			else if (low == "survivorbase")
				sur = true;
			else if (low == "house")
				house = true;
		}
		int f = 0;
		if (scope == 2)
		{
			if (sur)
				f = f | F_CREATURE | F_NPC;
			if (ani)
				f = f | F_CREATURE | F_ANIMAL;
			if (zom)
				f = f | F_CREATURE | F_ZOMBIE;
			if (clo)
				f = f | F_CLOTHING | F_ITEM;
			else if (inv)
				f = f | F_ITEM;
		}
		if (house)
			f = f | F_OBJECT;
		return f;
	}

	protected static void Put(string cls, string disp, int flags, int root, bool first)
	{
		int e = s_Class.Count();
		string low = INT_QST_Util.Lower(cls);
		string dlow = INT_QST_PickIndex.Fold(disp);
		s_Class.Insert(cls);
		s_Disp.Insert(disp);
		s_ClassLow.Insert(low);
		s_DispLow.Insert(dlow);
		s_Flags.Insert(flags);
		s_Root.Insert(root);
		// a space sorts before every character of a class name: the special entry goes first
		string sk = low + " " + e.ToString();
		if (first)
			sk = " " + e.ToString();
		s_SortKeys.Insert(sk);
		s_Revision++;
	}

	protected static void EndRoots()
	{
		s_SortKeys.Sort();
		s_Stage = 1;
		s_ParsePos = 0;
	}

	protected static void ParseOrder(int count)
	{
		int total = s_SortKeys.Count();
		int stop = s_ParsePos + count;
		if (stop > total)
			stop = total;
		for (int i = s_ParsePos; i < stop; i++)
		{
			string k = s_SortKeys[i];
			int sp = k.LastIndexOf(" ");
			string num = k.Substring(sp + 1, k.Length() - sp - 1);
			s_Order.Insert(num.ToInt());
		}
		s_ParsePos = stop;
		if (s_ParsePos < total)
			return;
		s_SortKeys.Clear();
		s_Stage = 2;
		s_Done = true;
		s_Revision++;
	}

	// Short word on the right of a row: what kind of class it is.
	static string TagKey(int e)
	{
		int root = s_Root[e];
		int fl = s_Flags[e];
		if (root == R_WEAPONS)
			return "#STR_INT_QST_PK_TAG_WEAPON";
		if (root == R_MAGAZINES)
			return "#STR_INT_QST_PK_TAG_MAG";
		if ((fl & F_PLAYER) != 0)
			return "#STR_INT_QST_PK_TAG_PLAYER";
		if ((fl & F_NPC) != 0)
			return "#STR_INT_QST_PK_TAG_SURVIVOR";
		if ((fl & F_ZOMBIE) != 0)
			return "#STR_INT_QST_PK_TAG_ZOMBIE";
		if ((fl & F_ANIMAL) != 0)
			return "#STR_INT_QST_PK_TAG_ANIMAL";
		if ((fl & F_CLOTHING) != 0)
			return "#STR_INT_QST_PK_TAG_CLOTHING";
		if ((fl & F_ITEM) != 0)
			return "#STR_INT_QST_PK_TAG_ITEM";
		return "#STR_INT_QST_PK_TAG_OBJECT";
	}
}

// One result row (fixed widgets, re-bound while scrolling).
class INT_QST_PickSlot extends INT_QST_VSlot
{
	Widget m_Mark;
	TextWidget m_NameB;
	TextWidget m_Name;
	TextWidget m_Cls;
	TextWidget m_Tag;
}

class INT_QST_PickSource extends INT_QST_VSource
{
	static const float ROW_H = 50;
	static const float TAG_W = 130;

	ref array<int> m_Rows = new array<int>();	// index entries
	string m_CurLow;				// current class (highlighted)
	INT_QST_ClassPicker m_Owner;

	override int Count()
	{
		return m_Rows.Count();
	}

	override float RowHeight(int i)
	{
		return ROW_H;
	}

	override INT_QST_VSlot NewSlot()
	{
		return new INT_QST_PickSlot();
	}

	override void BuildSlot(INT_QST_VSlot slot, float w)
	{
		INT_QST_PickSlot s = INT_QST_PickSlot.Cast(slot);
		float bw = w - 4;
		float bh = ROW_H - 4;
		Widget hover = INT_UIF_Draw.Frame(s.m_Root, 4, 2, bw, bh, "Hover");
		INT_UIF_Draw.Line(hover, 0, 0, bw, bh, 0x1F2A2218);
		s.m_Hover = hover;
		Widget mark = INT_UIF_Draw.Frame(s.m_Root, 4, 2, bw, bh, "Mark");
		INT_QST_Look.Tex(mark, 0, 0, bw, bh, "int_qst_highlight_ca", INT_QST_Look.Marker());
		INT_UIF_Draw.Line(mark, 0, 0, 3, bh, INT_QST_Look.Ink());
		mark.Show(false);
		s.m_Mark = mark;
		float nameW = w - 36 - TAG_W;
		int ink = INT_QST_Look.Ink();
		TextWidget twin = INT_QST_Look.Text(s.m_Root, 16.7, 4, nameW, 24, "", 15, ink);
		TextWidget nm = INT_QST_Look.Text(s.m_Root, 16, 4, nameW, 24, "", 15, ink);
		TextWidget cl = INT_QST_Look.Text(s.m_Root, 16, 27, nameW, 18, "", 12, INT_QST_Look.Ink3());
		TextWidget tg = INT_QST_Look.Text(s.m_Root, w - TAG_W - 10, 0, TAG_W, ROW_H, "", 12, INT_QST_Look.Ink3(), INT_UIF_Align.RIGHT);
		s.m_NameB = twin;
		s.m_Name = nm;
		s.m_Cls = cl;
		s.m_Tag = tg;
		INT_QST_Look.Hair(s.m_Root, 10, ROW_H - 1, w - 16);
	}

	override void BindSlot(INT_QST_VSlot slot, int i, float w)
	{
		INT_QST_PickSlot s = INT_QST_PickSlot.Cast(slot);
		if (!s || !s.m_Name)
			return;
		int e = m_Rows[i];
		string cls = INT_QST_PickIndex.s_Class[e];
		string disp = INT_QST_PickIndex.s_Disp[e];
		float nameW = w - 36 - TAG_W;
		string head = disp;
		string sub = cls;
		float ty = 4;
		if (head == "" || head == cls)
		{
			head = cls;
			sub = "";
			ty = 13;
		}
		head = INT_QST_Look.Fit(head, nameW, 15);
		sub = INT_QST_Look.Fit(sub, nameW, 12);
		INT_UIF_Draw.Place(s.m_NameB, 16.7, ty, nameW, 24);
		INT_UIF_Draw.Place(s.m_Name, 16, ty, nameW, 24);
		s.m_NameB.SetText(head);
		s.m_Name.SetText(head);
		s.m_Cls.SetText(sub);
		string tagKey = INT_QST_PickIndex.TagKey(e);
		string tag = INT_QST_Look.Tr(tagKey);
		s.m_Tag.SetText(tag);
		bool cur = m_CurLow != "" && INT_QST_PickIndex.s_ClassLow[e] == m_CurLow;
		s.m_Mark.Show(cur);
	}

	override void OnRowClick(int i, int button)
	{
		if (button != MouseState.LEFT || i < 0 || i >= m_Rows.Count())
			return;
		if (m_Owner)
			m_Owner.PickEntry(m_Rows[i]);
	}
}

// A closed card and the objects behind it: kept one frame longer, because Close / Open may run inside an event of
// one of them (a row click of the list, Enter in the search field, the host opening the picker again from Event_Pick).
class INT_QST_PickParts
{
	Widget m_Root;
	ref INT_QST_VList m_List;
	ref INT_QST_PickSource m_Src;
	ref INT_QST_Field m_Search;
	ref INT_QST_Choice m_Kinds;
}

class INT_QST_ClassPicker : Managed
{
	static const int K_ITEM = 1;		// inventory items: weapons, magazines / ammo, CfgVehicles items (Inventory_Base)
	static const int K_CREATURE = 2;	// kill targets: animals, infected, survivors + the "any player" entry
	static const int K_NPC = 3;		// survivor models for quest givers
	static const int K_OBJECT = 4;		// static objects for object-givers (House / HouseNoDestruct, scope >= 1)
	static const int K_CLOTHING = 5;	// Clothing_Base
	static const int K_ANY = 6;		// everything above

	static const int MAX_RESULTS = 2000;
	static const int INDEX_BUDGET = 300;	// config entries per Tick while the index is built
	static const float LIST_TOP = 262;

	static const int ID_CLOSE = 1;
	static const int ID_BG = 2;
	static const int ID_TYPED = 3;

	ref ScriptInvoker Event_Pick = new ScriptInvoker();	// (string className)

	protected ref INT_QST_Clicks m_Clicks;
	protected ref INT_QST_VList m_List;
	protected ref INT_QST_PickSource m_Src;
	protected ref INT_QST_Field m_Search;
	protected ref INT_QST_Choice m_Kinds;
	protected ref array<string> m_Tokens = new array<string>();
	protected ref array<int> m_Rest = new array<int>();
	protected Widget m_Root;
	protected Widget m_TypedRow;
	protected TextWidget m_TypedB;
	protected TextWidget m_Typed;
	protected TextWidget m_TypedHint;
	protected TextWidget m_Status;
	protected float m_CardW;
	protected int m_Kind;
	protected string m_Current;
	protected int m_Found;
	protected float m_FilterIn;		// > 0: refilter after this pause (typing)
	protected int m_SeenRevision;
	protected float m_LiveAcc;
	protected bool m_WantScroll;		// scroll to the current class once the sorted index is there
	protected bool m_SubmitPending;		// Enter was pressed in the search field
	protected string m_LastStatus;
	protected ref array<ref INT_QST_PickParts> m_Dead = new array<ref INT_QST_PickParts>();	// hidden, freed later
	protected float m_DeadAt = -1;		// tick time of the last close

	void INT_QST_ClassPicker()
	{
		m_Clicks = new INT_QST_Clicks();
		m_Clicks.Event_Click.Insert(OnClickId);
	}

	void ~INT_QST_ClassPicker()
	{
		if (m_Clicks && m_Clicks.Event_Click)
			m_Clicks.Event_Click.Remove(OnClickId);
	}

	// Cards closed in an earlier frame are unlinked and their objects freed (never in the frame of the close).
	protected void ReleaseDead()
	{
		if (m_Dead.Count() == 0)
			return;
		float now = GetGame().GetTickTime();
		if (now <= m_DeadAt)
			return;
		foreach (INT_QST_PickParts d : m_Dead)
		{
			if (!d || !d.m_Root)
				continue;
			m_Clicks.ForgetUnder(d.m_Root);
			d.m_Root.Unlink();
			d.m_Root = null;
		}
		m_Dead.Clear();
	}

	bool IsOpen()
	{
		return m_Root != null;
	}

	// Modal overlay (dim + paper card) inside `layer`, a full-window frame of w x h design px.
	void Open(Widget layer, float w, float h, int kind, string current)
	{
		if (IsOpen())
			Close();
		ReleaseDead();
		if (!layer)
			return;
		// the objects of the last card (closed, maybe in this very frame) wait in m_Dead: the new card gets its own
		if (m_List || m_Src || m_Search || m_Kinds)
			Bury(null);
		m_Kind = kind;
		if (m_Kind < K_ITEM || m_Kind > K_ANY)
			m_Kind = K_ANY;
		string cur = current + "";
		cur.TrimInPlace();
		m_Current = cur;
		m_Found = 0;
		m_FilterIn = 0;
		m_LiveAcc = 0;
		m_SubmitPending = false;
		m_LastStatus = "";
		INT_QST_PickIndex.Step(INDEX_BUDGET);
		m_SeenRevision = INT_QST_PickIndex.s_Revision;
		BuildCard(layer, w, h);
		Refilter();
		m_WantScroll = cur != "";
		if (m_Search)
			m_Search.Focus();
		INT_QST_Sfx.Play("page");
	}

	// The card is hidden at once; its widgets and objects are freed in a later frame (Close may run inside an event
	// of one of them: a row click, the × button, Enter in the search field).
	void Close()
	{
		if (!m_Root)
			return;
		Widget fw = GetFocus();
		if (fw && INT_UIF_Widgets.IsDescendant(fw, m_Root))
			SetFocus(null);
		m_Root.Show(false);
		Bury(m_Root);
		m_Root = null;
		m_TypedRow = null;
		m_TypedB = null;
		m_Typed = null;
		m_TypedHint = null;
		m_Status = null;
	}

	// Moves the card root and the objects of the card into m_Dead (freed by ReleaseDead after this frame).
	protected void Bury(Widget root)
	{
		INT_QST_PickParts d = new INT_QST_PickParts();
		d.m_Root = root;
		d.m_List = m_List;
		d.m_Src = m_Src;
		d.m_Search = m_Search;
		d.m_Kinds = m_Kinds;
		m_Dead.Insert(d);
		m_List = null;
		m_Src = null;
		m_Search = null;
		m_Kinds = null;
		m_DeadAt = GetGame().GetTickTime();
	}

	// True when it closed itself.
	bool OnEscape()
	{
		if (!IsOpen())
			return false;
		Close();
		INT_QST_Sfx.Play("close");
		return true;
	}

	// Called by the host every frame: index slices (also while closed, until it is complete), typing pause, status.
	void Tick(float dt)
	{
		ReleaseDead();
		if (INT_QST_PickIndex.s_Started && !INT_QST_PickIndex.s_Done)
			INT_QST_PickIndex.Step(INDEX_BUDGET);
		if (!IsOpen())
			return;
		if (m_SubmitPending)
		{
			m_SubmitPending = false;
			Submit();
			return;
		}
		if (m_FilterIn > 0)
		{
			m_FilterIn = m_FilterIn - dt;
			if (m_FilterIn <= 0)
				Refilter();
		}
		if (m_SeenRevision != INT_QST_PickIndex.s_Revision)
		{
			m_LiveAcc = m_LiveAcc + dt;
			if (INT_QST_PickIndex.s_Done || m_LiveAcc >= 0.6)
			{
				m_LiveAcc = 0;
				m_SeenRevision = INT_QST_PickIndex.s_Revision;
				Refilter();
			}
		}
		UpdateStatus();
		if (m_WantScroll && INT_QST_PickIndex.s_Done)
			ScrollToCurrent();
	}

	// --- building -------------------------------------------------------------------------------------------------------
	protected void BuildCard(Widget layer, float w, float h)
	{
		float cw = 860;
		if (cw > w - 60)
			cw = w - 60;
		float ch = h - 80;
		if (ch > 900)
			ch = 900;
		m_CardW = cw;
		float cx = (w - cw) * 0.5;
		float cy = (h - ch) * 0.5;
		m_Root = INT_UIF_Draw.Frame(layer, 0, 0, w, h, "ClassPicker");
		INT_UIF_Surface.Box(m_Root, 4, 4, w - 8, h - 8, 14, 0x9E0D0905, 0);
		ButtonWidget bg = INT_UIF_Draw.Hit(m_Root, 0, 0, w, h, m_Clicks);
		m_Clicks.Bind(bg, ID_BG);
		Widget card = INT_UIF_Draw.Frame(m_Root, cx, cy, cw, ch, "Card");
		INT_UIF_Surface.Shadow(card, -30, -20, cw + 60, ch + 60, 0x8C000000);
		INT_QST_Look.PaperSheet(card, 0, 0, cw, ch, INT_QST_Look.Note(), "Paper");
		INT_QST_Look.PinHead(card, cw * 0.5 - 13, -12);
		ButtonWidget cardHit = INT_UIF_Draw.Hit(card, 0, 0, cw, ch, m_Clicks);
		m_Clicks.Bind(cardHit, ID_BG);

		// title + close
		int ink = INT_QST_Look.Ink();
		INT_QST_Look.Icon(card, 36, 30, 22, "search", ink);
		string title = INT_QST_Look.Tr("#STR_INT_QST_PK_TITLE");
		INT_QST_Look.Heavy(card, 68, 24, cw - 200, 34, title, 22, ink);
		BuildClose(card, cw - 36 - 36, 22);
		INT_QST_Look.DoubleRule(card, 36, 66, cw - 72);

		// search + kinds
		m_Search = INT_QST_Field.Create(card, 36, 80, cw - 72, "#STR_INT_QST_PK_SEARCH", "", 80);
		m_Search.Event_Changed.Insert(OnSearchChanged);
		m_Search.Event_Submit.Insert(OnSearchSubmit);
		array<string> labels = new array<string>();
		labels.Insert("#STR_INT_QST_PK_K_ITEM");
		labels.Insert("#STR_INT_QST_PK_K_CREATURE");
		labels.Insert("#STR_INT_QST_PK_K_NPC");
		labels.Insert("#STR_INT_QST_PK_K_OBJECT");
		labels.Insert("#STR_INT_QST_PK_K_CLOTHING");
		labels.Insert("#STR_INT_QST_PK_K_ANY");
		m_Kinds = INT_QST_Choice.Create(card, 36, 144, cw - 72, "", labels, m_Kind - 1);
		m_Kinds.Event_Changed.Insert(OnKindChanged);

		// status line: found / index progress, current class on the right
		int ink3 = INT_QST_Look.Ink3();
		m_Status = INT_QST_Look.Text(card, 36, 180, cw * 0.55, 22, "", 13, ink3);
		if (m_Current != "")
		{
			string curText = INT_QST_Look.Fmt("#STR_INT_QST_PK_CURRENT", m_Current);
			curText = INT_QST_Look.Fit(curText, cw * 0.4, 13);
			INT_QST_Look.Text(card, cw * 0.55, 180, cw * 0.45 - 36, 22, curText, 13, ink3, INT_UIF_Align.RIGHT);
		}

		// "use as typed" row (a class of a mod this client does not load) / its hint while nothing is typed
		string hint = INT_QST_Look.Tr("#STR_INT_QST_PK_TYPED_HINT");
		hint = INT_QST_Look.Fit(hint, cw - 72, 13);
		m_TypedHint = INT_QST_Look.Text(card, 36, 208, cw - 72, 42, hint, 13, ink3);
		float rw = cw - 60;
		m_TypedRow = INT_UIF_Draw.Frame(card, 30, 208, rw, 42, "Typed");
		int rf = 0x26FFFFFF;
		int rb = INT_QST_Look.InkA(90);
		INT_UIF_Surface.Box(m_TypedRow, 0, 0, rw, 42, 6, rf, rb);
		Widget typedHover = INT_UIF_Draw.Frame(m_TypedRow, 0, 0, rw, 42, "Hover");
		int hb = INT_QST_Look.Blue();
		INT_UIF_Surface.Box(typedHover, 0, 0, rw, 42, 6, 0x33FFFFFF, hb);
		INT_QST_Look.Icon(m_TypedRow, 14, 12, 18, "pencil", hb);
		TextWidget tb = INT_QST_Look.Text(m_TypedRow, 44.7, 0, rw - 60, 42, "", 15, hb);
		TextWidget tm = INT_QST_Look.Text(m_TypedRow, 44, 0, rw - 60, 42, "", 15, hb);
		m_TypedB = tb;
		m_Typed = tm;
		ButtonWidget typedHit = INT_UIF_Draw.Hit(m_TypedRow, 0, 0, rw, 42, m_Clicks);
		m_Clicks.Bind(typedHit, ID_TYPED, 0, typedHover);

		// results
		m_Src = new INT_QST_PickSource();
		m_Src.m_Owner = this;
		string curLow = INT_QST_Util.Lower(m_Current);
		m_Src.m_CurLow = curLow;
		m_List = new INT_QST_VList();
		int bar = INT_QST_Look.InkA(30);
		int thumb = INT_QST_Look.InkA(120);
		float listH = ch - LIST_TOP - 50;
		m_List.Build(card, 24, LIST_TOP, cw - 48, listH, m_Src, bar, thumb);

		// footer
		string keys = INT_QST_Look.Tr("#STR_INT_QST_PK_HINT");
		INT_QST_Look.Text(card, 36, ch - 40, cw * 0.6, 22, keys, 12, ink3);
		INT_QST_Look.Text(card, cw * 0.6, ch - 40, cw * 0.4 - 36, 22, "DayZavr Quest System", 12, ink3, INT_UIF_Align.RIGHT);
		UpdateTyped();
	}

	protected void BuildClose(Widget card, float x, float y)
	{
		Widget f = INT_UIF_Draw.Frame(card, x, y, 36, 36, "Close");
		int fill = 0x26FFFFFF;
		int edge = INT_QST_Look.InkA(90);
		INT_UIF_Surface.Box(f, 0, 0, 36, 36, 6, fill, edge);
		Widget hover = INT_UIF_Draw.Frame(f, 0, 0, 36, 36, "Hover");
		int hf = INT_QST_Look.RedA(40);
		int hb = INT_QST_Look.RedA(200);
		INT_UIF_Surface.Box(hover, 0, 0, 36, 36, 6, hf, hb);
		INT_QST_Look.Icon(f, 10, 10, 16, "close", INT_QST_Look.Ink());
		ButtonWidget hit = INT_UIF_Draw.Hit(f, 0, 0, 36, 36, m_Clicks);
		m_Clicks.Bind(hit, ID_CLOSE, 0, hover);
	}

	// --- filter -----------------------------------------------------------------------------------------------------------
	protected int NeedFlags()
	{
		switch (m_Kind)
		{
			case K_ITEM:
				return INT_QST_PickIndex.F_ITEM;
			case K_CREATURE:
				return INT_QST_PickIndex.F_CREATURE;
			case K_NPC:
				return INT_QST_PickIndex.F_NPC;
			case K_OBJECT:
				return INT_QST_PickIndex.F_OBJECT;
			case K_CLOTHING:
				return INT_QST_PickIndex.F_CLOTHING;
		}
		return 0xFFFF;
	}

	protected string TypedText()
	{
		if (!m_Search)
			return "";
		string t = m_Search.GetText();
		t.TrimInPlace();
		return t;
	}

	// Matches of the kind and of every search word (class name or display name); prefix matches of the first word
	// first, then the rest, both in class-name order once the index is sorted. At most MAX_RESULTS rows.
	protected void Refilter()
	{
		m_FilterIn = 0;
		if (!m_Src || !m_List)
			return;
		string raw = TypedText();
		string q = INT_QST_PickIndex.Fold(raw);
		m_Tokens.Clear();
		q.Split(" ", m_Tokens);
		int nt = m_Tokens.Count();
		int need = NeedFlags();
		m_Src.m_Rows.Clear();
		m_Rest.Clear();
		m_Found = 0;
		bool sorted = INT_QST_PickIndex.s_Done;
		int n = INT_QST_PickIndex.Size();
		for (int k = 0; k < n; k++)
		{
			int e = k;
			if (sorted)
				e = INT_QST_PickIndex.s_Order[k];
			int fl = INT_QST_PickIndex.s_Flags[e];
			if ((fl & need) == 0)
				continue;
			int rank = 1;
			bool ok = true;
			for (int ti = 0; ti < nt; ti++)
			{
				int pc = INT_QST_PickIndex.s_ClassLow[e].IndexOf(m_Tokens[ti]);
				int pd = -1;
				if (pc != 0)
					pd = INT_QST_PickIndex.s_DispLow[e].IndexOf(m_Tokens[ti]);
				if (pc < 0 && pd < 0)
				{
					ok = false;
					break;
				}
				if (ti == 0 && (pc == 0 || pd == 0))
					rank = 0;
			}
			if (!ok)
				continue;
			m_Found++;
			if (rank == 0 && nt > 0)
			{
				if (m_Src.m_Rows.Count() < MAX_RESULTS)
					m_Src.m_Rows.Insert(e);
			}
			else if (m_Rest.Count() < MAX_RESULTS)
			{
				m_Rest.Insert(e);
			}
		}
		foreach (int re : m_Rest)
		{
			if (m_Src.m_Rows.Count() >= MAX_RESULTS)
				break;
			m_Src.m_Rows.Insert(re);
		}
		m_Rest.Clear();
		m_List.Refresh(true);
		m_LastStatus = "";
		UpdateStatus();
	}

	protected void UpdateStatus()
	{
		if (!m_Status)
			return;
		string found = m_Found.ToString();
		string text;
		if (m_Found == 0 && INT_QST_PickIndex.s_Done)
			text = INT_QST_Look.Tr("#STR_INT_QST_PK_EMPTY");
		else if (m_Found > MAX_RESULTS)
		{
			string maxText = MAX_RESULTS.ToString();
			text = INT_QST_Look.Fmt("#STR_INT_QST_PK_FOUND_MAX", found, maxText);
		}
		else
			text = INT_QST_Look.Fmt("#STR_INT_QST_PK_FOUND", found);
		if (!INT_QST_PickIndex.s_Done)
		{
			int pct = INT_QST_PickIndex.Percent();
			string pctText = pct.ToString();
			string building = INT_QST_Look.Fmt("#STR_INT_QST_PK_BUILDING", pctText, "%");
			text = building + "  ·  " + text;
		}
		if (text == m_LastStatus)
			return;
		m_LastStatus = text;
		m_Status.SetText(text);
	}

	protected void UpdateTyped()
	{
		if (!m_TypedRow)
			return;
		string t = TypedText();
		bool has = t != "";
		m_TypedRow.Show(has);
		if (m_TypedHint)
			m_TypedHint.Show(!has);
		if (!has)
			return;
		string line = INT_QST_Look.Fmt("#STR_INT_QST_PK_USE_TYPED", t);
		line = INT_QST_Look.Fit(line, m_CardW - 120, 15);
		m_TypedB.SetText(line);
		m_Typed.SetText(line);
	}

	protected void ScrollToCurrent()
	{
		m_WantScroll = false;
		if (!m_Src || !m_List || m_Src.m_CurLow == "")
			return;
		int n = m_Src.m_Rows.Count();
		for (int i = 0; i < n; i++)
		{
			int e = m_Src.m_Rows[i];
			if (INT_QST_PickIndex.s_ClassLow[e] == m_Src.m_CurLow)
			{
				m_List.ScrollToRow(i);
				return;
			}
		}
	}

	// --- events -----------------------------------------------------------------------------------------------------------
	void OnSearchChanged(INT_QST_Field f)
	{
		m_FilterIn = 0.15;
		UpdateTyped();
	}

	// Enter: done on the next Tick (closing here would destroy the edit box inside its own OnChange).
	void OnSearchSubmit(INT_QST_Field f)
	{
		m_SubmitPending = true;
	}

	// The first row, or the typed text when nothing matches.
	protected void Submit()
	{
		if (m_FilterIn > 0)
			Refilter();
		if (m_Src && m_Src.m_Rows.Count() > 0)
		{
			PickEntry(m_Src.m_Rows[0]);
			return;
		}
		PickTyped();
	}

	void OnKindChanged(INT_QST_Choice c, int index)
	{
		m_Kind = index + 1;
		Refilter();
	}

	void OnClickId(int id, int data, int button)
	{
		if (button != MouseState.LEFT)
			return;
		if (id == ID_CLOSE)
		{
			Close();
			INT_QST_Sfx.Play("close");
			return;
		}
		if (id == ID_TYPED)
			PickTyped();
	}

	void PickEntry(int e)
	{
		if (e < 0 || e >= INT_QST_PickIndex.Size())
			return;
		string cls = INT_QST_PickIndex.s_Class[e];
		Choose(cls);
	}

	protected void PickTyped()
	{
		string t = TypedText();
		string cls = INT_QST_Util.ReplaceByte(t, " ", "");
		if (cls == "")
			return;
		Choose(cls);
	}

	protected void Choose(string cls)
	{
		string pick = cls + "";
		Close();
		INT_QST_Sfx.Play("click");
		Event_Pick.Invoke(pick);
	}
}
