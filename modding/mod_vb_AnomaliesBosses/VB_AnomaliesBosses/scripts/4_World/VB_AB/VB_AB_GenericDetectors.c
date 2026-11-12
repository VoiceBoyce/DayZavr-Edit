// Чужие детекторы (ТЗ 2.2): строки detectors.json с Signal = "Generic", например детекторы OOTS.
// Их экран про наши находки ничего не знает, поэтому мод подаёт свой писк. Кода чужого мода не касаемся:
// работаем только по имени класса предмета в руках.
//
// Сервер раз в секунду смотрит, что у каждого игрока в руках. Если это чужой детектор из detectors.json и он
// работает (у предмета нет менеджера энергии или GetCompEM().IsWorking()), ищется ближайшая разрешённая находка
// по его Tier и Radius (во все стороны). Ближе SpawnDistance — находка становится предметом, иначе игроку уходит
// RPC с силой сигнала, и клиент пищит из предмета: чем ближе, тем чаще и громче.
//
// Запуск: VB_AB_GenericDetectors.Start() на сервере (MissionServer.OnInit, после VB_AB_Server.Init()).

class VB_AB_GenericDetectors
{
	static const int TICK_MS = 1000;
	static const int RPC_SIGNAL = 0x56414202;	// сервер → игрок: сила сигнала 0..100, -1 — сигнал пропал
	static const int RPC_FOUND = 0x56414203;	// сервер → клиенты рядом с игроком: звук появления находки

	protected static ref VB_AB_GenericDetectors s_Instance;
	protected ref array<Man> m_Players = new array<Man>;
	protected ref array<string> m_Signaled = new array<string>;		// кому в прошлый такт ушёл сигнал
	protected ref array<string> m_SignaledNow = new array<string>;

	static void Start()
	{
		if (s_Instance || !GetGame().IsServer())
			return;
		s_Instance = new VB_AB_GenericDetectors;
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(s_Instance.Tick, TICK_MS, true);
		VB_AB_Log.Info("писк для чужих детекторов (Signal Generic) запущен");
	}

	protected void Tick()
	{
		VB_AB_Cfg cfg = VB_AB_Cfg.Get();
		bool scan = VB_AB_Finds.Count() > 0 && HasGeneric(cfg);
		if (!scan && m_Signaled.Count() == 0)
			return;
		m_SignaledNow.Clear();
		m_Players.Clear();
		GetGame().GetPlayers(m_Players);
		PlayerBase p;
		string id;
		bool sent;
		foreach (Man m : m_Players)
		{
			p = PlayerBase.Cast(m);
			if (!p || !p.GetIdentity())
				continue;
			id = p.GetIdentity().GetId();
			sent = false;
			if (scan)
				sent = ScanPlayer(p, cfg);
			if (sent)
				m_SignaledNow.Insert(id);
			else if (m_Signaled.Find(id) != -1)
				Send(p, -1);		// сигнал пропал — клиент замолкает сразу, а не по таймауту
		}
		m_Signaled.Clear();
		m_Signaled.Copy(m_SignaledNow);
	}

	// true — игроку ушёл сигнал.
	protected bool ScanPlayer(PlayerBase p, VB_AB_Cfg cfg)
	{
		if (!p.IsAlive())
			return false;
		ItemBase item = p.GetItemInHands();
		if (!item)
			return false;
		string type = item.GetType();
		VB_AB_DetectorCfg det = cfg.m_Detectors.Get(type);
		if (!det || !IsGeneric(det))
			return false;
		if (item.HasEnergyManager() && !item.GetCompEM().IsWorking())
			return false;
		float dist;
		VB_AB_Find f = VB_AB_Finds.Nearest(p.GetPosition(), det.Radius, type, det.Tier, dist);
		if (!f)
			return false;
		if (dist < det.SpawnDistance)
		{
			vector at = f.m_Pos;
			VB_AB_Finds.Materialize(f, p, type);
			GetGame().RPCSingleParam(p, RPC_FOUND, new Param1<vector>(at), true);
			return false;
		}
		float k = 1;
		if (det.Radius > det.SpawnDistance)
			k = Math.Clamp(Math.InverseLerp(det.Radius, det.SpawnDistance, dist), 0, 1);
		int strength = Math.Round(k * 100);
		Send(p, strength);
		return true;
	}

	protected void Send(PlayerBase p, int strength)
	{
		GetGame().RPCSingleParam(p, RPC_SIGNAL, new Param1<int>(strength), true, p.GetIdentity());
	}

	static bool IsGeneric(VB_AB_DetectorCfg det)
	{
		string s = det.Signal;
		s.ToLower();
		return s == "generic";
	}

	protected bool HasGeneric(VB_AB_Cfg cfg)
	{
		foreach (string cls, VB_AB_DetectorCfg det : cfg.m_Detectors)
		{
			if (det && IsGeneric(det))
				return true;
		}
		return false;
	}
};

// Клиент: писк из чужого детектора в руках. Сила 0..1 (1 — вплотную): интервал 1,1 с → 0,15 с, громкость 35 → 100 %.
// Звук — VB_AB_DetectorBeep_SoundSet, если он есть в config.cpp (громкость тогда меняется по силе); иначе запасной
// одиночный писк vb_ab_detector_far из CfgSounds (у него громкость постоянная, ближе только чаще).
class VB_AB_GenericBeeper
{
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

	static void OnFound(vector pos)
	{
		EffectSound s = SEffectManager.PlaySound("VB_AB_Found_SoundSet", pos);
		if (s)
			s.SetAutodestroy(true);
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
		if (rpc_type == VB_AB_GenericDetectors.RPC_SIGNAL)
		{
			Param1<int> sig = new Param1<int>(0);
			if (ctx.Read(sig))
				VB_AB_GenericBeeper.OnSignal(sig.param1);
		}
		else if (rpc_type == VB_AB_GenericDetectors.RPC_FOUND)
		{
			Param1<vector> at = new Param1<vector>(vector.Zero);
			if (ctx.Read(at))
				VB_AB_GenericBeeper.OnFound(at.param1);
		}
	}
};
