// Гнёзда «Древнего корня», сервер. Гнездо — одна точка: сердце, его корни и щупальца. Все гнёзда лежат в nests.json
// (файлы — VB_AR_NestFiles.c). Гнездо без сердца не встаёт никогда (редактор помечает его «без сердца»).
// Живые рощи (ТЗ 2б.4, шаг 5) живут между рестартами: state.json → Groves (гнездо, ступень, очки роста). На старте
// VB_AdvanceGroves считает рост за прошлый рестарт, VB_Activate ставит все живые на их ступени и добирает ростками
// до ActiveNests. Вырезанная роща погибает (из Groves), древняя через AncientLife рестартов уходит в землю.
//
// Сервер ставит объекты обычными сетевыми (сердце должно быть сетевым, иначе его не вырезать),
// без сохранения в мире: после рестарта их нет, сервер выбирает новый набор.
//
// Редактор (клиент: VB_AR_Editor.c, протокол VB_AR_NestNet) шлёт RPC_EDIT на своего игрока, здесь его принимает
// modded PlayerBase внизу файла. Права — SteamID в settings.json → Admins.

class VB_AR_Nests
{
	static const string DIR_MODS = "$profile:VB_Mods";
	static const string DIR = "$profile:VB_Mods\\VB_AncientRoot";
	static const string FILE_SETTINGS = "$profile:VB_Mods\\VB_AncientRoot\\settings.json";
	static const string FILE_NESTS = "$profile:VB_Mods\\VB_AncientRoot\\nests.json";
	static const string FILE_TMP = "$profile:VB_Mods\\VB_AncientRoot\\nests_tmp.json";
	static const string FILE_BACKUP = "$profile:VB_Mods\\VB_AncientRoot\\nests_backup.json";
	static const string FILE_STATE = "$profile:VB_Mods\\VB_AncientRoot\\state.json";
	static const string DEFAULT_ADMIN = "76561198903710848";	// пишется в новый settings.json, если файла нет

	static const int READ_MAX = 100000000;			// как у JsonFileLoader
	static const int DENY_MS = 5000;				// не-админ: один запрос в столько мс, лишнее молча отбрасывается
	static const int RELOAD_ADMINS_MS = 30000;		// список админов с диска — не чаще
	static const int UNDO_KEEP = 64;				// сколько своих последних объектов помнить на админа («Убрать последний»)

	// границы мира для координат из файла и от клиента (Сахал 15360 м; с запасом)
	static const float MAP_MAX = 20000.0;
	static const float Y_MIN = -100.0;
	static const float Y_MAX = 2000.0;
	static const float ORI_MAX = 3600.0;

	protected static ref VB_AR_Nests s_VB_Server;

	ref VB_AR_NestSettings m_VB_Settings;
	ref VB_AR_NestFile m_VB_File;			// null — nests.json с ошибкой: гнёзд нет, файл не перезаписывается
	ref VB_AR_NestState m_VB_State;
	ref map<int, Object> m_VB_Live;			// id объекта -> поставленный объект
	ref TIntArray m_VB_LiveNests;			// id гнёзд, которые стоят сейчас
	ref TStringArray m_VB_Editors;			// SteamID админов с открытым редактором
	ref TIntArray m_VB_Killed;				// гнёзда, чьё сердце вырезано в этой сессии: до рестарта не ставятся снова
	ref TIntArray m_VB_PrevActive;			// гнёзда прошлого рестарта (state.json на старте) — прорастание их избегает
	protected ref map<string, int> m_VB_DenyUntil;			// SteamID не-админа -> до какого времени молчим
	protected ref map<string, ref TIntArray> m_VB_Placed;	// SteamID админа -> id его объектов в этой сессии
	protected int m_VB_AdminsAt;			// раньше этого времени (мс) список админов с диска не перечитывается
	protected bool m_VB_BackedUp;			// nests_backup.json в этой сессии уже сделан
	protected int m_VB_Regrowing;			// сколько прорастаний ждёт таймера
	protected int m_VB_RegrowGen;			// поколение таймеров: «Перебросить» его меняет, старые таймеры молчат

	static VB_AR_Nests VB_Get()
	{
		return s_VB_Server;
	}

	// сервер, из MissionServer.OnInit (VB_AR_MissionServer.c): прочитать файлы и поставить гнёзда
	static void VB_StartServer()
	{
		if (s_VB_Server)
			return;

		s_VB_Server = new VB_AR_Nests();
		s_VB_Server.VB_LoadStart();
		s_VB_Server.VB_AdvanceGroves();
		s_VB_Server.VB_Activate(false);
	}

	// подношение принято (VB_AR_GroveServer.c): копится к росту на следующем рестарте
	static void VB_OnGroveOffer(VB_AR_Heart heart)
	{
		if (s_VB_Server)
			s_VB_Server.VB_GroveEvent(heart, true);
	}

	// сбором подняли волну: в этот рестарт роща не растёт
	static void VB_OnGroveDisturbed(VB_AR_Heart heart)
	{
		if (s_VB_Server)
			s_VB_Server.VB_GroveEvent(heart, false);
	}

	// сердце вырезано (зовёт серверный VB_AR_Heart.VB_KillServer): гнездо до рестарта больше не встанет
	// (ни переброс, ни прорастание не вернут его со свежими спорами, куском сердца и волной), и, если включено,
	// через RegrowMinutes прорастёт другое
	static void VB_OnHeartKilled(VB_AR_Heart heart)
	{
		if (s_VB_Server)
			s_VB_Server.VB_HeartKilled(heart);
	}

	// Семя Древнего корня посажено у вырезанной сердцевины heart: её гнездо встанет первым на следующем рестарте.
	// false — сердцевина не из гнёзд (поставлена руками) или уже засеяна
	static bool VB_OnSeedPlanted(VB_AR_Heart heart)
	{
		if (!s_VB_Server)
			return false;
		return s_VB_Server.VB_SeedHeart(heart);
	}

	protected bool VB_SeedHeart(VB_AR_Heart heart)
	{
		int nestId = VB_NestOfEntity(heart);
		if (nestId == 0 || !m_VB_State)
			return false;
		if (!m_VB_State.Seeded)
			m_VB_State.Seeded = new TIntArray;
		if (m_VB_State.Seeded.Find(nestId) >= 0)
			return false;
		m_VB_State.Seeded.Insert(nestId);
		VB_SaveState();
		VB_AR_Nest nest = VB_FindNest(nestId);
		if (nest)
			Print("[VB_AncientRoot] гнёзда: в " + nest.Name + " посажено Семя — встанет на следующем рестарте");
		return true;
	}

	static void VB_OnEditRpc(PlayerBase player, PlayerIdentity sender, ParamsReadContext ctx)
	{
		if (s_VB_Server && sender)
			s_VB_Server.VB_HandleEdit(sender, ctx);
	}

	void VB_AR_Nests()
	{
		m_VB_Live = new map<int, Object>;
		m_VB_LiveNests = new TIntArray;
		m_VB_Editors = new TStringArray;
		m_VB_Killed = new TIntArray;
		m_VB_PrevActive = new TIntArray;
		m_VB_DenyUntil = new map<string, int>;
		m_VB_Placed = new map<string, ref TIntArray>;
	}

	// --- проверки ---------------------------------------------------------------------------------

	// координаты внутри мира (NaN не проходит ни одно сравнение)
	static bool VB_PosOk(vector p)
	{
		float x = p[0];
		float y = p[1];
		float z = p[2];
		return x >= 0 && x <= MAP_MAX && z >= 0 && z <= MAP_MAX && y >= Y_MIN && y <= Y_MAX;
	}

	static bool VB_OriOk(vector o)
	{
		float a = o[0];
		float b = o[1];
		float c = o[2];
		return a >= -ORI_MAX && a <= ORI_MAX && b >= -ORI_MAX && b <= ORI_MAX && c >= -ORI_MAX && c <= ORI_MAX;
	}

	// объект можно ставить: тип из редактора (сердце, VB_AR_Roots_*, щупальца; scope 2) и координаты в мире
	static bool VB_ObjOk(VB_AR_NestObj o)
	{
		if (!o || !VB_AR_NestUtil.VB_IsNestType(o.Type))
			return false;
		return VB_PosOk(o.Position) && VB_OriOk(o.Orientation);
	}

	// сердце гнезда, если его можно поставить
	protected VB_AR_NestObj VB_GoodHeart(VB_AR_Nest nest)
	{
		VB_AR_NestObj heart = nest.VB_FindHeart();
		if (heart && VB_ObjOk(heart))
			return heart;
		return null;
	}

	// --- файлы ------------------------------------------------------------------------------------

	// settings.json во временный объект: null — файла нет (broken = false) или он не разобрался (broken = true).
	// forWrite — для записи одного поля: без пометок «не задано» и без нормализации, поля как в файле
	protected VB_AR_NestSettings VB_ReadSettings(out bool broken, bool forWrite = false)
	{
		broken = false;
		if (!FileExist(FILE_SETTINGS))
			return null;

		VB_AR_NestSettings s = new VB_AR_NestSettings();
		if (!forWrite)
			s.VB_MarkUnset();
		string err;
		if (!JsonFileLoader<VB_AR_NestSettings>.LoadFile(FILE_SETTINGS, s, err) || !s)
		{
			broken = true;
			Print("[VB_AncientRoot] гнёзда: ОШИБКА в settings.json: " + err);
			return null;
		}
		if (forWrite)
		{
			if (!s.Admins)
				s.Admins = new TStringArray;
		}
		else
		{
			s.VB_Norm();
		}
		return s;
	}

	protected bool VB_WriteSettings(VB_AR_NestSettings s)
	{
		string err;
		if (!JsonFileLoader<VB_AR_NestSettings>.SaveFile(FILE_SETTINGS, s, err))
		{
			Print("[VB_AncientRoot] гнёзда: не удалось записать settings.json: " + err);
			return false;
		}
		return true;
	}

	// nests.json во временный объект; enabledKeys — сколько раз в тексте встречается ключ "Enabled"
	// (JSON без ключа даёт гнезду Enabled = 0, E57 — так видно, у скольких гнёзд ключа нет)
	protected VB_AR_NestFile VB_ReadNests(out bool broken, out int enabledKeys)
	{
		broken = false;
		enabledKeys = 0;
		if (!FileExist(FILE_NESTS))
			return null;

		FileHandle fh = OpenFile(FILE_NESTS, FileMode.READ);
		if (fh == 0)
		{
			broken = true;
			Print("[VB_AncientRoot] гнёзда: nests.json не открывается на чтение");
			return null;
		}
		string text;
		ReadFile(fh, text, READ_MAX);
		CloseFile(fh);

		VB_AR_NestFile f = new VB_AR_NestFile();
		string err;
		if (!JsonFileLoader<VB_AR_NestFile>.LoadData(text, f, err) || !f)
		{
			broken = true;
			Print("[VB_AncientRoot] гнёзда: ОШИБКА в nests.json: " + err);
			return null;
		}
		enabledKeys = VB_CountKey(text, "\"Enabled\"");
		return f;
	}

	protected static int VB_CountKey(string text, string key)
	{
		int n = 0;
		int len = key.Length();
		int at = text.IndexOf(key);
		while (at >= 0)
		{
			n++;
			at = text.IndexOfFrom(at + len, key);
		}
		return n;
	}

	protected VB_AR_NestState VB_ReadState()
	{
		if (!FileExist(FILE_STATE))
			return null;

		VB_AR_NestState s = new VB_AR_NestState();
		string err;
		if (!JsonFileLoader<VB_AR_NestState>.LoadFile(FILE_STATE, s, err) || !s)
		{
			Print("[VB_AncientRoot] гнёзда: state.json не прочитан (гнёзда прошлого рестарта не учитываются): " + err);
			return null;
		}
		if (!s.LastActive)
			s.LastActive = new TIntArray;
		return s;
	}

	protected void VB_LoadStart()
	{
		if (!FileExist(DIR_MODS))
			MakeDirectory(DIR_MODS);
		if (!FileExist(DIR))
			MakeDirectory(DIR);

		bool broken;
		VB_AR_NestSettings s = VB_ReadSettings(broken);
		if (s)
		{
			m_VB_Settings = s;
		}
		else
		{
			m_VB_Settings = new VB_AR_NestSettings();
			m_VB_Settings.Admins.Insert(DEFAULT_ADMIN);
			m_VB_Settings.VB_Norm();
			if (broken)
			{
				Print("[VB_AncientRoot] гнёзда: settings.json с ошибкой — до исправления настройки по умолчанию, файл не перезаписан");
			}
			else
			{
				VB_WriteSettings(m_VB_Settings);
				Print("[VB_AncientRoot] гнёзда: создан " + FILE_SETTINGS);
			}
		}
		m_VB_AdminsAt = g_Game.GetTime() + RELOAD_ADMINS_MS;

		// прошлый запуск мог упасть между удалением старого nests.json и копированием нового
		if (!FileExist(FILE_NESTS) && FileExist(FILE_TMP))
		{
			if (CopyFile(FILE_TMP, FILE_NESTS))
				Print("[VB_AncientRoot] гнёзда: nests.json восстановлен из nests_tmp.json (прошлая запись прервалась)");
		}

		if (!FileExist(FILE_NESTS))
		{
			m_VB_File = new VB_AR_NestFile();
			m_VB_BackedUp = true;		// копировать нечего
			VB_SaveNests();
			Print("[VB_AncientRoot] гнёзда: создан пустой " + FILE_NESTS);
		}
		else
		{
			int enabledKeys;
			VB_AR_NestFile f = VB_ReadNests(broken, enabledKeys);
			if (!f)
			{
				m_VB_File = null;
				Print("[VB_AncientRoot] гнёзда: nests.json не прочитан — гнёзда не поставлены, файл не перезаписывается, правка выключена. Исправь файл (копия прошлого старта — nests_backup.json) и нажми в редакторе «Перебросить гнёзда»");
			}
			else
			{
				VB_BackupNests();		// копия на каждый старт, до любых правок
				m_VB_File = f;
				VB_FixFile(enabledKeys);
			}
		}

		m_VB_State = VB_ReadState();
		if (!m_VB_State)
			m_VB_State = new VB_AR_NestState();
		if (!m_VB_State.Seeded)
			m_VB_State.Seeded = new TIntArray;		// state.json прошлых версий — без ключа
		if (!m_VB_State.Groves)
			m_VB_State.Groves = new array<ref VB_AR_Grove>;
		for (int gi = m_VB_State.Groves.Count() - 1; gi >= 0; gi--)
		{
			VB_AR_Grove lg = m_VB_State.Groves[gi];
			if (!lg || lg.Nest == 0)
			{
				m_VB_State.Groves.Remove(gi);
				continue;
			}
			lg.Stage = Math.Clamp(lg.Stage, VB_AR_Heart.STAGE_SPROUT, VB_AR_Heart.STAGE_ANCIENT);	// без ключа — 0 (E57)
			if (lg.Growth < 0)
				lg.Growth = 0;
			if (lg.Age < 0)
				lg.Age = 0;
		}
		m_VB_PrevActive.Copy(m_VB_State.LastActive);
	}

	// «Перебросить гнёзда»: перечитать файлы с диска (их можно поправить руками без рестарта). Каждый файл —
	// во временный объект: с ошибкой остаются прежние настройки/гнёзда. Вернёт пояснение для админа ("" — всё прочитано).
	protected string VB_ReloadFromDisk()
	{
		string note = "";
		bool broken;
		VB_AR_NestSettings s = VB_ReadSettings(broken);
		if (s)
			m_VB_Settings = s;
		else if (broken)
			note = "settings.json с ошибкой — остались прежние настройки";
		m_VB_AdminsAt = g_Game.GetTime() + RELOAD_ADMINS_MS;

		int enabledKeys;
		VB_AR_NestFile f = VB_ReadNests(broken, enabledKeys);
		if (f)
		{
			if (!m_VB_BackedUp)
				VB_BackupNests();	// на старте файл был с ошибкой: копия исправленного до правок
			m_VB_File = f;
			VB_FixFile(enabledKeys);
		}
		else
		{
			if (note != "")
				note += "; ";
			if (broken && m_VB_File)
				note += "nests.json с ошибкой — остались гнёзда из памяти";
			else if (broken)
				note += "nests.json с ошибкой — гнёзд нет, правка выключена";
			else if (m_VB_File)
				note += "nests.json нет на диске — остались гнёзда из памяти";
			else
			{
				m_VB_File = new VB_AR_NestFile();
				VB_SaveNests();
				note += "nests.json нет на диске — создан пустой";
			}
			if (broken)
				Print("[VB_AncientRoot] гнёзда: nests.json не прочитался при перебросе: " + note);
		}

		VB_AR_NestState st = VB_ReadState();
		if (st)
			m_VB_State = st;
		return note;
	}

	// Только список админов с диска (админа можно дописать или убрать без рестарта). Остальные поля живого сервера
	// не меняются, файл с ошибкой или без Admins ничего не сбрасывает.
	protected void VB_ReloadAdmins()
	{
		bool broken;
		VB_AR_NestSettings s = VB_ReadSettings(broken);
		if (!s || !s.Admins)
			return;
		m_VB_Settings.Admins = s.Admins;
	}

	// не чаще раза в RELOAD_ADMINS_MS: перечитать админов и выкинуть из редакторов снятых и вышедших
	protected void VB_RefreshAdmins(int now)
	{
		if (now < m_VB_AdminsAt)
			return;
		m_VB_AdminsAt = now + RELOAD_ADMINS_MS;
		VB_ReloadAdmins();
		VB_EditorPlayers();
	}

	// После загрузки: пустые списки, null-элементы (лишняя запятая), ключи, которых нет (E57), повторные и нулевые id,
	// счётчики; объекты, которые не будут ставиться (в лог); порядок «одно сердце на гнездо». Изменилось — записать.
	protected void VB_FixFile(int enabledKeys)
	{
		bool changed = false;
		if (!m_VB_File.Nests)
			m_VB_File.Nests = new array<ref VB_AR_Nest>;
		if (!m_VB_File.Decor)
			m_VB_File.Decor = new array<ref VB_AR_NestObj>;
		if (m_VB_File.Version < 1)
			m_VB_File.Version = 1;

		int k;
		int j;
		VB_AR_Nest nest;
		for (k = m_VB_File.Nests.Count() - 1; k >= 0; k--)
		{
			nest = m_VB_File.Nests[k];
			if (!nest)
			{
				m_VB_File.Nests.RemoveOrdered(k);
				changed = true;
				continue;
			}
			if (!nest.Objects)
				nest.Objects = new array<ref VB_AR_NestObj>;
			for (j = nest.Objects.Count() - 1; j >= 0; j--)
			{
				if (!nest.Objects[j])
				{
					nest.Objects.RemoveOrdered(j);
					changed = true;
				}
			}
		}
		for (k = m_VB_File.Decor.Count() - 1; k >= 0; k--)
		{
			if (!m_VB_File.Decor[k])
			{
				m_VB_File.Decor.RemoveOrdered(k);
				changed = true;
			}
		}

		// E57: гнездо без ключа Enabled читается выключенным. Редактор пишет ключ всегда; файл совсем без ключей
		// (написан руками) — все включены; если ключа нет только у части — в лог, какие гнёзда сейчас выключены
		int nestCount = m_VB_File.Nests.Count();
		if (nestCount > 0 && enabledKeys == 0)
		{
			foreach (VB_AR_Nest en : m_VB_File.Nests)
			{
				en.Enabled = 1;
			}
			changed = true;
			Print("[VB_AncientRoot] гнёзда: в nests.json нет ключей Enabled — все гнёзда включены");
		}
		else if (enabledKeys < nestCount)
		{
			string off = "";
			foreach (VB_AR_Nest dn : m_VB_File.Nests)
			{
				if (dn.Enabled == 0)
					off += " «" + dn.Name + "»";
			}
			int missing = nestCount - enabledKeys;
			Print("[VB_AncientRoot] гнёзда: у " + missing.ToString() + " гнёзд в nests.json нет ключа Enabled — они выключены, пока не впишешь \"Enabled\": 1. Выключены:" + off);
		}
		foreach (VB_AR_Nest bn : m_VB_File.Nests)
		{
			if (bn.Enabled != 0 && bn.Enabled != 1)
			{
				bn.Enabled = 1;
				changed = true;
			}
		}

		int maxId = 0;
		foreach (VB_AR_NestObj dm : m_VB_File.Decor)
		{
			maxId = Math.Max(maxId, dm.Id);
		}
		foreach (VB_AR_Nest mn : m_VB_File.Nests)
		{
			maxId = Math.Max(maxId, mn.Id);
			foreach (VB_AR_NestObj mo : mn.Objects)
			{
				maxId = Math.Max(maxId, mo.Id);
			}
		}
		if (m_VB_File.NextId <= maxId)
		{
			m_VB_File.NextId = maxId + 1;
			changed = true;
		}
		if (m_VB_File.NextNumber < 1)
		{
			m_VB_File.NextNumber = nestCount + 1;
			changed = true;
		}

		set<int> seen = new set<int>;
		foreach (VB_AR_Nest n : m_VB_File.Nests)
		{
			if (n.Id <= 0 || seen.Find(n.Id) >= 0)
			{
				n.Id = VB_TakeId();
				changed = true;
			}
			seen.Insert(n.Id);
			foreach (VB_AR_NestObj obj : n.Objects)
			{
				if (obj.Id <= 0 || seen.Find(obj.Id) >= 0)
				{
					obj.Id = VB_TakeId();
					changed = true;
				}
				seen.Insert(obj.Id);
			}
			if (n.Name == "")
			{
				n.Name = VB_TakeName();
				changed = true;
			}
		}
		foreach (VB_AR_NestObj dobj : m_VB_File.Decor)
		{
			if (dobj.Id <= 0 || seen.Find(dobj.Id) >= 0)
			{
				dobj.Id = VB_TakeId();
				changed = true;
			}
			seen.Insert(dobj.Id);
		}

		VB_LogBadObjects();

		if (VB_Regroup())
		{
			Print("[VB_AncientRoot] гнёзда: перегруппированы при загрузке (одно сердце на гнездо, корни к ближайшему сердцу)");
			changed = true;
		}
		if (changed)
			VB_SaveNests();
	}

	// Объекты с типом не из редактора (руками вписан чужой класс, абстрактная база, обвивка/корни на домах)
	// или с координатами вне мира остаются в файле, но не ставятся (VB_SpawnObj)
	protected void VB_LogBadObjects()
	{
		int bad = 0;
		string sample = "";
		foreach (VB_AR_Nest nest : m_VB_File.Nests)
		{
			foreach (VB_AR_NestObj o : nest.Objects)
			{
				if (VB_ObjOk(o))
					continue;
				bad++;
				if (bad <= 10)
					sample += " " + o.Id.ToString() + ":" + o.Type;
			}
		}
		foreach (VB_AR_NestObj d : m_VB_File.Decor)
		{
			if (VB_ObjOk(d))
				continue;
			bad++;
			if (bad <= 10)
				sample += " " + d.Id.ToString() + ":" + d.Type;
		}
		if (bad > 0)
			Print("[VB_AncientRoot] гнёзда: в nests.json " + bad.ToString() + " объектов не будут ставиться (тип не из редактора или координаты вне мира):" + sample);
	}

	// копия nests.json как есть на диске (DeleteFile перед CopyFile: CopyFile поверх не пишет)
	protected void VB_BackupNests()
	{
		m_VB_BackedUp = true;
		if (!FileExist(FILE_NESTS))
			return;
		if (FileExist(FILE_BACKUP))
			DeleteFile(FILE_BACKUP);
		if (!CopyFile(FILE_NESTS, FILE_BACKUP))
			Print("[VB_AncientRoot] гнёзда: не удалось сделать nests_backup.json");
	}

	// Запись через временный файл: сначала целиком nests_tmp.json, потом он заменяет nests.json. Оборвалось
	// посередине — на диске остаётся либо старый nests.json, либо полный nests_tmp.json (его подхватит старт).
	protected bool VB_SaveNests()
	{
		if (!m_VB_File)
			return false;
		if (!m_VB_BackedUp)
			VB_BackupNests();

		string err;
		if (!JsonFileLoader<VB_AR_NestFile>.SaveFile(FILE_TMP, m_VB_File, err))
		{
			Print("[VB_AncientRoot] гнёзда: не удалось записать nests_tmp.json: " + err);
			return false;
		}
		if (FileExist(FILE_NESTS) && !DeleteFile(FILE_NESTS))
		{
			Print("[VB_AncientRoot] гнёзда: nests.json не удалось заменить (занят?), свежая версия — в nests_tmp.json");
			return false;
		}
		if (!CopyFile(FILE_TMP, FILE_NESTS))
		{
			Print("[VB_AncientRoot] гнёзда: nests.json не записан, свежая версия — в nests_tmp.json");
			return false;
		}
		DeleteFile(FILE_TMP);
		return true;
	}

	protected void VB_SaveState()
	{
		string err;
		if (!JsonFileLoader<VB_AR_NestState>.SaveFile(FILE_STATE, m_VB_State, err))
			Print("[VB_AncientRoot] гнёзда: не удалось записать state.json: " + err);
	}

	// новый id объекта или гнезда (без x++ в выражении: в Enforce на это не полагаемся)
	protected int VB_TakeId()
	{
		int id = m_VB_File.NextId;
		m_VB_File.NextId = id + 1;
		return id;
	}

	protected string VB_TakeName()
	{
		int num = m_VB_File.NextNumber;
		m_VB_File.NextNumber = num + 1;
		return "Гнездо " + num.ToString();
	}

	protected VB_AR_Nest VB_NewNest()
	{
		VB_AR_Nest nest = new VB_AR_Nest();
		nest.Id = VB_TakeId();
		nest.Name = VB_TakeName();
		m_VB_File.Nests.Insert(nest);
		return nest;
	}

	// --- порядок в гнёздах ------------------------------------------------------------------------

	// Порядок: в гнезде одно сердце (лишние уходят в свои новые гнёзда), каждый корень и щупальце — в гнезде
	// ближайшего сердца не дальше NEST_RADIUS; щупальца вдали от сердец — в постоянный декор; опустевшие гнёзда
	// удаляются. Сердца ищутся по сетке VB_AR_NestHeartIndex, тип — по кэшу VB_AR_NestUtil (аудит №38).
	// true — что-то поменялось. После него — VB_SyncLive (что снять с карты) и VB_SpawnDecor.
	protected bool VB_Regroup()
	{
		bool changed = false;

		// 1. одно сердце на гнездо (перебор по копии: новые гнёзда добавляются в m_VB_File.Nests)
		array<VB_AR_Nest> nests = new array<VB_AR_Nest>;
		foreach (VB_AR_Nest n : m_VB_File.Nests)
		{
			nests.Insert(n);
		}
		bool hasHeart;
		array<ref VB_AR_NestObj> extra = new array<ref VB_AR_NestObj>;
		foreach (VB_AR_Nest nest : nests)
		{
			extra.Clear();
			hasHeart = false;
			foreach (VB_AR_NestObj o : nest.Objects)
			{
				if (!VB_AR_NestUtil.VB_IsHeartType(o.Type))
					continue;
				if (hasHeart)
					extra.Insert(o);
				hasHeart = true;
			}
			foreach (VB_AR_NestObj h : extra)
			{
				VB_AR_Nest split = VB_NewNest();
				split.Objects.Insert(h);
				nest.Objects.RemoveItem(h);
				changed = true;
			}
		}
		extra.Clear();

		// 2. корни и щупальца — к ближайшему сердцу; щупальца без сердца рядом — в постоянный декор
		VB_AR_NestHeartIndex index = new VB_AR_NestHeartIndex();
		index.VB_Build(m_VB_File.Nests);
		nests.Clear();
		foreach (VB_AR_Nest n2 : m_VB_File.Nests)
		{
			nests.Insert(n2);
		}
		VB_AR_Nest best;
		VB_AR_NestObj obj;
		foreach (VB_AR_Nest from : nests)
		{
			for (int i = from.Objects.Count() - 1; i >= 0; i--)
			{
				obj = from.Objects[i];
				if (VB_AR_NestUtil.VB_IsHeartType(obj.Type))
					continue;

				best = index.VB_Nearest(obj.Position, VB_AR_NestNet.NEST_RADIUS);
				if (best && best != from)
				{
					best.Objects.Insert(obj);		// сначала в новое гнездо (ref), потом из старого
					from.Objects.RemoveOrdered(i);
					changed = true;
				}
				else if (!best && VB_AR_NestUtil.VB_IsHangType(obj.Type))
				{
					m_VB_File.Decor.Insert(obj);
					from.Objects.RemoveOrdered(i);
					changed = true;
				}
			}
		}

		// 2б. щупальце декора, рядом с которым появилось сердце, уходит в его гнездо
		for (int di = m_VB_File.Decor.Count() - 1; di >= 0; di--)
		{
			obj = m_VB_File.Decor[di];
			best = index.VB_Nearest(obj.Position, VB_AR_NestNet.NEST_RADIUS);
			if (best)
			{
				best.Objects.Insert(obj);
				m_VB_File.Decor.RemoveOrdered(di);
				changed = true;
			}
		}

		// 3. пустые гнёзда убрать (в них нет сердца — сетка на них не ссылается)
		for (int k = m_VB_File.Nests.Count() - 1; k >= 0; k--)
		{
			if (m_VB_File.Nests[k].Objects.Count() == 0)
			{
				m_VB_LiveNests.RemoveItem(m_VB_File.Nests[k].Id);
				m_VB_File.Nests.RemoveOrdered(k);
				changed = true;
			}
		}
		return changed;
	}

	// После перегруппировки или удаления сердца: на карте остаются только объекты живых гнёзд, у которых есть сердце,
	// и постоянный декор. Корни, ушедшие в невыбранное гнездо, и корни гнезда без сердца снимаются.
	protected void VB_SyncLive()
	{
		set<int> keep = new set<int>;
		if (m_VB_File)
		{
			foreach (VB_AR_NestObj d : m_VB_File.Decor)
			{
				keep.Insert(d.Id);
			}
		}

		VB_AR_Nest nest;
		for (int i = m_VB_LiveNests.Count() - 1; i >= 0; i--)
		{
			nest = VB_FindNest(m_VB_LiveNests[i]);
			if (!nest || !nest.VB_HasHeart())
			{
				m_VB_LiveNests.Remove(i);
				continue;
			}
			foreach (VB_AR_NestObj o : nest.Objects)
			{
				keep.Insert(o.Id);
			}
		}

		TIntArray drop = new TIntArray;
		foreach (int id, Object live : m_VB_Live)
		{
			if (keep.Find(id) < 0)
				drop.Insert(id);
		}
		foreach (int dropId : drop)
		{
			VB_DespawnObj(dropId);
		}
	}

	// --- рост рощ (ТЗ 2б.4, шаг 5) ----------------------------------------------------------------

	protected VB_AR_Grove VB_GroveOf(int nestId)
	{
		if (!m_VB_State || !m_VB_State.Groves)
			return null;
		foreach (VB_AR_Grove g : m_VB_State.Groves)
		{
			if (g && g.Nest == nestId)
				return g;
		}
		return null;
	}

	protected VB_AR_Grove VB_NewGrove(int nestId)
	{
		VB_AR_Grove g = VB_GroveOf(nestId);
		if (g)
			return g;
		g = new VB_AR_Grove();
		g.Nest = nestId;
		g.Stage = VB_AR_Heart.STAGE_SPROUT;
		m_VB_State.Groves.Insert(g);
		return g;
	}

	protected void VB_DropGrove(int nestId)
	{
		for (int i = m_VB_State.Groves.Count() - 1; i >= 0; i--)
		{
			if (m_VB_State.Groves[i] && m_VB_State.Groves[i].Nest == nestId)
				m_VB_State.Groves.Remove(i);
		}
	}

	protected string VB_NestName(int nestId)
	{
		VB_AR_Nest nest = VB_FindNest(nestId);
		if (nest)
			return nest.Name;
		return "гнездо " + nestId.ToString();
	}

	static string VB_StageName(int stage)
	{
		if (stage <= VB_AR_Heart.STAGE_SPROUT)
			return "росток";
		if (stage >= VB_AR_Heart.STAGE_ANCIENT)
			return "древняя";
		return "роща";
	}

	// Старт сервера: прошлый рестарт живые рощи пережили. Древние стареют и через AncientLife уходят в землю;
	// остальным +1 очко (+1 за FeedOffers подношений, 0 — если сбором подняли волну); набрала GrowRestarts — ступень
	// вверх (за рестарт не больше одной). В древние — не больше MaxAncient, первыми — с большим запасом очков,
	// остальные ждут на ступени «роща». Потом счётчики рестарта обнуляются.
	void VB_AdvanceGroves()
	{
		if (!m_VB_State || !m_VB_Settings)
			return;
		array<ref VB_AR_Grove> gs = m_VB_State.Groves;
		int grow = m_VB_Settings.GrowRestarts;
		string log = "";
		VB_AR_Grove g;
		int i;
		int ancient = 0;

		for (i = gs.Count() - 1; i >= 0; i--)
		{
			g = gs[i];
			if (g.Stage < VB_AR_Heart.STAGE_ANCIENT)
				continue;
			g.Age = g.Age + 1;
			if (g.Age >= m_VB_Settings.AncientLife)
			{
				log += " " + VB_NestName(g.Nest) + " ушла в землю;";
				gs.Remove(i);
				continue;
			}
			ancient++;
		}

		int pts;
		foreach (VB_AR_Grove gp : gs)
		{
			if (gp.Stage >= VB_AR_Heart.STAGE_ANCIENT)
				continue;
			pts = 1;
			if (gp.Offers >= m_VB_Settings.FeedOffers)
				pts = pts + 1;
			if (gp.Disturbed != 0)
				pts = 0;
			gp.Growth = gp.Growth + pts;
		}

		// росток -> роща
		foreach (VB_AR_Grove gs1 : gs)
		{
			if (gs1.Stage == VB_AR_Heart.STAGE_SPROUT && gs1.Growth >= grow)
			{
				gs1.Stage = VB_AR_Heart.STAGE_GROVE;
				gs1.Growth = gs1.Growth - grow;
				log += " " + VB_NestName(gs1.Nest) + " выросла в рощу;";
			}
		}

		// роща -> древняя: по лимиту, первыми — с большим запасом очков; кто не прошёл — ждёт с полным запасом
		VB_AR_Grove best;
		while (true)
		{
			best = null;
			foreach (VB_AR_Grove gs2 : gs)
			{
				if (gs2.Stage != VB_AR_Heart.STAGE_GROVE || gs2.Growth < grow)
					continue;
				if (!best || gs2.Growth > best.Growth)
					best = gs2;
			}
			if (!best)
				break;
			if (ancient < m_VB_Settings.MaxAncient)
			{
				best.Stage = VB_AR_Heart.STAGE_ANCIENT;
				best.Growth = 0;
				best.Age = 0;
				ancient++;
				log += " " + VB_NestName(best.Nest) + " стала древней;";
			}
			else
			{
				best.Growth = grow - 1;		// ждёт места: на следующем старте снова кандидат
			}
		}

		foreach (VB_AR_Grove gz : gs)
		{
			gz.Offers = 0;
			gz.Disturbed = 0;
		}
		VB_SaveState();
		int liveN = gs.Count();
		if (log == "")
			log = " без перемен";
		Print("[VB_AncientRoot] рощи: живых " + liveN.ToString() + ", рост за рестарт:" + log);
	}

	// подношение (offer) или волна от сбора у сердцевины heart — в счётчики рестарта её рощи
	protected void VB_GroveEvent(VB_AR_Heart heart, bool offer)
	{
		int nestId = VB_NestOfEntity(heart);
		if (nestId == 0)
			return;
		VB_AR_Grove g = VB_GroveOf(nestId);
		if (!g)
			return;
		if (offer)
		{
			g.Offers = g.Offers + 1;
		}
		else
		{
			if (g.Disturbed != 0)
				return;
			g.Disturbed = 1;
			Print("[VB_AncientRoot] рощи: " + VB_NestName(nestId) + " потревожена сбором — в этот рестарт не растёт");
		}
		VB_SaveState();
	}

	// ступень живой рощи гнезда (сердце не из живой рощи — например, поставлено редактором — роща)
	protected int VB_StageOfNest(int nestId)
	{
		VB_AR_Grove g = VB_GroveOf(nestId);
		if (g)
			return g.Stage;
		return VB_AR_Heart.STAGE_GROVE;
	}

	// --- выбор и установка ------------------------------------------------------------------------

	// позиции игроков на сервере (новое гнездо у игрока не встаёт)
	protected array<vector> VB_PlayerSpots()
	{
		array<vector> spots = new array<vector>;
		array<Man> players = new array<Man>;
		g_Game.GetPlayers(players);
		foreach (Man someone : players)
		{
			if (someone)
				spots.Insert(someone.GetPosition());
		}
		return spots;
	}

	protected bool VB_PlayerNear(vector c, array<vector> spots)
	{
		foreach (vector p : spots)
		{
			if (VB_AR_NestUtil.VB_DistXZ(c, p) < VB_AR_NestNet.NEST_RADIUS)
				return true;
		}
		return false;
	}

	// снять всё, что стоит, и выбрать гнёзда заново, как на рестарте; avoidPlayers — не ставить гнездо, если в
	// NEST_RADIUS от его сердца игрок (переброс посреди рестарта). Гнёзда с вырезанным в этой сессии сердцем не встают.
	void VB_Activate(bool avoidPlayers)
	{
		VB_DespawnNests();
		if (!m_VB_File)
			return;

		array<VB_AR_Nest> hearts = new array<VB_AR_Nest>;
		int noHeart = 0;
		int killed = 0;
		foreach (VB_AR_Nest nest : m_VB_File.Nests)
		{
			if (nest.Enabled == 0 || nest.Objects.Count() == 0)
				continue;
			if (m_VB_Killed.Find(nest.Id) >= 0)
			{
				killed++;
				continue;
			}
			if (VB_GoodHeart(nest))
				hearts.Insert(nest);
			else
				noHeart++;
		}

		TIntArray avoid = new TIntArray;
		if (m_VB_Settings.NoRepeat == 1)
			avoid.Copy(m_VB_State.LastActive);

		array<vector> spots = null;
		if (avoidPlayers)
			spots = VB_PlayerSpots();

		array<VB_AR_Nest> chosen = new array<VB_AR_Nest>;
		array<vector> centers = new array<vector>;

		// живые рощи (ТЗ 2б.4) — на своих местах; гнездо удалено, выключено или без сердца — роща пропала
		VB_AR_Nest gn;
		VB_AR_Grove lg;
		for (int gi = 0; gi < m_VB_State.Groves.Count(); gi++)
		{
			lg = m_VB_State.Groves[gi];
			gn = VB_FindNest(lg.Nest);
			if (!gn || hearts.Find(gn) < 0)
			{
				Print("[VB_AncientRoot] рощи: " + VB_NestName(lg.Nest) + " — гнезда нет или оно выключено, роща пропала");
				m_VB_State.Groves.Remove(gi);
				gi--;
				continue;
			}
			if (chosen.Count() >= m_VB_Settings.ActiveNests)
			{
				Print("[VB_AncientRoot] рощи: " + gn.Name + " — живых больше ActiveNests, роща ушла");
				m_VB_State.Groves.Remove(gi);
				gi--;
				continue;
			}
			chosen.Insert(gn);
			centers.Insert(gn.VB_Center());
		}

		// посаженные Семенем (ТЗ 2б.11) — ростками первыми; забытые из списка, только если встали
		int seededN = 0;
		if (m_VB_State.Seeded && m_VB_State.Seeded.Count() > 0)
		{
			foreach (VB_AR_Nest sn : hearts)
			{
				if (chosen.Count() >= m_VB_Settings.ActiveNests)
					break;
				if (m_VB_State.Seeded.Find(sn.Id) < 0 || chosen.Find(sn) >= 0)
					continue;
				chosen.Insert(sn);
				centers.Insert(sn.VB_Center());
				m_VB_State.Seeded.RemoveItem(sn.Id);
				VB_NewGrove(sn.Id);
				seededN++;
			}
		}
		// добрать ростками до ActiveNests
		int before = chosen.Count();
		VB_Pick(hearts, m_VB_Settings.ActiveNests - chosen.Count(), avoid, centers, chosen, spots);
		for (int ni = before; ni < chosen.Count(); ni++)
		{
			VB_NewGrove(chosen[ni].Id);
		}
		if (seededN > 0)
			Print("[VB_AncientRoot] гнёзда: проросли посаженные Семенем: " + seededN.ToString());

		string names = "";
		int stage;
		m_VB_State.LastActive.Clear();
		foreach (VB_AR_Nest pick : chosen)
		{
			stage = VB_StageOfNest(pick.Id);
			VB_SpawnNest(pick, stage);
			m_VB_State.LastActive.Insert(pick.Id);
			if (names != "")
				names += ", ";
			names += pick.Name + " (" + VB_StageName(stage) + ")";
		}
		VB_SaveState();

		VB_SpawnDecor();
		// каждое число — в свою переменную до Format: несколько Count в одном списке аргументов затирают друг друга (E71)
		int chosenN = chosen.Count();
		int poolN = hearts.Count();
		int liveN = m_VB_Live.Count();
		int decorN = m_VB_File.Decor.Count();
		Print(string.Format("[VB_AncientRoot] гнёзда: поставлено %1 из %2 (%3 объектов вместе с декором, декора %4): %5", chosenN, poolN, liveN, decorN, names));
		if (noHeart > 0)
			Print("[VB_AncientRoot] гнёзда: без сердца, не ставятся: " + noHeart.ToString());
		if (killed > 0)
			Print("[VB_AncientRoot] гнёзда: с вырезанным сердцем, до рестарта не ставятся: " + killed.ToString());
	}

	// count гнёзд из pool: в случайном порядке, не ближе MinNestDistance к уже выбранным (centers), не у игроков
	// (spots, если не null); сначала без гнёзд из avoid, если их не хватает — и с ними
	protected void VB_Pick(array<VB_AR_Nest> pool, int count, TIntArray avoid, array<vector> centers, array<VB_AR_Nest> chosen, array<vector> spots)
	{
		array<VB_AR_Nest> cand = new array<VB_AR_Nest>;
		cand.Copy(pool);
		cand.ShuffleArray();

		int got = 0;
		vector c;
		for (int pass = 0; pass < 2 && got < count; pass++)
		{
			foreach (VB_AR_Nest nest : cand)
			{
				if (got >= count)
					break;
				if (chosen.Find(nest) >= 0)
					continue;
				if (pass == 0 && avoid.Find(nest.Id) >= 0)
					continue;

				c = nest.VB_Center();
				if (!VB_FarEnough(c, centers))
					continue;
				if (spots && VB_PlayerNear(c, spots))
					continue;

				chosen.Insert(nest);
				centers.Insert(c);
				got++;
			}
		}
	}

	protected bool VB_FarEnough(vector c, array<vector> centers)
	{
		foreach (vector other : centers)
		{
			if (VB_AR_NestUtil.VB_DistXZ(c, other) < m_VB_Settings.MinNestDistance)
				return false;
		}
		return true;
	}

	// По ступени рощи (ТЗ 2б.4): росток — сердце и 30–40 % корней, без щупалец; роща — сердце, щупальца и
	// RootsPercentMin..Max корней; древняя — всё. Ступень сердцу ставит VB_SpawnObj (по живой роще гнезда).
	protected void VB_SpawnNest(VB_AR_Nest nest, int stage = 2)
	{
		if (m_VB_LiveNests.Find(nest.Id) >= 0)
			return;

		float lo = Math.Clamp(m_VB_Settings.RootsPercentMin * 0.01, 0.0, 1.0);
		float hi = Math.Clamp(m_VB_Settings.RootsPercentMax * 0.01, lo, 1.0);
		if (stage <= VB_AR_Heart.STAGE_SPROUT)
		{
			lo = 0.3;
			hi = 0.4;
		}
		else if (stage >= VB_AR_Heart.STAGE_ANCIENT)
		{
			lo = 1.0;
			hi = 1.0;
		}
		float keep = Math.RandomFloat(lo, hi);

		array<VB_AR_NestObj> rest = new array<VB_AR_NestObj>;
		foreach (VB_AR_NestObj o : nest.Objects)
		{
			if (VB_AR_NestUtil.VB_IsHeartType(o.Type))
				VB_SpawnObj(o);		// сердце — всегда
			else if (VB_AR_NestUtil.VB_IsHangType(o.Type))
			{
				if (stage > VB_AR_Heart.STAGE_SPROUT)
					VB_SpawnObj(o);	// щупальца (их ставят поштучно) — у рощи и древней все, у ростка нет
			}
			else
				rest.Insert(o);
		}

		rest.ShuffleArray();
		int n = Math.Round(rest.Count() * keep);
		if (rest.Count() > 0 && n < 1)
			n = 1;
		for (int i = 0; i < n; i++)
		{
			VB_SpawnObj(rest[i]);
		}

		m_VB_LiveNests.Insert(nest.Id);
	}

	protected Object VB_SpawnObj(VB_AR_NestObj o)
	{
		if (m_VB_Live.Contains(o.Id))
			return m_VB_Live.Get(o.Id);
		// тип не из редактора или координаты вне мира — не ставим (в лог при загрузке, VB_LogBadObjects)
		if (!VB_ObjOk(o))
			return null;

		int flags = ECE_SETUP | ECE_CREATEPHYSICS | ECE_UPDATEPATHGRAPH | ECE_KEEPHEIGHT | ECE_NOLIFETIME | ECE_NOPERSISTENCY_WORLD;
		Object obj = g_Game.CreateObjectEx(o.Type, o.Position, flags);
		if (!obj)
		{
			string at = o.Position.ToString();
			Print("[VB_AncientRoot] гнёзда: не создался " + o.Type + " в " + at);
			return null;
		}

		VB_AR_Base grove = VB_AR_Base.Cast(obj);
		if (grove)
		{
			grove.VB_PlaceAt(o.Position, o.Orientation);
		}
		else
		{
			obj.SetPosition(o.Position);
			obj.SetOrientation(o.Orientation);
		}
		obj.Update();
		m_VB_Live.Set(o.Id, obj);

		VB_AR_Heart heart = VB_AR_Heart.Cast(obj);
		if (heart)
		{
			VB_AR_Nest owner = VB_FindNestOfObj(o.Id);
			int stage = VB_AR_Heart.STAGE_GROVE;
			if (owner)
				stage = VB_StageOfNest(owner.Id);
			heart.VB_SetStageServer(stage);
		}
		return obj;
	}

	protected void VB_DespawnObj(int objId)
	{
		Object obj;
		if (!m_VB_Live.Find(objId, obj))
			return;

		if (obj)
			g_Game.ObjectDelete(obj);
		m_VB_Live.Remove(objId);
	}

	protected void VB_DespawnNest(VB_AR_Nest nest)
	{
		foreach (VB_AR_NestObj o : nest.Objects)
		{
			VB_DespawnObj(o.Id);
		}
		m_VB_LiveNests.RemoveItem(nest.Id);
	}

	// снять всё, кроме постоянного декора (он стоит весь рестарт, «Перебросить» его не трогает)
	protected void VB_DespawnNests()
	{
		set<int> keep = new set<int>;
		if (m_VB_File)
		{
			foreach (VB_AR_NestObj d : m_VB_File.Decor)
			{
				keep.Insert(d.Id);
			}
		}
		TIntArray drop = new TIntArray;
		foreach (int id, Object obj : m_VB_Live)
		{
			if (keep.Find(id) < 0)
				drop.Insert(id);
		}
		foreach (int dropId : drop)
		{
			VB_DespawnObj(dropId);
		}
		m_VB_LiveNests.Clear();
	}

	protected void VB_SpawnDecor()
	{
		if (!m_VB_File)
			return;
		foreach (VB_AR_NestObj d : m_VB_File.Decor)
		{
			VB_SpawnObj(d);
		}
	}

	protected int VB_DecorIndex(int objId)
	{
		if (!m_VB_File)
			return -1;
		for (int i = 0; i < m_VB_File.Decor.Count(); i++)
		{
			if (m_VB_File.Decor[i].Id == objId)
				return i;
		}
		return -1;
	}

	// --- вырезание и прорастание ------------------------------------------------------------------

	protected void VB_HeartKilled(VB_AR_Heart heart)
	{
		int nestId = VB_NestOfEntity(heart);
		if (nestId != 0 && m_VB_Killed.Find(nestId) < 0)
		{
			m_VB_Killed.Insert(nestId);
			VB_AR_Nest nest = VB_FindNest(nestId);
			if (nest)
				Print("[VB_AncientRoot] гнёзда: сердце вырезано в " + nest.Name + " — до рестарта это гнездо больше не встанет");
		}
		if (nestId != 0 && VB_GroveOf(nestId))
		{
			string stageName = VB_StageName(VB_StageOfNest(nestId));
			VB_DropGrove(nestId);		// роща погибла: на следующем рестарте её место займёт росток в другом гнезде
			VB_SaveState();
			Print("[VB_AncientRoot] рощи: " + VB_NestName(nestId) + " (" + stageName + ") погибла");
		}

		if (!m_VB_Settings || m_VB_Settings.RegrowMinutes <= 0)
			return;

		m_VB_Regrowing++;
		int ms = m_VB_Settings.RegrowMinutes * 60000;
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_Regrow, ms, false, m_VB_RegrowGen);
		Print("[VB_AncientRoot] гнёзда: новое прорастёт через " + m_VB_Settings.RegrowMinutes.ToString() + " мин");
	}

	// id гнезда, которому принадлежит поставленный объект ent; 0 — не наш (поставлен не гнёздами)
	protected int VB_NestOfEntity(Object ent)
	{
		if (!ent)
			return 0;
		int objId = 0;
		foreach (int id, Object obj : m_VB_Live)
		{
			if (obj == ent)
			{
				objId = id;
				break;
			}
		}
		if (objId == 0)
			return 0;
		VB_AR_Nest nest = VB_FindNestOfObj(objId);
		if (!nest)
			return 0;
		return nest.Id;
	}

	// «Перебросить» снимает отложенные прорастания: иначе после переброса проросло бы лишнее гнездо
	protected void VB_CancelRegrow()
	{
		m_VB_RegrowGen++;
		for (int i = 0; i < m_VB_Regrowing; i++)
		{
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_Regrow);
		}
		m_VB_Regrowing = 0;
	}

	// через RegrowMinutes после вырезания: одно новое гнездо с сердцем там, где сейчас ничего нет. Не гнездо с
	// вырезанным в этой сессии сердцем, не у игроков; проросшее записывается в state.json как стоявшее на этом рестарте.
	void VB_Regrow(int gen)
	{
		if (gen != m_VB_RegrowGen)
			return;		// таймер от прошлого поколения: снят перебросом
		if (m_VB_Regrowing > 0)
			m_VB_Regrowing--;
		if (!m_VB_File)
			return;

		array<vector> centers = new array<vector>;
		array<VB_AR_Nest> pool = new array<VB_AR_Nest>;
		foreach (VB_AR_Nest nest : m_VB_File.Nests)
		{
			if (m_VB_LiveNests.Find(nest.Id) >= 0)
				centers.Insert(nest.VB_Center());
			else if (nest.Enabled == 1 && m_VB_Killed.Find(nest.Id) < 0 && VB_GoodHeart(nest))
				pool.Insert(nest);
		}

		array<VB_AR_Nest> chosen = new array<VB_AR_Nest>;
		array<vector> spots = VB_PlayerSpots();
		VB_Pick(pool, 1, m_VB_PrevActive, centers, chosen, spots);
		if (chosen.Count() == 0)
		{
			Print("[VB_AncientRoot] гнёзда: прорастать некуда (нет свободных гнёзд дальше " + m_VB_Settings.MinNestDistance.ToString() + " м и без игроков рядом)");
			return;
		}

		VB_AR_Nest grown = chosen[0];
		VB_NewGrove(grown.Id);
		VB_SpawnNest(grown, VB_AR_Heart.STAGE_SPROUT);
		if (m_VB_State.LastActive.Find(grown.Id) < 0)
			m_VB_State.LastActive.Insert(grown.Id);
		VB_SaveState();
		string at = grown.VB_Center().ToString();
		Print("[VB_AncientRoot] гнёзда: проросло " + grown.Name + " в " + at);
		VB_SendNestToEditors(grown);
	}

	// --- поиск ------------------------------------------------------------------------------------

	VB_AR_Nest VB_FindNest(int nestId)
	{
		if (!m_VB_File)
			return null;

		foreach (VB_AR_Nest nest : m_VB_File.Nests)
		{
			if (nest.Id == nestId)
				return nest;
		}
		return null;
	}

	VB_AR_Nest VB_FindNestOfObj(int objId)
	{
		if (!m_VB_File)
			return null;

		foreach (VB_AR_Nest nest : m_VB_File.Nests)
		{
			if (nest.VB_FindIndex(objId) >= 0)
				return nest;
		}
		return null;
	}

	// ближайшее гнездо (по сердцу, а без него — по середине объектов) не дальше maxDist; heartlessOnly — только без сердца
	protected VB_AR_Nest VB_NearestNest(vector pos, float maxDist, bool heartlessOnly = false)
	{
		VB_AR_Nest best;
		float bestD = maxDist;
		float d;
		foreach (VB_AR_Nest nest : m_VB_File.Nests)
		{
			if (nest.Objects.Count() == 0)
				continue;
			if (heartlessOnly && nest.VB_HasHeart())
				continue;
			d = VB_AR_NestUtil.VB_DistXZ(nest.VB_Center(), pos);
			if (d < bestD)
			{
				bestD = d;
				best = nest;
			}
		}
		return best;
	}

	// гнездо с сердцем, чьё сердце ближе всех к pos и не дальше NEST_RADIUS (один поиск — без сетки)
	protected VB_AR_Nest VB_NearestHeartNest(vector pos)
	{
		VB_AR_Nest best;
		float bestD = VB_AR_NestNet.NEST_RADIUS;
		float d;
		VB_AR_NestObj heart;
		foreach (VB_AR_Nest nest : m_VB_File.Nests)
		{
			heart = nest.VB_FindHeart();
			if (!heart || !VB_PosOk(heart.Position))
				continue;
			d = VB_AR_NestUtil.VB_DistXZ(heart.Position, pos);
			if (d < bestD)
			{
				bestD = d;
				best = nest;
			}
		}
		return best;
	}

	// --- редактор (сервер) ------------------------------------------------------------------------

	bool VB_IsAdmin(string plainId)
	{
		return m_VB_Settings && m_VB_Settings.Admins && m_VB_Settings.Admins.Find(plainId) >= 0;
	}

	protected void VB_HandleEdit(PlayerIdentity sender, ParamsReadContext ctx)
	{
		int op;
		if (!ctx.Read(op))
			return;

		// действует всегда свой персонаж отправителя, не объект, на который пришёл RPC; ответы — ему же
		PlayerBase actor = PlayerBase.Cast(sender.GetPlayer());
		if (!actor)
			return;

		string uid = sender.GetPlainId();
		if (op == VB_AR_NestNet.OP_CLOSE)
		{
			m_VB_Editors.RemoveItem(uid);
			return;
		}

		int now = g_Game.GetTime();
		if (!VB_IsAdmin(uid))
		{
			// Не админ: не чаще раза в DENY_MS, лишнее молча (до любого Print и чтения диска). Раньше каждый запрос
			// читал settings.json и писал строку в лог — читер заваливал сервер (аудит 2026-11-13, №28).
			int until;
			if (m_VB_DenyUntil.Find(uid, until) && now < until)
				return;
			if (m_VB_DenyUntil.Count() > 256)
				m_VB_DenyUntil.Clear();
			m_VB_DenyUntil.Set(uid, now + DENY_MS);
			// админа можно дописать в settings.json без рестарта: список перечитывается не чаще раза в 30 с
			if (op == VB_AR_NestNet.OP_OPEN)
				VB_RefreshAdmins(now);
		}
		else
		{
			// админа могли убрать из settings.json: сверка с диском не чаще раза в 30 с (аудит №31)
			VB_RefreshAdmins(now);
		}

		string who = sender.GetName() + " (" + uid + ")";
		if (!VB_IsAdmin(uid))
		{
			m_VB_DenyUntil.Set(uid, now + DENY_MS);
			m_VB_Editors.RemoveItem(uid);
			Print("[VB_AncientRoot] редактор гнёзд: нет прав у " + who + " (операция " + op.ToString() + ")");
			ScriptRPC deny = new ScriptRPC();
			deny.Write(VB_AR_NestNet.R_DENIED);
			deny.Write(uid);
			deny.Send(actor, VB_AR_NestNet.RPC_REPLY, true, sender);
			return;
		}

		// nests.json с ошибкой: можно войти (увидеть сообщение) и «Перебросить» (перечитает исправленный файл)
		if (!m_VB_File && op != VB_AR_NestNet.OP_OPEN && op != VB_AR_NestNet.OP_REROLL)
		{
			VB_SendMsg(actor, "nests.json не прочитался, правка выключена: исправь файл и нажми «Перебросить гнёзда»");
			return;
		}

		int nestId;
		int objId;
		int want;
		VB_AR_Nest nest;
		switch (op)
		{
			case VB_AR_NestNet.OP_OPEN:
				if (m_VB_Editors.Find(uid) < 0)
					m_VB_Editors.Insert(uid);
				Print("[VB_AncientRoot] редактор гнёзд открыл " + who);
				VB_SendAll(actor);
				if (!m_VB_File)
					VB_SendMsg(actor, "nests.json не прочитался: гнёзд нет. Исправь файл и нажми «Перебросить гнёзда»");
				break;

			case VB_AR_NestNet.OP_ADD:
				VB_EditAdd(actor, uid, who, ctx);
				break;

			case VB_AR_NestNet.OP_DEL:
				if (ctx.Read(objId))
					VB_EditDelete(objId, who);
				break;

			case VB_AR_NestNet.OP_UNDO:
				if (ctx.Read(nestId))
					VB_EditUndo(uid, who, nestId);
				break;

			case VB_AR_NestNet.OP_NEST_DEL:
				if (!ctx.Read(nestId))
					break;
				nest = VB_FindNest(nestId);
				if (nest)
				{
					VB_DespawnNest(nest);
					Print("[VB_AncientRoot] гнёзда: удалено " + nest.Name + " (" + who + ")");
					m_VB_File.Nests.RemoveItem(nest);
					VB_SaveNests();
					VB_SendGoneToEditors(nestId);
				}
				break;

			case VB_AR_NestNet.OP_NEST_TOGGLE:
				if (!ctx.Read(nestId))
					break;
				nest = VB_FindNest(nestId);
				if (nest)
				{
					nest.Enabled = 1 - nest.Enabled;
					VB_SaveNests();
					Print("[VB_AncientRoot] гнёзда: " + nest.Name + " в выборе: " + nest.Enabled.ToString() + " (" + who + ")");
					VB_SendNestToEditors(nest);
				}
				break;

			case VB_AR_NestNet.OP_REROLL:
				VB_EditReroll(actor, who);
				break;

			case VB_AR_NestNet.OP_TELEPORT:
				if (ctx.Read(nestId))
					VB_EditTeleport(actor, nestId);
				break;

			case VB_AR_NestNet.OP_SET_ACTIVE:
				if (ctx.Read(want))
					VB_EditSetActive(actor, who, want);
				break;
		}
	}

	protected void VB_EditReroll(PlayerBase actor, string who)
	{
		Print("[VB_AncientRoot] гнёзда: переброс по команде " + who + " — рощи заново, ростками");
		VB_CancelRegrow();
		string note = VB_ReloadFromDisk();
		m_VB_State.Groves.Clear();		// «Перебросить» — новые места: рост рощ начинается сначала
		VB_Activate(true);
		VB_SendAllToEditors();

		int liveCount = m_VB_LiveNests.Count();
		string msg = "Гнёзда выбраны заново: " + liveCount.ToString();
		int killedCount = m_VB_Killed.Count();
		if (killedCount > 0)
			msg += " (с вырезанным сердцем до рестарта не встают: " + killedCount.ToString() + ")";
		if (note != "")
			msg += ". " + note;
		VB_SendMsg(actor, msg);
	}

	// «Гнёзд за рестарт»: файл читается во временный объект, меняется только ActiveNests, остальное (ручные правки,
	// новые админы) не затирается памятью. Файл с ошибкой не трогаем — число действует до рестарта (аудит №31).
	protected void VB_EditSetActive(PlayerBase actor, string who, int want)
	{
		int n = want;
		if (n < 0)
			n = 0;
		if (n > 500)
			n = 500;
		m_VB_Settings.ActiveNests = n;

		bool broken;
		VB_AR_NestSettings disk = VB_ReadSettings(broken, true);
		if (broken)
		{
			Print("[VB_AncientRoot] гнёзда: settings.json с ошибкой — «гнёзд за рестарт» " + n.ToString() + " действует до рестарта, в файл не записано (" + who + ")");
			VB_SendMsg(actor, "settings.json с ошибкой: число " + n.ToString() + " действует до рестарта, в файл не записано");
		}
		else
		{
			if (!disk)
				disk = m_VB_Settings;		// файла нет — записать текущие настройки целиком
			disk.ActiveNests = n;
			if (VB_WriteSettings(disk))
				Print("[VB_AncientRoot] гнёзда: за рестарт теперь " + n.ToString() + " (" + who + ")");
		}
		VB_SendSettingsToEditors();
	}

	protected void VB_EditTeleport(PlayerBase actor, int nestId)
	{
		if (nestId == VB_AR_NestNet.DECOR_ID)
		{
			// к ближайшему щупальцу декора
			VB_AR_NestObj nearDecor;
			float nearD = 1000000000.0;
			float dd;
			vector me = actor.GetPosition();
			foreach (VB_AR_NestObj dt : m_VB_File.Decor)
			{
				dd = VB_AR_NestUtil.VB_DistXZ(dt.Position, me);
				if (dd < nearD)
				{
					nearD = dd;
					nearDecor = dt;
				}
			}
			if (nearDecor && VB_PosOk(nearDecor.Position))
			{
				float ax = nearDecor.Position[0] + 3.0;
				float az = nearDecor.Position[2] + 3.0;
				float ay = g_Game.SurfaceY(ax, az);
				actor.SetPosition(Vector(ax, ay, az));
			}
			return;
		}

		VB_AR_Nest nest = VB_FindNest(nestId);
		if (!nest || nest.Objects.Count() == 0)
			return;
		vector c = nest.VB_Center();
		if (!VB_PosOk(c))
			return;
		float tx = c[0] + 6.0;
		float tz = c[2] + 6.0;
		float ty = g_Game.SurfaceY(tx, tz);
		actor.SetPosition(Vector(tx, ty, tz));
	}

	// Куда ложится объект: force — в указанное гнездо («Обсадить корнями»). Иначе сердце начинает новое гнездо
	// (или становится сердцем гнезда без сердца рядом), а корни и щупальца идут в ближайшее гнездо в NEST_RADIUS;
	// дальше — в новое гнездо. Принимаются только типы редактора и координаты в мире (аудит №39).
	protected void VB_EditAdd(PlayerBase actor, string uid, string who, ParamsReadContext ctx)
	{
		int nestId;
		bool force;
		int count;
		if (!ctx.Read(nestId) || !ctx.Read(force) || !ctx.Read(count) || count <= 0 || count > 64)
			return;

		array<ref VB_AR_NestObj> added = new array<ref VB_AR_NestObj>;
		string type;
		vector pos;
		vector ori;
		int rejected = 0;
		for (int i = 0; i < count; i++)
		{
			if (!ctx.Read(type) || !ctx.Read(pos) || !ctx.Read(ori))
				return;
			if (!VB_AR_NestUtil.VB_IsNestType(type) || !VB_PosOk(pos) || !VB_OriOk(ori))
			{
				rejected++;
				if (rejected == 1)
				{
					string posText = pos.ToString();
					Print("[VB_AncientRoot] редактор гнёзд: отклонён " + type + " в " + posText + " от " + who + " (тип не из редактора или место вне мира)");
				}
				continue;
			}

			VB_AR_NestObj o = new VB_AR_NestObj();
			o.Type = type;
			o.Position = pos;
			o.Orientation = ori;
			added.Insert(o);
		}
		if (rejected > 0)
			VB_SendMsg(actor, "Отклонено объектов: " + rejected.ToString() + " (тип не из редактора или место вне мира)");
		if (added.Count() == 0)
			return;

		vector first = added[0].Position;
		VB_AR_Nest nest = null;
		if (force)
			nest = VB_FindNest(nestId);
		if (!nest && VB_AR_NestUtil.VB_IsHangType(added[0].Type))
		{
			// щупальце: в гнездо ближайшего сердца, а без сердца рядом — в постоянный декор
			nest = VB_NearestHeartNest(first);
			if (!nest)
			{
				VB_EditAddDecor(actor, uid, who, added);
				return;
			}
		}
		bool heartFirst = VB_AR_NestUtil.VB_IsHeartType(added[0].Type);
		if (!nest)
			nest = VB_NearestNest(first, VB_AR_NestNet.NEST_RADIUS, heartFirst);
		if (!nest)
		{
			nest = VB_NewNest();
			string firstText = first.ToString();
			Print("[VB_AncientRoot] гнёзда: новое " + nest.Name + " в " + firstText);
		}

		bool live = m_VB_LiveNests.Find(nest.Id) >= 0;
		bool heartAdded = false;
		foreach (VB_AR_NestObj add : added)
		{
			add.Id = VB_TakeId();
			nest.Objects.Insert(add);
			VB_RememberPlaced(uid, add.Id);
			if (live)
				VB_SpawnObj(add);
			if (VB_AR_NestUtil.VB_IsHeartType(add.Type))
				heartAdded = true;
		}
		int addedN = added.Count();
		Print("[VB_AncientRoot] редактор гнёзд: " + who + " поставил " + addedN.ToString() + " в " + nest.Name);

		// новое сердце забирает корни, которые теперь ближе к нему; ушедшие в невыбранное гнездо снимаются с карты
		if (heartAdded && !force && VB_Regroup())
		{
			VB_SyncLive();
			VB_SpawnDecor();
			VB_SendAllToEditors();
		}
		else
		{
			VB_SendNestToEditors(nest);
		}
		VB_SaveNests();

		ScriptRPC sel = new ScriptRPC();
		sel.Write(VB_AR_NestNet.R_SELECT);
		sel.Write(nest.Id);
		sel.Send(actor, VB_AR_NestNet.RPC_REPLY, true, actor.GetIdentity());
	}

	protected void VB_EditAddDecor(PlayerBase actor, string uid, string who, array<ref VB_AR_NestObj> added)
	{
		foreach (VB_AR_NestObj add : added)
		{
			add.Id = VB_TakeId();
			m_VB_File.Decor.Insert(add);
			VB_RememberPlaced(uid, add.Id);
			VB_SpawnObj(add);
		}
		int addedN = added.Count();
		Print("[VB_AncientRoot] редактор гнёзд: " + who + " поставил " + addedN.ToString() + " в постоянный декор");
		VB_SaveNests();
		VB_SendDecorToEditors();

		ScriptRPC sel = new ScriptRPC();
		sel.Write(VB_AR_NestNet.R_SELECT);
		sel.Write(VB_AR_NestNet.DECOR_ID);
		sel.Send(actor, VB_AR_NestNet.RPC_REPLY, true, actor.GetIdentity());
	}

	// свои объекты админа за эту сессию — «Убрать последний» убирает сначала своё, а не то, что поставил другой
	protected void VB_RememberPlaced(string uid, int objId)
	{
		TIntArray mine = m_VB_Placed.Get(uid);
		if (!mine)
		{
			mine = new TIntArray;
			m_VB_Placed.Set(uid, mine);
		}
		mine.Insert(objId);
		while (mine.Count() > UNDO_KEEP)
		{
			mine.RemoveOrdered(0);
		}
	}

	protected bool VB_InNest(int objId, int nestId)
	{
		if (nestId == VB_AR_NestNet.DECOR_ID)
			return VB_DecorIndex(objId) >= 0;
		VB_AR_Nest nest = VB_FindNest(nestId);
		if (!nest)
			return false;
		return nest.VB_FindIndex(objId) >= 0;
	}

	// «Убрать последний объект»: последний свой объект в этом гнезде за сессию; своих нет — самый новый в гнезде
	protected void VB_EditUndo(string uid, string who, int nestId)
	{
		int target = 0;
		TIntArray mine = m_VB_Placed.Get(uid);
		if (mine)
		{
			for (int i = mine.Count() - 1; i >= 0; i--)
			{
				if (VB_InNest(mine[i], nestId))
				{
					target = mine[i];
					mine.RemoveOrdered(i);
					break;
				}
			}
		}

		if (target == 0)
		{
			if (nestId == VB_AR_NestNet.DECOR_ID)
			{
				foreach (VB_AR_NestObj dl : m_VB_File.Decor)
				{
					target = Math.Max(target, dl.Id);
				}
			}
			else
			{
				VB_AR_Nest nest = VB_FindNest(nestId);
				if (nest)
				{
					foreach (VB_AR_NestObj o : nest.Objects)
					{
						target = Math.Max(target, o.Id);
					}
				}
			}
		}
		if (target > 0)
			VB_EditDelete(target, who);
	}

	protected void VB_EditDelete(int objId, string who)
	{
		int decorIdx = VB_DecorIndex(objId);
		if (decorIdx >= 0)
		{
			string decorType = m_VB_File.Decor[decorIdx].Type;
			VB_DespawnObj(objId);
			m_VB_File.Decor.RemoveOrdered(decorIdx);
			Print("[VB_AncientRoot] редактор гнёзд: " + who + " убрал " + decorType + " из постоянного декора");
			VB_SaveNests();
			VB_SendDecorToEditors();
			return;
		}

		VB_AR_Nest nest = VB_FindNestOfObj(objId);
		if (!nest)
			return;

		int idx = nest.VB_FindIndex(objId);
		string objType = nest.Objects[idx].Type;
		bool wasHeart = VB_AR_NestUtil.VB_IsHeartType(objType);
		VB_DespawnObj(objId);
		nest.Objects.RemoveOrdered(idx);
		Print("[VB_AncientRoot] редактор гнёзд: " + who + " убрал " + objType + " из " + nest.Name);

		// убрали сердце: корни переходят к ближайшему другому сердцу, если оно рядом; оставшиеся без сердца
		// снимаются с карты (гнездо без сердца не стоит)
		if (wasHeart)
		{
			bool regrouped = VB_Regroup();
			VB_SyncLive();
			VB_SpawnDecor();
			VB_SaveNests();
			if (regrouped)
				VB_SendAllToEditors();
			else
				VB_SendNestToEditors(nest);
			return;
		}

		if (nest.Objects.Count() == 0)
		{
			// пустое гнездо не нужно: следующий объект рядом создаст новое
			m_VB_LiveNests.RemoveItem(nest.Id);
			int goneId = nest.Id;
			m_VB_File.Nests.RemoveItem(nest);
			VB_SaveNests();
			VB_SendGoneToEditors(goneId);
			return;
		}
		VB_SaveNests();
		VB_SendNestToEditors(nest);
	}

	// --- отправка ---------------------------------------------------------------------------------

	// Игроки открытых редакторов. Вышедших и снятых из админов выкидывает из списка: им больше ничего не шлём.
	protected array<PlayerBase> VB_EditorPlayers()
	{
		array<PlayerBase> res = new array<PlayerBase>;
		if (m_VB_Editors.Count() == 0)
			return res;

		map<string, PlayerBase> online = new map<string, PlayerBase>;
		array<Man> players = new array<Man>;
		g_Game.GetPlayers(players);
		PlayerIdentity ident;
		foreach (Man someone : players)
		{
			if (!someone)
				continue;
			ident = someone.GetIdentity();
			if (ident)
				online.Set(ident.GetPlainId(), PlayerBase.Cast(someone));
		}

		string editor;
		PlayerBase ep;
		for (int i = m_VB_Editors.Count() - 1; i >= 0; i--)
		{
			editor = m_VB_Editors[i];
			ep = null;
			if (!VB_IsAdmin(editor) || !online.Find(editor, ep) || !ep)
			{
				m_VB_Editors.Remove(i);
				continue;
			}
			res.Insert(ep);
		}
		return res;
	}

	protected void VB_SendMsg(PlayerBase player, string text)
	{
		if (!player || !player.GetIdentity())
			return;
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(VB_AR_NestNet.R_MSG);
		rpc.Write(text);
		rpc.Send(player, VB_AR_NestNet.RPC_REPLY, true, player.GetIdentity());
	}

	// порядок полей читает VB_AR_NestClient.VB_ReadSettings
	protected void VB_WriteSettingsRpc(ScriptRPC rpc)
	{
		rpc.Write(m_VB_Settings.ActiveNests);
		rpc.Write(m_VB_Settings.MinNestDistance);
		rpc.Write(m_VB_Settings.RootsPercentMin);
		rpc.Write(m_VB_Settings.RootsPercentMax);
		rpc.Write(m_VB_Settings.RegrowMinutes);
	}

	protected void VB_SendSettingsToEditors()
	{
		array<PlayerBase> eds = VB_EditorPlayers();
		foreach (PlayerBase ep : eds)
		{
			ScriptRPC rpc = new ScriptRPC();
			rpc.Write(VB_AR_NestNet.R_SETTINGS);
			VB_WriteSettingsRpc(rpc);
			rpc.Send(ep, VB_AR_NestNet.RPC_REPLY, true, ep.GetIdentity());
		}
	}

	protected void VB_SendAll(PlayerBase player)
	{
		if (!player || !player.GetIdentity())
			return;

		int nestCount = 0;
		if (m_VB_File)
			nestCount = m_VB_File.Nests.Count();

		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(VB_AR_NestNet.R_BEGIN);
		VB_WriteSettingsRpc(rpc);
		rpc.Write(nestCount);
		rpc.Send(player, VB_AR_NestNet.RPC_REPLY, true, player.GetIdentity());

		if (!m_VB_File)
			return;
		foreach (VB_AR_Nest nest : m_VB_File.Nests)
		{
			VB_SendNest(player, nest);
		}
		VB_SendDecor(player);
	}

	protected void VB_SendDecor(PlayerBase player)
	{
		if (!m_VB_File)
			return;
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(VB_AR_NestNet.R_DECOR);
		rpc.Write(m_VB_File.Decor.Count());
		bool objLive;
		foreach (VB_AR_NestObj o : m_VB_File.Decor)
		{
			rpc.Write(o.Id);
			rpc.Write(o.Type);
			rpc.Write(o.Position);
			rpc.Write(o.Orientation);
			objLive = m_VB_Live.Contains(o.Id);
			rpc.Write(objLive);
		}
		rpc.Send(player, VB_AR_NestNet.RPC_REPLY, true, player.GetIdentity());
	}

	protected void VB_SendDecorToEditors()
	{
		array<PlayerBase> eds = VB_EditorPlayers();
		foreach (PlayerBase ep : eds)
		{
			VB_SendDecor(ep);
		}
	}

	protected void VB_SendNest(PlayerBase player, VB_AR_Nest nest)
	{
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(VB_AR_NestNet.R_NEST);
		rpc.Write(nest.Id);
		rpc.Write(nest.Name);
		rpc.Write(nest.Enabled);
		bool nestLive = m_VB_LiveNests.Find(nest.Id) >= 0;
		rpc.Write(nestLive);
		rpc.Write(nest.Objects.Count());
		bool objLive;
		foreach (VB_AR_NestObj o : nest.Objects)
		{
			rpc.Write(o.Id);
			rpc.Write(o.Type);
			rpc.Write(o.Position);
			rpc.Write(o.Orientation);
			objLive = m_VB_Live.Contains(o.Id);
			rpc.Write(objLive);
		}
		rpc.Send(player, VB_AR_NestNet.RPC_REPLY, true, player.GetIdentity());
	}

	protected void VB_SendNestToEditors(VB_AR_Nest nest)
	{
		array<PlayerBase> eds = VB_EditorPlayers();
		foreach (PlayerBase ep : eds)
		{
			VB_SendNest(ep, nest);
		}
	}

	protected void VB_SendAllToEditors()
	{
		array<PlayerBase> eds = VB_EditorPlayers();
		foreach (PlayerBase ep : eds)
		{
			VB_SendAll(ep);
		}
	}

	protected void VB_SendGoneToEditors(int nestId)
	{
		array<PlayerBase> eds = VB_EditorPlayers();
		foreach (PlayerBase ep : eds)
		{
			ScriptRPC rpc = new ScriptRPC();
			rpc.Write(VB_AR_NestNet.R_NEST_GONE);
			rpc.Write(nestId);
			rpc.Send(ep, VB_AR_NestNet.RPC_REPLY, true, ep.GetIdentity());
		}
	}
}

// Запросы редактора приходят на объект игрока (клиент: VB_AR_NestClient.VB_SendOp/VB_SendAdd, VB_AR_Nests.c).
// Ответы RPC_REPLY принимает клиентский modded PlayerBase в аддоне VB_AncientRoot.
modded class PlayerBase
{
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);

		if (rpc_type == VB_AR_NestNet.RPC_EDIT && g_Game.IsServer())
			VB_AR_Nests.VB_OnEditRpc(this, sender, ctx);
	}
}
