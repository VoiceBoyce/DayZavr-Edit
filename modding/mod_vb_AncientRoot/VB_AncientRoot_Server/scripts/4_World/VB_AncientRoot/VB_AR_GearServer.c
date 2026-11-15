// Серверная часть изделий Изнанки: заряды, таймеры, вызов зомби, хлопки спор, кровь, клинок, жгут, нить, плащ-кокон.
// Классы, поля, синхронизация, сохранение, условия действий и эффекты — VB_AncientRoot, VB_AR_Gear.c.
// Все события пишутся в script-лог сервера строками [VB_AncientRoot].

class VB_AR_GearServer
{
	// облако спор лопается — бьёт живых зомби в радиусе (игроков не трогает), клиентам рядом — хлопок
	static int VB_SporeBurstServer(vector pos, float radius, float damage)
	{
		array<Object> objs = new array<Object>;
		g_Game.GetObjectsAtPosition(pos, radius, objs, null);
		int hit = 0;
		ZombieBase zmb;
		foreach (Object o : objs)
		{
			zmb = ZombieBase.Cast(o);
			if (!zmb || !zmb.IsAlive())
				continue;
			zmb.AddHealth("", "", -damage);
			hit++;
		}
		VB_BurstFxServer(pos);
		string at = pos.ToString();
		string n = hit.ToString();
		Print("[VB_AncientRoot] хлопок спор в " + at + ": задето зомби " + n);
		return hit;
	}

	// Показать хлопок спор игрокам рядом. Без identity не шлём: Send с пустой identity уходит ВСЕМ (№39).
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
}

// ============================================================================================
// «Слушающий корень»

modded class VB_ActionListenRoot
{
	// Клиенту уходит только направление (сектор DIR_SECTOR_DEG) и ступень расстояния (DIST_STEP), не координаты
	// сердца: изменённый клиент не должен узнавать точку сердца за 3 км (№39).
	override void OnExecuteServer(ActionData action_data)
	{
		ItemBase root = action_data.m_MainItem;
		PlayerBase player = action_data.m_Player;
		if (!root || !player || !player.GetIdentity() || root.GetQuantity() <= 0)
			return;

		root.AddQuantity(-1);
		vector from = player.GetPosition();
		VB_AR_Heart heart = VB_AR_Heart.VB_NearestServer(from, VB_AR_ListenRoot.RANGE);
		bool found = heart != null;
		vector dir = vector.Zero;
		int step = 0;
		bool frantic = false;
		float dist = 0;
		if (heart)
		{
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

		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(found);
		rpc.Write(dir);
		rpc.Write(step);
		rpc.Write(frantic);
		rpc.Send(player, VB_AR_Gear.RPC_LISTEN, true, player.GetIdentity());

		string res = "сердец в 3 км нет";
		if (heart)
		{
			int distM = Math.Round(dist);
			res = "сердце в " + distM.ToString() + " м";
		}
		string who = VB_AR_ItemsServer.VB_Who(player);
		string left = root.GetQuantity().ToString();
		Print("[VB_AncientRoot] " + who + " прислушался: " + res + ", зарядов " + left);
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
// «Сердце-аккумулятор»: раз в минуту ночью заряжается, днём садится

modded class VB_AR_HeartBattery
{
	override void EEInit()
	{
		super.EEInit();
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_SelfServer, SELF_MS, true);
	}

	override void EEDelete(EntityAI parent)
	{
		if (g_Game)
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_SelfServer);
		super.EEDelete(parent);
	}

	void VB_SelfServer()
	{
		ComponentEnergyManager em = GetCompEM();
		if (!em || IsRuined())
			return;

		float night = VB_AR_Pulse.VB_NightFactor();
		if (night >= VB_AR_Pulse.NIGHT_FULL_ABOVE)
			em.AddEnergy(CHARGE_PER_MIN);
		else if (night < VB_AR_Pulse.NIGHT_DAY_BELOW)
			em.AddEnergy(-DRAIN_PER_MIN);
	}
}

// ============================================================================================
// «Споровая ловушка»

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

	void VB_BurstServer()
	{
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_NoiseServer);
		vector at = VB_AR_Items.VB_WorldPos(this);
		VB_AR_GearServer.VB_SporeBurstServer(at, BURST_RADIUS, BURST_DAMAGE);
		VB_AR_ItemsServer.VB_ReplaceServer(this, "");
	}

	override void EEDelete(EntityAI parent)
	{
		if (g_Game)
		{
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_NoiseServer);
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_BurstServer);
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
// «Фонарь-сердце»: расход минуты спор и зов зомби Изнанки (без лута, №26).
// Гашение не обнуляет таймеры: остаток текущей минуты и остаток до следующего зова сохраняются и продолжаются при
// следующем включении — гасить и зажигать раз в 50 с больше нельзя, чтобы фонарь светил вечно (№34).
// Сломанный фонарь гаснет сам (№39).

modded class VB_AR_Lantern
{
	protected bool m_VB_Running;					// сервер: идут таймеры расхода и зова
	protected int m_VB_MinuteLeftMs = MINUTE_MS;	// сервер: остаток текущей минуты расхода
	protected int m_VB_CallLeftMs = CALL_MS;		// сервер: остаток до следующей попытки позвать зомби

	void VB_SetOnServer(bool on)
	{
		if (on && (GetQuantity() <= 0 || IsRuined()))
			on = false;

		ScriptCallQueue q = g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY);
		if (m_VB_Running)
		{
			int left = q.GetRemainingTime(VB_MinuteServer);
			if (left > 0)
				m_VB_MinuteLeftMs = left;
			left = q.GetRemainingTime(VB_CallServer);
			if (left > 0)
				m_VB_CallLeftMs = left;
			q.Remove(VB_MinuteServer);
			q.Remove(VB_CallServer);
			m_VB_Running = false;
		}

		if (on)
		{
			q.CallLater(VB_MinuteServer, m_VB_MinuteLeftMs, false);
			q.CallLater(VB_CallServer, m_VB_CallLeftMs, false);
			m_VB_Running = true;
		}

		if (m_VB_On != on)
		{
			m_VB_On = on;
			SetSynchDirty();
		}
	}

	// таймеры одноразовые и перезапускают себя сами: так GetRemainingTime всегда видит текущий остаток
	void VB_MinuteServer()
	{
		m_VB_MinuteLeftMs = MINUTE_MS;
		AddQuantity(-1);
		if (GetQuantity() <= 0 || IsRuined())
		{
			VB_SetOnServer(false);
			return;
		}
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_MinuteServer, MINUTE_MS, false);
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
		Print("[VB_AncientRoot] светоч корня зовёт стражей к " + pos.ToString());
		VB_AR_Horde.VB_Spawn(pos, 1, 45, 70, false);	// без лута: фонарь не ферма материалов (№26)
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
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_MinuteServer);
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_CallServer);
		}
		m_VB_Running = false;
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
// «Кровь Изнанки»: укол (ванильные ActionInjectSelf/Target зовут OnApply только на сервере)

modded class VB_AR_Blood
{
	override void OnApply(PlayerBase player)
	{
		if (!player)
			return;

		player.AddHealth("", "Blood", BLOOD_ADD);
		player.AddHealth("", "Health", HEALTH_ADD);
		player.VB_StartRootBloodServer();
		Print("[VB_AncientRoot] " + VB_AR_ItemsServer.VB_Who(player) + ": укол Крови Изнанки");
	}
}

// ============================================================================================
// «Топливная капсула»: залить в бак; машина полчаса тянет шлейф, на остановках зовёт зомби Изнанки (без лута, №26)

modded class VB_ActionPourFuelCapsule
{
	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);

		CarScript car = VB_Car(action_data.m_Target);
		if (!car || !action_data.m_MainItem)
			return;

		car.Fill(CarFluid.FUEL, VB_AR_FuelCapsule.FUEL_LITERS);
		car.VB_StartRootFuelServer();
		Print("[VB_AncientRoot] " + VB_AR_ItemsServer.VB_Who(action_data.m_Player) + " залил капсулу в " + car.GetType());
		VB_AR_ItemsServer.VB_ReplaceServer(action_data.m_MainItem, "");
	}
}

modded class CarScript
{
	static const int VB_FUEL_MS = 1800000;
	static const int VB_STOP_CHECK_MS = 5000;
	static const float VB_STOP_SPEED = 3.0;			// км/ч: медленнее — «стоит»
	static const int VB_STOP_TIME_MS = 20000;
	static const int VB_CALL_COOLDOWN_MS = 180000;
	static const float VB_PLAYER_NEAR = 30.0;

	protected int m_VB_StopSinceMs = -1;
	protected int m_VB_LastCallMs = -1000000;

	void VB_StartRootFuelServer()
	{
		m_VB_AR_Fuel = true;
		SetSynchDirty();
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_EndRootFuelServer);
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_EndRootFuelServer, VB_FUEL_MS, false);
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_StopCheckServer);
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_StopCheckServer, VB_STOP_CHECK_MS, true);
	}

	void VB_EndRootFuelServer()
	{
		m_VB_AR_Fuel = false;
		SetSynchDirty();
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_StopCheckServer);
	}

	void VB_StopCheckServer()
	{
		int now = g_Game.GetTime();
		if (GetSpeedometerAbsolute() > VB_STOP_SPEED)
		{
			m_VB_StopSinceMs = -1;
			return;
		}
		if (m_VB_StopSinceMs < 0)
		{
			m_VB_StopSinceMs = now;
			return;
		}
		if (now - m_VB_StopSinceMs < VB_STOP_TIME_MS || now - m_VB_LastCallMs < VB_CALL_COOLDOWN_MS)
			return;
		if (!VB_PlayerNear())
			return;

		m_VB_LastCallMs = now;
		Print("[VB_AncientRoot] капсула: машина " + GetType() + " стоит, лезут зомби Изнанки");
		int count = Math.RandomIntInclusive(2, 3);
		VB_AR_Horde.VB_Spawn(GetPosition(), count, 30, 50, false);	// без лута (№26)
	}

	protected bool VB_PlayerNear()
	{
		array<Man> players = new array<Man>;
		g_Game.GetPlayers(players);
		float nearSq = VB_PLAYER_NEAR * VB_PLAYER_NEAR;
		vector carPos = GetPosition();
		foreach (Man m : players)
		{
			if (m && vector.DistanceSq(m.GetPosition(), carPos) < nearSq)
				return true;
		}
		return false;
	}

	override void EEDelete(EntityAI parent)
	{
		if (g_Game)
		{
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_EndRootFuelServer);
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_StopCheckServer);
		}
		super.EEDelete(parent);
	}
}

// ============================================================================================
// «Голодный клинок»: кормление за убийство, хлопок на полном заряде, голод только в игре и не ниже BLOOD_FLOOR

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

	void VB_OnKillServer(PlayerBase owner)
	{
		owner.AddHealth("", "Blood", BLOOD_PER_KILL);
		AddHealth("", "", -EXTRA_WEAR);
		m_VB_Hunger = 0;
		m_VB_Charge++;
		Print("[VB_AncientRoot] " + VB_AR_ItemsServer.VB_Who(owner) + ": голодный клинок насытился, заряд " + m_VB_Charge.ToString());
		if (m_VB_Charge >= CHARGE_MAX)
		{
			m_VB_Charge = 0;
			VB_AR_GearServer.VB_SporeBurstServer(owner.GetPosition(), STUN_RADIUS, STUN_DAMAGE);
		}
		SetSynchDirty();
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
// «Живой жгут»: при кровотечении у хозяина закрывает самое сильное и исчезает

modded class VB_AR_Tourniquet
{
	override void EEInit()
	{
		super.EEInit();
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_CheckServer, CHECK_MS, true);
	}

	override void EEDelete(EntityAI parent)
	{
		if (g_Game)
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_CheckServer);
		super.EEDelete(parent);
	}

	void VB_CheckServer()
	{
		PlayerBase player = PlayerBase.Cast(GetHierarchyRootPlayer());
		if (!player || !player.IsAlive() || IsRuined())
			return;

		BleedingSourcesManagerServer bleed = player.GetBleedingManagerServer();
		if (!bleed || bleed.GetBleedingSourcesCount() <= 0)
			return;

		bleed.RemoveMostSignificantBleedingSource();
		player.AddHealth("", "Blood", -BLOOD_COST);
		Print("[VB_AncientRoot] " + VB_AR_ItemsServer.VB_Who(player) + ": живой жгут закрыл кровотечение");
		player.MessageStatus("Лыковый жгут сам стянул рану.");
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_CheckServer);
		VB_AR_ItemsServer.VB_ReplaceServer(this, "");
	}
}

// ============================================================================================
// «Нить Изнанки»: сервер знает, прошита ли вещь, и латает её до THREAD_CAP (70 %, №35)

modded class VB_RecipeRootThread
{
	override bool CanDo(ItemBase ingredients[], PlayerBase player)
	{
		if (!super.CanDo(ingredients, player))
			return false;

		Clothing cloth = Clothing.Cast(ingredients[1]);
		if (!cloth || cloth.VB_IsThreaded())
			return false;

		// точное здоровье знает сервер: уже на потолке — нить не тратим
		return cloth.GetHealth01("", "") < THREAD_CAP - 0.005;
	}

	override void Do(ItemBase ingredients[], PlayerBase player, array<ItemBase> results, float specialty_weight)
	{
		Clothing cloth = Clothing.Cast(ingredients[1]);
		if (cloth)
			cloth.VB_StartThreadServer();
	}
}

modded class Clothing
{
	static const int VB_THREAD_MINUTES = 180;
	static const float VB_THREAD_PER_MIN = 0.01;

	protected int m_VB_ThreadLeft;			// сервер: минут починки осталось

	bool VB_IsThreaded()
	{
		return m_VB_ThreadLeft > 0;
	}

	void VB_StartThreadServer()
	{
		m_VB_ThreadLeft = VB_THREAD_MINUTES;
		Print("[VB_AncientRoot] нить Изнанки пришита к " + GetType());
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_ThreadTickServer);
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_ThreadTickServer, 60000, true);
	}

	// 1 % в минуту, но не выше THREAD_CAP от максимума; дошла до потолка — нить отработала
	void VB_ThreadTickServer()
	{
		float maxHp = GetMaxHealth("", "");
		float cap = maxHp * VB_RecipeRootThread.THREAD_CAP;
		float hp = GetHealth("", "");
		if (m_VB_ThreadLeft <= 0 || IsRuined() || hp >= cap)
		{
			m_VB_ThreadLeft = 0;
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_ThreadTickServer);
			return;
		}
		m_VB_ThreadLeft--;

		float add = Math.Min(maxHp * VB_THREAD_PER_MIN, cap - hp);
		AddHealth("", "", add);
	}

	override void EEDelete(EntityAI parent)
	{
		if (g_Game)
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_ThreadTickServer);
		super.EEDelete(parent);
	}
}

// ============================================================================================
// Игрок: таймер «Крови Изнанки» (снимается и при смерти, №27), «Плащ-кокон» (обычные зомби теряют присевшего
// неподвижного ночью; дешёвые проверки первыми, метка неподвижности сбрасывается при движении, №39).

modded class PlayerBase
{
	static const int VB_BLOOD_MS = 600000;
	static const int VB_COCOON_STILL_MS = 3000;
	static const float VB_COCOON_MOVE_SQ = 0.09;	// сдвинулся больше чем на 0.3 м — отсчёт заново

	protected int m_VB_CocoonSinceMs = -1;
	protected vector m_VB_CocoonPos;
	protected ref HumanMovementState m_VB_CocoonMs;

	void VB_StartRootBloodServer()
	{
		m_VB_AR_Blood = true;
		SetSynchDirty();
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_EndRootBloodServer);
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_EndRootBloodServer, VB_BLOOD_MS, false);
	}

	void VB_EndRootBloodServer()
	{
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_EndRootBloodServer);
		if (!m_VB_AR_Blood)
			return;

		m_VB_AR_Blood = false;
		SetSynchDirty();
	}

	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		VB_EndRootBloodServer();
	}

	override bool CanBeTargetedByAI(EntityAI ai)
	{
		if (!super.CanBeTargetedByAI(ai))
			return false;

		return !VB_CocoonHides(ai);
	}

	// «Плащ-кокон»: ночь, присел, не двигается VB_COCOON_STILL_MS — обычный зомби игрока не видит
	protected bool VB_CocoonHides(EntityAI ai)
	{
		if (!ZombieBase.Cast(ai))
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
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_EndRootBloodServer);
		super.EEDelete(parent);
	}
}

// ============================================================================================
// Голодный клинок против зомби.
// Урон: ванильный FangeKnife бьёт MeleeSharpLight_3 (Health 20) / MeleeSharpHeavy_3 (32), у зомби в корпус
// (Torso, transferToGlobalCoef 1) множитель Melee 1.2 — 24 / 38.4 в общее здоровье (P:\DZ\weapons\melee\config.cpp,
// P:\DZ\characters\zombies\config.cpp). Клинок добавляет VB_AR_Blade.HIT_BONUS = 30 за удар: лёгкий 54, тяжёлый 68.4.
// Обычный зомби (100): 2 удара любых. Зомби Изнанки (400 с 2026-11-14, Баги №41): 8 лёгких или 6 тяжёлых
// (без бонуса 17/11).
// Добавка идёт ДО ванильной обработки: если она добила, ванильный EEHitBy уже видит мёртвого и ставит анимацию
// смерти от этого удара. Добивание добавкой приходит в EEKilled без убийцы (E58) — поэтому последний удар клинком
// запоминаем, и кормление клинка (VB_OnKillServer) не теряется.

modded class ZombieBase
{
	static const int VB_BLADE_KILL_MS = 2000;		// смерть без убийцы позже — уже не от клинка

	protected VB_AR_Blade m_VB_AR_BladeHit;		// сервер: последний удар голодным клинком
	protected PlayerBase m_VB_AR_BladeOwner;
	protected int m_VB_AR_BladeHitMs;

	// кто бил голодным клинком: source — сам нож (удар оружием) или игрок с ним в руках
	protected VB_AR_Blade VB_AR_BladeOf(EntityAI source, out PlayerBase owner)
	{
		owner = null;
		if (!source)
			return null;

		VB_AR_Blade blade = VB_AR_Blade.Cast(source);
		if (blade)
		{
			owner = PlayerBase.Cast(blade.GetHierarchyRootPlayer());
			return blade;
		}

		owner = PlayerBase.Cast(source);
		if (owner)
			blade = VB_AR_Blade.Cast(owner.GetHumanInventory().GetEntityInHands());
		return blade;
	}

	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		if (damageType == DamageType.CLOSE_COMBAT && g_Game.IsServer())
		{
			PlayerBase owner;
			VB_AR_Blade blade = VB_AR_BladeOf(source, owner);
			if (blade && owner && !blade.IsRuined())
			{
				m_VB_AR_BladeHit = blade;
				m_VB_AR_BladeOwner = owner;
				m_VB_AR_BladeHitMs = g_Game.GetTime();
				if (IsAlive())
					AddHealth("", "Health", -VB_AR_Blade.HIT_BONUS);
			}
		}

		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
	}

	// убийство голодным клинком: убийца — сам нож, игрок с ним в руках или (добила добавка) недавний удар клинком
	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);

		if (!g_Game.IsServer())
			return;

		PlayerBase owner;
		VB_AR_Blade blade = VB_AR_BladeOf(EntityAI.Cast(killer), owner);
		if (!blade && (!killer || killer == this) && m_VB_AR_BladeHit)
		{
			if (g_Game.GetTime() - m_VB_AR_BladeHitMs <= VB_BLADE_KILL_MS)
			{
				blade = m_VB_AR_BladeHit;
				owner = m_VB_AR_BladeOwner;
			}
		}

		if (blade && owner && owner.IsAlive())
			blade.VB_OnKillServer(owner);
	}
}
