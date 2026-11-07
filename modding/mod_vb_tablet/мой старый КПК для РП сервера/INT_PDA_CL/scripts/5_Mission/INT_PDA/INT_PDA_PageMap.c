// INT_PDA (client) — «Карта» (spec.md §5, concept E «ЗОНА 2.0» map frame design/concepts_v1/5_e_zone2_map.png).
// No MAP module: the «нет карты» static and a plate. With MAP: a MapWidget over the whole page (own literal layout
// int_pda_map.layout), panned while the LEFT button is held (owner 2026-10-23: nothing else moves the view), zoomed by
// the wheel (about the cursor) and the +/− buttons;
// scale bar bottom left. GPS: the «Я» arrow (player heading, pulsing rings) every frame, «центр», coordinates, grid
// square and heading. GPS + GP module + faction: faction marks, mates (asked on open / «Обновить позиции», gap
// MATES_GAP_MS) and SOS (confirm, gap SOS_GAP_MS). Personal marks live only on this client (one JSON per server, at
// most 50) together with the layer switches. RMB or a double click on the map: kind chooser -> name prompt; a click on
// a marker: its card (delete / hide / centre).
// Rules of the map (measured on INT_Radiation / INT_PvEZ / INT_Quests): the MapWidget draws over every widget whose
// own sort is below 100, so every widget of our layers gets 290..340 (SortAll) and the map is hidden while the
// menu's prompt is up (the prompt lives below it) and while the PDA closes (it would draw over the shutter).
// MapToScreen gives absolute screen px: map-local = screen - the map's screen position. The map itself ignores the
// pointer: one hit widget takes the mouse, markers are hit-tested in script. Marker widgets are pooled (created only
// when a data change needs more, never per frame); per frame only the existing ones move, and only while the view
// changes. Rotation: the arrow sits in a frame and the FRAME is rotated (3rd angle of SetRotation; a rotated leaf
// image clips its corners).
// The view belongs to this script: every change goes through MapTo / MapScale, which remember it, and a view the
// widget moved by itself is set back on the next frame (DetectDrift; "map enforce 0" in the self-test only logs it).
// Measured 2026-10-23: the MapWidget takes a left press even under our buttons (ignorepointer or not); after a click
// on «Центрировать» the first one or two mouse moves put the view of the press back (+ a few metres of its own drag),
// so the camera followed the cursor with no button held (owner report). A view change made outside a click (the
// self-test command) is not undone.

class INT_PDA_MapOwnMark
{
	int Kind;					// INT_PDA_Const.MARK_*
	string Name;
	float X;
	float Z;
}

// Client file of the map (one per server): personal marks + layer switches. The JSON reader zero-fills what it
// creates, so the switches are stored as Hide* flags (0 = layer shown).
class INT_PDA_MapPrefs
{
	int Version = 1;
	int HideMine;
	int HideFaction;
	int HideMates;
	int HideSos;
	ref array<ref INT_PDA_MapOwnMark> Marks;

	static INT_PDA_MapPrefs ForRead()
	{
		INT_PDA_MapPrefs p = new INT_PDA_MapPrefs();
		p.Marks = new array<ref INT_PDA_MapOwnMark>();
		return p;
	}
}

// $profile:INT_Mods/INT_PDA/INT_PDA_Marks_<server>.json — read once per server key, written on every change.
class INT_PDA_MapStore
{
	static const int MAX_OWN = 50;
	static const int NAME_CHARS = 32;
	protected static ref INT_PDA_MapPrefs s_Data;
	protected static string s_Key;
	static int s_Saves;			// self-test counter

	// Server key for the file name: anything but A-Z a-z 0-9 '-' becomes '_' ("1.2.3.4:2302" -> "1_2_3_4_2302").
	static string FileKey()
	{
		string raw = INT_PDA_Client.ServerKey();
		string r = "";
		int n = raw.Length();
		int i;
		int c;
		string ch;
		for (i = 0; i < n; i++)
		{
			ch = raw.Get(i);
			c = ch.ToAscii();
			if ((c >= 48 && c <= 57) || (c >= 65 && c <= 90) || (c >= 97 && c <= 122) || c == 45)
				r = r + ch;
			else
				r = r + "_";
		}
		if (r == "")
			r = "local";
		return r;
	}

	static string PathOf(string key)
	{
		return INT_PDA_Client.PREFS_ROOT + "/INT_PDA_Marks_" + key + ".json";
	}

	static INT_PDA_MapPrefs Data()
	{
		string key = FileKey();
		if (s_Data && s_Key == key)
			return s_Data;
		s_Key = key;
		string path = PathOf(key);
		s_Data = Load(path);
		return s_Data;
	}

	static bool Save()
	{
		if (!s_Data || s_Key == "")
			return false;
		if (!FileExist(INT_PDA_Client.PREFS_DIR))
			MakeDirectory(INT_PDA_Client.PREFS_DIR);
		if (!FileExist(INT_PDA_Client.PREFS_ROOT))
			MakeDirectory(INT_PDA_Client.PREFS_ROOT);
		JsonSerializer js = new JsonSerializer();
		string txt;
		if (!js.WriteToString(s_Data, true, txt))
			return false;
		string path = PathOf(s_Key);
		FileHandle fh = OpenFile(path, FileMode.WRITE);
		if (fh == 0)
		{
			INT_PDA_Log.Warn("карта: файл меток не записан");
			return false;
		}
		FPrint(fh, txt);
		CloseFile(fh);
		s_Saves++;
		return true;
	}

	protected static INT_PDA_MapPrefs Load(string path)
	{
		INT_PDA_MapPrefs empty = INT_PDA_MapPrefs.ForRead();
		if (!FileExist(path))
			return empty;
		FileHandle fh = OpenFile(path, FileMode.READ);
		if (fh == 0)
			return empty;
		string txt;
		ReadFile(fh, txt, 200000);
		CloseFile(fh);
		INT_PDA_MapPrefs p = INT_PDA_MapPrefs.ForRead();
		string err;
		JsonSerializer js = new JsonSerializer();
		if (!js.ReadFromString(p, txt, err))
		{
			INT_PDA_Log.Warn("карта: файл меток не прочитан, начат пустой");
			return empty;
		}
		if (!p.Marks)
			p.Marks = new array<ref INT_PDA_MapOwnMark>();
		int i;
		int last;
		INT_PDA_MapOwnMark m;
		for (i = p.Marks.Count() - 1; i >= 0; i--)
		{
			m = p.Marks[i];
			if (!m)
			{
				p.Marks.RemoveOrdered(i);
				continue;
			}
			m.Kind = INT_PDA_Util.ClampI(m.Kind, INT_PDA_Const.MARK_BASE, INT_PDA_Const.MARK_NOTE);
			m.X = INT_PDA_Util.ClampF(m.X, -100000, 100000, 0);
			m.Z = INT_PDA_Util.ClampF(m.Z, -100000, 100000, 0);
			m.Name = INT_PDA_Util.CleanLine(m.Name, NAME_CHARS, INT_PDA_Const.MAX_NAME_BYTES);
		}
		while (p.Marks.Count() > MAX_OWN)
		{
			last = p.Marks.Count() - 1;
			p.Marks.RemoveOrdered(last);
		}
		p.HideMine = INT_PDA_Util.Bit(p.HideMine);
		p.HideFaction = INT_PDA_Util.Bit(p.HideFaction);
		p.HideMates = INT_PDA_Util.Bit(p.HideMates);
		p.HideSos = INT_PDA_Util.Bit(p.HideSos);
		return p;
	}
}

// One pooled marker: badge widgets in the marks layer, the name tag in the labels layer.
class INT_PDA_MapPin : Managed
{
	Widget m_Root;
	ImageWidget m_Halo;
	ImageWidget m_Pulse;
	ImageWidget m_Edge;
	ImageWidget m_Back;
	ImageWidget m_Fill;
	ImageWidget m_Icon;
	Widget m_Tag;
	ImageWidget m_TagBack;
	TextWidget m_TagText;
	TextWidget m_TagSub;
	// what it shows
	int m_Type;					// INT_PDA_PageMap.PIN_*
	int m_Ref;					// own: index in the file; faction: mark id; mate / SOS: list index
	string m_Key;				// mate: callsign; SOS: "callsign:time"
	string m_Name;
	int m_Kind;
	int m_Time;					// SOS: arrival (game ms)
	float m_X;
	float m_Z;
	int m_Color;
	int m_TagInk;
	int m_TagBackColor;
	string m_IconName;
	string m_SubLast;
	// geometry (screen px unless said otherwise)
	float m_Size;				// badge, design px
	float m_HalfPx;
	float m_HitPx;
	float m_TagW;				// design px, 0 = no tag
	float m_TagWPx;
	float m_TagHPx;
	float m_TagHalfPx;
	float m_TagDyPx;
	float m_StackDyPx;			// tag shift under the tags of earlier markers on the same spot (screen px)
	float m_LX;					// map-local point
	float m_LY;
	bool m_Shown;
	bool m_TagShown;
	bool m_OnScreen;
	bool m_Hot;
}

// A button of the map overlay: solid underlay (the map shows through UIF surfaces), hairline box, icon, caps text.
class INT_PDA_MapBtn : Managed
{
	Widget m_Root;
	ImageWidget m_Under;
	ImageWidget m_Icon;
	TextWidget m_Text;
	ButtonWidget m_Hit;
	int m_Fill;
	int m_FillHot;
	int m_FillOff;
	int m_Ink;
	int m_InkOff;
	bool m_Enabled = true;
	bool m_Hot;
	string m_Label;

	void SetEnabled(bool on)
	{
		if (m_Enabled == on)
			return;
		m_Enabled = on;
		Paint();
	}

	void SetHot(bool hot)
	{
		if (m_Hot == hot)
			return;
		m_Hot = hot;
		Paint();
	}

	void SetLabel(string text)
	{
		if (!m_Text || text == m_Label)
			return;
		m_Label = text;
		m_Text.SetText(text);
	}

	void SetIconName(string iconName)
	{
		if (!m_Icon)
			return;
		string path = INT_PDA_Look.IconPath(iconName);
		m_Icon.LoadImageFile(0, path);
		m_Icon.SetImage(0);
		Paint();
	}

	void Paint()
	{
		int fill = m_Fill;
		int ink = m_Ink;
		if (!m_Enabled)
		{
			fill = m_FillOff;
			ink = m_InkOff;
		}
		else if (m_Hot)
		{
			fill = m_FillHot;
		}
		if (m_Under)
			m_Under.SetColor(fill);
		if (m_Icon)
			m_Icon.SetColor(ink);
		if (m_Text)
			m_Text.SetColor(ink);
	}
}

// Toggle switch of concept E (46 x 24 orange track, dark knob) built from images only.
class INT_PDA_MapSwitch : Managed
{
	Widget m_Root;
	TextWidget m_Text;
	ImageWidget m_TrackL;
	ImageWidget m_TrackM;
	ImageWidget m_TrackR;
	ImageWidget m_Knob;
	ButtonWidget m_Hit;
	float m_KnobX0;
	float m_KnobX1;
	float m_KnobY;
	bool m_On;
	bool m_Enabled = true;

	void Paint(bool animate)
	{
		int track = 0xFF2B2B30;
		int knob = 0xFF77767A;
		int ink = INT_PDA_Look.TEXT;
		float kx = m_KnobX0;
		float sx;
		if (m_On)
		{
			track = INT_PDA_Look.ORANGE;
			knob = INT_PDA_Look.BG;
			kx = m_KnobX1;
		}
		if (!m_Enabled)
		{
			ink = INT_PDA_Look.DIM2;
			knob = 0xFF3A3A40;
			track = 0xFF1E1E22;
			if (m_On)
				track = 0xFF5A3418;
		}
		m_TrackL.SetColor(track);
		m_TrackM.SetColor(track);
		m_TrackR.SetColor(track);
		m_Knob.SetColor(knob);
		m_Text.SetColor(ink);
		if (animate)
		{
			sx = INT_UIF_Draw.S(kx);
			INT_UIF_Animator.MoveX(m_Knob, sx, 0.16);
		}
		else
		{
			INT_UIF_Draw.Move(m_Knob, kx, m_KnobY);
		}
	}
}

class INT_PDA_PageMap : INT_PDA_Page
{
	// view
	static const float SCALE_MIN = 0.005;
	static const float SCALE_MAX = 1.0;
	static const float SCALE_START = 0.33;		// vanilla map default
	static const float SCALE_GPS = 0.1;
	static const float WHEEL_STEP = 1.25;
	static const float BUTTON_STEP = 1.6;
	// sorts of the layers (every widget over a MapWidget needs its own sort >= 100)
	static const int SORT_VEIL = 290;
	static const int SORT_PINS = 300;
	static const int SORT_ME = 305;
	static const int SORT_LABELS = 310;
	static const int SORT_INPUT = 315;
	static const int SORT_OVERLAY = 320;
	static const int SORT_POPUP = 340;
	// markers
	static const int PIN_OWN = 0;
	static const int PIN_FACTION = 1;
	static const int PIN_MATE = 2;
	static const int PIN_SOS = 3;
	static const int MAX_PINS = 180;			// 50 own + 64 faction + 60 mates + a few SOS
	static const int MAX_MATES = 60;
	static const float PIN_BOX = 56;
	static const float ME_BOX = 120;
	static const int SOS_SHOW_MS = 900000;		// a SOS stays on the map 15 minutes
	static const float TAG_MAX_W = 320;			// marker tag plate (design px); its text is cut with «…» to fit
	static const float SAME_SPOT_M = 2;			// markers closer than this stack their tags (and share clicks)
	static const int TAGW_CACHE_MAX = 600;
	static const int DOUBLE_MS = 350;
	static const int FACTION_GAP_MS = 2000;
	// mouse
	static const int DRAG_NONE = 0;
	static const int DRAG_CLICK = 1;
	static const int DRAG_PAN = 2;
	// button styles
	static const int ST_NORMAL = 0;
	static const int ST_DANGER = 1;
	static const int ST_SOS = 2;
	// layer switches (index in m_Layers)
	static const int L_MINE = 0;
	static const int L_FACTION = 1;
	static const int L_MATES = 2;
	static const int L_SOS = 3;
	// geometry (design px of the 1456 x 674 page)
	static const float EDGE_X = 30;
	static const float EDGE_Y = 30;
	static const float COL_W = 290;
	static const float ADD_W = 336;
	static const float ADD_H = 256;
	static const float INFO_W = 300;
	static const float INFO_H = 150;
	static const float ASK_W = 420;
	static const float ASK_H = 204;

	// session memory (the page is rebuilt with every menu)
	protected static bool s_HasView;
	protected static float s_ViewScale;
	protected static vector s_ViewPos;
	protected static string s_ViewWorld;
	protected static int s_LastMatesMs = -10000000;
	protected static int s_LastFactionMs = -10000000;
	protected static int s_MatesAtMs = -1;
	protected static ref array<string> s_SosHidden = new array<string>();
	// self-test counters
	static int s_Rebuilds;
	static int s_PinsUsed;
	static int s_Placed;

	// widgets
	protected Widget m_NoMap;
	protected ImageWidget m_Static;
	protected Widget m_MapHost;
	protected MapWidget m_Map;
	protected Widget m_VeilLayer;
	protected Widget m_PinLayer;
	protected Widget m_TagLayer;
	protected Widget m_InputLayer;
	protected Widget m_Overlay;
	protected ButtonWidget m_MapHit;
	protected Widget m_PlateLayers;
	protected Widget m_PlateLegend;
	protected Widget m_PlateRight;
	protected Widget m_ScaleBox;
	protected ImageWidget m_ScaleBar;
	protected ImageWidget m_ScaleTickL;
	protected ImageWidget m_ScaleTickR;
	protected TextWidget m_ScaleText;
	// «Я»
	protected Widget m_MeRoot;
	protected Widget m_MeTurn;
	protected ImageWidget m_MeRing1;
	protected ImageWidget m_MeRing2;
	protected bool m_MeShown;
	protected float m_MeHalfPx;
	protected float m_RingT;
	protected float m_HeadingDeg;
	// right panel
	protected Widget m_NoGpsChip;
	protected TextWidget m_CoordX;
	protected TextWidget m_CoordY;
	protected TextWidget m_Grid;
	protected Widget m_CompassTurn;
	protected TextWidget m_HeadingText;
	protected TextWidget m_MatesAge;
	protected ref array<ref INT_PDA_MapBtn> m_AllBtns = new array<ref INT_PDA_MapBtn>();	// owns every button
	protected INT_PDA_MapBtn m_BtnIn;
	protected INT_PDA_MapBtn m_BtnOut;
	protected INT_PDA_MapBtn m_BtnCenter;
	protected INT_PDA_MapBtn m_BtnRefresh;
	protected INT_PDA_MapBtn m_BtnMark;
	protected INT_PDA_MapBtn m_BtnNext;
	protected INT_PDA_MapBtn m_BtnSos;
	protected INT_PDA_MapBtn m_BtnAddClose;
	protected INT_PDA_MapBtn m_BtnInfoAct;
	protected INT_PDA_MapBtn m_BtnInfoCenter;
	protected INT_PDA_MapBtn m_BtnInfoClose;
	protected INT_PDA_MapBtn m_BtnAskCancel;
	protected INT_PDA_MapBtn m_BtnAskSend;
	protected ref array<ref INT_PDA_MapSwitch> m_Layers = new array<ref INT_PDA_MapSwitch>();
	// popups
	protected Widget m_AddPop;
	protected TextWidget m_AddCoords;
	protected ref array<ButtonWidget> m_KindHits = new array<ButtonWidget>();
	protected ref array<ImageWidget> m_KindUnder = new array<ImageWidget>();
	protected ref INT_PDA_MapSwitch m_AddFactionSw;
	protected TextWidget m_AddPersonal;
	protected Widget m_InfoPop;
	protected ImageWidget m_InfoStripe;
	protected TextWidget m_InfoTitle;
	protected TextWidget m_InfoSub;
	protected TextWidget m_InfoSub2;
	protected Widget m_AskPop;
	// markers
	protected ref array<ref INT_PDA_MapPin> m_Pins = new array<ref INT_PDA_MapPin>();
	protected int m_Used;
	protected int m_SosPins;
	protected int m_Hover = -1;
	protected float m_BlinkT;
	// state
	protected bool m_Built;
	protected bool m_HasMapMod;
	protected bool m_Gps;
	protected bool m_Fac;
	protected bool m_CanManage;
	protected bool m_Dirty;
	protected bool m_FirstShow = true;
	protected bool m_CentreOnMe;
	protected bool m_HiddenByPrompt;
	protected bool m_Closing;
	protected bool m_MapVisible = true;
	protected int m_InitFrames;
	protected int m_PlaceFrames;
	protected float m_SlowT;
	protected float m_ShownT;
	protected float m_StaticT;
	protected float m_LastHostY;
	protected int m_DropFrames;
	protected float m_StillT;
	protected float m_GridSize = 100;
	protected int m_PendingInfo = -1;
	protected int m_PendingFrames;
	protected int m_NextIdx = -1;
	protected int m_LastPicked = -1;			// last marker opened by a click (several on one spot: the next click walks on)
	protected ref map<string, float> m_TagW = new map<string, float>();	// measured tag text widths ("size|text")
	// texts
	protected ref array<string> m_Dirs = new array<string>();
	protected string m_TxtM;
	protected string m_TxtKm;
	protected string m_TxtSos;
	protected string m_TxtRefresh;
	protected string m_TxtGrid;
	protected string m_TxtSecAgo;
	protected string m_TxtMinAgo;
	protected string m_TxtUpdated;
	protected string m_TxtNotUpdated;
	// map rect on the screen (px) and the view
	protected float m_MX;
	protected float m_MY;
	protected float m_MW;
	protected float m_MH;
	protected float m_F = 1;				// INT_UIF_Scale.Factor()
	protected float m_Mpp = 1;				// metres per screen px
	protected float m_LastScale = -1;
	protected vector m_LastPos;
	protected float m_LastMX = -1;
	protected float m_LastMY = -1;
	protected float m_LastMW = -1;
	protected float m_LastMH = -1;
	// mouse
	protected int m_MouseX;
	protected int m_MouseY;
	protected bool m_MouseIn;				// over the map's hit widget
	protected bool m_MouseInRect;
	protected int m_Drag;
	protected int m_DragButton;
	protected bool m_Moved;
	protected int m_DownX;
	protected int m_DownY;
	protected vector m_DownCenter;
	protected float m_DownMpp = 1;
	protected int m_LastClickMs = -100000;
	protected int m_LastClickX;
	protected int m_LastClickY;
	// the view our code set last (DetectDrift)
	protected vector m_ReqPos;
	protected float m_ReqScale;
	protected bool m_HasReq;
	static bool s_Enforce = true;
	static int s_Drifts;
	// zoom about the cursor: the world point that must end up under it (checked once, two frames later)
	protected int m_FixFrames;
	protected vector m_FixWorld;
	protected float m_FixLX;
	protected float m_FixLY;
	// popup state
	protected float m_AddX;
	protected float m_AddZ;
	protected bool m_AddFaction;
	protected bool m_AddForFaction;
	protected int m_AddKind;
	protected int m_InfoType = -1;
	protected int m_InfoRef;
	protected string m_InfoKey;
	protected float m_InfoX;
	protected float m_InfoZ;

	// --- build -------------------------------------------------------------------------------------------------------
	override void OnBuild()
	{
		float f = INT_UIF_Scale.Factor();
		if (f > 0)
			m_F = f;
		CacheTexts();
		BuildNoMap();
		m_MapHost = INT_UIF_Draw.Frame(m_Root, 0, 0, m_W, m_H, "INT_PDA_MapHost");
		Widget root = GetGame().GetWorkspace().CreateWidgets("INT_PDA_CL/gui/layouts/int_pda_map.layout", m_MapHost);
		m_Map = MapWidget.Cast(root);
		if (!m_Map)
		{
			INT_PDA_Log.Err("карта: слой MapWidget не создан");
			m_MapHost.Show(false);
			return;
		}
		m_VeilLayer = root.FindAnyWidget("INT_PDA_MapVeil");
		m_PinLayer = root.FindAnyWidget("INT_PDA_MapMarks");
		m_TagLayer = root.FindAnyWidget("INT_PDA_MapLabels");
		m_InputLayer = root.FindAnyWidget("INT_PDA_MapInput");
		m_Overlay = root.FindAnyWidget("INT_PDA_MapOverlay");
		if (!m_VeilLayer || !m_PinLayer || !m_TagLayer || !m_InputLayer || !m_Overlay)
		{
			INT_PDA_Log.Err("карта: в слое карты нет виджетов");
			m_MapHost.Show(false);
			m_Map = null;
			return;
		}
		string cfg = "CfgWorlds " + GetGame().GetWorldName() + " Grid Zoom1 stepX";
		float grid = GetGame().ConfigGetFloat(cfg);
		if (grid > 0)
			m_GridSize = grid;
		BuildVeil();
		BuildMe();
		m_MapHit = Hit(m_InputLayer, 0, 0, m_W, m_H);
		BuildLeft();
		BuildRight();
		BuildFrameLines();
		BuildPopups();
		SortAll(m_VeilLayer, SORT_VEIL);
		SortAll(m_PinLayer, SORT_PINS);
		SortAll(m_MeRoot, SORT_ME);
		SortAll(m_TagLayer, SORT_LABELS);
		SortAll(m_InputLayer, SORT_INPUT);
		SortAll(m_Overlay, SORT_OVERLAY);
		SortAll(m_AddPop, SORT_POPUP);
		SortAll(m_InfoPop, SORT_POPUP);
		SortAll(m_AskPop, SORT_POPUP);
		m_Built = true;
	}

	protected void CacheTexts()
	{
		m_Dirs.Clear();
		m_Dirs.Insert(Widget.TranslateString("#STR_INT_PDA_MAP_DIR_N"));
		m_Dirs.Insert(Widget.TranslateString("#STR_INT_PDA_MAP_DIR_NE"));
		m_Dirs.Insert(Widget.TranslateString("#STR_INT_PDA_MAP_DIR_E"));
		m_Dirs.Insert(Widget.TranslateString("#STR_INT_PDA_MAP_DIR_SE"));
		m_Dirs.Insert(Widget.TranslateString("#STR_INT_PDA_MAP_DIR_S"));
		m_Dirs.Insert(Widget.TranslateString("#STR_INT_PDA_MAP_DIR_SW"));
		m_Dirs.Insert(Widget.TranslateString("#STR_INT_PDA_MAP_DIR_W"));
		m_Dirs.Insert(Widget.TranslateString("#STR_INT_PDA_MAP_DIR_NW"));
		m_TxtM = Widget.TranslateString("#STR_INT_PDA_MAP_M");
		m_TxtKm = Widget.TranslateString("#STR_INT_PDA_MAP_KM");
		m_TxtSos = INT_PDA_Look.Caps("#STR_INT_PDA_MAP_SOS");
		m_TxtRefresh = INT_PDA_Look.Caps("#STR_INT_PDA_MAP_REFRESH");
		m_TxtGrid = INT_PDA_Look.Caps("#STR_INT_PDA_MAP_GRID");
		m_TxtSecAgo = Widget.TranslateString("#STR_INT_PDA_MAP_SEC_AGO");
		m_TxtMinAgo = Widget.TranslateString("#STR_INT_PDA_MAP_MIN_AGO");
		m_TxtUpdated = Widget.TranslateString("#STR_INT_PDA_MAP_UPDATED");
		m_TxtNotUpdated = Widget.TranslateString("#STR_INT_PDA_MAP_NOT_UPDATED");
	}

	// «Нет карты»: TV static (jittered by Update) under a plate with the Locked text.
	protected void BuildNoMap()
	{
		m_NoMap = INT_UIF_Draw.Clip(m_Root, 0, 0, m_W, m_H, "INT_PDA_NoMap");
		INT_UIF_Draw.Image(m_NoMap, 0, 0, m_W, m_H, "iuf_white", INT_PDA_Look.BG);
		m_Static = INT_UIF_Draw.Texture(m_NoMap, 0, 0, m_W + 256, m_H + 256, INT_PDA_Look.NOMAP, ARGB(70, 255, 255, 255));
		INT_UIF_Draw.Texture(m_NoMap, 0, 0, m_W, m_H, INT_PDA_Look.SET_DEV + "vignette", ARGB(210, 255, 255, 255));
		float pw = 660;
		float ph = 220;
		float px = (m_W - pw) * 0.5;
		float py = m_H * 0.5 - 122;
		int plate = 0xE60B0B0C;
		INT_UIF_Draw.Texture(m_NoMap, px + 3, py + 3, pw - 6, ph - 6, INT_UIF_WHITE, plate);
		INT_UIF_Surface.Box(m_NoMap, px, py, pw, ph, 6, plate, INT_PDA_Look.LINE);
		INT_UIF_Draw.Line(m_NoMap, px, py, pw, 3, INT_PDA_Look.ORANGE);
		Locked(m_NoMap, m_W, m_H, "map", "#STR_INT_PDA_MAP_NOMAP_TITLE", "#STR_INT_PDA_MAP_NOMAP_TEXT");
		m_NoMap.Show(false);
	}

	// A slight dark veil + vignette over the paper map (style E: the map is the background, panels float over it).
	protected void BuildVeil()
	{
		INT_UIF_Draw.Texture(m_VeilLayer, 0, 0, m_W, m_H, INT_UIF_WHITE, ARGB(64, 12, 10, 8));
		INT_UIF_Draw.Texture(m_VeilLayer, 0, 0, m_W, m_H, INT_PDA_Look.SET_DEV + "vignette", ARGB(150, 255, 255, 255));
	}

	protected void BuildMe()
	{
		float b = ME_BOX;
		float c = b * 0.5;
		int ring = INT_PDA_Look.ORANGE;
		int glow = INT_UIF_Color.WithAlpha(ring, 80);
		m_MeRoot = INT_UIF_Draw.Frame(m_PinLayer, 0, 0, b, b, "INT_PDA_MapMe");
		m_MeRing1 = INT_UIF_Draw.Image(m_MeRoot, c - 20, c - 20, 40, 40, "iuf_ringline64", ring);
		m_MeRing2 = INT_UIF_Draw.Image(m_MeRoot, c - 20, c - 20, 40, 40, "iuf_ringline64", ring);
		INT_UIF_Draw.Image(m_MeRoot, c - 30, c - 30, 60, 60, "iuf_radial", glow);
		m_MeTurn = INT_UIF_Draw.Frame(m_MeRoot, c - 22, c - 22, 44, 44, "INT_PDA_MapMeTurn");
		INT_PDA_Look.DrawIcon(m_MeTurn, 0, 0, 44, "arrow", INT_PDA_Look.BG);
		INT_PDA_Look.DrawIcon(m_MeTurn, 6, 6, 32, "arrow", INT_PDA_Look.ORANGE);
		float tw = INT_PDA_Look.TextW("#STR_INT_PDA_MAP_ME", 15) + 14;
		Widget tag = INT_UIF_Draw.Frame(m_MeRoot, c + 18, c - 42, tw, 22, "INT_PDA_MapMeTag");
		INT_UIF_Draw.Texture(tag, 0, 0, tw, 22, INT_UIF_WHITE, INT_PDA_Look.ORANGE);
		INT_UIF_Draw.Text(tag, 0, 0, tw, 22, "#STR_INT_PDA_MAP_ME", 15, INT_PDA_Look.ON_ORANGE, INT_UIF_Align.CENTER);
		m_MeHalfPx = INT_UIF_Draw.S(c);
		m_MeRoot.Show(false);
		m_MeShown = false;
	}

	// Left column: layer switches, legend, scale bar.
	protected void BuildLeft()
	{
		float w = COL_W;
		int i;
		int k;
		float ry;
		int fill;
		string ico;
		string kk;
		INT_PDA_MapSwitch sw;
		m_PlateLayers = MapPlate(m_Overlay, EDGE_X, EDGE_Y, w, 198);
		INT_UIF_Draw.Text(m_PlateLayers, 18, 12, w - 36, 20, INT_PDA_Look.Caps("#STR_INT_PDA_MAP_LAYERS"), 13, INT_PDA_Look.DIM);
		array<string> keys = {"#STR_INT_PDA_MAP_LAYER_MINE", "#STR_INT_PDA_MAP_LAYER_FACTION", "#STR_INT_PDA_MAP_LAYER_MATES", "#STR_INT_PDA_MAP_LAYER_SOS"};
		for (i = 0; i < keys.Count(); i++)
		{
			ry = 42 + i * 36;
			sw = MakeSwitch(m_PlateLayers, 18, ry, w - 36, 36, keys[i]);
			m_Layers.Insert(sw);
		}

		m_PlateLegend = MapPlate(m_Overlay, EDGE_X, EDGE_Y + 212, w, 324);
		INT_UIF_Draw.Text(m_PlateLegend, 18, 12, w - 36, 20, INT_PDA_Look.Caps("#STR_INT_PDA_MAP_LEGEND"), 13, INT_PDA_Look.DIM);
		ry = 42;
		INT_PDA_Look.DrawIcon(m_PlateLegend, 19, ry + 4, 22, "arrow", INT_PDA_Look.ORANGE);
		INT_UIF_Draw.Text(m_PlateLegend, 54, ry, w - 72, 30, "#STR_INT_PDA_MAP_LEG_ME", 15, INT_PDA_Look.TEXT);
		ry = 72;
		INT_UIF_Draw.Image(m_PlateLegend, 22, ry + 7, 16, 16, "iuf_circle32", INT_PDA_Look.ALLY);
		INT_UIF_Draw.Text(m_PlateLegend, 54, ry, w - 72, 30, "#STR_INT_PDA_MAP_LEG_MATE", 15, INT_PDA_Look.TEXT);
		ry = 102;
		INT_UIF_Draw.Image(m_PlateLegend, 19, ry + 4, 22, 22, "iuf_circle32", INT_PDA_Look.ENEMY);
		INT_PDA_Look.DrawIcon(m_PlateLegend, 23, ry + 8, 14, "sos", INT_PDA_Look.WHITE);
		INT_UIF_Draw.Text(m_PlateLegend, 54, ry, w - 72, 30, "#STR_INT_PDA_MAP_LEG_SOS", 15, INT_PDA_Look.TEXT);
		for (k = INT_PDA_Const.MARK_BASE; k <= INT_PDA_Const.MARK_NOTE; k++)
		{
			ry = 132 + k * 30;
			fill = KindColor(k);
			ico = KindIcon(k);
			kk = KindKey(k);
			INT_UIF_Draw.Image(m_PlateLegend, 19, ry + 4, 22, 22, "iuf_circle32", fill);
			INT_PDA_Look.DrawIcon(m_PlateLegend, 23, ry + 8, 14, ico, INT_PDA_Look.BG);
			INT_UIF_Draw.Text(m_PlateLegend, 54, ry, w - 72, 30, kk, 15, INT_PDA_Look.TEXT);
		}

		m_ScaleBox = INT_UIF_Draw.Frame(m_Overlay, EDGE_X, m_H - EDGE_Y - 34, w, 34, "INT_PDA_MapScale");
		INT_UIF_Draw.Texture(m_ScaleBox, 0, 0, w, 34, INT_UIF_WHITE, 0xC8080809);
		INT_UIF_Draw.Hit(m_ScaleBox, 0, 0, w, 34, null);
		m_ScaleTickL = INT_UIF_Draw.Line(m_ScaleBox, 14, 13, 2, 11, INT_PDA_Look.WHITE);
		m_ScaleBar = INT_UIF_Draw.Line(m_ScaleBox, 14, 22, 100, 2, INT_PDA_Look.WHITE);
		m_ScaleTickR = INT_UIF_Draw.Line(m_ScaleBox, 112, 13, 2, 11, INT_PDA_Look.WHITE);
		m_ScaleText = INT_UIF_Draw.Text(m_ScaleBox, 130, 0, w - 140, 34, "", 15, INT_PDA_Look.TEXT);
	}

	// Right column: coordinates, heading, zoom, centre, mates refresh, mark tools, SOS.
	protected void BuildRight()
	{
		float w = COL_W;
		float x = m_W - EDGE_X - w;
		float h = m_H - 2 * EDGE_Y;
		float bw = (w - 46) * 0.5;
		m_PlateRight = MapPlate(m_Overlay, x, EDGE_Y, w, h);
		Widget rp = m_PlateRight;
		INT_UIF_Draw.Text(rp, 18, 12, 160, 20, INT_PDA_Look.Caps("#STR_INT_PDA_MAP_COORDS"), 13, INT_PDA_Look.DIM);
		m_NoGpsChip = INT_UIF_Draw.Frame(rp, w - 18 - 96, 10, 96, 24, "INT_PDA_MapNoGps");
		INT_UIF_Surface.Pill(m_NoGpsChip, 0, 0, 96, 24, 0, INT_PDA_Look.DIM2);
		INT_UIF_Draw.Text(m_NoGpsChip, 0, 0, 96, 24, INT_PDA_Look.Caps("#STR_INT_PDA_MAP_NO_GPS"), 12, INT_PDA_Look.DIM, INT_UIF_Align.CENTER);
		m_CoordX = INT_UIF_Draw.Text(rp, 18, 36, w - 36, 40, "X —", 34, INT_PDA_Look.TEXT);
		m_CoordY = INT_UIF_Draw.Text(rp, 18, 74, w - 36, 40, "Y —", 34, INT_PDA_Look.TEXT);
		m_Grid = INT_UIF_Draw.Text(rp, 18, 116, w - 36, 18, "", 13, INT_PDA_Look.DIM);
		INT_UIF_Draw.Line(rp, 18, 146, w - 36, 1, INT_PDA_Look.LINE);
		INT_UIF_Draw.Image(rp, 18, 158, 54, 54, "iuf_ringline64", INT_PDA_Look.LINE2);
		INT_UIF_Draw.Line(rp, 44, 158, 2, 6, INT_PDA_Look.DIM);
		m_CompassTurn = INT_UIF_Draw.Frame(rp, 27, 167, 36, 36, "INT_PDA_MapCompass");
		INT_PDA_Look.DrawIcon(m_CompassTurn, 5, 5, 26, "arrow", INT_PDA_Look.ORANGE);
		INT_UIF_Draw.Text(rp, 86, 158, w - 104, 18, INT_PDA_Look.Caps("#STR_INT_PDA_MAP_HEADING"), 13, INT_PDA_Look.DIM);
		m_HeadingText = INT_UIF_Draw.Text(rp, 86, 176, w - 104, 36, "—", 30, INT_PDA_Look.TEXT);
		m_BtnIn = MakeBtn(rp, 18, 226, bw, 46, "plus", "", ST_NORMAL, 15, true);
		m_BtnOut = MakeBtn(rp, 28 + bw, 226, bw, 46, "minus", "", ST_NORMAL, 15, true);
		m_BtnCenter = MakeBtn(rp, 18, 282, w - 36, 46, "center", "#STR_INT_PDA_MAP_CENTER", ST_NORMAL, 15, false);
		m_BtnRefresh = MakeBtn(rp, 18, 338, w - 36, 46, "refresh", "#STR_INT_PDA_MAP_REFRESH", ST_NORMAL, 14, false);
		m_MatesAge = INT_UIF_Draw.Text(rp, 18, 388, w - 36, 18, "", 13, INT_PDA_Look.DIM2);
		m_BtnMark = MakeBtn(rp, 18, 414, bw, 44, "pin", "#STR_INT_PDA_MAP_MARK", ST_NORMAL, 13, true);
		m_BtnNext = MakeBtn(rp, 28 + bw, 414, bw, 44, "search", "#STR_INT_PDA_MAP_NEXT", ST_NORMAL, 13, true);
		INT_PDA_Look.MText(rp, 18, 468, w - 36, 60, "#STR_INT_PDA_MAP_HINT", 13, INT_PDA_Look.DIM2);
		m_BtnSos = MakeBtn(rp, 18, h - 78, w - 36, 60, "sos", "#STR_INT_PDA_MAP_SOS", ST_SOS, 26, true);
	}

	// Hairline frame of the map + orange corner brackets.
	protected void BuildFrameLines()
	{
		int edge = INT_PDA_Look.LINE2;
		int o = INT_PDA_Look.ORANGE;
		float k = 22;
		INT_UIF_Draw.Line(m_Overlay, 0, 0, m_W, 1, edge);
		INT_UIF_Draw.Line(m_Overlay, 0, m_H - 1, m_W, 1, edge);
		INT_UIF_Draw.Line(m_Overlay, 0, 0, 1, m_H, edge);
		INT_UIF_Draw.Line(m_Overlay, m_W - 1, 0, 1, m_H, edge);
		INT_UIF_Draw.Line(m_Overlay, 0, 0, k, 2, o);
		INT_UIF_Draw.Line(m_Overlay, 0, 0, 2, k, o);
		INT_UIF_Draw.Line(m_Overlay, m_W - k, 0, k, 2, o);
		INT_UIF_Draw.Line(m_Overlay, m_W - 2, 0, 2, k, o);
		INT_UIF_Draw.Line(m_Overlay, 0, m_H - 2, k, 2, o);
		INT_UIF_Draw.Line(m_Overlay, 0, m_H - k, 2, k, o);
		INT_UIF_Draw.Line(m_Overlay, m_W - k, m_H - 2, k, 2, o);
		INT_UIF_Draw.Line(m_Overlay, m_W - 2, m_H - k, 2, k, o);
	}

	protected void BuildPopups()
	{
		int k;
		int col;
		int row;
		float tx;
		float ty;
		int fill;
		string ico;
		string kk;
		Widget tile;
		ImageWidget under;
		ButtonWidget kh;

		// new mark: kind chooser (the name comes from the menu prompt)
		m_AddPop = INT_UIF_Draw.Frame(m_Overlay, 0, 0, ADD_W, ADD_H, "INT_PDA_MapAdd");
		PopupBase(m_AddPop, ADD_W, ADD_H, INT_PDA_Look.ORANGE);
		INT_UIF_Draw.Text(m_AddPop, 18, 12, ADD_W - 70, 26, INT_PDA_Look.Caps("#STR_INT_PDA_MAP_NEW_MARK"), 16, INT_PDA_Look.TEXT);
		m_BtnAddClose = MakeBtn(m_AddPop, ADD_W - 42, 10, 28, 28, "close", "", ST_NORMAL, 13, true);
		m_AddCoords = INT_UIF_Draw.Text(m_AddPop, 18, 38, ADD_W - 36, 18, "", 13, INT_PDA_Look.DIM);
		for (k = INT_PDA_Const.MARK_BASE; k <= INT_PDA_Const.MARK_NOTE; k++)
		{
			col = k % 3;
			row = k / 3;
			tx = 18 + col * 102;
			ty = 64 + row * 70;
			fill = KindColor(k);
			ico = KindIcon(k);
			kk = KindKey(k);
			tile = INT_UIF_Draw.Frame(m_AddPop, tx, ty, 96, 64, "INT_PDA_MapKind");
			under = INT_UIF_Draw.Texture(tile, 1, 1, 94, 62, INT_UIF_WHITE, 0xFF19191C);
			INT_UIF_Surface.Box(tile, 0, 0, 96, 64, 4, 0, INT_PDA_Look.LINE2);
			INT_UIF_Draw.Image(tile, 35, 7, 26, 26, "iuf_circle32", fill);
			INT_PDA_Look.DrawIcon(tile, 40, 12, 16, ico, INT_PDA_Look.BG);
			INT_UIF_Draw.Text(tile, 2, 38, 92, 20, kk, 12, INT_PDA_Look.TEXT, INT_UIF_Align.CENTER);
			kh = Hit(tile, 0, 0, 96, 64);
			m_KindHits.Insert(kh);
			m_KindUnder.Insert(under);
		}
		m_AddFactionSw = MakeSwitch(m_AddPop, 18, 208, ADD_W - 36, 36, "#STR_INT_PDA_MAP_FOR_FACTION");
		m_AddPersonal = INT_UIF_Draw.Text(m_AddPop, 18, 208, ADD_W - 36, 36, "#STR_INT_PDA_MAP_PERSONAL_NOTE", 13, INT_PDA_Look.DIM2);
		m_AddPop.Show(false);

		// marker card
		m_InfoPop = INT_UIF_Draw.Frame(m_Overlay, 0, 0, INFO_W, INFO_H, "INT_PDA_MapInfo");
		PopupBase(m_InfoPop, INFO_W, INFO_H, 0);
		m_InfoStripe = INT_UIF_Draw.Line(m_InfoPop, 0, 0, 4, INFO_H, INT_PDA_Look.ORANGE);
		m_InfoTitle = INT_UIF_Draw.Text(m_InfoPop, 20, 12, INFO_W - 66, 28, "", 20, INT_PDA_Look.TEXT);
		m_BtnInfoClose = MakeBtn(m_InfoPop, INFO_W - 40, 10, 28, 28, "close", "", ST_NORMAL, 13, true);
		m_InfoSub = INT_UIF_Draw.Text(m_InfoPop, 20, 44, INFO_W - 34, 18, "", 13, INT_PDA_Look.DIM);
		m_InfoSub2 = INT_UIF_Draw.Text(m_InfoPop, 20, 64, INFO_W - 34, 18, "", 13, INT_PDA_Look.DIM);
		m_BtnInfoAct = MakeBtn(m_InfoPop, 20, 94, 164, 42, "trash", "#STR_INT_PDA_MAP_DELETE", ST_DANGER, 14, true);
		m_BtnInfoCenter = MakeBtn(m_InfoPop, 194, 94, 86, 42, "center", "", ST_NORMAL, 14, true);
		m_InfoPop.Show(false);

		// SOS confirmation
		m_AskPop = INT_UIF_Draw.Frame(m_Overlay, (m_W - ASK_W) * 0.5, (m_H - ASK_H) * 0.5, ASK_W, ASK_H, "INT_PDA_MapAsk");
		PopupBase(m_AskPop, ASK_W, ASK_H, INT_PDA_Look.ENEMY);
		INT_PDA_Look.DrawIcon(m_AskPop, 24, 22, 30, "sos", INT_PDA_Look.ENEMY);
		INT_UIF_Draw.Text(m_AskPop, 66, 20, ASK_W - 90, 34, INT_PDA_Look.Caps("#STR_INT_PDA_MAP_SOS_TITLE"), 22, INT_PDA_Look.TEXT);
		INT_PDA_Look.MText(m_AskPop, 24, 66, ASK_W - 48, 60, "#STR_INT_PDA_MAP_SOS_TEXT", 15, INT_PDA_Look.DIM);
		m_BtnAskCancel = MakeBtn(m_AskPop, 24, 140, 178, 46, "", "#STR_INT_PDA_CANCEL", ST_NORMAL, 15, true);
		m_BtnAskSend = MakeBtn(m_AskPop, ASK_W - 24 - 178, 140, 178, 46, "sos", "#STR_INT_PDA_MAP_SOS", ST_SOS, 18, true);
		m_AskPop.Show(false);
	}

	// Floating panel: solid underlay (UIF surfaces alone show the map through — measured on INT_Radiation), hairline
	// box and a hit widget that keeps clicks and the wheel off the map under it.
	protected Widget MapPlate(Widget parent, float x, float y, float w, float h)
	{
		int fill = 0xEB0E0E10;
		Widget f = INT_UIF_Draw.Frame(parent, x, y, w, h, "INT_PDA_MapPlate");
		INT_UIF_Draw.Texture(f, 3, 3, w - 6, h - 6, INT_UIF_WHITE, fill);
		INT_UIF_Surface.Box(f, 0, 0, w, h, 6, fill, INT_PDA_Look.LINE);
		INT_UIF_Draw.Hit(f, 0, 0, w, h, null);
		return f;
	}

	protected void PopupBase(Widget f, float w, float h, int stripe)
	{
		int fill = 0xFA131315;
		INT_UIF_Surface.Shadow(f, -20, -14, w + 40, h + 36, ARGB(150, 0, 0, 0));
		INT_UIF_Draw.Texture(f, 3, 3, w - 6, h - 6, INT_UIF_WHITE, fill);
		INT_UIF_Surface.Box(f, 0, 0, w, h, 6, fill, INT_PDA_Look.LINE2);
		if (stripe != 0)
			INT_UIF_Draw.Line(f, 0, 0, w, 3, stripe);
		INT_UIF_Draw.Hit(f, 0, 0, w, h, null);
	}

	protected INT_PDA_MapBtn MakeBtn(Widget parent, float x, float y, float w, float h, string iconName, string key, int style, float fs, bool centre)
	{
		INT_PDA_MapBtn b = new INT_PDA_MapBtn();
		int fill = 0xFF19191C;
		int hot = 0xFF26262B;
		int ink = INT_PDA_Look.TEXT;
		int edge = 0xFF3C3C42;
		string cap = "";
		float isz = Math.Clamp(h * 0.46, 14, 30);
		float gap = 10;
		float tw = 0;
		float total = 0;
		float cx = 16;
		float sx = 12;
		if (style == ST_DANGER)
		{
			ink = 0xFFEC6A5C;
			edge = 0xFF5A2B27;
		}
		else if (style == ST_SOS)
		{
			fill = INT_PDA_Look.ENEMY;
			hot = 0xFFF26657;
			ink = INT_PDA_Look.WHITE;
			edge = INT_PDA_Look.ENEMY;
		}
		b.m_Fill = fill;
		b.m_FillHot = hot;
		b.m_FillOff = 0xFF141416;
		if (style == ST_SOS)
			b.m_FillOff = 0xFF3A2422;
		b.m_Ink = ink;
		b.m_InkOff = INT_PDA_Look.DIM2;
		b.m_Root = INT_UIF_Draw.Frame(parent, x, y, w, h, "INT_PDA_MapBtn");
		b.m_Under = INT_UIF_Draw.Texture(b.m_Root, 1, 1, w - 2, h - 2, INT_UIF_WHITE, fill);
		INT_UIF_Surface.Box(b.m_Root, 0, 0, w, h, 4, 0, edge);
		if (key != "")
			cap = INT_PDA_Look.Caps(key);
		b.m_Label = cap;
		if (cap != "")
			tw = INT_PDA_Look.TextW(cap, fs);
		if (style == ST_SOS)
		{
			// the text changes (cooldown): centred over the whole button, the icon left of it
			sx = Math.Max(12, w * 0.5 - 100);
			if (iconName != "")
				b.m_Icon = INT_PDA_Look.DrawIcon(b.m_Root, sx, (h - isz) * 0.5, isz, iconName, ink);
			b.m_Text = INT_UIF_Draw.Text(b.m_Root, 0, 0, w, h, cap, fs, ink, INT_UIF_Align.CENTER);
		}
		else
		{
			total = tw;
			if (iconName != "" && cap != "")
				total = isz + gap + tw;
			else if (iconName != "")
				total = isz;
			if (centre)
				cx = (w - total) * 0.5;
			if (iconName != "")
			{
				b.m_Icon = INT_PDA_Look.DrawIcon(b.m_Root, cx, (h - isz) * 0.5, isz, iconName, ink);
				cx = cx + isz + gap;
			}
			if (cap != "")
				b.m_Text = INT_UIF_Draw.Text(b.m_Root, cx, 0, w - cx - 6, h, cap, fs, ink);
		}
		b.m_Hit = Hit(b.m_Root, 0, 0, w, h);
		m_AllBtns.Insert(b);
		return b;
	}

	protected INT_PDA_MapSwitch MakeSwitch(Widget parent, float x, float y, float w, float h, string key)
	{
		INT_PDA_MapSwitch s = new INT_PDA_MapSwitch();
		int off = 0xFF2B2B30;
		float tx = w - 46;
		float ty = (h - 24) * 0.5;
		s.m_Root = INT_UIF_Draw.Frame(parent, x, y, w, h, "INT_PDA_MapSwitch");
		s.m_Text = INT_UIF_Draw.Text(s.m_Root, 0, 0, w - 56, h, key, 16, INT_PDA_Look.TEXT);
		s.m_TrackL = INT_UIF_Draw.Image(s.m_Root, tx, ty, 24, 24, "iuf_circle32", off);
		s.m_TrackR = INT_UIF_Draw.Image(s.m_Root, tx + 22, ty, 24, 24, "iuf_circle32", off);
		s.m_TrackM = INT_UIF_Draw.Image(s.m_Root, tx + 12, ty, 22, 24, "iuf_white", off);
		s.m_KnobX0 = tx + 3;
		s.m_KnobX1 = tx + 25;
		s.m_KnobY = ty + 3;
		s.m_Knob = INT_UIF_Draw.Image(s.m_Root, s.m_KnobX0, s.m_KnobY, 18, 18, "iuf_circle32", 0xFF77767A);
		s.m_Hit = Hit(s.m_Root, 0, 0, w, h);
		return s;
	}

	// Every widget of a subtree gets the sort (the map draws over widgets whose own sort is below 100).
	// SetSort moves the widget to the end of its siblings (measured on the stand): walking GetSibling() while sorting
	// stopped after the first child and left a plate's fill drawn over its text. The children are listed first, then
	// sorted in creation order, which keeps that order.
	protected void SortAll(Widget w, int sort)
	{
		if (!w)
			return;
		w.SetSort(sort);
		array<Widget> kids = new array<Widget>();
		Widget c = w.GetChildren();
		while (c)
		{
			kids.Insert(c);
			c = c.GetSibling();
		}
		for (int i = 0; i < kids.Count(); i++)
		{
			SortAll(kids[i], sort);
		}
	}

	// --- kinds ---------------------------------------------------------------------------------------------------------
	static string KindIcon(int kind)
	{
		if (kind == INT_PDA_Const.MARK_BASE)
			return "base";
		if (kind == INT_PDA_Const.MARK_RALLY)
			return "rally";
		if (kind == INT_PDA_Const.MARK_DANGER)
			return "danger";
		if (kind == INT_PDA_Const.MARK_STASH)
			return "stash";
		if (kind == INT_PDA_Const.MARK_ANOMALY)
			return "anomaly";
		return "note";
	}

	static int KindColor(int kind)
	{
		if (kind == INT_PDA_Const.MARK_BASE)
			return 0xFFFF7A1A;
		if (kind == INT_PDA_Const.MARK_RALLY)
			return 0xFFFFB27A;
		if (kind == INT_PDA_Const.MARK_DANGER)
			return 0xFFEC6A5C;
		if (kind == INT_PDA_Const.MARK_STASH)
			return 0xFFE8E2D6;
		if (kind == INT_PDA_Const.MARK_ANOMALY)
			return 0xFFE8C35A;
		return 0xFFB5B2AB;
	}

	static string KindKey(int kind)
	{
		if (kind == INT_PDA_Const.MARK_BASE)
			return "#STR_INT_PDA_MAP_KIND_BASE";
		if (kind == INT_PDA_Const.MARK_RALLY)
			return "#STR_INT_PDA_MAP_KIND_RALLY";
		if (kind == INT_PDA_Const.MARK_DANGER)
			return "#STR_INT_PDA_MAP_KIND_DANGER";
		if (kind == INT_PDA_Const.MARK_STASH)
			return "#STR_INT_PDA_MAP_KIND_STASH";
		if (kind == INT_PDA_Const.MARK_ANOMALY)
			return "#STR_INT_PDA_MAP_KIND_ANOMALY";
		return "#STR_INT_PDA_MAP_KIND_NOTE";
	}

	// --- show / hide / data ---------------------------------------------------------------------------------------------
	override void OnShow()
	{
		m_ShownT = 0;
		m_DropFrames = 0;
		m_StillT = 0;
		m_Closing = false;
		m_HiddenByPrompt = false;
		if (!m_Built)
		{
			if (m_NoMap)
				m_NoMap.Show(true);
			return;
		}
		float f = INT_UIF_Scale.Factor();
		if (f > 0)
			m_F = f;
		m_MeHalfPx = INT_UIF_Draw.S(ME_BOX * 0.5);
		m_Dirty = false;
		RefreshAccess(true);
		ApplyMapVisible();
		m_InitFrames = 0;
		m_CentreOnMe = false;
		if (m_FirstShow && m_Gps)
			m_CentreOnMe = true;
		m_FirstShow = false;
		m_PlaceFrames = 3;
		m_SlowT = 1;
		if (m_Fac)
			RequestMatesGated(false);
		INT_UIF_Draw.Move(m_PlateLayers, EDGE_X, EDGE_Y);
		INT_UIF_Draw.Move(m_PlateLegend, EDGE_X, EDGE_Y + 212);
		INT_UIF_Draw.Move(m_PlateRight, m_W - EDGE_X - COL_W, EDGE_Y);
		Reveal(m_PlateLayers, 0.04);
		Reveal(m_PlateLegend, 0.1);
		Reveal(m_PlateRight, 0.07);
	}

	override void OnHide()
	{
		SaveView();
		ClosePopups(true);
		m_Drag = DRAG_NONE;
		if (m_Hover >= 0 && m_Hover < m_Used)
			SetPinHot(m_Pins[m_Hover], false);
		m_Hover = -1;
		m_PendingInfo = -1;
	}

	override void OnChanged(int what)
	{
		if (what == INT_PDA_Client.CH_MATES)
			s_MatesAtMs = GetGame().GetTime();
		if (!m_Built)
			return;
		bool data = false;
		bool access = false;
		if (what == INT_PDA_Client.CH_FACTION || what == INT_PDA_Client.CH_MATES || what == INT_PDA_Client.CH_SOS)
			data = true;
		if (what == INT_PDA_Client.CH_PDA || what == INT_PDA_Client.CH_PROFILE || what == INT_PDA_Client.CH_AUTH || what == INT_PDA_Client.CH_WELCOME)
			access = true;
		if (!data && !access)
			return;
		if (!m_Active)
		{
			m_Dirty = true;
			return;
		}
		// CH_PDA also fires on battery steps: the markers are rebuilt only when the access really changed
		RefreshAccess(data);
	}

	override void OnResult(int requestOp, int code)
	{
		if (requestOp == INT_PDA_Const.OP_SOS)
		{
			// a refused SOS gives the cooldown back, an accepted / unheard one keeps it (INT_PDA_SosClock)
			INT_PDA_SosClock.OnServerAnswer(code);
			if (code == INT_PDA_Const.R_OK)
			{
				INT_UIF_Toast.Success("#STR_INT_PDA_MAP_SOS_SENT");
			}
			else if (code == INT_PDA_Const.R_NO_RECIPIENTS)
			{
				INT_PDA_Sfx.Play("Error");
				INT_UIF_Toast.Warning("#STR_INT_PDA_SOS_NOBODY");
			}
			else
			{
				Toast(code);
			}
			if (m_Built)
				UpdateCooldowns();
			return;
		}
		if (requestOp == INT_PDA_Const.OP_MARK_ADD)
		{
			if (code == INT_PDA_Const.R_OK)
				INT_UIF_Toast.Success("#STR_INT_PDA_MAP_FMARK_ADDED");
			else
				Toast(code);
		}
	}

	override bool HandleEscape()
	{
		if (ClosePopups(true))
			return true;
		if (m_Drag != DRAG_NONE)
		{
			m_Drag = DRAG_NONE;
			return true;
		}
		// nothing of ours is open: the menu powers off right after this, and the map would draw over the shutter
		m_Closing = true;
		ApplyMapVisible();
		return false;
	}

	// Module access (spec.md §5): MAP = map; GPS = «Я», centre, coordinates, heading; GPS + GP + faction = faction
	// marks, mates, SOS; leader / deputy (or a config admin) also add / delete faction marks.
	protected void RefreshAccess(bool force)
	{
		if (!m_Built)
			return;
		bool mapMod = HasMod(INT_PDA.SLOT_MAP);
		bool gps = HasMod(INT_PDA.SLOT_GPS);
		bool gp = HasMod(INT_PDA.SLOT_GP);
		bool fac = false;
		if (gps && gp && InFaction())
			fac = true;
		bool manage = false;
		INT_PDA_Profile prof = INT_PDA_Client.s_Profile;
		if (fac && prof)
		{
			if (prof.Role >= INT_PDA_Const.ROLE_DEPUTY || INT_PDA_Client.s_IsAdmin)
				manage = true;
		}
		bool changed = false;
		if (mapMod != m_HasMapMod || gps != m_Gps || fac != m_Fac || manage != m_CanManage)
			changed = true;
		m_HasMapMod = mapMod;
		m_Gps = gps;
		m_Fac = fac;
		m_CanManage = manage;
		m_NoMap.Show(!mapMod);
		m_MapHost.Show(mapMod);
		if (!mapMod)
		{
			ClosePopups(true);
			m_Drag = DRAG_NONE;
			return;
		}
		if (!changed && !force)
			return;
		m_NoGpsChip.Show(!gps);
		m_BtnCenter.SetEnabled(gps);
		INT_PDA_MapPrefs d = INT_PDA_MapStore.Data();
		PaintLayer(L_MINE, d.HideMine == 0, true);
		PaintLayer(L_FACTION, d.HideFaction == 0, fac);
		PaintLayer(L_MATES, d.HideMates == 0, fac);
		PaintLayer(L_SOS, d.HideSos == 0, fac);
		if (!manage)
			m_AddFaction = false;
		RebuildPins();
		UpdateReadouts();
		UpdateCooldowns();
	}

	protected void PaintLayer(int idx, bool on, bool enabled)
	{
		if (idx < 0 || idx >= m_Layers.Count())
			return;
		INT_PDA_MapSwitch sw = m_Layers[idx];
		sw.m_On = on;
		sw.m_Enabled = enabled;
		sw.Paint(false);
	}

	// --- markers -----------------------------------------------------------------------------------------------------------
	// Rebuilt on data changes only; widgets come from the pool (new ones only when the pool is short).
	protected void RebuildPins()
	{
		s_Rebuilds++;
		int i;
		int n;
		// the card «К метке» is about to open names a pool index: find that marker again after the rebuild
		int pendType = -1;
		int pendRef = 0;
		string pendKey = "";
		float pendX = 0;
		float pendZ = 0;
		INT_PDA_MapPin pend;
		if (m_PendingInfo >= 0 && m_PendingInfo < m_Used)
		{
			pend = m_Pins[m_PendingInfo];
			pendType = pend.m_Type;
			pendRef = pend.m_Ref;
			pendKey = pend.m_Key;
			pendX = pend.m_X;
			pendZ = pend.m_Z;
		}
		m_Used = 0;
		m_SosPins = 0;
		int now = GetGame().GetTime();
		INT_PDA_MapPrefs d = INT_PDA_MapStore.Data();
		INT_PDA_FactionView fv = INT_PDA_Client.s_Faction;
		array<ref INT_PDA_Mark> marks;
		array<ref INT_PDA_MatePos> mates;
		array<ref INT_PDA_SosMark> soses;
		INT_PDA_MapOwnMark om;
		// SOS first: a full pool (MAX_PINS) must never swallow a call for help
		if (m_HasMapMod && m_Fac && d.HideSos == 0)
		{
			soses = INT_PDA_Client.s_Sos;
			n = soses.Count();
			for (i = 0; i < n; i++)
				AddSosPin(i, soses[i], now);
		}
		if (m_HasMapMod && d.HideMine == 0)
		{
			n = d.Marks.Count();
			for (i = 0; i < n; i++)
			{
				om = d.Marks[i];
				AddOwnPin(i, om);
			}
		}
		if (m_HasMapMod && m_Fac)
		{
			if (d.HideFaction == 0 && fv)
			{
				marks = fv.Marks;
				n = marks.Count();
				for (i = 0; i < n; i++)
					AddFactionPin(marks[i]);
			}
			if (d.HideMates == 0)
			{
				mates = INT_PDA_Client.s_Mates;
				n = Math.Min(mates.Count(), MAX_MATES);
				for (i = 0; i < n; i++)
					AddMatePin(i, mates[i]);
			}
		}
		for (i = m_Used; i < m_Pins.Count(); i++)
			HidePin(m_Pins[i]);
		StackTags();
		m_Hover = -1;
		m_LastPicked = -1;
		if (m_NextIdx >= m_Used)
			m_NextIdx = -1;
		m_PendingInfo = -1;
		if (pendType >= 0)
			m_PendingInfo = FindPin(pendType, pendRef, pendKey, pendX, pendZ);
		s_PinsUsed = m_Used;
		m_PlaceFrames = 3;
		ValidateInfo();
	}

	// Markers on one spot (a faction mark on a personal one, a mate standing on a mark): every tag goes under the tags
	// of the earlier ones instead of over them. Runs on rebuild only (O(n²) over <= MAX_PINS markers).
	protected void StackTags()
	{
		int i;
		int j;
		float dx;
		float dz;
		float shift;
		float gap = INT_UIF_Draw.S(2);
		float lim = SAME_SPOT_M * SAME_SPOT_M;
		INT_PDA_MapPin p;
		INT_PDA_MapPin q;
		for (i = 0; i < m_Used; i++)
		{
			p = m_Pins[i];
			shift = 0;
			if (p.m_TagW > 0)
			{
				for (j = 0; j < i; j++)
				{
					q = m_Pins[j];
					if (q.m_TagW <= 0)
						continue;
					dx = q.m_X - p.m_X;
					dz = q.m_Z - p.m_Z;
					if (dx * dx + dz * dz > lim)
						continue;
					shift = shift + q.m_TagHPx + gap;
				}
			}
			p.m_StackDyPx = shift;
		}
	}

	// Pool index of a marker by what it shows (own: position, faction: mark id, mate / SOS: key); -1 = gone.
	protected int FindPin(int pinType, int markId, string pinKey, float x, float z)
	{
		int i;
		INT_PDA_MapPin p;
		for (i = 0; i < m_Used; i++)
		{
			p = m_Pins[i];
			if (p.m_Type != pinType)
				continue;
			if (pinType == PIN_OWN && p.m_X == x && p.m_Z == z)
				return i;
			if (pinType == PIN_FACTION && p.m_Ref == markId)
				return i;
			if ((pinType == PIN_MATE || pinType == PIN_SOS) && p.m_Key == pinKey)
				return i;
		}
		return -1;
	}

	protected INT_PDA_MapPin UsePin()
	{
		INT_PDA_MapPin p = null;
		if (m_Used < m_Pins.Count())
			p = m_Pins[m_Used];
		else if (m_Pins.Count() < MAX_PINS)
			p = NewPin();
		if (!p)
			return null;
		m_Used++;
		return p;
	}

	protected INT_PDA_MapPin NewPin()
	{
		float b = PIN_BOX;
		INT_PDA_MapPin p = new INT_PDA_MapPin();
		p.m_Root = INT_UIF_Draw.Frame(m_PinLayer, 0, 0, b, b, "INT_PDA_MapPin");
		p.m_Tag = INT_UIF_Draw.Frame(m_TagLayer, 0, 0, 100, 22, "INT_PDA_MapTag");
		if (!p.m_Root || !p.m_Tag)
			return null;
		p.m_Halo = INT_UIF_Draw.Image(p.m_Root, 0, 0, b, b, "iuf_radial", INT_PDA_Look.WHITE);
		p.m_Pulse = INT_UIF_Draw.Image(p.m_Root, 0, 0, b, b, "iuf_ringline64", INT_PDA_Look.ENEMY);
		p.m_Edge = INT_UIF_Draw.Image(p.m_Root, 0, 0, b, b, "iuf_ringline64", INT_PDA_Look.ORANGE);
		p.m_Back = INT_UIF_Draw.Image(p.m_Root, 0, 0, b, b, "iuf_circle64", 0xE6080809);
		p.m_Fill = INT_UIF_Draw.Image(p.m_Root, 0, 0, b, b, "iuf_circle64", INT_PDA_Look.ORANGE);
		p.m_Icon = INT_PDA_Look.DrawIcon(p.m_Root, 0, 0, 20, "note", INT_PDA_Look.BG);
		p.m_IconName = "note";
		p.m_TagBack = INT_UIF_Draw.Texture(p.m_Tag, 0, 0, 100, 22, INT_UIF_WHITE, 0xD0080809);
		p.m_TagText = INT_UIF_Draw.Text(p.m_Tag, 0, 0, 100, 22, "", 14, INT_PDA_Look.TEXT, INT_UIF_Align.CENTER);
		p.m_TagSub = INT_UIF_Draw.Text(p.m_Tag, 0, 19, 100, 17, "", 12, INT_PDA_Look.WHITE, INT_UIF_Align.CENTER);
		p.m_Halo.Show(false);
		p.m_Pulse.Show(false);
		p.m_Edge.Show(false);
		p.m_Root.Show(false);
		p.m_Tag.Show(false);
		SortAll(p.m_Root, SORT_PINS);
		SortAll(p.m_Tag, SORT_LABELS);
		m_Pins.Insert(p);
		return p;
	}

	protected void HidePin(INT_PDA_MapPin p)
	{
		if (!p)
			return;
		if (p.m_Shown)
			p.m_Root.Show(false);
		if (p.m_TagShown)
			p.m_Tag.Show(false);
		p.m_Shown = false;
		p.m_TagShown = false;
		p.m_OnScreen = false;
		if (p.m_Hot)
			SetPinHot(p, false);
	}

	protected void AddOwnPin(int idx, INT_PDA_MapOwnMark m)
	{
		if (!m)
			return;
		INT_PDA_MapPin p = UsePin();
		if (!p)
			return;
		int fill = KindColor(m.Kind);
		string ico = KindIcon(m.Kind);
		p.m_Type = PIN_OWN;
		p.m_Ref = idx;
		p.m_Key = "";
		p.m_Name = m.Name;
		p.m_Kind = m.Kind;
		p.m_X = m.X;
		p.m_Z = m.Z;
		StylePin(p, 30, fill, ico, INT_PDA_Look.BG, false, m.Name, "", INT_PDA_Look.TEXT, 0xD0080809);
	}

	protected void AddFactionPin(INT_PDA_Mark k)
	{
		if (!k)
			return;
		INT_PDA_MapPin p = UsePin();
		if (!p)
			return;
		int fill = KindColor(k.Kind);
		string ico = KindIcon(k.Kind);
		p.m_Type = PIN_FACTION;
		p.m_Ref = k.Id;
		p.m_Key = "";
		p.m_Name = k.Name;
		p.m_Kind = k.Kind;
		p.m_X = k.X;
		p.m_Z = k.Z;
		StylePin(p, 34, fill, ico, INT_PDA_Look.BG, true, k.Name, "", 0xFFFFB27A, 0xD0080809);
	}

	protected void AddMatePin(int idx, INT_PDA_MatePos mp)
	{
		if (!mp)
			return;
		INT_PDA_MapPin p = UsePin();
		if (!p)
			return;
		p.m_Type = PIN_MATE;
		p.m_Ref = idx;
		p.m_Key = mp.Callsign;
		p.m_Name = mp.Callsign;
		p.m_Kind = -1;
		p.m_X = mp.X;
		p.m_Z = mp.Z;
		StylePin(p, 20, INT_PDA_Look.ALLY, "", INT_PDA_Look.BG, false, mp.Callsign, "", 0xFF9FE6BF, 0xD0080809);
	}

	protected void AddSosPin(int idx, INT_PDA_SosMark s, int now)
	{
		if (!s)
			return;
		string key = s.Callsign + ":" + s.TimeMs.ToString();
		if (s_SosHidden.Find(key) >= 0)
			return;
		int age = now - s.TimeMs;
		if (age > SOS_SHOW_MS)
			return;
		INT_PDA_MapPin p = UsePin();
		if (!p)
			return;
		p.m_Type = PIN_SOS;
		p.m_Ref = idx;
		p.m_Key = key;
		p.m_Name = s.Callsign;
		p.m_Kind = -1;
		p.m_Time = s.TimeMs;
		p.m_X = s.X;
		p.m_Z = s.Z;
		string title = m_TxtSos + " · " + s.Callsign;
		string sub = SosSub(p, now);
		p.m_SubLast = sub;
		StylePin(p, 40, INT_PDA_Look.ENEMY, "sos", INT_PDA_Look.WHITE, false, title, sub, INT_PDA_Look.WHITE, 0xE6C23A2E);
		m_SosPins++;
	}

	protected string SosSub(INT_PDA_MapPin p, int now)
	{
		string age = AgeText(now - p.m_Time);
		if (!m_Gps)
			return age;
		string dist = DistText(p.m_X, p.m_Z);
		return dist + " · " + age;
	}

	// Sizes and colours of one marker (design px). Runs on rebuild only.
	protected void StylePin(INT_PDA_MapPin p, float size, int fill, string iconName, int iconInk, bool edge, string tagText, string tagSub, int tagInk, int tagBack)
	{
		float c = PIN_BOX * 0.5;
		float half = size * 0.5;
		float isz = size * 0.56;
		int halo = INT_UIF_Color.WithAlpha(fill, 150);
		string path;
		float wa;
		float wb;
		float tw;
		float th;
		p.m_Size = size;
		p.m_Color = fill;
		INT_UIF_Draw.Place(p.m_Halo, c - size, c - size, size * 2, size * 2);
		p.m_Halo.SetColor(halo);
		INT_UIF_Draw.Place(p.m_Back, c - half - 3, c - half - 3, size + 6, size + 6);
		INT_UIF_Draw.Place(p.m_Fill, c - half, c - half, size, size);
		p.m_Fill.SetColor(fill);
		if (iconName == "")
		{
			p.m_Icon.Show(false);
		}
		else
		{
			if (iconName != p.m_IconName)
			{
				path = INT_PDA_Look.IconPath(iconName);
				p.m_Icon.LoadImageFile(0, path);
				p.m_Icon.SetImage(0);
				p.m_IconName = iconName;
			}
			INT_UIF_Draw.Place(p.m_Icon, c - isz * 0.5, c - isz * 0.5, isz, isz);
			p.m_Icon.SetColor(iconInk);
			p.m_Icon.Show(true);
		}
		p.m_Edge.Show(edge);
		if (edge)
			INT_UIF_Draw.Place(p.m_Edge, c - half - 6, c - half - 6, size + 12, size + 12);
		p.m_Pulse.Show(p.m_Type == PIN_SOS);
		p.m_HalfPx = INT_UIF_Draw.S(c);
		p.m_HitPx = INT_UIF_Draw.S(half + 6);
		p.m_TagInk = tagInk;
		p.m_TagBackColor = tagBack;
		p.m_Hot = false;
		p.m_Halo.Show(false);
		p.m_Root.SetSort(PinSort(p));
		p.m_Tag.SetSort(TagSort(p));
		p.m_StackDyPx = 0;
		if (tagText == "")
		{
			p.m_TagW = 0;
			if (p.m_TagShown)
				p.m_Tag.Show(false);
			p.m_TagShown = false;
			return;
		}
		// widths come from the per-menu cache (a rebuild re-measures nothing it measured before); a long name is cut
		// with «…» so it never leaves its plate (frames do not clip text)
		float textMax = TAG_MAX_W - 18;
		wa = TagTextW(tagText, 14);
		if (wa > textMax)
		{
			tagText = INT_PDA_Look.Fit(tagText, textMax, 14);
			wa = TagTextW(tagText, 14);
		}
		wb = 0;
		if (tagSub != "")
			wb = TagTextW(tagSub, 12) + 30;
		tw = Math.Max(wa, wb) + 18;
		if (tw < 36)
			tw = 36;
		if (tw > TAG_MAX_W)
			tw = TAG_MAX_W;
		th = 22;
		if (tagSub != "")
			th = 38;
		INT_UIF_Draw.Place(p.m_Tag, 0, 0, tw, th);
		INT_UIF_Draw.Place(p.m_TagBack, 0, 0, tw, th);
		p.m_TagBack.SetColor(tagBack);
		if (tagSub != "")
			INT_UIF_Draw.Place(p.m_TagText, 0, 2, tw, 18);
		else
			INT_UIF_Draw.Place(p.m_TagText, 0, 0, tw, th);
		p.m_TagText.SetText(tagText);
		p.m_TagText.SetColor(tagInk);
		INT_UIF_Draw.Place(p.m_TagSub, 0, 19, tw, 17);
		p.m_TagSub.SetText(tagSub);
		p.m_TagSub.Show(tagSub != "");
		p.m_TagW = tw;
		p.m_TagWPx = INT_UIF_Draw.S(tw);
		p.m_TagHPx = INT_UIF_Draw.S(th);
		p.m_TagHalfPx = p.m_TagWPx * 0.5;
		p.m_TagDyPx = INT_UIF_Draw.S(half + 7);
	}

	// SOS markers lie over the other markers (they are built first, so their widgets are the oldest of the pool).
	protected int PinSort(INT_PDA_MapPin p)
	{
		if (p.m_Type == PIN_SOS)
			return SORT_PINS + 1;
		return SORT_PINS;
	}

	protected int TagSort(INT_PDA_MapPin p)
	{
		if (p.m_Type == PIN_SOS)
			return SORT_LABELS + 1;
		return SORT_LABELS;
	}

	protected float TagTextW(string text, float size)
	{
		string key = size.ToString() + "|" + text;
		float w;
		if (m_TagW.Find(key, w))
			return w;
		w = INT_PDA_Look.TextW(text, size);
		if (m_TagW.Count() >= TAGW_CACHE_MAX)
			m_TagW.Clear();
		m_TagW.Set(key, w);
		return w;
	}

	protected void SetPinHot(INT_PDA_MapPin p, bool hot)
	{
		if (!p || p.m_Hot == hot)
			return;
		p.m_Hot = hot;
		p.m_Halo.Show(hot);
		int ink = p.m_TagInk;
		int back = p.m_TagBackColor;
		int rootSort = PinSort(p);
		int tagSort = TagSort(p);
		if (hot)
		{
			ink = INT_PDA_Look.WHITE;
			if (p.m_Type != PIN_SOS)
				back = 0xF22A2A30;
			rootSort = SORT_PINS + 2;
			tagSort = SORT_LABELS + 2;
		}
		p.m_TagText.SetColor(ink);
		p.m_TagBack.SetColor(back);
		p.m_Root.SetSort(rootSort);
		p.m_Tag.SetSort(tagSort);
	}

	// Moves the existing markers to the current view (runs only for a few frames after the view changed).
	protected void PlacePins()
	{
		s_Placed++;
		float margin = INT_UIF_Draw.S(6);
		int i;
		INT_PDA_MapPin p;
		vector wp;
		vector sp;
		float lx;
		float ly;
		float tx;
		float ty;
		bool on;
		bool tagOn;
		for (i = 0; i < m_Used; i++)
		{
			p = m_Pins[i];
			wp = Vector(p.m_X, 0, p.m_Z);
			sp = m_Map.MapToScreen(wp);
			lx = sp[0] - m_MX;
			ly = sp[1] - m_MY;
			p.m_LX = lx;
			p.m_LY = ly;
			on = lx >= margin && ly >= margin && lx <= m_MW - margin && ly <= m_MH - margin;
			p.m_OnScreen = on;
			if (on != p.m_Shown)
			{
				p.m_Root.Show(on);
				p.m_Shown = on;
			}
			if (on)
				p.m_Root.SetPos(lx - p.m_HalfPx, ly - p.m_HalfPx);
			tx = lx - p.m_TagHalfPx;
			ty = ly + p.m_TagDyPx + p.m_StackDyPx;
			tagOn = false;
			if (on && p.m_TagW > 0 && tx >= 0 && ty >= 0 && tx + p.m_TagWPx <= m_MW && ty + p.m_TagHPx <= m_MH)
				tagOn = true;
			if (tagOn != p.m_TagShown)
			{
				p.m_Tag.Show(tagOn);
				p.m_TagShown = tagOn;
			}
			if (tagOn)
				p.m_Tag.SetPos(tx, ty);
		}
	}

	// SOS markers blink and send rings (a handful of widgets per frame).
	protected void AnimatePins(float dt)
	{
		m_BlinkT = m_BlinkT + dt;
		if (m_BlinkT > 1000)
			m_BlinkT = 0;
		if (m_SosPins == 0)
			return;
		float sn = Math.Sin(m_BlinkT * 4.0);
		float blink = 0.55 + 0.45 * Math.AbsFloat(sn);
		float ph = m_BlinkT / 1.4;
		float fl = Math.Floor(ph);
		ph = ph - fl;
		float s = 40 + 74 * ph;
		float c = PIN_BOX * 0.5;
		float a = 0.8 * (1 - ph);
		int i;
		INT_PDA_MapPin p;
		for (i = 0; i < m_Used; i++)
		{
			p = m_Pins[i];
			if (p.m_Type != PIN_SOS || !p.m_Shown)
				continue;
			p.m_Fill.SetAlpha(blink);
			INT_UIF_Draw.Place(p.m_Pulse, c - s * 0.5, c - s * 0.5, s, s);
			p.m_Pulse.SetAlpha(a);
		}
	}

	// Distance / age lines of the SOS tags (4 times a second); an expired SOS leaves the map.
	protected void UpdateSosTags()
	{
		if (m_SosPins == 0)
			return;
		int now = GetGame().GetTime();
		bool expired = false;
		int i;
		INT_PDA_MapPin p;
		string sub;
		for (i = 0; i < m_Used; i++)
		{
			p = m_Pins[i];
			if (p.m_Type != PIN_SOS)
				continue;
			if (now - p.m_Time > SOS_SHOW_MS)
				expired = true;
			sub = SosSub(p, now);
			if (sub == p.m_SubLast)
				continue;
			p.m_SubLast = sub;
			p.m_TagSub.SetText(sub);
		}
		if (expired)
			RebuildPins();
	}

	protected int PinAt(float lx, float ly)
	{
		int best = -1;
		float bestD = 1000000000;
		int i;
		INT_PDA_MapPin p;
		float dx;
		float dy;
		float d2;
		for (i = 0; i < m_Used; i++)
		{
			p = m_Pins[i];
			if (!p.m_OnScreen)
				continue;
			dx = lx - p.m_LX;
			dy = ly - p.m_LY;
			d2 = dx * dx + dy * dy;
			if (d2 > p.m_HitPx * p.m_HitPx || d2 >= bestD)
				continue;
			best = i;
			bestD = d2;
		}
		return best;
	}

	// The marker a click opens. Several markers on one spot: PinAt alone always gives the first one, so a repeated
	// click on the same spot walks on to the next one.
	protected int PinAtCycle(float lx, float ly)
	{
		int best = PinAt(lx, ly);
		if (best < 0)
			return -1;
		INT_PDA_MapPin b = m_Pins[best];
		array<int> same = new array<int>();
		int i;
		float dx;
		float dy;
		float lim = INT_UIF_Draw.S(3);
		INT_PDA_MapPin p;
		for (i = 0; i < m_Used; i++)
		{
			p = m_Pins[i];
			if (!p.m_OnScreen)
				continue;
			dx = p.m_LX - b.m_LX;
			dy = p.m_LY - b.m_LY;
			if (dx * dx + dy * dy <= lim * lim)
				same.Insert(i);
		}
		int k = same.Find(m_LastPicked);
		if (same.Count() > 1 && k >= 0)
		{
			k = k + 1;
			if (k >= same.Count())
				k = 0;
			best = same[k];
		}
		m_LastPicked = best;
		return best;
	}

	protected void UpdateHover()
	{
		int best = -1;
		if (m_MouseIn)
			best = PinAt(m_MouseX - m_MX, m_MouseY - m_MY);
		if (best == m_Hover)
			return;
		if (m_Hover >= 0 && m_Hover < m_Used)
			SetPinHot(m_Pins[m_Hover], false);
		m_Hover = best;
		if (best >= 0)
			SetPinHot(m_Pins[best], true);
	}

	// --- «Я» ------------------------------------------------------------------------------------------------------------
	protected void PlaceMe(float dt)
	{
		PlayerBase pl = null;
		if (m_Gps)
			pl = PlayerBase.Cast(GetGame().GetPlayer());
		if (!pl)
		{
			if (m_MeShown)
			{
				m_MeRoot.Show(false);
				m_MeShown = false;
			}
			return;
		}
		vector pos = pl.GetPosition();
		vector ypr = pl.GetYawPitchRoll();
		float ang = Math.Round(ypr[0]);
		if (ang < 0)
			ang = ang + 360;
		m_HeadingDeg = ang;
		if (m_CompassTurn)
			m_CompassTurn.SetRotation(0, 0, ang);
		vector sp = m_Map.MapToScreen(pos);
		float lx = sp[0] - m_MX;
		float ly = sp[1] - m_MY;
		float edge = INT_UIF_Draw.S(16);
		bool on = lx >= edge && ly >= edge && lx <= m_MW - edge && ly <= m_MH - edge;
		if (on != m_MeShown)
		{
			m_MeRoot.Show(on);
			m_MeShown = on;
		}
		if (!on)
			return;
		m_MeRoot.SetPos(lx - m_MeHalfPx, ly - m_MeHalfPx);
		m_MeTurn.SetRotation(0, 0, ang);
		m_RingT = m_RingT + dt;
		while (m_RingT >= 1.8)
			m_RingT = m_RingT - 1.8;
		float p1 = m_RingT / 1.8;
		float p2 = p1 + 0.5;
		if (p2 >= 1)
			p2 = p2 - 1;
		SetRing(m_MeRing1, p1);
		SetRing(m_MeRing2, p2);
	}

	protected void SetRing(ImageWidget r, float ph)
	{
		float s = 26 + 86 * ph;
		float c = ME_BOX * 0.5;
		INT_UIF_Draw.Place(r, c - s * 0.5, c - s * 0.5, s, s);
		r.SetAlpha(0.85 * (1 - ph));
	}

	// --- view ------------------------------------------------------------------------------------------------------------
	protected void ReadRect()
	{
		float x;
		float y;
		float w;
		float h;
		m_Map.GetScreenPos(x, y);
		m_Map.GetScreenSize(w, h);
		m_MX = x;
		m_MY = y;
		m_MW = w;
		m_MH = h;
	}

	protected void ReadMouse()
	{
		int mx;
		int my;
		GetMousePos(mx, my);
		m_MouseX = mx;
		m_MouseY = my;
		bool inRect = false;
		if (mx >= m_MX && my >= m_MY && mx < m_MX + m_MW && my < m_MY + m_MH)
			inRect = true;
		bool over = false;
		Widget under;
		if (inRect)
		{
			under = GetWidgetUnderCursor();
			if (!under || under == m_MapHit)
				over = true;
		}
		m_MouseInRect = inRect;
		m_MouseIn = over;
	}

	protected void MeasureMpp()
	{
		vector c = m_Map.GetMapPos();
		vector a = m_Map.MapToScreen(c);
		vector e = Vector(c[0] + 1000, c[1], c[2]);
		vector b = m_Map.MapToScreen(e);
		float dx = b[0] - a[0];
		if (dx > 0.0001)
			m_Mpp = 1000.0 / dx;
	}

	protected vector LocalToWorld(float lx, float ly)
	{
		vector sp = Vector(lx + m_MX, ly + m_MY, 0);
		return m_Map.ScreenToMap(sp);
	}

	protected bool ViewChanged()
	{
		float scale = m_Map.GetScale();
		vector pos = m_Map.GetMapPos();
		if (scale == m_LastScale && pos == m_LastPos && m_MX == m_LastMX && m_MY == m_LastMY && m_MW == m_LastMW && m_MH == m_LastMH)
			return false;
		m_LastScale = scale;
		m_LastPos = pos;
		m_LastMX = m_MX;
		m_LastMY = m_MY;
		m_LastMW = m_MW;
		m_LastMH = m_MH;
		return true;
	}

	protected vector WorldCentre()
	{
		string path = "CfgWorlds " + GetGame().GetWorldName() + " centerPosition";
		vector c = GetGame().ConfigGetVector(path);
		return Vector(c[0], 0, c[1]);
	}

	// First frames after a show (the widget must be laid out): the last view of this session, the player with GPS on
	// the first show of a menu, the world centre otherwise.
	protected void ApplyStartView()
	{
		float scale = SCALE_START;
		vector pos = WorldCentre();
		string worldName = GetGame().GetWorldName();
		if (s_HasView && s_ViewWorld == worldName)
		{
			scale = s_ViewScale;
			pos = s_ViewPos;
		}
		else if (m_Gps)
		{
			scale = SCALE_GPS;
		}
		if (m_CentreOnMe)
		{
			PlayerBase pl = PlayerBase.Cast(GetGame().GetPlayer());
			if (pl)
				pos = pl.GetPosition();
		}
		float sc = Math.Clamp(scale, SCALE_MIN, SCALE_MAX);
		MapScale(sc);
		MapTo(pos);
	}

	// --- our view ---------------------------------------------------------------------------------------------------
	protected void MapTo(vector pos)
	{
		m_Map.SetMapPos(pos);
		RememberView();
	}

	protected void MapScale(float scale)
	{
		m_Map.SetScale(scale);
		RememberView();
	}

	// What the widget holds right after our call (its own clamp included).
	protected void RememberView()
	{
		m_ReqPos = m_Map.GetMapPos();
		m_ReqScale = m_Map.GetScale();
		m_HasReq = true;
	}

	// Once per frame: the widget's view against ours. Only a press of our own (pan / zoom) changes the view.
	protected void DetectDrift()
	{
		if (!m_HasReq)
			return;
		vector pos = m_Map.GetMapPos();
		float scale = m_Map.GetScale();
		float tol = 0.05;
		if (m_Mpp * 0.25 > tol)
			tol = m_Mpp * 0.25;
		float dx = pos[0] - m_ReqPos[0];
		float dz = pos[2] - m_ReqPos[2];
		bool moved = Math.AbsFloat(dx) > tol || Math.AbsFloat(dz) > tol;
		bool zoomed = Math.AbsFloat(scale - m_ReqScale) > m_ReqScale * 0.0001;
		if (!moved && !zoomed)
			return;
		s_Drifts++;
		if (s_Drifts <= 20)
		{
			int lb = GetMouseState(MouseState.LEFT) & MB_PRESSED_MASK;
			int rb = GetMouseState(MouseState.RIGHT) & MB_PRESSED_MASK;
			string line = "map: drift " + s_Drifts.ToString() + " ours " + m_ReqPos.ToString() + " x" + m_ReqScale.ToString() + " widget " + pos.ToString() + " x" + scale.ToString();
			line = line + " mouse " + m_MouseX.ToString() + "," + m_MouseY.ToString() + " in " + m_MouseIn.ToString() + " lmb " + lb.ToString() + " rmb " + rb.ToString() + " drag " + m_Drag.ToString() + " enforce " + s_Enforce.ToString();
			INT_PDA_Log.Dbg(line);
		}
		if (!s_Enforce)
		{
			RememberView();
			return;
		}
		if (zoomed)
			m_Map.SetScale(m_ReqScale);
		m_Map.SetMapPos(m_ReqPos);
		m_PlaceFrames = 3;
	}

	protected void SaveView()
	{
		if (!m_Built || !m_Map || m_InitFrames < 3)
			return;
		s_ViewScale = m_Map.GetScale();
		s_ViewPos = m_Map.GetMapPos();
		s_ViewWorld = GetGame().GetWorldName();
		s_HasView = true;
	}

	protected void ZoomBy(float factor, bool atCursor)
	{
		if (!m_Map || m_MW < 20)
			return;
		float s = m_Map.GetScale();
		if (s <= 0)
			return;
		float ns = Math.Clamp(s * factor, SCALE_MIN, SCALE_MAX);
		if (Math.AbsFloat(ns - s) < 0.0000001)
			return;
		MeasureMpp();
		float lx = m_MW * 0.5;
		float ly = m_MH * 0.5;
		if (atCursor && m_MouseInRect)
		{
			lx = m_MouseX - m_MX;
			ly = m_MouseY - m_MY;
		}
		vector anchor = LocalToWorld(lx, ly);
		vector c = m_Map.GetMapPos();
		vector cs = m_Map.MapToScreen(c);
		float clx = cs[0] - m_MX;
		float cly = cs[1] - m_MY;
		float nm = m_Mpp * ns / s;
		vector nc = c;
		nc[0] = anchor[0] - (lx - clx) * nm;
		nc[2] = anchor[2] + (ly - cly) * nm;
		MapScale(ns);
		MapTo(nc);
		m_FixWorld = anchor;
		m_FixLX = lx;
		m_FixLY = ly;
		m_FixFrames = 2;
		m_PlaceFrames = 3;
	}

	// One correction two frames after a zoom: the anchor is shifted back under the cursor if the map put it elsewhere.
	protected void FixAnchor()
	{
		m_FixFrames--;
		if (m_FixFrames > 0)
			return;
		vector sp = m_Map.MapToScreen(m_FixWorld);
		float ex = sp[0] - m_MX - m_FixLX;
		float ey = sp[1] - m_MY - m_FixLY;
		if (Math.AbsFloat(ex) < 0.75 && Math.AbsFloat(ey) < 0.75)
			return;
		MeasureMpp();
		vector c = m_Map.GetMapPos();
		c[0] = c[0] + ex * m_Mpp;
		c[2] = c[2] - ey * m_Mpp;
		MapTo(c);
		m_PlaceFrames = 3;
	}

	protected void CentreOnMe()
	{
		if (!m_Gps)
		{
			INT_PDA_Sfx.Play("Error");
			INT_UIF_Toast.Warning("#STR_INT_PDA_MAP_NEED_GPS");
			return;
		}
		PlayerBase pl = PlayerBase.Cast(GetGame().GetPlayer());
		if (!pl || !m_Map)
			return;
		vector pos = pl.GetPosition();
		MapTo(pos);
		m_FixFrames = 0;
		m_PlaceFrames = 3;
		ClosePopups(false);
		INT_PDA_Sfx.Play("Click");
	}

	// Player (GPS) or the middle of the view.
	protected vector HerePos()
	{
		vector pos = m_Map.GetMapPos();
		if (m_Gps)
		{
			PlayerBase pl = PlayerBase.Cast(GetGame().GetPlayer());
			if (pl)
				pos = pl.GetPosition();
		}
		return pos;
	}

	protected void UpdateScaleBar()
	{
		if (!m_ScaleText || m_Mpp <= 0)
			return;
		float metres = m_Mpp * 140 * m_F;
		if (metres != metres || metres <= 0)
			return;
		float nice = 1;
		while (nice * 10 <= metres)
			nice = nice * 10;
		if (nice * 5 <= metres)
			nice = nice * 5;
		else if (nice * 2 <= metres)
			nice = nice * 2;
		float bw = nice / (m_Mpp * m_F);
		if (bw < 4)
			bw = 4;
		INT_UIF_Draw.Place(m_ScaleBar, 14, 22, bw, 2);
		INT_UIF_Draw.Move(m_ScaleTickR, 14 + bw - 2, 13);
		INT_UIF_Draw.Move(m_ScaleText, 14 + bw + 14, 0);
		string txt;
		int whole;
		if (nice >= 1000)
		{
			whole = (int)Math.Round(nice / 1000.0);
			txt = whole.ToString() + " " + m_TxtKm;
		}
		else
		{
			whole = (int)Math.Round(nice);
			txt = whole.ToString() + " " + m_TxtM;
		}
		m_ScaleText.SetText(txt);
	}

	// --- frame ------------------------------------------------------------------------------------------------------------
	override void Update(float dt)
	{
		m_ShownT = m_ShownT + dt;
		if (!m_Built || !m_HasMapMod)
		{
			TickNoMap(dt);
			return;
		}
		if (m_Dirty)
		{
			m_Dirty = false;
			RefreshAccess(true);
			if (!m_HasMapMod)
				return;
		}
		float f = INT_UIF_Scale.Factor();
		if (f > 0)
			m_F = f;
		TrackClosing(dt);
		bool prompt = false;
		if (m_Menu && m_Menu.HasPrompt())
			prompt = true;
		if (prompt != m_HiddenByPrompt)
		{
			m_HiddenByPrompt = prompt;
			if (prompt)
			{
				m_Drag = DRAG_NONE;
				ClosePopups(true);
			}
		}
		ApplyMapVisible();
		if (!m_MapVisible)
			return;
		ReadRect();
		if (m_MW < 20 || m_MH < 20)
			return;
		if (m_InitFrames < 3)
		{
			ApplyStartView();
			m_InitFrames++;
			m_PlaceFrames = 3;
			return;
		}
		ReadMouse();
		DetectDrift();
		if (m_FixFrames > 0)
			FixAnchor();
		TickDrag();
		if (ViewChanged())
			m_PlaceFrames = 3;
		if (m_PlaceFrames > 0)
		{
			m_PlaceFrames--;
			MeasureMpp();
			PlacePins();
			UpdateScaleBar();
		}
		PlaceMe(dt);
		AnimatePins(dt);
		int pend = m_PendingInfo;
		if (pend >= 0)
		{
			m_PendingFrames--;
			if (m_PendingFrames <= 0)
			{
				m_PendingInfo = -1;
				if (pend < m_Used && m_Pins[pend].m_OnScreen)
					OpenInfo(pend);
			}
		}
		if (m_Drag == DRAG_NONE)
			UpdateHover();
		m_SlowT = m_SlowT + dt;
		if (m_SlowT >= 0.25)
		{
			m_SlowT = 0;
			UpdateReadouts();
			UpdateCooldowns();
			UpdateSosTags();
		}
	}

	protected void TickNoMap(float dt)
	{
		if (!m_NoMap || !m_Static || !m_NoMap.IsVisible())
			return;
		m_StaticT = m_StaticT + dt;
		if (m_StaticT < 0.07)
			return;
		m_StaticT = 0;
		int ox = Math.RandomInt(0, 240);
		int oy = Math.RandomInt(0, 240);
		INT_UIF_Draw.Move(m_Static, -ox, -oy);
	}

	// The PDA closing (close cross, dead battery) drops the device down: the map goes first, or it would draw over the
	// shutter. A false alarm (the device stops without the menu closing) shows it again.
	protected void TrackClosing(float dt)
	{
		float rx;
		float ry;
		m_MapHost.GetScreenPos(rx, ry);
		if (m_ShownT > 0.6 && ry > m_LastHostY + 0.5)
			m_DropFrames++;
		else
			m_DropFrames = 0;
		if (Math.AbsFloat(ry - m_LastHostY) > 0.01)
			m_StillT = 0;
		else
			m_StillT = m_StillT + dt;
		m_LastHostY = ry;
		if (!m_Closing && m_DropFrames >= 2)
			m_Closing = true;
		else if (m_Closing && m_StillT > 0.6)
			m_Closing = false;
	}

	protected void ApplyMapVisible()
	{
		if (!m_Map)
			return;
		bool want = true;
		if (m_HiddenByPrompt || m_Closing)
			want = false;
		if (want == m_MapVisible)
			return;
		m_MapVisible = want;
		m_Map.Show(want);
		if (want)
			m_PlaceFrames = 3;
	}

	// Coordinates, grid square and heading (GPS only) — 4 times a second.
	protected void UpdateReadouts()
	{
		if (!m_Built)
			return;
		if (!m_Gps)
		{
			m_CoordX.SetText("X —");
			m_CoordY.SetText("Y —");
			m_Grid.SetText("");
			m_HeadingText.SetText("—");
			m_CompassTurn.Show(false);
			return;
		}
		PlayerBase pl = PlayerBase.Cast(GetGame().GetPlayer());
		if (!pl)
			return;
		m_CompassTurn.Show(true);
		vector pos = pl.GetPosition();
		int ix = (int)Math.Round(pos[0]);
		int iz = (int)Math.Round(pos[2]);
		string sx = ix.ToString();
		string sz = iz.ToString();
		m_CoordX.SetText("X " + sx);
		m_CoordY.SetText("Y " + sz);
		int gx = 0;
		int gz = 0;
		World wld = GetGame().GetWorld();
		if (wld)
			wld.GetGridCoords(pos, m_GridSize, gx, gz);
		gx = Math.AbsInt(gx);
		gz = Math.AbsInt(gz);
		string g1 = gx.ToStringLen(3);
		string g2 = gz.ToStringLen(3);
		m_Grid.SetText(m_TxtGrid + " " + g1 + " · " + g2);
		int deg = (int)Math.Round(m_HeadingDeg);
		if (deg >= 360)
			deg = deg - 360;
		string dir = DirOf(m_HeadingDeg);
		string sd = deg.ToString();
		m_HeadingText.SetText(dir + " " + sd + "°");
	}

	// SOS / refresh cooldowns on the buttons and the age of the mates' positions.
	protected void UpdateCooldowns()
	{
		if (!m_Built)
			return;
		int now = GetGame().GetTime();
		// one SOS clock with the faction tab (the server keeps one gap per player)
		int left = INT_PDA_SosClock.LeftSec();
		int since;
		string label = m_TxtSos;
		string num;
		if (left > 0)
		{
			num = left.ToString();
			label = m_TxtSos + " · " + num;
		}
		m_BtnSos.SetLabel(label);
		bool sosOn = false;
		if (m_Fac && left == 0)
			sosOn = true;
		m_BtnSos.SetEnabled(sosOn);
		left = 0;
		since = now - s_LastMatesMs;
		if (since >= 0 && since < INT_PDA_Const.MATES_GAP_MS)
			left = (INT_PDA_Const.MATES_GAP_MS - since + 999) / 1000;
		label = m_TxtRefresh;
		if (left > 0)
		{
			num = left.ToString();
			label = m_TxtRefresh + " · " + num;
		}
		m_BtnRefresh.SetLabel(label);
		bool refOn = false;
		if (m_Fac && left == 0)
			refOn = true;
		m_BtnRefresh.SetEnabled(refOn);
		string age = "";
		if (m_Fac)
		{
			if (s_MatesAtMs < 0)
				age = m_TxtNotUpdated;
			else
				age = m_TxtUpdated + " " + AgeText(now - s_MatesAtMs);
		}
		m_MatesAge.SetText(age);
	}

	// --- texts -------------------------------------------------------------------------------------------------------------
	protected string DirOf(float deg)
	{
		float a = deg;
		if (a != a)
			return "";
		while (a < 0)
			a = a + 360;
		while (a >= 360)
			a = a - 360;
		int idx = (int)Math.Round(a / 45.0);
		if (idx >= 8)
			idx = 0;
		if (idx < 0 || idx >= m_Dirs.Count())
			return "";
		return m_Dirs[idx];
	}

	// "340 м · СВ" / "2.4 км · Ю" from the player to a point.
	protected string DistText(float x, float z)
	{
		PlayerBase pl = PlayerBase.Cast(GetGame().GetPlayer());
		if (!pl)
			return "";
		vector me = pl.GetPosition();
		float dx = x - me[0];
		float dz = z - me[2];
		float dist = Math.Sqrt(dx * dx + dz * dz);
		float ang = Math.Atan2(dx, dz) * Math.RAD2DEG;
		string dir = DirOf(ang);
		string amount;
		int tenths;
		int whole;
		int frac;
		string sw;
		string sf;
		if (dist >= 1000)
		{
			tenths = (int)Math.Round(dist / 100.0);
			whole = tenths / 10;
			frac = tenths % 10;
			sw = whole.ToString();
			sf = frac.ToString();
			amount = sw + "." + sf + " " + m_TxtKm;
		}
		else
		{
			whole = (int)Math.Round(dist);
			sw = whole.ToString();
			amount = sw + " " + m_TxtM;
		}
		return amount + " · " + dir;
	}

	protected string CoordText(float x, float z)
	{
		int ix = (int)Math.Round(x);
		int iz = (int)Math.Round(z);
		string sx = ix.ToString();
		string sz = iz.ToString();
		return "X " + sx + " · Y " + sz;
	}

	protected string AgeText(int ms)
	{
		int secs = ms / 1000;
		if (secs < 0)
			secs = 0;
		string n;
		if (secs < 60)
		{
			n = secs.ToString();
			return n + " " + m_TxtSecAgo;
		}
		int mins = secs / 60;
		n = mins.ToString();
		return n + " " + m_TxtMinAgo;
	}

	protected string FactionAuthor(int id)
	{
		INT_PDA_FactionView fv = INT_PDA_Client.s_Faction;
		if (!fv)
			return "";
		array<ref INT_PDA_Mark> marks = fv.Marks;
		int i;
		for (i = 0; i < marks.Count(); i++)
		{
			if (marks[i] && marks[i].Id == id)
				return marks[i].Author;
		}
		return "";
	}

	// --- mouse -------------------------------------------------------------------------------------------------------------
	override bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		if (!m_Built || w != m_MapHit)
			return false;
		BeginPress(button);
		return true;
	}

	override bool OnMouseWheel(Widget w, int x, int y, int wheel)
	{
		if (!m_Built || w != m_MapHit)
			return false;
		if (!m_MapVisible || m_Drag != DRAG_NONE || wheel == 0)
			return true;
		ReadRect();
		ReadMouse();
		int steps = Math.AbsInt(wheel);
		if (steps > 3)
			steps = 3;
		float factor = Math.Pow(WHEEL_STEP, steps);
		if (wheel > 0)
			factor = 1.0 / factor;
		ClosePopups(false);
		ZoomBy(factor, true);
		return true;
	}

	protected void BeginPress(int button)
	{
		if (!m_Map || !m_MapVisible || m_Drag != DRAG_NONE)
			return;
		SetFocus(null);
		ReadRect();
		if (m_MW < 20)
			return;
		ReadMouse();
		m_DownX = m_MouseX;
		m_DownY = m_MouseY;
		m_Moved = false;
		m_DragButton = button;
		MeasureMpp();
		m_DownCenter = m_Map.GetMapPos();
		m_DownMpp = m_Mpp;
		m_Drag = DRAG_CLICK;
	}

	protected void TickDrag()
	{
		if (m_Drag == DRAG_NONE)
			return;
		bool down = (GetMouseState(m_DragButton) & MB_PRESSED_MASK) != 0;
		int dx = m_MouseX - m_DownX;
		int dy = m_MouseY - m_DownY;
		if (!m_Moved && dx * dx + dy * dy > 16)
		{
			m_Moved = true;
			ClosePopups(false);
		}
		if (!down)
		{
			EndPress();
			return;
		}
		// only the left button pans; a right press that moved is neither a pan nor a click
		if (!m_Moved || m_DragButton != MouseState.LEFT)
			return;
		m_Drag = DRAG_PAN;
		vector c = m_DownCenter;
		c[0] = m_DownCenter[0] - dx * m_DownMpp;
		c[2] = m_DownCenter[2] + dy * m_DownMpp;
		MapTo(c);
		m_PlaceFrames = 3;
	}

	protected void EndPress()
	{
		bool moved = m_Moved;
		int button = m_DragButton;
		m_Drag = DRAG_NONE;
		if (!moved)
			ClickAt(button, m_DownX, m_DownY);
	}

	// A click without a drag. LMB: a marker opens its card, a double click on the empty map adds a mark, a single one
	// closes the cards. RMB: a marker's card or a new mark.
	protected void ClickAt(int button, int sx, int sy)
	{
		float lx = sx - m_MX;
		float ly = sy - m_MY;
		int pin = PinAtCycle(lx, ly);
		int now = GetGame().GetTime();
		int since;
		int ddx;
		int ddy;
		bool dbl = false;
		if (button == MouseState.LEFT)
		{
			since = now - m_LastClickMs;
			ddx = sx - m_LastClickX;
			ddy = sy - m_LastClickY;
			if (since >= 0 && since <= DOUBLE_MS && ddx * ddx + ddy * ddy <= 64)
				dbl = true;
			m_LastClickMs = now;
			m_LastClickX = sx;
			m_LastClickY = sy;
			if (pin >= 0)
			{
				m_LastClickMs = -100000;
				OpenInfo(pin);
				return;
			}
			if (dbl)
			{
				m_LastClickMs = -100000;
				OpenAddAt(lx, ly);
				return;
			}
			ClosePopups(false);
			return;
		}
		if (button == MouseState.RIGHT)
		{
			if (pin >= 0)
				OpenInfo(pin);
			else
				OpenAddAt(lx, ly);
		}
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (!m_Built || !w)
			return false;
		if (w == m_MapHit)
			return true;
		int i;
		if (IsBtn(m_BtnIn, w))
		{
			ClosePopups(false);
			ZoomBy(1.0 / BUTTON_STEP, false);
			INT_PDA_Sfx.Play("Click");
			return true;
		}
		if (IsBtn(m_BtnOut, w))
		{
			ClosePopups(false);
			ZoomBy(BUTTON_STEP, false);
			INT_PDA_Sfx.Play("Click");
			return true;
		}
		if (IsBtn(m_BtnCenter, w))
		{
			CentreOnMe();
			return true;
		}
		if (IsBtn(m_BtnRefresh, w))
		{
			RequestMatesGated(true);
			return true;
		}
		if (IsBtn(m_BtnMark, w))
		{
			MarkHere();
			return true;
		}
		if (IsBtn(m_BtnNext, w))
		{
			NextMark();
			return true;
		}
		if (IsBtn(m_BtnSos, w))
		{
			AskSos();
			return true;
		}
		if (IsBtn(m_BtnAddClose, w) || IsBtn(m_BtnInfoClose, w) || IsBtn(m_BtnAskCancel, w))
		{
			INT_PDA_Sfx.Play("Click");
			ClosePopups(true);
			return true;
		}
		if (IsBtn(m_BtnInfoAct, w))
		{
			InfoAction();
			return true;
		}
		if (IsBtn(m_BtnInfoCenter, w))
		{
			InfoCentre();
			return true;
		}
		if (IsBtn(m_BtnAskSend, w))
		{
			SendSosNow();
			return true;
		}
		for (i = 0; i < m_Layers.Count(); i++)
		{
			if (m_Layers[i] && w == m_Layers[i].m_Hit)
			{
				ToggleLayer(i);
				return true;
			}
		}
		for (i = 0; i < m_KindHits.Count(); i++)
		{
			if (w == m_KindHits[i])
			{
				PickKind(i);
				return true;
			}
		}
		if (m_AddFactionSw && w == m_AddFactionSw.m_Hit)
		{
			m_AddFaction = !m_AddFaction;
			m_AddFactionSw.m_On = m_AddFaction;
			m_AddFactionSw.Paint(true);
			INT_PDA_Sfx.Play("Toggle");
			return true;
		}
		return false;
	}

	protected bool IsBtn(INT_PDA_MapBtn b, Widget w)
	{
		if (!b || !w)
			return false;
		return b.m_Hit == w;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		HoverWidget(w, true);
		return false;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		HoverWidget(w, false);
		return false;
	}

	protected void HoverWidget(Widget w, bool enter)
	{
		if (!m_Built || !w || w == m_MapHit)
			return;
		int i;
		int col = 0xFF19191C;
		INT_PDA_MapBtn b;
		for (i = 0; i < m_AllBtns.Count(); i++)
		{
			b = m_AllBtns[i];
			if (b && b.m_Hit == w)
			{
				b.SetHot(enter);
				return;
			}
		}
		if (enter)
			col = 0xFF26262B;
		for (i = 0; i < m_KindHits.Count(); i++)
		{
			if (m_KindHits[i] == w)
			{
				m_KindUnder[i].SetColor(col);
				return;
			}
		}
	}

	// --- popups --------------------------------------------------------------------------------------------------------------
	protected bool ClosePopups(bool all)
	{
		bool closed = false;
		if (m_AddPop && m_AddPop.IsVisible())
		{
			m_AddPop.Show(false);
			closed = true;
		}
		if (m_InfoPop && m_InfoPop.IsVisible())
		{
			m_InfoPop.Show(false);
			closed = true;
		}
		m_InfoType = -1;
		if (all && m_AskPop && m_AskPop.IsVisible())
		{
			m_AskPop.Show(false);
			closed = true;
		}
		return closed;
	}

	// Near the map-local point (screen px), right of it, or left when the right column is in the way.
	protected void PlacePopup(Widget pop, float pw, float ph, float lx, float ly)
	{
		float f = m_F;
		if (f <= 0)
			f = 1;
		float dx = lx / f;
		float dy = ly / f;
		float px = dx + 24;
		float limit = m_W - EDGE_X - COL_W - 10;
		if (px + pw > limit)
			px = dx - 24 - pw;
		if (px < 8)
			px = 8;
		if (px > m_W - pw - 8)
			px = m_W - pw - 8;
		float py = dy - 30;
		if (py < 8)
			py = 8;
		if (py > m_H - ph - 8)
			py = m_H - ph - 8;
		INT_UIF_Draw.Move(pop, px, py);
	}

	protected void OpenAddAt(float lx, float ly)
	{
		vector wp = LocalToWorld(lx, ly);
		OpenAdd(wp[0], wp[2], lx, ly);
	}

	// Kind chooser of a new mark at a world point; the «для группировки» switch only for leader / deputy.
	protected void OpenAdd(float x, float z, float lx, float ly)
	{
		ClosePopups(true);
		if (!m_Built || !m_HasMapMod)
			return;
		m_AddX = x;
		m_AddZ = z;
		bool canFac = m_CanManage;
		if (!canFac)
			m_AddFaction = false;
		m_AddFactionSw.m_Root.Show(canFac);
		m_AddPersonal.Show(!canFac);
		m_AddFactionSw.m_On = m_AddFaction;
		m_AddFactionSw.Paint(false);
		string coords = CoordText(x, z);
		m_AddCoords.SetText(coords);
		int i;
		for (i = 0; i < m_KindUnder.Count(); i++)
			m_KindUnder[i].SetColor(0xFF19191C);
		PlacePopup(m_AddPop, ADD_W, ADD_H, lx, ly);
		m_AddPop.Show(true);
		INT_PDA_Sfx.Play("Click");
	}

	protected void PickKind(int kind)
	{
		m_AddKind = INT_PDA_Util.ClampI(kind, INT_PDA_Const.MARK_BASE, INT_PDA_Const.MARK_NOTE);
		bool faction = false;
		if (m_AddFaction && m_CanManage)
			faction = true;
		INT_PDA_MapPrefs d = INT_PDA_MapStore.Data();
		if (!faction && d.Marks.Count() >= INT_PDA_MapStore.MAX_OWN)
		{
			INT_PDA_Sfx.Play("Error");
			INT_UIF_Toast.Error("#STR_INT_PDA_MAP_FULL");
			return;
		}
		m_AddForFaction = faction;
		ClosePopups(true);
		string kk = KindKey(m_AddKind);
		string def = Widget.TranslateString(kk);
		INT_PDA_Prompt p = m_Menu.OpenPrompt("#STR_INT_PDA_MAP_NEW_MARK", "#STR_INT_PDA_MAP_F_NAME", "", false, def);
		if (p)
			p.m_OnDone.Insert(OnNameDone);
	}

	protected void OnNameDone(bool ok, string a, string b)
	{
		m_Menu.ClosePrompt();
		if (!ok)
			return;
		string name = INT_PDA_Util.CleanLine(a, INT_PDA_MapStore.NAME_CHARS, INT_PDA_Const.MAX_NAME_BYTES);
		if (name == "")
		{
			string kk = KindKey(m_AddKind);
			name = Widget.TranslateString(kk);
		}
		if (m_AddForFaction)
		{
			if (!m_CanManage)
			{
				Toast(INT_PDA_Const.R_NO_RIGHTS);
				return;
			}
			INT_PDA_Client.SendMarkAdd(m_AddKind, name, m_AddX, m_AddZ);
			INT_PDA_Sfx.Play("Send");
			return;
		}
		AddOwnMark(m_AddKind, name, m_AddX, m_AddZ);
	}

	protected bool AddOwnMark(int kind, string name, float x, float z)
	{
		INT_PDA_MapPrefs d = INT_PDA_MapStore.Data();
		if (d.Marks.Count() >= INT_PDA_MapStore.MAX_OWN)
		{
			INT_PDA_Sfx.Play("Error");
			INT_UIF_Toast.Error("#STR_INT_PDA_MAP_FULL");
			return false;
		}
		INT_PDA_MapOwnMark m = new INT_PDA_MapOwnMark();
		m.Kind = INT_PDA_Util.ClampI(kind, INT_PDA_Const.MARK_BASE, INT_PDA_Const.MARK_NOTE);
		m.Name = INT_PDA_Util.CleanLine(name, INT_PDA_MapStore.NAME_CHARS, INT_PDA_Const.MAX_NAME_BYTES);
		m.X = INT_PDA_Util.ClampF(x, -100000, 100000, 0);
		m.Z = INT_PDA_Util.ClampF(z, -100000, 100000, 0);
		d.Marks.Insert(m);
		if (d.HideMine != 0)
			d.HideMine = 0;
		if (!INT_PDA_MapStore.Save())
			INT_UIF_Toast.Error("#STR_INT_PDA_R_WRITE_FAILED");
		INT_PDA_Sfx.Play("MapPing");
		PaintLayer(L_MINE, true, true);
		RebuildPins();
		return true;
	}

	protected void DeleteOwnMark(int idx, float x, float z)
	{
		INT_PDA_MapPrefs d = INT_PDA_MapStore.Data();
		int n = d.Marks.Count();
		int at = -1;
		int i;
		INT_PDA_MapOwnMark m;
		if (idx >= 0 && idx < n)
		{
			m = d.Marks[idx];
			if (m && m.X == x && m.Z == z)
				at = idx;
		}
		if (at < 0)
		{
			for (i = 0; i < n; i++)
			{
				m = d.Marks[i];
				if (m && m.X == x && m.Z == z)
				{
					at = i;
					break;
				}
			}
		}
		if (at < 0)
			return;
		d.Marks.RemoveOrdered(at);
		INT_PDA_MapStore.Save();
		INT_PDA_Sfx.Play("Click");
		INT_UIF_Toast.Info("#STR_INT_PDA_MAP_MARK_DELETED");
	}

	// Card of a marker: name, kind / owner, distance (GPS) or coordinates, action (delete own / delete faction mark for
	// leader-deputy / hide SOS) and «центр».
	protected void OpenInfo(int idx)
	{
		if (idx < 0 || idx >= m_Used)
			return;
		INT_PDA_MapPin p = m_Pins[idx];
		ClosePopups(false);
		m_InfoType = p.m_Type;
		m_InfoRef = p.m_Ref;
		m_InfoKey = p.m_Key;
		m_InfoX = p.m_X;
		m_InfoZ = p.m_Z;
		int now = GetGame().GetTime();
		string title = p.m_Name;
		string sub = "";
		string sub2 = "";
		string place = "";
		string kindName = "";
		string extra = "";
		string author = "";
		bool canAct = false;
		string actKey = "#STR_INT_PDA_MAP_DELETE";
		string actIcon = "trash";
		string cap = "";
		if (m_Gps)
			place = DistText(p.m_X, p.m_Z);
		else
			place = CoordText(p.m_X, p.m_Z);
		string kk = "";
		if (p.m_Kind >= 0)
		{
			kk = KindKey(p.m_Kind);
			kindName = INT_PDA_Look.Caps(kk);
		}
		if (p.m_Type == PIN_OWN)
		{
			extra = INT_PDA_Look.Caps("#STR_INT_PDA_MAP_PERSONAL");
			sub = kindName + " · " + extra;
			sub2 = place;
			canAct = true;
		}
		else if (p.m_Type == PIN_FACTION)
		{
			extra = INT_PDA_Look.Caps("#STR_INT_PDA_MAP_FACTION_MARK");
			sub = kindName + " · " + extra;
			author = FactionAuthor(p.m_Ref);
			sub2 = place;
			if (author != "")
			{
				extra = Widget.TranslateString("#STR_INT_PDA_MAP_AUTHOR");
				sub2 = extra + ": " + author + " · " + place;
			}
			canAct = m_CanManage;
		}
		else if (p.m_Type == PIN_MATE)
		{
			sub = INT_PDA_Look.Caps("#STR_INT_PDA_MAP_LEG_MATE");
			if (s_MatesAtMs >= 0)
			{
				extra = AgeText(now - s_MatesAtMs);
				sub = sub + " · " + m_TxtUpdated + " " + extra;
			}
			sub2 = place;
		}
		else
		{
			title = m_TxtSos + " · " + p.m_Name;
			sub = AgeText(now - p.m_Time);
			sub2 = place;
			canAct = true;
			actKey = "#STR_INT_PDA_MAP_HIDE";
			actIcon = "eye_off";
		}
		// frames do not clip text: a long name / author line is cut with «…» inside the card
		title = INT_PDA_Look.Fit(title, INFO_W - 66, 20);
		sub = INT_PDA_Look.Fit(sub, INFO_W - 34, 13);
		sub2 = INT_PDA_Look.Fit(sub2, INFO_W - 34, 13);
		m_InfoTitle.SetText(title);
		m_InfoSub.SetText(sub);
		m_InfoSub2.SetText(sub2);
		m_InfoStripe.SetColor(p.m_Color);
		m_BtnInfoAct.m_Root.Show(canAct);
		if (canAct)
		{
			cap = INT_PDA_Look.Caps(actKey);
			m_BtnInfoAct.SetLabel(cap);
			m_BtnInfoAct.SetIconName(actIcon);
			INT_UIF_Draw.Move(m_BtnInfoCenter.m_Root, 194, 94);
		}
		else
		{
			INT_UIF_Draw.Move(m_BtnInfoCenter.m_Root, 20, 94);
		}
		PlacePopup(m_InfoPop, INFO_W, INFO_H, p.m_LX, p.m_LY);
		m_InfoPop.Show(true);
		INT_PDA_Sfx.Play("Click");
	}

	// The card's target vanished with a data change (mark deleted, new mates list): the card closes.
	protected void ValidateInfo()
	{
		if (!m_InfoPop || !m_InfoPop.IsVisible())
			return;
		if (FindPin(m_InfoType, m_InfoRef, m_InfoKey, m_InfoX, m_InfoZ) >= 0)
			return;
		ClosePopups(false);
	}

	protected void InfoAction()
	{
		int what = m_InfoType;
		if (what == PIN_OWN)
		{
			DeleteOwnMark(m_InfoRef, m_InfoX, m_InfoZ);
		}
		else if (what == PIN_FACTION)
		{
			if (!m_CanManage)
			{
				Toast(INT_PDA_Const.R_NO_RIGHTS);
				return;
			}
			INT_PDA_Client.SendMarkDel(m_InfoRef);
			INT_PDA_Sfx.Play("Click");
		}
		else if (what == PIN_SOS)
		{
			if (s_SosHidden.Find(m_InfoKey) < 0)
				s_SosHidden.Insert(m_InfoKey);
			while (s_SosHidden.Count() > 50)
				s_SosHidden.RemoveOrdered(0);
			INT_PDA_Sfx.Play("Click");
		}
		ClosePopups(false);
		RebuildPins();
	}

	protected void InfoCentre()
	{
		vector pos = Vector(m_InfoX, 0, m_InfoZ);
		ClosePopups(false);
		MapTo(pos);
		m_FixFrames = 0;
		m_PlaceFrames = 3;
		INT_PDA_Sfx.Play("Click");
	}

	// «Метка»: the kind chooser at the player (GPS) or at the middle of the view.
	protected void MarkHere()
	{
		vector wp = HerePos();
		vector sp = m_Map.MapToScreen(wp);
		float lx = sp[0] - m_MX;
		float ly = sp[1] - m_MY;
		OpenAdd(wp[0], wp[2], lx, ly);
	}

	// «К метке»: centres the next marker in order and opens its card once the view has moved.
	protected void NextMark()
	{
		if (m_Used == 0)
		{
			INT_PDA_Sfx.Play("Error");
			INT_UIF_Toast.Info("#STR_INT_PDA_MAP_NO_MARKS");
			return;
		}
		m_NextIdx = m_NextIdx + 1;
		if (m_NextIdx >= m_Used || m_NextIdx < 0)
			m_NextIdx = 0;
		INT_PDA_MapPin p = m_Pins[m_NextIdx];
		vector pos = Vector(p.m_X, 0, p.m_Z);
		ClosePopups(false);
		MapTo(pos);
		m_FixFrames = 0;
		m_PlaceFrames = 3;
		m_PendingInfo = m_NextIdx;
		m_PendingFrames = 3;
		INT_PDA_Sfx.Play("Click");
	}

	protected void ToggleLayer(int idx)
	{
		if (idx < 0 || idx >= m_Layers.Count())
			return;
		INT_PDA_MapSwitch sw = m_Layers[idx];
		if (!sw.m_Enabled)
		{
			INT_PDA_Sfx.Play("Error");
			INT_UIF_Toast.Warning("#STR_INT_PDA_MAP_NEED_GP");
			return;
		}
		sw.m_On = !sw.m_On;
		sw.Paint(true);
		int hide = 1;
		if (sw.m_On)
			hide = 0;
		INT_PDA_MapPrefs d = INT_PDA_MapStore.Data();
		if (idx == L_MINE)
			d.HideMine = hide;
		else if (idx == L_FACTION)
			d.HideFaction = hide;
		else if (idx == L_MATES)
			d.HideMates = hide;
		else
			d.HideSos = hide;
		INT_PDA_MapStore.Save();
		INT_PDA_Sfx.Play("Toggle");
		RebuildPins();
	}

	// --- network ---------------------------------------------------------------------------------------------------------------
	protected void RequestMatesGated(bool loud)
	{
		if (!m_Fac)
		{
			if (loud)
			{
				INT_PDA_Sfx.Play("Error");
				INT_UIF_Toast.Warning("#STR_INT_PDA_MAP_NEED_GP");
			}
			return;
		}
		int now = GetGame().GetTime();
		int since = now - s_LastMatesMs;
		if (since >= 0 && since < INT_PDA_Const.MATES_GAP_MS)
		{
			if (loud)
				INT_PDA_Sfx.Play("Error");
			return;
		}
		s_LastMatesMs = now;
		INT_PDA_Client.RequestMates();
		RequestFactionGated();
		if (loud)
			INT_PDA_Sfx.Play("Click");
		UpdateCooldowns();
	}

	protected void RequestFactionGated()
	{
		int now = GetGame().GetTime();
		int since = now - s_LastFactionMs;
		if (since >= 0 && since < FACTION_GAP_MS)
			return;
		s_LastFactionMs = now;
		INT_PDA_Client.RequestFaction();
	}

	protected void AskSos()
	{
		if (!m_Fac)
		{
			INT_PDA_Sfx.Play("Error");
			INT_UIF_Toast.Warning("#STR_INT_PDA_MAP_NEED_GP");
			return;
		}
		if (INT_PDA_SosClock.LeftMs() > 0)
		{
			INT_PDA_Sfx.Play("Error");
			return;
		}
		ClosePopups(true);
		m_AskPop.Show(true);
		INT_PDA_Sfx.Play("Click");
	}

	protected void SendSosNow()
	{
		ClosePopups(true);
		if (!m_Fac)
			return;
		if (INT_PDA_SosClock.LeftMs() > 0)
			return;
		INT_PDA_SosClock.Start();
		INT_PDA_Client.SendSos();
		INT_PDA_Sfx.Play("Send");
		UpdateCooldowns();
	}

	// --- self-test ---------------------------------------------------------------------------------------------------------------
	// map state | zoom <f> | in | out | center | goto <x> <z> | mark [name] | kind <0-5> | fmark [name] | del <i> | clear |
	// layers | layer <mine|faction|mates|sos> | mates | sos [send] | add | next | close | enforce <0|1> (DetectDrift)
	override void OnTestCommand(string cmd)
	{
		if (cmd.IndexOf("map") != 0)
			return;
		array<string> parts = new array<string>();
		cmd.Split(" ", parts);
		int n = parts.Count();
		if (n < 2 || parts[0] != "map")
			return;
		string verb = parts[1];
		string rest = "";
		string word;
		int i;
		int num;
		float fx;
		float fz;
		vector pos;
		INT_PDA_MapPrefs d;
		INT_PDA_MapOwnMark om;
		for (i = 2; i < n; i++)
		{
			word = parts[i];
			if (rest != "")
				rest = rest + " ";
			rest = rest + word;
		}
		if (verb == "state")
		{
			LogState();
			return;
		}
		if (verb == "enforce" && n >= 3)
		{
			s_Enforce = parts[2] != "0";
			s_Drifts = 0;
			LogState();
			return;
		}
		if (!m_Built || !m_Map)
		{
			INT_PDA_Log.Info("map: карта не построена");
			return;
		}
		if (verb == "zoom" && n >= 3)
		{
			fx = parts[2].ToFloat();
			fz = Math.Clamp(fx, SCALE_MIN, SCALE_MAX);
			MapScale(fz);
			m_PlaceFrames = 3;
		}
		else if (verb == "in")
		{
			ZoomBy(1.0 / BUTTON_STEP, false);
		}
		else if (verb == "out")
		{
			ZoomBy(BUTTON_STEP, false);
		}
		else if (verb == "center")
		{
			if (m_Gps)
			{
				CentreOnMe();
			}
			else
			{
				pos = WorldCentre();
				MapTo(pos);
				m_PlaceFrames = 3;
			}
		}
		else if (verb == "goto" && n >= 4)
		{
			fx = parts[2].ToFloat();
			fz = parts[3].ToFloat();
			pos = Vector(fx, 0, fz);
			MapTo(pos);
			m_PlaceFrames = 3;
		}
		else if (verb == "mark")
		{
			if (rest == "")
				rest = "Test";
			pos = HerePos();
			AddOwnMark(INT_PDA_Const.MARK_NOTE, rest, pos[0], pos[2]);
		}
		else if (verb == "kind" && n >= 3)
		{
			num = parts[2].ToInt();
			num = INT_PDA_Util.ClampI(num, INT_PDA_Const.MARK_BASE, INT_PDA_Const.MARK_NOTE);
			word = KindKey(num);
			rest = Widget.TranslateString(word);
			pos = HerePos();
			AddOwnMark(num, rest, pos[0], pos[2]);
		}
		else if (verb == "fmark")
		{
			if (!m_CanManage)
			{
				INT_PDA_Log.Info("map: метки группировки — только лидер или зам с GPS и ГП");
			}
			else
			{
				if (rest == "")
					rest = "Test";
				pos = HerePos();
				INT_PDA_Client.SendMarkAdd(INT_PDA_Const.MARK_RALLY, rest, pos[0], pos[2]);
			}
		}
		else if (verb == "del" && n >= 3)
		{
			d = INT_PDA_MapStore.Data();
			num = parts[2].ToInt();
			if (num >= 0 && num < d.Marks.Count())
			{
				om = d.Marks[num];
				if (om)
				{
					DeleteOwnMark(num, om.X, om.Z);
					RebuildPins();
				}
			}
		}
		else if (verb == "clear")
		{
			d = INT_PDA_MapStore.Data();
			d.Marks.Clear();
			INT_PDA_MapStore.Save();
			ClosePopups(true);
			RebuildPins();
		}
		else if (verb == "layers")
		{
			LogLayers();
		}
		else if (verb == "layer" && n >= 3)
		{
			word = parts[2];
			num = -1;
			if (word == "mine")
				num = L_MINE;
			else if (word == "faction")
				num = L_FACTION;
			else if (word == "mates")
				num = L_MATES;
			else if (word == "sos")
				num = L_SOS;
			if (num >= 0)
				ToggleLayer(num);
			LogLayers();
		}
		else if (verb == "mates")
		{
			RequestMatesGated(true);
		}
		else if (verb == "sos")
		{
			if (rest == "send")
				SendSosNow();
			else
				AskSos();
		}
		else if (verb == "add")
		{
			pos = m_Map.GetMapPos();
			OpenAdd(pos[0], pos[2], m_MW * 0.5, m_MH * 0.5);
		}
		else if (verb == "next")
		{
			NextMark();
		}
		else if (verb == "close")
		{
			ClosePopups(true);
		}
		LogState();
	}

	protected void LogState()
	{
		INT_PDA_MapPrefs d = INT_PDA_MapStore.Data();
		int own = d.Marks.Count();
		string line = "map: built " + m_Built.ToString() + ", mod " + m_HasMapMod.ToString() + ", gps " + m_Gps.ToString() + ", faction " + m_Fac.ToString() + ", manage " + m_CanManage.ToString();
		INT_PDA_Log.Info(line);
		float sc = 0;
		vector mp = "0 0 0";
		if (m_Map)
		{
			sc = m_Map.GetScale();
			mp = m_Map.GetMapPos();
		}
		string scText = sc.ToString();
		string posText = mp.ToString();
		line = "map: scale " + scText + ", centre " + posText + ", mpp " + m_Mpp.ToString() + ", visible " + m_MapVisible.ToString() + ", drifts " + s_Drifts.ToString() + ", enforce " + s_Enforce.ToString();
		INT_PDA_Log.Info(line);
		int pool = m_Pins.Count();
		line = "map: pins " + m_Used.ToString() + "/" + pool.ToString() + ", sos " + m_SosPins.ToString() + ", own " + own.ToString() + ", rebuilds " + s_Rebuilds.ToString() + ", saves " + INT_PDA_MapStore.s_Saves.ToString();
		INT_PDA_Log.Info(line);
		string key = INT_PDA_MapStore.FileKey();
		string path = INT_PDA_MapStore.PathOf(key);
		INT_PDA_Log.Info("map: file " + path);
	}

	protected void LogLayers()
	{
		INT_PDA_MapPrefs d = INT_PDA_MapStore.Data();
		string line = "map: layers mine " + d.HideMine.ToString() + ", faction " + d.HideFaction.ToString() + ", mates " + d.HideMates.ToString() + ", sos " + d.HideSos.ToString() + " (1 = hidden), gp+faction " + m_Fac.ToString();
		INT_PDA_Log.Info(line);
	}
}
