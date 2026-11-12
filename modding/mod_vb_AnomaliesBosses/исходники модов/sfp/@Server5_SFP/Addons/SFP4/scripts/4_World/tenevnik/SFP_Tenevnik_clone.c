// Клон должен наследоваться от заражённых (ZombieBase), чтобы работали анимации и ИИ.
// Одежда/шмот отображаются через прокси на p3d модели и слоты вложений в config.cpp.
class SFP_Tenevnik_clone extends ZombieBase
{
	// RPC (удалённые вызовы) для FX (эффекты) смерти клона (используем тот же ид, что и в мастере)
	static const int SFP_RPC_CLONE_DEATH_FX = 721501;

	protected SFP_Tenevnik m_SFP_Master;
	protected PlayerBase m_SFP_AggroTarget;
	protected ref Timer m_SFP_AggroPulse;
	static ref NoiseParams s_SFP_NoisePar;

	void SFP_SetMaster(SFP_Tenevnik master)
	{
		m_SFP_Master = master;
	}

	// версия 13: сделать так, чтобы клон сразу был "агрессивен" на игрока,
	// а не ходил, пока не увидит. Прямого SetTarget у инфектед в скриптах нет,
	// поэтому мы: 1) разворачиваем клона на цель, 2) создаём сильный шум-таргет на позиции игрока.
	void SFP_SetAggroTarget(PlayerBase pb)
	{
		if (!GetGame().IsServer())
			return;
		m_SFP_AggroTarget = pb;
		SFP_PulseAggro();

		if (!m_SFP_AggroPulse)
			m_SFP_AggroPulse = new Timer(CALL_CATEGORY_GAMEPLAY);
		m_SFP_AggroPulse.Stop();
		// несколько секунд пульсируем шумом, чтобы ИИ точно "подхватил" цель
		m_SFP_AggroPulse.Run(0.6, this, "SFP_PulseAggro", null, true);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.SFP_StopAggroPulse, 4500, false);
	}

	void SFP_StopAggroPulse()
	{
		if (m_SFP_AggroPulse)
			m_SFP_AggroPulse.Stop();
	}

	void SFP_PulseAggro()
	{
		if (!GetGame().IsServer())
			return;
		if (!IsAlive())
		{
			SFP_StopAggroPulse();
			return;
		}
		if (!m_SFP_AggroTarget || !m_SFP_AggroTarget.IsAlive())
		{
			SFP_StopAggroPulse();
			return;
		}

		// повернуть на цель
		vector dir = m_SFP_AggroTarget.GetPosition() - GetPosition();
		dir[1] = 0;
		if (dir.LengthSq() > 0.01)
		{
			dir.Normalize();
			float yaw = Math.Atan2(dir[0], dir[2]) * Math.RAD2DEG;
			// Порядок компонентов ориентации в DayZ: рыскание (Yaw), тангаж (Pitch), крен (Roll).
			// Вектор (0, yaw, 0) даёт тангаж = yaw, из‑за чего заражённый может заспавниться «на спине».
			SetOrientation(Vector(yaw, 0, 0));
		}

		// сильный шум на позиции игрока
		if (!s_SFP_NoisePar)
		{
			s_SFP_NoisePar = new NoiseParams();
			s_SFP_NoisePar.LoadFromPath("CfgVehicles SurvivorBase NoiseActionDefault");
		}
		NoiseSystem ns = GetGame().GetNoiseSystem();
		if (ns)
		{
			vector ppos = m_SFP_AggroTarget.GetPosition();
			ns.AddNoisePos(m_SFP_AggroTarget, ppos, s_SFP_NoisePar, 12.0);
			ns.AddNoiseTarget(ppos, 1.5, s_SFP_NoisePar, 12.0);
		}
	}

	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		if (GetGame().IsServer())
			SFP_StopAggroPulse();

		if (GetGame().IsServer())
		{
			vector pos = GetPosition();
			// версия 14: FX (эффекты) шлём RPC (удалённые вызовы) от САМОГО КЛОНА.
			// Клон точно находится в сетевом пузыре у игрока, который его убил.
			Param1<vector> p = new Param1<vector>(pos);
			GetGame().RPCSingleParam(this, SFP_RPC_CLONE_DEATH_FX, p, true);

			if (m_SFP_Master)
				m_SFP_Master.SFP_OnCloneKilled(this, pos);

			// удалить тело + инвентарь почти сразу
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(GetGame().ObjectDelete, 100, false, this);
		}
	}

	// ------------------------------------------------------------
	// Клиентские эффекты: партикл + звук (НЕ привязаны к клону)
	// ------------------------------------------------------------
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);

		if (rpc_type != SFP_RPC_CLONE_DEATH_FX)
			return;

		if (GetGame().IsDedicatedServer())
			return;

		Param1<vector> ppos;
		if (!ctx.Read(ppos))
			return;

		SFP_PlayCloneDeathFX_Client(ppos.param1);
	}

	protected void SFP_PlayCloneDeathFX_Client(vector pos)
	{
		vector ppos = pos + "0 0.2 0";

		// Партикл (НЕ на объекте)
		int pid = SFP_Tenevnik.FX_ParticlesCloneDeath[0];
		if (ParticleList.IsValidId(pid))
		{
			Particle p = ParticleManager.GetInstance().PlayInWorld(pid, ppos);
			// если это цикличный-партикл — остановим через несколько секунд
			if (p)
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(p.Stop, 4000, false);
		}

		// Звук
		SEffectManager.PlaySound(SFP_Tenevnik.FX_SoundsetsCloneDeath[0], ppos);
	}


}
