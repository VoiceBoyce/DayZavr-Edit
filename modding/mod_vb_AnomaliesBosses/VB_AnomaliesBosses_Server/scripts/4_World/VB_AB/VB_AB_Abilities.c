// Способности боссов (ТЗ 4.3, 4.4). Всё решает сервер; клиентам уходят эффекты (VB_AB_Fx), экранные эффекты
// конкретного игрока — через VB_AB_PlayerFx.
//
// Цикл: босс при раскрытии поля бросает Chance каждой строки Abilities (нет или 0 — всегда) и собирает свой набор.
// Каждые 0,5 с (VB_AB_BossBase.VB_AbilityTick) выбирается готовая способность: кулдаун прошёл, в поле есть
// нужная аномалия (RequiresAnomaly), есть цель. Сначала предупреждение — частица и звук за WarnSec до удара,
// чтобы от удара можно было уйти, потом удар. Между активными способностями босса — общий перерыв GAP_SEC.
// Пассивные (Тлен, Ярость, Стая) работают сами и перерыв не занимают.

class VB_AB_Ability
{
	static const float GAP_SEC = 3.0;

	VB_AB_AbilityCfg m_Cfg;
	float m_ReadyAt;

	// Отложенные шаги перемещения не должны пережить способность (набор чистится при сбросе и смерти босса)
	void ~VB_AB_Ability()
	{
		if (GetGame())
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(MoveSteps);
	}

	void Setup(VB_AB_AbilityCfg cfg, float now)
	{
		m_Cfg = cfg;
		m_ReadyAt = now + Math.RandomFloat(2, 6);		// не всё разом сразу после раскрытия
	}

	string Kind()
	{
		return m_Cfg.Type;
	}

	float Cooldown()
	{
		if (m_Cfg.Cooldown > 0)
			return m_Cfg.Cooldown;
		return 20;
	}

	float Range()
	{
		if (m_Cfg.Range > 0)
			return m_Cfg.Range;
		return 15;
	}

	static const float MAX_DURATION = 60;		// опечатка в bosses.json не даёт минуты ослепления/метели

	float Duration(float def)
	{
		if (m_Cfg.DurationSec > 0)
			return Math.Min(m_Cfg.DurationSec, MAX_DURATION);
		return def;
	}

	float WarnSec()
	{
		return 1.0;
	}

	bool IsPassive()
	{
		return false;
	}

	bool NeedsTarget()
	{
		return true;
	}

	// Условия, кроме цели: нужная аномалия в поле
	bool CanUse(VB_AB_BossBase boss)
	{
		VB_AB_Field f = boss.VB_GetField();
		return f && !f.IsClosing() && f.HasAnyOf(m_Cfg.RequiresAnomaly);
	}

	// Ближайший живой игрок в радиусе способности, которого босс видит (не за стеной)
	PlayerBase PickTarget(VB_AB_BossBase boss)
	{
		array<PlayerBase> ps = new array<PlayerBase>;
		VB_AB_Server.PlayersNear(boss.GetPosition(), Range(), ps);
		foreach (PlayerBase p : ps)
		{
			if (VB_AB_Server.Sees(boss, p))
				return p;
		}
		return null;
	}

	void OnOpen(VB_AB_BossBase boss)
	{
	}

	// Набор сбрасывается (сброс босса, переброс поля, уход): убрать то, что способность поставила в мир
	void OnClear(VB_AB_BossBase boss)
	{
	}

	// Босс убит: поставленное способностью доживает несколько минут и уходит (VB_AB_Server.DespawnLater)
	void OnBossDead(VB_AB_BossBase boss)
	{
	}

	// Каждый тик босса, даже если способность не готова (следить за игроками, пассивы)
	void Tick(VB_AB_BossBase boss, float now)
	{
	}

	void Warn(VB_AB_BossBase boss, PlayerBase target, vector at)
	{
	}

	void Fire(VB_AB_BossBase boss, PlayerBase target, vector at)
	{
	}

	// ---------------------------------------------------------------- помощники
	// Урон способности: свой патрон VB_AB_AbilityDmg (Health 1, Shock 1,5), сила — коэффициентом
	static void Hit(VB_AB_BossBase boss, EntityAI e, float dmg)
	{
		if (!e || dmg <= 0 || !e.IsAlive())
			return;
		e.ProcessDirectDamage(DT_CUSTOM, boss, "", "VB_AB_AbilityDmg", "0 0 0", dmg);
	}

	// Стаггер (тяжёлая анимация попадания) без урона — ванильный Dummy_Heavy
	static void Stagger(VB_AB_BossBase boss, PlayerBase p)
	{
		VB_AB_PlayerFx.KnockDown(boss, p);
	}

	static void Fx(VB_AB_BossBase boss, string ptc, string snd, vector pos)
	{
		VB_AB_Fx.Send(boss, ptc, snd, pos);
	}

	static vector Ground(vector p)
	{
		return Vector(p[0], GetGame().SurfaceY(p[0], p[2]), p[2]);
	}

	// Сдвинуть игрока по шагам (толчок/притяжение): SetPosition на сервере, по 0,12 с. Шаг только по земле:
	// без стены на пути, без обрыва, не в воду и не из дома/с крыши/из бункера (раньше SetPosition по рельефу
	// проносил сквозь стены, в т.ч. внутрь чужой базы, и ронял под пол, аудит 2026-11-13).
	void MoveSteps(PlayerBase p, vector dir, float stepLen, int steps)
	{
		if (!p || !p.IsAlive() || steps <= 0 || p.IsInVehicle())
			return;
		vector cur = p.GetPosition();
		vector np = Ground(cur + dir * stepLen);
		if (!CanStep(p, cur, np, dir))
			return;
		p.SetPosition(np);
		SetVelocity(p, vector.Zero);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(MoveSteps, 120, false, p, dir, stepLen, steps - 1);
	}

	static bool CanStep(PlayerBase p, vector cur, vector np, vector dir)
	{
		if (Math.AbsFloat(cur[1] - GetGame().SurfaceY(cur[0], cur[2])) > 0.6)
			return false;		// стоит не на рельефе: пол, крыша, камень, бункер
		if (Math.AbsFloat(np[1] - cur[1]) > 1.0)
			return false;		// обрыв или крутой склон
		if (GetGame().SurfaceIsSea(np[0], np[2]) || GetGame().SurfaceIsPond(np[0], np[2]))
			return false;
		vector from = cur + "0 1.2 0";
		vector to = np + "0 1.2 0";
		to = to + dir * 0.4;
		return !VB_AB_Server.RayBlocked(from, to, p, null);		// стена, дом, постройка, дерево, камень, рельеф
	}

	// Игрок стоит на рельефе (не на полу дома, крыше, мосту, не в бункере)
	static bool OnGround(PlayerBase p)
	{
		vector pp = p.GetPosition();
		return Math.AbsFloat(pp[1] - GetGame().SurfaceY(pp[0], pp[2])) <= 1.0;
	}

	static vector FlatDir(vector from, vector to)
	{
		vector d = Vector(to[0] - from[0], 0, to[2] - from[2]);
		d.Normalize();
		return d;
	}

	static float FlatDist(vector a, vector b)
	{
		return vector.Distance(Vector(a[0], 0, a[2]), Vector(b[0], 0, b[2]));
	}
};

// ==================================================================== активные
// Аномалия под ногами: на игрока, который стоит на месте в поле 3 с, через 1,5 с встаёт аномалия поля
class VB_AB_AbUnderFoot extends VB_AB_Ability
{
	static const float STILL_SEC = 3.0;
	static const float STILL_DIST = 1.0;
	ref map<PlayerBase, vector> m_Pos = new map<PlayerBase, vector>;
	ref map<PlayerBase, float> m_Since = new map<PlayerBase, float>;
	string m_Cls;

	override float WarnSec()
	{
		return 1.5;
	}

	override void Tick(VB_AB_BossBase boss, float now)
	{
		VB_AB_Field f = boss.VB_GetField();
		if (!f)
			return;
		array<PlayerBase> ps = new array<PlayerBase>;
		VB_AB_Server.PlayersNear(f.GetCenter(), f.GetRadius(), ps);
		foreach (PlayerBase p : ps)
		{
			vector was;
			if (!m_Pos.Find(p, was) || vector.Distance(was, p.GetPosition()) > STILL_DIST)
			{
				m_Pos.Set(p, p.GetPosition());
				m_Since.Set(p, now);
			}
		}
	}

	override PlayerBase PickTarget(VB_AB_BossBase boss)
	{
		VB_AB_Field f = boss.VB_GetField();
		float now = VB_AB_Server.Now();
		array<PlayerBase> ps = new array<PlayerBase>;
		VB_AB_Server.PlayersNear(f.GetCenter(), f.GetRadius(), ps);
		array<PlayerBase> still = new array<PlayerBase>;
		foreach (PlayerBase p : ps)
		{
			float since;
			if (m_Since.Find(p, since) && now - since >= STILL_SEC)
				still.Insert(p);
		}
		if (still.Count() == 0)
			return null;
		m_Cls = PickClass(f);
		if (m_Cls == "")
			return null;
		return still.GetRandomElement();
	}

	// Класс: из конфига (Anomaly) или любая небольшая аномалия поля
	protected string PickClass(VB_AB_Field f)
	{
		if (m_Cfg.Anomaly != "")
			return m_Cfg.Anomaly;
		array<string> small = new array<string>;
		array<VB_AB_AnomalyBase> list = f.GetAnomalies();
		if (!list)
			return "";
		for (int i = 0; i < list.Count(); i++)
		{
			VB_AB_AnomalyBase a = list[i];
			if (!a || a.IsSetForDeletion())
				continue;
			string cls = a.GetType();
			if (a.VB_GetRadius() <= 3.0 && small.Find(cls) == -1)
				small.Insert(cls);
		}
		if (small.Count() == 0)
			return "";
		return small.GetRandomElement();
	}

	override void Warn(VB_AB_BossBase boss, PlayerBase target, vector at)
	{
		string ptc = GetGame().ConfigGetTextOut("CfgVehicles " + m_Cls + " vbABPtcIdle");
		string snd = GetGame().ConfigGetTextOut("CfgVehicles " + m_Cls + " vbABSndIdle");
		Fx(boss, ptc, snd, at);
	}

	override void Fire(VB_AB_BossBase boss, PlayerBase target, vector at)
	{
		VB_AB_Field f = boss.VB_GetField();
		if (f && m_Cls != "")
			f.SpawnExtra(m_Cls, at, 40);
		// следующая — опять после 3 с стояния; позиции тоже сбрасываются, иначе стоящий на месте игрок
		// больше никогда не получал аномалию под ноги (Tick не ставил ему время заново)
		m_Since.Clear();
		m_Pos.Clear();
	}
};

// Разряд: молния в ближайшего игрока (Электра, Гроза)
class VB_AB_AbDischarge extends VB_AB_Ability
{
	override void Warn(VB_AB_BossBase boss, PlayerBase target, vector at)
	{
		Fx(boss, "vb_ab_electra_idle", "vb_ab_electra_idle", boss.GetPosition());
	}

	override void Fire(VB_AB_BossBase boss, PlayerBase target, vector at)
	{
		if (!target || !target.IsAlive() || target.IsInVehicle())
			return;
		vector bp = boss.GetPosition();
		vector tp = target.GetPosition();
		if (FlatDist(bp, tp) > Range() + 5 || !VB_AB_Server.Sees(boss, target))
			return;
		Fx(boss, "vb_ab_electra_active", "vb_ab_electra_active", tp);
		Hit(boss, target, m_Cfg.Damage);
	}
};

// Импульс: гасит фонари, ПНВ, рации рядом на DurationSec (ЭМИ)
class VB_AB_AbPulse extends VB_AB_Ability
{
	override float WarnSec()
	{
		return 1.2;
	}

	override void Warn(VB_AB_BossBase boss, PlayerBase target, vector at)
	{
		Fx(boss, "vb_ab_emp_idle", "vb_ab_emp_idle", boss.GetPosition());
	}

	override void Fire(VB_AB_BossBase boss, PlayerBase target, vector at)
	{
		Fx(boss, "vb_ab_emp_active", "vb_ab_emp_active", boss.GetPosition());
		array<PlayerBase> ps = new array<PlayerBase>;
		VB_AB_Server.PlayersNear(boss.GetPosition(), Range(), ps);
		float until = VB_AB_Server.Now() + Duration(20);
		foreach (PlayerBase p : ps)
			KeepOff(p, until);
	}

	void ~VB_AB_AbPulse()
	{
		if (GetGame())
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(KeepOff);
	}

	void KeepOff(PlayerBase p, float until)
	{
		if (!p || !p.IsAlive() || VB_AB_Server.Now() > until)
			return;
		VB_AB_PlayerFx.SwitchOffDevices(p);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(KeepOff, 1000, false, p, until);
	}
};

// Жар: волна жара вокруг (Жарка, Огненный шторм): урон, быстрый перегрев, горячий воздух DurationSec
class VB_AB_AbHeat extends VB_AB_Ability
{
	override void Warn(VB_AB_BossBase boss, PlayerBase target, vector at)
	{
		Fx(boss, "vb_ab_zharka_idle", "vb_ab_zharka_idle", boss.GetPosition());
	}

	override void Fire(VB_AB_BossBase boss, PlayerBase target, vector at)
	{
		Fx(boss, "vb_ab_fireshtorm_active", "vb_ab_fireshtorm_active", boss.GetPosition());
		boss.VB_GetTemp().Pulse(boss, 60, Range(), Duration(8));
		array<PlayerBase> ps = new array<PlayerBase>;
		VB_AB_Server.PlayersNear(boss.GetPosition(), Range(), ps);
		foreach (PlayerBase p : ps)
		{
			if (!VB_AB_Server.Sees(boss, p))
				continue;
			Hit(boss, p, m_Cfg.Damage);
			VB_AB_PlayerFx.Overheat(p, 0.2);
		}
	}
};

// Ледяное дыхание: конус перед боссом — холод, стамина в ноль, замедление (Стекло, Застой)
class VB_AB_AbIceBreath extends VB_AB_Ability
{
	static const float CONE_COS = 0.8;		// ±37°

	override void Warn(VB_AB_BossBase boss, PlayerBase target, vector at)
	{
		Fx(boss, "vb_ab_zastoy_idle", "vb_ab_zastoy_idle", boss.GetPosition());
	}

	override void Fire(VB_AB_BossBase boss, PlayerBase target, vector at)
	{
		vector fwd = boss.GetDirection();
		fwd[1] = 0;
		fwd.Normalize();
		Fx(boss, "vb_ab_steklo_active", "vb_ab_steklo_active", Ground(boss.GetPosition() + fwd * (Range() * 0.5)));
		boss.VB_GetTemp().Pulse(boss, -25, Range(), Duration(6));
		array<PlayerBase> ps = new array<PlayerBase>;
		VB_AB_Server.PlayersNear(boss.GetPosition(), Range(), ps);
		foreach (PlayerBase p : ps)
		{
			vector toP = FlatDir(boss.GetPosition(), p.GetPosition());
			if (vector.Dot(toP, fwd) < CONE_COS || !VB_AB_Server.Sees(boss, p))
				continue;
			Hit(boss, p, m_Cfg.Damage);
			VB_AB_PlayerFx.ZeroStamina(p);
			VB_AB_PlayerFx.Slow(p, Duration(6));
			VB_AB_Fx.SendPlayer(boss, p, VB_AB_Fx.P_COLD, Duration(6));
		}
	}
};

// Метель: снег вокруг босса, видимость падает (Снег)
class VB_AB_AbBlizzard extends VB_AB_Ability
{
	override float WarnSec()
	{
		return 1.5;
	}

	override void Warn(VB_AB_BossBase boss, PlayerBase target, vector at)
	{
		Fx(boss, "vb_ab_snow_idle", "vb_ab_snow_idle", boss.GetPosition());
	}

	override void Fire(VB_AB_BossBase boss, PlayerBase target, vector at)
	{
		Fx(boss, "vb_ab_snow_active", "vb_ab_snow_active", boss.GetPosition());
		boss.VB_GetTemp().Pulse(boss, -20, Range(), Duration(25));
		array<PlayerBase> ps = new array<PlayerBase>;
		VB_AB_Server.PlayersNear(boss.GetPosition(), Range(), ps);
		foreach (PlayerBase p : ps)
			VB_AB_Fx.SendPlayer(boss, p, VB_AB_Fx.P_BLIZZARD, Duration(25));
	}
};

// Притяжение: тянет игрока к ближайшей Воронке, а если её нет — к боссу
class VB_AB_AbPull extends VB_AB_Ability
{
	override PlayerBase PickTarget(VB_AB_BossBase boss)
	{
		array<PlayerBase> ps = new array<PlayerBase>;
		VB_AB_Server.PlayersNear(boss.GetPosition(), Range(), ps);
		foreach (PlayerBase p : ps)
		{
			if (FlatDist(boss.GetPosition(), p.GetPosition()) > 4 && OnGround(p) && VB_AB_Server.Sees(boss, p))
				return p;
		}
		return null;
	}

	override void Warn(VB_AB_BossBase boss, PlayerBase target, vector at)
	{
		Fx(boss, "vb_ab_voronka_idle", "vb_ab_voronka_idle", target.GetPosition());
	}

	override void Fire(VB_AB_BossBase boss, PlayerBase target, vector at)
	{
		if (!target || !target.IsAlive())
			return;
		vector dest = boss.GetPosition();
		array<string> vor = {"VB_AB_Anomaly_Voronka"};
		VB_AB_AnomalyBase a = boss.VB_GetField().NearestOf(target.GetPosition(), vor);
		if (a && FlatDist(a.GetPosition(), target.GetPosition()) < 25)
			dest = a.GetPosition();
		float d = FlatDist(dest, target.GetPosition());
		float len = Math.Min(d - 1.5, 8.0);
		if (len <= 0.5)
			return;
		Fx(boss, "vb_ab_voronka_active", "vb_ab_voronka_active", target.GetPosition());
		MoveSteps(target, FlatDir(target.GetPosition(), dest), len / 5, 5);
	}
};

// Отброс: волна от босса отталкивает игроков (Ломка, Атом)
class VB_AB_AbKnockback extends VB_AB_Ability
{
	override float WarnSec()
	{
		return 0.8;
	}

	override void Warn(VB_AB_BossBase boss, PlayerBase target, vector at)
	{
		Fx(boss, "vb_ab_lomka_idle", "vb_ab_lomka_idle", boss.GetPosition());
	}

	override void Fire(VB_AB_BossBase boss, PlayerBase target, vector at)
	{
		Fx(boss, "vb_ab_atom_active", "vb_ab_atom_active", boss.GetPosition());
		array<PlayerBase> ps = new array<PlayerBase>;
		VB_AB_Server.PlayersNear(boss.GetPosition(), Range(), ps);
		foreach (PlayerBase p : ps)
		{
			if (!VB_AB_Server.Sees(boss, p))
				continue;
			Hit(boss, p, m_Cfg.Damage);
			Stagger(boss, p);
			MoveSteps(p, FlatDir(boss.GetPosition(), p.GetPosition()), 0.8, 4);
		}
	}
};

// Удар оземь (Энт): волна валит с ног рядом
class VB_AB_AbGroundSlam extends VB_AB_Ability
{
	override void Warn(VB_AB_BossBase boss, PlayerBase target, vector at)
	{
		VB_AB_Voice.ServerSay(boss, VB_AB_Voice.EV_SCREAM);		// крик перед ударом
		Fx(boss, "vb_ab_tramplin_idle", "vb_ab_tramplin_idle", boss.GetPosition());
	}

	override void Fire(VB_AB_BossBase boss, PlayerBase target, vector at)
	{
		Fx(boss, "vb_ab_tramplin_active", "vb_ab_tramplin_active", boss.GetPosition());
		array<PlayerBase> ps = new array<PlayerBase>;
		VB_AB_Server.PlayersNear(boss.GetPosition(), Range(), ps);
		foreach (PlayerBase p : ps)
		{
			if (!VB_AB_Server.Sees(boss, p))
				continue;
			Hit(boss, p, m_Cfg.Damage);
			VB_AB_PlayerFx.KnockDown(boss, p);
			VB_AB_Fx.SendPlayer(boss, p, VB_AB_Fx.P_SHAKE, 1.5);
		}
	}
};

// Кислотный плевок: дальняя атака — портит одежду, отравляет (Студень, Холера)
class VB_AB_AbAcidSpit extends VB_AB_Ability
{
	override PlayerBase PickTarget(VB_AB_BossBase boss)
	{
		array<PlayerBase> ps = new array<PlayerBase>;
		VB_AB_Server.PlayersNear(boss.GetPosition(), Range(), ps);
		foreach (PlayerBase p : ps)
		{
			if (FlatDist(boss.GetPosition(), p.GetPosition()) > 3 && VB_AB_Server.Sees(boss, p))
				return p;		// вплотную плеваться незачем
		}
		return null;
	}

	override float WarnSec()
	{
		return 0.8;
	}

	override void Warn(VB_AB_BossBase boss, PlayerBase target, vector at)
	{
		Fx(boss, "vb_ab_studen_idle", "vb_ab_studen_idle", boss.GetPosition());
	}

	override void Fire(VB_AB_BossBase boss, PlayerBase target, vector at)
	{
		if (!target)
			return;
		// плевок долетает туда, где игрок стоял в момент предупреждения: увернулся — промах
		Fx(boss, "vb_ab_studen_active", "vb_ab_studen_active", at);
		if (!target.IsAlive() || FlatDist(target.GetPosition(), at) > 2.0)
			return;
		Hit(boss, target, m_Cfg.Damage);
		VB_AB_PlayerFx.DamageClothes(target, 0.15);
		VB_AB_PlayerFx.Poison(target);
	}
};

// Пси-удар: тряска, размытие, предмет выпадает из рук, шёпот (Злая линза)
class VB_AB_AbPsiStrike extends VB_AB_Ability
{
	override float WarnSec()
	{
		return 1.2;
	}

	override void Warn(VB_AB_BossBase boss, PlayerBase target, vector at)
	{
		Fx(boss, "vb_ab_linceagr_idle", "vb_ab_linceagr_active", boss.GetPosition());
	}

	override void Fire(VB_AB_BossBase boss, PlayerBase target, vector at)
	{
		if (!target || !target.IsAlive() || target.IsInVehicle())
			return;
		vector bp = boss.GetPosition();
		vector tp = target.GetPosition();
		if (FlatDist(bp, tp) > Range() + 5 || !VB_AB_Server.Sees(boss, target))
			return;
		Fx(boss, "vb_ab_linceagr_active", "", tp);
		VB_AB_Fx.SendPlayer(boss, target, VB_AB_Fx.P_PSI, Duration(6));
		if (Math.RandomFloat01() < 0.5)
			VB_AB_PlayerFx.DropHands(target);
	}
};

// Вспышка: ослепляет тех, кто смотрит на босса (Вспышка)
class VB_AB_AbFlash extends VB_AB_Ability
{
	override float WarnSec()
	{
		return 1.5;		// успеть отвернуться
	}

	override void Warn(VB_AB_BossBase boss, PlayerBase target, vector at)
	{
		Fx(boss, "vb_ab_flash_idle", "vb_ab_flash_idle", boss.GetPosition());
	}

	override void Fire(VB_AB_BossBase boss, PlayerBase target, vector at)
	{
		Fx(boss, "vb_ab_flash_active", "vb_ab_flash_active", boss.GetPosition());
		array<PlayerBase> ps = new array<PlayerBase>;
		VB_AB_Server.PlayersNear(boss.GetPosition(), Range(), ps);
		foreach (PlayerBase p : ps)
		{
			vector look = p.GetDirection();
			look[1] = 0;
			look.Normalize();
			vector toBoss = FlatDir(p.GetPosition(), boss.GetPosition());
			if (vector.Dot(look, toBoss) > 0.3 && VB_AB_Server.Sees(boss, p))
				VB_AB_Fx.SendPlayer(boss, p, VB_AB_Fx.P_BLIND, Duration(5));
		}
	}
};

// Рывок (Мутаволк): прыжок к игроку на 5–15 м и удар
class VB_AB_AbDash extends VB_AB_Ability
{
	override PlayerBase PickTarget(VB_AB_BossBase boss)
	{
		array<PlayerBase> ps = new array<PlayerBase>;
		VB_AB_Server.PlayersNear(boss.GetPosition(), Range(), ps);
		foreach (PlayerBase p : ps)
		{
			if (FlatDist(boss.GetPosition(), p.GetPosition()) >= 5 && OnGround(p) && VB_AB_Server.Sees(boss, p))
				return p;
		}
		return null;
	}

	override float WarnSec()
	{
		return 0.6;
	}

	override void Warn(VB_AB_BossBase boss, PlayerBase target, vector at)
	{
		VB_AB_Voice.ServerSay(boss, VB_AB_Voice.EV_SCREAM);		// крик перед ударом
		Fx(boss, "vb_ab_tramplin_idle", "vb_ab_tramplin_idle", boss.GetPosition());
	}

	override void Fire(VB_AB_BossBase boss, PlayerBase target, vector at)
	{
		if (!target || !target.IsAlive() || target.IsInVehicle() || !OnGround(target))
			return;
		vector dir = FlatDir(boss.GetPosition(), target.GetPosition());
		vector dest = Ground(target.GetPosition() - dir * 1.8);
		Fx(boss, "vb_ab_tramplin_active", "vb_ab_tramplin_active", boss.GetPosition());
		boss.VB_TeleportTo(dest);
		boss.SetDirection(dir);
		Hit(boss, target, m_Cfg.Damage);
		Stagger(boss, target);
	}
};

// Сдвиг (Теневик): уходит в одну свою аномалию и выходит из другой
class VB_AB_AbShift extends VB_AB_Ability
{
	override float WarnSec()
	{
		return 0.8;
	}

	override bool NeedsTarget()
	{
		return false;
	}

	override bool CanUse(VB_AB_BossBase boss)
	{
		if (!super.CanUse(boss))
			return false;
		VB_AB_Field f = boss.VB_GetField();
		return f.GetAnomalies().Count() >= 2 && VB_AB_Server.AnyPlayerNear(f.GetCenter(), f.GetRadius() + 10);
	}

	override void Warn(VB_AB_BossBase boss, PlayerBase target, vector at)
	{
		Fx(boss, "vb_ab_lince_active", "vb_ab_lince_active", boss.GetPosition());
	}

	override void Fire(VB_AB_BossBase boss, PlayerBase target, vector at)
	{
		VB_AB_Field f = boss.VB_GetField();
		if (!f)
			return;
		array<VB_AB_AnomalyBase> far = new array<VB_AB_AnomalyBase>;
		array<VB_AB_AnomalyBase> list = f.GetAnomalies();
		for (int i = 0; i < list.Count(); i++)
		{
			VB_AB_AnomalyBase a = list[i];
			if (a && !a.IsSetForDeletion() && FlatDist(a.GetPosition(), boss.GetPosition()) >= 8)
				far.Insert(a);
		}
		if (far.Count() == 0)
			return;
		VB_AB_AnomalyBase exitA = far.GetRandomElement();
		vector dest = VB_AB_Finds.RandomGroundPos(exitA.GetPosition(), exitA.VB_GetRadius() + 1.0, exitA.VB_GetRadius() + 2.5);
		boss.VB_TeleportTo(dest);
		Fx(boss, "vb_ab_lince_active", "vb_ab_lince_active", dest);
	}
};

// Крик (Шум): шок и звон в ушах вокруг
class VB_AB_AbScream extends VB_AB_Ability
{
	override void Warn(VB_AB_BossBase boss, PlayerBase target, vector at)
	{
		VB_AB_Voice.ServerSay(boss, VB_AB_Voice.EV_SCREAM);		// крик перед ударом
		Fx(boss, "vb_ab_linceagr_idle", "vb_ab_par_idle", boss.GetPosition());
	}

	override void Fire(VB_AB_BossBase boss, PlayerBase target, vector at)
	{
		Fx(boss, "", "vb_ab_flash_active", boss.GetPosition());
		array<PlayerBase> ps = new array<PlayerBase>;
		VB_AB_Server.PlayersNear(boss.GetPosition(), Range(), ps);
		foreach (PlayerBase p : ps)
		{
			if (!VB_AB_Server.Sees(boss, p))
				continue;
			p.AddHealth("", "Shock", -30);
			VB_AB_Fx.SendPlayer(boss, p, VB_AB_Fx.P_SCREAM, Duration(4));
		}
	}
};

// Зов (Мозготряс): поднимает 3–5 обычных зомби вокруг цели
class VB_AB_AbCall extends VB_AB_Ability
{
	static const int MAX_ALIVE = 8;
	static const string NOISE = "VB_AB_CallNoise";
	static ref array<string> s_Zombies = {"ZmbM_HermitSkinny_Beige", "ZmbM_FarmerFat_Green", "ZmbF_JoggerSkinny_Red",
		"ZmbM_CitizenASkinny_Brown", "ZmbF_CitizenANormal_Blue", "ZmbM_HikerSkinny_Green"};
	ref array<EntityAI> m_Called = new array<EntityAI>;

	override float WarnSec()
	{
		return 0.8;
	}

	override bool CanUse(VB_AB_BossBase boss)
	{
		if (!super.CanUse(boss))
			return false;
		int alive = 0;
		foreach (EntityAI z : m_Called)
		{
			if (z && z.IsAlive())
				alive++;
		}
		return alive < MAX_ALIVE;
	}

	override void Warn(VB_AB_BossBase boss, PlayerBase target, vector at)
	{
		VB_AB_Voice.ServerSay(boss, VB_AB_Voice.EV_SCREAM);		// крик перед ударом
		Fx(boss, "vb_ab_linceagr_idle", "vb_ab_linceagr_idle", boss.GetPosition());
	}

	override void Fire(VB_AB_BossBase boss, PlayerBase target, vector at)
	{
		if (!target)
			return;
		int alive = 0;
		foreach (EntityAI c : m_Called)
		{
			if (c && c.IsAlive())
				alive++;
		}
		int n = Math.Min(Math.RandomIntInclusive(3, 5), MAX_ALIVE - alive);
		for (int i = 0; i < n; i++)
		{
			vector p = VB_AB_Finds.RandomGroundPos(target.GetPosition(), 10, 18);
			EntityAI z = EntityAI.Cast(GetGame().CreateObjectEx(s_Zombies.GetRandomElement(), p, ECE_PLACE_ON_SURFACE | ECE_INITAI | ECE_CREATEPHYSICS | ECE_NOPERSISTENCY_WORLD));
			if (z)
				m_Called.Insert(z);
		}
		VB_AB_PlayerFx.NoiseAt(target.GetPosition(), NOISE, 15);
	}

	// после смерти босса поднятые зомби доживают 5 минут и уходят (раньше оставались до рестарта)
	override void OnBossDead(VB_AB_BossBase boss)
	{
		VB_AB_Server.DespawnLater(m_Called, 300);
	}

	// сброс босса (все ушли): поднятые им зомби уходят вместе с полем
	override void OnClear(VB_AB_BossBase boss)
	{
		foreach (EntityAI z : m_Called)
		{
			if (z && z.IsAlive())
				GetGame().ObjectDelete(z);
		}
		m_Called.Clear();
	}
};

// ==================================================================== пассивные
// Тлен: аура — медленная потеря крови и тошнота (Мёртвая зона)
class VB_AB_AbDecay extends VB_AB_Ability
{
	override bool IsPassive()
	{
		return true;
	}

	override float Cooldown()
	{
		if (m_Cfg.Cooldown > 0)
			return m_Cfg.Cooldown;
		return 2;
	}

	override void Tick(VB_AB_BossBase boss, float now)
	{
		if (now < m_ReadyAt || !CanUse(boss))
			return;
		m_ReadyAt = now + Cooldown();
		array<PlayerBase> ps = new array<PlayerBase>;
		VB_AB_Server.PlayersNear(boss.GetPosition(), Range(), ps);
		foreach (PlayerBase p : ps)
		{
			// Damage — крови за один тик (по умолчанию 2 -> x10 = 20 мл в 2 с)
			p.AddHealth("", "Blood", -Math.Max(1, m_Cfg.Damage) * 10);
			if (Math.RandomFloat01() < 0.05)
				VB_AB_PlayerFx.Vomit(p);
		}
	}
};

// Ярость (Мозготряс красный): ниже 30 % здоровья быстрее и бьёт сильнее
class VB_AB_AbRage extends VB_AB_Ability
{
	static const float AT = 0.3;

	override bool IsPassive()
	{
		return true;
	}

	override void Tick(VB_AB_BossBase boss, float now)
	{
		if (boss.VB_IsEnraged() || boss.VB_GetHealth01() > AT)
			return;
		boss.VB_SetEnraged(true);
		Fx(boss, "vb_ab_fireiskra_active", "vb_ab_fireiskra_active", boss.GetPosition());
		VB_AB_Log.Info("босс " + boss.VB_GetId() + ": ярость");
	}
};

// Стая (Ричар): с ним 2–3 обычных Ричара того же вида, слабее и меньше
class VB_AB_AbPack extends VB_AB_Ability
{
	static const float MINION_HP = 400;
	static const float MINION_SCALE = 0.8;
	ref array<EntityAI> m_Pack = new array<EntityAI>;

	override bool IsPassive()
	{
		return true;
	}

	override void OnOpen(VB_AB_BossBase boss)
	{
		int alive = 0;
		foreach (EntityAI m : m_Pack)
		{
			if (m && m.IsAlive())
				alive++;
		}
		int n = Math.RandomIntInclusive(2, 3) - alive;
		for (int i = 0; i < n; i++)
		{
			vector p = VB_AB_Finds.RandomGroundPos(boss.GetPosition(), 2, 5);
			VB_AB_BossBase mob = VB_AB_BossBase.Cast(GetGame().CreateObjectEx(boss.GetType(), p, ECE_PLACE_ON_SURFACE | ECE_INITAI | ECE_CREATEPHYSICS | ECE_NOPERSISTENCY_WORLD));
			if (mob)
			{
				mob.VB_SetupMinion(MINION_HP, MINION_SCALE);
				m_Pack.Insert(mob);
			}
		}
	}

	// после смерти вожака спутники доживают 5 минут и уходят (раньше бродили до рестарта)
	override void OnBossDead(VB_AB_BossBase boss)
	{
		VB_AB_Server.DespawnLater(m_Pack, 300);
	}

	// живые спутники уходят вместе с набором (иначе при каждом раскрытии их становилось больше)
	override void OnClear(VB_AB_BossBase boss)
	{
		foreach (EntityAI m : m_Pack)
		{
			if (m && m.IsAlive())
				GetGame().ObjectDelete(m);
		}
		m_Pack.Clear();
	}
};

// ==================================================================== фабрика
class VB_AB_Abilities
{
	static VB_AB_Ability Create(string kind)
	{
		switch (kind)
		{
			case "AnomalyUnderFoot":	return new VB_AB_AbUnderFoot();
			case "Discharge":			return new VB_AB_AbDischarge();
			case "Pulse":				return new VB_AB_AbPulse();
			case "Heat":				return new VB_AB_AbHeat();
			case "IceBreath":			return new VB_AB_AbIceBreath();
			case "Blizzard":			return new VB_AB_AbBlizzard();
			case "Pull":				return new VB_AB_AbPull();
			case "Knockback":			return new VB_AB_AbKnockback();
			case "GroundSlam":			return new VB_AB_AbGroundSlam();
			case "AcidSpit":			return new VB_AB_AbAcidSpit();
			case "PsiStrike":			return new VB_AB_AbPsiStrike();
			case "Flash":				return new VB_AB_AbFlash();
			case "Dash":				return new VB_AB_AbDash();
			case "Shift":				return new VB_AB_AbShift();
			case "Scream":				return new VB_AB_AbScream();
			case "Call":				return new VB_AB_AbCall();
			case "Decay":				return new VB_AB_AbDecay();
			case "Rage":				return new VB_AB_AbRage();
			case "Pack":				return new VB_AB_AbPack();
		}
		return null;
	}
};
