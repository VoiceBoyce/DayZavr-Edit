// «Защита рощи» — нашествие пауков MSFZ на живую рощу (ТЗ_сервер_18.md, ветка 2в п.1; выбор пользователя 2026-11-16:
// только пауки, награда — сразу ступень вверх, числа подобрать на стенде) — только сервер.
//
// Жребий: раз в SiegeCheckMinutes ночью (settings.json), если нашествия нет. Кандидаты — живые рощи, у которых игрок
// ближе SiegePlayerDistance и с прошлого нашествия на них прошло SiegeCooldownMinutes; шанс SiegeChance на проверку.
// Админ — кнопкой «Нашествие нечисти» в редакторе /grove (без ночи и перерыва). На сервере одно нашествие за раз.
//
// Ход: игрокам в CALL_DIST — уведомление и треск корней с той стороны (RPC_SIEGE). Через WARN_MS (успеть добежать) —
// первая волна, дальше раз в WAVE_GAP_MS, всего три; пауки (VB_AR_Horde, 40–60 м от сердцевины) идут к сердцевине:
// число — ступень + номер волны (росток 2/3/4, роща 3/4/5, древняя 4/5/6). Ведём их сами (OverrideHeading/Speed, как
// ванильная кинематика DayZAnimal.c): у кого игрок ближе FIGHT_DIST — отпускаем, его ИИ дерётся; спящих от ловушки
// не трогаем, раненых тёрном ведём шагом. Паук у сердцевины (GNAW_DIST) грызёт: сила рощи −GNAW_PER_SEC в секунду
// с паука, но не больше GNAW_MAX_PER_SEC всего. Без защиты роща держится ~4–5 минут от зова (пользователь 2026-11-16:
// «за 1,5 минуты игрок не успеет» — было 2/с с паука без потолка и без паузы перед первой волной, роща гибла за ~1,5 мин).
// Сила и «в осаде» синхронизируются (худ, «Коснуться рощи», треск корней у клиента; роща не принимает даров,
// сбор и отлом закрыты).
//   Победа — все три волны вылезли и перебиты не позже FINISH_MS после третьей: каждому защитнику (по SteamID провёл
//     в DEFEND_DIST от рощи суммарно не меньше DEFEND_MIN_MS за нашествие — время копим по тикам — и жив в момент
//     победы) — +WIN_FAVOR расположения, янтарь и пыльца у ног (у древней ещё вкрапление);
//     роща сразу на ступень вверх (VB_AR_Nests.VB_OnGroveDefended).
//   Сила 0 — роща увядает (как вырезанная, но без добычи и без штрафа): гнездо гаснет, прорастание как после резки.
//   Время вышло — пауки уходят, награды нет, сила возвращается.
// Убитый паук и так даёт +0,1 расположения убившему (VB_AR_FavorServer.VB_OnCreatureKilled).

class VB_AR_Siege
{
	static const int TICK_MS = 500;
	static const int SCHEDULE_MS = 60000;		// жребий считает минуты
	static const int WAVES = 3;
	static const int WARN_MS = 30000;			// от зова до первой волны: успеть добежать
	static const int WAVE_GAP_MS = 90000;
	static const int FINISH_MS = 360000;		// после третьей волны — на победу
	static const float SPAWN_MIN = 40.0;
	static const float SPAWN_MAX = 60.0;
	static const float GNAW_DIST = 6.5;			// м от сердцевины (по горизонтали): паук грызёт. Ближе 4–5 м их не пускают
												// корни-арки и коллизия сердца (стенд 2026-11-16)
	static const float GNAW_PER_SEC = 0.15;		// силы в секунду с одного паука
	static const float GNAW_MAX_PER_SEC = 0.5;	// всего, сколько бы пауков ни грызло (из 100 — не меньше 200 с грызни)
	static const float FIGHT_DIST = 12.0;		// игрок ближе к пауку — паук дерётся сам, не идёт к роще
	static const float CALL_DIST = 400.0;		// кому слать зов и итог
	static const float DEFEND_DIST = 80.0;		// кто считается защитником
	static const int DEFEND_MIN_MS = 60000;		// столько всего пробыть в DEFEND_DIST за нашествие, чтобы получить награду
	static const float SOUND_DIST = 40.0;		// треск — на этом расстоянии от игрока в сторону рощи
	static const float WIN_FAVOR = 10.0;
	static const float MOVE_SPEED = 3.0;		// шкала зверей (ваниль DayZAnimal.c): 2 шаг, 3 рысь, 5 бег
	static const float SLOW_SPEED = 2.0;
	static const float LEAVE_SPEED = 5.0;
	static const int LEAVE_MS = 20000;			// поражение или конец: пауки бегут прочь, потом пропадают
	static const float LEAVE_KEEP_DIST = 60.0;	// не пропадают на глазах: игрок ближе — остаются (их уберёт волна)
	protected static const ref TStringArray SPIDERS = {"MSFZ_Mutant_Spider1", "MSFZ_Mutant_Spider2", "MSFZ_Mutant_Spider3"};
	static const string AMBER = "VB_AR_Amber";
	static const string AMBER_INCL = "VB_AR_AmberInclusion";
	static const string POLLEN = "VB_AR_Spores";

	static const int END_WIN = 1;
	static const int END_WITHER = 2;
	static const int END_TIMEOUT = 3;
	static const int END_GONE = 4;				// сердцевину вырезали или роща ушла с карты

	protected static ref VB_AR_Siege s_VB_Active;
	protected static ref map<string, int> s_VB_LastAt;		// место рощи -> g_Game.GetTime() начала нашествия
	protected static bool s_VB_Started;
	protected static int s_VB_Minutes;

	protected VB_AR_Heart m_VB_Heart;
	protected vector m_VB_Center;
	protected int m_VB_Stage;
	protected float m_VB_Power = 100;
	protected int m_VB_Wave;
	protected int m_VB_LastWaveMs;
	protected int m_VB_StartMs;
	protected int m_VB_Spawned;
	protected ref array<ref VB_AR_Horde> m_VB_Hordes;
	protected ref array<EntityAI> m_VB_Spiders;		// живые пауки, которых ведём
	protected ref array<EntityAI> m_VB_Seen;		// все уже учтённые пауки (живые и мёртвые; удалённые — null)
	protected ref map<string, int> m_VB_DefendMs;	// SteamID -> сколько мс провёл в DEFEND_DIST за нашествие
	protected bool m_VB_Leaving;
	protected int m_VB_LeaveUntil;
	protected string m_VB_Why;

	// ------------------------------------------------------------------ запуск и жребий

	static void VB_Start()
	{
		if (s_VB_Started)
			return;
		s_VB_Started = true;
		s_VB_LastAt = new map<string, int>;
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_Schedule, SCHEDULE_MS, true);
	}

	static bool VB_IsActive()
	{
		return s_VB_Active != null;
	}

	protected static string VB_Key(vector p)
	{
		int x = Math.Round(p[0]);
		int z = Math.Round(p[2]);
		return x.ToString() + "_" + z.ToString();
	}

	static void VB_Schedule()
	{
		VB_AR_Nests nests = VB_AR_Nests.VB_Get();
		if (!nests || !nests.m_VB_Settings)
			return;
		VB_AR_NestSettings st = nests.m_VB_Settings;
		s_VB_Minutes++;
		if (s_VB_Minutes < st.SiegeCheckMinutes)
			return;
		s_VB_Minutes = 0;
		if (s_VB_Active || st.SiegeChance <= 0)
			return;
		if (VB_AR_Pulse.VB_NightFactor() < VB_AR_Pulse.NIGHT_FULL_ABOVE)
			return;

		array<VB_AR_Heart> cands = new array<VB_AR_Heart>;
		array<Man> players = new array<Man>;
		g_Game.GetPlayers(players);
		float nearSq = st.SiegePlayerDistance * st.SiegePlayerDistance;
		int coolMs = st.SiegeCooldownMinutes * 60000;
		int now = g_Game.GetTime();
		int last;
		bool near;
		array<VB_AR_Heart> hearts = VB_AR_Heart.VB_ServerHeartsList();
		foreach (VB_AR_Heart h : hearts)
		{
			if (!h || h.VB_IsDead())
				continue;
			if (s_VB_LastAt.Find(VB_Key(h.GetPosition()), last) && now - last < coolMs)
				continue;
			near = false;
			foreach (Man m : players)
			{
				if (m && m.IsAlive() && vector.DistanceSq(m.GetPosition(), h.GetPosition()) < nearSq)
				{
					near = true;
					break;
				}
			}
			if (near)
				cands.Insert(h);
		}
		if (cands.Count() == 0)
			return;
		if (Math.RandomFloat01() >= st.SiegeChance)
			return;
		VB_Begin(cands.GetRandomElement(), "жребий");
	}

	// админ из редактора: сейчас, без ночи и перерыва; строка — ответ админу
	static string VB_BeginByAdmin(VB_AR_Heart heart, string who)
	{
		if (s_VB_Active)
			return "нашествие уже идёт в другом месте — дождись конца";
		if (!VB_Begin(heart, "админ " + who))
			return "нашествие не началось (см. лог сервера)";
		return "нашествие пауков началось";
	}

	protected static bool VB_Begin(VB_AR_Heart heart, string why)
	{
		if (s_VB_Active || !heart || heart.VB_IsDead())
			return false;
		s_VB_Active = new VB_AR_Siege(heart, why);
		return true;
	}

	// ------------------------------------------------------------------ одно нашествие

	void VB_AR_Siege(VB_AR_Heart heart, string why)
	{
		m_VB_Heart = heart;
		m_VB_Center = heart.GetPosition();
		m_VB_Stage = heart.VB_Stage();
		m_VB_Why = why;
		m_VB_Hordes = new array<ref VB_AR_Horde>;
		m_VB_Spiders = new array<EntityAI>;
		m_VB_Seen = new array<EntityAI>;
		m_VB_DefendMs = new map<string, int>;
		m_VB_StartMs = g_Game.GetTime();
		s_VB_LastAt.Set(VB_Key(m_VB_Center), m_VB_StartMs);

		heart.VB_SetSiegeServer(true, 100);
		string at = m_VB_Center.ToString();
		string stName = VB_AR_Nests.VB_StageName(m_VB_Stage);
		Print("[VB_AncientRoot] нашествие пауков на рощу в " + at + " (" + stName + "), причина: " + why);
		VB_Call("Роща зовёт на помощь", "Нечисть ползёт к корням. Защити рощу: убей пауков, пока они не выгрызли сердцевину.", true);
		m_VB_LastWaveMs = m_VB_StartMs - WAVE_GAP_MS + WARN_MS;		// первая волна — через WARN_MS (VB_Tick)
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_Tick, TICK_MS, true);
	}

	// уведомление игрокам в CALL_DIST; sound — ещё треск корней с той стороны
	protected void VB_Call(string title, string text, bool sound)
	{
		array<Man> players = new array<Man>;
		g_Game.GetPlayers(players);
		float callSq = CALL_DIST * CALL_DIST;
		PlayerBase p;
		vector pp;
		vector dir;
		float dist;
		vector spot;
		foreach (Man m : players)
		{
			p = PlayerBase.Cast(m);
			if (!p || !p.IsAlive() || !p.GetIdentity())
				continue;
			pp = p.GetPosition();
			if (vector.DistanceSq(pp, m_VB_Center) > callSq)
				continue;
			NotificationSystem.SendNotificationToPlayerExtended(p, 8, title, text);
			if (!sound)
				continue;
			dir = m_VB_Center - pp;
			dist = dir.Length();
			spot = m_VB_Center;
			if (dist > SOUND_DIST)
				spot = pp + dir.Normalized() * SOUND_DIST;
			g_Game.RPCSingleParam(p, VB_AR_Favor.RPC_SIEGE, new Param1<vector>(spot), true, p.GetIdentity());
		}
	}

	protected void VB_RaiseWave()
	{
		m_VB_Wave++;
		m_VB_LastWaveMs = g_Game.GetTime();
		int count = m_VB_Stage + m_VB_Wave;
		TStringArray types = new TStringArray;
		for (int i = 0; i < count; i++)
			types.Insert(SPIDERS.GetRandomElement());
		VB_AR_Horde h = VB_AR_Horde.VB_SpawnWaveTypes(m_VB_Center, types, SPAWN_MIN, SPAWN_MAX, true);
		string w = m_VB_Wave.ToString();
		string c = count.ToString();
		if (h)
			m_VB_Hordes.Insert(h);
		else
			Print("[VB_AncientRoot] нашествие: волна " + w + " не поднялась (потолок существ)");
		Print("[VB_AncientRoot] нашествие: волна " + w + ", пауков " + c);
	}

	// новые пауки из волн: каждого учитываем один раз (m_VB_Seen; иначе убитый, выпавший из m_VB_Spiders, вернулся бы
	// туда из списка волны и посчитался снова), вести — только живых
	protected void VB_Collect()
	{
		array<EntityAI> members;
		foreach (VB_AR_Horde h : m_VB_Hordes)
		{
			if (!h)
				continue;
			members = h.VB_Members();
			foreach (EntityAI e : members)
			{
				if (!e || m_VB_Seen.Find(e) >= 0)
					continue;
				m_VB_Seen.Insert(e);
				m_VB_Spawned++;
				if (e.IsAlive())
					m_VB_Spiders.Insert(e);
			}
		}
	}

	// защитник — SteamID, набравший в DEFEND_DIST не меньше DEFEND_MIN_MS (жив ли он сейчас — проверяет VB_Reward)
	protected bool VB_IsDefender(string id)
	{
		int ms;
		return m_VB_DefendMs.Find(id, ms) && ms >= DEFEND_MIN_MS;
	}

	protected int VB_DefenderCount()
	{
		int n = 0;
		foreach (string id, int ms : m_VB_DefendMs)
		{
			if (ms >= DEFEND_MIN_MS)
				n++;
		}
		return n;
	}

	protected bool VB_AllSpawned()
	{
		if (m_VB_Wave < WAVES)
			return false;
		foreach (VB_AR_Horde h : m_VB_Hordes)
		{
			if (h && h.VB_LeftToSpawn() > 0)
				return false;
		}
		return true;
	}

	protected static float VB_Dist2D(vector a, vector b)
	{
		float dx = a[0] - b[0];
		float dz = a[2] - b[2];
		return Math.Sqrt(dx * dx + dz * dz);
	}

	protected static bool VB_PlayerNear(vector pos, array<Man> players, float dist)
	{
		float dSq = dist * dist;
		foreach (Man m : players)
		{
			if (m && m.IsAlive() && vector.DistanceSq(m.GetPosition(), pos) < dSq)
				return true;
		}
		return false;
	}

	// вести зверя: направление на точку to (away — от неё), скорость speed; 0 — стоять мордой к точке
	protected static void VB_Drive(DayZAnimal an, vector to, float speed, bool away)
	{
		DayZAnimalInputController ic = DayZAnimalInputController.Cast(an.GetInputController());
		if (!ic)
			return;
		vector p = an.GetPosition();
		float dx = to[0] - p[0];
		float dz = to[2] - p[2];
		if (away)
		{
			dx = -dx;
			dz = -dz;
		}
		float heading = Math.Atan2(dx, dz);		// рад; как GetOrientation()[0] * DEG2RAD в ванили
		ic.OverrideBehaviourSlot(true, DayZAnimalBehaviourSlot.NON_SPECIFIC_THREAT);
		ic.OverrideBehaviourAction(true, DayZAnimalBehaviourAction.TRAVELING_INPUT);
		ic.OverrideTurnSpeed(true, Math.PI2);
		ic.OverrideMovementSpeed(true, speed);
		ic.OverrideHeading(true, heading);
		ic.OverrideAlertLevel(true, true, 1, 0.2);
	}

	// отпустить: ИИ решает сам. Скорость сна (0) и шаг от терновой раны не снимаем — их держит состояние существа
	// (VB_AR_CreatureServer.c, те же значения, что ставит оно)
	protected static void VB_Release(DayZAnimal an)
	{
		DayZAnimalInputController ic = DayZAnimalInputController.Cast(an.GetInputController());
		if (!ic)
			return;
		ic.OverrideBehaviourSlot(false, 0);
		ic.OverrideBehaviourAction(false, 0);
		ic.OverrideTurnSpeed(false, 0);
		if (VB_AR_CreatureState.VB_AsleepServer(an))
			ic.OverrideMovementSpeed(true, 0);
		else if (VB_AR_CreatureState.VB_SlowedServer(an))
			ic.OverrideMovementSpeed(true, VB_AR_CreatureState.ANIMAL_SLOW);
		else
			ic.OverrideMovementSpeed(false, 0);
		ic.OverrideHeading(false, 0);
		ic.OverrideAlertLevel(false, false, 0, 0);
	}

	void VB_Tick()
	{
		array<Man> players = new array<Man>;
		g_Game.GetPlayers(players);
		if (m_VB_Leaving)
		{
			VB_LeaveTick(players);
			return;
		}
		if (!m_VB_Heart || m_VB_Heart.VB_IsDead())
		{
			VB_End(END_GONE);
			return;
		}

		int now = g_Game.GetTime();
		if (m_VB_Wave < WAVES && now - m_VB_LastWaveMs >= WAVE_GAP_MS)
			VB_RaiseWave();
		VB_Collect();

		// защитники: копим время рядом с рощей по SteamID (один тик — TICK_MS)
		string id;
		int had;
		float defSq = DEFEND_DIST * DEFEND_DIST;
		foreach (Man m : players)
		{
			if (!m || !m.IsAlive() || !m.GetIdentity())
				continue;
			if (vector.DistanceSq(m.GetPosition(), m_VB_Center) > defSq)
				continue;
			id = m.GetIdentity().GetPlainId();
			had = 0;
			m_VB_DefendMs.Find(id, had);
			m_VB_DefendMs.Set(id, had + TICK_MS);
		}

		// пауки: к сердцевине, у неё — грызть
		int alive = 0;
		int gnawing = 0;
		DayZAnimal an;
		vector sp;
		for (int i = m_VB_Spiders.Count() - 1; i >= 0; i--)
		{
			EntityAI e = m_VB_Spiders[i];
			if (!e || !e.IsAlive())
			{
				m_VB_Spiders.Remove(i);
				continue;
			}
			alive++;
			an = DayZAnimal.Cast(e);
			if (!an)
				continue;
			if (VB_AR_CreatureState.VB_AsleepServer(an))
				continue;					// спит от ловушки — пусть спит
			sp = e.GetPosition();
			if (VB_PlayerNear(sp, players, FIGHT_DIST))
			{
				VB_Release(an);				// рядом игрок — дерётся своим ИИ
				continue;
			}
			if (VB_Dist2D(sp, m_VB_Center) <= GNAW_DIST)
			{
				VB_Drive(an, m_VB_Center, 0, false);
				gnawing++;
				continue;
			}
			float speed = MOVE_SPEED;
			if (VB_AR_CreatureState.VB_SlowedServer(an))
				speed = SLOW_SPEED;
			VB_Drive(an, m_VB_Center, speed, false);
		}

		if (gnawing > 0)
		{
			float drain = gnawing * GNAW_PER_SEC;
			if (drain > GNAW_MAX_PER_SEC)
				drain = GNAW_MAX_PER_SEC;
			m_VB_Power = m_VB_Power - drain * TICK_MS / 1000.0;
			if (m_VB_Power < 0)
				m_VB_Power = 0;
			int powerInt = Math.Ceil(m_VB_Power);
			m_VB_Heart.VB_SetSiegeServer(true, powerInt);
		}
		if (m_VB_Power <= 0)
		{
			VB_End(END_WITHER);
			return;
		}
		if (VB_AllSpawned() && alive == 0)
		{
			if (m_VB_Spawned > 0)
				VB_End(END_WIN);
			else
				VB_End(END_GONE);		// ни один паук не вылез (потолок существ) — нашествия как не было
			return;
		}
		if (m_VB_Wave >= WAVES && now - m_VB_LastWaveMs >= FINISH_MS)
			VB_End(END_TIMEOUT);
	}

	protected void VB_End(int how)
	{
		string at = m_VB_Center.ToString();
		int defCount = VB_DefenderCount();
		string defN = defCount.ToString();
		// осаду снять до смены ступени: роща может встать заново другим сердцем
		if (m_VB_Heart && how != END_WITHER)
			m_VB_Heart.VB_SetSiegeServer(false, 100);
		if (how == END_WIN)
		{
			string grew = VB_AR_Nests.VB_OnGroveDefended(m_VB_Heart);		// роща может встать заново: сердце уже другое
			m_VB_Heart = null;
			VB_Reward(grew);
			Print("[VB_AncientRoot] нашествие в " + at + ": роща отбилась, защитников " + defN + "; " + grew);
		}
		else if (how == END_WITHER)
		{
			if (m_VB_Heart)
				m_VB_Heart.VB_WitherServer();
			m_VB_Heart = null;
			VB_Call("Роща пала", "Нечисть выгрызла сердцевину. Роща увяла, а лес запомнит эту ночь.", false);
			Print("[VB_AncientRoot] нашествие в " + at + ": роща увяла (сила 0), защитников " + defN);
		}
		else if (how == END_TIMEOUT)
		{
			VB_Call("Нечисть отступила", "Пауки ушли в чащу. Роща выстояла, но защитникам благодарить некого.", false);
			Print("[VB_AncientRoot] нашествие в " + at + ": время вышло, пауки уходят");
		}
		else
		{
			Print("[VB_AncientRoot] нашествие в " + at + ": прервано (сердцевины нет или пауки не вылезли)");
		}

		m_VB_Leaving = true;
		m_VB_LeaveUntil = g_Game.GetTime() + LEAVE_MS;
	}

	// победа: каждому живому защитнику (набрал DEFEND_MIN_MS у рощи) — расположение и дар у ног
	protected void VB_Reward(string grew)
	{
		array<Man> players = new array<Man>;
		g_Game.GetPlayers(players);
		PlayerBase p;
		string text = "Нечисть изгнана. Роща благодарит защитников";
		if (grew != "")
			text = text + ": " + grew;
		text = text + ".";
		foreach (Man m : players)
		{
			p = PlayerBase.Cast(m);
			if (!p || !p.IsAlive() || !p.GetIdentity())
				continue;
			if (!VB_IsDefender(p.GetIdentity().GetPlainId()))
				continue;
			VB_AR_FavorServer.VB_Add(p, WIN_FAVOR, "отстоял рощу от нечисти");
			VB_AR_ItemsServer.VB_DropAtFeet(p, AMBER);
			VB_AR_ItemsServer.VB_DropAtFeet(p, POLLEN);
			if (m_VB_Stage >= VB_AR_Heart.STAGE_ANCIENT)
				VB_AR_ItemsServer.VB_DropAtFeet(p, AMBER_INCL);
			NotificationSystem.SendNotificationToPlayerExtended(p, 8, "Роща отбилась", text);
		}
	}

	// конец: пауки бегут прочь LEAVE_MS, потом пропадают те, рядом с кем нет игроков
	protected void VB_LeaveTick(array<Man> players)
	{
		bool last = g_Game.GetTime() >= m_VB_LeaveUntil;
		DayZAnimal an;
		for (int i = m_VB_Spiders.Count() - 1; i >= 0; i--)
		{
			EntityAI e = m_VB_Spiders[i];
			if (!e || !e.IsAlive())
			{
				m_VB_Spiders.Remove(i);
				continue;
			}
			an = DayZAnimal.Cast(e);
			if (!an)
				continue;
			if (!last)
			{
				if (VB_PlayerNear(e.GetPosition(), players, FIGHT_DIST) || VB_AR_CreatureState.VB_AsleepServer(an))
					VB_Release(an);
				else
					VB_Drive(an, m_VB_Center, LEAVE_SPEED, true);
				continue;
			}
			VB_Release(an);
			if (!VB_PlayerNear(e.GetPosition(), players, LEAVE_KEEP_DIST))
				g_Game.ObjectDelete(e);
		}
		if (!last)
			return;
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_Tick);
		if (s_VB_Active == this)
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_Forget, 0, false);
	}

	// снять ссылку на себя не изнутри своего же метода
	static void VB_Forget()
	{
		s_VB_Active = null;
	}
}

modded class VB_AR_Heart
{
	// все живые сердца гнёзд на сервере (жребий нашествия)
	static array<VB_AR_Heart> VB_ServerHeartsList()
	{
		if (!s_VB_ServerHearts)
			s_VB_ServerHearts = new array<VB_AR_Heart>;
		return s_VB_ServerHearts;
	}

	void VB_SetSiegeServer(bool on, int power)
	{
		int pw = Math.Clamp(power, 0, 100);
		if (m_VB_Siege == on && m_VB_SiegePower == pw)
			return;
		m_VB_Siege = on;
		m_VB_SiegePower = pw;
		SetSynchDirty();
	}

	// увяла под нашествием: гаснет, как вырезанная, но без добычи, штрафа и молнии; прорастание — как после резки
	void VB_WitherServer()
	{
		if (m_VB_Dead)
			return;
		m_VB_Dead = true;
		m_VB_Disturbed = false;
		m_VB_Siege = false;
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_CalmServer);
		SetSynchDirty();
		if (m_VB_Heat && m_VB_Heat.IsActive())
			m_VB_Heat.SetActive(false);
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_UpdateHeatServer);
		string at = GetPosition().ToString();
		Print("[VB_AncientRoot] сердце увяло под нашествием нечисти в " + at);
		VB_AR_Nests.VB_OnHeartKilled(this);
	}

	// во время нашествия роща не принимает даров (условие действия закрыто и на клиенте)
	override bool VB_OfferServer(PlayerBase player, ItemBase item)
	{
		if (m_VB_Siege)
			return false;
		return super.VB_OfferServer(player, item);
	}
}
