// INT Quests (client) — admin window «Квестовики» (design B «Полевой дневник», spec.md §7). Left page: every giver
// the server knows (name, key, NPC / object, «выключен» and «без знака» stamps) and «Новый квестовик». Right page:
// the card of the chosen giver — key (typed only for a new one), name, kind, model, item in hands and clothes (picked
// with INT_QST_ClassPicker), greeting, the per-giver sign switch, «выключен», and the quests he gives or takes (read
// only, from the catalog). The server checks the admin list, re-broadcasts the givers after every change
// (INT_QST_Client.s_OnChanged) and answers through INT_QST_Admin.s_OnReply. The card is rebuilt only when another giver
// is chosen: a broadcast refreshes the list and the read-only lines, never what the admin is typing.

// Shared bits of the two admin windows (this file and INT_QST_ExchangeMenu.c).
class INT_QST_GvKit
{
	// Names the server accepts for giver keys and folders (INT_QST_SE_Paths.IsSafeName): latin, digits, '_'.
	static bool IsSafeName(string s, int maxLen)
	{
		int n = s.Length();
		if (n < 1 || n > maxLen)
			return false;
		for (int i = 0; i < n; i++)
		{
			string c = s.Get(i);
			int a = c.ToAscii();
			bool ok = (a >= 97 && a <= 122) || (a >= 65 && a <= 90) || (a >= 48 && a <= 57) || a == 95;
			if (!ok)
				return false;
		}
		return true;
	}

	// Russian plural form of n for "_1" / "_2" / "_5" keys (1 задание, 2 задания, 5 заданий).
	static int Plural(int n)
	{
		int a = n;
		if (a < 0)
			a = -a;
		int m100 = a % 100;
		int m10 = a % 10;
		if (m100 >= 11 && m100 <= 14)
			return 5;
		if (m10 == 1)
			return 1;
		if (m10 >= 2 && m10 <= 4)
			return 2;
		return 5;
	}

	// Paper title plate on the leather (as in the journal and the board), the system name and the admin line under it.
	static void CoverTitle(Widget header, string icon, string titleKey, string subKey)
	{
		Widget plate = INT_UIF_Draw.Frame(header, 24, 14, 330, 54, "TitlePlate");
		int pf = INT_QST_Look.Note();
		int pb = INT_QST_Look.Tan2();
		int ink = INT_QST_Look.Ink();
		int red = INT_QST_Look.Red();
		int redDeep = INT_QST_Look.RedDeep();
		int tan = INT_QST_Look.Tan();
		int tan2 = INT_QST_Look.Tan2();
		INT_UIF_Surface.Box(plate, 0, 0, 330, 54, 4, pf, pb);
		INT_QST_Look.Tex(plate, 0, 0, 330, 54, "int_qst_paper_ca", pf);
		INT_QST_Look.Icon(plate, 14, 13, 28, icon, redDeep);
		string brand = INT_QST_Look.Spaced("DayZavr Quests");
		INT_QST_Look.Heavy(plate, 54, 6, 270, 18, brand, 11, red);
		string title = INT_QST_Look.Upper(titleKey);
		INT_QST_Look.Heavy(plate, 54, 22, 270, 28, title, 20, ink);
		INT_QST_Look.Text(header, 382, 14, 400, 20, "DayZavr Quest System", 14, tan);
		INT_QST_Look.Icon(header, 382, 42, 16, "shield", tan2);
		string sub = INT_QST_Look.Tr(subKey);
		INT_QST_Look.Text(header, 404, 38, 700, 24, sub, 14, tan2);
	}

	// Leather tile «В журнал» left of the close tile (the journal opens on its admin tab again).
	static void JournalTile(Widget header, float w, INT_QST_Clicks clicks, int id)
	{
		float bw = 180;
		Widget f = INT_UIF_Draw.Frame(header, w - 64 - 14 - bw, 19, bw, 40, "ToJournal");
		int cf = INT_QST_Look.LeatherDark();
		int cb = INT_QST_Look.LeatherLine();
		INT_UIF_Surface.Box(f, 0, 0, bw, 40, 6, cf, cb);
		Widget hover = INT_UIF_Draw.Frame(f, 0, 0, bw, 40, "Hover");
		int hf = INT_QST_Look.CreamA(26);
		int hb = INT_QST_Look.Tan2();
		INT_UIF_Surface.Box(hover, 0, 0, bw, 40, 6, hf, hb);
		int tan = INT_QST_Look.Tan();
		int cream = INT_QST_Look.Cream();
		INT_QST_Look.Icon(f, 14, 11, 18, "book", tan);
		string t = INT_QST_Look.Upper("#STR_INT_QST_A_TO_JOURNAL");
		INT_QST_Look.Heavy(f, 42, 0, bw - 50, 40, t, 14, cream);
		ButtonWidget hit = INT_UIF_Draw.Hit(f, 0, 0, bw, 40, clicks);
		clicks.Bind(hit, id, 0, hover);
	}

	// Wrapped grey lines of a stringtable text (at most maxLines); returns the height used.
	static float HintLines(Widget p, float x, float y, float w, string key, float size, int maxLines)
	{
		array<string> lines = new array<string>();
		string text = INT_QST_Look.Tr(key);
		// a little narrower than the box: long paths have no spaces and the width estimate is short for them
		INT_QST_Text.Wrap(text, w * 0.93, size, lines);
		int c = INT_QST_Look.Ink3();
		float step = size + 5;
		float ly = y;
		int k = 0;
		foreach (string ln : lines)
		{
			if (k >= maxLines)
				break;
			INT_QST_Look.Text(p, x, ly, w, step, ln, size, c);
			ly = ly + step;
			k++;
		}
		return ly - y;
	}
}

// Faux-bold text whose words change on rebind (INT_QST_Look.Heavy draws the pair once and returns only one half).
class INT_QST_GvTwin
{
	TextWidget m_Back;
	TextWidget m_Front;

	static INT_QST_GvTwin Make(Widget p, float x, float y, float w, float h, float size, int color, int align = 0)
	{
		INT_QST_GvTwin t = new INT_QST_GvTwin();
		float dx = size * 0.045;
		if (dx < 0.6)
			dx = 0.6;
		t.m_Back = INT_QST_Look.Text(p, x + dx, y, w, h, "", size, color, align);
		t.m_Front = INT_QST_Look.Text(p, x, y, w, h, "", size, color, align);
		return t;
	}

	void SetLine(string s)
	{
		if (m_Back)
			m_Back.SetText(s);
		if (m_Front)
			m_Front.SetText(s);
	}

	void Tint(int c)
	{
		if (m_Back)
			m_Back.SetColor(c);
		if (m_Front)
			m_Front.SetColor(c);
	}
}

// --- left page: the givers -------------------------------------------------------------------------------------------
class INT_QST_GvSlot extends INT_QST_VSlot
{
	Widget m_Sel;
	ImageWidget m_IcNpc;
	ImageWidget m_IcObj;
	ref INT_QST_GvTwin m_Name;
	TextWidget m_Sub;
	Widget m_StOff;
	// SIGNS-OFF (owner 2026-10-31): stamp «без знака»
	// Widget m_StNoSign;
}

class INT_QST_GvListSource extends INT_QST_VSource
{
	static const float ROW_H = 66;

	INT_QST_GiversMenu m_Owner;
	protected float m_OffW;
	// protected float m_SignW;		// SIGNS-OFF (owner 2026-10-31)

	override int Count()
	{
		return INT_QST_Client.s_Givers.Count();
	}

	override float RowHeight(int i)
	{
		return ROW_H;
	}

	override INT_QST_VSlot NewSlot()
	{
		return new INT_QST_GvSlot();
	}

	// Every widget of a row is made here once; BindSlot only changes words, colours and what is shown.
	override void BuildSlot(INT_QST_VSlot slot, float w)
	{
		INT_QST_GvSlot s = INT_QST_GvSlot.Cast(slot);
		float rh = ROW_H - 4;
		Widget hover = INT_UIF_Draw.Frame(s.m_Root, 6, 2, w - 6, rh, "Hover");
		INT_UIF_Draw.Line(hover, 0, 0, w - 6, rh, 0x1F2A2218);
		s.m_Hover = hover;
		int marker = INT_QST_Look.Marker();
		int ink = INT_QST_Look.Ink();
		int ink2 = INT_QST_Look.Ink2();
		int ink3 = INT_QST_Look.Ink3();
		int red = INT_QST_Look.Red();
		s.m_Sel = INT_UIF_Draw.Frame(s.m_Root, 6, 2, w - 6, rh, "Sel");
		INT_QST_Look.Tex(s.m_Sel, 0, 0, w - 6, rh, "int_qst_highlight_ca", marker);
		INT_UIF_Draw.Line(s.m_Sel, 0, 0, 3, rh, ink);
		s.m_IcNpc = INT_QST_Look.Icon(s.m_Root, 20, 21, 24, "person", ink2);
		s.m_IcObj = INT_QST_Look.Icon(s.m_Root, 20, 21, 24, "box", ink2);
		s.m_Name = INT_QST_GvTwin.Make(s.m_Root, 56, 7, w - 70, 26, 17, ink);
		s.m_Sub = INT_QST_Look.Text(s.m_Root, 56, 36, w - 70, 22, "", 13, ink3);
		m_OffW = INT_QST_Look.StampWidth("#STR_INT_QST_GV_ST_OFF", 10);
		// SIGNS-OFF (owner 2026-10-31)
		// m_SignW = INT_QST_Look.StampWidth("#STR_INT_QST_GV_ST_NOSIGN", 10);
		s.m_StOff = INT_QST_Look.Stamp(s.m_Root, w - 14 - m_OffW, 37, "#STR_INT_QST_GV_ST_OFF", red, 10);
		// s.m_StNoSign = INT_QST_Look.Stamp(s.m_Root, w - 14 - m_SignW, 37, "#STR_INT_QST_GV_ST_NOSIGN", ink3, 10);
		INT_QST_Look.Hair(s.m_Root, 14, ROW_H - 1, w - 20);
	}

	override void BindSlot(INT_QST_VSlot slot, int i, float w)
	{
		INT_QST_GvSlot s = INT_QST_GvSlot.Cast(slot);
		if (i < 0 || i >= INT_QST_Client.s_Givers.Count())
			return;
		INT_QST_GiverView g = INT_QST_Client.s_Givers[i];
		if (!g)
			return;
		bool sel = m_Owner && m_Owner.IsSelectedKey(g.Key);
		bool npc = g.Kind != 1;
		if (s.m_Sel)
			s.m_Sel.Show(sel);
		if (s.m_IcNpc)
			s.m_IcNpc.Show(npc);
		if (s.m_IcObj)
			s.m_IcObj.Show(!npc);
		string title = g.Title;
		if (title == "")
			title = g.Key;
		string shown = INT_QST_Look.Fit(title, w - 70, 17);
		s.m_Name.SetLine(shown);
		int nameColor = INT_QST_Look.Ink();
		if (g.Disabled)
			nameColor = INT_QST_Look.Ink3();
		s.m_Name.Tint(nameColor);
		// stamps from the right edge: «выключен», then «без знака» left of it
		float right = w - 14;
		if (s.m_StOff)
			s.m_StOff.Show(g.Disabled);
		if (g.Disabled)
			right = right - m_OffW - 8;
		// SIGNS-OFF (owner 2026-10-31)
		// if (s.m_StNoSign)
		// {
		// 	s.m_StNoSign.Show(g.NoSign);
		// 	if (g.NoSign)
		// 	{
		// 		INT_UIF_Draw.Move(s.m_StNoSign, right - m_SignW, 37);
		// 		right = right - m_SignW - 8;
		// 	}
		// }
		string kind = INT_QST_Look.Tr("#STR_INT_QST_GV_NPC_SHORT");
		if (!npc)
			kind = INT_QST_Look.Tr("#STR_INT_QST_GV_OBJ_SHORT");
		string sub = g.Key + " · " + kind;
		float subW = right - 56 - 6;
		if (s.m_Sub)
			s.m_Sub.SetText(INT_QST_Look.Fit(sub, subW, 13));
	}

	override void OnRowClick(int i, int button)
	{
		if (!m_Owner || i < 0 || i >= INT_QST_Client.s_Givers.Count())
			return;
		INT_QST_GiverView g = INT_QST_Client.s_Givers[i];
		if (g)
			m_Owner.AskSelect(g.Key, false);
	}
}

// --- right page: clothes of an NPC -----------------------------------------------------------------------------------
class INT_QST_GvClothSlot extends INT_QST_VSlot
{
	TextWidget m_Name;
	TextWidget m_Class;
	ButtonWidget m_Del;
	Widget m_DelHover;
}

class INT_QST_GvClothSource extends INT_QST_VSource
{
	static const float ROW_H = 28;

	INT_QST_GiversMenu m_Owner;
	ref array<string> m_Items = new array<string>();

	override int Count()
	{
		return m_Items.Count();
	}

	override float RowHeight(int i)
	{
		return ROW_H;
	}

	override INT_QST_VSlot NewSlot()
	{
		return new INT_QST_GvClothSlot();
	}

	override void BuildSlot(INT_QST_VSlot slot, float w)
	{
		INT_QST_GvClothSlot s = INT_QST_GvClothSlot.Cast(slot);
		Widget hover = INT_UIF_Draw.Frame(s.m_Root, 0, 1, w, ROW_H - 2, "Hover");
		INT_UIF_Draw.Line(hover, 0, 0, w, ROW_H - 2, 0x142A2218);
		s.m_Hover = hover;
		int ink = INT_QST_Look.Ink();
		int ink3 = INT_QST_Look.Ink3();
		float nameW = w * 0.55;
		s.m_Name = INT_QST_Look.Text(s.m_Root, 6, 0, nameW - 8, ROW_H, "", 14, ink);
		s.m_Class = INT_QST_Look.Text(s.m_Root, nameW, 0, w - nameW - 36, ROW_H, "", 12, ink3, INT_UIF_Align.RIGHT);
		// × removes the item
		Widget del = INT_UIF_Draw.Frame(s.m_Root, w - 28, 2, 24, 24, "Del");
		s.m_DelHover = INT_UIF_Draw.Frame(del, 0, 0, 24, 24, "Hover");
		int dh = INT_QST_Look.RedA(40);
		int db = INT_QST_Look.RedA(150);
		INT_UIF_Surface.Box(s.m_DelHover, 0, 0, 24, 24, 4, dh, db);
		int red = INT_QST_Look.Red();
		INT_QST_Look.Icon(del, 7, 7, 10, "close", red);
		if (m_Owner)
			s.m_Del = INT_UIF_Draw.Hit(del, 0, 0, 24, 24, m_Owner.Clicks());
		INT_QST_Look.Hair(s.m_Root, 4, ROW_H - 1, w - 8);
	}

	override void BindSlot(INT_QST_VSlot slot, int i, float w)
	{
		INT_QST_GvClothSlot s = INT_QST_GvClothSlot.Cast(slot);
		string cls = m_Items[i];
		string nm = INT_QST_Text.Name(cls);
		float nameW = w * 0.55;
		if (s.m_Name)
			s.m_Name.SetText(INT_QST_Look.Fit(nm, nameW - 10, 14));
		float classW = w - nameW - 40;
		if (s.m_Class)
			s.m_Class.SetText(INT_QST_Look.Fit(cls, classW, 12));
		if (m_Owner && s.m_Del)
			m_Owner.Clicks().Bind(s.m_Del, INT_QST_GiversMenu.ID_CLOTH_DEL, i, s.m_DelHover);
	}
}

// --- right page: quests the giver gives / takes (read only) ---------------------------------------------------------
class INT_QST_GvQuestRow
{
	int m_QuestId;
	int m_Role;		// 1 gives, 2 takes, 3 both
}

class INT_QST_GvQuestSlot extends INT_QST_VSlot
{
	ImageWidget m_SgGive;
	ImageWidget m_SgTake;
	TextWidget m_Name;
	TextWidget m_Role;
}

class INT_QST_GvQuestSource extends INT_QST_VSource
{
	static const float ROW_H = 24;

	ref array<ref INT_QST_GvQuestRow> m_Rows = new array<ref INT_QST_GvQuestRow>();

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
		return new INT_QST_GvQuestSlot();
	}

	override void BuildSlot(INT_QST_VSlot slot, float w)
	{
		INT_QST_GvQuestSlot s = INT_QST_GvQuestSlot.Cast(slot);
		int ink = INT_QST_Look.Ink();
		int ink3 = INT_QST_Look.Ink3();
		s.m_SgGive = INT_QST_Look.Sign(s.m_Root, 2, 3, 18, "avail");
		s.m_SgTake = INT_QST_Look.Sign(s.m_Root, 2, 3, 18, "turnin");
		s.m_Name = INT_QST_Look.Text(s.m_Root, 28, 0, w - 28 - 150, ROW_H, "", 14, ink);
		s.m_Role = INT_QST_Look.Text(s.m_Root, w - 150, 0, 146, ROW_H, "", 12, ink3, INT_UIF_Align.RIGHT);
	}

	override void BindSlot(INT_QST_VSlot slot, int i, float w)
	{
		INT_QST_GvQuestSlot s = INT_QST_GvQuestSlot.Cast(slot);
		INT_QST_GvQuestRow r = m_Rows[i];
		INT_QST_QuestDef q = INT_QST_Client.s_Catalog.Get(r.m_QuestId);
		string nm = "#" + r.m_QuestId.ToString();
		if (q)
			nm = q.Name;
		bool gives = (r.m_Role & 1) != 0;
		if (s.m_SgGive)
			s.m_SgGive.Show(gives);
		if (s.m_SgTake)
			s.m_SgTake.Show(!gives);
		if (s.m_Name)
			s.m_Name.SetText(INT_QST_Look.Fit(nm, w - 190, 14));
		string roleKey = "#STR_INT_QST_GV_Q_GIVES";
		if (r.m_Role == 2)
			roleKey = "#STR_INT_QST_GV_Q_TAKES";
		else if (r.m_Role == 3)
			roleKey = "#STR_INT_QST_GV_Q_BOTH";
		if (s.m_Role)
			s.m_Role.SetText(INT_QST_Look.Tr(roleKey));
	}
}

// What the card holds right now: kept over a rebuild of the card (a new giver typed on after «Сохранить»).
class INT_QST_GvCardValues
{
	string Title;
	int Kind;
	string Model;
	string Hands;
	string Greeting;
	// SIGNS-OFF (owner 2026-10-31)
	// bool NoSign;
	bool Disabled;
	int FocusField;		// 1 name, 2 model, 3 in hands (the field that had the keyboard)
	ref array<string> Clothes = new array<string>();
}

// --- the window -------------------------------------------------------------------------------------------------------
class INT_QST_GiversMenu extends INT_QST_Book
{
	static const float FOLD = 480;
	static const float COL_W = 396;		// two columns on the right page
	static const float COL_B = 426;
	static const float GREET_Y = 62;
	static const float GREET_H = 188;
	static const float CLOTH_Y = 262;
	static const float CLOTH_H = 180;
	static const float QUEST_Y = 454;
	static const float QUEST_H = 160;

	static const int ID_NEW = 1;
	static const int ID_SAVE = 2;
	static const int ID_HERE = 3;
	static const int ID_REMOVE = 4;
	static const int ID_PICK_MODEL = 5;
	static const int ID_PICK_HANDS = 6;
	static const int ID_CLOTH_ADD = 7;
	static const int ID_CLOTH_DEL = 8;
	static const int ID_TO_JOURNAL = 9;

	static const int ASK_REMOVE = 1;
	static const int ASK_DISCARD = 2;	// another giver / a new one chosen with unsaved changes
	static const int ASK_CLOSE = 3;		// Esc / close with unsaved changes
	static const int ASK_JOURNAL = 4;

	static const int PICK_MODEL = 1;
	static const int PICK_HANDS = 2;
	static const int PICK_CLOTH = 3;

	static string s_LastKey;		// the card shown last time (window reopened from the journal)

	protected Widget m_ListHead;
	protected Widget m_Right;
	protected Widget m_Form;
	protected Widget m_ClothBlock;
	protected Widget m_ObjNote;
	protected Widget m_QuestBlock;
	protected Widget m_PickLayer;
	protected Widget m_HandsBox;
	protected Widget m_DirtyStamp;
	protected TextWidget m_ClothCount;
	protected TextWidget m_ClothEmpty;
	protected TextWidget m_QuestCount;
	protected TextWidget m_QuestEmpty;
	protected TextWidget m_PosText;

	protected ref INT_QST_VList m_List;
	protected ref INT_QST_GvListSource m_Src;
	protected ref INT_QST_VList m_ClothList;
	protected ref INT_QST_GvClothSource m_ClothSrc;
	protected ref INT_QST_VList m_QuestList;
	protected ref INT_QST_GvQuestSource m_QuestSrc;
	protected ref INT_QST_ClassPicker m_Picker;

	// card inputs (made again for every card)
	protected ref INT_QST_Field m_FKey;
	protected ref INT_QST_Field m_FTitle;
	protected ref INT_QST_Choice m_CKind;
	protected ref INT_QST_Field m_FModel;
	protected ref INT_QST_Field m_FHands;
	protected ref INT_QST_Area m_AGreet;
	// SIGNS-OFF (owner 2026-10-31)
	// protected ref INT_QST_Check m_CNoSign;
	protected ref INT_QST_Check m_CDisabled;

	protected string m_EditKey;		// giver of the card ("" = no card / new giver)
	protected bool m_IsNew;
	protected bool m_HasCard;
	protected int m_Kind;			// 0 NPC, 1 object
	protected bool m_CardDirty;		// unsaved changes on the card (INT_QST_Book.m_DataDirty is the data refresh)
	protected int m_EditGen;		// +1 per change on the card (never reset)
	protected int m_SaveGen = -1;		// m_EditGen when «Сохранить» was sent: more = typed on after it
	protected int m_PickFor;
	protected string m_AskKey;		// where to go after «не сохранять»
	protected bool m_AskNew;
	protected string m_RemoveKey;		// the giver «Убрать» asked about (the list may change under the question)
	protected string m_PendingKey;		// a saved new giver: open his card when the broadcast lists him
	protected int m_WaitOp;			// admin op sent, answer not here yet (no double sends)
	protected string m_WaitKey;		// the giver of that op (the server answers OK with the key)
	protected float m_WaitLeft;
	protected bool m_Hooked;
	protected int m_HeadCount = -1;
	protected int m_SeenShape = -1;		// INT_QST_Client.s_ShapeRevision of the last refresh

	override void GetWindowSize(out float w, out float h)
	{
		w = 1440;
		h = 840;
	}

	override float CoverTop()
	{
		return 92;
	}

	override bool AdminOnly()
	{
		return true;
	}

	override bool WantsKeyboard()
	{
		return true;
	}

	override string ToggleInput()
	{
		return "";
	}

	INT_QST_Clicks Clicks()
	{
		return m_Clicks;
	}

	bool IsSelectedKey(string key)
	{
		return !m_IsNew && m_EditKey != "" && m_EditKey == key;
	}

	// --- cover ----------------------------------------------------------------------------------------------------------
	override void BuildCoverHeader(Widget header, float w)
	{
		INT_QST_GvKit.CoverTitle(header, "person", "#STR_INT_QST_A_GIVERS", "#STR_INT_QST_GV_SUB");
		INT_QST_GvKit.JournalTile(header, w, m_Clicks, ID_TO_JOURNAL);
	}

	// --- body (built once) ---------------------------------------------------------------------------------------------
	override void OnBuild(Widget body)
	{
		Pages(body, FOLD);
		float bh = GetBodyH();
		float bw = GetBodyW();
		int bar = INT_QST_Look.InkA(30);
		int thumb = INT_QST_Look.InkA(120);
		// left page: the givers and «Новый квестовик»
		m_ListHead = INT_UIF_Draw.Frame(body, 0, 0, FOLD, 60, "ListHead");
		m_Src = new INT_QST_GvListSource();
		m_Src.m_Owner = this;
		m_List = new INT_QST_VList();
		m_List.Build(body, 8, 60, FOLD - 28, bh - 60 - 88, m_Src, bar, thumb);
		Widget hv;
		int pc = INT_QST_Look.Sticky();
		Widget nb = INT_QST_Look.Button(body, 30, bh - 74, FOLD - 60, 52, "#STR_INT_QST_GV_NEW", "plus", INT_QST_Look.BTN_INK, pc, hv);
		ButtonWidget nh = INT_UIF_Draw.Hit(nb, 0, 0, FOLD - 60, 52, m_Clicks);
		m_Clicks.Bind(nh, ID_NEW, 0, hv);
		// right page: the card (rebuilt per giver) and the two lists of the second column (built once)
		float rx = FOLD + 60;
		float rw = bw - rx - 34;
		m_Right = INT_UIF_Draw.Frame(body, rx, 0, rw, bh, "Card");
		m_Form = INT_UIF_Draw.Frame(m_Right, 0, 0, rw, bh, "Form");
		BuildClothBlock(bar, thumb);
		BuildQuestBlock(bar, thumb);
		// the class picker draws over everything, so its layer is made last
		m_PickLayer = INT_UIF_Draw.Frame(m_Window, 0, 0, m_WinW, m_WinH, "PickLayer");
		m_Picker = new INT_QST_ClassPicker();
		if (m_Picker.Event_Pick)
			m_Picker.Event_Pick.Insert(OnPicked);
		AddKey("ESC", "#STR_INT_QST_K_CLOSE");
		AddKey(INT_QST_Look.Tr("#STR_INT_QST_K_LMB"), "#STR_INT_QST_K_SELECT");
	}

	protected void BuildClothBlock(int bar, int thumb)
	{
		int ink = INT_QST_Look.Ink();
		int ink2 = INT_QST_Look.Ink2();
		int ink3 = INT_QST_Look.Ink3();
		m_ClothBlock = INT_UIF_Draw.Frame(m_Right, COL_B, CLOTH_Y, COL_W, CLOTH_H, "Clothes");
		string cap = INT_QST_Look.Spaced("#STR_INT_QST_GV_CLOTHES");
		INT_QST_Look.Heavy(m_ClothBlock, 0, 6, 120, 20, cap, 11, ink2);
		m_ClothCount = INT_QST_Look.Text(m_ClothBlock, 112, 4, 80, 22, "", 12, ink3);
		Widget hv;
		Widget ab = INT_QST_Look.Button(m_ClothBlock, COL_W - 150, 0, 150, 28, "#STR_INT_QST_GV_ADD", "plus", INT_QST_Look.BTN_GHOST, ink, hv);
		ButtonWidget ah = INT_UIF_Draw.Hit(ab, 0, 0, 150, 28, m_Clicks);
		m_Clicks.Bind(ah, ID_CLOTH_ADD, 0, hv);
		INT_QST_Look.DoubleRule(m_ClothBlock, 0, 32, COL_W);
		m_ClothSrc = new INT_QST_GvClothSource();
		m_ClothSrc.m_Owner = this;
		m_ClothList = new INT_QST_VList();
		m_ClothList.Build(m_ClothBlock, 0, 40, COL_W, CLOTH_H - 40, m_ClothSrc, bar, thumb);
		string none = INT_QST_Look.Tr("#STR_INT_QST_GV_NO_CLOTHES");
		m_ClothEmpty = INT_QST_Look.Text(m_ClothBlock, 4, 44, COL_W - 8, 24, none, 13, ink3);
		// an object wears nothing: a note in the same place
		m_ObjNote = INT_UIF_Draw.Frame(m_Right, COL_B, CLOTH_Y, COL_W, CLOTH_H, "ObjNote");
		int nf = INT_QST_Look.Note();
		INT_QST_Look.PaperSheet(m_ObjNote, 0, 12, COL_W, CLOTH_H - 24, nf, "Paper");
		INT_QST_Look.Icon(m_ObjNote, 18, 30, 18, "info", ink2);
		INT_QST_GvKit.HintLines(m_ObjNote, 46, 28, COL_W - 64, "#STR_INT_QST_GV_OBJ_NOTE", 14, 6);
		m_ObjNote.Show(false);
	}

	protected void BuildQuestBlock(int bar, int thumb)
	{
		int ink2 = INT_QST_Look.Ink2();
		int ink3 = INT_QST_Look.Ink3();
		m_QuestBlock = INT_UIF_Draw.Frame(m_Right, COL_B, QUEST_Y, COL_W, QUEST_H, "Quests");
		string cap = INT_QST_Look.Spaced("#STR_INT_QST_GV_QUESTS");
		INT_QST_Look.Heavy(m_QuestBlock, 0, 6, COL_W - 60, 20, cap, 11, ink2);
		m_QuestCount = INT_QST_Look.Text(m_QuestBlock, COL_W - 60, 4, 60, 22, "", 12, ink3, INT_UIF_Align.RIGHT);
		INT_QST_Look.DoubleRule(m_QuestBlock, 0, 32, COL_W);
		m_QuestSrc = new INT_QST_GvQuestSource();
		m_QuestList = new INT_QST_VList();
		m_QuestList.Build(m_QuestBlock, 0, 40, COL_W, QUEST_H - 40, m_QuestSrc, bar, thumb);
		string none = INT_QST_Look.Tr("#STR_INT_QST_GV_Q_NONE");
		m_QuestEmpty = INT_QST_Look.Text(m_QuestBlock, 4, 44, COL_W - 8, 24, none, 13, ink3);
	}

	override void OnReady()
	{
		string first = s_LastKey;
		if (first == "" || !INT_QST_Client.Giver(first))
			first = FirstKey();
		m_EditKey = first;
		m_IsNew = false;
		m_SeenShape = INT_QST_Client.s_ShapeRevision;
		DrawListHead();
		m_List.Refresh(true);
		ScrollToKey(first);
		BuildForm();
	}

	override void OnShow()
	{
		super.OnShow();
		if (!m_Hooked)
		{
			INT_QST_Client.s_OnChanged.Insert(OnDataChanged);
			INT_QST_Admin.s_OnReply.Insert(OnAdminReply);
			m_Hooked = true;
		}
		if (!INT_QST_Client.IsAdmin())
			Flash(INT_QST_Look.Tr("#STR_INT_QST_E_17"));
	}

	override void OnHide()
	{
		Unhook();
		if (m_Picker && m_Picker.IsOpen())
			m_Picker.Close();
		super.OnHide();
	}

	void ~INT_QST_GiversMenu()
	{
		Unhook();
		if (m_Picker && m_Picker.Event_Pick)
			m_Picker.Event_Pick.Remove(OnPicked);
	}

	protected void Unhook()
	{
		if (!m_Hooked)
			return;
		m_Hooked = false;
		if (INT_QST_Client.s_OnChanged)
			INT_QST_Client.s_OnChanged.Remove(OnDataChanged);
		if (INT_QST_Admin.s_OnReply)
			INT_QST_Admin.s_OnReply.Remove(OnAdminReply);
	}

	// --- list -----------------------------------------------------------------------------------------------------------
	// Caption and «всего N» (redrawn only when N changes: s_OnChanged comes often).
	protected void DrawListHead()
	{
		int count = INT_QST_Client.s_Givers.Count();
		if (!m_ListHead || count == m_HeadCount)
			return;
		m_HeadCount = count;
		INT_UIF_Widgets.ClearChildren(m_ListHead);
		int ink = INT_QST_Look.Ink();
		int ink3 = INT_QST_Look.Ink3();
		string cap = INT_QST_Look.Spaced("#STR_INT_QST_GV_LIST");
		INT_QST_Look.Heavy(m_ListHead, 30, 20, FOLD - 170, 22, cap, 13, ink);
		string n = count.ToString();
		string all = INT_QST_Look.Fmt("#STR_INT_QST_TOTAL", n);
		INT_QST_Look.Text(m_ListHead, FOLD - 150, 19, 120, 22, all, 14, ink3, INT_UIF_Align.RIGHT);
		INT_QST_Look.DoubleRule(m_ListHead, 30, 49, FOLD - 60);
	}

	protected string FirstKey()
	{
		if (INT_QST_Client.s_Givers.Count() == 0)
			return "";
		INT_QST_GiverView g = INT_QST_Client.s_Givers[0];
		if (!g)
			return "";
		return g.Key;
	}

	protected void ScrollToKey(string key)
	{
		int n = INT_QST_Client.s_Givers.Count();
		for (int i = 0; i < n; i++)
		{
			INT_QST_GiverView g = INT_QST_Client.s_Givers[i];
			if (g && g.Key == key)
			{
				m_List.ScrollToRow(i);
				return;
			}
		}
	}

	// A giver row or «Новый квестовик» was clicked: asks first when the card has unsaved changes.
	void AskSelect(string key, bool asNew)
	{
		if (!asNew && !m_IsNew && key == m_EditKey)
			return;
		if (m_CardDirty)
		{
			m_AskKey = key;
			m_AskNew = asNew;
			string text = INT_QST_Look.Tr("#STR_INT_QST_GV_ASK_DISCARD_TEXT");
			Ask(ASK_DISCARD, 0, "#STR_INT_QST_GV_ASK_DISCARD", text, "#STR_INT_QST_GV_DISCARD", true);
			return;
		}
		SelectCard(key, asNew);
		INT_QST_Sfx.Play("page");
	}

	// keep: values typed on the card that must survive the rebuild (the picker then stays open as well).
	protected void SelectCard(string key, bool asNew, INT_QST_GvCardValues keep = null)
	{
		m_IsNew = asNew;
		m_EditKey = key;
		if (asNew)
			m_EditKey = "";
		else
			s_LastKey = key;
		if (!keep && m_Picker && m_Picker.IsOpen())
			m_Picker.Close();
		m_List.Refresh(false);
		BuildForm(keep);
	}

	// Givers were re-broadcast (or any other client change): the refresh comes from Update, throttled (s_OnChanged
	// comes with every progress packet of the admin as a player too).
	void OnDataChanged()
	{
		if (IsClosingDown())
			return;
		MarkDirty();
	}

	// List, counts and read-only lines follow; the card is rebuilt only when its giver is gone or a freshly saved new
	// giver shows up (what was typed after «Сохранить» is carried over into his card).
	override void OnDirtyRefresh()
	{
		if (IsClosingDown())
			return;
		int shape = INT_QST_Client.s_ShapeRevision;
		if (shape == m_SeenShape)
			return;		// only progress counters moved: nothing of this window
		m_SeenShape = shape;
		DrawListHead();
		if (m_PendingKey != "" && INT_QST_Client.Giver(m_PendingKey))
		{
			string saved = m_PendingKey;
			m_PendingKey = "";
			if (m_CardDirty && m_EditGen != m_SaveGen)
			{
				INT_QST_GvCardValues keep = TakeValues();
				SelectCard(saved, false, keep);
			}
			else
			{
				SetDirty(false);
				SelectCard(saved, false);
			}
			ScrollToKey(saved);
			return;
		}
		bool gone = !m_IsNew && m_EditKey != "" && !INT_QST_Client.Giver(m_EditKey);
		bool appeared = !m_IsNew && m_EditKey == "" && INT_QST_Client.s_Givers.Count() > 0;
		if (gone || appeared)
		{
			string next = FirstKey();
			SetDirty(false);
			SelectCard(next, false);
			return;
		}
		m_List.Refresh(false);
		RefreshPosLine();
		RefreshQuests(false);
	}

	// --- card -----------------------------------------------------------------------------------------------------------
	// The card of m_EditKey (or an empty one for a new giver). Inputs are made again here only (never from an event
	// of one of them: callers are list rows, the question card, the throttled refresh). keep: shown instead of the
	// giver's values, the card stays unsaved.
	protected void BuildForm(INT_QST_GvCardValues keep = null)
	{
		SetFocus(null);
		m_Clicks.ForgetUnder(m_Form);
		INT_UIF_Widgets.ClearChildren(m_Form);
		m_FKey = null;
		m_FTitle = null;
		m_CKind = null;
		m_FModel = null;
		m_FHands = null;
		m_AGreet = null;
		// SIGNS-OFF
		// m_CNoSign = null;
		m_CDisabled = null;
		m_PosText = null;
		m_DirtyStamp = null;
		m_HandsBox = null;
		m_CardDirty = false;
		m_ClothSrc.m_Items.Clear();
		INT_QST_GiverView g = null;
		if (!m_IsNew && m_EditKey != "")
			g = INT_QST_Client.Giver(m_EditKey);
		bool hasCard = false;
		if (m_IsNew)
			hasCard = true;
		else if (g)
			hasCard = true;
		m_HasCard = hasCard;
		if (!m_HasCard)
		{
			BuildEmptyCard();
			ApplyKind();
			RefreshClothes();
			RefreshQuests(true);
			return;
		}
		float rw = GetBodyW() - (FOLD + 60) - 34;
		int ink = INT_QST_Look.Ink();
		int ink3 = INT_QST_Look.Ink3();
		int red = INT_QST_Look.Red();
		// head
		string capKey = "#STR_INT_QST_GV_CARD";
		if (m_IsNew)
			capKey = "#STR_INT_QST_GV_NEW";
		string cap = INT_QST_Look.Spaced(capKey);
		INT_QST_Look.Heavy(m_Form, 0, 20, 500, 22, cap, 13, ink);
		INT_QST_Look.DoubleRule(m_Form, 0, 49, rw);
		float dw = INT_QST_Look.StampWidth("#STR_INT_QST_GV_DIRTY", 10);
		m_DirtyStamp = INT_QST_Look.Stamp(m_Form, rw - dw, 16, "#STR_INT_QST_GV_DIRTY", red, 10);
		if (m_DirtyStamp)
			m_DirtyStamp.Show(false);
		// values
		string title = "";
		int kind = 0;
		string model = "";
		string hands = "";
		string greet = "";
		// SIGNS-OFF (owner 2026-10-31)
		// bool noSign = false;
		bool off = false;
		if (g)
		{
			title = g.Title;
			kind = g.Kind;
			model = g.Model;
			hands = g.InHands;
			greet = g.Greeting;
			// noSign = g.NoSign;		// SIGNS-OFF
			off = g.Disabled;
			if (!keep)
			{
				foreach (string cl : g.Clothes)
					m_ClothSrc.m_Items.Insert(cl);
			}
		}
		if (keep)
		{
			title = keep.Title;
			kind = keep.Kind;
			model = keep.Model;
			hands = keep.Hands;
			greet = keep.Greeting;
			// noSign = keep.NoSign;		// SIGNS-OFF
			off = keep.Disabled;
			foreach (string kc : keep.Clothes)
				m_ClothSrc.m_Items.Insert(kc);
		}
		if (kind != 1)
			kind = 0;
		m_Kind = kind;
		// first column
		if (m_IsNew)
		{
			m_FKey = INT_QST_Field.Create(m_Form, 0, 62, COL_W, "#STR_INT_QST_GV_F_KEY", "", 64);
			m_FKey.Event_Changed.Insert(OnFieldChanged);
		}
		else
			KeyPlate(m_EditKey);
		m_FTitle = INT_QST_Field.Create(m_Form, 0, 124, COL_W, "#STR_INT_QST_GV_F_TITLE", title, 60);
		m_FTitle.Event_Changed.Insert(OnFieldChanged);
		array<string> kinds = new array<string>();
		kinds.Insert("#STR_INT_QST_GV_KIND_NPC");
		kinds.Insert("#STR_INT_QST_GV_KIND_OBJ");
		m_CKind = INT_QST_Choice.Create(m_Form, 0, 186, COL_W, "#STR_INT_QST_GV_F_KIND", kinds, kind);
		m_CKind.Event_Changed.Insert(OnKindChanged);
		m_FModel = INT_QST_Field.Create(m_Form, 0, 246, COL_W - 116, "#STR_INT_QST_GV_F_MODEL", model, 64);
		m_FModel.Event_Changed.Insert(OnFieldChanged);
		PickButton(m_Form, COL_W - 108, 266, ID_PICK_MODEL);
		m_HandsBox = INT_UIF_Draw.Frame(m_Form, 0, 308, COL_W, 54, "Hands");
		m_FHands = INT_QST_Field.Create(m_HandsBox, 0, 0, COL_W - 116, "#STR_INT_QST_GV_F_HANDS", hands, 64);
		m_FHands.Event_Changed.Insert(OnFieldChanged);
		PickButton(m_HandsBox, COL_W - 108, 20, ID_PICK_HANDS);
		// SIGNS-OFF (owner 2026-10-31): «без знака» check (its place stays empty)
		// m_CNoSign = INT_QST_Check.Create(m_Form, 0, 374, COL_W, "#STR_INT_QST_GV_NOSIGN", noSign);
		// m_CNoSign.Event_Changed.Insert(OnCheckChanged);
		// INT_QST_GvKit.HintLines(m_Form, 32, 402, COL_W - 32, "#STR_INT_QST_GV_NOSIGN_HINT", 12, 2);
		m_CDisabled = INT_QST_Check.Create(m_Form, 0, 446, COL_W, "#STR_INT_QST_GV_DISABLED", off);
		m_CDisabled.Event_Changed.Insert(OnCheckChanged);
		INT_QST_GvKit.HintLines(m_Form, 32, 474, COL_W - 32, "#STR_INT_QST_GV_DISABLED_HINT", 12, 2);
		INT_QST_Look.Icon(m_Form, 0, 524, 16, "map_pin", ink3);
		m_PosText = INT_QST_Look.Text(m_Form, 24, 520, COL_W - 24, 24, "", 13, ink3);
		RefreshPosLine();
		// second column: the greeting (clothes and quests are the blocks under it)
		m_AGreet = INT_QST_Area.Create(m_Form, COL_B, GREET_Y, COL_W, GREET_H, "#STR_INT_QST_GV_F_GREET", greet);
		m_AGreet.Event_Changed.Insert(OnAreaChanged);
		string gh = INT_QST_Look.Tr("#STR_INT_QST_GV_GREET_HINT");
		INT_QST_Look.Text(m_Form, COL_B + 160, GREET_Y, COL_W - 160, 18, gh, 12, ink3, INT_UIF_Align.RIGHT);
		DrawCardButtons(rw);
		ApplyKind();
		RefreshClothes();
		RefreshQuests(true);
		if (!keep)
			return;
		SetDirty(true);
		if (keep.FocusField == 1)
			m_FTitle.Focus();
		else if (keep.FocusField == 2)
			m_FModel.Focus();
		else if (keep.FocusField == 3 && m_Kind == 0)
			m_FHands.Focus();
	}

	// The card as it is now (TakeValues -> BuildForm(keep) carries it over a rebuild).
	protected INT_QST_GvCardValues TakeValues()
	{
		INT_QST_GvCardValues v = new INT_QST_GvCardValues();
		v.Kind = m_Kind;
		if (m_FTitle)
		{
			v.Title = m_FTitle.GetText();
			if (m_FTitle.IsFocused())
				v.FocusField = 1;
		}
		if (m_FModel)
		{
			v.Model = m_FModel.GetText();
			if (m_FModel.IsFocused())
				v.FocusField = 2;
		}
		if (m_FHands)
		{
			v.Hands = m_FHands.GetText();
			if (m_FHands.IsFocused())
				v.FocusField = 3;
		}
		if (m_AGreet)
			v.Greeting = m_AGreet.GetText();
		// SIGNS-OFF
		// if (m_CNoSign)
		// 	v.NoSign = m_CNoSign.IsOn();
		if (m_CDisabled)
			v.Disabled = m_CDisabled.IsOn();
		foreach (string cl : m_ClothSrc.m_Items)
			v.Clothes.Insert(cl);
		return v;
	}

	// A field's text for saving: exactly as it was put in when the admin did not touch it, else trimmed.
	protected string FieldValue(INT_QST_Field f)
	{
		if (!f)
			return "";
		string t = f.GetText();
		if (f.IsChanged())
			t.TrimInPlace();
		return t;
	}

	protected void BuildEmptyCard()
	{
		float rw = GetBodyW() - (FOLD + 60) - 34;
		int ink = INT_QST_Look.Ink();
		int ink2 = INT_QST_Look.Ink2();
		string cap = INT_QST_Look.Spaced("#STR_INT_QST_GV_CARD");
		INT_QST_Look.Heavy(m_Form, 0, 20, 500, 22, cap, 13, ink);
		INT_QST_Look.DoubleRule(m_Form, 0, 49, rw);
		INT_QST_Look.Icon(m_Form, 0, 92, 30, "person", ink2);
		string title = INT_QST_Look.Tr("#STR_INT_QST_GV_EMPTY");
		INT_QST_Look.Heavy(m_Form, 44, 88, rw - 44, 34, title, 22, ink);
		INT_QST_GvKit.HintLines(m_Form, 44, 132, rw - 120, "#STR_INT_QST_GV_EMPTY_TEXT", 15, 3);
	}

	// Read-only key of an existing giver (quests refer to him by it).
	protected void KeyPlate(string key)
	{
		int ink2 = INT_QST_Look.Ink2();
		int ink3 = INT_QST_Look.Ink3();
		string cap = INT_QST_Look.Spaced("#STR_INT_QST_GV_F_KEY_RO");
		INT_QST_Look.Text(m_Form, 0, 62, COL_W, 18, cap, 11, ink3);
		int fill = 0x14000000;
		int edge = INT_QST_Look.InkA(50);
		INT_UIF_Surface.Box(m_Form, 0, 82, COL_W, 34, 4, fill, edge);
		INT_QST_Look.Icon(m_Form, 10, 91, 16, "lock", ink3);
		string shown = INT_QST_Look.Fit(key, COL_W - 190, 16);
		INT_QST_Look.Heavy(m_Form, 34, 82, COL_W - 180, 34, shown, 16, ink2);
		string fixedText = INT_QST_Look.Tr("#STR_INT_QST_GV_KEY_FIXED");
		INT_QST_Look.Text(m_Form, COL_W - 150, 82, 140, 34, fixedText, 12, ink3, INT_UIF_Align.RIGHT);
	}

	protected void PickButton(Widget parent, float x, float y, int id)
	{
		Widget hv;
		int ink = INT_QST_Look.Ink();
		Widget b = INT_QST_Look.Button(parent, x, y, 108, 34, "#STR_INT_QST_GV_PICK", "", INT_QST_Look.BTN_GHOST, ink, hv);
		ButtonWidget hit = INT_UIF_Draw.Hit(b, 0, 0, 108, 34, m_Clicks);
		m_Clicks.Bind(hit, id, 0, hv);
	}

	protected void DrawCardButtons(float rw)
	{
		float by = GetBodyH() - 64;
		Widget hv;
		int sc = INT_QST_Look.Sticky();
		Widget sb = INT_QST_Look.Button(m_Form, 0, by, 240, 50, "#STR_INT_QST_GV_SAVE", "save", INT_QST_Look.BTN_INK, sc, hv);
		ButtonWidget sh = INT_UIF_Draw.Hit(sb, 0, 0, 240, 50, m_Clicks);
		m_Clicks.Bind(sh, ID_SAVE, 0, hv);
		if (m_IsNew)
			return;
		int ink = INT_QST_Look.Ink();
		Widget hb = INT_QST_Look.Button(m_Form, 252, by, 240, 50, "#STR_INT_QST_GV_HERE", "map_pin", INT_QST_Look.BTN_GHOST, ink, hv);
		ButtonWidget hh = INT_UIF_Draw.Hit(hb, 0, 0, 240, 50, m_Clicks);
		m_Clicks.Bind(hh, ID_HERE, 0, hv);
		int rc = INT_QST_Look.Red();
		Widget xb = INT_QST_Look.Button(m_Form, rw - 180, by, 180, 50, "#STR_INT_QST_GV_REMOVE", "trash", INT_QST_Look.BTN_DANGER, rc, hv);
		ButtonWidget xh = INT_UIF_Draw.Hit(xb, 0, 0, 180, 50, m_Clicks);
		m_Clicks.Bind(xh, ID_REMOVE, 0, hv);
	}

	// NPC: hands + clothes; object: the note instead.
	protected void ApplyKind()
	{
		bool npc = m_Kind == 0;
		if (m_HandsBox)
			m_HandsBox.Show(npc);
		if (m_ClothBlock)
			m_ClothBlock.Show(m_HasCard && npc);
		if (m_ObjNote)
			m_ObjNote.Show(m_HasCard && !npc);
		if (m_QuestBlock)
			m_QuestBlock.Show(m_HasCard);
	}

	protected void RefreshClothes()
	{
		int n = m_ClothSrc.m_Items.Count();
		m_ClothList.Refresh(false);
		string have = n.ToString();
		string most = INT_QST_Const.MAX_CLOTHES.ToString();
		if (m_ClothCount)
			m_ClothCount.SetText(have + " / " + most);
		if (m_ClothEmpty)
			m_ClothEmpty.Show(n == 0);
	}

	protected void RefreshQuests(bool toTop)
	{
		m_QuestSrc.m_Rows.Clear();
		string key = m_EditKey;
		if (!m_IsNew && key != "")
		{
			foreach (INT_QST_QuestDef q : INT_QST_Client.s_Catalog.m_List)
			{
				int role = 0;
				if (q.GiverStart == key)
					role = role + 1;
				if (q.GiverFinish == key)
					role = role + 2;
				if (role == 0)
					continue;
				INT_QST_GvQuestRow r = new INT_QST_GvQuestRow();
				r.m_QuestId = q.Id;
				r.m_Role = role;
				m_QuestSrc.m_Rows.Insert(r);
			}
		}
		int n = m_QuestSrc.m_Rows.Count();
		m_QuestList.Refresh(toTop);
		if (m_QuestCount)
			m_QuestCount.SetText(n.ToString());
		if (m_QuestEmpty)
			m_QuestEmpty.Show(n == 0);
	}

	// Where the giver stands (follows «Поставить здесь» through the broadcast).
	protected void RefreshPosLine()
	{
		if (!m_PosText)
			return;
		if (m_IsNew)
		{
			m_PosText.SetText(INT_QST_Look.Tr("#STR_INT_QST_GV_POS_NEW"));
			return;
		}
		INT_QST_GiverView g = INT_QST_Client.Giver(m_EditKey);
		if (!g)
		{
			m_PosText.SetText("");
			return;
		}
		string where = INT_QST_Util.VecText(g.Position);
		m_PosText.SetText(INT_QST_Look.Fmt("#STR_INT_QST_GV_POS", where));
	}

	protected void SetDirty(bool on)
	{
		m_CardDirty = on;
		if (on)
			m_EditGen++;
		if (m_DirtyStamp)
			m_DirtyStamp.Show(on);
	}

	void OnFieldChanged(INT_QST_Field f)
	{
		SetDirty(true);
	}

	void OnAreaChanged(INT_QST_Area a)
	{
		SetDirty(true);
	}

	void OnCheckChanged(INT_QST_Check c, bool on)
	{
		SetDirty(true);
	}

	void OnKindChanged(INT_QST_Choice c, int index)
	{
		m_Kind = index;
		SetDirty(true);
		ApplyKind();
	}

	// --- class picker ---------------------------------------------------------------------------------------------------
	protected void OpenPicker(int forWhat)
	{
		if (!m_Picker || !m_PickLayer || !m_HasCard)
			return;
		int kind = INT_QST_ClassPicker.K_CLOTHING;
		string cur = "";
		if (forWhat == PICK_MODEL)
		{
			kind = INT_QST_ClassPicker.K_NPC;
			if (m_Kind == 1)
				kind = INT_QST_ClassPicker.K_OBJECT;
			if (m_FModel)
				cur = m_FModel.GetText();
		}
		else if (forWhat == PICK_HANDS)
		{
			kind = INT_QST_ClassPicker.K_ITEM;
			if (m_FHands)
				cur = m_FHands.GetText();
		}
		m_PickFor = forWhat;
		SetFocus(null);
		INT_QST_Sfx.Play("click");
		m_Picker.Open(m_PickLayer, m_WinW, m_WinH, kind, cur);
	}

	void OnPicked(string cls)
	{
		if (IsClosingDown() || cls == "")
			return;
		if (m_PickFor == PICK_MODEL && m_FModel)
		{
			m_FModel.SetText(cls);
			SetDirty(true);
		}
		else if (m_PickFor == PICK_HANDS && m_FHands)
		{
			m_FHands.SetText(cls);
			SetDirty(true);
		}
		else if (m_PickFor == PICK_CLOTH)
			AddCloth(cls);
	}

	protected void AddCloth(string cls)
	{
		int n = m_ClothSrc.m_Items.Count();
		if (n >= INT_QST_Const.MAX_CLOTHES)
		{
			string most = INT_QST_Const.MAX_CLOTHES.ToString();
			Flash(INT_QST_Look.Fmt("#STR_INT_QST_GV_ERR_CLOTHES_MAX", most));
			return;
		}
		string low = INT_QST_Util.Lower(cls);
		foreach (string have : m_ClothSrc.m_Items)
		{
			string haveLow = INT_QST_Util.Lower(have);
			if (haveLow == low)
			{
				Flash(INT_QST_Look.Tr("#STR_INT_QST_GV_ERR_CLOTHES_DUP"));
				return;
			}
		}
		m_ClothSrc.m_Items.Insert(cls);
		RefreshClothes();
		m_ClothList.ScrollToRow(n);
		SetDirty(true);
	}

	protected void RemoveCloth(int i)
	{
		if (i < 0 || i >= m_ClothSrc.m_Items.Count())
			return;
		m_ClothSrc.m_Items.RemoveOrdered(i);
		RefreshClothes();
		SetDirty(true);
		INT_QST_Sfx.Play("click");
	}

	// --- server requests ------------------------------------------------------------------------------------------------
	protected void StartWait(int op, string key)
	{
		m_WaitOp = op;
		m_WaitKey = key;
		m_WaitLeft = 10;
	}

	// True (and says so) while an earlier request has no answer yet.
	protected bool IsWaiting()
	{
		if (m_WaitOp == 0)
			return false;
		Flash(INT_QST_Look.Tr("#STR_INT_QST_GV_WAIT"), false);
		return true;
	}

	protected void SaveCard()
	{
		if (!m_HasCard || IsWaiting())
			return;
		string key = m_EditKey;
		if (m_IsNew)
		{
			string typed = "";
			if (m_FKey)
				typed = m_FKey.GetText();
			key = INT_QST_Util.Lower(typed);
			key.TrimInPlace();
			if (!INT_QST_GvKit.IsSafeName(key, 64))
			{
				Flash(INT_QST_Look.Tr("#STR_INT_QST_GV_ERR_KEY"));
				return;
			}
			if (INT_QST_Client.Giver(key))
			{
				Flash(INT_QST_Look.Fmt("#STR_INT_QST_GV_ERR_KEY_TAKEN", key));
				return;
			}
		}
		// fields the admin did not touch go back exactly as they came (a save without changes changes nothing)
		string title = FieldValue(m_FTitle);
		string titleCheck = title + "";
		titleCheck.TrimInPlace();
		if (titleCheck == "")
		{
			Flash(INT_QST_Look.Tr("#STR_INT_QST_GV_ERR_TITLE"));
			return;
		}
		string model = FieldValue(m_FModel);
		model.TrimInPlace();
		if (model == "")
		{
			Flash(INT_QST_Look.Tr("#STR_INT_QST_GV_ERR_MODEL"));
			return;
		}
		if (!INT_QST_Util.ClassExists(model))
		{
			Flash(INT_QST_Look.Fmt("#STR_INT_QST_GV_ERR_CLASS", model));
			return;
		}
		INT_QST_GiverDef d = new INT_QST_GiverDef();
		d.Key = key;
		d.Title = title;
		d.Kind = m_Kind;
		d.Model = model;
		d.InHands = "";
		if (m_Kind == 0)
		{
			string hands = FieldValue(m_FHands);
			string handsCheck = hands + "";
			handsCheck.TrimInPlace();
			if (handsCheck == "")
				hands = "";
			if (hands != "" && !INT_QST_Util.ClassExists(hands))
			{
				Flash(INT_QST_Look.Fmt("#STR_INT_QST_GV_ERR_CLASS", hands));
				return;
			}
			d.InHands = hands;
			foreach (string cl : m_ClothSrc.m_Items)
			{
				if (!INT_QST_Util.ClassExists(cl))
				{
					Flash(INT_QST_Look.Fmt("#STR_INT_QST_GV_ERR_CLASS", cl));
					return;
				}
				d.Clothes.Insert(cl);
			}
		}
		string greet = "";
		if (m_AGreet)
		{
			greet = m_AGreet.GetText();
			if (m_AGreet.IsChanged())
				greet.TrimInPlace();
		}
		d.Greeting = INT_QST_Util.FitBytes(greet, INT_QST_Const.MAX_GREETING_BYTES);
		// SIGNS-OFF (owner 2026-10-31)
		// d.NoSign = 0;
		// if (m_CNoSign && m_CNoSign.IsOn())
		// 	d.NoSign = 1;
		d.Disabled = 0;
		if (m_CDisabled && m_CDisabled.IsOn())
			d.Disabled = 1;
		// an existing giver keeps his place («Поставить здесь» moves him); a new one takes the admin's place and facing
		vector pos;
		vector ori;
		if (m_IsNew)
		{
			PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
			if (!p)
				return;
			pos = p.GetPosition();
			ori = p.GetOrientation();
		}
		else
		{
			INT_QST_GiverView g = INT_QST_Client.Giver(key);
			if (!g)
			{
				Flash(INT_QST_Look.Tr("#STR_INT_QST_E_1"));
				return;
			}
			pos = g.Position;
			ori = g.Orientation;
		}
		d.Position = INT_QST_Util.VecText(pos);
		d.Orientation = INT_QST_Util.VecText(ori);
		if (m_IsNew)
			m_PendingKey = key;
		INT_QST_Admin.SaveGiver(d);
		m_SaveGen = m_EditGen;		// what is typed from now on is not in this save
		StartWait(INT_QST_A.GIVER_SAVE, key);
		INT_QST_Sfx.Play("click");
	}

	protected void PlaceHere()
	{
		if (m_IsNew || m_EditKey == "" || IsWaiting())
			return;
		INT_QST_Admin.GiverHere(m_EditKey);
		StartWait(INT_QST_A.GIVER_HERE, m_EditKey);
		INT_QST_Sfx.Play("click");
	}

	protected void AskRemove()
	{
		if (m_IsNew || m_EditKey == "")
			return;
		m_RemoveKey = m_EditKey;	// the answer deletes THIS giver, whatever the list shows by then
		string name = INT_QST_Text.GiverName(m_EditKey);
		string text = INT_QST_Look.Fmt("#STR_INT_QST_GV_ASK_REMOVE_TEXT", name);
		Ask(ASK_REMOVE, 0, "#STR_INT_QST_GV_ASK_REMOVE", text, "#STR_INT_QST_GV_REMOVE", true);
	}

	protected void AskLeave(int tag)
	{
		string text = INT_QST_Look.Tr("#STR_INT_QST_GV_ASK_DISCARD_TEXT");
		Ask(tag, 0, "#STR_INT_QST_GV_ASK_DISCARD", text, "#STR_INT_QST_GV_DISCARD", true);
	}

	protected void OpenJournal()
	{
		INT_QST_Sfx.Play("click");
		INT_UIF_Menu.Open(INT_QST_Const.MENU_JOURNAL);
	}

	// Answers of the giver operations (other admin answers belong to other windows). OK answers carry the giver key:
	// a late answer of an earlier request (after the 10 s wait) does not touch the card shown now.
	void OnAdminReply(int op, int code, string text)
	{
		if (IsClosingDown())
			return;
		if (op != INT_QST_A.GIVER_SAVE && op != INT_QST_A.GIVER_DELETE && op != INT_QST_A.GIVER_HERE)
			return;
		string sentKey = m_WaitKey;
		bool mine = op == m_WaitOp;
		if (mine)
		{
			m_WaitOp = 0;
			m_WaitKey = "";
			m_WaitLeft = 0;
		}
		if (code != INT_QST_Res.OK)
		{
			if (op == INT_QST_A.GIVER_SAVE)
				m_PendingKey = "";
			string codeText = code.ToString();
			Flash(INT_QST_Look.Tr("#STR_INT_QST_E_" + codeText));
			return;
		}
		string forKey = text;
		forKey.TrimInPlace();
		bool sameKey = mine && (forKey == "" || forKey == sentKey);
		if (op == INT_QST_A.GIVER_SAVE)
		{
			// the card of that giver, nothing typed after «Сохранить»: saved; else it stays «не сохранено»
			bool onCard = sameKey && (sentKey == m_EditKey || sentKey == m_PendingKey);
			if (onCard && m_EditGen == m_SaveGen)
				SetDirty(false);
			INT_QST_Sfx.Play("stamp");
			Flash(INT_QST_Look.Tr("#STR_INT_QST_GV_SAVED"), false);
			return;
		}
		if (op == INT_QST_A.GIVER_HERE)
		{
			RefreshPosLine();
			INT_QST_Sfx.Play("stamp");
			Flash(INT_QST_Look.Tr("#STR_INT_QST_GV_MOVED"), false);
			return;
		}
		Flash(INT_QST_Look.Tr("#STR_INT_QST_GV_REMOVED"), false);
	}

	// --- input ----------------------------------------------------------------------------------------------------------
	override void Update(float timeslice)
	{
		super.Update(timeslice);
		if (m_WaitLeft > 0)
		{
			m_WaitLeft = m_WaitLeft - timeslice;
			if (m_WaitLeft <= 0)
				m_WaitOp = 0;
		}
		if (m_Picker)
			m_Picker.Tick(timeslice);
	}

	// Esc: the picker closes first; unsaved changes ask before the window closes.
	override bool OnEscapeOverlay()
	{
		if (m_Picker && m_Picker.IsOpen())
			return m_Picker.OnEscape();
		if (m_CardDirty)
		{
			AskLeave(ASK_CLOSE);
			return true;
		}
		return false;
	}

	override void OnClickId(int id, int data, int button)
	{
		if (id == INT_QST_Book.ID_CLOSE && m_CardDirty)
		{
			AskLeave(ASK_CLOSE);
			return;
		}
		if (CommonClick(id))
			return;
		switch (id)
		{
			case ID_NEW:
				AskSelect("", true);
				return;
			case ID_SAVE:
				SaveCard();
				return;
			case ID_HERE:
				PlaceHere();
				return;
			case ID_REMOVE:
				AskRemove();
				return;
			case ID_PICK_MODEL:
				OpenPicker(PICK_MODEL);
				return;
			case ID_PICK_HANDS:
				OpenPicker(PICK_HANDS);
				return;
			case ID_CLOTH_ADD:
				OpenPicker(PICK_CLOTH);
				return;
			case ID_CLOTH_DEL:
				RemoveCloth(data);
				return;
			case ID_TO_JOURNAL:
				if (m_CardDirty)
				{
					AskLeave(ASK_JOURNAL);
					return;
				}
				OpenJournal();
				return;
		}
	}

	// The wheel over a × of the clothes list scrolls that list.
	override void OnWheelId(int id, int wheel)
	{
		if (id == ID_CLOTH_DEL && m_ClothList)
			m_ClothList.ScrollBy(-wheel * 60);
	}

	override void OnAnswer(int tag, int data, bool yes)
	{
		if (!yes)
			return;
		if (tag == ASK_REMOVE)
		{
			string removeKey = m_RemoveKey;
			m_RemoveKey = "";
			if (removeKey == "" || IsWaiting())
				return;
			if (!INT_QST_Client.Giver(removeKey))
			{
				Flash(INT_QST_Look.Tr("#STR_INT_QST_E_1"));
				return;
			}
			INT_QST_Admin.DeleteGiver(removeKey);
			StartWait(INT_QST_A.GIVER_DELETE, removeKey);
			return;
		}
		if (tag == ASK_DISCARD)
		{
			SetDirty(false);
			SelectCard(m_AskKey, m_AskNew);
			INT_QST_Sfx.Play("page");
			return;
		}
		if (tag == ASK_CLOSE)
		{
			SetDirty(false);
			RequestClose();
			return;
		}
		if (tag == ASK_JOURNAL)
		{
			SetDirty(false);
			OpenJournal();
		}
	}
}
