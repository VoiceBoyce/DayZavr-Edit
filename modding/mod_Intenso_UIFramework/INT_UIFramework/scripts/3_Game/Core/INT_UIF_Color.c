// INT_UIFramework - ARGB colour maths. ARGB() itself is vanilla (proto.c:322).
class INT_UIF_Color
{
	static int A(int c)
	{
		return (c >> 24) & 0xFF;
	}

	static int R(int c)
	{
		return (c >> 16) & 0xFF;
	}

	static int G(int c)
	{
		return (c >> 8) & 0xFF;
	}

	static int B(int c)
	{
		return c & 0xFF;
	}

	static int WithAlpha(int c, int alpha)
	{
		return ARGB(alpha, R(c), G(c), B(c));
	}

	static int LerpChannel(int a, int b, float t)
	{
		float v = a + (b - a) * t;
		int res = (int)Math.Round(v);
		if (res < 0)
			res = 0;
		if (res > 255)
			res = 255;
		return res;
	}

	// Linear blend c1 -> c2, t in [0..1], all four channels.
	static int Lerp(int c1, int c2, float t)
	{
		t = Math.Clamp(t, 0, 1);
		int a = LerpChannel(A(c1), A(c2), t);
		int r = LerpChannel(R(c1), R(c2), t);
		int g = LerpChannel(G(c1), G(c2), t);
		int b = LerpChannel(B(c1), B(c2), t);
		return ARGB(a, r, g, b);
	}

	// f > 0 lightens toward white, f < 0 darkens toward black; alpha kept.
	static int Shade(int c, float f)
	{
		int target = ARGB(A(c), 0, 0, 0);
		if (f > 0)
			target = ARGB(A(c), 255, 255, 255);
		return Lerp(c, target, Math.AbsFloat(f));
	}

	// "#RRGGBB" or "#AARRGGBB" (the '#' is optional). Invalid input -> magenta.
	static int Hex(string hex)
	{
		string h = hex + "";
		h.ToUpper();
		if (h.Length() > 0 && h.Get(0) == "#")
			h = h.Substring(1, h.Length() - 1);

		if (h.Length() == 6)
			h = "FF" + h;
		if (h.Length() != 8)
			return ARGB(255, 255, 0, 255);

		string digits = "0123456789ABCDEF";
		int vals[4];
		for (int i = 0; i < 4; i++)
		{
			int hi = digits.IndexOf(h.Get(i * 2));
			int lo = digits.IndexOf(h.Get(i * 2 + 1));
			if (hi < 0 || lo < 0)
				return ARGB(255, 255, 0, 255);
			vals[i] = hi * 16 + lo;
		}
		return ARGB(vals[0], vals[1], vals[2], vals[3]);
	}
}
