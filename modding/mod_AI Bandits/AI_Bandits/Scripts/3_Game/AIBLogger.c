class AIBLogger {

	// Раньше файл открывался при каждом вызове и при выключенной отладке не закрывался: утечка дескрипторов
	static bool IsDebug() {
		StaticAIBConfig config = GetDayZGame().GetStaticAIBConfig();
		return config && config.NPCDebug;
	}

	static void Write(string prefix, string txt) {
		if (!IsDebug())
			return;
		FileHandle logFile = OpenFile("$profile:\\AI_Bandits\\AIB.log", FileMode.APPEND);
		if (logFile == 0)
			return;
		FPrintln(logFile, GetDate() + prefix + txt);
		CloseFile(logFile);
	}

	static void Log(string txt) {
		Write(" [AI Bandits] ", txt);
	}

	static void Debug(string txt) {
		Write(" [AI Bandits Debug] ", txt);
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