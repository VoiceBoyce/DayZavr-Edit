// SFP Display JSON config (multi-display by position key)
// Config path: $profile:SFP\\SFP_Display.json
//
// New format:
// {
//   "Defaults": { "Text": "install JSON text", "ScrollSpeed": 0.5 },
//   "Displays": {
//     "pos_1234_0_5678": { "Text": "МАГАЗИН", "ScrollSpeed": 0.4 }
//   }
// }
//
// Old format (still supported):
// { "Text": "...", "ScrollSpeed": 0.5 }

class SFP_DisplayEntry
{
	string Text;
	float ScrollSpeed;

	void SFP_DisplayEntry()
	{
		Text = "install JSON text";
		ScrollSpeed = 0.3; // seconds
	}
}

class SFP_DisplayConfigFile
{
	ref SFP_DisplayEntry Defaults;
	ref map<string, ref SFP_DisplayEntry> Displays;

	// Backward compatibility fields (if someone still has the old JSON)
	string Text;
	float ScrollSpeed;

	void SFP_DisplayConfigFile()
	{
		Defaults = new SFP_DisplayEntry();
		Displays = new map<string, ref SFP_DisplayEntry>();
	}
}

class SFP_DisplayConfig
{
	protected static const string FOLDER_PATH = "$profile:SFP";
	protected static const string FILE_PATH = "$profile:SFP\\SFP_Display.json";

	protected static ref SFP_DisplayConfigFile m_File;
	protected static int m_LastReloadMS;

	static string GetFilePath()
	{
		return FILE_PATH;
	}

	static string GetFolderPath()
	{
		return FOLDER_PATH;
	}

	protected static void EnsureDirectory()
	{
		// Create $profile:SFP if missing
		if (!FileExist(FOLDER_PATH))
		{
			MakeDirectory(FOLDER_PATH);
		}
	}

	//! Builds a stable key for a display based on its position.
	//! Example: pos_1234_0_5678
	static string MakeKeyFromPos(vector pos)
	{
		int x = Math.Round(pos[0]);
		int y = Math.Round(pos[1]);
		int z = Math.Round(pos[2]);
		return string.Format("pos_%1_%2_%3", x, y, z);
	}

	//! Ensures that a position-key exists in JSON (auto-register).
	//! If missing, creates an entry using Defaults and saves the file.
	static void EnsureDisplayEntry(string displayKey)
	{
		if (displayKey == string.Empty)
			return;

		EnsureLoaded();
		if (!m_File)
			return;

		if (!m_File.Defaults)
			m_File.Defaults = new SFP_DisplayEntry();
		if (!m_File.Displays)
			m_File.Displays = new map<string, ref SFP_DisplayEntry>();

		ref SFP_DisplayEntry existing;
		if (m_File.Displays.Find(displayKey, existing))
			return;

		ref SFP_DisplayEntry e = new SFP_DisplayEntry();
		// Copy defaults as initial values
		if (m_File.Defaults)
		{
			e.Text = m_File.Defaults.Text;
			e.ScrollSpeed = m_File.Defaults.ScrollSpeed;
		}
		SanitizeEntry(e);

		m_File.Displays.Insert(displayKey, e);

		string saveErr;
		JsonFileLoader<SFP_DisplayConfigFile>.SaveFile(FILE_PATH, m_File, saveErr);

		SFP_Log.Info(string.Format("[SFP_Display] Added display entry '%1' to %2", displayKey, FILE_PATH));
	}

	static void EnsureLoaded()
	{
		if (!m_File)
		{
			LoadOrCreate();
			m_LastReloadMS = GetGame().GetTime();
		}
	}

	//! Forces a reload from disk (and creates the file if it doesn't exist).
	static bool ForceReload()
	{
		m_LastReloadMS = GetGame().GetTime();
		return LoadOrCreate();
	}

	//! Reloads at most once per minIntervalMS.
	static bool ReloadIfNeeded(int minIntervalMS = 5000)
	{
		EnsureLoaded();
		int now = GetGame().GetTime();
		if ((now - m_LastReloadMS) < minIntervalMS)
			return false;

		m_LastReloadMS = now;
		return LoadOrCreate();
	}

	//! Returns per-display entry for given key, falling back to Defaults.
	static SFP_DisplayEntry GetForKey(string displayKey)
	{
		EnsureLoaded();
		if (!m_File)
			return null;

		if (!m_File.Defaults)
			m_File.Defaults = new SFP_DisplayEntry();

		if (!m_File.Displays)
			m_File.Displays = new map<string, ref SFP_DisplayEntry>();

		ref SFP_DisplayEntry outEntry;
		if (displayKey != string.Empty && m_File.Displays.Find(displayKey, outEntry))
		{
			// Merge with defaults (so missing fields can still fall back)
			ref SFP_DisplayEntry merged = new SFP_DisplayEntry();
			merged.Text = m_File.Defaults.Text;
			merged.ScrollSpeed = m_File.Defaults.ScrollSpeed;
			if (outEntry && outEntry.Text != string.Empty)
				merged.Text = outEntry.Text;
			if (outEntry && outEntry.ScrollSpeed > 0)
				merged.ScrollSpeed = outEntry.ScrollSpeed;
			SanitizeEntry(merged);
			return merged;
		}

		SanitizeEntry(m_File.Defaults);
		return m_File.Defaults;
	}

	protected static bool LoadOrCreate()
	{
		EnsureDirectory();

		ref SFP_DisplayConfigFile tmp = new SFP_DisplayConfigFile();
		string err;
		bool loaded = JsonFileLoader<SFP_DisplayConfigFile>.LoadFile(FILE_PATH, tmp, err);

		if (!loaded)
		{
			// Create default file
			string saveErr;
			JsonFileLoader<SFP_DisplayConfigFile>.SaveFile(FILE_PATH, tmp, saveErr);
			SFP_Log.Info(string.Format("[SFP_Display] Config not loaded (%1). Created default: %2", err, FILE_PATH));
		}

		SanitizeFile(tmp);
		m_File = tmp;
		return loaded;
	}

	protected static void SanitizeFile(SFP_DisplayConfigFile data)
	{
		if (!data)
			return;

		if (!data.Defaults)
			data.Defaults = new SFP_DisplayEntry();

		// Backward compatibility: old JSON had Text/ScrollSpeed at root.
		if (data.Text != string.Empty)
			data.Defaults.Text = data.Text;
		if (data.ScrollSpeed > 0)
			data.Defaults.ScrollSpeed = data.ScrollSpeed;

		SanitizeEntry(data.Defaults);

		if (!data.Displays)
			data.Displays = new map<string, ref SFP_DisplayEntry>();

		// Sanitize entries (cap count to avoid crazy configs)
		int safeCount = 0;
		foreach (string k, SFP_DisplayEntry e : data.Displays)
		{
			safeCount++;
			if (safeCount > 512)
				break;
			SanitizeEntry(e);
		}
	}

	protected static void SanitizeEntry(SFP_DisplayEntry e)
	{
		if (!e)
			return;

		if (e.Text == string.Empty)
			e.Text = "install JSON text";

		// Replace line breaks with spaces to keep the display 1-line.
		e.Text.Replace("\r", " ");
		e.Text.Replace("\n", " ");
		e.Text.Replace("\t", " ");

		// Clamp speed
		if (e.ScrollSpeed <= 0)
			e.ScrollSpeed = 0.5;
		if (e.ScrollSpeed < 0.05)
			e.ScrollSpeed = 0.05;
		if (e.ScrollSpeed > 10.0)
			e.ScrollSpeed = 10.0;

		// Safety length limit (prevents huge JSON strings)
		if (e.Text.LengthUtf8() > 256)
			e.Text = e.Text.SubstringUtf8(0, 256);
	}
}
