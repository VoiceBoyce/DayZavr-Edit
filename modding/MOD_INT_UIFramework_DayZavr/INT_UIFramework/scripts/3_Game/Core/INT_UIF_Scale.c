// INT_UIFramework - resolution scaling.
// Layouts are proportional by default; anything that must be a fixed number of
// pixels (row heights, knobs, toasts) is authored at 1080p and scaled here by
// the screen height, so it keeps the same RELATIVE size at 1440p/4K/ultrawide.
class INT_UIF_Scale
{
	static float Factor()
	{
		int w;
		int h;
		GetScreenSize(w, h);
		if (h <= 0 || w <= 0)
			return 1.0;
		// Designs are authored on a 1920x1080 canvas: scale by height, but never
		// past the width (4:3 / 5:4 screens would otherwise cut windows off).
		float fh = h / 1080.0;
		float fw = w / 1920.0;
		if (fw < fh)
			return fw;
		return fh;
	}

	// Pixel size at the current resolution for a size authored at 1080p.
	static float Px(float px1080)
	{
		return Math.Round(px1080 * Factor());
	}

	// Glyph size for SetTextExactSize, for a size authored at 1080p.
	// The engine already scales "exact text" by screen height / 1080 on its own
	// (measured 2026-10-15: at 2560x1440 boxes grew 1.33x, text built with Px()
	// grew ~1.75x and overflowed). So only the part of Factor() beyond the height
	// ratio is applied: 1.0 on 16:9 and ultrawide, < 1 on 4:3 / 5:4.
	static int TextPx(float px1080)
	{
		int w;
		int h;
		GetScreenSize(w, h);
		if (h <= 0)
			return Math.Round(px1080);
		float f = Factor() * 1080.0 / h;
		return Math.Round(px1080 * f);
	}

	static int ScreenW()
	{
		int w;
		int h;
		GetScreenSize(w, h);
		return w;
	}

	static int ScreenH()
	{
		int w;
		int h;
		GetScreenSize(w, h);
		return h;
	}
}
