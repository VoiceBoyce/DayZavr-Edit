// Расположение леса (ТЗ_сервер_18.md п. 2б.11) — клиентская и общая часть: ступени, названия, последнее значение
// от сервера. Считает и хранит расположение только сервер (VB_AncientRoot_Server, VB_AR_FavorServer.c), игроку
// присылает RPC_FAVOR (значение и на сколько изменилось) — при заходе и при каждом изменении.
// Знак в худе — VB_AR_FavorHud.c (5_Mission), пути вверх (Отвар, Семя) — VB_AR_Recovery.c.
//
// Ступени: Друг леса ≥ 60, Свой ≥ 20, Чужак −19…19, Враг леса ≤ −20, Проклятый ≤ −60.

class VB_AR_Favor
{
	static const int RPC_FAVOR = 1447186020;		// сервер -> клиент: Param3<float, float, bool> (значение, изменение, прощение)
	static const int RPC_HOWL = 1447186021;		// сервер -> клиент: Param1<vector> — вой вдали перед волной
	static const int RPC_SIEGE = 1447186022;	// сервер -> клиент: Param1<vector> — откуда трещат корни (нашествие нечисти)

	static const int TIER_CURSED = -2;
	static const int TIER_ENEMY = -1;
	static const int TIER_STRANGER = 0;
	static const int TIER_OWN = 1;
	static const int TIER_FRIEND = 2;

	static const float FRIEND_FROM = 60;
	static const float OWN_FROM = 20;
	static const float ENEMY_TO = -20;
	static const float CURSED_TO = -60;

	// клиент: что прислал сервер
	static float s_VB_Value;
	static float s_VB_LastDelta;
	static int s_VB_ChangedAt = -100000;		// g_Game.GetTime() последнего изменения (для всплывающего «+10»)
	static bool s_VB_Known;
	static bool s_VB_Pardon;		// после Отвара примирения роща примет один дар даже у «Проклятого»

	static int VB_TierOf(float v)
	{
		if (v >= FRIEND_FROM)
			return TIER_FRIEND;
		if (v >= OWN_FROM)
			return TIER_OWN;
		if (v > ENEMY_TO)
			return TIER_STRANGER;
		if (v > CURSED_TO)
			return TIER_ENEMY;
		return TIER_CURSED;
	}

	static string VB_TierName(int tier)
	{
		switch (tier)
		{
			case TIER_FRIEND: return "Друг леса";
			case TIER_OWN: return "Свой";
			case TIER_ENEMY: return "Враг леса";
			case TIER_CURSED: return "Проклятый";
		}
		return "Чужак";
	}

	// клиент: своя ступень (до первого RPC — Чужак)
	static int VB_MyTier()
	{
		if (!s_VB_Known)
			return TIER_STRANGER;
		return VB_TierOf(s_VB_Value);
	}
}

modded class PlayerBase
{
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		if (g_Game.IsDedicatedServer())
			return;

		if (rpc_type == VB_AR_Favor.RPC_FAVOR)
		{
			Param3<float, float, bool> p = new Param3<float, float, bool>(0, 0, false);
			if (!ctx.Read(p))
				return;
			VB_AR_Favor.s_VB_Value = p.param1;
			VB_AR_Favor.s_VB_LastDelta = p.param2;
			VB_AR_Favor.s_VB_Pardon = p.param3;
			VB_AR_Favor.s_VB_Known = true;
			if (p.param2 != 0)
				VB_AR_Favor.s_VB_ChangedAt = g_Game.GetTime();
		}
		else if (rpc_type == VB_AR_Favor.RPC_HOWL)
		{
			Param1<vector> h = new Param1<vector>(vector.Zero);
			if (!ctx.Read(h))
				return;
			EffectSound snd = SEffectManager.PlaySound("WolfHowl_SoundSet", h.param1);
			if (snd)
				snd.SetAutodestroy(true);
		}
		else if (rpc_type == VB_AR_Favor.RPC_SIEGE)
		{
			// нашествие нечисти: треск корней и шёпот рощи с той стороны (звуки мода, ТЗ 2в п.5 — из того, что есть)
			Param1<vector> sp = new Param1<vector>(vector.Zero);
			if (!ctx.Read(sp))
				return;
			EffectSound crack = SEffectManager.PlaySound(VB_AR_Pulse.CREAK_SET, sp.param1);
			if (crack)
				crack.SetAutodestroy(true);
			EffectSound whisper = SEffectManager.PlaySound(VB_AR_Pulse.WHISPER_SET, sp.param1);
			if (whisper)
				whisper.SetAutodestroy(true);
		}
	}
}
