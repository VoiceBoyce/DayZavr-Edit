// Голос боссов (ТЗ 2.3): звуки мутантов-доноров SFP и MSFZ по состоянию босса. Звук — только у клиента,
// на сервере (и безголовом клиенте) клиентские методы сразу выходят.
//
// Почему свой проигрыватель, а не ванильный механизм зомби:
//  - ванильный InfectedSoundEventHandler ищет наборы "<ClassName()>_<Состояние>_SoundSet", а ClassName() — имя
//    СКРИПТОВОГО класса. У всех боссов это VB_AB_BossBase (лог клиента: «No sound callback for : 'VB_AB_BossBase'
//    in state: 'CalmMove'»), поэтому голос модели по такому имени не различить;
//  - голос из анимаций (CfgVehicles <класс> AnimEvents SoundVoice, DayZCreatureAIType) пришлось бы дописывать
//    в каждый класс bosses.hpp, а попадания (LightHit/HeavyHit) у боссов всё равно молчат: анимаций попадания нет
//    (EvaluateDamageHitAnimation = false), события смерти в SoundVoice нет вовсе.
// Поэтому VB_AB_BossBase отдаёт сюда все события, а модель берётся из имени класса.
//
// Наборы: VB_AB_Voice_<Модель>_<Событие>_SoundSet (cfg/voices_sets.hpp, генерирует tools/ab_voices.py).
// Модель: поле vbABVoice класса в CfgVehicles, если оно есть, иначе из имени: VB_AB_MozgoTrasRed_Fire -> MozgoTrasRed.
// Набора нет — тишина без предупреждений в логе.
//
// Откуда события:
//   состояние ИИ (спокоен / тревога / погоня) — HandleSoundEvents босса (ZombieBase зовёт его при синхронизации);
//     звук состояния повторяет таймер (один на всех боссов), следующий — через паузу ПОСЛЕ конца предыдущего;
//   удар, прыжок, «зов» (крик при входе в погоню) — анимационные события OnSoundVoiceEvent (id ванильного SoundVoice);
//   попадание — сервер (EEHitBy) шлёт RPC_VOICE, не чаще SRV_PAIN_SEC; крик способности — ServerSay(EV_SCREAM);
//   смерть — OnDamageDestroyed у клиента (запасной путь — таймер видит !IsAlive()).
// У босса одновременно звучит один голос: разовый звук прерывает звук не важнее себя
// (смерть > крик > удар и боль > состояние), звук состояния ждёт тишины.

class VB_AB_VoiceState
{
	VB_AB_BossBase m_Boss;			// слабая ссылка: станет null, если босса удалили без Detach
	string m_Model;					// Ent, Ricar, MozgoTras ... (часть имени набора)
	int m_Cat = -1;					// событие текущего состояния (EV_IDLE / EV_ALERT / EV_CHASE), -1 — молчит
	float m_NextAt;					// с: не раньше этого — следующий звук состояния
	float m_Gap;					// с: пауза после конца текущего звука состояния
	EffectSound m_Snd;				// текущий голос; владеет SEffectManager (autodestroy)
	int m_Prio;						// важность текущего голоса
	float m_StartAt;				// с: когда запущен текущий голос
	float m_AttackAt = -100;
	float m_PainAt = -100;
	float m_ScreamAt = -100;
	bool m_SeenAlive;				// клиент видел босса живым: только тогда звучит смерть
	bool m_DeadDone;				// смерть уже отыграна (или босс пришёл к клиенту мёртвым)
	// сервер: перезарядки RPC
	float m_SrvPainAt = -100;
	float m_SrvScreamAt = -100;
	float m_SrvOtherAt = -100;
};

class VB_AB_Voice
{
	// события (имена — VB_EventName, они же в tools/ab_voices.py EVENTS)
	static const int EV_IDLE = 0;		// спокоен
	static const int EV_ALERT = 1;		// встревожен
	static const int EV_CHASE = 2;		// погоня, бой
	static const int EV_ATTACK = 3;		// удар
	static const int EV_SCREAM = 4;		// крик
	static const int EV_PAIN = 5;		// попадание
	static const int EV_DEATH = 6;		// смерть

	static const int RPC_VOICE = 0x56414204;	// «VAB» 04: сервер -> клиенты, Param1<int> событие

	// id ванильных AnimEvents SoundVoice зомби (P:\DZ\characters\zombies\config.cpp, ZmbM_HermitSkinny_Base)
	static const int ANIM_LIGHTHIT = 1;
	static const int ANIM_HEAVYHIT = 2;
	static const int ANIM_ATTACK = 5;
	static const int ANIM_JUMP = 10;
	static const int ANIM_LAND = 11;
	static const int ANIM_CALL = 20;		// CallToArmsShort: «зов» при входе в погоню

	static const int PRIO_STATE = 1;
	static const int PRIO_HIT = 2;			// удар и боль
	static const int PRIO_SCREAM = 3;
	static const int PRIO_DEATH = 4;

	static const int TICK_MS = 300;
	static const float HEAR_STATE = 90;		// м: камера дальше — звуки состояния не заводим (дальность наборов 110–170 м,
	static const float HEAR_ONESHOT = 140;	// м:   слышно ~0,45 дальности)
	static const vector MOUTH = "0 1.5 0";	// точка звука в осях модели (голова отшельника)
	static const float BUSY_MIN = 0.8;		// с: только что запущенный звук считается звучащим (заголовок ещё грузится)
	static const float AFTER_ONESHOT = 1.5;	// с: после разового звука звук состояния — не сразу

	// паузы после конца звука состояния и первая задержка при смене состояния, с (ванильный зомби: 1–4 с)
	static const float IDLE_GAP_MIN = 9;
	static const float IDLE_GAP_MAX = 16;
	static const float ALERT_GAP_MIN = 5;
	static const float ALERT_GAP_MAX = 9;
	static const float CHASE_GAP_MIN = 3.5;
	static const float CHASE_GAP_MAX = 6;
	static const float IDLE_FIRST_MIN = 3;
	static const float IDLE_FIRST_MAX = 8;
	static const float ALERT_FIRST_MIN = 0.5;
	static const float ALERT_FIRST_MAX = 2;
	static const float CHASE_FIRST_MIN = 0.5;
	static const float CHASE_FIRST_MAX = 1.5;

	// клиент: перезарядка разовых звуков, с
	static const float ATTACK_CD = 0.9;
	static const float PAIN_CD = 1.2;
	static const float SCREAM_CD = 6;
	// сервер: не чаще, с
	static const float SRV_PAIN_SEC = 2;
	static const float SRV_SCREAM_SEC = 8;
	static const float SRV_OTHER_SEC = 1;

	protected static ref array<ref VB_AB_VoiceState> s_States;		// клиент
	protected static ref array<ref VB_AB_VoiceState> s_SrvStates;	// сервер: только перезарядки RPC
	protected static ref map<string, bool> s_SetOk;					// есть ли набор в CfgSoundSets
	protected static ref VB_AB_Voice s_Ticker;
	protected static bool s_TickOn;
	protected static int s_LastTick;

	// ================================================================ клиент: вызовы из VB_AB_BossBase
	// EEInit: завести босса сразу (необязательно: первый HandleSoundEvents заведёт его и сам).
	static void Attach(VB_AB_BossBase boss)
	{
		if (!boss || !VB_IsClient())
			return;
		VB_AB_VoiceState st = GetState(boss, true);
		if (st && boss.IsAlive())
			UpdateMind(st, boss.GetMindStateSynced());
	}

	// HandleSoundEvents: состояние ИИ (ZombieBase.GetMindStateSynced()).
	static void OnMindState(VB_AB_BossBase boss, int mind)
	{
		if (!boss || !VB_IsClient())
			return;
		VB_AB_VoiceState st;
		if (!boss.IsAlive())
		{
			st = GetState(boss, false);		// тело: заново не заводим
			if (st)
				CheckDeath(st);
			return;
		}
		st = GetState(boss, true);
		if (!st)
			return;
		st.m_SeenAlive = true;
		UpdateMind(st, mind);
	}

	// OnSoundVoiceEvent: голос из анимации вместо ванильного голоса отшельника (на сервере ничего не делает).
	static void OnAnimVoice(VB_AB_BossBase boss, int eventId)
	{
		if (!boss || !VB_IsClient())
			return;
		if (eventId == ANIM_ATTACK || eventId == ANIM_JUMP)
			Say(boss, EV_ATTACK);
		else if (eventId == ANIM_CALL)
			Say(boss, EV_SCREAM);
		else if (eventId == ANIM_LIGHTHIT || eventId == ANIM_HEAVYHIT)
			Say(boss, EV_PAIN);
		// ANIM_LAND и прочие — тишина
	}

	// OnDamageDestroyed: смерть. oldLevel -1 — босс появился у клиента уже мёртвым (зашёл в сетевой пузырь) — молчим.
	static void OnDeath(VB_AB_BossBase boss, int oldLevel)
	{
		if (!boss || !VB_IsClient())
			return;
		VB_AB_VoiceState st = GetState(boss, true);
		if (!st)
			return;
		if (oldLevel >= 0)
			st.m_SeenAlive = true;
		CheckDeath(st);
	}

	// OnRPC: событие от сервера (RPC_VOICE).
	static void OnRPC(VB_AB_BossBase boss, ParamsReadContext ctx)
	{
		if (!boss || !VB_IsClient())
			return;
		Param1<int> p = new Param1<int>(-1);
		if (!ctx.Read(p))
			return;
		Say(boss, p.param1);
	}

	// EEDelete: голос оборвать сразу, состояние убрать (и на сервере — перезарядки).
	static void Detach(VB_AB_BossBase boss)
	{
		if (!boss || !GetGame())
			return;
		int i = FindIdx(s_SrvStates, boss);
		if (i >= 0)
			s_SrvStates.Remove(i);
		if (!VB_IsClient())
			return;
		i = FindIdx(s_States, boss);
		if (i < 0)
			return;
		StopVoice(s_States[i]);
		s_States.Remove(i);
	}

	// Разовый звук у клиента: EV_ATTACK, EV_SCREAM, EV_PAIN, EV_DEATH (звуки состояний ведёт таймер).
	static void Say(VB_AB_BossBase boss, int ev)
	{
		if (!boss || !VB_IsClient())
			return;
		VB_AB_VoiceState st = GetState(boss, true);
		if (!st || st.m_DeadDone)
			return;
		if (ev == EV_DEATH)
		{
			st.m_SeenAlive = true;
			CheckDeath(st);
			return;
		}
		if (!boss.IsAlive())
			return;
		st.m_SeenAlive = true;
		float now = Now();
		if (ev == EV_ATTACK)
		{
			if (now - st.m_AttackAt < ATTACK_CD)
				return;
			st.m_AttackAt = now;
		}
		else if (ev == EV_PAIN)
		{
			if (now - st.m_PainAt < PAIN_CD)
				return;
			st.m_PainAt = now;
		}
		else if (ev == EV_SCREAM)
		{
			if (now - st.m_ScreamAt < SCREAM_CD)
				return;
			st.m_ScreamAt = now;
		}
		else
			return;
		if (!Near(boss, HEAR_ONESHOT))
			return;
		int prio = PrioOf(ev);
		if (IsBusy(st, now) && st.m_Prio > prio)
			return;		// более важный голос не перебиваем
		if (PlayEv(st, ev, prio, now))
			st.m_NextAt = Math.Max(st.m_NextAt, now + AFTER_ONESHOT);
	}

	// ================================================================ сервер
	// Попросить клиентов проиграть разовый звук (EEHitBy -> EV_PAIN; способности -> EV_SCREAM). С перезарядкой.
	static void ServerSay(VB_AB_BossBase boss, int ev)
	{
		if (!boss || !GetGame().IsServer() || !boss.IsAlive())
			return;
		VB_AB_VoiceState st = SrvState(boss);
		float now = Now();
		if (ev == EV_PAIN)
		{
			if (now - st.m_SrvPainAt < SRV_PAIN_SEC)
				return;
			st.m_SrvPainAt = now;
		}
		else if (ev == EV_SCREAM)
		{
			if (now - st.m_SrvScreamAt < SRV_SCREAM_SEC)
				return;
			st.m_SrvScreamAt = now;
		}
		else
		{
			if (now - st.m_SrvOtherAt < SRV_OTHER_SEC)
				return;
			st.m_SrvOtherAt = now;
		}
		if (!GetGame().IsMultiplayer())
		{
			Say(boss, ev);		// одиночная игра: сервер и клиент — один процесс
			return;
		}
		bool sure = (ev == EV_SCREAM);	// боль терпит потерю пакета, крик способности — нет
		GetGame().RPCSingleParam(boss, RPC_VOICE, new Param1<int>(ev), sure);
	}

	protected static VB_AB_VoiceState SrvState(VB_AB_BossBase boss)
	{
		if (!s_SrvStates)
			s_SrvStates = new array<ref VB_AB_VoiceState>;
		int i = FindIdx(s_SrvStates, boss);
		if (i >= 0)
			return s_SrvStates[i];
		VB_AB_VoiceState st = new VB_AB_VoiceState;
		st.m_Boss = boss;
		s_SrvStates.Insert(st);
		return st;
	}

	// ================================================================ помощники
	static string VB_EventName(int ev)
	{
		if (ev == EV_IDLE)
			return "Idle";
		if (ev == EV_ALERT)
			return "Alert";
		if (ev == EV_CHASE)
			return "Chase";
		if (ev == EV_ATTACK)
			return "Attack";
		if (ev == EV_SCREAM)
			return "Scream";
		if (ev == EV_PAIN)
			return "Pain";
		if (ev == EV_DEATH)
			return "Death";
		return "";
	}

	// Модель голоса: vbABVoice из конфига класса или имя класса без префикса и палитры.
	static string VB_ModelOf(Object boss)
	{
		if (!boss)
			return "";
		string m = boss.ConfigGetString("vbABVoice");
		if (m != "")
			return m;
		string tn = boss.GetType();
		if (tn.IndexOf("VB_AB_") == 0)
		{
			int len = tn.Length() - 6;
			tn = tn.Substring(6, len);
		}
		int k = tn.LastIndexOf("_");
		if (k > 0)
			tn = tn.Substring(0, k);
		return tn;
	}

	static string VB_SetName(string model, int ev)
	{
		return "VB_AB_Voice_" + model + "_" + VB_EventName(ev) + "_SoundSet";
	}

	protected static bool VB_SetExists(string ss)
	{
		if (!s_SetOk)
			s_SetOk = new map<string, bool>;
		bool ok;
		if (s_SetOk.Find(ss, ok))
			return ok;
		ok = GetGame().ConfigIsExisting("CfgSoundSets " + ss);
		s_SetOk.Set(ss, ok);
		return ok;
	}

	protected static bool VB_IsClient()
	{
		if (!GetGame())
			return false;
		return !GetGame().IsHeadlessOrDedicatedServer();
	}

	protected static float Now()
	{
		return GetGame().GetTime() / 1000.0;
	}

	protected static bool Near(Object boss, float dist)
	{
		// аргументы — через локальные (затирание результата вызова в списке аргументов Enforce)
		vector cam = GetGame().GetCurrentCameraPosition();
		vector pos = boss.GetPosition();
		return vector.DistanceSq(cam, pos) <= dist * dist;
	}

	protected static int CatOf(int mind)
	{
		if (mind == DayZInfectedConstants.MINDSTATE_CALM)
			return EV_IDLE;
		if (mind == DayZInfectedConstants.MINDSTATE_DISTURBED || mind == DayZInfectedConstants.MINDSTATE_ALERTED)
			return EV_ALERT;
		if (mind == DayZInfectedConstants.MINDSTATE_CHASE || mind == DayZInfectedConstants.MINDSTATE_FIGHT)
			return EV_CHASE;
		return -1;		// COWER, мёртв
	}

	protected static int PrioOf(int ev)
	{
		if (ev == EV_DEATH)
			return PRIO_DEATH;
		if (ev == EV_SCREAM)
			return PRIO_SCREAM;
		if (ev == EV_ATTACK || ev == EV_PAIN)
			return PRIO_HIT;
		return PRIO_STATE;
	}

	protected static float GapOf(int cat)
	{
		if (cat == EV_CHASE)
			return Math.RandomFloat(CHASE_GAP_MIN, CHASE_GAP_MAX);
		if (cat == EV_ALERT)
			return Math.RandomFloat(ALERT_GAP_MIN, ALERT_GAP_MAX);
		return Math.RandomFloat(IDLE_GAP_MIN, IDLE_GAP_MAX);
	}

	protected static float FirstDelayOf(int cat)
	{
		if (cat == EV_CHASE)
			return Math.RandomFloat(CHASE_FIRST_MIN, CHASE_FIRST_MAX);
		if (cat == EV_ALERT)
			return Math.RandomFloat(ALERT_FIRST_MIN, ALERT_FIRST_MAX);
		return Math.RandomFloat(IDLE_FIRST_MIN, IDLE_FIRST_MAX);
	}

	protected static int FindIdx(array<ref VB_AB_VoiceState> list, VB_AB_BossBase boss)
	{
		if (!list)
			return -1;
		for (int i = list.Count() - 1; i >= 0; i--)
		{
			VB_AB_VoiceState st = list[i];
			if (!st || !st.m_Boss)
			{
				list.Remove(i);		// босса удалили без Detach
				continue;
			}
			if (st.m_Boss == boss)
				return i;
		}
		return -1;
	}

	protected static VB_AB_VoiceState GetState(VB_AB_BossBase boss, bool create)
	{
		if (!s_States)
			s_States = new array<ref VB_AB_VoiceState>;
		int i = FindIdx(s_States, boss);
		if (i >= 0)
			return s_States[i];
		if (!create || boss.ToDelete())
			return null;
		VB_AB_VoiceState st = new VB_AB_VoiceState;
		st.m_Boss = boss;
		st.m_Model = VB_ModelOf(boss);
		st.m_SeenAlive = boss.IsAlive();
		s_States.Insert(st);
		EnsureTicker();
		return st;
	}

	protected static void UpdateMind(VB_AB_VoiceState st, int mind)
	{
		if (st.m_DeadDone)
			return;
		int cat = CatOf(mind);
		if (cat == st.m_Cat)
			return;
		int prev = st.m_Cat;
		st.m_Cat = cat;
		float now = Now();
		// звук прежнего состояния: замолчать, если босс затих или стал злее (погоня — не ждать конца спокойного)
		if (st.m_Prio == PRIO_STATE && IsBusy(st, now) && (cat < 0 || cat > prev))
			StopVoice(st);
		if (cat >= 0)
			st.m_NextAt = now + FirstDelayOf(cat);
	}

	protected static void CheckDeath(VB_AB_VoiceState st)
	{
		if (st.m_DeadDone)
			return;
		st.m_DeadDone = true;
		st.m_Cat = -1;
		if (st.m_SeenAlive && st.m_Boss && Near(st.m_Boss, HEAR_ONESHOT))
			PlayEv(st, EV_DEATH, PRIO_DEATH, Now());
		else
			StopVoice(st);
	}

	protected static bool IsBusy(VB_AB_VoiceState st, float now)
	{
		if (!st.m_Snd)
			return false;
		if (now - st.m_StartAt < BUSY_MIN)
			return true;
		return st.m_Snd.IsSoundPlaying();
	}

	protected static bool PlayEv(VB_AB_VoiceState st, int ev, int prio, float now)
	{
		if (!st.m_Boss)
			return false;
		string ss = VB_SetName(st.m_Model, ev);
		if (!VB_SetExists(ss))
			return false;
		StopVoice(st);
		VB_AB_BossBase boss = st.m_Boss;
		// без плавности (fade 0): короткие голоса с fade длиннее файла дают SoundError (E60)
		vector pos = boss.GetPosition();
		EffectSound es = SEffectManager.CreateSound(ss, pos, 0, 0, false, false);
		if (!es)
			return false;
		es.SetParent(boss);
		es.SetLocalPosition(MOUTH);
		es.SetSoundAutodestroy(true);
		es.SoundPlay();
		st.m_Snd = es;
		st.m_Prio = prio;
		st.m_StartAt = now;
		return true;
	}

	protected static void StopVoice(VB_AB_VoiceState st)
	{
		if (st.m_Snd)
			SEffectManager.DestroyEffect(st.m_Snd);		// fade 0 -> удаляется сразу, звук обрывается
		st.m_Snd = null;
		st.m_Prio = 0;
	}

	// ================================================================ таймер (клиент, один на всех боссов)
	protected static void EnsureTicker()
	{
		if (!s_Ticker)
			s_Ticker = new VB_AB_Voice;
		// очередь вызовов могла очиститься при смене миссии — тогда таймер давно молчит, перезапускаем
		if (s_TickOn && GetGame().GetTime() - s_LastTick < TICK_MS * 3)
			return;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(s_Ticker.VB_Tick);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(s_Ticker.VB_Tick, TICK_MS, true);
		s_TickOn = true;
		s_LastTick = GetGame().GetTime();
	}

	protected static void StopTicker()
	{
		if (s_Ticker && GetGame())
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(s_Ticker.VB_Tick);
		s_TickOn = false;
	}

	void VB_Tick()
	{
		s_LastTick = GetGame().GetTime();
		if (!s_States || s_States.Count() == 0)
		{
			StopTicker();
			return;
		}
		float now = Now();
		vector cam = GetGame().GetCurrentCameraPosition();
		for (int i = s_States.Count() - 1; i >= 0; i--)
		{
			VB_AB_VoiceState st = s_States[i];
			if (!st || !st.m_Boss || st.m_Boss.ToDelete())
			{
				if (st)
					StopVoice(st);
				s_States.Remove(i);
				continue;
			}
			if (!st.m_Boss.IsAlive())
			{
				CheckDeath(st);
				if (!IsBusy(st, now))
				{
					StopVoice(st);
					s_States.Remove(i);		// тело: голос смерти отзвучал, следить больше не за чем
				}
				continue;
			}
			TickState(st, now, cam);
		}
	}

	protected static void TickState(VB_AB_VoiceState st, float now, vector cam)
	{
		if (st.m_Cat < 0)
			return;
		if (IsBusy(st, now))
		{
			if (st.m_Prio == PRIO_STATE)
				st.m_NextAt = now + st.m_Gap;		// пауза отсчитывается от конца звука
			else
				st.m_NextAt = Math.Max(st.m_NextAt, now + AFTER_ONESHOT);
			return;
		}
		if (now < st.m_NextAt)
			return;
		vector pos = st.m_Boss.GetPosition();
		if (vector.DistanceSq(cam, pos) > HEAR_STATE * HEAR_STATE)
		{
			st.m_NextAt = now + 2;		// далеко: проверить позже
			return;
		}
		st.m_Gap = GapOf(st.m_Cat);
		st.m_NextAt = now + st.m_Gap;
		PlayEv(st, st.m_Cat, PRIO_STATE, now);
	}
};
