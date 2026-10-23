class DRL_CraftLogger
{
	static const string DIR_BASE = "$profile:\\DRL_CraftMenu\\";
	static const string DIR_LOGS = "$profile:\\DRL_CraftMenu\\Logs\\";
	static string m_LogFilePath;
	static bool m_Inited;

	static void InitOnce()
	{
		if (m_Inited)
		{
			return;
		}

		if (!FileExist(DIR_BASE))
		{
			MakeDirectory(DIR_BASE);
		}

		if (!FileExist(DIR_LOGS))
		{
			MakeDirectory(DIR_LOGS);
		}

		string stamp = BuildStamp();
		m_LogFilePath = DIR_LOGS + "CraftMenu_" + stamp + ".log";
		m_Inited = true;

		Info("=== DRL CraftMenu Log Started ===");
	}

	static void Info(string msg)
	{
		WriteLine("INFO", msg);
	}

	static void Warn(string msg)
	{
		WriteLine("WARN", msg);
	}

	static void Error(string msg)
	{
		WriteLine("ERROR", msg);
	}

	static void WriteLine(string level, string msg)
	{
		if (!m_Inited)
		{
			InitOnce();
		}

		string timeStr = GetTimeStr();
		string line = "[" + timeStr + "] [" + level + "] " + msg;

		FileHandle fh = OpenFile(m_LogFilePath, FileMode.APPEND);
		if (fh != 0)
		{
			FPrintln(fh, line);
			CloseFile(fh);
		}
		
		if (level == "ERROR")
		{
			ErrorEx(line);
		}
		else
		{
			Print(line);
		}
	}

	static string GetTimeStr()
	{
		int year, month, day;
		int hour, minute, second;
	
		GetYearMonthDay(year, month, day);
		GetHourMinuteSecond(hour, minute, second);
	
		return Pad2(hour) + ":" + Pad2(minute) + ":" + Pad2(second);
	}

	static string BuildStamp()
	{
		int year, month, day;
		int hour, minute, second;
	
		GetYearMonthDay(year, month, day);
		GetHourMinuteSecond(hour, minute, second);
		
		return year.ToString() + "-" + Pad2(month) + "-" + Pad2(day) + "_" + Pad2(hour) + "-" + Pad2(minute) + "-" + Pad2(second);
	}

	static string Pad2(int v)
	{
		if (v < 10)
		{
			return "0" + v.ToString();
		}
		return v.ToString();
	}
};
