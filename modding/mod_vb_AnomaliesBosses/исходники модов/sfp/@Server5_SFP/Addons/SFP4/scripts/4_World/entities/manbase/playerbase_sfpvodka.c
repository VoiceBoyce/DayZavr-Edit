// SFP Vodka effects — логика на игроке (стакинг, спад, стамина, визуал)
//
// Где что «крутить»:
//  - scripts/4_World/sfp_vodkaconstants.c
//      баланс: как быстро напиваться/трезветь, штрафы стамины, шанс бессознанки, согревание
//  - scripts/3_Game/ppemanager/requesters/pperdrunkvodka.c
//      картинка: gaussian blur, двоение (ghost), хром.аберрация
//  - scripts/4_World/entities/manbase/dayzplayer/dayzplayercamera_base_sfpvodka.c
//      камера без ADS (1/3 лицо): «GTA4» качание + микросрывы

modded class PlayerBase
{
	protected float m_SFP_VodkaLevel = 0.0; // уровень в «дозах» (синхронизируется по сети)

	// сервер: чтобы не спамить синхронизацию каждый кадр
	protected float m_SFP_VodkaServerSyncAccum = 0.0;

	// клиент (только локальный игрок): таймер и флаги визуала
	protected float m_SFP_VodkaClientTime = 0.0;
	protected bool m_SFP_VodkaClientActive = false;
	protected PPERequester_DrunkVodka m_SFP_VodkaRequester;

	// клиент: шейк ADS (через AimingModel), сглаженный
	protected float m_SFP_VodkaCamX = 0.0;
	protected float m_SFP_VodkaCamY = 0.0;
	// Math.SmoothCD в DayZ принимает «скорость» как float[1] (inout массив)
	protected float m_SFP_VodkaCamXVel[1];
	protected float m_SFP_VodkaCamYVel[1];

	// «Иконка психа» во время пьянки.
	// ВНИМАНИЕ: мы используем слот DELM_BADGE_POISONED (часто в сборках туда цепляют Kuru/insanity).
	protected bool m_SFP_VodkaActivatedCrazyBadge = false;

	override void Init()
	{
		super.Init();
		RegisterNetSyncVariableFloat("m_SFP_VodkaLevel", 0.0, SFP_VodkaConstants.MAX_LEVEL, 2);
		m_SFP_VodkaLevel = 0.0;
		m_SFP_VodkaServerSyncAccum = 0.0;
		m_SFP_VodkaCamXVel[0] = 0.0;
		m_SFP_VodkaCamYVel[0] = 0.0;
	}

	// Добавить «пьянство» от выпитого объёма.
	// bottleFraction: 0..1, где 1.0 = полная бутылка.
	void SFP_AddVodkaFromBottleFraction(float bottleFraction)
	{
		// Делаем это ТОЛЬКО на сервере, чтобы не рассинхронило
		if (!(GetGame().IsServer() || !GetGame().IsMultiplayer()))
			return;

		bottleFraction = Math.Clamp(bottleFraction, 0.0, 1.0);

		float oldLevel = m_SFP_VodkaLevel;
		float add = bottleFraction * SFP_VodkaConstants.DOSES_PER_BOTTLE;
		m_SFP_VodkaLevel = Math.Clamp(m_SFP_VodkaLevel + add, 0.0, SFP_VodkaConstants.MAX_LEVEL);

		// Обновим синхронизацию (и сразу сбросим таймер «раз в секунду»)
		if (Math.AbsFloat(m_SFP_VodkaLevel - oldLevel) > 0.0001)
		{
			SetSynchDirty();
			m_SFP_VodkaServerSyncAccum = 0.0;
		}

		SFP_UpdateVodkaStamina();
		SFP_TryVodkaUnconsciousHit(oldLevel, m_SFP_VodkaLevel);
	}

	float SFP_GetVodkaIntensity()
	{
		// 0..INTENSITY_MAX (где 1 ≈ «одна полная бутылка»)
		return Math.Clamp(m_SFP_VodkaLevel / SFP_VodkaConstants.DOSES_PER_BOTTLE, 0.0, SFP_VodkaConstants.INTENSITY_MAX);
	}

	// Риск «упасть» (удар по шоку) — чем больше выпил, тем выше шанс.
	protected void SFP_TryVodkaUnconsciousHit(float oldLevel, float newLevel)
	{
		if (!(GetGame().IsServer() || !GetGame().IsMultiplayer()))
			return;

		if (newLevel < SFP_VodkaConstants.UNCON_START_LEVEL)
			return;

		// Кидаем шанс ТОЛЬКО когда уровень растёт (то есть при питье), а не при спадании
		if (newLevel <= oldLevel)
			return;

		float intensity = SFP_GetVodkaIntensity();

		// t: 0 на 1 бутылке, 1 на INTENSITY_MAX (по умолчанию 3 бутылки)
		float t = 0.0;
		if (SFP_VodkaConstants.INTENSITY_MAX > 1.0)
			t = Math.Clamp((intensity - 1.0) / (SFP_VodkaConstants.INTENSITY_MAX - 1.0), 0.0, 1.0);

		float prob = Math.Lerp(SFP_VodkaConstants.UNCON_PROB_AT_1_BOTTLE, SFP_VodkaConstants.UNCON_PROB_AT_2_BOTTLES, t);
		prob = Math.Clamp(prob, 0.0, SFP_VodkaConstants.UNCON_PROB_MAX);

		// Доп.усиление при очень высоком стаке (до 3 бутылок)
		float high = Math.Clamp(m_SFP_VodkaLevel / SFP_VodkaConstants.MAX_LEVEL, 0.0, 1.0);
		prob = Math.Clamp(prob + (0.15 * high), 0.0, SFP_VodkaConstants.UNCON_PROB_MAX);

		if (Math.RandomFloat01() < prob)
		{
			float shockHit = Math.Lerp(SFP_VodkaConstants.UNCON_SHOCK_HIT_MIN, SFP_VodkaConstants.UNCON_SHOCK_HIT_MAX, high);
			GiveShock(-shockHit); // удар по шоку -> может отправить в бессознанку
		}
	}

	// Штрафы стамины: реген, максимум (cap), ускоренный расход.
	protected void SFP_UpdateVodkaStamina()
	{
		if (!(GetGame().IsServer() || !GetGame().IsMultiplayer()))
			return;

		StaminaHandler sh = GetStaminaHandler();
		if (!sh)
			return;

		if (m_SFP_VodkaLevel > 0.01)
		{
			float intensity = SFP_GetVodkaIntensity();

			// t_hi: 0 после 1 бутылки, 1 на INTENSITY_MAX
			float t_hi = 0.0;
			if (SFP_VodkaConstants.INTENSITY_MAX > 1.0)
				t_hi = Math.Clamp((intensity - 1.0) / (SFP_VodkaConstants.INTENSITY_MAX - 1.0), 0.0, 1.0);

			// 1) Реген стамины
			float mult;
			if (intensity <= 1.0)
				mult = Math.Lerp(1.0, SFP_VodkaConstants.STAMINA_RECOVERY_AT_1_BOTTLE, intensity);
			else
				mult = Math.Lerp(SFP_VodkaConstants.STAMINA_RECOVERY_AT_1_BOTTLE, SFP_VodkaConstants.STAMINA_RECOVERY_MIN, t_hi);
			mult = Math.Clamp(mult, SFP_VodkaConstants.STAMINA_RECOVERY_MIN, 1.0);
			sh.SFP_SetVodkaRecoveryMultiplier(mult);

			// 2) Максимальная стамина (cap)
			float capMult;
			if (intensity <= 1.0)
				capMult = Math.Lerp(1.0, SFP_VodkaConstants.STAMINA_CAP_AT_1_BOTTLE, intensity);
			else
				capMult = Math.Lerp(SFP_VodkaConstants.STAMINA_CAP_AT_1_BOTTLE, SFP_VodkaConstants.STAMINA_CAP_MIN, t_hi);
			capMult = Math.Clamp(capMult, SFP_VodkaConstants.STAMINA_CAP_MIN, 1.0);
			sh.SFP_SetVodkaCapMultiplier(capMult);

			// 3) Расход стамины (бег/драка быстрее высаживают)
			float depMult;
			if (intensity <= 1.0)
				depMult = Math.Lerp(1.0, SFP_VodkaConstants.STAMINA_DEPLETE_AT_1_BOTTLE, intensity);
			else
				depMult = Math.Lerp(SFP_VodkaConstants.STAMINA_DEPLETE_AT_1_BOTTLE, SFP_VodkaConstants.STAMINA_DEPLETE_MAX, t_hi);
			depMult = Math.Clamp(depMult, 1.0, SFP_VodkaConstants.STAMINA_DEPLETE_MAX);
			sh.SFP_SetVodkaDepletionMultiplier(depMult);
		}
		else
		{
			// Возврат к норме
			sh.SFP_SetVodkaRecoveryMultiplier(1.0);
			sh.SFP_SetVodkaCapMultiplier(1.0);
			sh.SFP_SetVodkaDepletionMultiplier(1.0);
		}
	}

	// Сервер: спад уровня и «согрев» пока пьян
	protected void SFP_ServerVodkaDecay(float delta_time)
	{
		if (!(GetGame().IsServer() || !GetGame().IsMultiplayer()))
			return;

		if (m_SFP_VodkaLevel <= 0.0)
			return;

		// Согрев/комфорт идёт постоянно, пока есть уровень
		float intensity = SFP_GetVodkaIntensity();
		float n = Math.Clamp(intensity / SFP_VodkaConstants.INTENSITY_MAX, 0.0, 1.0);
		float hbPerSec = Math.Lerp(SFP_VodkaConstants.HEATBUFFER_GAIN_PER_SEC_AT_1_BOTTLE, SFP_VodkaConstants.HEATBUFFER_GAIN_MAX_PER_SEC, n);
		float hcPerSec = Math.Lerp(SFP_VodkaConstants.HEATCOMFORT_GAIN_PER_SEC_AT_1_BOTTLE, SFP_VodkaConstants.HEATCOMFORT_GAIN_MAX_PER_SEC, n);
		GetStatHeatBuffer().Add(hbPerSec * delta_time, "SFP_Vodka");
		GetStatHeatComfort().Add(hcPerSec * delta_time, "SFP_Vodka");

		// Спад: сколько «доз» уходит за секунду
		m_SFP_VodkaLevel = Math.Max(0.0, m_SFP_VodkaLevel - (delta_time / SFP_VodkaConstants.SECONDS_PER_DOSE));

		// Синхронизируем уровень примерно раз в секунду, либо когда упал до 0
		m_SFP_VodkaServerSyncAccum += delta_time;
		if (m_SFP_VodkaServerSyncAccum >= 1.0 || m_SFP_VodkaLevel <= 0.0)
		{
			SetSynchDirty();
			m_SFP_VodkaServerSyncAccum = 0.0;
		}

		SFP_UpdateVodkaStamina();
	}

	// Включить/выключить «иконку психа» на HUD пока пьян.
	protected void SFP_ClientVodkaUpdateCrazyBadge(bool active)
	{
		NotifiersManager nm = GetNotifiersManager();
		if (!nm)
			return;

		VirtualHud vh = nm.GetVirtualHud();
		if (!vh)
			return;

		// Во многих модпаках этот слот используют для Kuru/insanity.
		// Если у тебя «псих» в другом слоте — скажи, поменяем eDisplayElements.
		DisplayElementBadge badge = DisplayElementBadge.Cast(vh.GetElement(eDisplayElements.DELM_BADGE_POISONED));
		if (!badge)
			return;

		if (active)
		{
			if (!m_SFP_VodkaActivatedCrazyBadge)
			{
				badge.SetLevel(eBadgeLevel.FIRST);
				m_SFP_VodkaActivatedCrazyBadge = true;
			}
		}
		else
		{
			if (m_SFP_VodkaActivatedCrazyBadge)
			{
				badge.SetLevel(eBadgeLevel.NONE);
				m_SFP_VodkaActivatedCrazyBadge = false;
			}
		}
	}

	// Клиент: постпроцесс + камера
	protected void SFP_ClientVodkaVisuals(float delta_time)
	{
		if (GetGame().IsDedicatedServer())
			return;
		if (!IsPlayerSelected())
			return;

		float intensity = SFP_GetVodkaIntensity();

		if (intensity > 0.01)
		{
			m_SFP_VodkaClientTime += delta_time;

			if (!m_SFP_VodkaRequester)
				Class.CastTo(m_SFP_VodkaRequester, PPERequesterBank.GetRequester(PPERequester_DrunkVodka));

			if (m_SFP_VodkaRequester)
				m_SFP_VodkaRequester.SetVodkaEffects(intensity, m_SFP_VodkaClientTime);

			// Иконка на HUD (пока пьян)
			SFP_ClientVodkaUpdateCrazyBadge(true);

			// Определяем ADS (прицеливание)
			DayZPlayerImplementAiming aim = GetAimingModel();
			HumanInputController hic = GetInputController();
			bool ads = false;
			if (hic)
				ads = hic.WeaponADS();

			// Камера БЕЗ ADS (1/3 лицо): делаем «GTA4» качание через DayZPlayerCameraBase.ProcessCameraShake()
			DayZPlayerCameraBase camBase = DayZPlayerCameraBase.Cast(GetCurrentCamera());
			if (camBase)
				camBase.SFP_SetVodkaSway(intensity, m_SFP_VodkaClientTime, ads);

			// ADS (прицел): оставляем старый подход через AimingModel.SetCamShakeValues()
			if (aim)
			{
				float n = Math.Clamp(intensity / SFP_VodkaConstants.INTENSITY_MAX, 0.0, 1.0);
				float t = m_SFP_VodkaClientTime;

				// ВАЖНО: тернарник избегаем (в некоторых сборках старый Enforce Script парсит плохо)
				float speed;
				float amp;
				float smoothTime;
				float maxSpeed;
				if (ads)
				{
					speed = 1.5;
					amp = Math.Lerp(0.35, 7.80, n);
					smoothTime = 0.20;
					maxSpeed = 1000.0;
				}
				else
				{
					// Вне ADS шейк AimingModel выключаем (в этом режиме качает модифицированная камера)
					speed = 0.0;
					amp = 0.0;
					smoothTime = 0.10;
					maxSpeed = 1000.0;
				}

				float targetX = (Math.Sin(t * 0.38 * speed) * 0.90 + Math.Sin(t * 0.82 * speed) * 0.35) * amp;
				float targetY = (Math.Cos(t * 0.34 * speed) * 0.85 + Math.Sin(t * 0.70 * speed) * 0.35) * amp;

				m_SFP_VodkaCamX = Math.SmoothCD(m_SFP_VodkaCamX, targetX, m_SFP_VodkaCamXVel, smoothTime, maxSpeed, delta_time);
				m_SFP_VodkaCamY = Math.SmoothCD(m_SFP_VodkaCamY, targetY, m_SFP_VodkaCamYVel, smoothTime, maxSpeed, delta_time);

				if (ads)
					aim.SetCamShakeValues(m_SFP_VodkaCamX, m_SFP_VodkaCamY);
				else
					aim.SetCamShakeValues(0.0, 0.0);
			}

			m_SFP_VodkaClientActive = true;
		}
		else
		{
			if (m_SFP_VodkaClientActive)
			{
				m_SFP_VodkaClientActive = false;
				m_SFP_VodkaClientTime = 0.0;
				m_SFP_VodkaCamX = 0.0;
				m_SFP_VodkaCamY = 0.0;
				m_SFP_VodkaCamXVel[0] = 0.0;
				m_SFP_VodkaCamYVel[0] = 0.0;

				// Остановить постпроцесс
				if (m_SFP_VodkaRequester)
					m_SFP_VodkaRequester.Stop();

				// Остановить ADS-шейк
				DayZPlayerImplementAiming aim2 = GetAimingModel();
				if (aim2)
					aim2.SetCamShakeValues(0.0, 0.0);

				// Остановить качание камеры 1/3 лица
				DayZPlayerCameraBase camBase2 = DayZPlayerCameraBase.Cast(GetCurrentCamera());
				if (camBase2)
					camBase2.SFP_ClearVodkaSway();

				// Выключить иконку
				SFP_ClientVodkaUpdateCrazyBadge(false);
			}
			else
			{
				// На всякий случай гарантируем, что наш бейдж не останется висеть
				SFP_ClientVodkaUpdateCrazyBadge(false);
			}
		}
	}

	override void OnCommandHandlerTick(float delta_time, int pCurrentCommandID)
	{
		super.OnCommandHandlerTick(delta_time, pCurrentCommandID);

		// Сервер: спад
		SFP_ServerVodkaDecay(delta_time);

		// Клиент: постпроцесс + камера
		SFP_ClientVodkaVisuals(delta_time);
	}
}
