// Предметы Изнанки.
// «Сердце Изнанки» (VB_RiftHeartChunk) живое: стучит (слышно в HEAR_DIST), а пока его несут, каждые NOISE_MS
// рядом с несущим шум, и зомби идут проверить. Живёт только до рестарта: в сохранении лежит метка сессии сервера,
// и если после загрузки она не совпала с текущей, сердце протухает в «Мёртвое сердце Изнанки» (VB_RiftHeartDead)
// там же, где лежало (в руках, рюкзаке, ящике, на земле). Перезаход игрока в ту же сессию сердце не портит.
// «Споры Изнанки» (VB_RiftSpores) выдыхаются: через SPORES_LIFE_S после сбора исчезают (время старта тоже в сохранении).
// «Жила» (VB_RiftVein), «Корень» (VB_RiftRootPiece), «Мёртвое сердце» — без скриптов, материалы для крафта.
// Изделия из них — VB_RiftGear.c.

class VB_RiftItems
{
	protected static int s_VB_Session;
	protected static ref map<string, ref NoiseParams> s_VB_Noises;

	// сервер: метка текущей сессии — время первого вызова (старт сервера) по UTC с точностью до секунды
	static int VB_Session()
	{
		if (s_VB_Session == 0)
			s_VB_Session = VB_NowUtc();
		return s_VB_Session;
	}

	// секунды по UTC от 2020 года (месяц считается за 31 день: важны только разности и порядок)
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
		int days = (year - 2020) * 372 + (month - 1) * 31 + (day - 1);
		return days * 86400 + hour * 3600 + minute * 60 + second + 1;
	}

	// сервер: заменить предмет другим на том же месте (в руках, рюкзаке, на земле); newType "" — просто убрать
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

	// сервер: шум для зомби в точке (CfgNoises <noise>), держится time секунд
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

	// где предмет физически: у игрока или в контейнере — позиция корня иерархии
	static vector VB_WorldPos(EntityAI item)
	{
		EntityAI root = item.GetHierarchyRoot();
		if (root)
			return root.GetPosition();
		return item.GetPosition();
	}

	// Сервер: добыча (сбор спор, отлом корня, разделка сердца) — на землю у ног игрока, не в инвентарь.
	// Правило пользователя для всех наших модов (2026-11-13): игрок видит добычу и сам решает, брать ли её.
	static EntityAI VB_DropAtFeet(PlayerBase player, string type)
	{
		vector p = player.GetPosition();
		float a = Math.RandomFloat(0, Math.PI2);
		float d = Math.RandomFloat(0.2, 0.6);
		p[0] = p[0] + Math.Cos(a) * d;
		p[2] = p[2] + Math.Sin(a) * d;
		p[1] = g_Game.SurfaceY(p[0], p[2]);
		EntityAI item = EntityAI.Cast(g_Game.CreateObjectEx(type, p, ECE_PLACE_ON_SURFACE));
		Print("[VB_Rift] " + VB_Who(player) + " добыл " + type + " (на землю у ног)");
		return item;
	}

	// для логов админа: ник и SteamID
	static string VB_Who(PlayerBase player)
	{
		if (!player || !player.GetIdentity())
			return "?";
		return player.GetIdentity().GetName() + " (" + player.GetIdentity().GetPlainId() + ")";
	}
}

class VB_RiftHeartChunk extends Inventory_Base
{
	static const string DEAD_TYPE = "VB_RiftHeartDead";
	static const string BEAT_SOUND = "VB_RiftChunkBeat_SoundSet";
	static const string NOISE = "VB_RiftChunkNoise";
	static const int BEAT_MS = 3200;		// стук: «тук-тук» и пауза
	static const float HEAR_DIST = 30.0;	// м, дальше стук не играем
	static const int NOISE_MS = 4000;		// как часто шумит у несущего
	static const float NOISE_TIME = 5.0;	// с, сколько держится каждый шум
	static const int ROT_CHECK_MS = 3000;	// после загрузки: подождать, пока игрок/контейнер встанут на место
	static const int ROT_RETRY_MS = 10000;	// замена не удалась (игрок ещё входит) — попробовать снова

	protected int m_VB_Stamp;				// сервер: метка сессии, в которой сердце вырезано (в сохранении)

	void VB_RiftHeartChunk()
	{
		if (g_Game.IsServer())
			m_VB_Stamp = VB_RiftItems.VB_Session();
	}

	override void EEInit()
	{
		super.EEInit();

		if (g_Game.IsServer())
		{
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_NoiseServer, NOISE_MS, true);
			VB_ScheduleRot(ROT_CHECK_MS);
		}
		if (!g_Game.IsDedicatedServer())
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_BeatClient, BEAT_MS + Math.RandomInt(0, 600), true);
	}

	override void EEDelete(EntityAI parent)
	{
		if (g_Game)
		{
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_NoiseServer);
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_BeatClient);
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_CheckRotServer);
		}
		super.EEDelete(parent);
	}

	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		ctx.Write(m_VB_Stamp);
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;

		if (!ctx.Read(m_VB_Stamp))
			return false;

		return true;
	}

	override void AfterStoreLoad()
	{
		super.AfterStoreLoad();

		if (g_Game.IsServer())
			VB_ScheduleRot(ROT_CHECK_MS);
	}

	protected void VB_ScheduleRot(int delayMs)
	{
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_CheckRotServer);
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_CheckRotServer, delayMs, false);
	}

	// сервер: сердце пролежало рестарт — заменить мёртвым на том же месте
	void VB_CheckRotServer()
	{
		if (m_VB_Stamp == VB_RiftItems.VB_Session() || IsSetForDeletion())
			return;

		// уже заменено (старый объект доживает без места в инвентаре, E52) — только убрать, без лога
		InventoryLocation loc = new InventoryLocation();
		if (!GetInventory().GetCurrentInventoryLocation(loc))
		{
			Delete();
			return;
		}

		Print("[VB_Rift] Сердце Изнанки протухло (" + VB_RiftItems.VB_WorldPos(this).ToString() + ")");
		VB_RiftItems.VB_ReplaceServer(this, DEAD_TYPE);
		// при удачной замене старое сердце удалится и EEDelete снимет повтор
		VB_ScheduleRot(ROT_RETRY_MS);
	}

	// сервер: несут — шум у несущего (в ящике, машине или на земле сердце молчит для зомби)
	void VB_NoiseServer()
	{
		Man carrier = GetHierarchyRootPlayer();
		if (carrier)
			VB_RiftItems.VB_NoiseAt(carrier.GetPosition(), NOISE, NOISE_TIME);
	}

	// клиент: стук, если игрок рядом (звук на том, кто несёт, или на самом сердце)
	void VB_BeatClient()
	{
		Man me = g_Game.GetPlayer();
		if (!me)
			return;

		Object src = GetHierarchyRoot();
		if (!src)
			src = this;
		if (vector.DistanceSq(me.GetPosition(), src.GetPosition()) > HEAR_DIST * HEAR_DIST)
			return;

		EffectSound snd = SEffectManager.PlaySoundOnObject(BEAT_SOUND, src);
		if (snd)
			snd.SetAutodestroy(true);
	}
}

// «Споры Изнанки»: выдыхаются через SPORES_LIFE_S после сбора (реальное время, рестарты не останавливают).
class VB_RiftSpores extends Inventory_Base
{
	static const int SPORES_LIFE_S = 86400;
	static const int CHECK_MS = 300000;		// раз в 5 минут
	static const int FIRST_CHECK_MS = 5000;

	protected int m_VB_Born;				// сервер: когда собраны (VB_RiftItems.VB_NowUtc), в сохранении

	void VB_RiftSpores()
	{
		if (g_Game.IsServer())
			m_VB_Born = VB_RiftItems.VB_NowUtc();
	}

	override void EEInit()
	{
		super.EEInit();
		if (g_Game.IsServer())
		{
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_CheckServer, FIRST_CHECK_MS, false);
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_CheckServer, CHECK_MS, true);
		}
	}

	override void EEDelete(EntityAI parent)
	{
		if (g_Game)
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_CheckServer);
		super.EEDelete(parent);
	}

	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		ctx.Write(m_VB_Born);
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;

		if (!ctx.Read(m_VB_Born))
			return false;

		return true;
	}

	void VB_CheckServer()
	{
		if (VB_RiftItems.VB_NowUtc() - m_VB_Born < SPORES_LIFE_S)
			return;

		VB_RiftItems.VB_ReplaceServer(this, "");	// удалится — EEDelete снимет проверки; не вышло — повторим
	}
}
