// INT PartyMode (server) — PartyHistory log (spec.md §6, owner D9): one file per server start named by the
// machine's local date and time, Russian lines with Steam IDs, the file stays open for the session (6-9 us per
// line; lines survive a killed process, measured by INT_AdminGameLogger).

class INT_PTY_SE_History
{
	protected static FileHandle s_File;
	protected static bool s_Open;
	static string s_Path;
	static int s_Lines;
	static ref array<string> s_Capture;		// self-test: lines go here instead of the file

	static void Open()
	{
		if (s_Open)
			return;
		if (INT_PTY_SE_Config.Get().WriteHistoryLog != 1)
			return;
		INT_PTY_SE_Paths.MakeDirs();
		s_Path = INT_PTY_SE_Paths.HistoryDir() + "/" + INT_PTY_Util.FileStamp() + ".log";
		s_File = OpenFile(s_Path, FileMode.APPEND);
		if (s_File == 0)
		{
			INT_PTY_Log.Err("PartyHistory: не удалось открыть " + s_Path);
			return;
		}
		s_Open = true;
		FPrintln(s_File, INT_PTY_Util.Clock() + " | --- старт сервера, INT PartyMode " + INT_PTY_Const.VERSION + " ---");
	}

	static void Close()
	{
		if (!s_Open)
			return;
		FPrintln(s_File, INT_PTY_Util.Clock() + " | --- остановка сервера ---");
		CloseFile(s_File);
		s_Open = false;
	}

	static bool IsOpen()
	{
		return s_Open;
	}

	static string Who(string name, string id)
	{
		return name + " (" + id + ")";
	}

	static void Line(string text)
	{
		INT_PTY_Log.Dbg("история: " + text);
		if (s_Capture)
		{
			s_Capture.Insert(text);
			return;
		}
		if (!s_Open)
			return;
		FPrintln(s_File, INT_PTY_Util.Clock() + " | " + text);
		s_Lines++;
	}
}
