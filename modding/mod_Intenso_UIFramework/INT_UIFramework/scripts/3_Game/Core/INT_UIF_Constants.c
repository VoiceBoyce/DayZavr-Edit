// ============================================================================
// INT_UIFramework - constants shared by every part of the framework.
// 3_Game: widgets (1_core protos) and UIScriptedMenu (3_game/tools) are both
// available here, so consumer mods can use the framework from 4_World and up.
// ============================================================================

const string INT_UIF_VERSION = "2.0.0";

const string INT_UIF_LAYOUTS = "INT_UIFramework/gui/layouts/";
const string INT_UIF_IMAGESET_FILE = "INT_UIFramework/gui/imagesets/iuf_ui.imageset";
const string INT_UIF_STYLES_FILE = "INT_UIFramework/gui/styles/iuf.styles";
const string INT_UIF_WHITE = "INT_UIFramework/gui/textures/white.paa";
// Imageset reference prefix: "set:<Name> image:<cell>" (vanilla icon.c:1535).
const string INT_UIF_SET = "set:iuf_ui image:";
const string INT_UIF_RING_TEX = "INT_UIFramework/gui/textures/iuf_ring_ca.paa";
const string INT_UIF_RING_MASK = "INT_UIFramework/gui/textures/iuf_ring_mask_ca.paa";
// 256 px circle outline, 3 px stroke: crisp hairline circles at 100-250 px.
const string INT_UIF_RING_THIN = "INT_UIFramework/gui/textures/iuf_ringthin_ca.paa";

// Menu IDs reserved by the framework. Vanilla uses 1..46 (constants.c:170-215).
const int INT_UIF_MENU_DIALOG = 9110001;
const int INT_UIF_MENU_DEMO = 9110002;

// Z-order of top-level overlays (higher draws later = on top).
const int INT_UIF_SORT_POPUP = 15000;
const int INT_UIF_SORT_TOAST = 20000;
const int INT_UIF_SORT_TOOLTIP = 30000;

// Dialog result when closed without pressing a button (ESC).
const int INT_UIF_DIALOG_CANCEL = -1;

// Colour roles of interactive components (v2 look):
// PRIMARY solid amber, ACCENT solid teal, SECONDARY neutral tile, GHOST bare;
// SUCCESS / DANGER / WARNING / INFO are "soft": tinted fill + border + text.
enum INT_UIF_Variant
{
	PRIMARY,
	SECONDARY,
	SUCCESS,
	DANGER,
	WARNING,
	GHOST,
	ACCENT,
	INFO
}

// Horizontal text alignment of INT_UIF_Draw.Text.
enum INT_UIF_Align
{
	LEFT,
	CENTER,
	RIGHT
}

enum INT_UIF_ToastKind
{
	INFO,
	SUCCESS,
	WARNING,
	ERROR
}

// Typography roles for INT_UIF_Widgets.Label.
enum INT_UIF_TextRole
{
	TITLE,
	SUBTITLE,
	BODY,
	MUTED,
	CAPTION
}
