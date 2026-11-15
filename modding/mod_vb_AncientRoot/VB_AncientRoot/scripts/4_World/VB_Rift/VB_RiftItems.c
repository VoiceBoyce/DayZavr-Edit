// Предметы Изнанки (клиентская часть: классы, поля, сохранение, эффекты).
// «Сердце Изнанки» (VB_RiftHeartChunk) живое: стучит (слышно в HEAR_DIST), а пока его несут, рядом с несущим шум,
// и зомби идут проверить. Живёт только до рестарта: в сохранении лежит метка сессии сервера, и если после загрузки
// она не совпала с текущей, сердце протухает в «Мёртвое сердце Изнанки» (VB_RiftHeartDead) там же, где лежало.
// «Споры Изнанки» (VB_RiftSpores) выдыхаются: через SPORES_LIFE_S после сбора исчезают (время сбора в сохранении).
// «Жила» (VB_RiftVein), «Корень» (VB_RiftRootPiece), «Мёртвое сердце» — без скриптов, материалы для крафта.
// Изделия из них — VB_RiftGear.c. Серверная часть (метки, протухание, шум, добыча) — VB_Rift_Server, VB_RiftItemsServer.c.

class VB_RiftItems
{
	// Маркер нового формата сохранения предметов Изнанки (2026-11-13). Пишется первым int после super; в старом
	// формате первым шло неотрицательное поле (метка времени, заряд), поэтому отрицательный маркер с ним не спутать.
	static const int STORE_MARK = -1447186100;

	// где предмет физически: у игрока или в контейнере — позиция корня иерархии
	static vector VB_WorldPos(EntityAI item)
	{
		EntityAI root = item.GetHierarchyRoot();
		if (root)
			return root.GetPosition();
		return item.GetPosition();
	}

	// Дней от 1970-01-01 до даты (григорианский календарь, «days from civil» Х. Хиннанта), годы >= 1970.
	static int VB_DaysFromCivil(int y, int m, int d)
	{
		if (m <= 2)
			y = y - 1;
		int era = y / 400;
		int yoe = y - era * 400;
		int mp = m + 9;
		if (m > 2)
			mp = m - 3;
		int doy = (153 * mp + 2) / 5 + d - 1;
		int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
		return era * 146097 + doe - 719468;
	}

	// Секунды от 2020-01-01 UTC (+1, чтобы 0 значил «не задано») по честному календарю.
	static int VB_ClockOf(int year, int month, int day, int hour, int minute, int second)
	{
		int dayN = VB_DaysFromCivil(year, month, day);
		int day0 = VB_DaysFromCivil(2020, 1, 1);
		return (dayN - day0) * 86400 + hour * 3600 + minute * 60 + second + 1;
	}

	// Метка времени старого формата (до 2026-11-13: каждый месяц считался за 31 день, год за 372) — в нынешнюю.
	// Нужна при чтении старых сохранений: иначе споры не выдыхались бы ~1,5 месяца, а клинок не голодал.
	static int VB_FromOldClock(int v)
	{
		if (v <= 0)
			return v;

		int t = v - 1;
		int secs = t % 86400;
		int days = t / 86400;
		int y = 2020 + days / 372;
		int r = days % 372;
		int m = r / 31 + 1;
		int d = r % 31 + 1;
		int dayN = VB_DaysFromCivil(y, m, d);
		int day0 = VB_DaysFromCivil(2020, 1, 1);
		return (dayN - day0) * 86400 + secs + 1;
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
	static const int STORE_VER = 1;

	protected int m_VB_Stamp;				// сервер: метка сессии, в которой сердце вырезано (в сохранении)

	override void EEInit()
	{
		super.EEInit();

		if (!g_Game.IsDedicatedServer())
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_BeatClient, BEAT_MS + Math.RandomInt(0, 600), true);
	}

	override void EEDelete(EntityAI parent)
	{
		if (g_Game)
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_BeatClient);
		super.EEDelete(parent);
	}

	// формат 1: STORE_MARK, версия, метка сессии
	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		int mark = VB_RiftItems.STORE_MARK;
		int ver = STORE_VER;
		ctx.Write(mark);
		ctx.Write(ver);
		ctx.Write(m_VB_Stamp);
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;

		int first;
		if (!ctx.Read(first))
			return false;

		// старый формат: единственное поле — метка сессии (старый календарь); такое сердце в любом случае
		// из прошлой сессии и протухнет
		if (first != VB_RiftItems.STORE_MARK)
		{
			m_VB_Stamp = VB_RiftItems.VB_FromOldClock(first);
			return true;
		}

		int ver;
		if (!ctx.Read(ver) || ver < 1 || ver > STORE_VER)
			return false;

		int stamp;
		if (!ctx.Read(stamp))
			return false;

		m_VB_Stamp = stamp;
		return true;
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
	static const int STORE_VER = 1;

	protected int m_VB_Born;				// сервер: когда собраны (секунды от 2020 UTC), в сохранении

	// формат 1: STORE_MARK, версия, время сбора
	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		int mark = VB_RiftItems.STORE_MARK;
		int ver = STORE_VER;
		ctx.Write(mark);
		ctx.Write(ver);
		ctx.Write(m_VB_Born);
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;

		int first;
		if (!ctx.Read(first))
			return false;

		// старый формат: единственное поле — время сбора по старому календарю
		if (first != VB_RiftItems.STORE_MARK)
		{
			m_VB_Born = VB_RiftItems.VB_FromOldClock(first);
			return true;
		}

		int ver;
		if (!ctx.Read(ver) || ver < 1 || ver > STORE_VER)
			return false;

		int born;
		if (!ctx.Read(born))
			return false;

		m_VB_Born = born;
		return true;
	}
}
