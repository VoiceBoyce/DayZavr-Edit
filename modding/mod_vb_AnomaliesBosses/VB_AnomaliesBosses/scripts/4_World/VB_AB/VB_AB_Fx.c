// Эффекты способностей боссов на клиенте (ТЗ 4.3: у каждой способности предупреждение звуком и частицами).
// Сервер шлёт RPC от имени босса: всем, у кого босс в сети, — частица и звук в точке; одному игроку — эффект на экране.
// Частицы — те же, что у аномалий (VB_AB_ParticlesGen.c), звуки — их SoundSet (cfg/soundsets.hpp).

class VB_AB_Fx
{
	static const int RPC_FX = 0x56414202;		// «VAB» 02: частица и звук в точке
	static const int RPC_PLAYER = 0x56414203;	// «VAB» 03: эффект на экране одного игрока
	// Частицы аномалий частью зациклены (Repeat в .ptc): запущенные в мире без остановки они висели вечно —
	// вспышки фазы, разряды, шар Грозы оставались после смерти босса, «поле не пропадает» (2026-11-13).
	// Поэтому каждая частица эффекта гасится через FX_LIFE_MS. Долгие эффекты (снег Метели) перезапускаются чаще.
	static const int FX_LIFE_MS = 4000;

	// эффекты на экране (VB_AB_PlayerFxClient)
	static const int P_BLIND = 1;		// Вспышка
	static const int P_PSI = 2;			// Пси-удар
	static const int P_SCREAM = 3;		// Крик
	static const int P_COLD = 4;		// Ледяное дыхание
	static const int P_BLIZZARD = 5;	// Метель
	static const int P_SHAKE = 6;		// Удар оземь

	// ---------------------------------------------------------------- сервер
	static void Send(Object from, string ptc, string snd, vector pos)
	{
		if (!from || !GetGame().IsServer())
			return;
		GetGame().RPCSingleParam(from, RPC_FX, new Param3<string, string, vector>(ptc, snd, pos), true);
	}

	static void SendPlayer(Object from, PlayerBase p, int kind, float sec)
	{
		if (!from || !p || !p.GetIdentity() || !GetGame().IsServer())
			return;
		GetGame().RPCSingleParam(from, RPC_PLAYER, new Param2<int, float>(kind, sec), true, p.GetIdentity());
	}

	// ---------------------------------------------------------------- клиент
	static void Play(string ptc, string snd, vector pos)
	{
		if (ptc != "")
		{
			int id = ParticleList.GetParticleIDByName(ptc);
			if (id > 0)
			{
				// своя частица, не из пула ParticleManager: к сроку гашения пул мог бы отдать её чужому эффекту
				Particle p = Particle.PlayInWorld(id, pos);
				if (p)
					GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_AB_Fx.StopFx, FX_LIFE_MS, false, p);
			}
		}
		if (snd != "")
		{
			EffectSound s = SEffectManager.PlaySound(snd + "_SoundSet", pos);
			if (s)
				s.SetAutodestroy(true);
		}
	}

	static void StopFx(Particle p)
	{
		if (p)
			p.Stop();		// плавно: новые не рождаются, старые догорают; затем частица удаляется сама
	}

	static void OnPlayerFx(int kind, float sec)
	{
		PlayerBase me = PlayerBase.Cast(GetGame().GetPlayer());
		if (!me || !me.IsAlive())
			return;
		VB_AB_PlayerFxClient.Get().Apply(me, kind, sec);
	}
};
