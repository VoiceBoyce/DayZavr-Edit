// Хранитель рощи — мини-босс (ТЗ_сервер_18.md, ветка 2в п.2) — только сервер. Выходит в третьей волне у древней рощи
// (VB_RootGuards.VB_WaveTypes). Здоровье ×1,5 к Энту MSFZ — в config.cpp клиента. Умения — пока рядом (FIGHT_DIST)
// игрок, которого стражи не щадят (VB_AR_FavorServer.VB_Peaceful: «Свой», «Друг», благословлённый, с оберегом — не в
// счёт), и пока Хранитель не спит от пыльцевой ловушки (крупных она и не усыпляет — VB_AR_CreatureServer.c):
//   «Хватка корней»: раз в GRIP_MIN_MS..GRIP_MAX_MS одному такому игроку в GRIP_DIST — выносливость в ноль, бег закрыт
//     LIMP_MS, первые секунды стоит, потом шагом (клиент, VB_AR_Keeper.c); у ног всем видна обвивка корней.
//   «Зов стаи»: на CALL1 и CALL2 здоровья — по два Лесовых волка рядом (VB_AR_Horde, без лута: призванные), вой с его стороны.
//   «Кора»: на BARK_AT здоровья BARK_MS почти неуязвим — проходит BARK_TAKE урона, свечение ярче (RPC_BARK). Урон гасим
//     ДО применения (Object.EEOnDamageCalculated → false) и снимаем BARK_TAKE сами, не ниже 1 здоровья: под корой даже
//     смертельный выстрел не убивает. Если движок (или класс-наследник) это событие не позовёт — запасной путь: вернуть
//     (1 − BARK_TAKE) урона после удара в EEHitBy, как было раньше.
// Тело убитого держится BODY_KEEP_MS для разделки: CE даёт мёртвому инфицированному ~330 с и назначает срок не сразу,
// поэтому SetLifetime повторяем раз в BODY_TICK_MS остатком времени (E59).
// Трофей «Сердце Хранителя» — при разделке, всегда (VB_RootGuards.VB_SkinServer).

class VB_AR_Keeper
{
	static const int TICK_MS = 1000;
	static const float FIGHT_DIST = 40.0;		// умения — только когда игрок ближе
	static const float GRIP_DIST = 15.0;
	static const int GRIP_MIN_MS = 15000;
	static const int GRIP_MAX_MS = 20000;
	static const int GRIP_FIRST_MS = 8000;		// первая хватка — не сразу после встречи
	static const float CALL1 = 0.75;
	static const float CALL2 = 0.40;
	static const float BARK_AT = 0.50;
	static const int BARK_MS = 10000;
	static const float BARK_TAKE = 0.2;
	static const float WARN_DIST = 60.0;		// кому писать о зове стаи и коре
	static const float HOWL_DIST = 200.0;
	static const int BODY_KEEP_MS = 900000;		// тело — 15 мин на разделку
	static const int BODY_TICK_MS = 20000;		// CE перезаписывает срок жизни — повторять (E59)

	protected static ref array<ref VB_AR_Keeper> s_VB_All;
	protected static bool s_VB_Ticking;

	protected ZombieBase m_VB_Body;
	protected bool m_VB_Called1;
	protected bool m_VB_Called2;
	protected bool m_VB_Barked;
	protected int m_VB_BarkUntil;
	protected int m_VB_BarkHits;		// удары, погашенные до применения; EEHitBy по ним не возвращает урон
	protected int m_VB_NextGripMs;		// 0 — игрок ещё не подходил

	void VB_AR_Keeper(ZombieBase body)
	{
		m_VB_Body = body;
	}

	static void VB_Register(ZombieBase body)
	{
		if (!body)
			return;
		if (!s_VB_All)
			s_VB_All = new array<ref VB_AR_Keeper>;
		if (VB_Of(body))
			return;
		s_VB_All.Insert(new VB_AR_Keeper(body));
		string at = body.GetPosition().ToString();
		Print("[VB_AncientRoot] Хранитель рощи вышел в " + at);
		if (!s_VB_Ticking)
		{
			s_VB_Ticking = true;
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_Tick, TICK_MS, true);
		}
	}

	static VB_AR_Keeper VB_Of(ZombieBase body)
	{
		if (!s_VB_All)
			return null;
		foreach (VB_AR_Keeper k : s_VB_All)
		{
			if (k && k.m_VB_Body == body)
				return k;
		}
		return null;
	}

	static void VB_Tick()
	{
		if (!s_VB_All || s_VB_All.Count() == 0)
		{
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_Tick);
			s_VB_Ticking = false;
			return;
		}
		array<Man> players = new array<Man>;
		g_Game.GetPlayers(players);
		int now = g_Game.GetTime();
		for (int i = s_VB_All.Count() - 1; i >= 0; i--)
		{
			VB_AR_Keeper k = s_VB_All[i];
			if (!k || !k.m_VB_Body || !k.m_VB_Body.IsAlive())
			{
				s_VB_All.Remove(i);
				continue;
			}
			k.VB_Update(now, players);
		}
	}

	// «Кора», основной путь — до применения урона (EEOnDamageCalculated). false — урон не применять: BARK_TAKE сняли
	// сами (общему здоровью и зоне), не ниже 1. События удара (злость стражей, терновая рана) зовём сами: зовёт ли
	// движок EEHitBy для отменённого урона, не проверено, а повтор у них безвреден.
	static bool VB_OnDamage(ZombieBase body, TotalDamageResult res, int damageType, EntityAI source, string zone)
	{
		VB_AR_Keeper k = VB_Of(body);
		if (!k || !res || !body.IsAlive() || g_Game.GetTime() >= k.m_VB_BarkUntil)
			return true;
		k.m_VB_BarkHits++;
		float dmg = res.GetDamage("", "Health");
		VB_TakeFloor(body, "", dmg * BARK_TAKE);
		if (zone != "")
		{
			float zdmg = res.GetDamage(zone, "Health");
			VB_TakeFloor(body, zone, zdmg * BARK_TAKE);
		}
		VB_AR_GuardHit.VB_OnHit(body, source);
		VB_AR_CreatureState.VB_OnHit(body, damageType, source);
		return false;
	}

	// снять take здоровья, но оставить не меньше 1
	protected static void VB_TakeFloor(ZombieBase body, string zone, float take)
	{
		if (take <= 0)
			return;
		float room = body.GetHealth(zone, "Health") - 1.0;
		if (take > room)
			take = room;
		if (take > 0)
			body.AddHealth(zone, "Health", -take);
	}

	// «Кора», запасной путь — после удара: вернуть (1 − BARK_TAKE) урона и общему здоровью, и зоне.
	// Удар, уже погашенный в VB_OnDamage, пропускаем (урона не было — возвращать нечего).
	static void VB_OnHit(ZombieBase body, TotalDamageResult res, string zone)
	{
		VB_AR_Keeper k = VB_Of(body);
		if (!k || !res || !body.IsAlive() || g_Game.GetTime() >= k.m_VB_BarkUntil)
			return;
		if (k.m_VB_BarkHits > 0)
		{
			k.m_VB_BarkHits--;
			return;
		}
		float back = 1.0 - BARK_TAKE;
		float dmg = res.GetDamage("", "Health");
		if (dmg > 0)
			body.AddHealth("", "Health", dmg * back);
		if (zone != "")
		{
			float zdmg = res.GetDamage(zone, "Health");
			if (zdmg > 0)
				body.AddHealth(zone, "Health", zdmg * back);
		}
	}

	// игрок, которого стражи не щадят: жив и не «мирный» (VB_AR_FavorServer.VB_Peaceful)
	protected static bool VB_IsFoe(PlayerBase p)
	{
		return p && p.IsAlive() && !VB_AR_FavorServer.VB_Peaceful(p);
	}

	// рядом есть такой игрок (дешёвая дистанция — первой)
	protected static bool VB_FoeNear(vector pos, array<Man> players, float dist)
	{
		float dSq = dist * dist;
		PlayerBase p;
		foreach (Man m : players)
		{
			if (!m || vector.DistanceSq(m.GetPosition(), pos) >= dSq)
				continue;
			p = PlayerBase.Cast(m);
			if (VB_IsFoe(p))
				return true;
		}
		return false;
	}

	protected static void VB_Warn(vector pos, array<Man> players, string title, string text)
	{
		float dSq = WARN_DIST * WARN_DIST;
		PlayerBase p;
		foreach (Man m : players)
		{
			p = PlayerBase.Cast(m);
			if (p && p.IsAlive() && p.GetIdentity() && vector.DistanceSq(p.GetPosition(), pos) < dSq)
				NotificationSystem.SendNotificationToPlayerExtended(p, 5, title, text);
		}
	}

	void VB_Update(int now, array<Man> players)
	{
		if (VB_AR_CreatureState.VB_AsleepServer(m_VB_Body))
			return;				// спит от пыльцевой ловушки — умения молчат
		vector kp = m_VB_Body.GetPosition();
		if (!VB_FoeNear(kp, players, FIGHT_DIST))
			return;
		if (m_VB_NextGripMs == 0)
			m_VB_NextGripMs = now + GRIP_FIRST_MS;

		float hp = m_VB_Body.GetHealth01("", "Health");
		if (!m_VB_Called1 && hp <= CALL1)
		{
			m_VB_Called1 = true;
			VB_CallPack(players);
		}
		if (!m_VB_Barked && hp <= BARK_AT)
		{
			m_VB_Barked = true;
			VB_Bark(now, players);
		}
		if (!m_VB_Called2 && hp <= CALL2)
		{
			m_VB_Called2 = true;
			VB_CallPack(players);
		}
		if (now >= m_VB_NextGripMs)
		{
			m_VB_NextGripMs = now + Math.RandomIntInclusive(GRIP_MIN_MS, GRIP_MAX_MS);
			VB_Grip(players);
		}
	}

	// «Зов стаи»: два Лесовых волка рядом и вой с его стороны. Волки призванные (loot = false): без добычи
	protected void VB_CallPack(array<Man> players)
	{
		vector kp = m_VB_Body.GetPosition();
		TStringArray types = new TStringArray;
		types.Insert(VB_RootGuards.WOLF);
		types.Insert(VB_RootGuards.WOLF);
		VB_AR_Horde.VB_SpawnWaveTypes(kp, types, 10, 20, false);
		float howlSq = HOWL_DIST * HOWL_DIST;
		PlayerBase p;
		foreach (Man m : players)
		{
			p = PlayerBase.Cast(m);
			if (p && p.GetIdentity() && vector.DistanceSq(p.GetPosition(), kp) < howlSq)
				g_Game.RPCSingleParam(p, VB_AR_Favor.RPC_HOWL, new Param1<vector>(kp), true, p.GetIdentity());
		}
		VB_Warn(kp, players, "Хранитель зовёт стаю", "Из чащи бегут Лесовые волки.");
		Print("[VB_AncientRoot] Хранитель рощи: зов стаи в " + kp.ToString());
	}

	// «Кора»: BARK_MS почти неуязвим, свечение ярче
	protected void VB_Bark(int now, array<Man> players)
	{
		m_VB_BarkUntil = now + BARK_MS;
		m_VB_BarkHits = 0;
		m_VB_Body.RPCSingleParam(VB_AR_KeeperFx.RPC_BARK, new Param1<int>(BARK_MS), true, null);
		vector kp = m_VB_Body.GetPosition();
		VB_Warn(kp, players, "Хранитель одевается корой", "Десять секунд удары почти не берут его — переждите.");
		Print("[VB_AncientRoot] Хранитель рощи: кора в " + kp.ToString());
	}

	// «Хватка корней»: случайный игрок в GRIP_DIST (не в машине, в сознании, не «мирный»)
	protected void VB_Grip(array<Man> players)
	{
		vector kp = m_VB_Body.GetPosition();
		array<PlayerBase> cands = new array<PlayerBase>;
		float gripSq = GRIP_DIST * GRIP_DIST;
		PlayerBase p;
		foreach (Man m : players)
		{
			p = PlayerBase.Cast(m);
			if (!p || !p.IsAlive() || p.IsUnconscious() || p.IsInVehicle() || !p.GetIdentity())
				continue;
			if (vector.DistanceSq(p.GetPosition(), kp) >= gripSq)
				continue;
			if (VB_AR_FavorServer.VB_Peaceful(p))
				continue;
			cands.Insert(p);
		}
		if (cands.Count() == 0)
			return;
		PlayerBase target = cands.GetRandomElement();
		vector tp = target.GetPosition();
		if (target.GetStaminaHandler())
			target.GetStaminaHandler().SetStamina(0);
		target.VB_AR_SetLimp(VB_AR_KeeperFx.LIMP_MS);
		float seeSq = 100.0 * 100.0;
		bool isTarget;
		foreach (Man o : players)
		{
			p = PlayerBase.Cast(o);
			if (!p || !p.GetIdentity() || vector.DistanceSq(p.GetPosition(), tp) > seeSq)
				continue;
			isTarget = p == target;
			g_Game.RPCSingleParam(p, VB_AR_KeeperFx.RPC_GRIP, new Param2<vector, bool>(tp, isTarget), true, p.GetIdentity());
		}
		NotificationSystem.SendNotificationToPlayerExtended(target, 4, "Корни хватают тебя за ноги", "Хранитель держит тебя: бежать не выйдет.");
		Print("[VB_AncientRoot] Хранитель рощи: хватка корней — " + VB_AR_ItemsServer.VB_Who(target));
	}
}

modded class ZombieBase
{
	protected int m_VB_AR_KeepUntil;	// сервер: тело Хранителя держим до этого g_Game.GetTime()

	override void EEInit()
	{
		super.EEInit();
		if (g_Game.IsServer() && IsKindOf(VB_RootGuards.ENT))
			VB_AR_Keeper.VB_Register(this);
	}

	override bool EEOnDamageCalculated(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		if (!super.EEOnDamageCalculated(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef))
			return false;
		if (g_Game.IsServer() && IsKindOf(VB_RootGuards.ENT))
			return VB_AR_Keeper.VB_OnDamage(this, damageResult, damageType, source, dmgZone);
		return true;
	}

	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
		if (g_Game.IsServer() && IsKindOf(VB_RootGuards.ENT))
			VB_AR_Keeper.VB_OnHit(this, damageResult, dmgZone);
	}

	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		if (!g_Game.IsServer() || !IsKindOf(VB_RootGuards.ENT))
			return;
		m_VB_AR_KeepUntil = g_Game.GetTime() + VB_AR_Keeper.BODY_KEEP_MS;
		VB_AR_KeepBody();
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_AR_KeepBody, VB_AR_Keeper.BODY_TICK_MS, true);
	}

	// тело Хранителя: срок жизни CE = остаток до m_VB_AR_KeepUntil (CE свой срок назначает позже — повторяем, E59)
	void VB_AR_KeepBody()
	{
		int left = m_VB_AR_KeepUntil - g_Game.GetTime();
		if (left <= 0)
		{
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_AR_KeepBody);
			return;
		}
		float sec = left * 0.001;
		SetLifetimeMax(sec);
		SetLifetime(sec);
	}

	override void EEDelete(EntityAI parent)
	{
		if (g_Game)
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_AR_KeepBody);
		super.EEDelete(parent);
	}
}
