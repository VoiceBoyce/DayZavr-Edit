// ------------------------------------------------------------
// SFP: Авто-удаление лута без взаимодействия
// ------------------------------------------------------------
//
// Назначение:
// - После открытия кейса лут появляется на земле.
// - Если предмет никто НЕ трогал (не поднимали / не помещали в инвентарь),
//   он удаляется через N минут.
// - Таймер настраивается в JSON (SFP_CaseLoot.json):
//     LootCleanup.UntouchedMinutes (глобально)
//   и опционально для конкретного кейса:
//     Cases[].UseCustomCleanup = true
//     Cases[].CleanupMinutes = N
//
// Реализация:
// - При спавне лута кейс вызывает RegisterUntouchedDespawn().
// - Менеджер 1 раз в секунду проверяет записи:
//   * если предмет оказался в иерархии (GetHierarchyParent()!=null) -> считаем,
//     что было взаимодействие, удаление отменяется;
//   * если наступил дедлайн и предмет всё ещё лежит на земле -> удаляем.
//
// [НЕ ТРОГАТЬ] Это серверная логика. На клиентах не работает и не нужна.

class SFP_LootCleanupRecord
{
	EntityAI ObjectRef;
	int DeadlineMs;
};

class SFP_LootCleanupManager
{
	protected static ref SFP_LootCleanupManager s_Instance;
	protected ref map<int, ref SFP_LootCleanupRecord> m_Records;
	protected bool m_TickScheduled;

	// [НАСТРАИВАЕТСЯ] Частота проверки (мс). Чем меньше — тем точнее ловим
	// краткие взаимодействия, но тем выше нагрузка.
	// Обычно 1000 мс достаточно.
	protected const int TICK_INTERVAL_MS = 1000;

	static SFP_LootCleanupManager GetInstance()
	{
		if (!s_Instance)
			s_Instance = new SFP_LootCleanupManager();
		return s_Instance;
	}

	void SFP_LootCleanupManager()
	{
		m_Records = new map<int, ref SFP_LootCleanupRecord>();
		m_TickScheduled = false;
	}

	// ------------------------------------------------------------
	// [НАСТРАИВАЕТСЯ] Зарегистрировать предмет на удаление, если его не трогали
	// minutes:
	//   0 = не удалять
	//   1..N = удалить через N минут без взаимодействия
	// ------------------------------------------------------------
	void RegisterUntouchedDespawn(EntityAI obj, int minutes)
	{
		if (!obj)
			return;

		// Сервер-only
		if (!GetGame() || !(GetGame().IsServer() || !GetGame().IsMultiplayer()))
			return;

		if (minutes <= 0)
			return;

		if (!m_Records)
			m_Records = new map<int, ref SFP_LootCleanupRecord>();

		int id = obj.GetID();
		if (m_Records.Contains(id))
			return;

		SFP_LootCleanupRecord rec = new SFP_LootCleanupRecord();
		rec.ObjectRef = obj;
		rec.DeadlineMs = GetGame().GetTime() + (minutes * 60 * 1000);

		m_Records.Insert(id, rec);
		EnsureTick();
	}

	// ------------------------------------------------------------
	// [НЕ ТРОГАТЬ] Запускаем цикл проверки только когда он нужен
	// ------------------------------------------------------------
	protected void EnsureTick()
	{
		if (m_TickScheduled)
			return;

		m_TickScheduled = true;
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TickOnce, TICK_INTERVAL_MS, false);
	}

	// ------------------------------------------------------------
	// [НЕ ТРОГАТЬ] Однократный тик. Сам себя перепланирует, если ещё есть записи.
	// ------------------------------------------------------------
	protected void TickOnce()
	{
		// Сервер-only
		if (!GetGame() || !(GetGame().IsServer() || !GetGame().IsMultiplayer()))
		{
			m_TickScheduled = false;
			return;
		}

		if (!m_Records || m_Records.Count() == 0)
		{
			m_TickScheduled = false;
			return;
		}

		int now = GetGame().GetTime();

		array<int> keys = m_Records.GetKeyArray();
		if (!keys)
			keys = new array<int>;

		int k;
		for (k = 0; k < keys.Count(); k++)
		{
			int id = keys.Get(k);
			SFP_LootCleanupRecord rec = m_Records.Get(id);
			if (!rec)
			{
				m_Records.Remove(id);
				continue;
			}

			EntityAI obj = rec.ObjectRef;

			// Если объект исчез сам (удалили/деспавн/рестарт), снимаем запись
			if (!obj)
			{
				m_Records.Remove(id);
				continue;
			}

			// Любой parent означает взаимодействие (взяли в руки/в инвентарь/прикрепили).
			if (obj.GetHierarchyParent() != null)
			{
				m_Records.Remove(id);
				continue;
			}

			// Дедлайн наступил и предмет всё ещё на земле => удаляем
			if (now >= rec.DeadlineMs)
			{
				GetGame().ObjectDelete(obj);
				m_Records.Remove(id);
				continue;
			}
		}

		// Планируем следующий тик, если ещё есть записи
		if (m_Records.Count() > 0)
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TickOnce, TICK_INTERVAL_MS, false);
		else
			m_TickScheduled = false;
	}
};
