// INT GlobalChat — the «Латунь» look (owner D1, design/concepts_v1/2_b_brass.png): warm dark glass, brass #D8AE62,
// sand text, brass corner brackets, round buttons, palette «coins». Colours from INT_GC_Style win when not 0
// (public API). Own atlas: imageset int_gc_ui (white masks, tinted here; tools/gen_gc_art.py).

class INT_GC_Look
{
	static const string IMAGESET = "INT_GlobalChat_CL/gui/imagesets/int_gc_ui.imageset";
	static const string SET = "set:int_gc_ui image:";

	protected static bool s_Loaded;

	static void EnsureLoaded()
	{
		if (s_Loaded)
			return;
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;
		s_Loaded = true;
		INT_UIF_Assets.EnsureLoaded();
		LoadWidgetImageSet(IMAGESET);
	}

	static int Pick(int styleColor, int fallback)
	{
		if (styleColor != 0)
			return styleColor;
		return fallback;
	}

	// --- palette (fallbacks of INT_GC_Style colours) ----------------------------------------------------------
	static int Brass()
	{
		return Pick(INT_GC_UI.Style().AccentColor, ARGB(255, 216, 174, 98));
	}

	static int BrassA(int alpha)
	{
		int c = Brass();
		return INT_UIF_Color.WithAlpha(c, alpha);
	}

	static int Panel()
	{
		int base = Pick(INT_GC_UI.Style().PanelColor, ARGB(236, 22, 18, 12));
		float op = INT_GC_UI.Style().Opacity;
		int a = Math.Round(INT_UIF_Color.A(base) * op);
		return INT_UIF_Color.WithAlpha(base, a);
	}

	static int HeaderBand()
	{
		return Pick(INT_GC_UI.Style().HeaderColor, ARGB(40, 216, 174, 98));
	}

	static int Border()
	{
		return Pick(INT_GC_UI.Style().BorderColor, ARGB(110, 216, 174, 98));
	}

	static int Text()
	{
		return Pick(INT_GC_UI.Style().TextColor, ARGB(255, 239, 230, 214));
	}

	static int Muted()
	{
		return Pick(INT_GC_UI.Style().MutedColor, ARGB(255, 170, 152, 122));
	}

	static int Time()
	{
		return Pick(INT_GC_UI.Style().TimeColor, ARGB(255, 140, 124, 98));
	}

	static int SystemText()
	{
		return Pick(INT_GC_UI.Style().SystemColor, ARGB(255, 238, 220, 184));
	}

	static int Mod()
	{
		return Pick(INT_GC_UI.Style().ModColor, ARGB(255, 255, 160, 72));
	}

	static int Local()
	{
		return Pick(INT_GC_UI.Style().LocalColor, ARGB(255, 156, 204, 142));
	}

	static int Pm()
	{
		return Pick(INT_GC_UI.Style().PmColor, ARGB(255, 205, 164, 245));
	}

	static int Mention()
	{
		return Pick(INT_GC_UI.Style().MentionColor, ARGB(255, 216, 174, 98));
	}

	static int Admin()
	{
		return Pick(INT_GC_UI.Style().AdminColor, ARGB(255, 216, 174, 98));
	}

	static int Notice(int level)
	{
		int c = INT_GC_UI.Style().NoticeColor;
		if (c != 0)
			return c;
		if (level >= 2)
			return ARGB(255, 255, 128, 96);
		if (level == 1)
			return ARGB(255, 255, 196, 102);
		return ARGB(255, 226, 210, 174);
	}

	static int InputFill()
	{
		return Pick(INT_GC_UI.Style().InputColor, ARGB(150, 0, 0, 0));
	}

	static int Dark()
	{
		return ARGB(255, 35, 25, 12);
	}

	// --- drawing helpers --------------------------------------------------------------------------------------
	// Atlas cell of int_gc_ui (or a full "set:..." path from the public API), tinted.
	static ImageWidget Cell(Widget parent, float x, float y, float w, float h, string cellOrPath, int color)
	{
		string path = cellOrPath;
		if (path.IndexOf("set:") != 0)
			path = SET + cellOrPath;
		return INT_UIF_Draw.Texture(parent, x, y, w, h, path, color);
	}

	// Brass corner brackets on the top-left and bottom-right of a rect (design px, inside parent).
	static void Corners(Widget parent, float w, float h, float size, int color)
	{
		Cell(parent, -1, -1, size, size, "gc_corner", color);
		Cell(parent, w - size + 1, h - size + 1, size, size, "gc_corner_br", color);
	}

	// Round button: soft disc + brass ring + icon. Returns the frame (the caller adds a Hit on top); the icon
	// widget comes back through iconOut.
	static Widget RoundButton(Widget parent, float x, float y, float size, string icon, int iconColor, string name, out ImageWidget iconOut)
	{
		iconOut = null;
		Widget f = INT_UIF_Draw.Frame(parent, x, y, size, size, name);
		if (!f)
			return null;
		int discCol = BrassA(26);
		int ringCol = BrassA(120);
		INT_UIF_Draw.Image(f, 0, 0, size, size, "iuf_circle64", discCol);
		INT_UIF_Draw.Image(f, 0, 0, size, size, "iuf_ringline64", ringCol);
		float isz = size * 0.5;
		float io = (size - isz) * 0.5;
		iconOut = Cell(f, io, io, isz, isz, icon, iconColor);
		return f;
	}

	// Engine text width (design px) of a single-line text widget already filled.
	static float MeasuredWidth(TextWidget tw, string value, float size)
	{
		float width = INT_UIF_Draw.TextWidth(value, size);
		if (!tw)
			return width;
		tw.Update();
		int mw;
		int mh;
		tw.GetTextSize(mw, mh);
		float f = INT_UIF_Scale.Factor();
		if (mw > 0 && f > 0)
			width = mw / f;
		return width;
	}
}

// UI sounds (2D, UI channel). The player switches message / mention sounds in the editor.
class INT_GC_Sfx
{
	static const string MESSAGE = "INT_GC_Message_SoundSet";
	static const string MENTION = "INT_GC_Mention_SoundSet";
	static const string SEND = "INT_GC_Send_SoundSet";
	static const string FAIL = "INT_GC_Error_SoundSet";
	static const string CLICK = "INT_GC_Click_SoundSet";
	static const float MIN_GAP = 0.15;

	protected static float s_LastAt = -100;
	protected static ref array<AbstractWave> s_Waves;
	protected static int s_At;

	static void Play(string soundSet)
	{
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;
		float now = GetGame().GetTickTime();
		if (now - s_LastAt < MIN_GAP)
			return;
		SoundParams soundParams = new SoundParams(soundSet);
		if (!soundParams.IsValid())
		{
			INT_GC_Log.Warn("набор звука не найден: " + soundSet);
			return;
		}
		SoundObjectBuilder soundBuilder = new SoundObjectBuilder(soundParams);
		SoundObject soundObject = soundBuilder.BuildSoundObject();
		if (!soundObject)
			return;
		soundObject.SetKind(WaveKind.WAVEUI);
		AbstractSoundScene scene = GetGame().GetSoundScene();
		if (!scene)
			return;
		AbstractWave wave = scene.Play2D(soundObject, soundBuilder);
		if (!wave)
			return;
		wave.Loop(false);
		wave.Play();
		s_LastAt = now;
		// keep the last few waves referenced until they finish
		if (!s_Waves)
		{
			s_Waves = new array<AbstractWave>();
			for (int i = 0; i < 4; i++)
				s_Waves.Insert(null);
		}
		s_Waves.Set(s_At, wave);
		s_At++;
		if (s_At >= 4)
			s_At = 0;
	}
}
