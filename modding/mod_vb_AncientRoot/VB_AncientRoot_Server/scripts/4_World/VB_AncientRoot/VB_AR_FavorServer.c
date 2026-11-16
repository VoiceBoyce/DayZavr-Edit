// Расположение леса (ТЗ_сервер_18.md п. 2б.11) — сервер. Число −100…+100 на игрока (SteamID), у новичка 0, переживает
// смерть и рестарт: $profile:VB_Mods\VB_AncientRoot\favor.json. Каждые сутки реального времени на DECAY_PER_DAY ближе к нулю
// (считается по часам UTC при обращении). Игроку значение уходит RPC (VB_AR_Favor.RPC_FAVOR) при заходе и при изменении.
//
// Поступки: подношение +5/+10/+15 (по ценности дара), отросток и пыльца −1, вырезанная сердцевина −25,
// убитый страж −3 (Хранитель −10).
// Пути вверх: время (сутки — на 5 к нулю); изгнание нечисти — паук MSFZ +0.1 (+1 за 10), ERA-зомби +0.2 (+1 за 5);
// Отвар примирения +15 и разовое прощение (роща примет дар даже у «Проклятого», до рестарта), раз в сутки;
// Семя на месте вырезанной рощи +20 (гнездо встанет на следующем рестарте, VB_AR_Nests.VB_OnSeedPlanted) — не чаще раза
// в сутки и не тому, кто в этот рестарт вырезал рощу (VB_SeedFavor).
// Файл: запись через favor_tmp.json; на старте favor.json → favor_tmp.json → favor_backup.json (копия последнего
// удачного старта). Файл с ошибкой не затирается молча: сначала копия в favor_broken.json.
// Последствия:
//   Свой и Друг леса — стражи не нападают первыми (как Благословение); Друг — Благословение вдвое дольше, дар щедрее;
//   Враг леса — ночью в лесу изредка стая волков по следу; Проклятый — чаще и с занозами, рощи не принимают даров.
// «Не нападают первыми»: ударил стража, поднял волну жадным сбором или начал резать сердцевину — стражи злы на игрока
// HOSTILE_MS (VB_Anger), Благословение снято.

class VB_AR_FavorEntry
{
	string Id;			// SteamID64
	float Favor;
	int Hour;			// час UTC (от 1970), до которого возврат к нулю уже посчитан
	int ReconcileHour = -100000;	// час UTC последнего Отвара примирения (раз в сутки)
	int MetHour;		// час UTC, когда последний раз был ночью у живой рощи (0 — ни разу): «Голос леса»
	int SeedHour = -100000;		// час UTC, когда последний раз получил расположение за Семя (раз в сутки)
}

class VB_AR_FavorFile
{
	int Version = 1;
	ref array<ref VB_AR_FavorEntry> Players;

	void VB_AR_FavorFile()
	{
		Players = new array<ref VB_AR_FavorEntry>;
	}
}

class VB_AR_FavorServer
{
	static const string FILE = "$profile:VB_Mods\\VB_AncientRoot\\favor.json";
	static const string FILE_TMP = "$profile:VB_Mods\\VB_AncientRoot\\favor_tmp.json";
	static const string FILE_BACKUP = "$profile:VB_Mods\\VB_AncientRoot\\favor_backup.json";
	static const string FILE_BROKEN = "$profile:VB_Mods\\VB_AncientRoot\\favor_broken.json";
	static const int SEED_HOURS = 24;
	static const int FORGET_HOURS = 2160;		// 90 суток: давно не заходивший, у кого расположение ушло в ноль, — забыт
	static const float LIMIT = 100;
	static const float DECAY_PER_DAY = 5;
	static const int SAVE_DELAY_MS = 30000;

	// поступки
	static const float OFFER_MEAT = 5;
	static const float OFFER_MID = 10;
	static const float OFFER_BEST = 15;
	static const float GATHER = -1;			// отросток, пыльца
	static const float CUT = -25;			// вырезанная сердцевина (роща)
	static const float CUT_SPROUT = -10;	// вырезанный росток (ТЗ 2б.4)
	static const float CUT_ANCIENT = -40;	// вырезанная древняя роща
	static const float KILL_GUARD = -3;
	static const float KILL_KEEPER = -10;	// Хранитель рощи
	// пути вверх
	static const float PURGE_SPIDER = 0.1;	// паук MSFZ: +1 за 10
	static const float PURGE_ERA = 0.2;		// ERA-зомби: +1 за 5
	static const float RECONCILE = 15;
	static const int RECONCILE_HOURS = 24;
	static const float SEED = 20;

	// злость стражей и волны за рейтинг
	static const int HOSTILE_MS = 600000;			// 10 минут стражи помнят обиду
	static const int WAVE_CHECK_MS = 60000;
	static const int ENEMY_COOLDOWN_MS = 2400000;	// Враг леса — не чаще раза в 40 минут
	static const int CURSED_COOLDOWN_MS = 1800000;	// Проклятый — в 30 минут
	static const float ENEMY_CHANCE = 0.35;			// на каждой проверке после паузы
	static const float CURSED_CHANCE = 0.6;
	static const int HOWL_LEAD_MS = 12000;			// вой и «Лес помнит тебя…» — за столько до волны
	static const float FOREST_RADIUS = 20;
	static const int FOREST_TREES = 8;				// столько деревьев рядом — «в лесу»
	static const float GROVE_NEAR = 80;				// или живая сердцевина ближе
	static const float BASE_RADIUS = 60;			// флаг территории ближе — база, волн нет

	// «Голос леса» (решение пользователя 2026-11-16, вариант А): подсказка про выбор при первой встрече с рощей
	static const int VOICE_CHECK_MS = 10000;
	static const float VOICE_NEAR = 30;
	static const int VOICE_REPEAT_HOURS = 168;		// не был у рощ неделю — услышит снова
	static const string VOICE_TITLE = "Голос леса";
	static const string VOICE_TEXT = "Лес смотрит на тебя. Роща примет дар — руну, кристалл, трофей или мясо — и отблагодарит. Нож у сердцевины она тоже почувствует.";

	protected static ref VB_AR_FavorFile s_VB_File;
	protected static ref map<string, VB_AR_FavorEntry> s_VB_Index;
	protected static bool s_VB_Dirty;
	protected static ref map<string, int> s_VB_Tier;		// последняя ступень (для AI — без чтения часов)
	protected static ref map<string, int> s_VB_Hostile;		// SteamID -> GetTime(), до которого стражи злы
	protected static ref map<string, bool> s_VB_Cutters;	// SteamID вырезавших сердцевину в этот рестарт (оберег)
	protected static ref map<string, int> s_VB_NextWave;	// SteamID -> GetTime(), раньше которого волны нет
	protected static ref map<string, bool> s_VB_Pardon;		// SteamID -> разовое прощение после Отвара (до рестарта)

	static void VB_Start()
	{
		s_VB_Index = new map<string, VB_AR_FavorEntry>;
		s_VB_Tier = new map<string, int>;
		s_VB_Hostile = new map<string, int>;
		s_VB_NextWave = new map<string, int>;
		s_VB_Pardon = new map<string, bool>;
		s_VB_File = VB_LoadAny();
		if (!s_VB_File)
			s_VB_File = new VB_AR_FavorFile();
		if (!s_VB_File.Players)
			s_VB_File.Players = new array<ref VB_AR_FavorEntry>;
		int hourNow = VB_HourNow();
		int forgot = 0;
		for (int i = s_VB_File.Players.Count() - 1; i >= 0; i--)
		{
			VB_AR_FavorEntry e = s_VB_File.Players[i];
			if (!e || e.Id == "" || s_VB_Index.Contains(e.Id))
			{
				s_VB_File.Players.Remove(i);		// пустая запись (лишняя запятая) или повтор
				continue;
			}
			if (VB_Forgotten(e, hourNow))
			{
				s_VB_File.Players.Remove(i);
				forgot++;
				continue;
			}
			s_VB_Index.Set(e.Id, e);
		}
		if (forgot > 0)
			VB_MarkDirty();
		int n = s_VB_Index.Count();
		Print("[VB_AncientRoot] расположение леса: игроков в favor.json " + n.ToString() + ", забыто давно не заходивших " + forgot.ToString());
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_WaveTick, WAVE_CHECK_MS, true);
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_VoiceTick, VOICE_CHECK_MS, true);
	}

	// один файл: null — нет или не разобрался (broken = true)
	protected static VB_AR_FavorFile VB_LoadFrom(string path, out bool broken)
	{
		broken = false;
		if (!FileExist(path))
			return null;
		string err;
		VB_AR_FavorFile f = new VB_AR_FavorFile();
		if (JsonFileLoader<VB_AR_FavorFile>.LoadFile(path, f, err) && f)
			return f;
		broken = true;
		Print("[VB_AncientRoot] расположение леса: ОШИБКА в " + path + ": " + err);
		return null;
	}

	// favor.json, если его нет или он битый — favor_tmp.json (прервалась запись), потом favor_backup.json. Битый
	// основной сначала копируется в favor_broken.json: следующая запись его заменит, а данные не должны пропасть молча.
	protected static VB_AR_FavorFile VB_LoadAny()
	{
		bool broken;
		VB_AR_FavorFile f = VB_LoadFrom(FILE, broken);
		if (f)
		{
			VB_CopyOver(FILE, FILE_BACKUP);		// удачный старт — копия на случай беды
			return f;
		}
		if (broken)
		{
			VB_CopyOver(FILE, FILE_BROKEN);
			Print("[VB_AncientRoot] расположение леса: favor.json с ошибкой сохранён как favor_broken.json");
		}
		bool b2;
		f = VB_LoadFrom(FILE_TMP, b2);
		if (f)
		{
			Print("[VB_AncientRoot] расположение леса: взят favor_tmp.json (прошлая запись прервалась)");
			VB_MarkDirty();
			return f;
		}
		f = VB_LoadFrom(FILE_BACKUP, b2);
		if (f)
		{
			Print("[VB_AncientRoot] расположение леса: взят favor_backup.json (копия прошлого удачного старта)");
			VB_MarkDirty();
			return f;
		}
		if (broken)
			Print("[VB_AncientRoot] расположение леса: favor.json не прочитан и копий нет — начинаю с пустого (старый — в favor_broken.json)");
		return null;
	}

	// копия файла поверх другого (CopyFile поверх не пишет)
	static void VB_CopyOver(string from, string to)
	{
		if (!FileExist(from))
			return;
		if (FileExist(to))
			DeleteFile(to);
		if (!CopyFile(from, to))
			Print("[VB_AncientRoot] не удалось скопировать " + from + " -> " + to);
	}

	// давно не был (FORGET_HOURS), расположение за это время ушло в ноль, суточных отметок не осталось
	protected static bool VB_Forgotten(VB_AR_FavorEntry e, int hourNow)
	{
		int hours = hourNow - e.Hour;
		if (hours < FORGET_HOURS || hourNow - e.MetHour < FORGET_HOURS)
			return false;
		return Math.AbsFloat(e.Favor) <= DECAY_PER_DAY * hours / 24.0;
	}

	// Ночью игрок у живой рощи (VOICE_NEAR): первый раз (или после VOICE_REPEAT_HOURS вдали от рощ) — «Голос леса».
	// Час встречи пишется в favor.json (MetHour), файл сохраняется отложенно (VB_MarkDirty).
	static void VB_VoiceTick()
	{
		if (VB_AR_Pulse.VB_NightFactor() < VB_AR_Pulse.NIGHT_FULL_ABOVE)
			return;
		array<Man> players = new array<Man>;
		g_Game.GetPlayers(players);
		int hour = VB_HourNow();
		PlayerBase p;
		string id;
		VB_AR_FavorEntry e;
		bool fresh;
		foreach (Man m : players)
		{
			p = PlayerBase.Cast(m);
			id = VB_Id(p);
			if (id == "" || !p.IsAlive())
				continue;
			// по списку сердец сервера, а не поиском объектов вокруг каждого игрока раз в 10 с
			VB_AR_Heart voiceHeart = VB_AR_Heart.VB_NearestServer(p.GetPosition(), VOICE_NEAR);
			if (!voiceHeart)
				continue;
			e = VB_Entry(id, true);
			if (!e)
				continue;
			fresh = e.MetHour <= 0 || hour - e.MetHour >= VOICE_REPEAT_HOURS;
			if (e.MetHour != hour)
			{
				e.MetHour = hour;
				VB_MarkDirty();
			}
			if (fresh)
			{
				NotificationSystem.SendNotificationToPlayerExtended(p, 12, VOICE_TITLE, VOICE_TEXT);
				// звук «голоса леса» (ТЗ 2в п.5): шёпот рощи со стороны ближайшей сердцевины
				VB_AR_Memory.VB_WhisperAt(p, voiceHeart.GetPosition() + "0 1 0");
				Print("[VB_AncientRoot] голос леса: " + VB_AR_ItemsServer.VB_Who(p));
			}
		}
	}

	// ------------------------------------------------------------ значение
	static string VB_Id(PlayerBase p)
	{
		if (!p || !p.GetIdentity())
			return "";
		return p.GetIdentity().GetPlainId();
	}

	// дней от 1970-01-01 (григорианский календарь)
	static int VB_Days(int y, int m, int d)
	{
		if (m <= 2)
			y--;
		int era = y / 400;
		int yoe = y - era * 400;
		int mp = (m + 9) % 12;
		int doy = (153 * mp + 2) / 5 + d - 1;
		int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
		return era * 146097 + doe - 719468;
	}

	static int VB_HourNow()
	{
		int y;
		int mo;
		int d;
		int h;
		int mi;
		int s;
		GetYearMonthDayUTC(y, mo, d);
		GetHourMinuteSecondUTC(h, mi, s);
		return VB_Days(y, mo, d) * 24 + h;
	}

	// минут UTC от 1970 (длина запуска сервера для роста рощ, VB_AR_Nests)
	static int VB_MinuteNow()
	{
		int y;
		int mo;
		int d;
		int h;
		int mi;
		int s;
		GetYearMonthDayUTC(y, mo, d);
		GetHourMinuteSecondUTC(h, mi, s);
		int days = VB_Days(y, mo, d);
		return (days * 24 + h) * 60 + mi;
	}

	protected static VB_AR_FavorEntry VB_Entry(string id, bool create)
	{
		if (id == "" || !s_VB_Index)
			return null;
		VB_AR_FavorEntry e;
		if (s_VB_Index.Find(id, e))
			return e;
		if (!create)
			return null;
		e = new VB_AR_FavorEntry();
		e.Id = id;
		e.Favor = 0;
		e.Hour = VB_HourNow();
		s_VB_File.Players.Insert(e);
		s_VB_Index.Set(id, e);
		return e;
	}

	// возврат к нулю за прошедшие часы
	protected static void VB_Decay(VB_AR_FavorEntry e)
	{
		int now = VB_HourNow();
		int hours = now - e.Hour;
		if (hours == 0)
			return;
		e.Hour = now;
		if (hours < 0 || e.Favor == 0)
			return;
		float step = DECAY_PER_DAY * hours / 24.0;
		if (e.Favor > 0)
			e.Favor = Math.Max(0, e.Favor - step);
		else
			e.Favor = Math.Min(0, e.Favor + step);
		VB_MarkDirty();
	}

	static float VB_Get(PlayerBase p)
	{
		VB_AR_FavorEntry e = VB_Entry(VB_Id(p), false);
		if (!e)
			return 0;
		VB_Decay(e);
		return e.Favor;
	}

	static int VB_TierNow(PlayerBase p)
	{
		return VB_AR_Favor.VB_TierOf(VB_Get(p));
	}

	// ступень без чтения часов — для частых проверок ИИ (обновляется при изменении, заходе и раз в минуту)
	static int VB_TierCached(PlayerBase p)
	{
		string id = VB_Id(p);
		int t;
		if (id != "" && s_VB_Tier && s_VB_Tier.Find(id, t))
			return t;
		return VB_AR_Favor.TIER_STRANGER;
	}

	static void VB_Add(PlayerBase p, float delta, string why)
	{
		string id = VB_Id(p);
		VB_AR_FavorEntry e = VB_Entry(id, true);
		if (!e || delta == 0)
			return;
		VB_Decay(e);
		int before = VB_AR_Favor.VB_TierOf(e.Favor);
		e.Favor = Math.Clamp(e.Favor + delta, -LIMIT, LIMIT);
		int after = VB_AR_Favor.VB_TierOf(e.Favor);
		s_VB_Tier.Set(id, after);
		VB_MarkDirty();
		VB_Send(p, e.Favor, delta);

		string who = VB_AR_ItemsServer.VB_Who(p);
		string ds = delta.ToString();
		string vs = e.Favor.ToString();
		Print("[VB_AncientRoot] расположение леса: " + who + " — " + why + ": " + ds + " = " + vs + " (" + VB_AR_Favor.VB_TierName(after) + ")");
		if (after != before)
			NotificationSystem.SendNotificationToPlayerExtended(p, 7, "Расположение леса: " + VB_AR_Favor.VB_TierName(after), VB_TierLine(after));
	}

	static string VB_TierLine(int tier)
	{
		switch (tier)
		{
			case VB_AR_Favor.TIER_FRIEND: return "Лес считает тебя своим другом. Стражи не тронут, дары щедрее.";
			case VB_AR_Favor.TIER_OWN: return "Лес признал тебя. Стражи не нападут первыми.";
			case VB_AR_Favor.TIER_ENEMY: return "Лес насторожен. Ночью в чаще за тобой могут пойти по следу.";
			case VB_AR_Favor.TIER_CURSED: return "Лес тебя ненавидит. Рощи не примут твоих даров.";
		}
		return "Лес тебя не знает.";
	}

	static void VB_Send(PlayerBase p, float v, float d)
	{
		if (p && p.GetIdentity())
			g_Game.RPCSingleParam(p, VB_AR_Favor.RPC_FAVOR, new Param3<float, float, bool>(v, d, VB_HasPardon(p)), true, p.GetIdentity());
	}

	// ------------------------------------------------------------ пути вверх
	static bool VB_HasPardon(PlayerBase p)
	{
		string id = VB_Id(p);
		bool v;
		if (id == "" || !s_VB_Pardon)
			return false;
		if (!s_VB_Pardon.Find(id, v))
			return false;
		return v;
	}

	// подношение «Проклятого» по прощению — прощение тратится
	static void VB_UsePardon(PlayerBase p)
	{
		string id = VB_Id(p);
		if (id == "" || !s_VB_Pardon)
			return;
		s_VB_Pardon.Remove(id);
		VB_Send(p, VB_Get(p), 0);
	}

	// Отвар примирения выпит у живой рощи; false — не прошли сутки с прошлого (отвар не тратится)
	static bool VB_Reconcile(PlayerBase p)
	{
		string id = VB_Id(p);
		VB_AR_FavorEntry e = VB_Entry(id, true);
		if (!e)
			return false;
		int now = VB_HourNow();
		if (now - e.ReconcileHour < RECONCILE_HOURS)
		{
			NotificationSystem.SendNotificationToPlayerExtended(p, 6, "Отвар не действует", "Лес ещё помнит прошлый раз. Отвар помогает раз в сутки.");
			return false;
		}
		e.ReconcileHour = now;
		s_VB_Pardon.Set(id, true);
		VB_Add(p, RECONCILE, "Отвар примирения");
		NotificationSystem.SendNotificationToPlayerExtended(p, 7, "Лес смягчился", "Роща примет от тебя один дар, даже если лес тебя ненавидит.");
		return true;
	}

	// Семя посажено (VB_ActionPlantSeed, VB_AR_GroveServer.c): +SEED, но не тому, кто в этот рестарт вырезал рощу,
	// и не чаще раза в SEED_HOURS. Вернёт пояснение для игрока ("" — расположение начислено)
	static string VB_SeedFavor(PlayerBase p)
	{
		if (VB_IsCutter(p))
		{
			Print("[VB_AncientRoot] расположение леса: " + VB_AR_ItemsServer.VB_Who(p) + " посадил Семя, но в этот рестарт вырезал рощу — без расположения");
			return "Лес принял семя, но не забыл твоего ножа.";
		}
		VB_AR_FavorEntry e = VB_Entry(VB_Id(p), true);
		if (!e)
			return "";
		int now = VB_HourNow();
		if (now - e.SeedHour < SEED_HOURS)
			return "Лес благодарен, но за Семя он помнит тебя лишь раз в сутки.";
		e.SeedHour = now;
		VB_Add(p, SEED, "посадил Семя Древнего корня");
		return "";
	}

	// убита нечисть: паук MSFZ или ERA-зомби — лес благодарен (стражи и обычные звери не в счёт)
	static void VB_OnCreatureKilled(EntityAI body, Object killer)
	{
		if (!body || !killer)
			return;
		string type = body.GetType();
		float d = 0;
		string why = "";
		if (type.IndexOf("MSFZ_Mutant_Spider") == 0)
		{
			d = PURGE_SPIDER;
			why = "изгнал паука";
		}
		else if (type.IndexOf("Era_Zombies_") == 0)
		{
			d = PURGE_ERA;
			why = "изгнал нечисть";
		}
		if (d == 0)
			return;
		PlayerBase p = PlayerBase.Cast(killer);
		EntityAI ek = EntityAI.Cast(killer);
		if (!p && ek)
			p = PlayerBase.Cast(ek.GetHierarchyRootPlayer());
		if (p)
			VB_Add(p, d, why);
	}

	static void VB_OnConnect(PlayerBase p)
	{
		string id = VB_Id(p);
		if (id == "" || !s_VB_Tier)
			return;
		float v = VB_Get(p);
		s_VB_Tier.Set(id, VB_AR_Favor.VB_TierOf(v));
		VB_Send(p, v, 0);
	}

	// ------------------------------------------------------------ стражи
	// обидел рощу: Благословение снято, стражи злы HOSTILE_MS
	static void VB_Anger(PlayerBase p, string why)
	{
		string id = VB_Id(p);
		if (id == "" || !s_VB_Hostile)
			return;
		VB_AR_Blessing.VB_Break(p, why);
		s_VB_Hostile.Set(id, g_Game.GetTime() + HOSTILE_MS);
	}

	// вырезал сердцевину — до рестарта Янтарный оберег ему не помогает
	static void VB_MarkCutter(PlayerBase p)
	{
		string id = VB_Id(p);
		if (id == "")
			return;
		if (!s_VB_Cutters)
			s_VB_Cutters = new map<string, bool>;
		bool had = p.VB_HasAmulet() && !s_VB_Cutters.Contains(id);
		s_VB_Cutters.Set(id, true);
		if (had)
			NotificationSystem.SendNotificationToPlayerExtended(p, 7, "Янтарный оберег потускнел", "Лес не простит вырезанной сердцевины: до рестарта оберег тебя не защитит.");
	}

	static bool VB_IsCutter(PlayerBase p)
	{
		if (!s_VB_Cutters || s_VB_Cutters.Count() == 0)
			return false;
		string id = VB_Id(p);
		return id != "" && s_VB_Cutters.Contains(id);
	}

	// стражи не нападают первыми: не злы, и Благословение, ступень Свой и выше или Янтарный оберег. Оберег не помогает
	// Врагу леса и Проклятому: иначе один предмет снимал бы наказание (волны по следу — тоже стражи)
	static bool VB_Peaceful(PlayerBase p)
	{
		string id = VB_Id(p);
		if (id == "")
			return false;
		int until;
		if (s_VB_Hostile && s_VB_Hostile.Find(id, until) && until > g_Game.GetTime())
			return false;
		if (VB_AR_Blessing.VB_Has(p))
			return true;
		int tier = VB_TierCached(p);
		if (tier >= VB_AR_Favor.TIER_OWN)
			return true;
		if (tier <= VB_AR_Favor.TIER_ENEMY)
			return false;
		return p.VB_AmuletWorks();
	}

	// ------------------------------------------------------------ волны за рейтинг
	static void VB_WaveTick()
	{
		array<Man> players = new array<Man>;
		g_Game.GetPlayers(players);
		int now = g_Game.GetTime();
		PlayerBase p;
		string id;
		int tier;
		int next;
		foreach (Man m : players)
		{
			p = PlayerBase.Cast(m);
			id = VB_Id(p);
			if (id == "" || !p.IsAlive())
				continue;
			tier = VB_TierNow(p);			// заодно возврат к нулю и кэш ступени
			s_VB_Tier.Set(id, tier);
			if (tier > VB_AR_Favor.TIER_ENEMY)
				continue;
			if (s_VB_NextWave.Find(id, next) && next > now)
				continue;
			if (VB_AR_Pulse.VB_NightFactor() < VB_AR_Pulse.NIGHT_FULL_ABOVE)
				continue;
			float chance = ENEMY_CHANCE;
			if (tier == VB_AR_Favor.TIER_CURSED)
				chance = CURSED_CHANCE;
			if (Math.RandomFloat01() >= chance)
				continue;
			if (!VB_WaveSpotOk(p))
				continue;

			int cooldown = ENEMY_COOLDOWN_MS;
			if (tier == VB_AR_Favor.TIER_CURSED)
				cooldown = CURSED_COOLDOWN_MS;
			s_VB_NextWave.Set(id, now + cooldown);
			VB_StartWave(p, tier);
		}
		if (s_VB_Dirty)
			VB_Save();
	}

	// ночь проверена; здесь — в лесу или у рощи, не в машине, не под крышей, не на базе (и «Лес помнит», VB_AR_MemoryServer.c)
	static bool VB_WaveSpotOk(PlayerBase p)
	{
		if (p.IsInVehicle() || MiscGameplayFunctions.IsUnderRoof(p))
			return false;
		vector pos = p.GetPosition();
		bool wild = VB_AR_Heart.VB_NearestServer(pos, GROVE_NEAR) != null;
		array<Object> objs = new array<Object>;
		if (!wild)
		{
			g_Game.GetObjectsAtPosition(pos, FOREST_RADIUS, objs, null);
			int trees = 0;
			foreach (Object o : objs)
			{
				if (o && o.IsTree())
					trees++;
			}
			wild = trees >= FOREST_TREES;
		}
		if (!wild)
			return false;
		objs.Clear();
		g_Game.GetObjectsAtPosition(pos, BASE_RADIUS, objs, null);
		foreach (Object b : objs)
		{
			if (b && b.IsKindOf("TerritoryFlag"))
				return false;
		}
		return true;
	}

	protected static void VB_StartWave(PlayerBase p, int tier)
	{
		vector pos = p.GetPosition();
		float a = Math.RandomFloat(0, Math.PI2);
		vector howl = Vector(pos[0] + Math.Cos(a) * 60, pos[1], pos[2] + Math.Sin(a) * 60);
		howl[1] = g_Game.SurfaceY(howl[0], howl[2]) + 1;
		if (p.GetIdentity())
			g_Game.RPCSingleParam(p, VB_AR_Favor.RPC_HOWL, new Param1<vector>(howl), true, p.GetIdentity());
		NotificationSystem.SendNotificationToPlayerExtended(p, 6, "Лес помнит тебя…", "");
		Print("[VB_AncientRoot] расположение леса: " + VB_AR_ItemsServer.VB_Who(p) + " — " + VB_AR_Favor.VB_TierName(tier) + ", волна по следу");
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_SpawnWave, HOWL_LEAD_MS, false, p, tier);
	}

	static void VB_SpawnWave(PlayerBase p, int tier)
	{
		// за 12 с после воя игрок мог выйти, сесть в машину, зайти в дом или на базу, или рассвело — волны нет
		if (!p || !p.IsAlive() || !p.GetIdentity())
			return;
		if (VB_AR_Pulse.VB_NightFactor() < VB_AR_Pulse.NIGHT_FULL_ABOVE || !VB_WaveSpotOk(p))
		{
			Print("[VB_AncientRoot] расположение леса: " + VB_AR_ItemsServer.VB_Who(p) + " — волна по следу не пришла (ушёл из леса)");
			return;
		}
		TStringArray types = new TStringArray;
		int wolves = Math.RandomIntInclusive(2, 3);
		int zanozas = 0;
		if (tier == VB_AR_Favor.TIER_CURSED)
		{
			wolves = Math.RandomIntInclusive(3, 4);
			zanozas = Math.RandomIntInclusive(1, 2);
		}
		for (int i = 0; i < wolves; i++)
			types.Insert(VB_RootGuards.WOLF);
		for (int k = 0; k < zanozas; k++)
			types.Insert(VB_RootGuards.ZANOZA);
		// волна за рейтинг — без шанса на отросток при разделке (не ферма)
		VB_AR_Horde.VB_SpawnWaveTypes(p.GetPosition(), types, 25, 45, false);
	}

	// ------------------------------------------------------------ файл
	protected static void VB_MarkDirty()
	{
		if (s_VB_Dirty)
			return;
		s_VB_Dirty = true;
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_Save, SAVE_DELAY_MS, false);
	}

	static void VB_Save()
	{
		if (!s_VB_File || !s_VB_Dirty)
			return;
		s_VB_Dirty = false;
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_Save);
		if (!FileExist(VB_AR_Nests.DIR_MODS))
			MakeDirectory(VB_AR_Nests.DIR_MODS);
		if (!FileExist(VB_AR_Nests.DIR))
			MakeDirectory(VB_AR_Nests.DIR);
		string err;
		if (!JsonFileLoader<VB_AR_FavorFile>.SaveFile(FILE_TMP, s_VB_File, err))
		{
			Print("[VB_AncientRoot] расположение леса: не записался favor_tmp.json: " + err);
			s_VB_Dirty = true;
			return;
		}
		// старт подхватит favor_tmp.json, если запись оборвётся между удалением и копированием
		if (FileExist(FILE))
			DeleteFile(FILE);
		if (CopyFile(FILE_TMP, FILE))
			DeleteFile(FILE_TMP);
		else
			Print("[VB_AncientRoot] расположение леса: favor.json не обновился (данные в favor_tmp.json)");
	}
}
