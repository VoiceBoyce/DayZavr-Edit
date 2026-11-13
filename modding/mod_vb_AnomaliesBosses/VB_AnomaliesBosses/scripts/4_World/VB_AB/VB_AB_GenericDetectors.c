// Чужие детекторы (ТЗ 2.2), клиентская часть: писк из чужого детектора в руках по сигналу сервера.
// Кто и когда получает сигнал, решает серверный мод (VB_AnomaliesBosses_Server, VB_AB_GenericDetectors.c).

// Клиент: писк из чужого детектора в руках. Сила 0..1 (1 — вплотную): интервал 1,1 с → 0,15 с, громкость 35 → 100 %.
// Звук — VB_AB_DetectorBeep_SoundSet, если он есть в config.cpp (громкость тогда меняется по силе); иначе запасной
// одиночный писк vb_ab_detector_far из CfgSounds (у него громкость постоянная, ближе только чаще).
class VB_AB_GenericBeeper
{
	static const int RPC_SIGNAL = 0x56414205;	// сервер → игрок: сила сигнала 0..100, -1 — сигнал пропал
	static const int RPC_FOUND = 0x56414206;	// сервер → клиенты рядом с игроком: звук появления находки

	static const string BEEP_SET = "VB_AB_DetectorBeep_SoundSet";
	static const string BEEP_SOUND = "vb_ab_detector_far";
	static const int TIMEOUT_MS = 2500;		// дольше нет вестей от сервера — замолкаем

	protected static ref VB_AB_GenericBeeper s_Instance;
	protected float m_Strength;
	protected int m_Until;
	protected bool m_Queued;
	protected EntityAI m_Item;
	protected int m_UseSet = -1;			// -1 — ещё не проверяли config.cpp

	static void OnSignal(int strength)
	{
		if (!s_Instance)
			s_Instance = new VB_AB_GenericBeeper;
		s_Instance.VB_Signal(strength);
	}

	protected void VB_Signal(int strength)
	{
		if (strength < 0)
		{
			m_Until = 0;
			m_Queued = false;
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_Beep);
			return;
		}
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		if (!p)
			return;
		m_Item = p.GetItemInHands();
		float k = strength;
		m_Strength = Math.Clamp(k / 100, 0, 1);
		m_Until = GetGame().GetTime() + TIMEOUT_MS;
		if (!m_Queued)
			VB_Beep();
	}

	protected void VB_Beep()
	{
		m_Queued = false;
		if (GetGame().GetTime() > m_Until)
			return;
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		if (!p || !p.IsAlive() || !m_Item || p.GetItemInHands() != m_Item)
			return;
		// выключили — молчим сразу (батарейку, севшую на сервере, сервер отметит сигналом -1)
		if (m_Item.HasEnergyManager() && !m_Item.GetCompEM().IsSwitchedOn())
			return;
		float vol = Math.Lerp(0.35, 1.0, m_Strength);
		VB_PlayOn(m_Item, vol);
		int next = Math.Lerp(1100, 150, m_Strength);
		m_Queued = true;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_Beep, next, false);
	}

	protected void VB_PlayOn(EntityAI item, float vol)
	{
		if (m_UseSet < 0)
		{
			m_UseSet = 0;
			if (GetGame().ConfigIsExisting("CfgSoundSets " + BEEP_SET))
				m_UseSet = 1;
		}
		if (m_UseSet == 1)
		{
			EffectSound s = SEffectManager.PlaySoundOnObject(BEEP_SET, item);
			if (s)
			{
				s.SetSoundVolume(vol);
				s.SetAutodestroy(true);
			}
			return;
		}
		item.PlaySound(BEEP_SOUND, 10);
	}
};

// RPC общего сигнала приходят на объект игрока: так не нужно трогать классы чужих детекторов.
modded class PlayerBase
{
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		if (GetGame().IsDedicatedServer())
			return;
		if (rpc_type == VB_AB_GenericBeeper.RPC_SIGNAL)
		{
			Param1<int> sig = new Param1<int>(0);
			if (ctx.Read(sig))
				VB_AB_GenericBeeper.OnSignal(sig.param1);
		}
		else if (rpc_type == VB_AB_GenericBeeper.RPC_FOUND)
		{
			VB_AB_FoundFx.OnRpc(ctx);		// звук, всплеск частиц и вспышка стихии (VB_AB_Materials.c)
		}
	}
};
