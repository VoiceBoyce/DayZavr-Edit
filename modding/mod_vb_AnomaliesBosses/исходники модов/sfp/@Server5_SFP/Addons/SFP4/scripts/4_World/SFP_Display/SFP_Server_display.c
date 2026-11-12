// SFP_Server_display - 10-cell texture display with JSON-configurable text + optional scrolling.
//
// Requirements from server owner (JSON):
//  - Edit $profile:SFP\SFP_Display.json
//  - New format:
//      {
//        "Defaults": { "Text": "install JSON text", "ScrollSpeed": 0.5 },
//        "Displays": {
//          "pos_1234_0_5678": { "Text": "МАГАЗИН", "ScrollSpeed": 0.4 }
//        }
//      }
//    The key is derived from display position (rounded): pos_X_Y_Z
//  - Backward compatible old format: { "Text": "...", "ScrollSpeed": 0.5 }
//
// Model selections (hiddenSelections / animations):
//  - cell_1 .. cell_10 (left to right)
//
// NOTE:
//  - English letters are expected as textures: ...\latter_A_co.paa (A-Z)
//  - Digits: ...\number_0_co.paa (0-9)
//  - Supported symbols: - @ . ! # + ? / = , ( ) $
//  - Russian letters supported via "keyboard layout" mapping to ASCII keys.
//    Example: Russian "Ф" (key A) => ...\Rus_latter_A_co.paa
//
//    Mapping (RU -> key):
//      Ф=A, И=B, С=C, В=D, У=E, А=F, П=G, Р=H, Ш=I, О=J, Л=K, Д=L,
//      Ь=M, Т=N, Щ=O, З=P, Й=Q, К=R, Ы=S, Е=T, Г=U, М=V, Ц=W, Ч=X,
//      Н=Y, Я=Z
//
//    Extra RU letters (optional tokens):
//      Ё=GRAVE, Х=LBRACKET, Ъ=RBRACKET, Ж=SEMICOLON, Э=APOSTROPHE, Б=COMMA, Ю=DOT
//    Expected textures (if you want these letters):
//      ...\Rus_latter_GRAVE_co.paa, ...\Rus_latter_LBRACKET_co.paa, ...\Rus_latter_RBRACKET_co.paa,
//      ...\Rus_latter_SEMICOLON_co.paa, ...\Rus_latter_APOSTROPHE_co.paa, ...\Rus_latter_COMMA_co.paa,
//      ...\Rus_latter_DOT_co.paa
//
//  - Unknown characters become blank.

class SFP_DisplayTextures
{
	static const string BASE = "SFP3\\1_item\\Display\\data\\alfavit\\";
	static const string TRANSPARENT = "#(argb,8,8,3)color(0,0,0,0,CA)";

	static string Letter(string oneUpperLetter)
	{
		return string.Format("%1latter_%2_co.paa", BASE, oneUpperLetter);
	}

	static string RusLetter(string keyToken)
	{
		// keyToken can be A..Z or a token like GRAVE/LBRACKET/... (see header comment)
		return string.Format("%1Rus_latter_%2_co.paa", BASE, keyToken);
	}

	static string Number(string oneDigit)
	{
		return string.Format("%1number_%2_co.paa", BASE, oneDigit);
	}

	static string Symbol(string ch)
	{
		switch (ch)
		{
			case "-": return BASE + "Comma_dash_co.paa";
			case "@": return BASE + "Comma_dogs_co.paa";
			case ".": return BASE + "Comma_dot_co.paa";
			case "!": return BASE + "Comma_exclamation_co.paa";
			case "#": return BASE + "Comma_lattice_co.paa";
			case "+": return BASE + "Comma_plus_co.paa";
			case "?": return BASE + "Comma_question_co.paa";
			case "/": return BASE + "Comma_slash_co.paa";
			case "=": return BASE + "Comma_smooth_co.paa";
			case ",": return BASE + "Comma_symbol_co.paa";
			case "(": return BASE + "left_parenthesis_co.paa";
			case ")": return BASE + "right_parenthesis_co.paa";
			case "$": return BASE + "Comma_dollar_co.paa";
		}

		return string.Empty;
	}
}

class SFP_Server_display : House
{
	protected static const int CELL_COUNT = 13;
	protected static const float ACTIVE_RADIUS = 90.0;
	protected static const float ACTIVE_RADIUS_SQ = ACTIVE_RADIUS * ACTIVE_RADIUS;

	// How often we check for players around the display.
	protected static const int PLAYER_CHECK_INTERVAL_MS = 5000;
	// How often we allow JSON reload checks while active.
	protected static const int CONFIG_RELOAD_MIN_INTERVAL_MS = 5000;

	protected ref array<string> m_CellNames;
	protected ref array<int> m_CellHiddenSelectionIdx;
	protected string m_DisplayKey;


	protected bool m_IsActive;
	protected bool m_IsScrolling;

	protected int m_ScrollPos;
	protected string m_CurrentText;          // normalized (uppercase)
	protected float m_CurrentScrollSpeed;   // seconds
	protected string m_ScrollBuffer;

	void SFP_Server_display()
	{
		m_CellNames = new array<string>();
		m_CellHiddenSelectionIdx = new array<int>();

		for (int i = 1; i <= CELL_COUNT; i++)
		{
			m_CellNames.Insert(string.Format("cell_%1", i));
			m_CellHiddenSelectionIdx.Insert(-1);
		}
	}

	override void EEInit()
	{
		super.EEInit();

		if (GetGame().IsServer())
		{
			InitHiddenSelectionIndexes();

			// Build position key once (used for multi-display JSON mapping).
			m_DisplayKey = SFP_DisplayConfig.MakeKeyFromPos(GetPosition());
			SFP_Log.Debug(string.Format("[SFP_Display] Init display key: %1 (pos=%2)", m_DisplayKey, GetPosition()));

			// Auto-register this display in JSON (so admin can edit per-position).
			SFP_DisplayConfig.EnsureDisplayEntry(m_DisplayKey);

			// Load config and set initial text once.
			ApplyConfig(true);
			UpdateDisplayOnce();

			// Player proximity gate.
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(CheckPlayersAndUpdate, PLAYER_CHECK_INTERVAL_MS, true);
			CheckPlayersAndUpdate();
		}
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);

		if (GetGame() && GetGame().IsServer())
		{
			StopScroll();
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(CheckPlayersAndUpdate);
		}
	}

	protected void InitHiddenSelectionIndexes()
	{
		for (int i = 0; i < CELL_COUNT; i++)
		{
			m_CellHiddenSelectionIdx[i] = GetHiddenSelectionIndex(m_CellNames[i]);
		}
	}

	//! Checks if any players are within ACTIVE_RADIUS, and starts/stops scrolling accordingly.
	protected void CheckPlayersAndUpdate()
	{
		if (!GetGame() || !GetGame().IsServer())
			return;

		bool hasNearbyPlayers = false;
		vector myPos = GetPosition();

		array<Man> players = new array<Man>();
		GetGame().GetPlayers(players);

		foreach (Man man : players)
		{
			PlayerBase pb = PlayerBase.Cast(man);
			if (!pb)
				continue;

			if (!pb.IsAlive())
				continue;

			if (vector.DistanceSq(pb.GetPosition(), myPos) <= ACTIVE_RADIUS_SQ)
			{
				hasNearbyPlayers = true;
				break;
			}
		}

		if (hasNearbyPlayers)
		{
			// While active, allow config reload checks.
			ApplyConfig(false);

			if (!m_IsActive)
			{
				m_IsActive = true;
				StartScrollIfNeeded();
			}
			else
			{
				// Already active; if speed/text changed, StartScrollIfNeeded() is handled inside ApplyConfig.
			}
		}
		else
		{
			if (m_IsActive)
			{
				m_IsActive = false;
				StopScroll();
			}
		}
	}

	protected void ApplyConfig(bool forceReload)
	{
		if (!GetGame() || !GetGame().IsServer())
			return;

		if (forceReload)
			SFP_DisplayConfig.ForceReload();
		else
			SFP_DisplayConfig.ReloadIfNeeded(CONFIG_RELOAD_MIN_INTERVAL_MS);

		SFP_DisplayEntry cfg = SFP_DisplayConfig.GetForKey(m_DisplayKey);
		if (!cfg)
			return;

		string newText = cfg.Text;
		// normalize to uppercase (textures are A-Z; and our RU mapping uses uppercase Cyrillic)
		newText.ToUpper();

		float newSpeed = cfg.ScrollSpeed;

		bool changed = (newText != m_CurrentText) || (newSpeed != m_CurrentScrollSpeed);
		if (!changed)
			return;

		m_CurrentText = newText;
		m_CurrentScrollSpeed = newSpeed;
		m_ScrollPos = 0;

		BuildScrollBuffer();
		UpdateDisplayOnce();

		// If we're active, restart scrolling with new speed if needed.
		if (m_IsActive)
			StartScrollIfNeeded();
	}

	protected void BuildScrollBuffer()
	{
		m_IsScrolling = (m_CurrentText.LengthUtf8() > CELL_COUNT);

		if (!m_IsScrolling)
		{
			m_ScrollBuffer = m_CurrentText;
			return;
		}

		// pad with 10 spaces to create a smooth loop
		string pad = string.Empty;
		for (int i = 0; i < CELL_COUNT; i++)
			pad += " ";

		m_ScrollBuffer = pad + m_CurrentText + pad;
		// Start with the first visible character (skip the leading padding)
		m_ScrollPos = CELL_COUNT;
	}

	protected void StartScrollIfNeeded()
	{
		StopScroll();

		if (!m_IsActive)
			return;

		if (!m_IsScrolling)
			return;

		int delayMS = Math.Round(m_CurrentScrollSpeed * 1000.0);
		if (delayMS < 50)
			delayMS = 50;

		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(ScrollStep, delayMS, true);
	}

	protected void StopScroll()
	{
		if (!GetGame())
			return;

		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(ScrollStep);
	}

	protected void ScrollStep()
	{
		if (!GetGame() || !GetGame().IsServer())
			return;

		if (!m_IsActive)
		{
			StopScroll();
			return;
		}

		int len = m_ScrollBuffer.LengthUtf8();
		if (len <= 0)
			return;

		m_ScrollPos++;
		if (m_ScrollPos >= len)
			m_ScrollPos = 0;

		UpdateDisplayOnce();
	}

	protected void UpdateDisplayOnce()
	{
		if (!GetGame() || !GetGame().IsServer())
			return;

		string view = GetCurrentViewString();

		for (int i = 0; i < CELL_COUNT; i++)
		{
			string ch = " ";
			if (i < view.LengthUtf8())
				ch = view.SubstringUtf8(i, 1);

			ApplyCharToCell(i, ch);
		}
	}

	protected string GetCurrentViewString()
	{
		if (!m_IsScrolling)
			return m_CurrentText;

		int len = m_ScrollBuffer.LengthUtf8();
		if (len <= CELL_COUNT)
			return m_ScrollBuffer;

		// Build a 10-char window with wrap-around
		string outStr = string.Empty;
		for (int i = 0; i < CELL_COUNT; i++)
		{
			int idx = (m_ScrollPos + i) % len;
			outStr += m_ScrollBuffer.SubstringUtf8(idx, 1);
		}

		return outStr;
	}

	protected void ApplyCharToCell(int cellIndex, string ch)
	{
		string selectionName = m_CellNames[cellIndex];
		int hsIdx = m_CellHiddenSelectionIdx[cellIndex];

		string tex = TextureForChar(ch);

		if (tex == string.Empty)
		{
			// blank
			HideSelection(selectionName);
			if (hsIdx != -1)
				SetObjectTexture(hsIdx, SFP_DisplayTextures.TRANSPARENT);
			return;
		}

		ShowSelection(selectionName);
		if (hsIdx != -1)
			SetObjectTexture(hsIdx, tex);
	}

	protected string TextureForChar(string ch)
	{
		if (ch == string.Empty)
			return string.Empty;

		// whitespace = blank
		if (ch == " " || ch == "\t")
			return string.Empty;

		// Numbers 0-9
		if ("0123456789".IndexOf(ch) != -1)
			return SFP_DisplayTextures.Number(ch);

		// Russian letters
		string ru = RussianTexture(ch);
		if (ru != string.Empty)
			return ru;

		// English letters A-Z (input is normalized to upper-case)
		if ("ABCDEFGHIJKLMNOPQRSTUVWXYZ".IndexOf(ch) != -1)
			return SFP_DisplayTextures.Letter(ch);

		// Symbols
		return SFP_DisplayTextures.Symbol(ch);
	}

	// RU letter -> texture mapping based on RU keyboard layout.
	protected string RussianTexture(string ch)
	{
		switch (ch)
		{
			// A..Z keys
			case "Ф": case "ф": return SFP_DisplayTextures.RusLetter("A");
			case "И": case "и": return SFP_DisplayTextures.RusLetter("B");
			case "С": case "с": return SFP_DisplayTextures.RusLetter("C");
			case "В": case "в": return SFP_DisplayTextures.RusLetter("D");
			case "У": case "у": return SFP_DisplayTextures.RusLetter("E");
			case "А": case "а": return SFP_DisplayTextures.RusLetter("F");
			case "П": case "п": return SFP_DisplayTextures.RusLetter("G");
			case "Р": case "р": return SFP_DisplayTextures.RusLetter("H");
			case "Ш": case "ш": return SFP_DisplayTextures.RusLetter("I");
			case "О": case "о": return SFP_DisplayTextures.RusLetter("J");
			case "Л": case "л": return SFP_DisplayTextures.RusLetter("K");
			case "Д": case "д": return SFP_DisplayTextures.RusLetter("L");
			case "Ь": case "ь": return SFP_DisplayTextures.RusLetter("M");
			case "Т": case "т": return SFP_DisplayTextures.RusLetter("N");
			case "Щ": case "щ": return SFP_DisplayTextures.RusLetter("O");
			case "З": case "з": return SFP_DisplayTextures.RusLetter("P");
			case "Й": case "й": return SFP_DisplayTextures.RusLetter("Q");
			case "К": case "к": return SFP_DisplayTextures.RusLetter("R");
			case "Ы": case "ы": return SFP_DisplayTextures.RusLetter("S");
			case "Е": case "е": return SFP_DisplayTextures.RusLetter("T");
			case "Г": case "г": return SFP_DisplayTextures.RusLetter("U");
			case "М": case "м": return SFP_DisplayTextures.RusLetter("V");
			case "Ц": case "ц": return SFP_DisplayTextures.RusLetter("W");
			case "Ч": case "ч": return SFP_DisplayTextures.RusLetter("X");
			case "Н": case "н": return SFP_DisplayTextures.RusLetter("Y");
			case "Я": case "я": return SFP_DisplayTextures.RusLetter("Z");

			// Optional extra keys (see header comment)
			case "Ё": case "ё": return SFP_DisplayTextures.RusLetter("GRAVE");
			case "Х": case "х": return SFP_DisplayTextures.RusLetter("LBRACKET");
			case "Ъ": case "ъ": return SFP_DisplayTextures.RusLetter("RBRACKET");
			case "Ж": case "ж": return SFP_DisplayTextures.RusLetter("SEMICOLON");
			case "Э": case "э": return SFP_DisplayTextures.RusLetter("APOSTROPHE");
			case "Б": case "б": return SFP_DisplayTextures.RusLetter("COMMA");
			case "Ю": case "ю": return SFP_DisplayTextures.RusLetter("DOT");
		}

		return string.Empty;
	}
}
