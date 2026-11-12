// ------------------------------------------------------------
// SFP_OpenableCase_Base
// Базовый класс "открываемого" кейса.
// ------------------------------------------------------------
// Логика:
// - кейс нельзя взять в руки/инвентарь (привязан к миру)
// - кейс "открывается" ТОЛЬКО от удара или выстрела
// - при открытии:
//   1) крышка на модели скрывается
//   2) спавнится отдельная крышка и отлетает по физике
//   3) спавнится рандомный лут на memory-точке и получает физику
//
// [НАСТРАИВАЕТСЯ] Основные настройки переопределяются в наследниках:
//   - GetLidClassName()            -> classname крышки
//   - GetLootConfigKey()           -> ключ таблицы в JSON (по умолчанию GetType())
//   - GetLidSelectionName()        -> имя selection крышки (в модели)
//   - GetSpawnItemMemoryPoint()    -> имя memory-точки спавна лута
//   - GetLidDeleteDelayMs()        -> через сколько удалять крышку (мс)
//
// [ЗАВИСИТ ОТ МОДЕЛИ]
//   SEL_CASE_LID / MEM_SPAWN_ITEM должны совпадать с именами в .p3d (selection/memory).

class SFP_OpenableCase_Base : Container_Base
{
	// ------------------------------------------------------------
	// [ЗАВИСИТ ОТ МОДЕЛИ] Имена selection/memory
	// ------------------------------------------------------------
	protected const string SEL_CASE_LID      = "case_lid";
	protected const string MEM_SPAWN_ITEM   = "spawn_item";

	// ------------------------------------------------------------
	// [НАСТРАИВАЕТСЯ] Дефолтный classname крышки (можно override в наследнике)
	// ------------------------------------------------------------
	protected const string DEFAULT_LID_CLASSNAME = "SFP_Case_Lid_Base";

	// ------------------------------------------------------------
	// [НЕ ТРОГАТЬ] Визуальный fallback: если lidSel прописан в hiddenSelections[],
	// то можно принудительно поставить прозрачную текстуру.
	// ------------------------------------------------------------
	protected const string TEX_TRANSPARENT   = "#(argb,8,8,3)color(0,0,0,0,ca)";

	// ------------------------------------------------------------
	// [НАСТРАИВАЕТСЯ] Тюнинг физики (если крышка тяжёлая — увеличивайте)
	// ------------------------------------------------------------
	protected const float  LID_THROW_MIN     = 12.0;
	protected const float  LID_THROW_MAX     = 22.0;
	protected const float  ITEM_THROW_MIN    = 1.5;
	protected const float  ITEM_THROW_MAX    = 4.0;
	protected const float  LID_SPAWN_UP      = 0.12; // чтобы крышка не упиралась сразу в кейс
	protected const float  LID_SPAWN_FWD     = 0.08;
	protected const float  ITEM_SPAWN_RADIUS = 0.06;

	// ------------------------------------------------------------
	// [НЕ ТРОГАТЬ] Состояние/кеш
	// ------------------------------------------------------------
	protected bool m_IsOpened;
	protected int m_LidHiddenSelIndex = -2; // -2 = неизвестно, -1 = не найдено, >=0 = валидно
	protected string m_LidOrigTexture = string.Empty;
	protected bool m_LidOrigTextureSaved;

	void SFP_OpenableCase_Base()
	{
		// [НЕ ТРОГАТЬ] Репликация состояния открытия
		RegisterNetSyncVariableBool("m_IsOpened");
	}

	// ------------------------------------------------------------
	// [НАСТРАИВАЕТСЯ] Переопределения под конкретный кейс
	// ------------------------------------------------------------
	protected string GetLidClassName()
	{
		return DEFAULT_LID_CLASSNAME;
	}

	// [НАСТРАИВАЕТСЯ] По умолчанию ключ = classname кейса.
	// Тогда каждый кейс может иметь свою таблицу в JSON.
	protected string GetLootConfigKey()
	{
		return GetType();
	}

	protected string GetLidSelectionName()
	{
		return SEL_CASE_LID;
	}

	protected string GetSpawnItemMemoryPoint()
	{
		return MEM_SPAWN_ITEM;
	}

	// [НАСТРАИВАЕТСЯ] Удаление крышки через N мс после отлёта. 0 = не удалять.
	protected int GetLidDeleteDelayMs()
	{
		return 20000; // 20 секунд
	}

	override void EEInit()
	{
		super.EEInit();

		// [НЕ ТРОГАТЬ] Кешируем hiddenSelection для fallback'а
		CacheLidHiddenSelection();

		// [НАСТРАИВАЕТСЯ] Авто-регистрация кейса в JSON.
		// Делается ТОЛЬКО для кейсов, которые наследуются от SFP_OpenableCase_Base,
		// потому что вызов находится здесь.
		if (GetGame() && (GetGame().IsServer() || !GetGame().IsMultiplayer()))
		{
			SFP_CaseLootConfigHandler.EnsureCaseRegistered(GetLootConfigKey());
		}

		UpdateVisualState();
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		UpdateVisualState();
	}

	// ------------------------------------------------------------
	// [НЕ ТРОГАТЬ] Визуализация
	// ------------------------------------------------------------
	protected void UpdateVisualState()
	{
		ApplyLidHidden(m_IsOpened);
	}

	protected void CacheLidHiddenSelection()
	{
		if (m_LidHiddenSelIndex != -2) // уже кешировано
			return;

		m_LidHiddenSelIndex = GetHiddenSelectionIndex(GetLidSelectionName());
		if (m_LidHiddenSelIndex >= 0)
		{
			m_LidOrigTexture = GetObjectTexture(m_LidHiddenSelIndex);
			m_LidOrigTextureSaved = true;
		}
	}

	protected void ApplyLidHidden(bool hide)
	{
		string lidSel = GetLidSelectionName();

		// 1) Основной путь: hide-анимация + HideSelection
		// Работает, если model.cfg настроен корректно.
		if (hide)
		{
			SetAnimationPhaseNow(lidSel, 1.0);
			HideSelection(lidSel);
		}
		else
		{
			SetAnimationPhaseNow(lidSel, 0.0);
			ShowSelection(lidSel);
		}

		// 2) Максимально надёжный fallback:
		// если lidSel указан в hiddenSelections[] в config.cpp,
		// принудительно делаем его прозрачным.
		CacheLidHiddenSelection();
		if (m_LidHiddenSelIndex >= 0)
		{
			if (hide)
			{
				SetObjectTexture(m_LidHiddenSelIndex, TEX_TRANSPARENT);
			}
			else
			{
				if (m_LidOrigTextureSaved)
					SetObjectTexture(m_LidHiddenSelIndex, m_LidOrigTexture);
			}
		}
	}

	// ------------------------------------------------------------
	// [НЕ ТРОГАТЬ] Привязка к миру (нельзя взять)
	// ------------------------------------------------------------
	override bool CanPutInCargo(EntityAI parent) { return false; }
	override bool CanPutIntoHands(EntityAI parent) { return false; }
	override bool IsTakeable() { return false; }

	// [НАСТРАИВАЕТСЯ] Если хотите использовать кейс как контейнер — меняйте эти методы.
	// Сейчас они запрещают всё, чтобы это был именно "кейс-объект".
	override bool CanReceiveItemIntoCargo(EntityAI item) { return false; }
	override bool CanLoadItemIntoCargo(EntityAI item) { return false; }
	override bool CanReceiveAttachment(EntityAI attachment, int slotId) { return false; }
	override bool CanLoadAttachment(EntityAI attachment) { return false; }

	// ------------------------------------------------------------
	// Открытие от попадания (удар/выстрел)
	// ------------------------------------------------------------
	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);

		// Сервер-only
		if (!GetGame().IsServer() && GetGame().IsMultiplayer())
			return;

		if (m_IsOpened)
			return;

		if (damageType != DamageType.CLOSE_COMBAT && damageType != DamageType.FIRE_ARM)
			return;

		OpenCase();
	}

	protected void OpenCase()
	{
		m_IsOpened = true;
		SetSynchDirty();
		UpdateVisualState();

		EntityAI lid = SpawnAndThrowLid();
		SpawnAndThrowRandomItems();

		int delMs = GetLidDeleteDelayMs();
		if (delMs > 0 && lid)
			ScheduleDelete(lid, delMs);

		// [НАСТРАИВАЕТСЯ] Чтобы кейс не "руинился" после открытия
		SetAllowDamage(false);
	}

	// ------------------------------------------------------------
	// Спавн/отлёт крышки
	// ------------------------------------------------------------
	protected EntityAI SpawnAndThrowLid()
	{
		string lidSel = GetLidSelectionName();
		vector localPos = GetSelectionPositionMS(lidSel);
		// Если selection не найден — некоторые модели возвращают vector.Zero
		if (localPos == vector.Zero)
			localPos = Vector(0, 0.25, 0);

		vector dir = GetRandomThrowDirection(1.0, 2.2);
		vector worldPos = ModelToWorld(localPos);
		worldPos = worldPos + (dir * LID_SPAWN_FWD);
		worldPos[1] = worldPos[1] + LID_SPAWN_UP;

		EntityAI lidEnt = EntityAI.Cast(GetGame().CreateObjectEx(GetLidClassName(), worldPos, ECE_CREATEPHYSICS | ECE_UPDATEPATHGRAPH));
		if (!lidEnt)
			return null;

		lidEnt.SetOrientation(Vector(Math.RandomFloatInclusive(0, 360), Math.RandomFloatInclusive(0, 360), Math.RandomFloatInclusive(0, 360)));

		vector force = dir * Math.RandomFloatInclusive(LID_THROW_MIN, LID_THROW_MAX);

		ItemBase lidIB;
		if (CastTo(lidIB, lidEnt))
		{
			lidIB.ThrowPhysically(null, force, false);
		}
		else
		{
			dBodyApplyImpulse(lidEnt, force);
		}

		// Немного вращения для реалистичности
		dBodyApplyTorqueImpulse(lidEnt, GetRandomTorqueImpulse(4.0, 10.0));

		return lidEnt;
	}

	// ------------------------------------------------------------
	// Спавн рандомного лута (с физикой)
	// ------------------------------------------------------------
	protected void SpawnAndThrowRandomItems()
	{
		// [НАСТРАИВАЕТСЯ] Авто-регистрация на случай, если кейс появился впервые
		// и его ещё нет в JSON.
		SFP_CaseLootConfigHandler.EnsureCaseRegistered(GetLootConfigKey());

		SFP_CaseLootEntry entry;
		int count;
		if (!SFP_CaseLootConfigHandler.GetRandomLootEntryForCase(GetLootConfigKey(), entry, count))
			return;
		if (!entry)
			return;

		string typeName = entry.ClassName;
		if (typeName == string.Empty)
			return;
		if (count <= 0)
			count = 1;

		string memPoint = GetSpawnItemMemoryPoint();
		vector localPos = GetSelectionPositionMS(memPoint);
		if (localPos == vector.Zero)
			localPos = Vector(0, 0.25, 0);

		vector basePos = ModelToWorld(localPos);
		basePos[1] = basePos[1] + 0.05;

		int i;
		for (i = 0; i < count; i++)
		{
			vector offset = Vector(Math.RandomFloatInclusive(-ITEM_SPAWN_RADIUS, ITEM_SPAWN_RADIUS), 0.0, Math.RandomFloatInclusive(-ITEM_SPAWN_RADIUS, ITEM_SPAWN_RADIUS));
			vector worldPos = basePos + offset;
			worldPos[1] = worldPos[1] + (0.01 * i);

			EntityAI spawned = EntityAI.Cast(GetGame().CreateObjectEx(typeName, worldPos, ECE_CREATEPHYSICS | ECE_UPDATEPATHGRAPH));
			if (!spawned)
				continue;

			spawned.SetOrientation(Vector(0, Math.RandomFloatInclusive(0, 360), 0));

			// [НАСТРАИВАЕТСЯ] Рандом состояния предмета (качество) из JSON: MinCondition/MaxCondition.
			// 0/0 (или поля отсутствуют) => состояние не меняем.
			ApplyRandomCondition(spawned, entry.MinCondition, entry.MaxCondition);

			// [НАСТРАИВАЕТСЯ] Если заспавнилось оружие, можно выдать ему рандомные модули (прицел/глушитель/фонарь и т.п.).
			ApplyWeaponAttachments(spawned, entry);

			vector force = GetRandomThrowImpulse(ITEM_THROW_MIN, ITEM_THROW_MAX, 0.6, 1.2);

			ItemBase spawnedIB;
			if (CastTo(spawnedIB, spawned))
			{
				spawnedIB.ThrowPhysically(null, force, false);
			}
			else
			{
				dBodyApplyImpulse(spawned, force);
			}
		}
	}

	// ------------------------------------------------------------
	// Модули оружия (attachments): ручной список или авто-режим
	// ------------------------------------------------------------
	//
	// Приоритет:
	// 1) Если в JSON у записи лута заполнено поле Attachments -> используется ручной режим (веса/варианты).
	// 2) Если Attachments пустой и включён авто-режим -> модули подбираются автоматически по слотам оружия.
	//
	// [НАСТРАИВАЕТСЯ] Авто-режим включается в JSON через раздел AutoWeaponAttachments
	// (и/или через поля записи: AutoAttachments/AutoAttachmentsFillChance/AutoAttachmentsMax).
	protected void ApplyWeaponAttachments(EntityAI spawned, SFP_CaseLootEntry entry)
	{
		if (!spawned) return;
		if (!entry) return;
		if (!spawned.IsWeapon()) return;

		// 1) Ручной режим (явно заданный список Attachments)
		if (entry.Attachments && entry.Attachments.Count() > 0)
		{
			ApplyManualWeaponAttachments(spawned, entry);
			// [НЕ ТРОГАТЬ] После программного добавления магазина у оружия
			// визуальный прокси-магазин на земле иногда не обновляется у клиентов,
			// пока оружие не попадёт в руки/инвентарь.
			// Решение — синхронизировать состояние Weapon FSM.
			ForceWeaponFSMResync(spawned);
			return;
		}

		// 2) Авто-режим (без ручного списка)
		if (!IsAutoAttachmentsEnabledForEntry(entry))
			return;

		float fillChance = ResolveAutoFillChance(entry);
		int maxAtt = ResolveAutoMaxAttachments(entry);
		ApplyAutoWeaponAttachments(spawned, entry, fillChance, maxAtt);
		// [НЕ ТРОГАТЬ] То же самое для авто-режима.
		ForceWeaponFSMResync(spawned);
	}

	// ------------------------------------------------------------
	// [НЕ ТРОГАТЬ] Принудительная синхронизация FSM оружия
	// ------------------------------------------------------------
	// Нужна для корректного отображения прокси-магазина/селекций у оружия,
	// которое было заспавнено на земле с уже вставленным магазином.
	protected void ForceWeaponFSMResync(EntityAI weapon)
	{
		// Синхронизацию имеет смысл делать на сервере.
		if (!GetGame() || !GetGame().IsServer())
			return;

		Weapon_Base wpn;
		if (CastTo(wpn, weapon))
		{
			// Обновляет selection state (включая наличие магазина) и реплицирует клиентам.
			wpn.RandomizeFSMState();
			wpn.Synchronize();
			// Повторяем через небольшой тик, чтобы гарантированно обновилось отображение на клиентах
			// (иногда при спавне на земле репликация прокси-магазина запаздывает).
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(ForceWeaponFSMResyncDelayed, 50, false, weapon);
		}
	}

	// [НЕ ТРОГАТЬ] Второй проход синхронизации (без рекурсивного планирования)
	protected void ForceWeaponFSMResyncDelayed(EntityAI weapon)
	{
		if (!GetGame() || !GetGame().IsServer())
			return;

		Weapon_Base wpn;
		if (CastTo(wpn, weapon))
		{
			// Второй проход: достаточно принудительной синхронизации.
			wpn.Synchronize();
		}
	}

	// [НЕ ТРОГАТЬ] Определение включён ли авто-режим (tri-state + глобальная настройка)
	protected bool IsAutoAttachmentsEnabledForEntry(SFP_CaseLootEntry entry)
	{
		if (!entry) return false;
		if (entry.AutoAttachments == 1) return true;
		if (entry.AutoAttachments == 0) return false;
		SFP_AutoWeaponAttachmentSettings s = SFP_CaseLootConfigHandler.GetAutoWeaponAttachmentSettings();
		if (!s) return false;
		return s.Enabled;
	}

	// [НЕ ТРОГАТЬ] Шанс заполнения слотов (override -> глобальный)
	protected float ResolveAutoFillChance(SFP_CaseLootEntry entry)
	{
		float fc = -1.0;
		if (entry) fc = entry.AutoAttachmentsFillChance;
		if (fc >= 0.0)
		{
			if (fc > 1.0) fc = 1.0;
			return fc;
		}
		SFP_AutoWeaponAttachmentSettings s = SFP_CaseLootConfigHandler.GetAutoWeaponAttachmentSettings();
		if (!s) return 0.0;
		return s.FillChance;
	}

	// [НЕ ТРОГАТЬ] Лимит модулей (override -> глобальный). 0 = без лимита
	protected int ResolveAutoMaxAttachments(SFP_CaseLootEntry entry)
	{
		if (entry && entry.AutoAttachmentsMax != -1)
			return entry.AutoAttachmentsMax;
		SFP_AutoWeaponAttachmentSettings s = SFP_CaseLootConfigHandler.GetAutoWeaponAttachmentSettings();
		if (!s) return 0;
		return s.MaxAttachments;
	}

	// ------------------------------------------------------------
	// Авто-модули оружия (без ручного списка)
	// ------------------------------------------------------------
	//
	// [НЕ ТРОГАТЬ] Выбирает слоты из конфига оружия и подбирает случайные совместимые модули.
	// Источник модулей: скан CfgVehicles на предмет inventorySlot == slot (кешируется).
	//
	// ВАЖНО ПРО МАГАЗИНЫ:
	// - магазины обычно задаются в конфиге оружия в массиве magazines[] (CfgWeapons),
	//   и находятся в CfgMagazines, а не в CfgVehicles.
	// - поэтому для слотов magazine/magazine2/magazine3 используется отдельная логика
	//   выбора из списка совместимых магазинов оружия.
	protected void ApplyAutoWeaponAttachments(EntityAI weapon, SFP_CaseLootEntry entry, float fillChance, int maxAttachments)
	{
		if (!weapon) return;
		if (!entry) return;
		if (!weapon.IsWeapon()) return;

		SFP_AutoWeaponAttachmentSettings s = SFP_CaseLootConfigHandler.GetAutoWeaponAttachmentSettings();
		if (!s) return;

		array<string> slots = new array<string>();
		GetWeaponAttachmentSlots(weapon.GetType(), slots);
		if (!slots || slots.Count() == 0)
			return;

		// [НЕ ТРОГАТЬ] Магазины обрабатываем в приоритете.
		// Иначе при малом MaxAttachments оружие может получить несколько модулей
		// и остаться без магазина.
		array<string> orderedSlots = new array<string>();
		int oi;
		for (oi = 0; oi < slots.Count(); oi++)
		{
			string sn = slots.Get(oi);
			if (sn == "magazine" || sn == "magazine2" || sn == "magazine3")
				orderedSlots.Insert(sn);
		}
		for (oi = 0; oi < slots.Count(); oi++)
		{
			string sn2 = slots.Get(oi);
			if (sn2 == "magazine" || sn2 == "magazine2" || sn2 == "magazine3")
				continue;
			orderedSlots.Insert(sn2);
		}

		int added = 0;
		int i;
		for (i = 0; i < orderedSlots.Count(); i++)
		{
			if (maxAttachments > 0 && added >= maxAttachments)
				break;

			string slotName = orderedSlots.Get(i);
			if (slotName == string.Empty)
				continue;

			if (IsSlotExcluded(slotName, s))
				continue;

			// Шанс заполнить слот
			if (fillChance < 1.0)
			{
				float r = Math.RandomFloatInclusive(0.0, 1.0);
				if (r > fillChance)
					continue;
			}

			// Если слот уже занят — пропускаем
			if (weapon.FindAttachmentBySlotName(slotName))
				continue;

			// [НЕ ТРОГАТЬ] Магазины спавним из списка совместимых magazines[] у оружия.
			if (slotName == "magazine" || slotName == "magazine2" || slotName == "magazine3")
			{
				EntityAI mag = CreateRandomCompatibleMagazine(weapon, slotName, s);
				if (mag)
				{
					ApplyRandomCondition(mag, entry.MinCondition, entry.MaxCondition);
					added++;
				}
				continue;
			}

			array<string> pool = SFP_CaseLootConfigHandler.GetAttachmentPoolForSlot(slotName);
			if (!pool || pool.Count() == 0)
				continue;

			EntityAI created = null;
			int tries;
			for (tries = 0; tries < 10 && !created; tries++)
			{
				string candidate = pool.Get(Math.RandomInt(0, pool.Count()));
				if (candidate == string.Empty)
					continue;
				if (IsClassBlacklisted(candidate, s))
					continue;

				int slotId = InventorySlots.GetSlotIdFromString(slotName);
				if (slotId != InventorySlots.INVALID)
					created = weapon.GetInventory().CreateAttachmentEx(candidate, slotId);
				else
					created = weapon.GetInventory().CreateAttachment(candidate);
			}

			if (created)
			{
				// Состояние модулей наследуем из MinCondition/MaxCondition базового предмета
				ApplyRandomCondition(created, entry.MinCondition, entry.MaxCondition);
				added++;
			}
		}
	}

	// [НЕ ТРОГАТЬ] Создаём случайный совместимый магазин по данным конфига оружия (CfgWeapons <weapon> magazines[])
	protected EntityAI CreateRandomCompatibleMagazine(EntityAI weapon, string slotName, SFP_AutoWeaponAttachmentSettings s)
	{
		if (!weapon) return null;
		if (!GetGame()) return null;
		string weaponType = weapon.GetType();
		if (weaponType == string.Empty) return null;
		if (slotName == string.Empty) return null;

		string prop = "magazines";
		if (slotName == "magazine2") prop = "magazines2";
		if (slotName == "magazine3") prop = "magazines3";

		array<string> mags = new array<string>();
		string p = "CfgWeapons " + weaponType + " " + prop;
		if (!GetGame().ConfigIsExisting(p))
			return null;
		GetGame().ConfigGetTextArray(p, mags);
		if (!mags || mags.Count() == 0)
			return null;

		EntityAI created = null;
		int tries;
		for (tries = 0; tries < 12 && !created; tries++)
		{
			string candidate = mags.Get(Math.RandomInt(0, mags.Count()));
			if (candidate == string.Empty)
				continue;
			if (IsClassBlacklisted(candidate, s))
				continue;

			int slotId = InventorySlots.GetSlotIdFromString(slotName);
			if (slotId != InventorySlots.INVALID)
				created = weapon.GetInventory().CreateAttachmentEx(candidate, slotId);
			else
				created = weapon.GetInventory().CreateAttachment(candidate);
		}
		// [НЕ ТРОГАТЬ] Сразу же ресинхронизируем FSM, чтобы магазин был виден на земле.
		if (created)
			ForceWeaponFSMResync(weapon);

		return created;
	}

	// [НЕ ТРОГАТЬ] Читаем список слотов оружия из конфига (attachments[])
	protected void GetWeaponAttachmentSlots(string weaponType, notnull array<string> outSlots)
	{
		outSlots.Clear();
		if (!GetGame()) return;
		if (weaponType == string.Empty) return;

		string p1 = "CfgWeapons " + weaponType + " attachments";
		if (GetGame().ConfigIsExisting(p1))
		{
			GetGame().ConfigGetTextArray(p1, outSlots);
		}

		if (outSlots.Count() == 0)
		{
			string p2 = "CfgVehicles " + weaponType + " attachments";
			if (GetGame().ConfigIsExisting(p2))
				GetGame().ConfigGetTextArray(p2, outSlots);
		}

		// [НЕ ТРОГАТЬ] Добавляем слоты магазинов, даже если их нет в attachments[].
		// Совместимость магазинов берём из массива magazines[] у оружия.
		array<string> tmp = new array<string>();
		string m1 = "CfgWeapons " + weaponType + " magazines";
		if (GetGame().ConfigIsExisting(m1))
		{
			GetGame().ConfigGetTextArray(m1, tmp);
			if (tmp.Count() > 0)
				AddSlotIfMissing(outSlots, "magazine");
		}
		tmp.Clear();
		string m2 = "CfgWeapons " + weaponType + " magazines2";
		if (GetGame().ConfigIsExisting(m2))
		{
			GetGame().ConfigGetTextArray(m2, tmp);
			if (tmp.Count() > 0)
				AddSlotIfMissing(outSlots, "magazine2");
		}
		tmp.Clear();
		string m3 = "CfgWeapons " + weaponType + " magazines3";
		if (GetGame().ConfigIsExisting(m3))
		{
			GetGame().ConfigGetTextArray(m3, tmp);
			if (tmp.Count() > 0)
				AddSlotIfMissing(outSlots, "magazine3");
		}
	}

	// [НЕ ТРОГАТЬ] Вставляем слот в список, если его там ещё нет
	protected void AddSlotIfMissing(notnull array<string> slots, string slotName)
	{
		int i;
		for (i = 0; i < slots.Count(); i++)
		{
			if (slots.Get(i) == slotName)
				return;
		}
		slots.Insert(slotName);
	}

	// [НЕ ТРОГАТЬ] Проверка исключённых слотов
	protected bool IsSlotExcluded(string slotName, SFP_AutoWeaponAttachmentSettings s)
	{
		if (!s) return false;
		if (!s.ExcludeSlots) return false;
		int i;
		for (i = 0; i < s.ExcludeSlots.Count(); i++)
		{
			if (s.ExcludeSlots.Get(i) == slotName)
				return true;
		}
		return false;
	}

	// [НЕ ТРОГАТЬ] Проверка чёрного списка модулей
	protected bool IsClassBlacklisted(string className, SFP_AutoWeaponAttachmentSettings s)
	{
		if (!s) return false;
		if (!s.Blacklist) return false;
		int i;
		for (i = 0; i < s.Blacklist.Count(); i++)
		{
			if (s.Blacklist.Get(i) == className)
				return true;
		}
		return false;
	}

	// ------------------------------------------------------------
	// Рандомные модули (attachments) для оружия
	// ------------------------------------------------------------
	//
	// [НАСТРАИВАЕТСЯ] Список модулей настраивается в JSON (поле Attachments у записи лута).
	//
	// Как это работает:
	// - Если заспавненный предмет НЕ оружие (IsWeapon()==false) — ничего не делаем.
	// - Для каждой "группы" в Attachments выбираем 1 опцию по весу Chance.
	// - Slot задаётся строкой из CfgSlots (пример: "weaponOptics", "pistolMuzzle").
	// - Если ClassName пустая строка — означает "ничего не ставить".
	//
	// [НЕ ТРОГАТЬ] Это серверная логика, клиента она не касается.
	protected void ApplyManualWeaponAttachments(EntityAI spawned, SFP_CaseLootEntry entry)
	{
		if (!spawned)
			return;
		if (!entry)
			return;
		if (!spawned.IsWeapon())
			return;
		if (!entry.Attachments)
			return;
		if (entry.Attachments.Count() == 0)
			return;

		int gi;
		for (gi = 0; gi < entry.Attachments.Count(); gi++)
		{
			SFP_CaseAttachmentGroup g = entry.Attachments.Get(gi);
			if (!g)
				continue;
			if (!g.Options || g.Options.Count() == 0)
				continue;

			SFP_CaseAttachmentOption chosenOpt = PickWeightedAttachmentOption(g.Options);
			if (!chosenOpt)
				continue;

			string chosen = chosenOpt.ClassName;
			if (chosen == string.Empty)
				continue; // выбран вариант "ничего"

			int slotId = InventorySlots.GetSlotIdFromString(g.Slot);
			EntityAI created;

			if (slotId != InventorySlots.INVALID)
			{
				created = spawned.GetInventory().CreateAttachmentEx(chosen, slotId);
			}
			else
			{
				// Если слот не указан/неверный — пробуем авто-подбор слота
				created = spawned.GetInventory().CreateAttachment(chosen);
			}

			// [НАСТРАИВАЕТСЯ] Рандом состояния модулей.
			// Если у модуля не задано (0/0) — наследуем диапазон от базового предмета.
			if (created)
			{
				int minC = chosenOpt.MinCondition;
				int maxC = chosenOpt.MaxCondition;
				if (minC == 0 && maxC == 0)
				{
					minC = entry.MinCondition;
					maxC = entry.MaxCondition;
				}
				ApplyRandomCondition(created, minC, maxC);
			}

			// Если модуль несовместим — движок вернёт null, просто игнорируем
		}
	}

	// [НЕ ТРОГАТЬ] Выбор по весам (Chance)
	protected SFP_CaseAttachmentOption PickWeightedAttachmentOption(array<ref SFP_CaseAttachmentOption> options)
	{
		float total = 0;
		int i;
		for (i = 0; i < options.Count(); i++)
		{
			SFP_CaseAttachmentOption o = options.Get(i);
			if (!o) continue;
			if (o.Chance <= 0) continue;
			total += o.Chance;
		}

		if (total <= 0)
			return null;

		float roll = Math.RandomFloatInclusive(0, total);
		float acc = 0;
		for (i = 0; i < options.Count(); i++)
		{
			SFP_CaseAttachmentOption o2 = options.Get(i);
			if (!o2) continue;
			if (o2.Chance <= 0) continue;

			acc += o2.Chance;
			if (roll <= acc)
			{
				return o2; // ClassName может быть пустой строкой => "ничего"
			}
		}

		return null;
	}

	// ------------------------------------------------------------
	// Рандом состояния предмета (качество)
	// ------------------------------------------------------------
	// [НАСТРАИВАЕТСЯ] Диапазон задаётся в JSON полями MinCondition/MaxCondition.
	// - 10..100 => может выпасть как в плохом, так и в хорошем состоянии.
	// - 80..100 => выпадает в относительно хорошем состоянии.
	// - 0/0 => состояние не меняем.
	protected void ApplyRandomCondition(EntityAI obj, int minPercent, int maxPercent)
	{
		if (!obj)
			return;

		// 0/0 = "не менять" (в том числе если поля отсутствуют в JSON)
		if (minPercent == 0 && maxPercent == 0)
			return;

		int minP = minPercent;
		int maxP = maxPercent;
		if (minP < 0) minP = 0;
		if (maxP < 0) maxP = 0;
		if (minP > 100) minP = 100;
		if (maxP > 100) maxP = 100;
		if (maxP < minP) maxP = minP;

		float maxH = obj.GetMaxHealth("", "");
		if (maxH <= 0)
			return;

		int roll = Math.RandomIntInclusive(minP, maxP);
		float health = maxH * (roll / 100.0);
		obj.SetHealth("", "", health);
	}

	// ------------------------------------------------------------
	// [НЕ ТРОГАТЬ] Удаление крышки/объектов (сервер реплицирует клиентам)
	// ------------------------------------------------------------
	protected void ScheduleDelete(EntityAI obj, int delayMs)
	{
		if (!obj) return;
		if (!GetGame()) return;
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(DeleteObjectSafe, delayMs, false, obj);
	}

	protected void DeleteObjectSafe(EntityAI obj)
	{
		if (!GetGame()) return;
		if (!obj) return;
		GetGame().ObjectDelete(obj);
	}

	// ------------------------------------------------------------
	// [НЕ ТРОГАТЬ] Вспомогательная математика
	// ------------------------------------------------------------
	protected vector GetRandomThrowDirection(float yMin, float yMax)
	{
		vector dir = Vector(Math.RandomFloatInclusive(-1, 1), Math.RandomFloatInclusive(yMin, yMax), Math.RandomFloatInclusive(-1, 1));
		dir.Normalize();
		return dir;
	}

	protected vector GetRandomThrowImpulse(float minStrength, float maxStrength, float yMin = 0.5, float yMax = 1.2)
	{
		vector dir = GetRandomThrowDirection(yMin, yMax);
		float strength = Math.RandomFloatInclusive(minStrength, maxStrength);
		return dir * strength;
	}

	protected vector GetRandomTorqueImpulse(float minStrength, float maxStrength)
	{
		vector dir = Vector(Math.RandomFloatInclusive(-1, 1), Math.RandomFloatInclusive(-1, 1), Math.RandomFloatInclusive(-1, 1));
		dir.Normalize();
		return dir * Math.RandomFloatInclusive(minStrength, maxStrength);
	}

	// ------------------------------------------------------------
	// [НЕ ТРОГАТЬ] Персистентность (сохранение состояния открытия)
	// ------------------------------------------------------------
	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		ctx.Write(m_IsOpened);
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;

		if (!ctx.Read(m_IsOpened))
			return false;

		return true;
	}
}
