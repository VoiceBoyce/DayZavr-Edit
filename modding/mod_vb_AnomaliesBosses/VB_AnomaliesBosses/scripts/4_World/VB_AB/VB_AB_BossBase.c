// Базовый класс всех боссов (config: VB_AB_BossBase). ТЗ, разделы 3–4.
//
// Здоровье: у класса в config.cpp огромный ванильный запас и нет смерти «в голову» (VB_AB_BOSS_DAMAGE),
// а настоящее здоровье — запас m_VB_HP из bosses.json (Health), его уменьшает каждое попадание.
// Поэтому другое здоровье босса не требует пересборки мода.
//
// Масштаб — абсолютный, каждый кадр (анимация сбрасывает матрицу, умножение накапливалось бы, см. MSFZ_Scale).
// Значение синхронизируется: на сервере его может переопределить bosses.json (Scale).

class VB_AB_BossBase extends ZmbM_HermitSkinny_Base
{
	static const int TICK_MS = 500;

	protected float m_VB_Scale = 1.0;			// синхр.
	protected float m_VB_CutTime = 15.0;		// синхр.: время «Вырезать трофей», с (Trophy.CutTimeSec)
	protected bool m_VB_Carved;					// синхр.: тело уже разрезано

	// сервер
	protected VB_AB_BossCfg m_VB_Cfg;
	protected VB_AB_PointRuntime m_VB_Point;
	protected ref VB_AB_Field m_VB_Field;
	protected float m_VB_HP;
	protected float m_VB_HPMax;
	protected int m_VB_PhaseIdx;
	protected float m_VB_NoPlayersSec;
	protected bool m_VB_Dead;
	protected Man m_VB_LastHitter;		// смерть наступает от SetHealth(0) без убийцы, поэтому помним стрелявшего

	void VB_AB_BossBase()
	{
		RegisterNetSyncVariableFloat("m_VB_Scale", 0.2, 5.0, 3);
		RegisterNetSyncVariableFloat("m_VB_CutTime", 0, 120, 1);
		RegisterNetSyncVariableBool("m_VB_Carved");
	}

	override void EEInit()
	{
		super.EEInit();
		float s = GetGame().ConfigGetFloat("CfgVehicles " + GetType() + " vbABScale");
		if (s > 0)
			m_VB_Scale = s;
		VB_UpdateScaleMask();
		if (GetGame().IsServer())
		{
			m_VB_HPMax = GetGame().ConfigGetFloat("CfgVehicles " + GetType() + " vbABHealth");
			if (m_VB_HPMax <= 0)
				m_VB_HPMax = 3000;
			m_VB_HP = m_VB_HPMax;
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_BossTick, TICK_MS, true);
		}
	}

	override void EEDelete(EntityAI parent)
	{
		if (GetGame() && GetGame().IsServer())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_BossTick);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_RemoveCorpse);
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_KeepCorpse);
			if (m_VB_Field && !m_VB_Dead)
				m_VB_Field.CloseNow();
		}
		super.EEDelete(parent);
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		VB_UpdateScaleMask();
	}

	protected void VB_UpdateScaleMask()
	{
		if (m_VB_Scale != 1.0)
			SetEventMask(EntityEvent.POSTFRAME);
	}

	override void EOnPostFrame(IEntity other, int extra)
	{
		if (m_VB_Scale != 1.0)
			VB_ApplyScale(m_VB_Scale);
	}

	protected void VB_ApplyScale(float scale)
	{
		vector transform[4];
		GetTransform(transform);
		for (int i = 0; i < 3; i++)
		{
			vector axis = transform[i];
			axis.Normalize();
			transform[i] = axis * scale;
		}
		SetTransform(transform);
		Update();
	}

	// Босса нельзя убить добиванием со спины.
	override bool CanBeBackstabbed()
	{
		return false;
	}

	// Попадания не сбивают босса анимацией оглушения.
	override bool EvaluateDamageHitAnimation(EntityAI pSource, string pComponent, string pAmmoType, out bool pHeavyHit, out int pAnimType, out float pAnimHitDir)
	{
		return false;
	}

	// Ванильный голос зомби боссу не подходит, а свои звуки мутантов ещё не перенесены:
	// без этого клиент ищет наборы <Класс>_CalmMove_SoundSet и пишет предупреждения (так же сделано у Энта в MSFZ).
	override protected void HandleSoundEvents()
	{
	}

	// Ванильная разделка не нужна: трофей режется своим действием (ТЗ 2.7).
	override bool CanBeSkinned()
	{
		return false;
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

	// ---------------------------------------------------------------- трофей (ТЗ 2.7)
	float VB_GetCutTime()
	{
		return m_VB_CutTime;
	}

	bool VB_IsCarved()
	{
		return m_VB_Carved;
	}

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
		EntityAI it;
		if (player)
			it = player.GetInventory().CreateInInventory(cls);
		if (!it)
		{
			vector p = GetPosition();
			if (player)
				p = player.GetPosition();
			it = EntityAI.Cast(GetGame().CreateObjectEx(cls, VB_AB_Finds.RandomGroundPos(p, 0.3, 1.0), ECE_PLACE_ON_SURFACE));
		}
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
			if (GetMindStateSynced() >= DayZInfectedConstants.MINDSTATE_CHASE)
				VB_OpenField();
			return;
		}
		vector pos = GetPosition();
		vector c = m_VB_Field.GetCenter();
		float d = vector.Distance(Vector(pos[0], 0, pos[2]), Vector(c[0], 0, c[2]));
		if (d > m_VB_Cfg.Field.LeashRadius)
			VB_ReturnToField(d);

		if (VB_AB_Server.AnyPlayerNear(c, m_VB_Cfg.Field.LeashRadius + 20))
			m_VB_NoPlayersSec = 0;
		else
		{
			m_VB_NoPlayersSec += TICK_MS / 1000.0;
			if (m_VB_NoPlayersSec >= VB_AB_Server.Settings().ResetAfterSec)
				VB_Reset();
		}
	}

	void VB_OpenField()
	{
		m_VB_Field = new VB_AB_Field(this, m_VB_Cfg, GetPosition());
		m_VB_NoPlayersSec = 0;
		m_VB_Field.Open();
		VB_CheckPhases();		// здоровье могли сбить ещё до раскрытия
	}

	// Увели дальше поводка — рывком возвращается в своё поле.
	void VB_ReturnToField(float dist)
	{
		vector c = VB_AB_Finds.RandomGroundPos(m_VB_Field.GetCenter(), 1, 3);
		SetPosition(c);
		VB_AB_Log.Info("босс " + VB_GetId() + " вернулся в поле: отошёл на " + Math.Round(dist) + " м, поводок " + m_VB_Cfg.Field.LeashRadius + " м");
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
		if (m_VB_Point)
			SetPosition(m_VB_Point.GetPos());
		VB_AB_Log.Info("босс " + VB_GetId() + ": игроки ушли, поле свёрнуто, здоровье восстановлено");
	}

	// Уход без боя (никого рядом SleepAfterMin минут) или «Убрать босса» в редакторе.
	void VB_Despawn()
	{
		if (m_VB_Field)
			m_VB_Field.CloseNow();
		m_VB_Field = null;
		m_VB_Dead = true;
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
		}
	}

	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		if (!GetGame().IsServer() || m_VB_Dead)
			return;
		m_VB_Dead = true;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_BossTick);
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
		// CE даёт мёртвому зомби 330 с жизни (на стенде lifetime тела был 330), и тело пропало бы раньше
		// CorpseLifetimeMin. Ставим своё время сразу и ещё раз через секунду — на случай, если CE назначит позже.
		m_VB_CorpseSec = corpseMin * 60 + 30;
		VB_KeepCorpse();
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_KeepCorpse, 1000, false);
	}

	protected float m_VB_CorpseSec;

	void VB_KeepCorpse()
	{
		SetLifetimeMax(m_VB_CorpseSec);
		SetLifetime(m_VB_CorpseSec);
	}

	void VB_RemoveCorpse()
	{
		GetGame().ObjectDelete(this);
	}
};
