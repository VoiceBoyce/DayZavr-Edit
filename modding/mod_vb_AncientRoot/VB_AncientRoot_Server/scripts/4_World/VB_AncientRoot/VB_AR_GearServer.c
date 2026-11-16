// Серверная часть изделий «Древнего корня»: заряды, таймеры, вызов стражей, вспышка пыльцы, Живица, клинок, плащ,
// светоч. Классы, поля, синхронизация, сохранение, условия действий и эффекты — VB_AncientRoot, VB_AR_Gear.c.
// Сон существ (ловушка) и терновая рана с кормлением клинка — VB_AR_CreatureServer.c.
// Все события пишутся в script-лог сервера строками [VB_AncientRoot] (отказы «Прислушаться» — только уведомлением игроку).

class VB_AR_GearServer
{
	protected static ref array<VB_AR_Lantern> s_VB_Lanterns;	// горящие светочи (для отпугивания)
	protected static ref map<string, int> s_VB_BloodUntil;		// SteamID -> g_Game.GetTime() конца «Живицы» (перезаход)

	// живой игрок ближе dist к точке
	static bool VB_PlayerNear(vector pos, float dist)
	{
		array<Man> players = new array<Man>;
		g_Game.GetPlayers(players);
		float dSq = dist * dist;
		foreach (Man m : players)
		{
			if (m && m.IsAlive() && vector.DistanceSq(m.GetPosition(), pos) < dSq)
				return true;
		}
		return false;
	}

	static string VB_IdOf(PlayerBase p)
	{
		if (!p || !p.GetIdentity())
			return "";
		return p.GetIdentity().GetPlainId();
	}

	// «Живица» по SteamID: конец действия переживает перезаход (сущность игрока пересоздаётся). Истёкшие записи чистим.
	static void VB_BloodRemember(string id, int until)
	{
		if (id == "")
			return;
		if (!s_VB_BloodUntil)
			s_VB_BloodUntil = new map<string, int>;
		int now = g_Game.GetTime();
		TStringArray old = new TStringArray;
		foreach (string key, int endMs : s_VB_BloodUntil)
		{
			if (endMs <= now)
				old.Insert(key);
		}
		foreach (string gone : old)
			s_VB_BloodUntil.Remove(gone);
		s_VB_BloodUntil.Set(id, until);
	}

	static void VB_BloodForget(string id)
	{
		if (s_VB_BloodUntil && id != "")
			s_VB_BloodUntil.Remove(id);
	}

	// сколько мс «Живицы» осталось у SteamID; 0 — нет
	static int VB_BloodLeft(string id)
	{
		if (!s_VB_BloodUntil || id == "")
			return 0;
		int until;
		if (!s_VB_BloodUntil.Find(id, until))
			return 0;
		int left = until - g_Game.GetTime();
		if (left <= 0)
		{
			s_VB_BloodUntil.Remove(id);
			return 0;
		}
		return left;
	}

	// вспышка пыльцы: живые зомби и звери в радиусе засыпают на ms (крупные — нет, VB_AR_CreatureState.VB_IsBig;
	// игроков не трогает), клиентам рядом — хлопок
	static int VB_PollenSleepServer(vector pos, float radius, int ms)
	{
		array<Object> objs = new array<Object>;
		g_Game.GetObjectsAtPosition(pos, radius, objs, null);
		int n = 0;
		DayZCreatureAI c;
		foreach (Object o : objs)
		{
			c = DayZCreatureAI.Cast(o);
			if (!c || !c.IsAlive())
				continue;
			if (VB_AR_CreatureState.VB_SleepServer(c, ms))
				n++;
		}
		VB_BurstFxServer(pos);
		string at = pos.ToString();
		Print("[VB_AncientRoot] вспышка пыльцы в " + at + ": уснуло существ " + n.ToString());
		return n;
	}

	// Показать вспышку игрокам рядом. Без identity не шлём: Send с пустой identity уходит ВСЕМ (№39).
	static void VB_BurstFxServer(vector pos)
	{
		array<Man> players = new array<Man>;
		g_Game.GetPlayers(players);
		float maxSq = VB_AR_Gear.BURST_FX_DIST * VB_AR_Gear.BURST_FX_DIST;
		PlayerIdentity id;
		ScriptRPC rpc;
		foreach (Man m : players)
		{
			if (!m)
				continue;
			id = m.GetIdentity();
			if (!id || vector.DistanceSq(m.GetPosition(), pos) > maxSq)
				continue;
			rpc = new ScriptRPC();
			rpc.Write(pos);
			rpc.Send(m, VB_AR_Gear.RPC_BURST, false, id);
		}
	}

	// гроза для Корня-вещуна: облачность уже за порогом молний или прогноз к нему идёт
	static int VB_StormServer()
	{
		Weather w = g_Game.GetWeather();
		if (!w)
			return VB_AR_ListenRoot.STORM_NONE;
		Overcast oc = w.GetOvercast();
		if (!oc)
			return VB_AR_ListenRoot.STORM_NONE;
		if (oc.GetActual() >= VB_AR_ListenRoot.STORM_OVERCAST)
			return VB_AR_ListenRoot.STORM_NOW;
		if (oc.GetForecast() >= VB_AR_ListenRoot.STORM_OVERCAST)
			return VB_AR_ListenRoot.STORM_COMING;
		return VB_AR_ListenRoot.STORM_NONE;
	}

	static void VB_LanternLit(VB_AR_Lantern lantern, bool on)
	{
		if (!lantern)
			return;
		if (!s_VB_Lanterns)
			s_VB_Lanterns = new array<VB_AR_Lantern>;
		int i = s_VB_Lanterns.Find(lantern);
		if (on && i < 0)
			s_VB_Lanterns.Insert(lantern);
		else if (!on && i >= 0)
			s_VB_Lanterns.Remove(i);
	}

	// Обычный заражённый (ванильный класс Zmb*, не страж) не трогает игрока ближе SCARE_DIST к горящему светочу (в руках
	// или на земле). Боссы, мутанты и Хранитель — тоже ZombieBase, но не Zmb*: игрока не теряют. Зовётся из
	// CanBeTargetedByAI часто: сначала пустой список и тип, конфиг стража — последним.
	static bool VB_LanternScares(PlayerBase player, EntityAI ai)
	{
		if (!s_VB_Lanterns || s_VB_Lanterns.Count() == 0 || !ZombieBase.Cast(ai))
			return false;
		string cls = ai.GetType();
		if (cls.IndexOf("Zmb") != 0)
			return false;

		vector pos = player.GetPosition();
		float sq = VB_AR_Lantern.SCARE_DIST * VB_AR_Lantern.SCARE_DIST;
		bool near = false;
		VB_AR_Lantern l;
		for (int i = s_VB_Lanterns.Count() - 1; i >= 0; i--)
		{
			l = s_VB_Lanterns[i];
			if (!l)
			{
				s_VB_Lanterns.Remove(i);
				continue;
			}
			if (!l.VB_IsOn() || !l.VB_IsShown())
				continue;
			if (vector.DistanceSq(VB_AR_Items.VB_WorldPos(l), pos) <= sq)
			{
				near = true;
				break;
			}
		}
		if (!near)
			return false;
		return !VB_AR_Guard.VB_Is(ai);
	}
}

// ============================================================================================
// «Корень-вещун»

modded class VB_ActionListenRoot
{
	static const int VB_COOLDOWN_MS = 5000;				// не чаще раза в 5 с на игрока
	protected static ref map<string, int> s_VB_LastUse;	// SteamID -> g_Game.GetTime() последнего «прислушаться»

	// кулдаун прошёл? Старые записи (старше кулдауна) чистим, когда их набралось много
	protected static bool VB_CooldownOk(string id, int now)
	{
		if (!s_VB_LastUse)
			s_VB_LastUse = new map<string, int>;
		int last;
		if (s_VB_LastUse.Find(id, last) && now - last < VB_COOLDOWN_MS)
			return false;
		if (s_VB_LastUse.Count() > 64)
		{
			TStringArray old = new TStringArray;
			foreach (string key, int at : s_VB_LastUse)
			{
				if (now - at >= VB_COOLDOWN_MS)
					old.Insert(key);
			}
			foreach (string gone : old)
				s_VB_LastUse.Remove(gone);
		}
		return true;
	}

	// Клиенту уходит только направление (сектор DIR_SECTOR_DEG) и ступень расстояния (DIST_STEP), не координаты
	// сердцевины: изменённый клиент не должен узнавать точку за 3 км (№39). Днём заряд не тратится, поиска нет.
	// Кулдаун VB_COOLDOWN_MS на игрока (без строк в лог). Отказ — уведомлением с причиной: в сумерках клиент может
	// считать «день» (действие без заряда), а сервер — «ночь» при пустом корне.
	override void OnExecuteServer(ActionData action_data)
	{
		ItemBase root = action_data.m_MainItem;
		PlayerBase player = action_data.m_Player;
		if (!root || !player || !player.GetIdentity())
			return;

		int now = g_Game.GetTime();
		string id = player.GetIdentity().GetPlainId();
		if (!VB_CooldownOk(id, now))
		{
			NotificationSystem.SendNotificationToPlayerExtended(player, 3, "Корень-вещун", "Корень ещё гудит после прошлого раза. Прислушайся через пару секунд.");
			return;
		}

		bool night = VB_AR_Pulse.VB_NightFactor() >= VB_AR_Pulse.NIGHT_LIGHT_ABOVE;
		if (night && root.GetQuantity() <= 0)
		{
			NotificationSystem.SendNotificationToPlayerExtended(player, 5, "Корень-вещун молчит", "Ночь уже наступила, а корень пуст: подпитай его у живой сердцевины.");
			return;
		}
		s_VB_LastUse.Set(id, now);

		VB_AR_Heart heart;
		vector from = player.GetPosition();
		if (night)
		{
			root.AddQuantity(-1);
			heart = VB_AR_Heart.VB_NearestServer(from, VB_AR_ListenRoot.RANGE);
		}
		bool found = heart != null;
		vector dir = vector.Zero;
		int step = 0;
		bool frantic = false;
		float dist = 0;
		int stage = 0;
		if (heart)
		{
			stage = heart.VB_Stage();
			vector d = heart.GetPosition() - from;
			d[1] = 0;
			dist = d.Length();
			float sector = VB_AR_ListenRoot.DIR_SECTOR_DEG * Math.DEG2RAD;
			float ang = Math.Atan2(d[0], d[2]);
			ang = Math.Round(ang / sector) * sector;
			float sx = Math.Sin(ang);
			float cz = Math.Cos(ang);
			dir = Vector(sx, 0, cz);
			step = Math.Floor(dist / VB_AR_ListenRoot.DIST_STEP);
			frantic = heart.VB_IsDisturbed();
		}
		int storm = VB_AR_GearServer.VB_StormServer();

		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(night);
		rpc.Write(found);
		rpc.Write(dir);
		rpc.Write(step);
		rpc.Write(frantic);
		rpc.Write(storm);
		rpc.Write(stage);
		rpc.Send(player, VB_AR_Gear.RPC_LISTEN, true, player.GetIdentity());

		string res = "день, сердцевины не слышно";
		if (night)
			res = "сердцевин в 3 км нет";
		if (heart)
		{
			int distM = Math.Round(dist);
			res = "сердцевина в " + distM.ToString() + " м";
		}
		string who = VB_AR_ItemsServer.VB_Who(player);
		string left = root.GetQuantity().ToString();
		Print("[VB_AncientRoot] " + who + " прислушался: " + res + ", гроза " + storm.ToString() + ", зарядов " + left);
	}
}

modded class VB_ActionFeedListenRoot
{
	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);

		VB_AR_Heart heart = VB_AR_Heart.Cast(action_data.m_Target.GetObject());
		if (heart)
			heart.VB_DisturbServer(action_data.m_Player, false);
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);

		VB_AR_Heart heart = VB_AR_Heart.Cast(action_data.m_Target.GetObject());
		if (heart && !heart.VB_IsDead() && action_data.m_MainItem)
			action_data.m_MainItem.SetQuantity(action_data.m_MainItem.GetQuantityMax());
	}
}

// ============================================================================================
// «Пыльцевая ловушка»

modded class VB_AR_SporeTrap
{
	void VB_ArmServer()
	{
		if (m_VB_Armed)
			return;

		m_VB_Armed = true;
		SetSynchDirty();
		VB_NoiseServer();
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_NoiseServer, NOISE_MS, true);
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_BurstServer, ARM_MS, false);
	}

	void VB_NoiseServer()
	{
		vector at = VB_AR_Items.VB_WorldPos(this);
		VB_AR_ItemsServer.VB_NoiseAt(at, NOISE, NOISE_MS * 0.001 + 1.0);
	}

	static const int VB_CLEANUP_MS = 1000;

	void VB_BurstServer()
	{
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_NoiseServer);
		vector at = VB_AR_Items.VB_WorldPos(this);
		VB_AR_GearServer.VB_PollenSleepServer(at, BURST_RADIUS, SLEEP_MS);
		VB_AR_ItemsServer.VB_ReplaceServer(this, "");
		// замена может не пройти (инвентарь занят) — тогда ловушка висела бы раскрытой вечно: через секунду убрать
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_CleanupServer, VB_CLEANUP_MS, false);
	}

	void VB_CleanupServer()
	{
		if (IsSetForDeletion())
			return;
		// замена прошла, а объект ещё доживает без места в инвентаре (E52) — только удалить
		InventoryLocation loc = new InventoryLocation();
		if (!GetInventory().GetCurrentInventoryLocation(loc))
		{
			Delete();
			return;
		}
		vector at = VB_AR_Items.VB_WorldPos(this);
		Print("[VB_AncientRoot] пыльцевая ловушка не заменилась после вспышки — удалена в " + at.ToString());
		DeleteSafe();
	}

	override void EEDelete(EntityAI parent)
	{
		if (g_Game)
		{
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_NoiseServer);
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_BurstServer);
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_CleanupServer);
		}
		super.EEDelete(parent);
	}
}

modded class VB_ActionArmSporeTrap
{
	override void OnExecuteServer(ActionData action_data)
	{
		VB_AR_SporeTrap trap = VB_AR_SporeTrap.Cast(action_data.m_MainItem);
		if (trap)
			trap.VB_ArmServer();
	}
}

// ============================================================================================
// «Светоч корня»: расход пыльцы, отпугивание обычных зомби (список горящих — VB_AR_GearServer) и зов стражей
// на свет (без лута, №26). Пыльца тратится, только пока светоч на виду (в руках или на земле): раз в BURN_TICK_MS
// копим время «на виду», набралась минута — минус минута пыльцы; в рюкзаке, на поясе, в ящике не тратится.
// Гашение не обнуляет таймеры: остаток такта, накопленное время и остаток до следующего зова сохраняются и
// продолжаются при следующем включении — гасить и зажигать, чтобы светоч светил вечно, нельзя (№34).
// Страж выходит, только если у светоча (CALL_PLAYER_DIST) есть живой игрок. Сломанный светоч гаснет сам (№39).

modded class VB_AR_Lantern
{
	protected bool m_VB_Running;					// сервер: идут таймеры расхода и зова
	protected int m_VB_TickLeftMs = BURN_TICK_MS;	// сервер: остаток текущего такта расхода
	protected int m_VB_BurnMs;						// сервер: сколько горел на виду в текущей минуте пыльцы
	protected int m_VB_CallLeftMs = CALL_MS;		// сервер: остаток до следующей попытки позвать стража

	void VB_SetOnServer(bool on)
	{
		if (on && (GetQuantity() <= 0 || IsRuined()))
			on = false;

		ScriptCallQueue q = g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY);
		if (m_VB_Running)
		{
			int left = q.GetRemainingTime(VB_BurnServer);
			if (left > 0)
				m_VB_TickLeftMs = left;
			left = q.GetRemainingTime(VB_CallServer);
			if (left > 0)
				m_VB_CallLeftMs = left;
			q.Remove(VB_BurnServer);
			q.Remove(VB_CallServer);
			m_VB_Running = false;
		}

		if (on)
		{
			q.CallLater(VB_BurnServer, m_VB_TickLeftMs, false);
			q.CallLater(VB_CallServer, m_VB_CallLeftMs, false);
			m_VB_Running = true;
		}
		VB_AR_GearServer.VB_LanternLit(this, on);

		if (m_VB_On != on)
		{
			m_VB_On = on;
			SetSynchDirty();
		}
	}

	// таймеры одноразовые и перезапускают себя сами: так GetRemainingTime всегда видит текущий остаток.
	// Такт расхода: на виду — копим время, минута набралась — минус минута пыльцы.
	void VB_BurnServer()
	{
		m_VB_TickLeftMs = BURN_TICK_MS;
		if (VB_IsShown())
			m_VB_BurnMs += BURN_TICK_MS;
		if (m_VB_BurnMs >= MINUTE_MS)
		{
			m_VB_BurnMs -= MINUTE_MS;
			AddQuantity(-1);
			if (GetQuantity() <= 0 || IsRuined())
			{
				VB_SetOnServer(false);
				return;
			}
		}
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_BurnServer, BURN_TICK_MS, false);
	}

	void VB_CallServer()
	{
		m_VB_CallLeftMs = CALL_MS;
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_CallServer, CALL_MS, false);

		if (!VB_IsShown() || VB_AR_Pulse.VB_NightFactor() < VB_AR_Pulse.NIGHT_FULL_ABOVE)
			return;
		if (Math.RandomFloat01() >= CALL_CHANCE)
			return;

		vector pos = VB_AR_Items.VB_WorldPos(this);
		if (!VB_AR_GearServer.VB_PlayerNear(pos, CALL_PLAYER_DIST))
			return;			// светоч брошен в лесу без людей — страж не выходит
		Print("[VB_AncientRoot] стражи увидели светоч корня у " + pos.ToString());
		VB_AR_Horde.VB_Spawn(pos, 1, CALL_DIST_MIN, CALL_DIST_MAX, false);	// без лута: светоч не ферма (№26)
		VB_AR_ItemsServer.VB_NoiseAt(pos, CALL_NOISE, 20);
	}

	override void EEHealthLevelChanged(int oldLevel, int newLevel, string zone)
	{
		super.EEHealthLevelChanged(oldLevel, newLevel, zone);
		if (m_VB_On && IsRuined())
			VB_SetOnServer(false);
	}

	override void AfterStoreLoad()
	{
		super.AfterStoreLoad();
		if (m_VB_On)
			VB_SetOnServer(true);
	}

	override void EEDelete(EntityAI parent)
	{
		if (g_Game)
		{
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_BurnServer);
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_CallServer);
		}
		m_VB_Running = false;
		VB_AR_GearServer.VB_LanternLit(this, false);
		super.EEDelete(parent);
	}
}

modded class VB_ActionLanternOn
{
	override void OnExecuteServer(ActionData action_data)
	{
		VB_AR_Lantern lantern = VB_AR_Lantern.Cast(action_data.m_MainItem);
		if (lantern)
			lantern.VB_SetOnServer(true);
	}
}

modded class VB_ActionLanternOff
{
	override void OnExecuteServer(ActionData action_data)
	{
		VB_AR_Lantern lantern = VB_AR_Lantern.Cast(action_data.m_MainItem);
		if (lantern)
			lantern.VB_SetOnServer(false);
	}
}

// ============================================================================================
// «Живица»: укол (ванильные ActionInjectSelf/Target зовут OnApply только на сервере)

modded class VB_AR_Blood
{
	override void OnApply(PlayerBase player)
	{
		if (!player)
			return;

		player.AddHealth("", "Blood", BLOOD_ADD);
		player.AddHealth("", "Health", HEALTH_ADD);
		player.VB_StartRootBloodServer();
		string legs = "";
		if (player.GetBrokenLegs() != eBrokenLegs.NO_BROKEN_LEGS)
			legs = " (перелом — срастается)";
		Print("[VB_AncientRoot] " + VB_AR_ItemsServer.VB_Who(player) + ": укол Живицы" + legs);
	}
}

// ============================================================================================
// «Терновый клинок»: кормление за убийство (зовёт VB_AR_CreatureServer.c), голод только в игре и не ниже BLOOD_FLOOR

modded class VB_AR_Blade
{
	protected int m_VB_LastMsgMs = -1;			// сервер: когда последний раз писали игроку про голод

	override void EEInit()
	{
		super.EEInit();
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_HungerServer, HUNGER_TICK_MS, true);
	}

	override void EEDelete(EntityAI parent)
	{
		if (g_Game)
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_HungerServer);
		super.EEDelete(parent);
	}

	void VB_OnKillServer(PlayerBase owner, string what)
	{
		owner.AddHealth("", "Blood", BLOOD_PER_KILL);
		AddHealth("", "", -EXTRA_WEAR);
		m_VB_Hunger = 0;
		Print("[VB_AncientRoot] " + VB_AR_ItemsServer.VB_Who(owner) + ": терновый клинок насытился (" + what + ")");
	}

	// Голод копится только у игрока в игре (клинок у него в инвентаре, игрок жив и подключён); офлайн, в ящике,
	// в машине — нет. Голодный клинок берёт кровь, но не ниже BLOOD_FLOOR: не убивает.
	void VB_HungerServer()
	{
		PlayerBase owner = PlayerBase.Cast(GetHierarchyRootPlayer());
		if (!owner || !owner.IsAlive() || !owner.GetIdentity() || IsRuined())
			return;

		m_VB_Hunger += HUNGER_TICK_MS / 1000;
		if (m_VB_Hunger < HUNGER_AFTER_S)
			return;
		m_VB_Hunger = HUNGER_AFTER_S;

		float blood = owner.GetHealth("", "Blood");
		float take = Math.Min(HUNGER_BLOOD, blood - BLOOD_FLOOR);
		if (take <= 0)
			return;

		owner.AddHealth("", "Blood", -take);
		int now = g_Game.GetTime();
		if (m_VB_LastMsgMs < 0 || now - m_VB_LastMsgMs >= HUNGER_MSG_S * 1000)
		{
			m_VB_LastMsgMs = now;
			owner.MessageStatus("Терновый клинок впивается шипами в руку. Накорми его.");
		}
	}
}

// ============================================================================================
// «Настой пыльцы»: выпил — флаг на BREW_MS (подсветка — клиент, VB_AR_BrewFx)

modded class VB_ActionDrinkPollenBrew
{
	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);
		PlayerBase p = action_data.m_Player;
		ItemBase item = action_data.m_MainItem;
		if (!p || !item)
			return;
		item.DeleteSafe();
		p.VB_StartBrewServer();
		Print("[VB_AncientRoot] " + VB_AR_ItemsServer.VB_Who(p) + ": выпил Настой пыльцы");
	}
}

// ============================================================================================
// Игрок: таймер «Живицы» и сращивание перелома (снимается и при смерти, №27; перезаход не снимает — конец по SteamID
// в VB_AR_GearServer, восстановление в OnConnect); таймер «Настоя пыльцы»;
// «Янтарный оберег» (есть ли в инвентаре — перебор не чаще раза в AMULET_CHECK_MS); «Плащ лешего» (обычные зомби и
// звери теряют присевшего неподвижного ночью; дешёвые проверки первыми, метка неподвижности сбрасывается при
// движении, №39); «Светоч корня» (обычные зомби не трогают игрока у горящего светоча).

modded class PlayerBase
{
	static const int VB_COCOON_STILL_MS = 3000;
	static const float VB_COCOON_MOVE_SQ = 0.09;	// сдвинулся больше чем на 0.3 м — отсчёт заново
	static const float VB_LEG_HEALTHY = 100.0;		// ваниль BrokenLegsMdfr: ноги здоровы — перелома нет

	protected int m_VB_CocoonSinceMs = -1;
	protected vector m_VB_CocoonPos;
	protected ref HumanMovementState m_VB_CocoonMs;
	protected bool m_VB_LegsMending;				// сервер: Живица сращивает перелом (для сообщения в конце)
	protected string m_VB_BloodId;					// сервер: SteamID, под которым записан конец Живицы (смерть без identity)
	protected bool m_VB_HasAmulet;					// сервер: кэш «оберег в инвентаре»
	protected int m_VB_AmuletCheckMs = -100000;

	// «Янтарный оберег» где угодно в инвентаре (целый). Зовётся из CanBeTargetedByAI — поэтому кэш.
	bool VB_HasAmulet()
	{
		int now = g_Game.GetTime();
		if (now - m_VB_AmuletCheckMs < VB_AR_Amulet.AMULET_CHECK_MS)
			return m_VB_HasAmulet;
		m_VB_AmuletCheckMs = now;
		m_VB_HasAmulet = false;
		array<EntityAI> items = new array<EntityAI>;
		GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
		foreach (EntityAI e : items)
		{
			if (e && e != this && e.IsKindOf("VB_AR_Amulet") && !e.IsRuined())
			{
				m_VB_HasAmulet = true;
				break;
			}
		}
		return m_VB_HasAmulet;
	}

	// оберег не помогает тому, кто в этот рестарт вырезал сердцевину
	bool VB_AmuletWorks()
	{
		return VB_HasAmulet() && !VB_AR_FavorServer.VB_IsCutter(this);
	}

	void VB_StartBrewServer()
	{
		m_VB_AR_Brew = true;
		SetSynchDirty();
		ScriptCallQueue q = g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY);
		q.Remove(VB_EndBrewServer);
		q.CallLater(VB_EndBrewServer, VB_AR_PollenBrew.BREW_MS, false);
	}

	void VB_EndBrewServer()
	{
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_EndBrewServer);
		if (!m_VB_AR_Brew)
			return;
		m_VB_AR_Brew = false;
		SetSynchDirty();
	}

	// укол: конец действия записываем по SteamID (VB_AR_GearServer.VB_BloodRemember) — перезаход его не снимет
	void VB_StartRootBloodServer()
	{
		m_VB_BloodId = VB_AR_GearServer.VB_IdOf(this);
		VB_AR_GearServer.VB_BloodRemember(m_VB_BloodId, g_Game.GetTime() + VB_AR_Blood.BLOOD_MS);
		VB_RunRootBloodServer(VB_AR_Blood.BLOOD_MS);
	}

	protected void VB_RunRootBloodServer(int ms)
	{
		m_VB_AR_Blood = true;
		SetSynchDirty();
		m_VB_LegsMending = GetBrokenLegs() != eBrokenLegs.NO_BROKEN_LEGS;
		ScriptCallQueue q = g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY);
		q.Remove(VB_EndRootBloodServer);
		q.Remove(VB_RootBloodTickServer);
		q.CallLater(VB_EndRootBloodServer, ms, false);
		q.CallLater(VB_RootBloodTickServer, VB_AR_Blood.HEAL_TICK_MS, true);
	}

	// вход в игру (новая сущность из базы): «Живица» ещё действует — дрожь и сращивание на остаток времени
	override void OnConnect()
	{
		super.OnConnect();
		string id = VB_AR_GearServer.VB_IdOf(this);
		int left = VB_AR_GearServer.VB_BloodLeft(id);
		if (left <= 0 || !IsAlive())
			return;
		m_VB_BloodId = id;
		VB_RunRootBloodServer(left);
		int leftS = left / 1000;
		string who = VB_AR_ItemsServer.VB_Who(this);
		string secs = leftS.ToString();
		Print("[VB_AncientRoot] " + who + ": Живица продолжается после входа, осталось " + secs + " с");
	}

	// перелом срастается: ногам понемногу здоровья, ваниль сама снимает перелом и шину, когда ноги здоровы
	void VB_RootBloodTickServer()
	{
		if (!m_VB_AR_Blood || !IsAlive())
			return;
		if (GetBrokenLegs() == eBrokenLegs.NO_BROKEN_LEGS)
		{
			if (m_VB_LegsMending)
			{
				m_VB_LegsMending = false;
				MessageStatus("Живица срастила кость. Нога снова держит.");
				Print("[VB_AncientRoot] " + VB_AR_ItemsServer.VB_Who(this) + ": Живица срастила перелом");
			}
			return;
		}
		m_VB_LegsMending = true;
		float heal = VB_AR_Blood.LEG_HEAL;
		AddHealth("LeftLeg", "Health", heal);
		AddHealth("RightLeg", "Health", heal);
		AddHealth("LeftFoot", "Health", heal);
		AddHealth("RightFoot", "Health", heal);
	}

	// конец по таймеру или смерть: запись по SteamID стираем (выход из игры сюда не попадает — EEDelete)
	void VB_EndRootBloodServer()
	{
		ScriptCallQueue q = g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY);
		q.Remove(VB_EndRootBloodServer);
		q.Remove(VB_RootBloodTickServer);
		if (m_VB_BloodId != "")
		{
			VB_AR_GearServer.VB_BloodForget(m_VB_BloodId);
			m_VB_BloodId = "";
		}
		if (!m_VB_AR_Blood)
			return;

		m_VB_AR_Blood = false;
		m_VB_LegsMending = false;
		SetSynchDirty();
	}

	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		VB_EndRootBloodServer();
		VB_EndBrewServer();
	}

	override bool CanBeTargetedByAI(EntityAI ai)
	{
		if (!super.CanBeTargetedByAI(ai))
			return false;
		if (VB_CocoonHides(ai))
			return false;
		return !VB_AR_GearServer.VB_LanternScares(this, ai);
	}

	// «Плащ лешего»: ночь, присел, не двигается VB_COCOON_STILL_MS — обычный зомби или зверь игрока не видит
	protected bool VB_CocoonHides(EntityAI ai)
	{
		if (!ZombieBase.Cast(ai) && !AnimalBase.Cast(ai))
			return false;

		EntityAI body = GetInventory().FindAttachment(InventorySlots.BODY);
		if (!body || body.IsRuined() || !body.IsKindOf("VB_AR_Cloak"))
			return false;

		if (VB_AR_Pulse.VB_NightFactor() < VB_AR_Pulse.NIGHT_FULL_ABOVE)
			return false;

		if (!VB_CocoonStill())
			return false;

		return !VB_AR_Guard.VB_Is(ai);		// стражей рощи не обмануть; чтение конфига — последним
	}

	// Присел и не двигается VB_COCOON_STILL_MS. Метка ставится на месте и сбрасывается, если игрок встал, пошёл
	// или сдвинулся: иначе старая метка из прошлой засады прятала его сразу на новом месте.
	protected bool VB_CocoonStill()
	{
		if (!m_VB_CocoonMs)
			m_VB_CocoonMs = new HumanMovementState();
		GetMovementState(m_VB_CocoonMs);
		if (m_VB_CocoonMs.m_iStanceIdx != DayZPlayerConstants.STANCEIDX_CROUCH || m_VB_CocoonMs.m_iMovement != DayZPlayerConstants.MOVEMENTIDX_IDLE)
		{
			m_VB_CocoonSinceMs = -1;
			return false;
		}

		int now = g_Game.GetTime();
		vector pos = GetPosition();
		if (m_VB_CocoonSinceMs < 0 || vector.DistanceSq(pos, m_VB_CocoonPos) > VB_COCOON_MOVE_SQ)
		{
			m_VB_CocoonSinceMs = now;
			m_VB_CocoonPos = pos;
		}
		return now - m_VB_CocoonSinceMs >= VB_COCOON_STILL_MS;
	}

	override void EEDelete(EntityAI parent)
	{
		if (g_Game)
		{
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_EndRootBloodServer);
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_RootBloodTickServer);
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_EndBrewServer);
		}
		super.EEDelete(parent);
	}
}
