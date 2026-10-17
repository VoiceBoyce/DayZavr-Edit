// INT_UIFramework - log helper. One tag so the RPT can be grepped for "[INT_UIF]".
class INT_UIF_Log
{
	// Traces every widget/component build. Off for release: problems are
	// logged at Error level anyway; flip on to follow a build step by step.
	static bool s_Debug = false;

	static void Info(string msg)
	{
		Print("[INT_UIF] " + msg);
	}

	static void Debug(string msg)
	{
		if (s_Debug)
			Print("[INT_UIF][dbg] " + msg);
	}

	static void Error(string msg)
	{
		Print("[INT_UIF][ERROR] " + msg);
	}
}
