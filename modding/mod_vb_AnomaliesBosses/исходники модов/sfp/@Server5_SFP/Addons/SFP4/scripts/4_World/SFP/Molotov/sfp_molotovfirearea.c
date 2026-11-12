/**
 * SFP_MolotovFireArea
 *
 * Серверная зона огня (24 секунды), создаётся в точке удара предмета SFP_Molotov.
 *
 * Важно (почему сделано именно так):
 * - Мы НЕ полагаемся на список объектов внутри триггера AreaDamageTrigger, т.к. он может не всегда корректно
 *   включать транспорт и предметы в мире.
 * - Поэтому сервер периодически (по таймеру) сканирует радиус и вручную наносит урон:
 *     - живым сущностям (игроки/заражённые/животные)
 *     - предметам в мире (ItemBase на земле)
 *     - транспорту (Transport) по всем damage-зонам
 *     - всем предметам внутри инвентаря сущностей, находящихся в зоне (как вы требовали)
 *
 * Клиент:
 * - звук (SoundSet) в зоне
 * - партикл огня зоны в мире
 * - партикл на объектах/живых внутри зоны (снимается через 3 секунды после выхода)
 */


// ---------- Вспомогательный класс (клиент) ----------
class SFP_MolotovObjFxData
{
	ParticleSource m_Fx;
	float m_LastSeenTime;
	vector m_LastPos;

	void SFP_MolotovObjFxData(ParticleSource fx, float lastSeen, vector lastPos)
	{
		m_Fx = fx;
		m_LastSeenTime = lastSeen;
		m_LastPos = lastPos;
	}
}

// ---------- Зона ----------
class SFP_MolotovFireArea : House
{
	// =========================
	// НАСТРОЙКИ (крутить здесь)
	// =========================
	// КАК НАСТРАИВАТЬ:
	// - RADIUS: радиус зоны в метрах
	// - LIFETIME_MS: время жизни зоны в миллисекундах (24000 = 24с)
	// - SERVER_TICK_MS: как часто сервер наносит урон (меньше = чаще и сильнее нагрузка)
	// - DMG_COEF_*: базовые множители силы урона (в дополнение к настройкам ammoName (класс урона))
	// - DMG_MULT_PER_SEC / DMG_MULT_MAX: рост урона от времени нахождения
	// - VEHICLE_ZONE_DIV: делитель урона по зонам транспорта (больше = слабее урон по машине)
	// - CLIENT_SCAN_INTERVAL_MS: частота скана на клиенте для FX на объектах
	// - OBJ_FX_REMOVE_DELAY_SEC: через сколько секунд после выхода FX исчезает
	// - HEAT_* / ITEM_TEMP_*: сила нагрева (аналог костра, но здесь очень сильно)

	static const float RADIUS = 6.0;                 // радиус зоны (м)
	static const int   LIFETIME_MS = 22000;          // время жизни зоны (мс)

	// Частота тика урона (сервер)
	static const int   SERVER_TICK_MS = 500;          // 0.5 сек

	// Базовая сила урона (настройка в скрипте). Увеличивайте, если урон кажется слабым.
	// Это множители, которые передаются в ProcessDirectDamage (доп. масштабирование к ammoName (классу урона)).
	static const float DMG_COEF_LIVING_BASE   = 6.0;  // живые: игроки/заражённые/животные
	static const float DMG_COEF_OBJECT_BASE   = 13.0;  // предметы/объекты в мире (на земле/в сцене)
	static const float DMG_COEF_INVENTORY_BASE = 14.0; // предметы внутри инвентаря сущностей в зоне

	// Эскалация: чем дольше внутри зоны, тем выше урон
	static const float DMG_MULT_PER_SEC = 0.24;       // +12% за секунду внутри зоны
	static const float DMG_MULT_MAX     = 4.0;        // ограничение множителя

	// Транспорт: урон распределяется по всем зонам; делитель нужен, чтобы не было слишком сильного «бурста»
	static const float VEHICLE_ZONE_DIV = 6.0;        // больше = слабее урон по каждой зоне

	// Класс урона (ammoName), берётся из вашего конфига
	static const string DAMAGE_AMMO = "SFP_Melee_Molotov_FireDamaged";

	// Партиклы (должны быть зарегистрированы в ParticleList как константы)
	// (Вы писали, что они уже есть в ParticleList и работают в других скриптах.)
	static const int PARTICLE_ZONE_ID = ParticleList.SFP_MOLOTOV_PART;
	static const int PARTICLE_OBJ_ID  = ParticleList.SFP_MOLOTOV_PART_obj;

	// Клиентский скан для FX (чтобы повесить огонь на объекты внутри зоны)
	static const int   CLIENT_SCAN_INTERVAL_MS = 350; // частота скана клиента для FX (мс)
	static const float OBJ_FX_REMOVE_DELAY_SEC = 3.0; // задержка снятия FX после выхода (сек)

	// Согрев (очень сильный, как вы просили)
	static const float HEAT_TEMP_CAP = 250.0;        // кап температуры источника тепла
	static const float ITEM_TEMP_CAP = 200.0;       // кап температуры предметов рядом
	static const float ITEM_TEMP_COEF = 5.0;        // коэффициент нагрева предметов
	static const float HEAT_UPDATE_INTERVAL = 0.25; // интервал обновления нагрева (сек)

	// =========================
	// =========================
	// Состояние сервера
	// =========================
	// =========================
	protected ref map<Object, float> m_EnterTimeByObject;     // object -> enter time (tick time, seconds)
	protected ref map<EntityAI, bool> m_ExplosiveRolled;      // item -> rolled already
	protected ref array<Object> m_SeenThisTick;

	protected ref UniversalTemperatureSource m_UTSource;
	protected ref UniversalTemperatureSourceSettings m_UTSSettings;
	protected ref UniversalTemperatureSourceLambdaConstant m_UTSLambda;

	// =========================
	// =========================
	// Клиентские FX
	// =========================
	// =========================
	protected ParticleSource m_MainParticle;
	protected EffectSound m_FlameSound;
	protected ref map<Object, ref SFP_MolotovObjFxData> m_ObjFx;

	// =========================
	// =========================
	// Жизненный цикл
	// =========================
	// =========================
	override void EEInit()
	{
		super.EEInit();

		// Не показываем физический прокси-объект (он нужен только как реплицируемая точка в мире)
		SetInvisible(true);

		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
			ServerInit();

		if (!GetGame().IsDedicatedServer())
			ClientInit();
	}

	protected void ServerInit()
	{
		m_EnterTimeByObject = new map<Object, float>;
		m_ExplosiveRolled = new map<EntityAI, bool>;
		m_SeenThisTick = new array<Object>;

		// --- Сильный источник тепла ---
		m_UTSSettings = new UniversalTemperatureSourceSettings();
		m_UTSSettings.m_UpdateInterval = HEAT_UPDATE_INTERVAL;
		m_UTSSettings.m_Updateable = true;
		m_UTSSettings.m_TemperatureItemCap = ITEM_TEMP_CAP;
		m_UTSSettings.m_TemperatureItemCoef = ITEM_TEMP_COEF;
		m_UTSSettings.m_TemperatureCap = HEAT_TEMP_CAP;
		m_UTSSettings.m_RangeFull = RADIUS;
		m_UTSSettings.m_RangeMax = RADIUS;

		m_UTSLambda = new UniversalTemperatureSourceLambdaConstant();
		m_UTSource = new UniversalTemperatureSource(this, m_UTSSettings, m_UTSLambda);
		m_UTSource.SetActive(true);

		// Серверный цикл урона
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(ServerTick, SERVER_TICK_MS, true);

		// Время жизни зоны
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(DeleteSafe, LIFETIME_MS, false);
	}

	protected void ClientInit()
	{
		m_ObjFx = new map<Object, ref SFP_MolotovObjFxData>;

		StartZoneFx();
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ClientScan, CLIENT_SCAN_INTERVAL_MS, true);
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);

		// Остановка клиентских FX
		if (!GetGame().IsDedicatedServer())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(ClientScan);
			StopAllObjectFx();
			StopZoneFx();
		}

		// Остановка серверных систем (урон/тепло)
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(ServerTick);

			if (m_UTSource)
			{
				m_UTSource.SetActive(false);
				m_UTSource = null;
			}
		}
	}

	// =========================
	// =========================
	// Клиентские FX
	// FX зоны
	// =========================
	protected void StartZoneFx()
	{
		// Партикл зоны: проигрываем в мире (не привязываем), чтобы он гарантированно рисовался даже при невидимом прокси
		// (Небольшой подъём по Y, чтобы не клиповаться в землю.)
		if (ParticleList.IsValidId(PARTICLE_ZONE_ID))
		{
			vector p = GetPosition();
			p[1] = p[1] + 0.10;
			m_MainParticle = ParticleManager.GetInstance().PlayInWorld(PARTICLE_ZONE_ID, p);
		}

		// Петля звука (SoundSet)
		if (!m_FlameSound)
			PlaySoundSetLoop(m_FlameSound, "SFP_MolotovFlame_SoundSet", 1.0, 1.0);
	}

	protected void StopZoneFx()
	{
		if (m_MainParticle)
		{
			m_MainParticle.Stop();
			m_MainParticle = null;
		}

		if (m_FlameSound)
		{
			m_FlameSound.SoundStop();
			m_FlameSound = null;
		}
	}

	// =========================
	// =========================
	// Клиентские FX
	// FX на объектах внутри зоны
	// =========================
	protected void ClientScan()
	{
		if (!m_ObjFx)
			return;

		float now = GetGame().GetTickTime();

		// --- ВАЖНОЕ: пред-очистка "подвисших" FX ---
		// Иногда Particle, запущенный через PlayOnObject, может стать "сиротой" при удалении/деспавне объекта
		// (например, предмет уничтожился/удалился между тиками). Чтобы исключить такие "вечные огни",
		// мы НЕ привязываем огонь напрямую к объекту, а держим его как PlayInWorld и обновляем позицию.
		// Плюс: если объект уже в состоянии удаления — гасим FX немедленно.
		array<Object> preKeys = m_ObjFx.GetKeyArray();
		foreach (Object pk : preKeys)
		{
			SFP_MolotovObjFxData pd;
			if (!m_ObjFx.Find(pk, pd))
				continue;

			EntityAI pent = EntityAI.Cast(pk);
			bool invalid = (!pk || !pent || pent.IsSetForDeletion() || pk.IsDamageDestroyed());
			if (invalid)
			{
				if (pd && pd.m_Fx)
				{
					pd.m_Fx.SetWiggle(0, 0);
					pd.m_Fx.Stop();
				}
				m_ObjFx.Remove(pk);
			}
		}
		array<Object> objects = new array<Object>;
		GetGame().GetObjectsAtPosition(GetPosition(), RADIUS, objects, null);

		bool objParticleValid = ParticleList.IsValidId(PARTICLE_OBJ_ID);

		foreach (Object o : objects)
		{
			if (!o || o == this)
				continue;

			EntityAI ent;
			if (!Class.CastTo(ent, o))
				continue;

			// Если объект уже в очереди на удаление/уничтожен — не вешаем FX
			if (ent.IsSetForDeletion() || o.IsDamageDestroyed())
				continue;

			SFP_MolotovObjFxData data;
			if (m_ObjFx.Find(o, data))
			{
				data.m_LastSeenTime = now;
				data.m_LastPos = o.GetPosition();
				if (data.m_Fx)
					data.m_Fx.SetPosition(data.m_LastPos);
			}
			else
			{
				ParticleSource fx;
				vector pos = o.GetPosition();
				pos[1] = pos[1] + 0.05; // небольшой подъём, чтобы не клиповаться в землю
				if (objParticleValid)
					fx = ParticleManager.GetInstance().PlayInWorld(PARTICLE_OBJ_ID, pos);

				m_ObjFx.Insert(o, new SFP_MolotovObjFxData(fx, now, pos));
			}
		}

		// Снимаем партиклы с объектов, которые не были замечены внутри радиуса N секунд
		array<Object> keys = m_ObjFx.GetKeyArray();
		foreach (Object key : keys)
		{
			SFP_MolotovObjFxData d;
			if (!m_ObjFx.Find(key, d))
				continue;

			EntityAI kent = EntityAI.Cast(key);
			if (!key || !kent || kent.IsSetForDeletion() || key.IsDamageDestroyed())
			{
				if (d && d.m_Fx)
				{
					d.m_Fx.SetWiggle(0, 0);
					d.m_Fx.Stop();
				}
				m_ObjFx.Remove(key);
				continue;
			}

			if ((now - d.m_LastSeenTime) >= OBJ_FX_REMOVE_DELAY_SEC)
			{
				if (d.m_Fx)
				{
					d.m_Fx.SetWiggle(0, 0);
					d.m_Fx.Stop();
				}

				m_ObjFx.Remove(key);
			}
		}
	}

	protected void StopAllObjectFx()
	{
		if (!m_ObjFx)
			return;

		array<Object> keys = m_ObjFx.GetKeyArray();
		foreach (Object key : keys)
		{
			SFP_MolotovObjFxData d;
			if (m_ObjFx.Find(key, d) && d && d.m_Fx)
			{
				d.m_Fx.SetWiggle(0, 0);
				d.m_Fx.Stop();
			}
		}
		m_ObjFx.Clear();
	}

	// =========================
	// Серверный цикл урона (ручной радиусный скан)
	// =========================
	protected void ServerTick()
	{
		if (!m_EnterTimeByObject || !m_SeenThisTick)
			return;

		float now = GetGame().GetTickTime();
		m_SeenThisTick.Clear();

		array<Object> objects = new array<Object>;
		GetGame().GetObjectsAtPosition(GetPosition(), RADIUS, objects, null);

		foreach (Object o : objects)
		{
			if (!o || o == this)
				continue;

			EntityAI eai;
			if (!Class.CastTo(eai, o))
				continue; // skip pure Object / terrain objects

			if (m_SeenThisTick.Find(o) == -1) m_SeenThisTick.Insert(o);

			float enterTime;
			if (!m_EnterTimeByObject.Find(o, enterTime))
			{
				m_EnterTimeByObject.Insert(o, now);
				enterTime = now;

				// При входе: один раз кидаем шанс взрыва на взрывоопасные предметы в инвентаре
				RollExplosivesInInventory(eai);
			}

			float timeInside = now - enterTime;
			float mult = 1.0 + (timeInside * DMG_MULT_PER_SEC);
			mult = Math.Min(mult, DMG_MULT_MAX);

			ApplyDamageToWorldEntity(eai, mult);
			DamageInventoryRecursive(eai, mult);
		}

		// Очистка объектов, которые вышли из зоны (удаляем их FX через задержку)
		array<Object> keys = m_EnterTimeByObject.GetKeyArray();
		foreach (Object k : keys)
		{
			if (m_SeenThisTick.Find(k) == -1)
				m_EnterTimeByObject.Remove(k);
		}
	}

	protected void ApplyDamageToWorldEntity(EntityAI eai, float mult)
	{
		if (!eai)
			return;

		Man man = Man.Cast(eai);
		Transport tr = Transport.Cast(eai);

		float coef;
		int flags = 0;

		if (man)
		{
			coef = DMG_COEF_LIVING_BASE * mult;
			// Одежду/инвентарь мы дамажим явно в DamageInventoryRecursive; не нужно двойное повреждение через урон игроку
			flags = ProcessDirectDamageFlags.NO_ATTACHMENT_TRANSFER;
			// Достаточно одной hitzone (ammoName должен корректно применять Health/Blood). Используем Torso, если доступно.
			eai.ProcessDirectDamage(DamageType.CUSTOM, this, "Torso", DAMAGE_AMMO, vector.Zero, coef, flags);
			return;
		}

		if (tr)
		{
			coef = DMG_COEF_OBJECT_BASE * mult;

			TStringArray zones = new TStringArray;
			eai.GetDamageZones(zones);

			if (zones && zones.Count() > 0)
			{
				float perZone = coef / Math.Max(VEHICLE_ZONE_DIV, 1.0);

				foreach (string z : zones)
				{
					// На всякий случай пропускаем пустые имена зон
					if (z && z != string.Empty)
						eai.ProcessDirectDamage(DamageType.CUSTOM, this, z, DAMAGE_AMMO, vector.Zero, perZone);
				}
			}
			else
			{
				// Запасной вариант, если Torso не найден
				eai.ProcessDirectDamage(DamageType.CUSTOM, this, "", DAMAGE_AMMO, vector.Zero, coef);
			}
			return;
		}

		// Обычный объект/предмет в мире
		coef = DMG_COEF_OBJECT_BASE * mult;
		eai.ProcessDirectDamage(DamageType.CUSTOM, this, "", DAMAGE_AMMO, vector.Zero, coef);
	}

	protected void DamageInventoryRecursive(EntityAI root, float mult)
	{
		if (!root || !root.GetInventory())
			return;

		float coefInv = DMG_COEF_INVENTORY_BASE * mult;

		array<EntityAI> items = new array<EntityAI>;
		root.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);

		foreach (EntityAI e : items)
		{
			if (!e)
				continue;

			// Не дамажим корневой объект дважды через перечисление инвентаря
			if (e == root)
				continue;

			// Избегаем цепочек наследованных повреждений; нам нужен явный контроль
			e.ProcessDirectDamage(DamageType.CUSTOM, this, "", DAMAGE_AMMO, vector.Zero, coefInv, ProcessDirectDamageFlags.NO_TRANSFER);
		}
	}

	protected void RollExplosivesInInventory(EntityAI root)
	{
		if (!root || !root.GetInventory() || !m_ExplosiveRolled)
			return;

		array<EntityAI> items = new array<EntityAI>;
		root.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);

		foreach (EntityAI e : items)
		{
			if (!e)
				continue;

			ItemBase it = ItemBase.Cast(e);
			if (!it)
				continue;

			if (!(it.IsExplosive() || it.CanExplodeInFire()))
				continue;

			bool already;
			if (m_ExplosiveRolled.Find(e, already))
				continue;

			m_ExplosiveRolled.Insert(e, true);

			// 50% шанс
			if (Math.RandomFloat01() < 0.5)
			{
				// Обычно взрывоопасные предметы детонируют при состоянии Ruined (сломано/уничтожено)
				it.SetHealth("", "", 0.0);
			}
		}
	}
}
