// Серверная часть предметов Изнанки: метка сессии, время, шум для зомби, замена предмета, добыча к ногам,
// протухание «Сердца Изнанки», выдыхание спор. Классы, поля и формат сохранения — VB_Rift, VB_RiftItems.c.
// VB_RiftItemsServer — общий серверный помощник: его зовут и другие части мода (сердце, зомби, гнёзда).

class VB_RiftItemsServer
{
	protected static int s_VB_Session;
	protected static ref map<string, ref NoiseParams> s_VB_Noises;

	static const float DROP_LIFT = 0.3;		// м над ногами: отсюда ECE_PLACE_ON_SURFACE ищет опору вниз

	// метка текущей сессии — время первого вызова (старт сервера) по UTC с точностью до секунды
	static int VB_Session()
	{
		if (s_VB_Session == 0)
			s_VB_Session = VB_NowUtc();
		return s_VB_Session;
	}

	// секунды по UTC от 2020-01-01 (+1), честный календарь с високосными годами
	static int VB_NowUtc()
	{
		int year;
		int month;
		int day;
		int hour;
		int minute;
		int second;
		GetYearMonthDayUTC(year, month, day);
		GetHourMinuteSecondUTC(hour, minute, second);
		return VB_RiftItems.VB_ClockOf(year, month, day, hour, minute, second);
	}

	// заменить предмет другим на том же месте (в руках, рюкзаке, на земле); newType "" — просто убрать
	static void VB_ReplaceServer(EntityAI item, string newType)
	{
		if (!item || item.IsSetForDeletion())
			return;

		// После удачной ванильной замены старый объект ещё какое-то время живёт в скрипте, но уже без места
		// в инвентаре: повторная замена такого даёт «no inventory location», а создание нового — дубликат
		// (проверено на стенде 11-12, E52). Такой объект только удаляем.
		InventoryLocation loc = new InventoryLocation();
		if (!item.GetInventory().GetCurrentInventoryLocation(loc))
		{
			item.Delete();
			return;
		}

		PlayerBase player = PlayerBase.Cast(item.GetHierarchyRootPlayer());
		ReplaceItemWithNewLambdaBase lambda = new ReplaceItemWithNewLambdaBase(item, newType);
		if (player)
			player.ServerReplaceItemWithNew(lambda);
		else
			item.GetInventory().ReplaceItemWithNew(InventoryMode.SERVER, lambda);
	}

	// шум для зомби в точке (CfgNoises <noise>), держится time секунд
	static void VB_NoiseAt(vector pos, string noise, float time)
	{
		if (!s_VB_Noises)
			s_VB_Noises = new map<string, ref NoiseParams>;

		NoiseParams np;
		if (!s_VB_Noises.Find(noise, np))
		{
			np = new NoiseParams();
			np.Load(noise);
			s_VB_Noises.Insert(noise, np);
		}
		g_Game.GetNoiseSystem().AddNoiseTarget(pos, time, np);
	}

	// Добыча (сбор спор, отлом корня, разделка сердца) — на землю у ног игрока, не в инвентарь.
	// Правило пользователя для всех наших модов (2026-11-13): игрок видит добычу и сам решает, брать ли её.
	// Высота — от ног + DROP_LIFT, а не по рельефу: ECE_PLACE_ON_SURFACE опускает предмет на ближайшую опору под
	// точкой (пол дома, помост, скала), а рельеф под мостом или скалой уводил добычу под них (баг №33).
	// Игрок в воде — тоже от ног, на берег не переносим.
	static EntityAI VB_DropAtFeet(PlayerBase player, string type)
	{
		vector feet = player.GetPosition();
		float a = Math.RandomFloat(0, Math.PI2);
		float d = Math.RandomFloat(0.2, 0.6);
		float px = feet[0] + Math.Cos(a) * d;
		float py = feet[1] + DROP_LIFT;
		float pz = feet[2] + Math.Sin(a) * d;
		vector p = Vector(px, py, pz);
		EntityAI item = EntityAI.Cast(g_Game.CreateObjectEx(type, p, ECE_PLACE_ON_SURFACE));
		Print("[VB_Rift] " + VB_Who(player) + " добыл " + type + " (на землю у ног)");
		return item;
	}

	// для логов админа: ник и SteamID
	static string VB_Who(PlayerBase player)
	{
		if (!player)
			return "?";
		PlayerIdentity id = player.GetIdentity();
		if (!id)
			return "?";
		string name = id.GetName();
		string plain = id.GetPlainId();
		return name + " (" + plain + ")";
	}
}

modded class VB_RiftHeartChunk
{
	void VB_RiftHeartChunk()
	{
		m_VB_Stamp = VB_RiftItemsServer.VB_Session();
	}

	override void EEInit()
	{
		super.EEInit();
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_NoiseServer, NOISE_MS, true);
		VB_ScheduleRot(ROT_CHECK_MS);
	}

	override void EEDelete(EntityAI parent)
	{
		if (g_Game)
		{
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_NoiseServer);
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_CheckRotServer);
		}
		super.EEDelete(parent);
	}

	override void AfterStoreLoad()
	{
		super.AfterStoreLoad();
		VB_ScheduleRot(ROT_CHECK_MS);
	}

	protected void VB_ScheduleRot(int delayMs)
	{
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_CheckRotServer);
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_CheckRotServer, delayMs, false);
	}

	// сердце пролежало рестарт — заменить мёртвым на том же месте
	void VB_CheckRotServer()
	{
		if (m_VB_Stamp == VB_RiftItemsServer.VB_Session() || IsSetForDeletion())
			return;

		// уже заменено (старый объект доживает без места в инвентаре, E52) — только убрать, без лога
		InventoryLocation loc = new InventoryLocation();
		if (!GetInventory().GetCurrentInventoryLocation(loc))
		{
			Delete();
			return;
		}

		vector at = VB_RiftItems.VB_WorldPos(this);
		Print("[VB_Rift] Сердце Изнанки протухло (" + at.ToString() + ")");
		VB_RiftItemsServer.VB_ReplaceServer(this, DEAD_TYPE);
		// при удачной замене старое сердце удалится и EEDelete снимет повтор
		VB_ScheduleRot(ROT_RETRY_MS);
	}

	// несут — шум у несущего (в ящике, машине или на земле сердце молчит для зомби)
	void VB_NoiseServer()
	{
		Man carrier = GetHierarchyRootPlayer();
		if (carrier)
			VB_RiftItemsServer.VB_NoiseAt(carrier.GetPosition(), NOISE, NOISE_TIME);
	}
}

modded class VB_RiftSpores
{
	void VB_RiftSpores()
	{
		m_VB_Born = VB_RiftItemsServer.VB_NowUtc();
	}

	override void EEInit()
	{
		super.EEInit();
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_CheckServer, FIRST_CHECK_MS, false);
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
		if (VB_RiftItemsServer.VB_NowUtc() - m_VB_Born < SPORES_LIFE_S)
			return;

		VB_RiftItemsServer.VB_ReplaceServer(this, "");	// удалится — EEDelete снимет проверки; не вышло — повторим
	}
}
