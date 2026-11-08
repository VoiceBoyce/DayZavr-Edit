// INT GlobalChat — 30 nick colours (spec.md §5.8, owner brief p.10): no red, crimson, burgundy or pink-red.
// Hue 29..270 degrees or neutral, checked by script 2026-10-20 (design/concepts_v1/src/common.js). The index
// travels in every message; receivers clamp it, so an unknown index never paints a forbidden colour.

class INT_GC_Palette
{
	protected static ref array<int> s_Colors;
	protected static ref array<string> s_Keys;

	protected static void Add(int r, int g, int b, string key)
	{
		s_Colors.Insert(ARGB(255, r, g, b));
		s_Keys.Insert(key);
	}

	protected static void Ensure()
	{
		if (s_Colors)
			return;
		s_Colors = new array<int>();
		s_Keys = new array<string>();
		Add(244, 246, 248, "#STR_INT_GC_COL_WHITE");
		Add(195, 202, 212, "#STR_INT_GC_COL_SILVER");
		Add(226, 210, 174, "#STR_INT_GC_COL_SAND");
		Add(242, 232, 201, "#STR_INT_GC_COL_IVORY");
		Add(232, 192, 122, "#STR_INT_GC_COL_HONEY");
		Add(255, 213, 74, "#STR_INT_GC_COL_GOLD");
		Add(255, 181, 71, "#STR_INT_GC_COL_AMBER");
		Add(255, 159, 67, "#STR_INT_GC_COL_TANGERINE");
		Add(184, 194, 90, "#STR_INT_GC_COL_OLIVE");
		Add(198, 240, 77, "#STR_INT_GC_COL_LIME");
		Add(155, 225, 93, "#STR_INT_GC_COL_SALAD");
		Add(91, 214, 107, "#STR_INT_GC_COL_GREEN");
		Add(46, 204, 143, "#STR_INT_GC_COL_EMERALD");
		Add(127, 232, 192, "#STR_INT_GC_COL_MINT");
		Add(79, 168, 106, "#STR_INT_GC_COL_PINE");
		Add(46, 196, 182, "#STR_INT_GC_COL_TURQUOISE");
		Add(79, 227, 232, "#STR_INT_GC_COL_AQUA");
		Add(63, 208, 255, "#STR_INT_GC_COL_CYAN");
		Add(110, 198, 255, "#STR_INT_GC_COL_SKY");
		Add(169, 221, 245, "#STR_INT_GC_COL_ICE");
		Add(77, 163, 255, "#STR_INT_GC_COL_AZURE");
		Add(91, 124, 255, "#STR_INT_GC_COL_BLUE");
		Add(127, 156, 245, "#STR_INT_GC_COL_CORNFLOWER");
		Add(143, 169, 201, "#STR_INT_GC_COL_STEEL");
		Add(140, 124, 255, "#STR_INT_GC_COL_INDIGO");
		Add(167, 139, 250, "#STR_INT_GC_COL_VIOLET");
		Add(195, 177, 255, "#STR_INT_GC_COL_LAVENDER");
		Add(179, 107, 255, "#STR_INT_GC_COL_AMETHYST");
		Add(205, 164, 245, "#STR_INT_GC_COL_LILAC");
		Add(154, 160, 168, "#STR_INT_GC_COL_GRAPHITE");
	}

	static int Count()
	{
		Ensure();
		return s_Colors.Count();
	}

	static int Clamp(int idx)
	{
		Ensure();
		if (idx < 0 || idx >= s_Colors.Count())
			return INT_GC_Const.DEFAULT_COLOR;
		return idx;
	}

	static int Color(int idx)
	{
		Ensure();
		int i = Clamp(idx);
		return s_Colors.Get(i);
	}

	static string NameKey(int idx)
	{
		Ensure();
		int i = Clamp(idx);
		return s_Keys.Get(i);
	}
}
