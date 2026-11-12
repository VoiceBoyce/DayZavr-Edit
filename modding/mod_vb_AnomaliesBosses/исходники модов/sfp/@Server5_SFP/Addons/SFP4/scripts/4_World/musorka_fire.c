class SFP_Static_MusorkaFire extends ItemBase
{
	// ----------------------------
	// Настройки
	// ----------------------------
	static const float BURN_UPDATE_INTERVAL_S = 1.0;   // как часто тикаем
	static const float BURN_MAXHP_PCT_PER_SEC = 0.07;  // 7% от maxHP в секунду (~14с до ruined)
	
	static const float HEAT_UPDATE_INTERVAL_S = 2.0;
	static const float HEAT_RANGE_FULL = 3.0;          // "как у костра" — ближняя зона
	static const float HEAT_RANGE_MAX  = 10.0;         // небольшой радиус
	static const float HEAT_TEMP_CAP   = 25.0;         // как в примере Bonfire
	
	static const string MEMPOINT_LIGHT = "light";      // точка света в модели
	static const string MEMPOINT_FIRE  = "fire";       // точка партикла (если нет — будет 0,0,0)
	
	// RPC (лучше держать в высоких значениях, чтобы не конфликтовать)
	static const int RPC_SFP_BURNT_ITEM_FX = 772801;

	// ----------------------------
	// Эффекты (клиент)
	// ----------------------------
	protected FireplaceLight m_Light;
	protected Particle       m_FireParticle;
	protected EffectSound    m_FireSoundLoop;

	// ----------------------------
	// Серверная логика
	// ----------------------------
	protected ref Timer m_BurnTimer;

	// Комфорт/согревание как от костра
	protected ref UniversalTemperatureSource m_UTSource;
	protected ref UniversalTemperatureSourceSettings m_UTSSettings;
	protected ref UniversalTemperatureSourceLambdaConstant m_UTSLambda;

	// ----------------------------
	// Init / Delete
	// ----------------------------
	override void EEInit()
	{
		super.EEInit();

		// Нельзя взять/перетаскивать + не толкать физикой
		SetTakeable(false);
		DisableSimulation(true);

		// Клиентские эффекты (звук/партикл/свет)
		if (!GetGame().IsDedicatedServer())
		{
			StartAmbientFireEffects();
		}

		// Серверная часть (урон предметам + согревание)
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			InitHeatSource();

			// Подпишемся на изменения карго (если у предмета есть cargo)
			GetOnItemAddedIntoCargo().Insert(OnCargoItemIn);
			GetOnItemRemovedFromCargo().Insert(OnCargoItemOut);

			UpdateBurnTimer();
		}
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);

		if (!GetGame().IsDedicatedServer())
		{
			StopAmbientFireEffects();
		}

		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			if (m_UTSource)
				m_UTSource.SetActive(false);

			if (m_BurnTimer)
				m_BurnTimer.Stop();

			// Отписка (на всякий)
			GetOnItemAddedIntoCargo().Remove(OnCargoItemIn);
			GetOnItemRemovedFromCargo().Remove(OnCargoItemOut);
		}
	}

	// ----------------------------
	// Запрет перемещения в инвентарь / руки
	// ----------------------------
	override bool IsTakeable()
	{
		return false;
	}

	override bool CanPutInCargo(EntityAI parent)
	{
		return false;
	}

	override bool CanPutIntoHands(EntityAI parent)
	{
		return false;
	}

	override bool CanBeMovedOverride()
	{
		// доп. страховка на странные конфликтные перемещения
		return false;
	}

	override bool DisableVicinityIcon()
	{
		// можно убрать иконку из vicinity, чтобы даже не пытались
		return true;
	}

	// ----------------------------
	// Хуки на вставку/вынимание предметов
	// ----------------------------
	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);

		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
			UpdateBurnTimer();
	}

	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);

		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
			UpdateBurnTimer();
	}

	void OnCargoItemIn(EntityAI item, EntityAI parent)
	{
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
			UpdateBurnTimer();
	}

	void OnCargoItemOut(EntityAI item, EntityAI parent)
	{
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
			UpdateBurnTimer();
	}

	// ----------------------------
	// Урон предметам внутри
	// ----------------------------
	protected bool HasAnyBurnableItems()
	{
		if (GetInventory().AttachmentCount() > 0)
			return true;

		CargoBase cargo = GetInventory().GetCargo();
		if (cargo && cargo.GetItemCount() > 0)
			return true;

		return false;
	}

	protected void UpdateBurnTimer()
	{
		if (!HasAnyBurnableItems())
		{
			if (m_BurnTimer)
				m_BurnTimer.Stop();
			return;
		}

		if (!m_BurnTimer)
			m_BurnTimer = new Timer(CALL_CATEGORY_GAMEPLAY);

		// уже запущен? Timer сам не любит повторный Run, поэтому просто перезапустим
		m_BurnTimer.Stop();
		m_BurnTimer.Run(BURN_UPDATE_INTERVAL_S, this, "BurnTick", NULL, true);
	}

	void BurnTick()
	{
		if (!(GetGame().IsServer() || !GetGame().IsMultiplayer()))
			return;

		// 1) attachments
		int attCount = GetInventory().AttachmentCount();
		for (int i = attCount - 1; i >= 0; --i)
		{
			EntityAI e = GetInventory().GetAttachmentFromIndex(i);
			BurnSingleEntity(e);
		}

		// 2) cargo
		CargoBase cargo = GetInventory().GetCargo();
		if (cargo)
		{
			int c = cargo.GetItemCount();
			for (int j = c - 1; j >= 0; --j)
			{
				EntityAI e2 = cargo.GetItem(j);
				BurnSingleEntity(e2);
			}
		}

		// если опустело — остановим тик
		if (!HasAnyBurnableItems())
			UpdateBurnTimer();
	}

	protected void BurnSingleEntity(EntityAI entity)
	{
		ItemBase item = ItemBase.Cast(entity);
		if (!item)
			return;

		// Защита: работаем только с прямыми детьми этого контейнера
		if (item.GetHierarchyParent() != this)
			return;

		// Наносим урон
		float maxHp = item.GetMaxHealth("", "");
		if (maxHp <= 0)
			maxHp = 100;

		float dmg = maxHp * BURN_MAXHP_PCT_PER_SEC * BURN_UPDATE_INTERVAL_S;
		item.AddHealth("", "", -dmg);

		// Если сгорел — FX + удалить
		if (item.IsRuined())
		{
			vector pos = item.GetPosition();
			PlayBurntItemFx(pos);
			GetGame().ObjectDelete(item);
		}
	}

	// ----------------------------
	// FX при сгорании предмета (3 сек)
	// ----------------------------
	protected void PlayBurntItemFx(vector worldPos)
	{
		// В сингле можно сразу локально
		if (!GetGame().IsMultiplayer())
		{
			PlayBurntItemFx_Local(worldPos);
			return;
		}

		// На сервере рассылаем RPC всем клиентам (в пределах репликации объекта)
		if (GetGame().IsServer())
		{
			Param1<vector> p = new Param1<vector>(worldPos);
			GetGame().RPCSingleParam(this, RPC_SFP_BURNT_ITEM_FX, p, true, NULL);
		}
	}

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);

		if (rpc_type == RPC_SFP_BURNT_ITEM_FX)
		{
			Param1<vector> ppos;
			if (ctx.Read(ppos))
			{
				PlayBurntItemFx_Local(ppos.param1);
			}
		}
	}

	protected void PlayBurntItemFx_Local(vector worldPos)
	{
		if (GetGame().IsDedicatedServer())
			return;

		Particle p = ParticleManager.GetInstance().PlayInWorld(ParticleList.SFP_MOLOTOV_PART_obj, worldPos);
		if (p)
		{
			// Остановить через 3 секунды
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(StopParticleSafe, 3000, false, p);
		}
	}

	protected void StopParticleSafe(Particle p)
	{
		if (p)
			p.Stop();
	}

	// ----------------------------
	// Постоянные эффекты огня (клиент)
	// ----------------------------
	protected void StartAmbientFireEffects()
	{
		// Свет
		if (!m_Light)
		{
			m_Light = FireplaceLight.Cast(ScriptedLightBase.CreateLight(FireplaceLight, Vector(0,0,0), 1));
			if (m_Light)
			{
				m_Light.AttachOnMemoryPoint(this, MEMPOINT_LIGHT);
				m_Light.FadeBrightnessTo(FireplaceLight.m_FireplaceBrightness, 0);
				m_Light.FadeRadiusTo(FireplaceLight.m_FireplaceRadius, 0);
			}
		}

		// Партикл огня
		if (!m_FireParticle)
		{
			vector lp = GetMemoryPointPos(MEMPOINT_FIRE);
			m_FireParticle = ParticleManager.GetInstance().PlayOnObject(ParticleList.SFP_FIRE01_GRILL, this, lp, vector.Zero, true);
		}

		// Звук
		if (!m_FireSoundLoop)
		{
			PlaySoundSetLoop(m_FireSoundLoop, "HeavyFire_SoundSet", 1.0, 2.0);
		}
	}

	protected void StopAmbientFireEffects()
	{
		if (m_Light)
			m_Light.FadeOut();

		if (m_FireParticle)
			m_FireParticle.Stop();

		if (m_FireSoundLoop)
			m_FireSoundLoop.SoundStop();
	}

	// ----------------------------
	// Согревание/комфорт как от костра (сервер)
	// ----------------------------
	protected void InitHeatSource()
	{
		m_UTSSettings = new UniversalTemperatureSourceSettings();
		m_UTSSettings.m_UpdateInterval      = HEAT_UPDATE_INTERVAL_S;
		m_UTSSettings.m_Updateable          = true;
		m_UTSSettings.m_TemperatureItemCap  = GameConstants.ITEM_TEMPERATURE_NEUTRAL_ZONE_MIDDLE;
		m_UTSSettings.m_TemperatureCap      = HEAT_TEMP_CAP;
		m_UTSSettings.m_RangeFull           = HEAT_RANGE_FULL;
		m_UTSSettings.m_RangeMax            = HEAT_RANGE_MAX;

		m_UTSLambda = new UniversalTemperatureSourceLambdaConstant();
		m_UTSource  = new UniversalTemperatureSource(this, m_UTSSettings, m_UTSLambda);
		m_UTSource.SetActive(true);
	}
}

/*class SFP_Static_MusorkaFire : ItemBase
{
	// ——— ЗВУК/ЧАСТИЦЫ/СВЕТ ———
	const string LOOP_SOUND = "HeavyFire_SoundSet";
	const vector PARTICLE_OFFSET = "0 0.24 0";

	// RPC для спавна партикла сгорания
	static const int RPC_SFP_MUSORKA_BURN_PARTICLE = 250219; // любое уникальное число

	protected EffectSound m_AmbientSoundLoop;
	protected Particle m_ParticleEfx;
	protected FireplaceLight m_TreeLight;

	// ——— СЖИГАНИЕ ПРЕДМЕТОВ ———
	const float BURN_TICK_TIME        = 5.0;   // сек между тиками
	const float BURN_DAMAGE_PER_TICK  = 15.0;  // урон за тик

	// ——— UNIVERSAL TEMPERATURE SOURCE (как у костра) ———
	protected ref UniversalTemperatureSource m_UTSource;
	protected ref UniversalTemperatureSourceSettings m_UTSSettings;
	protected ref UniversalTemperatureSourceLambdaFireplace m_UTSLFireplace;

	// Настройки «вечного» источника тепла
	const float HEAT_TEMPERATURE     = 300.0; // макс. температура источника (°C)
	const float HEAT_RADIUS_METERS   = 5.0;   // радиус действия тепла
	const float HEAT_UPDATE_INTERVAL = 3.0;   // период обновления UTS (сек)

	// ——— БАЗОВОЕ ПОВЕДЕНИЕ ———
	override bool CanPutIntoHands(EntityAI parent)
	{
		return false;
	}

	override bool CanReceiveItemIntoCargo(EntityAI item)
	{
		return IsOpen() && super.CanReceiveItemIntoCargo(item);
	}

	override void EEInit()
	{
		super.EEInit();

		// Сервер: цикл сжигания + источник тепла (UTS)
		if (GetGame().IsServer())
		{
			// Запускаем периодическое «сгорание» содержимого
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(BurnItemsInTrash, BURN_TICK_TIME * 1000, true);

			// Поднимаем UniversalTemperatureSource (как у FireplaceBase)
			InitUTS();
		}

		// Клиент: визуальные эффекты
		if (!GetGame().IsDedicatedServer())
		{
			InitializeEffects();
		}

		// Обновляемся каждый кадр (чтобы UTS гарантированно работал)
		SetEventMask(EntityEvent.FRAME);
	}

	protected void InitializeEffects()
	{
		PlaySoundSetLoop(m_AmbientSoundLoop, LOOP_SOUND, 0, 0);
		m_ParticleEfx = ParticleManager.GetInstance().PlayOnObject(ParticleList.SFP_FIRE01_GRILL, this, PARTICLE_OFFSET);

		m_TreeLight = FireplaceLight.Cast(ScriptedLightBase.CreateLight(FireplaceLight, "0 0 0"));
		if (m_TreeLight)
			m_TreeLight.AttachOnMemoryPoint(this, "light");
	}

	protected void InitUTS()
	{
		m_UTSSettings = new UniversalTemperatureSourceSettings();
		m_UTSSettings.m_ManualUpdate       = true; // сами дёргаем Update()
		m_UTSSettings.m_TemperatureItemCap = GameConstants.ITEM_TEMPERATURE_NEUTRAL_ZONE_MIDDLE;
		m_UTSSettings.m_TemperatureCap     = HEAT_TEMPERATURE;

		// Радиусы: «ядро» ~60% от общего радиуса, максимум = HEAT_RADIUS_METERS
		m_UTSSettings.m_RangeFull          = HEAT_RADIUS_METERS * 0.6;
		m_UTSSettings.m_RangeMax           = HEAT_RADIUS_METERS;

		// «Каминовская» лямбда — уже умеет греть игроков/вещи вокруг
		m_UTSLFireplace = new UniversalTemperatureSourceLambdaFireplace();
		m_UTSLFireplace.SetSmallFireplaceTemperatureMax(HEAT_TEMPERATURE * 0.6);
		m_UTSLFireplace.SetNormalFireplaceTemperatureMax(HEAT_TEMPERATURE);

		m_UTSource = new UniversalTemperatureSource(this, m_UTSSettings, m_UTSLFireplace);
	}

	override void EOnFrame(IEntity other, float timeSlice)
	{
		super.EOnFrame(other, timeSlice);

		if (GetGame().IsServer() && m_UTSource)
		{
			UpdateHeatAlwaysOn();
		}
	}

	// ——— ТЕПЛО: ВСЕГДА ВКЛЮЧЕНО ———
	// Этот апдейт делает объект горячим и раздаёт тепло вокруг через UTS
	void UpdateHeatAlwaysOn()
	{
		// Поддерживаем высокую температуру самого объекта
		SetTemperature(HEAT_TEMPERATURE);

		// Данные для лямбды (иначе она может ограничивать температуру)
		m_UTSLFireplace.SetFuelCount(1);
		m_UTSLFireplace.SetCurrentTemperature(GetTemperature());

		// Распространяем тепло на игроков/вещи в радиусе
		m_UTSource.Update(m_UTSSettings, m_UTSLFireplace);
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);

		// Клиент: убрать эффекты
		if (!GetGame().IsDedicatedServer())
			CleanupEffects();

		// Сервер: остановить очереди и обнулить UTS
		if (GetGame().IsServer())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(BurnItemsInTrash);

			m_UTSource      = null;
			m_UTSSettings   = null;
			m_UTSLFireplace = null;
		}
	}

	protected void CleanupEffects()
	{
		if (m_AmbientSoundLoop) SEffectManager.DestroyEffect(m_AmbientSoundLoop);
		if (m_ParticleEfx)      m_ParticleEfx.Stop();
		if (m_TreeLight)        m_TreeLight.Destroy();

		m_AmbientSoundLoop = null;
		m_ParticleEfx      = null;
		m_TreeLight        = null;
	}

	// ——— СЖИГАНИЕ ПРЕДМЕТОВ ВНУТРИ ———
	void BurnItemsInTrash()
	{
		// ЛОГИКА СЖИГАНИЯ ИДЁТ ТОЛЬКО НА СЕРВЕРЕ
		if (!GetGame().IsServer())
			return;

		if (!GetInventory())
			return;

		array<EntityAI> items = new array<EntityAI>;
		GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);

		foreach (EntityAI item : items)
		{
			if (!item || item == this)            continue;
			if (item.IsDamageDestroyed())         continue;

			float newHealth = item.GetHealth("", "") - BURN_DAMAGE_PER_TICK;

			if (newHealth <= 0)
			{
				// Позиция, где должен сыграть партикл
				vector pos = item.GetPosition();

				// Посылаем RPC всем клиентам, чтобы они локально заспавнили партикл
				SendBurnParticleRPC(pos);

				// Удаляем предмет на сервере (репликнется на всех)
				GetGame().ObjectDelete(item);
			}
			else
			{
				item.SetHealth("", "", newHealth);
			}
		}
	}

	// Отправка RPC с позицией сгорающего предмета
	protected void SendBurnParticleRPC(vector pos)
	{
		if (!GetGame().IsServer())
			return;

		Param1<vector> data = new Param1<vector>(pos);
		// identity = null -> шлём всем, кто "видит" этот объект
		GetGame().RPCSingleParam(this, RPC_SFP_MUSORKA_BURN_PARTICLE, data, true);
	}

	// Клиент: получаем RPC и создаём частицы
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);

		if (rpc_type == RPC_SFP_MUSORKA_BURN_PARTICLE)
		{
			vector pos;
			if (!ctx.Read(pos))
				return;

			if (!GetGame().IsDedicatedServer())
			{
				Particle p = ParticleManager.GetInstance().PlayInWorld(ParticleList.SFP_MOLOTOV_PART_obj, pos);
				GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(StopBurnParticle, 3000, false, p);
			}
		}
	}

	// Остановка временного партикла
	void StopBurnParticle(Particle p)
	{
		if (p) p.Stop();
	}
}
