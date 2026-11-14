// INT_VPP — one-time move of our files into $profile:INT_Mods/INT_VppMap/ (2026-11-12, CL/SE split; review fixes the
// same evening). Shared by both sides: the client moves Client.json (INT_VPP_Prefs), the server part INT_VppMap_SE moves
// VPPMapConfig.json, Settings.json and Zones.json (INT_VPP_SE_Migrate). Files are moved by their KNOWN names only
// (never by listing a folder). DayZ has no rename, so a move = CopyFile, a whole-file byte compare of the copy, then
// DeleteFile of the old file. A group = a main file + its .tmp / .bak; it moves together or not at all.
//   old place empty                      -> NOTHING: the new place is used (missing files = defaults there)
//   only the old place has files         -> copy + byte check of every file, then the old files are deleted (MOVED),
//                                           one INFO line per file "[INT_VPP] migrated <old> -> <new>"
//   both places have files:
//     (a) new main == old main byte for byte, companions in both places equal -> an unfinished move: the missing
//         companions are copied, the old files deleted (MOVED, INFO lines, no WARN)
//     (b) the new main is a valid file of its kind -> the new place is used, the old files are left alone (BOTH, WARN);
//         also the way back from a rollback to the old mod (old place = VPP's demo defaults, new = the real config)
//     (c) the new main is missing or not valid while the old main is valid -> the new files the old ones replace are
//         set aside as .broken-<stamp> (byte-identical copy first), then the normal move old -> new (MOVED)
//     (d) neither main is valid -> NEITHER: the new place is used (its loader sets the file aside and writes the
//         defaults), nothing is deleted (WARN)
//   a copy / its check failed, or a file is bigger than INT_READ_MAX (cannot be compared to the last byte)
//                                        -> the copies made are deleted, the old files stay and are used THIS session
//                                           (FAILED, one WARN line); the next start tries again
// "Valid" = parsed with JsonSerializer.ReadFromString into the file's own class (INT_VPP_FileCheck), never with a
// loader that throws. A second start finds the old place empty: no-op, no line.

// What a valid file of a group's kind looks like (one subclass per file kind).
class INT_VPP_FileCheck : Managed
{
	// text = the whole file; true = it parses into this kind's class
	bool INT_Parses(string text)
	{
		return true;
	}
}

class INT_VPP_FileMove
{
	static const string INT_MODS_DIR = "$profile:INT_Mods";
	static const string INT_DIR = "$profile:INT_Mods/INT_VppMap";
	static const int INT_READ_MAX = 2000000;

	static const int INT_RES_NOTHING = 0;
	static const int INT_RES_MOVED = 1;
	static const int INT_RES_BOTH = 2;
	static const int INT_RES_FAILED = 3;
	static const int INT_RES_NEITHER = 4;

	// INT_ReadEx results
	static const int INT_READ_OK = 0;
	static const int INT_READ_MISSING = 1;
	static const int INT_READ_TOO_BIG = 2;

	// TestKit only: the copy number `failAt` (counted from this call on, 1 = the next copy) fails.
	static const int INT_TEST_NO_COPY = 1;        // CopyFile is not done (= CopyFile returned false)
	static const int INT_TEST_DAMAGED_COPY = 2;   // CopyFile is done, then one byte is added to the copy

	protected static int s_INT_TestFailAt;
	protected static int s_INT_TestFailMode;
	protected static int s_INT_Copies;
	// TestKit only: a smaller size limit (0 = INT_READ_MAX)
	protected static int s_INT_TestReadMax;
	// WARN lines printed by the moves (TestKit: "a second start is silent")
	static int s_INT_Warns;

	static void INT_SetTestFail(int failAt, int mode)
	{
		s_INT_TestFailAt = failAt;
		s_INT_TestFailMode = mode;
		s_INT_Copies = 0;
	}

	static void INT_SetTestReadMax(int bytes)
	{
		s_INT_TestReadMax = bytes;
	}

	static int INT_ReadMax()
	{
		if (s_INT_TestReadMax > 0)
			return s_INT_TestReadMax;
		return INT_READ_MAX;
	}

	static string INT_ResName(int res)
	{
		if (res == INT_RES_NOTHING)
			return "nothing";
		if (res == INT_RES_MOVED)
			return "moved";
		if (res == INT_RES_BOTH)
			return "both";
		if (res == INT_RES_FAILED)
			return "failed";
		if (res == INT_RES_NEITHER)
			return "neither";
		return "res" + res.ToString();
	}

	protected static void INT_Warn(string text)
	{
		s_INT_Warns++;
		Print("[INT_VPP] WARN " + text);
	}

	// "$profile:" + name, or dir + "/" + name
	static string INT_Join(string dir, string name)
	{
		int n = dir.Length();
		if (n == 0)
			return name;
		int li = n - 1;
		string last = dir.Get(li);
		if (last == ":" || last == "/")
			return dir + name;
		return dir + "/" + name;
	}

	// Creates every missing level of "$profile:A/B/C" (MakeDirectory makes one level). true = the folder exists.
	static bool INT_EnsureDir(string dir)
	{
		if (dir == "")
			return false;
		if (FileExist(dir))
			return true;
		int colon = dir.IndexOf(":");
		if (colon < 0)
		{
			MakeDirectory(dir);
			bool plain = FileExist(dir);
			return plain;
		}
		int headLen = colon + 1;
		string head = dir.Substring(0, headLen);
		int total = dir.Length();
		int restLen = total - headLen;
		string rest = "";
		if (restLen > 0)
			rest = dir.Substring(headLen, restLen);
		array<string> parts = new array<string>();
		rest.Split("/", parts);
		string cur = head;
		bool first = true;
		foreach (string part : parts)
		{
			if (part == "")
				continue;
			if (first)
				cur = cur + part;
			else
				cur = cur + "/" + part;
			first = false;
			if (!FileExist(cur))
				MakeDirectory(cur);
		}
		bool ok = FileExist(dir);
		return ok;
	}

	// Whole file in one ReadFile (no string.Replace, no SubstringUtf8). INT_READ_TOO_BIG = more than INT_ReadMax()
	// bytes: content is then NOT the whole file and must never be used to prove anything.
	static int INT_ReadEx(string path, out string content)
	{
		content = "";
		if (!FileExist(path))
			return INT_READ_MISSING;
		FileHandle fh = OpenFile(path, FileMode.READ);
		if (fh == 0)
			return INT_READ_MISSING;
		int limit = INT_VPP_FileMove.INT_ReadMax();
		int want = limit + 1;
		string txt;
		ReadFile(fh, txt, want);
		CloseFile(fh);
		content = txt;
		int got = txt.Length();
		if (got > limit)
			return INT_READ_TOO_BIG;
		return INT_READ_OK;
	}

	// The whole file (false = missing, unreadable or too big to read whole).
	static bool INT_ReadAll(string path, out string content)
	{
		string txt;
		int st = INT_VPP_FileMove.INT_ReadEx(path, txt);
		content = txt;
		if (st != INT_READ_OK)
			return false;
		return true;
	}

	static bool INT_TooBig(string path)
	{
		string txt;
		int st = INT_VPP_FileMove.INT_ReadEx(path, txt);
		if (st == INT_READ_TOO_BIG)
			return true;
		return false;
	}

	// Both files exist and hold the same bytes to the last one (length + whole-file compare; too big = not equal).
	static bool INT_SameBytes(string a, string b)
	{
		string ta;
		string tb;
		if (!INT_VPP_FileMove.INT_ReadAll(a, ta))
			return false;
		if (!INT_VPP_FileMove.INT_ReadAll(b, tb))
			return false;
		int la = ta.Length();
		int lb = tb.Length();
		if (la != lb)
			return false;
		if (ta != tb)
			return false;
		return true;
	}

	// The file exists, is read whole and is a valid file of the kind (no check = any whole file).
	static bool INT_FileValid(string path, INT_VPP_FileCheck check)
	{
		string txt;
		if (!INT_VPP_FileMove.INT_ReadAll(path, txt))
			return false;
		if (!check)
			return true;
		bool ok = check.INT_Parses(txt);
		return ok;
	}

	static string INT_Two(int v)
	{
		string s = v.ToString();
		if (v < 10)
			s = "0" + s;
		return s;
	}

	// UTC stamp for file names: 20261112-153000
	static string INT_Stamp()
	{
		int yy;
		int mo;
		int dd;
		int hh;
		int mi;
		int ss;
		GetYearMonthDayUTC(yy, mo, dd);
		GetHourMinuteSecondUTC(hh, mi, ss);
		string sMo = INT_VPP_FileMove.INT_Two(mo);
		string sDd = INT_VPP_FileMove.INT_Two(dd);
		string sHh = INT_VPP_FileMove.INT_Two(hh);
		string sMi = INT_VPP_FileMove.INT_Two(mi);
		string sSs = INT_VPP_FileMove.INT_Two(ss);
		string res = yy.ToString() + sMo + sDd + "-" + sHh + sMi + sSs;
		return res;
	}

	// Moves a file aside: byte-identical "<path>.<tag>-<stamp>" copy (a free name), then the original is deleted.
	// Returns the copy, or "" = nothing done (the original stays).
	static string INT_SetAside(string path, string tag)
	{
		if (!FileExist(path))
			return "";
		string stamp = INT_VPP_FileMove.INT_Stamp();
		string dst = path + "." + tag + "-" + stamp;
		int n = 1;
		while (FileExist(dst) && n < 20)
		{
			n++;
			dst = path + "." + tag + "-" + stamp + "-" + n.ToString();
		}
		if (FileExist(dst))
			return "";
		bool copied = CopyFile(path, dst);
		if (!copied)
			return "";
		if (!INT_VPP_FileMove.INT_SameBytes(path, dst))
		{
			DeleteFile(dst);
			return "";
		}
		DeleteFile(path);
		if (FileExist(path))
			return "";
		return dst;
	}

	protected static bool INT_Copy(string src, string dst)
	{
		s_INT_Copies++;
		int at = s_INT_TestFailAt;
		if (at > 0 && s_INT_Copies == at)
		{
			int mode = s_INT_TestFailMode;
			if (mode == INT_TEST_NO_COPY)
				return false;
			if (mode == INT_TEST_DAMAGED_COPY)
			{
				bool done = CopyFile(src, dst);
				FileHandle fh = OpenFile(dst, FileMode.APPEND);
				if (fh != 0)
				{
					FPrint(fh, " ");
					CloseFile(fh);
				}
				return done;
			}
		}
		bool ok = CopyFile(src, dst);
		return ok;
	}

	// Copies every old file of the group that has no file in the new place, checks each copy to the last byte, then
	// deletes the old files that are byte-equal to their new copy. Any failure: the copies made are deleted, nothing
	// old is deleted, FAILED + one WARN. `note` goes into the INFO lines.
	protected static int INT_CopyGroup(string oldDir, string newDir, array<string> names, string what, string note)
	{
		foreach (string big : names)
		{
			string bigPath = INT_VPP_FileMove.INT_Join(oldDir, big);
			if (!FileExist(bigPath))
				continue;
			if (INT_VPP_FileMove.INT_TooBig(bigPath))
			{
				int lim = INT_VPP_FileMove.INT_ReadMax();
				INT_VPP_FileMove.INT_Warn(what + ": " + big + " is bigger than " + lim.ToString() + " bytes and cannot be checked byte for byte, so it is not moved; the old file stays and is used this session: " + oldDir);
				return INT_RES_FAILED;
			}
		}
		INT_VPP_FileMove.INT_EnsureDir(newDir);
		array<string> made = new array<string>();
		string why = "";
		foreach (string name : names)
		{
			string src = INT_VPP_FileMove.INT_Join(oldDir, name);
			if (!FileExist(src))
				continue;
			string dst = INT_VPP_FileMove.INT_Join(newDir, name);
			if (FileExist(dst))
				continue;
			bool copied = INT_VPP_FileMove.INT_Copy(src, dst);
			if (FileExist(dst))
				made.Insert(dst);
			if (!copied)
			{
				why = "copy of " + name + " failed";
				break;
			}
			bool same = INT_VPP_FileMove.INT_SameBytes(src, dst);
			if (!same)
			{
				why = "copy of " + name + " differs from the original";
				break;
			}
		}
		if (why != "")
		{
			int left = 0;
			foreach (string bad : made)
			{
				DeleteFile(bad);
				if (FileExist(bad))
					left++;
			}
			string tail = "";
			if (left > 0)
				tail = " (" + left.ToString() + " partial copies could not be removed)";
			INT_VPP_FileMove.INT_Warn(what + ": move to " + newDir + " failed (" + why + "); the old file stays and is used this session: " + oldDir + tail);
			return INT_RES_FAILED;
		}
		foreach (string done : names)
		{
			string oldPath = INT_VPP_FileMove.INT_Join(oldDir, done);
			if (!FileExist(oldPath))
				continue;
			string newPath = INT_VPP_FileMove.INT_Join(newDir, done);
			// only an old file whose copy was compared to the last byte is ever deleted
			if (!INT_VPP_FileMove.INT_SameBytes(oldPath, newPath))
				continue;
			DeleteFile(oldPath);
			if (FileExist(oldPath))
				INT_VPP_FileMove.INT_Warn(what + ": " + newPath + " is in place, but the old file could not be deleted: " + oldPath);
			else
				Print("[INT_VPP] migrated " + oldPath + " -> " + newPath + note);
		}
		return INT_RES_MOVED;
	}

	// Moves the group `names` (names[0] = the main file) from oldDir to newDir (rules at the top of this file).
	// check = what a valid main file of this kind is; what = the name for the log lines.
	static int INT_MoveGroup(string oldDir, string newDir, array<string> names, string what, INT_VPP_FileCheck check)
	{
		if (!names || names.Count() == 0)
			return INT_RES_NOTHING;
		int nOld = 0;
		int nNew = 0;
		foreach (string nm : names)
		{
			string po = INT_VPP_FileMove.INT_Join(oldDir, nm);
			string pn = INT_VPP_FileMove.INT_Join(newDir, nm);
			if (FileExist(po))
				nOld++;
			if (FileExist(pn))
				nNew++;
		}
		if (nOld == 0)
			return INT_RES_NOTHING;
		if (nNew == 0)
		{
			int resCopy = INT_VPP_FileMove.INT_CopyGroup(oldDir, newDir, names, what, "");
			return resCopy;
		}
		string mainName = names.Get(0);
		string oldMain = INT_VPP_FileMove.INT_Join(oldDir, mainName);
		string newMain = INT_VPP_FileMove.INT_Join(newDir, mainName);
		bool oldHas = FileExist(oldMain);
		bool newHas = FileExist(newMain);
		// (a) an unfinished move: same main, every companion in both places equal
		bool sameMain = false;
		if (oldHas && newHas)
			sameMain = INT_VPP_FileMove.INT_SameBytes(oldMain, newMain);
		if (sameMain)
		{
			bool allEqual = true;
			int n = names.Count();
			for (int i = 1; i < n; i++)
			{
				string cn = names.Get(i);
				string co = INT_VPP_FileMove.INT_Join(oldDir, cn);
				string cw = INT_VPP_FileMove.INT_Join(newDir, cn);
				if (FileExist(co) && FileExist(cw))
				{
					if (!INT_VPP_FileMove.INT_SameBytes(co, cw))
						allEqual = false;
				}
			}
			if (allEqual)
			{
				int resFinish = INT_VPP_FileMove.INT_CopyGroup(oldDir, newDir, names, what, " (the copy was already there: unfinished move)");
				return resFinish;
			}
		}
		// (b) the new main is valid: the new place is used, the old files stay as they are
		bool newValid = false;
		if (newHas)
			newValid = INT_VPP_FileMove.INT_FileValid(newMain, check);
		if (newValid)
		{
			INT_VPP_FileMove.INT_Warn(what + ": files in the old AND the new place; the new place is used, the old files are left untouched: " + oldDir);
			return INT_RES_BOTH;
		}
		// (c) the new main is missing or not valid, the old main is valid: the old group replaces the new one
		bool oldValid = false;
		if (oldHas)
			oldValid = INT_VPP_FileMove.INT_FileValid(oldMain, check);
		if (oldValid)
		{
			foreach (string rep : names)
			{
				string ro = INT_VPP_FileMove.INT_Join(oldDir, rep);
				string rw = INT_VPP_FileMove.INT_Join(newDir, rep);
				if (!FileExist(ro) || !FileExist(rw))
					continue;
				string kept = INT_VPP_FileMove.INT_SetAside(rw, "broken");
				if (kept == "")
				{
					INT_VPP_FileMove.INT_Warn(what + ": the unreadable " + rw + " could not be set aside; the old file stays and is used this session: " + oldDir);
					return INT_RES_FAILED;
				}
				Print("[INT_VPP] " + what + ": the new " + rep + " was not readable, set aside as " + kept + "; the old one replaces it");
			}
			int resRepl = INT_VPP_FileMove.INT_CopyGroup(oldDir, newDir, names, what, " (replaces an unreadable new file)");
			return resRepl;
		}
		// (d) neither main is valid: the new place is used (its loader sets the file aside and writes defaults)
		INT_VPP_FileMove.INT_Warn(what + ": neither the old nor the new file is readable; the new place is used (its loader keeps a .broken copy and writes defaults), nothing is deleted: " + oldDir);
		return INT_RES_NEITHER;
	}
}
