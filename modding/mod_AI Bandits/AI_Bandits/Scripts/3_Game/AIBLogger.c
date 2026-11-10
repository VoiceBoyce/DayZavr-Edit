class AIBLogger {
	
	static void Log(string txt) {
		string file_path = "$profile:\\AI_Bandits\\AIB.log";
		FileHandle logFile = OpenFile(file_path, FileMode.APPEND);
		StaticAIBConfig config = GetDayZGame().GetStaticAIBConfig();
		if (logFile != 0 && config && config.NPCDebug) {
				FPrintln(logFile, GetDate() + " [AI Bandits] " + txt);
				CloseFile(logFile);
		}
	}
	
	static void Debug(string txt) {
		string file_path = "$profile:\\AI_Bandits\\AIB.log";
		FileHandle logFile = OpenFile(file_path, FileMode.APPEND);
		StaticAIBConfig config = GetDayZGame().GetStaticAIBConfig();
		if (logFile != 0 && config.NPCDebug) {
				FPrintln(logFile, GetDate() + " [AI Bandits Debug] " + txt);
				CloseFile(logFile);
		}
	}

	
		
	static private string GetDate(bool fileFriendly = false) {
		int year, month, day, hour, minute, second;

		GetYearMonthDay(year, month, day);
		GetHourMinuteSecond(hour, minute, second);

		string date = day.ToStringLen(2) + "." + month.ToStringLen(2) + "." + year.ToStringLen(4) + " " + hour.ToStringLen(2) + ":" + minute.ToStringLen(2) + ":" + second.ToStringLen(2);
		if (fileFriendly) {
			date.Replace(" ", "_");
			date.Replace(".", "-");
			date.Replace(":", "-");
		}

		return date;
	}
}