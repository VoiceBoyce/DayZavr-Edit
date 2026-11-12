// Изделия Изнанки (крафт в DRL_CraftMenu): слушающий корень, сердце-аккумулятор, споровая ловушка, фонарь-сердце,
// кровь Изнанки, топливная капсула, голодный клинок, плащ-кокон, живой жгут, нить Изнанки.
// Конфиги — config.cpp (раздел «изделия Изнанки»), модели — tools/gen_rift_veins.py, перекраски — data/retex.

class VB_RiftGear
{
	static const int RPC_LISTEN = 1447186012;	// сервер -> клиент: результат «прислушаться»
	static const int RPC_BURST = 1447186013;	// сервер -> клиенты рядом: хлопок спор в точке
	static const float BURST_FX_DIST = 200.0;
	static const string BURST_SOUND = "VB_RiftBurst_SoundSet";

	// сервер: облако спор лопается — бьёт живых зомби в радиусе (игроков не трогает), клиентам рядом — хлопок
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
		Print("[VB_Rift] хлопок спор в " + pos.ToString() + ": задето зомби " + hit.ToString());
		return hit;
	}

	// сервер: показать хлопок спор всем игрокам рядом
	static void VB_BurstFxServer(vector pos)
	{
		array<Man> players = new array<Man>;
		g_Game.GetPlayers(players);
		float maxSq = BURST_FX_DIST * BURST_FX_DIST;
		foreach (Man m : players)
		{
			if (!m || vector.DistanceSq(m.GetPosition(), pos) > maxSq)
				continue;
			ScriptRPC rpc = new ScriptRPC();
			rpc.Write(pos);
			rpc.Send(m, RPC_BURST, false, m.GetIdentity());
		}
	}

	// клиент: облако красных спор и мокрый хлопок
	static void VB_BurstFxClient(vector pos)
	{
		ParticleManager pm = ParticleManager.GetInstance();
		if (pm)
			pm.PlayInWorld(ParticleList.VB_RIFT_BURST, pos + "0 0.6 0");

		EffectSound snd = SEffectManager.PlaySound(BURST_SOUND, pos);
		if (snd)
			snd.SetAutodestroy(true);
	}

	// где игрок держит предмет: в руках у этого игрока
	static bool VB_InHandsOf(EntityAI item, PlayerBase player)
	{
		return player && player.GetHumanInventory().GetEntityInHands() == item;
	}
}

// ============================================================================================
// «Слушающий корень»: ночью «прислушаться» (1 заряд) — сервер ищет ближайшее живое сердце в RANGE, клиент слышит
// серию ударов со стороны сердца: ближе — громче и чаще, если сердце режут — бешеный стук.
// Подзарядка — у живого сердца ночью (VB_ActionFeedListenRoot): заряды до полного, сердце просыпается.

class VB_RiftListenRoot extends Inventory_Base
{
	static const float RANGE = 3000.0;

	override void SetActions()
	{
		super.SetActions();
		AddAction(VB_ActionListenRoot);
		AddAction(VB_ActionFeedListenRoot);
	}
}

// клиент: серия ударов в стороне сердца
class VB_RiftListen
{
	static const string SOUND = "VB_RiftListen_SoundSet";
	static const float SOUND_DIST = 6.0;	// м от игрока в сторону сердца
	static const int TOTAL_MS = 7000;

	protected static ref VB_RiftListen s_VB_Cur;

	protected vector m_VB_Dir;
	protected float m_VB_Volume;
	protected int m_VB_Left;

	static void VB_Play(bool found, vector heartPos, bool frantic)
	{
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		if (!player)
			return;

		if (!found)
		{
			player.MessageStatus("Корень молчит: живых сердец поблизости нет.");
			return;
		}

		if (s_VB_Cur)
			s_VB_Cur.VB_Stop();
		s_VB_Cur = new VB_RiftListen(player.GetPosition(), heartPos, frantic);
	}

	void VB_RiftListen(vector from, vector to, bool frantic)
	{
		vector d = to - from;
		d[1] = 0;
		float dist = d.Length();
		d.Normalize();
		m_VB_Dir = d;

		float k = Math.Clamp(dist / VB_RiftListenRoot.RANGE, 0.0, 1.0);
		m_VB_Volume = Math.Lerp(1.0, 0.3, k);
		int intervalMs = Math.Lerp(650, 2000, k);
		if (frantic)
		{
			m_VB_Volume = 1.0;
			intervalMs = 420;
		}
		m_VB_Left = TOTAL_MS / intervalMs;
		VB_Beat();
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_Beat, intervalMs, true);
	}

	void VB_Stop()
	{
		m_VB_Left = 0;
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_Beat);
	}

	void VB_Beat()
	{
		Man player = g_Game.GetPlayer();
		if (m_VB_Left <= 0 || !player)
		{
			VB_Stop();
			return;
		}
		m_VB_Left--;

		vector pos = player.GetPosition() + m_VB_Dir * SOUND_DIST + "0 1.5 0";
		EffectSound snd = SEffectManager.CreateSound(SOUND, pos);
		if (!snd)
			return;
		snd.SetSoundVolume(m_VB_Volume);
		snd.SetAutodestroy(true);
		snd.SoundPlay();
	}
}

class VB_ActionListenRoot : ActionSingleUseBase
{
	void VB_ActionListenRoot()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_ITEM_ON;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_ITEM_ON;
		m_Text = "Прислушаться";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined();
		m_ConditionTarget = new CCTNone();
	}

	override bool HasTarget()
	{
		return false;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		return item.GetQuantity() > 0 && VB_RiftPulse.VB_NightFactor() >= VB_RiftPulse.NIGHT_LIGHT_ABOVE;
	}

	override void OnExecuteServer(ActionData action_data)
	{
		ItemBase root = action_data.m_MainItem;
		PlayerBase player = action_data.m_Player;
		if (!root || !player || root.GetQuantity() <= 0)
			return;

		root.AddQuantity(-1);
		VB_RiftHeart heart = VB_RiftHeart.VB_NearestServer(player.GetPosition(), VB_RiftListenRoot.RANGE);
		vector pos = vector.Zero;
		bool frantic = false;
		if (heart)
		{
			pos = heart.GetPosition();
			frantic = heart.VB_IsDisturbed();
		}
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(heart != null);
		rpc.Write(pos);
		rpc.Write(frantic);
		rpc.Send(player, VB_RiftGear.RPC_LISTEN, true, player.GetIdentity());
		string res = "сердец в 3 км нет";
		if (heart)
			res = "сердце в " + Math.Round(vector.Distance(player.GetPosition(), pos)).ToString() + " м";
		Print("[VB_Rift] " + VB_RiftItems.VB_Who(player) + " прислушался: " + res + ", зарядов " + root.GetQuantity().ToString());
	}
}

class VB_ActionFeedListenRootCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(VB_ActionFeedListenRoot.FEED_TIME);
	}
}

class VB_ActionFeedListenRoot : ActionContinuousBase
{
	static const float FEED_TIME = 5.0;

	void VB_ActionFeedListenRoot()
	{
		m_CallbackClass = VB_ActionFeedListenRootCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_FullBody = true;
		m_Text = "Подпитать корень";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined();
		m_ConditionTarget = new CCTCursor(UAMaxDistances.DEFAULT);
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		VB_RiftHeart heart = VB_RiftHeart.Cast(target.GetObject());
		if (!heart || heart.VB_IsDead() || item.GetQuantity() >= item.GetQuantityMax())
			return false;

		return VB_RiftPulse.VB_NightFactor() >= VB_RiftPulse.NIGHT_FULL_ABOVE;
	}

	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);

		VB_RiftHeart heart = VB_RiftHeart.Cast(action_data.m_Target.GetObject());
		if (heart)
			heart.VB_DisturbServer(action_data.m_Player, false);
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);

		VB_RiftHeart heart = VB_RiftHeart.Cast(action_data.m_Target.GetObject());
		if (heart && !heart.VB_IsDead() && action_data.m_MainItem)
			action_data.m_MainItem.SetQuantity(action_data.m_MainItem.GetQuantityMax());
	}
}

// ============================================================================================
// «Сердце-аккумулятор»: автомобильный аккумулятор. Сервер раз в минуту: ночью +CHARGE_PER_MIN, днём -DRAIN_PER_MIN.
// Клиент: игрок в машине с таким аккумулятором, ночь, живое сердце в CABIN_DIST — стук в салоне, ближе — чаще.

class VB_RiftHeartBattery extends CarBattery
{
	static const int SELF_MS = 60000;
	static const float CHARGE_PER_MIN = 10.0;	// из 500 (2 %): полный за ~50 минут ночи
	static const float DRAIN_PER_MIN = 1.25;	// 0.25 %
	static const float CABIN_DIST = 500.0;
	static const int CABIN_TICK_MS = 250;
	static const string CABIN_SOUND = "VB_RiftChunkBeat_SoundSet";

	protected float m_VB_NextBeat;

	override void EEInit()
	{
		super.EEInit();
		if (g_Game.IsServer())
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_SelfServer, SELF_MS, true);
		if (!g_Game.IsDedicatedServer())
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_CabinClient, CABIN_TICK_MS, true);
	}

	override void EEDelete(EntityAI parent)
	{
		if (g_Game)
		{
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_SelfServer);
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_CabinClient);
		}
		super.EEDelete(parent);
	}

	void VB_SelfServer()
	{
		ComponentEnergyManager em = GetCompEM();
		if (!em || IsRuined())
			return;

		float night = VB_RiftPulse.VB_NightFactor();
		if (night >= VB_RiftPulse.NIGHT_FULL_ABOVE)
			em.AddEnergy(CHARGE_PER_MIN);
		else if (night < VB_RiftPulse.NIGHT_DAY_BELOW)
			em.AddEnergy(-DRAIN_PER_MIN);
	}

	void VB_CabinClient()
	{
		CarScript car = CarScript.Cast(GetHierarchyParent());
		if (!car)
			return;

		PlayerBase me = PlayerBase.Cast(g_Game.GetPlayer());
		if (!me)
			return;

		HumanCommandVehicle hcv = me.GetCommand_Vehicle();
		if (!hcv || hcv.GetTransport() != car)
			return;

		if (VB_RiftPulse.VB_NightFactor() < VB_RiftPulse.NIGHT_LIGHT_ABOVE)
			return;

		VB_RiftBase heart = VB_RiftPulse.VB_NearestLiveHeart(car.GetPosition(), CABIN_DIST);
		if (!heart)
			return;

		float now = g_Game.GetTime() * 0.001;
		if (now < m_VB_NextBeat)
			return;

		float k = vector.Distance(car.GetPosition(), heart.GetPosition()) / CABIN_DIST;
		m_VB_NextBeat = now + Math.Lerp(0.9, 3.0, k);
		EffectSound snd = SEffectManager.PlaySoundOnObject(CABIN_SOUND, me);
		if (snd)
		{
			snd.SetSoundVolume(Math.Lerp(1.0, 0.35, k));
			snd.SetAutodestroy(true);
		}
	}
}

// ============================================================================================
// «Споровая ловушка»: «Раскрыть» в руках, потом бросить. Облако спор шумит (зомби идут), через ARM_MS лопается:
// живым зомби в BURST_RADIUS урон BURST_DAMAGE (обычных с полным здоровьем это почти валит, зомби Изнанки — нет).

class VB_RiftSporeTrap extends Inventory_Base
{
	static const int ARM_MS = 10000;
	static const int NOISE_MS = 2000;
	static const string NOISE = "VB_RiftTrapNoise";
	static const float BURST_RADIUS = 7.0;
	static const float BURST_DAMAGE = 70.0;

	protected bool m_VB_Armed;				// синхронизируется: облако спор над ловушкой
	protected ParticleSource m_VB_Cloud;

	void VB_RiftSporeTrap()
	{
		RegisterNetSyncVariableBool("m_VB_Armed");
	}

	override void SetActions()
	{
		super.SetActions();
		AddAction(VB_ActionArmSporeTrap);
	}

	bool VB_IsArmed()
	{
		return m_VB_Armed;
	}

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
		VB_RiftItems.VB_NoiseAt(VB_RiftItems.VB_WorldPos(this), NOISE, NOISE_MS * 0.001 + 1.0);
	}

	void VB_BurstServer()
	{
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_NoiseServer);
		VB_RiftGear.VB_SporeBurstServer(VB_RiftItems.VB_WorldPos(this), BURST_RADIUS, BURST_DAMAGE);
		VB_RiftItems.VB_ReplaceServer(this, "");
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		VB_UpdateCloud();
	}

	override void EEInit()
	{
		super.EEInit();
		if (!g_Game.IsDedicatedServer())
			VB_UpdateCloud();
	}

	override void EEDelete(EntityAI parent)
	{
		if (g_Game)
		{
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_NoiseServer);
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_BurstServer);
		}
		if (m_VB_Cloud)
		{
			m_VB_Cloud.StopParticle();
			m_VB_Cloud = null;
		}
		super.EEDelete(parent);
	}

	protected void VB_UpdateCloud()
	{
		if (m_VB_Armed && !m_VB_Cloud)
		{
			ParticleManager pm = ParticleManager.GetInstance();
			if (pm)
				m_VB_Cloud = pm.PlayOnObject(ParticleList.VB_RIFT_SPORES, this, "0 0.05 0", "0 0 0", true);
		}
	}
}

class VB_ActionArmSporeTrap : ActionSingleUseBase
{
	void VB_ActionArmSporeTrap()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_ITEM_ON;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_ITEM_ON;
		m_Text = "Раскрыть ловушку";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined();
		m_ConditionTarget = new CCTNone();
	}

	override bool HasTarget()
	{
		return false;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		VB_RiftSporeTrap trap = VB_RiftSporeTrap.Cast(item);
		return trap && !trap.VB_IsArmed();
	}

	override void OnExecuteServer(ActionData action_data)
	{
		VB_RiftSporeTrap trap = VB_RiftSporeTrap.Cast(action_data.m_MainItem);
		if (trap)
			trap.VB_ArmServer();
	}
}

// ============================================================================================
// «Фонарь-сердце»: горит красным без батареек (в руках или на земле), количество = минуты спор.
// Клиент: пульс света, вдвое-втрое чаще, если живой зомби ближе ALARM_DIST. Сервер: ночью раз в CALL_MS с шансом
// CALL_CHANCE из темноты выходит зомби Изнанки (VB_RiftHorde, 45–70 м) и идёт на свет.

class VB_RiftLanternLight extends PointLightBase
{
	void VB_RiftLanternLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(8.0);
		SetBrightnessTo(0.8);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetAmbientColor(1.0, 0.12, 0.15);
		SetDiffuseColor(1.0, 0.12, 0.15);
	}
}

class VB_RiftLantern extends Inventory_Base
{
	static const int MINUTE_MS = 60000;
	static const int CALL_MS = 180000;
	static const float CALL_CHANCE = 0.45;
	static const string CALL_NOISE = "VB_RiftChunkNoise";
	static const float ALARM_DIST = 30.0;
	static const int TICK_MS = 50;
	static const float BEAT_PERIOD = 1.6;

	protected bool m_VB_On;					// синхронизируется
	protected VB_RiftLanternLight m_VB_Light;
	protected float m_VB_Phase;
	protected float m_VB_Rate = 1.0;
	protected int m_VB_NearCheckMs;
	protected bool m_VB_Alarm;

	void VB_RiftLantern()
	{
		RegisterNetSyncVariableBool("m_VB_On");
	}

	override void SetActions()
	{
		super.SetActions();
		AddAction(VB_ActionLanternOn);
		AddAction(VB_ActionLanternOff);
	}

	bool VB_IsOn()
	{
		return m_VB_On;
	}

	void VB_SetOnServer(bool on)
	{
		if (on && GetQuantity() <= 0)
			return;

		m_VB_On = on;
		SetSynchDirty();
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_MinuteServer);
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_CallServer);
		if (on)
		{
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_MinuteServer, MINUTE_MS, true);
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_CallServer, CALL_MS, true);
		}
	}

	void VB_MinuteServer()
	{
		AddQuantity(-1);
		if (GetQuantity() <= 0)
			VB_SetOnServer(false);
	}

	void VB_CallServer()
	{
		if (!VB_IsShown() || VB_RiftPulse.VB_NightFactor() < VB_RiftPulse.NIGHT_FULL_ABOVE)
			return;
		if (Math.RandomFloat01() >= CALL_CHANCE)
			return;

		vector pos = VB_RiftItems.VB_WorldPos(this);
		Print("[VB_Rift] фонарь-сердце зовёт зомби Изнанки к " + pos.ToString());
		VB_RiftHorde.VB_Spawn(pos, 1, 45, 70);
		VB_RiftItems.VB_NoiseAt(pos, CALL_NOISE, 20);
	}

	// светит в руках или на земле (в рюкзаке, на поясе, в ящике — нет)
	bool VB_IsShown()
	{
		EntityAI parent = GetHierarchyParent();
		if (!parent)
			return true;

		return VB_RiftGear.VB_InHandsOf(this, PlayerBase.Cast(parent));
	}

	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		ctx.Write(m_VB_On);
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;

		if (!ctx.Read(m_VB_On))
			return false;

		return true;
	}

	override void AfterStoreLoad()
	{
		super.AfterStoreLoad();
		if (m_VB_On)
			VB_SetOnServer(true);
	}

	override void EEInit()
	{
		super.EEInit();
		if (!g_Game.IsDedicatedServer())
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_TickClient, TICK_MS, true);
	}

	override void EEDelete(EntityAI parent)
	{
		if (g_Game)
		{
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_MinuteServer);
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_CallServer);
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_TickClient);
		}
		VB_SetLight(false);
		super.EEDelete(parent);
	}

	protected void VB_SetLight(bool on)
	{
		if (on && !m_VB_Light)
		{
			m_VB_Light = VB_RiftLanternLight.Cast(ScriptedLightBase.CreateLight(VB_RiftLanternLight, GetPosition()));
			if (m_VB_Light)
				m_VB_Light.AttachOnObject(this, "0 0.12 0");
		}
		else if (!on && m_VB_Light)
		{
			m_VB_Light.Destroy();
			m_VB_Light = null;
		}
	}

	void VB_TickClient()
	{
		VB_SetLight(m_VB_On && VB_IsShown());
		if (!m_VB_Light)
			return;

		int nowMs = g_Game.GetTime();
		if (nowMs - m_VB_NearCheckMs > 1000)
		{
			m_VB_NearCheckMs = nowMs;
			m_VB_Alarm = VB_ZombieNear();
		}

		float target = 1.0;
		if (m_VB_Alarm)
			target = 2.8;
		m_VB_Rate += (target - m_VB_Rate) * 0.08;
		m_VB_Phase = Math.ModFloat(m_VB_Phase + TICK_MS * 0.001 * m_VB_Rate, BEAT_PERIOD);
		float k = Math.Clamp(VB_RiftPulse.VB_Beat(m_VB_Phase, 0.1) + 0.65 * VB_RiftPulse.VB_Beat(m_VB_Phase, 0.45), 0.0, 1.0);
		m_VB_Light.SetBrightnessTo(0.55 + 1.5 * k);
		m_VB_Light.SetRadiusTo(6.5 + 2.5 * k);
	}

	protected bool VB_ZombieNear()
	{
		array<Object> objs = new array<Object>;
		g_Game.GetObjectsAtPosition(VB_RiftItems.VB_WorldPos(this), ALARM_DIST, objs, null);
		ZombieBase zmb;
		foreach (Object o : objs)
		{
			zmb = ZombieBase.Cast(o);
			if (zmb && zmb.IsAlive())
				return true;
		}
		return false;
	}
}

class VB_ActionLanternOn : ActionSingleUseBase
{
	void VB_ActionLanternOn()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_ITEM_ON;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_ITEM_ON;
		m_Text = "Разбудить фонарь";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined();
		m_ConditionTarget = new CCTNone();
	}

	override bool HasTarget()
	{
		return false;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		VB_RiftLantern lantern = VB_RiftLantern.Cast(item);
		return lantern && !lantern.VB_IsOn() && lantern.GetQuantity() > 0;
	}

	override void OnExecuteServer(ActionData action_data)
	{
		VB_RiftLantern lantern = VB_RiftLantern.Cast(action_data.m_MainItem);
		if (lantern)
			lantern.VB_SetOnServer(true);
	}
}

class VB_ActionLanternOff : VB_ActionLanternOn
{
	void VB_ActionLanternOff()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_ITEM_OFF;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_ITEM_OFF;
		m_Text = "Усыпить фонарь";
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		VB_RiftLantern lantern = VB_RiftLantern.Cast(item);
		return lantern && lantern.VB_IsOn();
	}

	override void OnExecuteServer(ActionData action_data)
	{
		VB_RiftLantern lantern = VB_RiftLantern.Cast(action_data.m_MainItem);
		if (lantern)
			lantern.VB_SetOnServer(false);
	}
}

// ============================================================================================
// «Кровь Изнанки»: ванильный укол (себе или другому). Кровь и здоровье сразу; EFFECT_MS у игрока синхронизируемый
// флаг: руки дрожат при прицеливании (уровень тряски как от холода), у него на клиенте живые зомби в GLOW_DIST
// светятся красным, экран слабо пульсирует красным в такт сердцу.

class VB_RiftBlood extends Inventory_Base
{
	static const float BLOOD_ADD = 1500.0;
	static const float HEALTH_ADD = 40.0;

	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionInjectTarget);
		AddAction(ActionInjectSelf);
	}

	override void OnApply(PlayerBase player)
	{
		if (!player)
			return;

		player.AddHealth("", "Blood", BLOOD_ADD);
		player.AddHealth("", "Health", HEALTH_ADD);
		player.VB_StartRiftBloodServer();
		Print("[VB_Rift] " + VB_RiftItems.VB_Who(player) + ": укол Крови Изнанки");
	}
}

// клиент: эффект «Крови Изнанки» у своего игрока
class VB_RiftBloodFx
{
	static const string GLOW_MAT = "VB_Rift\\data\\zombies\\vb_rift_zmb_glow.rvmat";
	static const float GLOW_DIST = 40.0;
	static const int SCAN_MS = 1000;
	static const int BEAT_MS = 1600;
	static const string BEAT_SOUND = "VB_RiftChunkBeat_SoundSet";
	static const float FLASH = 0.05;

	protected static ref VB_RiftBloodFx s_VB_Inst;

	protected ref array<ZombieBase> m_VB_Lit;
	protected ref array<string> m_VB_LitMat;

	static void VB_Set(bool on)
	{
		if (on && !s_VB_Inst)
			s_VB_Inst = new VB_RiftBloodFx();
		else if (!on && s_VB_Inst)
		{
			s_VB_Inst.VB_Stop();
			s_VB_Inst = null;
		}
	}

	void VB_RiftBloodFx()
	{
		m_VB_Lit = new array<ZombieBase>;
		m_VB_LitMat = new array<string>;
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_Scan, SCAN_MS, true);
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_Beat, BEAT_MS, true);
		VB_Scan();
	}

	void VB_Stop()
	{
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_Scan);
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_Beat);
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_FlashOff);
		VB_FlashOff();
		for (int i = m_VB_Lit.Count() - 1; i >= 0; i--)
			VB_Restore(i);
	}

	protected void VB_Restore(int i)
	{
		ZombieBase zmb = m_VB_Lit[i];
		if (zmb)
		{
			int idx = zmb.GetHiddenSelectionIndex("camo");
			if (idx >= 0)
				zmb.SetObjectMaterial(idx, m_VB_LitMat[i]);
		}
		m_VB_Lit.Remove(i);
		m_VB_LitMat.Remove(i);
	}

	// обычные живые зомби в GLOW_DIST: подсветить новых, вернуть материал ушедшим и умершим
	void VB_Scan()
	{
		Man player = g_Game.GetPlayer();
		if (!player)
			return;

		vector ppos = player.GetPosition();
		float glowSq = GLOW_DIST * GLOW_DIST * 1.2;
		for (int i = m_VB_Lit.Count() - 1; i >= 0; i--)
		{
			if (!m_VB_Lit[i] || !m_VB_Lit[i].IsAlive() || vector.DistanceSq(ppos, m_VB_Lit[i].GetPosition()) > glowSq)
				VB_Restore(i);
		}

		array<Object> objs = new array<Object>;
		g_Game.GetObjectsAtPosition(ppos, GLOW_DIST, objs, null);
		ZombieBase zmb;
		TStringArray mats = new TStringArray;
		int idx;
		foreach (Object o : objs)
		{
			zmb = ZombieBase.Cast(o);
			if (!zmb || !zmb.IsAlive() || VB_RiftZombie.VB_Is(zmb) || m_VB_Lit.Find(zmb) >= 0)
				continue;
			idx = zmb.GetHiddenSelectionIndex("camo");
			if (idx < 0)
				continue;
			mats.Clear();
			zmb.ConfigGetTextArray("hiddenSelectionsMaterials", mats);
			if (mats.Count() <= idx)
				continue;	// исходный материал не известен — вернуть было бы нечем
			zmb.SetObjectMaterial(idx, GLOW_MAT);
			m_VB_Lit.Insert(zmb);
			m_VB_LitMat.Insert(mats[idx]);
		}
	}

	void VB_Beat()
	{
		Man player = g_Game.GetPlayer();
		if (!player)
			return;

		EffectSound snd = SEffectManager.PlaySoundOnObject(BEAT_SOUND, player);
		if (snd)
		{
			snd.SetSoundVolume(0.45);
			snd.SetAutodestroy(true);
		}

		PPERequester_HealthHitReaction req = PPERequester_HealthHitReaction.Cast(PPERequesterBank.GetRequester(PPERequester_HealthHitReaction));
		if (req)
		{
			req.Start();
			req.SetHitIntensity(FLASH);
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_FlashOff, 180, false);
		}
	}

	void VB_FlashOff()
	{
		PPERequester_HealthHitReaction req = PPERequester_HealthHitReaction.Cast(PPERequesterBank.GetRequester(PPERequester_HealthHitReaction));
		if (req)
			req.Stop();
	}
}

// ============================================================================================
// «Топливная капсула»: залить в бак (+FUEL_LITERS). Машина FUEL_MS тянет красный шлейф спор; если она стоит дольше
// STOP_TIME_MS и рядом игрок, из земли вокруг лезут 2–3 зомби Изнанки (не чаще раза в CALL_COOLDOWN_MS).

class VB_RiftFuelCapsule extends Inventory_Base
{
	static const float FUEL_LITERS = 20.0;

	override void SetActions()
	{
		super.SetActions();
		AddAction(VB_ActionPourFuelCapsule);
	}
}

class VB_ActionPourFuelCapsuleCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(VB_ActionPourFuelCapsule.POUR_TIME);
	}
}

class VB_ActionPourFuelCapsule : ActionContinuousBase
{
	static const float POUR_TIME = 6.0;

	void VB_ActionPourFuelCapsule()
	{
		m_CallbackClass = VB_ActionPourFuelCapsuleCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_EMPTY_VESSEL;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_FullBody = true;
		m_Text = "Залить капсулу в бак";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined();
		m_ConditionTarget = new CCTCursor(UAMaxDistances.DEFAULT);
	}

	static CarScript VB_Car(ActionTarget target)
	{
		CarScript car = CarScript.Cast(target.GetObject());
		if (!car)
			car = CarScript.Cast(target.GetParent());
		return car;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		CarScript car = VB_Car(target);
		return car && !car.IsRuined() && car.GetFluidFraction(CarFluid.FUEL) < 0.99;
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);

		CarScript car = VB_Car(action_data.m_Target);
		if (!car || !action_data.m_MainItem)
			return;

		car.Fill(CarFluid.FUEL, VB_RiftFuelCapsule.FUEL_LITERS);
		car.VB_StartRiftFuelServer();
		Print("[VB_Rift] " + VB_RiftItems.VB_Who(action_data.m_Player) + " залил капсулу в " + car.GetType());
		VB_RiftItems.VB_ReplaceServer(action_data.m_MainItem, "");
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

	protected bool m_VB_RiftFuel;					// синхронизируется: красный шлейф
	protected int m_VB_StopSinceMs = -1;
	protected int m_VB_LastCallMs = -1000000;
	protected ParticleSource m_VB_Trail;

	void CarScript()
	{
		RegisterNetSyncVariableBool("m_VB_RiftFuel");
	}

	void VB_StartRiftFuelServer()
	{
		m_VB_RiftFuel = true;
		SetSynchDirty();
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_EndRiftFuelServer);
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_EndRiftFuelServer, VB_FUEL_MS, false);
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_StopCheckServer);
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_StopCheckServer, VB_STOP_CHECK_MS, true);
	}

	void VB_EndRiftFuelServer()
	{
		m_VB_RiftFuel = false;
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
		Print("[VB_Rift] капсула: машина " + GetType() + " стоит, лезут зомби Изнанки");
		VB_RiftHorde.VB_Spawn(GetPosition(), Math.RandomIntInclusive(2, 3), 30, 50);
	}

	protected bool VB_PlayerNear()
	{
		array<Man> players = new array<Man>;
		g_Game.GetPlayers(players);
		float nearSq = VB_PLAYER_NEAR * VB_PLAYER_NEAR;
		foreach (Man m : players)
		{
			if (m && vector.DistanceSq(m.GetPosition(), GetPosition()) < nearSq)
				return true;
		}
		return false;
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		VB_UpdateTrail();
	}

	protected void VB_UpdateTrail()
	{
		if (m_VB_RiftFuel && !m_VB_Trail)
		{
			ParticleManager pm = ParticleManager.GetInstance();
			if (!pm)
				return;

			vector mm[2];
			vector rear = "0 0.5 -2";
			if (GetCollisionBox(mm))
				rear = Vector(0, mm[0][1] + 0.5, mm[0][2] - 0.2);
			m_VB_Trail = pm.PlayOnObject(ParticleList.VB_RIFT_SPORES, this, rear, "0 0 0", true);
		}
		else if (!m_VB_RiftFuel && m_VB_Trail)
		{
			m_VB_Trail.StopParticle();
			m_VB_Trail = null;
		}
	}

	override void EEDelete(EntityAI parent)
	{
		if (g_Game)
		{
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_EndRiftFuelServer);
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_StopCheckServer);
		}
		if (m_VB_Trail)
		{
			m_VB_Trail.StopParticle();
			m_VB_Trail = null;
		}
		super.EEDelete(parent);
	}
}

// ============================================================================================
// «Голодный клинок»: убийство зомби (этим ножом) — владельцу +BLOOD_PER_KILL крови, ножу +1 заряд и лишний износ;
// на CHARGE_MAX зарядов — хлопок спор вокруг владельца (урон зомби в STUN_RADIUS). Если клинок у игрока и не ел
// больше HUNGER_AFTER_S, раз в минуту забирает HUNGER_BLOOD крови. Заряд и время последнего убийства — в сохранении.

class VB_RiftBlade extends FangeKnife
{
	static const int CHARGE_MAX = 10;
	static const float BLOOD_PER_KILL = 150.0;
	static const float EXTRA_WEAR = 1.0;
	static const float STUN_RADIUS = 6.0;
	static const float STUN_DAMAGE = 60.0;
	static const int HUNGER_AFTER_S = 3600;
	static const int HUNGER_TICK_MS = 60000;
	static const float HUNGER_BLOOD = 60.0;
	static const int HUNGER_MSG_S = 600;

	protected int m_VB_Charge;				// синхронизируется
	protected int m_VB_LastFed;				// сервер: VB_RiftItems.VB_NowUtc последнего убийства
	protected int m_VB_LastMsg;

	void VB_RiftBlade()
	{
		RegisterNetSyncVariableInt("m_VB_Charge", 0, CHARGE_MAX);
		if (g_Game.IsServer())
			m_VB_LastFed = VB_RiftItems.VB_NowUtc();
	}

	int VB_Charge()
	{
		return m_VB_Charge;
	}

	override void EEInit()
	{
		super.EEInit();
		if (g_Game.IsServer())
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_HungerServer, HUNGER_TICK_MS, true);
	}

	override void EEDelete(EntityAI parent)
	{
		if (g_Game)
		{
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_HungerServer);
		}
		super.EEDelete(parent);
	}


	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		ctx.Write(m_VB_Charge);
		ctx.Write(m_VB_LastFed);
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;

		if (!ctx.Read(m_VB_Charge) || !ctx.Read(m_VB_LastFed))
			return false;

		return true;
	}

	void VB_OnKillServer(PlayerBase owner)
	{
		owner.AddHealth("", "Blood", BLOOD_PER_KILL);
		AddHealth("", "", -EXTRA_WEAR);
		m_VB_LastFed = VB_RiftItems.VB_NowUtc();
		m_VB_Charge++;
		Print("[VB_Rift] " + VB_RiftItems.VB_Who(owner) + ": голодный клинок насытился, заряд " + m_VB_Charge.ToString());
		if (m_VB_Charge >= CHARGE_MAX)
		{
			m_VB_Charge = 0;
			VB_RiftGear.VB_SporeBurstServer(owner.GetPosition(), STUN_RADIUS, STUN_DAMAGE);
		}
		SetSynchDirty();
	}

	void VB_HungerServer()
	{
		PlayerBase owner = PlayerBase.Cast(GetHierarchyRootPlayer());
		if (!owner || !owner.IsAlive() || IsRuined())
			return;

		int now = VB_RiftItems.VB_NowUtc();
		if (now - m_VB_LastFed < HUNGER_AFTER_S)
			return;

		owner.AddHealth("", "Blood", -HUNGER_BLOOD);
		if (now - m_VB_LastMsg >= HUNGER_MSG_S)
		{
			m_VB_LastMsg = now;
			owner.MessageStatus("Голодный клинок тянет из тебя кровь. Накорми его.");
		}
	}
}

// ============================================================================================
// «Живой жгут»: лежит у игрока где угодно; при кровотечении сам закрывает самое сильное, забирает BLOOD_COST крови
// и исчезает.

class VB_RiftTourniquet extends Inventory_Base
{
	static const int CHECK_MS = 3000;
	static const float BLOOD_COST = 150.0;

	override void EEInit()
	{
		super.EEInit();
		if (g_Game.IsServer())
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
		Print("[VB_Rift] " + VB_RiftItems.VB_Who(player) + ": живой жгут закрыл кровотечение");
		player.MessageStatus("Живой жгут сам стянул рану.");
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_CheckServer);
		VB_RiftItems.VB_ReplaceServer(this, "");
	}
}

// ============================================================================================
// «Нить Изнанки»: перетащить на одежду (рецепт VB_RecipeRiftThread). Вещь THREAD_MINUTES минут сама латается на
// 1 % в минуту. Состояние только в памяти сервера: рестарт его сбрасывает (в сохранение одежды ничего не пишем,
// чтобы мод можно было снять без порчи вещей).

class VB_RecipeRiftThread extends RecipeBase
{
	override void Init()
	{
		m_Name = "Пришить нить Изнанки";
		m_IsInstaRecipe = false;
		m_AnimationLength = 1.5;
		m_Specialty = -0.02;

		m_MinDamageIngredient[0] = -1;
		m_MaxDamageIngredient[0] = 3;
		m_MinQuantityIngredient[0] = -1;
		m_MaxQuantityIngredient[0] = -1;
		m_MinDamageIngredient[1] = -1;
		m_MaxDamageIngredient[1] = 3;
		m_MinQuantityIngredient[1] = -1;
		m_MaxQuantityIngredient[1] = -1;

		InsertIngredient(0, "VB_RiftThread");
		m_IngredientAddHealth[0] = 0;
		m_IngredientSetHealth[0] = -1;
		m_IngredientAddQuantity[0] = 0;
		m_IngredientDestroy[0] = true;

		InsertIngredient(1, "Clothing");
		m_IngredientAddHealth[1] = 0;
		m_IngredientSetHealth[1] = -1;
		m_IngredientAddQuantity[1] = 0;
		m_IngredientDestroy[1] = false;
	}

	override bool CanDo(ItemBase ingredients[], PlayerBase player)
	{
		Clothing cloth = Clothing.Cast(ingredients[1]);
		return cloth && !cloth.IsRuined() && !cloth.VB_IsThreaded();
	}

	override void Do(ItemBase ingredients[], PlayerBase player, array<ItemBase> results, float specialty_weight)
	{
		Clothing cloth = Clothing.Cast(ingredients[1]);
		if (cloth)
			cloth.VB_StartThreadServer();
	}
}

modded class PluginRecipesManagerBase
{
	override void RegisterRecipies()
	{
		super.RegisterRecipies();
		RegisterRecipe(new VB_RecipeRiftThread);
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
		if (!g_Game.IsServer())
			return;

		m_VB_ThreadLeft = VB_THREAD_MINUTES;
		Print("[VB_Rift] нить Изнанки пришита к " + GetType());
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_ThreadTickServer);
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_ThreadTickServer, 60000, true);
	}

	void VB_ThreadTickServer()
	{
		if (m_VB_ThreadLeft <= 0 || IsRuined())
		{
			m_VB_ThreadLeft = 0;
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_ThreadTickServer);
			return;
		}
		m_VB_ThreadLeft--;

		float maxHp = GetMaxHealth("", "");
		if (GetHealth("", "") < maxHp)
			AddHealth("", "", maxHp * VB_THREAD_PER_MIN);
	}

	override void EEDelete(EntityAI parent)
	{
		if (g_Game)
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_ThreadTickServer);
		super.EEDelete(parent);
	}
}

// ============================================================================================
// Игрок: «Кровь Изнанки» (флаг и тряска), «Плащ-кокон» (обычные зомби теряют присевшего неподвижного ночью),
// RPC «прислушаться» и хлопков спор.

modded class PlayerBase
{
	static const int VB_BLOOD_MS = 600000;
	static const int VB_BLOOD_SHAKE = 3;			// из SHAKE_LEVEL_MAX = 7
	static const int VB_COCOON_STILL_MS = 3000;

	protected bool m_VB_RiftBlood;					// синхронизируется: действует «Кровь Изнанки»
	protected bool m_VB_RiftBloodFx;				// клиент: эффект запущен
	protected int m_VB_StillSinceMs = -1;
	protected ref HumanMovementState m_VB_Ms;

	void PlayerBase()
	{
		RegisterNetSyncVariableBool("m_VB_RiftBlood");
	}

	void VB_StartRiftBloodServer()
	{
		m_VB_RiftBlood = true;
		SetSynchDirty();
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_EndRiftBloodServer);
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_EndRiftBloodServer, VB_BLOOD_MS, false);
	}

	void VB_EndRiftBloodServer()
	{
		m_VB_RiftBlood = false;
		SetSynchDirty();
	}

	// тряска при прицеливании считается и на сервере, и у клиента: берём синхронизируемый флаг
	override int GetShakeLevel()
	{
		int lvl = super.GetShakeLevel();
		if (m_VB_RiftBlood && lvl < VB_BLOOD_SHAKE)
			return VB_BLOOD_SHAKE;
		return lvl;
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();

		if (PlayerBase.Cast(g_Game.GetPlayer()) != this || m_VB_RiftBlood == m_VB_RiftBloodFx)
			return;

		m_VB_RiftBloodFx = m_VB_RiftBlood;
		VB_RiftBloodFx.VB_Set(m_VB_RiftBlood);
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
		if (!ZombieBase.Cast(ai) || VB_RiftZombie.VB_Is(ai))
			return false;

		EntityAI body = GetInventory().FindAttachment(InventorySlots.BODY);
		if (!body || !body.IsKindOf("VB_RiftCloak") || body.IsRuined())
			return false;

		if (VB_RiftPulse.VB_NightFactor() < VB_RiftPulse.NIGHT_FULL_ABOVE)
			return false;

		if (!m_VB_Ms)
			m_VB_Ms = new HumanMovementState();
		GetMovementState(m_VB_Ms);
		int now = g_Game.GetTime();
		if (m_VB_Ms.m_iStanceIdx != DayZPlayerConstants.STANCEIDX_CROUCH || m_VB_Ms.m_iMovement != DayZPlayerConstants.MOVEMENTIDX_IDLE)
		{
			m_VB_StillSinceMs = -1;
			return false;
		}
		if (m_VB_StillSinceMs < 0)
			m_VB_StillSinceMs = now;
		return now - m_VB_StillSinceMs >= VB_COCOON_STILL_MS;
	}

	override void EEDelete(EntityAI parent)
	{
		if (g_Game)
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_EndRiftBloodServer);
		if (m_VB_RiftBloodFx && PlayerBase.Cast(g_Game.GetPlayer()) == this)
			VB_RiftBloodFx.VB_Set(false);
		super.EEDelete(parent);
	}

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);

		if (g_Game.IsDedicatedServer())
			return;

		if (rpc_type == VB_RiftGear.RPC_LISTEN)
		{
			bool found;
			vector pos;
			bool frantic;
			if (ctx.Read(found) && ctx.Read(pos) && ctx.Read(frantic))
				VB_RiftListen.VB_Play(found, pos, frantic);
		}
		else if (rpc_type == VB_RiftGear.RPC_BURST)
		{
			vector at;
			if (ctx.Read(at))
				VB_RiftGear.VB_BurstFxClient(at);
		}
	}
}

// убийство «Голодным клинком»: убийца — сам нож или игрок с ним в руках
modded class ZombieBase
{
	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);

		if (!g_Game.IsServer())
			return;

		VB_RiftBlade blade = VB_RiftBlade.Cast(killer);
		PlayerBase owner;
		if (blade)
		{
			owner = PlayerBase.Cast(blade.GetHierarchyRootPlayer());
		}
		else
		{
			owner = PlayerBase.Cast(killer);
			if (owner)
				blade = VB_RiftBlade.Cast(owner.GetHumanInventory().GetEntityInHands());
		}
		if (blade && owner && owner.IsAlive())
			blade.VB_OnKillServer(owner);
	}
}

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(VB_ActionListenRoot);
		actions.Insert(VB_ActionFeedListenRoot);
		actions.Insert(VB_ActionArmSporeTrap);
		actions.Insert(VB_ActionLanternOn);
		actions.Insert(VB_ActionLanternOff);
		actions.Insert(VB_ActionPourFuelCapsule);
	}
}
