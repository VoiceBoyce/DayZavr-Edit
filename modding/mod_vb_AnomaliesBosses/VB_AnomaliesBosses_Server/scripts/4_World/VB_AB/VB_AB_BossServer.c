// Серверная часть босса (ТЗ, разделы 3–4): здоровье из bosses.json, поле, поводок, способности, щит, фазы,
// смерть и трофеи. Клиентская часть (масштаб, аура, голос, RPC эффектов) — VB_AnomaliesBosses, VB_AB_BossBase.c.
//
// Здоровье: у класса в config.cpp огромный ванильный запас и нет смерти «в голову» (VB_AB_BOSS_DAMAGE),
// а настоящее здоровье — запас m_VB_HP из bosses.json (Health), его уменьшает каждое попадание.
// Поэтому другое здоровье босса не требует пересборки мода.

modded class VB_AB_BossBase
{
	static const int TICK_MS = 500;
	static const float SENSE_RADIUS = 10.0;		// м: игрок ближе — поле раскрывается и без агра
	static const float LEASH_GRACE_SEC = 3.0;	// с за поводком до рывка назад
	protected float m_VB_OutSec;
	protected float m_VB_InvulnUntil;			// неуязвим до (выброс на фазе)

	// способности (VB_AB_Abilities.c)
	protected ref array<ref VB_AB_Ability> m_VB_Abil = new array<ref VB_AB_Ability>;
	protected float m_VB_AbilBusyUntil;

	protected VB_AB_BossCfg m_VB_Cfg;
	protected VB_AB_PointRuntime m_VB_Point;
	protected ref VB_AB_Field m_VB_Field;
	protected float m_VB_HP;
	protected float m_VB_HPMax;
	protected int m_VB_PhaseIdx;
	protected float m_VB_NoPlayersSec;
	protected bool m_VB_Dead;
	protected Man m_VB_LastHitter;		// смерть наступает от SetHealth(0) без убийцы, поэтому помним стрелявшего

	override void EEInit()
	{
		super.EEInit();
		if (!GetGame().IsServer())
			return;
		m_VB_HPMax = GetGame().ConfigGetFloat("CfgVehicles " + GetType() + " vbABHealth");
		if (m_VB_HPMax <= 0)
			m_VB_HPMax = 3000;
		m_VB_HP = m_VB_HPMax;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_BossTick, TICK_MS, true);
	}

	override void EEDelete(EntityAI parent)
	{
		if (GetGame() && GetGame().IsServer())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_BossTick);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_RemoveCorpse);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_KeepCorpse);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_AbilityFire);
			if (m_VB_Field && !m_VB_Dead)
				m_VB_Field.CloseNow();
		}
		super.EEDelete(parent);
	}


	// ---------------------------------------------------------------- сервер
	void VB_Setup(VB_AB_BossCfg cfg, VB_AB_PointRuntime point)
	{
		m_VB_Cfg = cfg;
		m_VB_Point = point;
		m_VB_HPMax = Math.Max(1, cfg.Health);
		m_VB_HP = m_VB_HPMax;
		m_VB_PhaseIdx = 0;
		if (cfg.Scale > 0)
		{
			m_VB_Scale = cfg.Scale;
			VB_UpdateScaleMask();
		}
		if (cfg.Trophy && cfg.Trophy.CutTimeSec > 0)
			m_VB_CutTime = cfg.Trophy.CutTimeSec;
		SetSynchDirty();
	}

	// Во сколько раз удар босса сильнее зомби (PlayerBase.EEHitBy, VB_AB_PlayerFx.c). Спутники Стаи бьют как зомби.
	float VB_GetMeleeMul()
	{
		if (m_VB_Minion || !m_VB_Cfg)
			return 1;
		return m_VB_Cfg.MeleeMul;
	}

	// Спутник «Стаи»: свой запас здоровья, меньше ростом, поля и трофея нет
	void VB_SetupMinion(float hp, float scale)
	{
		m_VB_Minion = true;
		m_VB_HPMax = hp;
		m_VB_HP = hp;
		m_VB_Scale = scale;
		VB_UpdateScaleMask();
		SetSynchDirty();
	}

	void VB_SetEnraged(bool on)
	{
		m_VB_Enraged = on;
		SetSynchDirty();
	}

	// ---------------------------------------------------------------- способности (ТЗ 4.3)
	// Снять набор: отложенные удары отменяются, поставленное способностями (спутники «Стаи») убирается
	protected void VB_ClearAbilities()
	{
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_AbilityFire);
		foreach (VB_AB_Ability ab : m_VB_Abil)
			ab.OnClear(this);
		m_VB_Abil.Clear();
	}

	// «Перебросить поле» (редактор): свернуть и раскрыть заново с новым броском шансов
	void VB_RerollField()
	{
		if (m_VB_Dead || !IsAlive() || !m_VB_Cfg)
			return;
		VB_ClearAbilities();
		if (m_VB_Field)
			m_VB_Field.CloseNow();
		m_VB_Field = null;
		VB_OpenField();
	}

	// Набор на эту встречу: строки с шансом бросаются при каждом раскрытии поля
	protected void VB_RollAbilities()
	{
		VB_ClearAbilities();
		m_VB_AbilBusyUntil = 0;
		if (!m_VB_Cfg || !m_VB_Cfg.Abilities)
			return;
		float now = VB_AB_Server.Now();
		string log = "";
		foreach (VB_AB_AbilityCfg ac : m_VB_Cfg.Abilities)
		{
			if (!ac)
				continue;
			if (ac.Chance > 0 && ac.Chance < 100 && Math.RandomFloat(0, 100) >= ac.Chance)
				continue;
			VB_AB_Ability ab = VB_AB_Abilities.Create(ac.Type);
			if (!ab)
			{
				VB_AB_Log.Warn("босс " + VB_GetId() + ": неизвестная способность " + ac.Type);
				continue;
			}
			ab.Setup(ac, now);
			m_VB_Abil.Insert(ab);
			ab.OnOpen(this);
			log += " " + ac.Type;
		}
		VB_AB_Log.Info("босс " + VB_GetId() + ": способности:" + log);
	}

	protected void VB_AbilityTick()
	{
		if (m_VB_Abil.Count() == 0 || !IsAlive())
			return;
		float now = VB_AB_Server.Now();
		foreach (VB_AB_Ability t : m_VB_Abil)
			t.Tick(this, now);
		if (now < m_VB_AbilBusyUntil)
			return;
		// активные — в случайном порядке, первая подходящая
		array<VB_AB_Ability> ready = new array<VB_AB_Ability>;
		foreach (VB_AB_Ability a : m_VB_Abil)
		{
			if (!a.IsPassive() && now >= a.m_ReadyAt)
				ready.Insert(a);
		}
		while (ready.Count() > 0)
		{
			int k = Math.RandomInt(0, ready.Count());
			VB_AB_Ability ab = ready[k];
			ready.Remove(k);
			if (!ab.CanUse(this))
				continue;
			PlayerBase target = ab.PickTarget(this);
			if (ab.NeedsTarget() && !target)
				continue;
			vector at = GetPosition();
			if (target)
				at = target.GetPosition();
			ab.Warn(this, target, at);
			ab.m_ReadyAt = now + ab.Cooldown();
			m_VB_AbilBusyUntil = now + ab.WarnSec() + VB_AB_Ability.GAP_SEC;
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_AbilityFire, ab.WarnSec() * 1000, false, ab, target, at);
			VB_AB_Log.Dbg("босс " + VB_GetId() + ": " + ab.Kind());
			return;
		}
	}

	void VB_AbilityFire(VB_AB_Ability ab, PlayerBase target, vector at)
	{
		if (m_VB_Dead || !IsAlive() || !ab || m_VB_Abil.Find(ab) == -1)
			return;
		ab.Fire(this, target, at);
	}

	// ---------------------------------------------------------------- трофей (ТЗ 2.7)
	float VB_GetToolDamage()
	{
		if (m_VB_Cfg && m_VB_Cfg.Trophy)
			return m_VB_Cfg.Trophy.ToolDamage;
		return 15;
	}

	// Сервер: разрезать тело. false — уже разрезано (двое резали одновременно, второй опоздал).
	bool VB_CarveServer(PlayerBase player)
	{
		if (m_VB_Carved || IsAlive())
			return false;
		m_VB_Carved = true;
		SetSynchDirty();

		string got = "";
		if (m_VB_Cfg && m_VB_Cfg.Trophy && m_VB_Cfg.Trophy.Items)
		{
			foreach (VB_AB_ChanceLine line : m_VB_Cfg.Trophy.Items)
			{
				if (!line || !line.RollChance())
					continue;
				string cls = line.PickClass();
				int n = line.PickCount();
				for (int i = 0; i < n; i++)
				{
					if (VB_GiveTrophy(player, cls))
						got += " " + cls;
				}
			}
		}
		else
		{
			// босс поставлен не из bosses.json: трофей его скина, если такой класс есть (VB_AB_Ent_Storm -> VB_AB_Trophy_Ent_Storm)
			string t = GetType();
			t.Replace("VB_AB_", "VB_AB_Trophy_");
			if (VB_GiveTrophy(player, t))
				got = " " + t;
		}
		string who = "?";
		if (player && player.GetIdentity())
			who = player.GetIdentity().GetName() + " (" + player.GetIdentity().GetPlainId() + ")";
		VB_AB_Log.Info("трофей: " + who + " разрезал " + VB_GetId() + ", получил:" + got);

		// тело исчезает, как после ванильной разделки (частицы палитры — вместе с аурами)
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_RemoveCorpse);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_RemoveCorpse, 1500, false);
		return true;
	}

	protected bool VB_GiveTrophy(PlayerBase player, string cls)
	{
		if (cls == "" || !VB_AB_Cfg.ClassExists(cls))
		{
			VB_AB_Log.Warn("трофей " + cls + " у босса " + VB_GetId() + ": класса нет на сервере");
			return false;
		}
		// всегда под ноги резавшему, не в инвентарь (пользователь, 2026-11-13): трофей видно, как он выпал
		vector p = GetPosition();
		if (player)
			p = player.GetPosition();
		EntityAI it = EntityAI.Cast(GetGame().CreateObjectEx(cls, VB_AB_Finds.RandomGroundPos(p, 0.2, 0.6), ECE_PLACE_ON_SURFACE));
		return it != null;
	}

	VB_AB_BossCfg VB_GetCfg()
	{
		return m_VB_Cfg;
	}

	VB_AB_Field VB_GetField()
	{
		return m_VB_Field;
	}

	float VB_GetHealth01()
	{
		if (m_VB_HPMax <= 0)
			return 1;
		return m_VB_HP / m_VB_HPMax;
	}

	string VB_GetId()
	{
		if (m_VB_Cfg)
			return m_VB_Cfg.Id;
		return GetType();
	}

	void VB_BossTick()
	{
		if (m_VB_Dead || !IsAlive() || !m_VB_Cfg)
			return;
		if (!m_VB_Field)
		{
			// агр зомби или чужой вплотную к логову: ИИ иногда долго «не замечает» игрока рядом (на стенде — минуты)
			if (GetMindStateSynced() >= DayZInfectedConstants.MINDSTATE_CHASE || VB_AB_Server.AnyPlayerNear(GetPosition(), SENSE_RADIUS))
				VB_OpenField();
			return;
		}
		vector pos = GetPosition();
		vector c = m_VB_Field.GetCenter();
		float d = vector.Distance(Vector(pos[0], 0, pos[2]), Vector(c[0], 0, c[2]));
		// поводок: возврат, если за ним дольше LEASH_GRACE_SEC или ушёл далеко — иначе у края поля босс дёргался
		// назад каждые пару секунд, догоняя игрока (на стенде: «отошёл на 31 м, поводок 30 м» раз за разом)
		if (d > m_VB_Cfg.Field.LeashRadius)
		{
			m_VB_OutSec += TICK_MS / 1000.0;
			if (m_VB_OutSec >= LEASH_GRACE_SEC || d > m_VB_Cfg.Field.LeashRadius + 15)
			{
				VB_ReturnToField(d);
				m_VB_OutSec = 0;
			}
		}
		else
			m_VB_OutSec = 0;

		if (VB_AB_Server.AnyPlayerNear(c, m_VB_Cfg.Field.LeashRadius + 20))
			m_VB_NoPlayersSec = 0;
		else
		{
			m_VB_NoPlayersSec += TICK_MS / 1000.0;
			if (m_VB_NoPlayersSec >= VB_AB_Server.Settings().ResetAfterSec)
			{
				VB_Reset();
				return;
			}
		}
		// щит: внутри своей аномалии понемногу лечится (ТЗ 8.1)
		float heal = m_VB_Cfg.Field.ShieldHealPct;
		if (heal > 0 && m_VB_HP < m_VB_HPMax && m_VB_Field.IsInsideOwnAnomaly(GetPosition()))
			m_VB_HP = Math.Min(m_VB_HPMax, m_VB_HP + m_VB_HPMax * heal / 100.0 * TICK_MS / 1000.0);
		VB_AbilityTick();
	}

	void VB_OpenField()
	{
		m_VB_Field = new VB_AB_Field(this, m_VB_Cfg, GetPosition());
		m_VB_NoPlayersSec = 0;
		m_VB_Field.Open();
		VB_RollAbilities();
		VB_CheckPhases();		// здоровье могли сбить ещё до раскрытия
	}

	// Увели дальше поводка — рывком возвращается в своё поле.
	void VB_ReturnToField(float dist)
	{
		vector c = VB_AB_Finds.RandomGroundPos(m_VB_Field.GetCenter(), 1, 3);
		VB_TeleportTo(c);
		VB_AB_Log.Info("босс " + VB_GetId() + " вернулся в поле: отошёл на " + Math.Round(dist) + " м, поводок " + m_VB_Cfg.Field.LeashRadius + " м");
	}

	// Перенос босса со сбросом скорости (как DeveloperTeleport): без этого после SetPosition физика
	// разгоняла Энта — на стенде за секунду «отошёл на 183 м», потом на 468 м
	void VB_TeleportTo(vector pos)
	{
		SetPosition(pos);
		SetVelocity(this, vector.Zero);
	}

	// Все ушли: поле сворачивается, босс лечится и стоит в логове.
	void VB_Reset()
	{
		if (m_VB_Field)
			m_VB_Field.CloseNow();
		m_VB_Field = null;
		m_VB_HP = m_VB_HPMax;
		m_VB_PhaseIdx = 0;
		m_VB_NoPlayersSec = 0;
		m_VB_InvulnUntil = 0;
		VB_ClearAbilities();
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_AbilityFire);
		if (m_VB_Enraged)
			VB_SetEnraged(false);
		if (m_VB_Point)
			VB_TeleportTo(m_VB_Point.GetPos());
		VB_AB_Log.Info("босс " + VB_GetId() + ": игроки ушли, поле свёрнуто, здоровье восстановлено");
	}

	// Уход без боя (никого рядом SleepAfterMin минут) или «Убрать босса» в редакторе.
	void VB_Despawn()
	{
		if (m_VB_Field)
			m_VB_Field.CloseNow();
		m_VB_Field = null;
		m_VB_Dead = true;
		VB_ClearAbilities();
		GetGame().ObjectDelete(this);
	}

	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
		if (!GetGame().IsServer() || m_VB_Dead || !damageResult)
			return;
		float dmg = damageResult.GetDamage("", "Health");
		if (dmg <= 0)
			return;
		// выброс на пороге фазы: несколько секунд неуязвим (ТЗ 8.2)
		if (VB_AB_Server.Now() < m_VB_InvulnUntil)
		{
			SetHealth("", "", GetMaxHealth("", ""));
			VB_AB_Log.Dbg("попадание по " + VB_GetId() + ": неуязвим (фаза)");
			return;
		}
		// щит: внутри своей аномалии урон меньше (ТЗ 8.1) — выманить из поля и есть тактика
		if (m_VB_Cfg && m_VB_Field && m_VB_Cfg.Field.ShieldDamage > 0 && m_VB_Cfg.Field.ShieldDamage < 1 && m_VB_Field.IsInsideOwnAnomaly(GetPosition()))
			dmg *= m_VB_Cfg.Field.ShieldDamage;
		m_VB_HP -= dmg;
		if (source && source.GetHierarchyRootPlayer())
			m_VB_LastHitter = source.GetHierarchyRootPlayer();
		VB_AB_Log.Dbg("попадание по " + VB_GetId() + ": " + dmgZone + " " + ammo + " −" + dmg + ", осталось " + m_VB_HP + "/" + m_VB_HPMax);
		if (m_VB_HP <= 0)
		{
			m_VB_HP = 0;
			SetHealth("", "", 0);
			return;
		}
		// ванильное здоровье держим полным: умирает босс только от своего запаса
		SetHealth("", "", GetMaxHealth("", ""));
		VB_AB_Voice.ServerSay(this, VB_AB_Voice.EV_PAIN);
		// Выстрел издалека зомби замечает не сразу (на стенде погоня началась через 15 с),
		// а босс без поля — лёгкая добыча для снайпера. Поэтому попадание игрока раскрывает поле сразу.
		if (!m_VB_Field && m_VB_Cfg && source && source.GetHierarchyRootPlayer())
			VB_OpenField();
		else
			VB_CheckPhases();
	}

	protected void VB_CheckPhases()
	{
		if (!m_VB_Cfg || !m_VB_Cfg.Phases || !m_VB_Field)
			return;
		float h = VB_GetHealth01();
		while (m_VB_PhaseIdx < m_VB_Cfg.Phases.Count())
		{
			VB_AB_PhaseCfg ph = m_VB_Cfg.Phases[m_VB_PhaseIdx];
			if (h > ph.Health)
				break;
			m_VB_PhaseIdx++;
			VB_AB_Log.Info("босс " + VB_GetId() + ": фаза " + m_VB_PhaseIdx + " (здоровье " + Math.Round(h * 100) + " %)");
			m_VB_Field.AddPhase(ph, GetPosition());
			if (m_VB_Cfg.Field.PhaseShieldSec > 0)
			{
				m_VB_InvulnUntil = VB_AB_Server.Now() + m_VB_Cfg.Field.PhaseShieldSec;
				VB_AB_Fx.Send(this, "vb_ab_lince_active", "vb_ab_linceagr_active", GetPosition());
			}
		}
	}

	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		if (!GetGame().IsServer() || m_VB_Dead)
			return;
		m_VB_Dead = true;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_BossTick);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_AbilityFire);
		m_VB_Abil.Clear();
		float after = 120;
		float corpseMin = 15;
		if (m_VB_Cfg)
		{
			after = m_VB_Cfg.Field.AfterDeathSec;
			if (m_VB_Cfg.Trophy)
				corpseMin = m_VB_Cfg.Trophy.CorpseLifetimeMin;
		}
		if (m_VB_Field)
			m_VB_Field.Collapse(after);
		Man killerMan = m_VB_LastHitter;
		EntityAI k = EntityAI.Cast(killer);
		if (k && k.GetHierarchyRootPlayer())
			killerMan = k.GetHierarchyRootPlayer();
		string who = "?";
		if (killerMan && killerMan.GetIdentity())
			who = killerMan.GetIdentity().GetName() + " (" + killerMan.GetIdentity().GetPlainId() + ")";
		VB_AB_Log.Info("босс " + VB_GetId() + " убит: " + who + ", " + GetPosition().ToString());
		if (m_VB_Point)
			m_VB_Point.OnBossDead(this);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_RemoveCorpse, corpseMin * 60000, false);
		// CE даёт мёртвому зомби 330 с жизни, и тело пропало бы раньше CorpseLifetimeMin. Причём назначает их
		// не сразу: на стенде через 98 с после смерти осталось 277 с, хотя через 1 с ставили 930. Поэтому
		// своё время (остаток до VB_RemoveCorpse) ставим сразу и потом каждые 20 с, пока тело лежит.
		m_VB_CorpseUntil = VB_AB_Server.Now() + corpseMin * 60 + 30;
		VB_KeepCorpse();
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_KeepCorpse, 20000, true);
	}

	protected float m_VB_CorpseUntil;

	void VB_KeepCorpse()
	{
		float left = Math.Max(30, m_VB_CorpseUntil - VB_AB_Server.Now());
		SetLifetimeMax(left);
		SetLifetime(left);
	}

	void VB_RemoveCorpse()
	{
		GetGame().ObjectDelete(this);
	}
};
