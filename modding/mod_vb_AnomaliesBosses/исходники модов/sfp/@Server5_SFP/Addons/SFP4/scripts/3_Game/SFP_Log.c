// Simple logger for SFP mods (to prevent log spam on servers)
//
// By default INFO/DEBUG are disabled.
// Enable if you need extra logs during testing.

class SFP_Log
{
	static bool ENABLE_INFO  = false;
	static bool ENABLE_DEBUG = false;

	static void Info(string msg)
	{
		if (ENABLE_INFO)
			Print(msg);
	}

	static void Debug(string msg)
	{
		if (ENABLE_DEBUG)
			Print(msg);
	}

	// Errors are always printed (server owners usually want to see real errors)
	// ErrorEx, not Error: inside this class Error() resolves to SFP_Log.Error itself (endless recursion, crash).
	static void Error(string msg)
	{
		string text = "[SFP] " + msg;
		text.Replace("%", " pct"); // a bare % in printed text can kill the client
		ErrorEx(text);
	}
}
