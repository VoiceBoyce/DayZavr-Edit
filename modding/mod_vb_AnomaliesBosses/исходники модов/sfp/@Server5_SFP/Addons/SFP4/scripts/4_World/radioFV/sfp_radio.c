// Маленький красный индикатор питания для радио (динамический свет)
class SFP_RadioPowerLight extends PointLightBase
{
    void SFP_RadioPowerLight()
    {
        SetVisibleDuringDaylight(true);

        SetRadiusTo(0.1);
        SetBrightnessTo(1.0);

        SetCastShadow(false);
        SetFlareVisible(false);
        SetFadeOutTime(0.1);

        // красный цвет
        SetAmbientColor(1.0, 0.1, 0.1);
        SetDiffuseColor(1.0, 0.1, 0.1);
    }
}

// Само радио
class SFP_Radio extends Radio
{
	// RPC IDs (custom, must not collide with vanilla ERPCs)
	static const int SFP_RPC_RADIO_REQUEST_TRACKINFO = 23881;
	static const int SFP_RPC_RADIO_SEND_TRACKINFO    = 23882;

    // ===== LED как у RemoteDetonator (селекция "led") =====
    const string SFP_COLOR_LED_OFF      = "#(argb,8,8,3)color(0,0,0,1.0,co)";
    const string SFP_COLOR_LED_LIT      = "#(argb,8,8,3)color(1,0,0,1.0,co)";
    const string SFP_SELECTION_NAME_LED = "led"; // имя селекции в модели/config

    // ===== КАССЕТА / ЗВУК =====
    protected EffectSound         m_SFPCassetteSound;
    protected bool                m_SFPCassettePlaying;   // реально играет звук
    protected bool                m_SFPCassetteSwitch;    // Play/Stop
    protected float               m_SFPCassetteVolume;    // 0.0 .. 1.0

	// current soundset to play (client receives it from server via RPC)
	protected string               m_SFPCurrentSoundSet;
	protected bool                 m_SFPPendingRestartAfterTrackInfo;
	protected bool                 m_SFPRequestedTrackInfo;

    // ===== СИНХРОНИЗАЦИЯ СМЕНЫ ТРЕКА =====
    protected int                 m_SFPTrackChangeCounter;       // сервер: увеличивается при смене трека во время воспроизведения
    protected int                 m_SFPLastTrackChangeCounter;   // клиент: последнее известное значение, чтобы отслеживать изменение

	// ===== AUTO NEXT TRACK (server) =====
	// Раньше окончание трека проверялось в OnWork() (раз в 1-3 секунды),
	// из-за чего при loop-проигрывании трек успевал стартовать заново с начала
	// и только потом переключался на следующий. Теперь используем точный таймер CallLater.
	protected bool                m_SFPServerAutoNextArmed;

    // ===== ЛАМПОЧКА ПИТАНИЯ (динамический свет) =====
    protected SFP_RadioPowerLight m_PowerLight;

    // ===== ШУМ ДЛЯ AI =====
    static ref NoiseParams        m_SFPRadioNoiseQuiet;
    static ref NoiseParams        m_SFPRadioNoiseNormal;
    static ref NoiseParams        m_SFPRadioNoiseLoud;
    static NoiseSystem            m_SFPRadioNoiseSystem;

    protected float               m_SFPNoiseTimer;       // счётчик до следующего шума
    static const float            SFP_NOISE_INTERVAL = 5.0; // раз в 5 сек генерим шум

    // -----------------------
    //  КОНСТРУКТОР
    // -----------------------
    void SFP_Radio()
    {
        m_SFPCassettePlaying        = false;
        m_SFPCassetteSwitch         = false;
        m_SFPCassetteVolume         = 1.0;
        m_SFPNoiseTimer             = 0.0;
        m_PowerLight                = null;
        m_SFPTrackChangeCounter     = 0;
        m_SFPLastTrackChangeCounter = 0;
		m_SFPCurrentSoundSet          = string.Empty;
		m_SFPPendingRestartAfterTrackInfo = false;
		m_SFPRequestedTrackInfo       = false;
		m_SFPServerAutoNextArmed      = false;

        RegisterNetSyncVariableBool("m_SFPCassetteSwitch");
        RegisterNetSyncVariableFloat("m_SFPCassetteVolume");
        RegisterNetSyncVariableInt("m_SFPTrackChangeCounter");
    }

	// -----------------------
	//  RPC: client requests current soundset, server responds
	// -----------------------
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);

		// SERVER: answer requests
		if (GetGame() && GetGame().IsServer())
		{
			switch (rpc_type)
			{
				case SFP_RPC_RADIO_REQUEST_TRACKINFO:
				{
					// no params needed
					SFP_ServerSendTrackInfo(sender);
					break;
				}
			}
			return;
		}

		// CLIENT/SP: receive current soundset
		if (GetGame() && (!GetGame().IsDedicatedServer()))
		{
			switch (rpc_type)
			{
				case SFP_RPC_RADIO_SEND_TRACKINFO:
				{
					Param1<string> p = new Param1<string>(string.Empty);
					if (ctx.Read(p))
					{
						string oldSS = m_SFPCurrentSoundSet;
						m_SFPCurrentSoundSet = p.param1;
						m_SFPRequestedTrackInfo = false;

						// If we were waiting for fresh trackinfo to restart audio
						if (m_SFPPendingRestartAfterTrackInfo)
						{
							m_SFPPendingRestartAfterTrackInfo = false;
							if (m_SFPCassetteSwitch && CanPlaySFPCassette())
							{
								StopSFPCassette();
								StartSFPCassette();
							}
						}
						else
						{
							// Сервер присылает TrackInfo при смене трека.
							// Чтобы не ждать netSync-счётчик, переключаем звук сразу по RPC.
							if (m_SFPCassetteSwitch && CanPlaySFPCassette())
							{
								if (m_SFPCassettePlaying && oldSS != string.Empty && oldSS != m_SFPCurrentSoundSet)
								{
									StopSFPCassette();
									StartSFPCassette();
								}
								else
								{
									SFP_UpdateCassetteSound();
								}
							}
							else
							{
								SFP_UpdateCassetteSound();
							}
						}
					}
					break;
				}
			}
		}
	}

	protected void SFP_ClientRequestTrackInfo()
	{
		if (!IsClient())
			return;
		if (m_SFPRequestedTrackInfo)
			return;
		m_SFPRequestedTrackInfo = true;
		GetGame().RPCSingleParam(this, SFP_RPC_RADIO_REQUEST_TRACKINFO, null, true);
	}

	protected string SFP_ServerGetCurrentSoundSet()
	{
		SFP_Cassette_Base cassette = FindInsertedCassette();
		if (!cassette)
			return string.Empty;
		return cassette.SFP_GetCurrentSoundSet();
	}

	protected void SFP_ServerSendTrackInfo(PlayerIdentity target = null)
	{
		if (!GetGame() || !GetGame().IsServer())
			return;

		string ss = SFP_ServerGetCurrentSoundSet();
		Param1<string> p = new Param1<string>(ss);
		GetGame().RPCSingleParam(this, SFP_RPC_RADIO_SEND_TRACKINFO, p, true, target);
	}

	protected void SFP_ServerArmAutoNextFromCurrentTrack()
	{
		// отменяем предыдущий таймер
		if (GetGame())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(this.SFP_ServerAutoNextTimer);
		}
		m_SFPServerAutoNextArmed = false;

		if (!GetGame() || !GetGame().IsServer())
			return;
		if (!m_SFPCassetteSwitch)
			return;
		if (!GetCompEM() || !GetCompEM().IsWorking())
			return;
		SFP_Cassette_Base cassette = FindInsertedCassette();
		if (!cassette)
			return;

		SFP_RadioConfig cfg = SFP_RadioConfig.Get();
		if (!cfg || !cfg.AutoNextTrackEnabled)
			return;

		float dur = cassette.SFP_GetCurrentTrackDurationSeconds();
		if (dur <= 0.0)
			return;
		if (dur < cfg.MinDurationSeconds)
			return;

		// Небольшой "зазор" до конца, чтобы трек не успел стартовать заново по кругу
		// (loop) из-за сетевой задержки. Настраивается в JSON.
		float lead = cfg.TrackSwitchLeadSeconds;
		if (lead < 0.0)
			lead = 0.0;
		if (lead > 5.0)
			lead = 5.0;

		int delayMS = (int)Math.Round((dur - lead) * 1000.0);
		if (delayMS < 0)
			delayMS = 0;

		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.SFP_ServerAutoNextTimer, delayMS, false);
		m_SFPServerAutoNextArmed = true;
	}

	// Серверный таймер окончания трека
	void SFP_ServerAutoNextTimer()
	{
		m_SFPServerAutoNextArmed = false;
		if (!GetGame() || !GetGame().IsServer())
			return;
		if (!m_SFPCassetteSwitch)
			return;
		if (!GetCompEM() || !GetCompEM().IsWorking())
			return;
		if (!FindInsertedCassette())
			return;

		SFP_ServerAdvanceTrack();
	}

	protected void SFP_ServerAdvanceTrack()
	{
		if (!GetGame() || !GetGame().IsServer())
			return;
		SFP_Cassette_Base cassette = FindInsertedCassette();
		if (!cassette)
			return;

		cassette.SFP_NextTrack();
		SFP_MarkTrackChanged();
	}

    protected bool IsClient()
    {
        if (!GetGame())
            return false;

        return (GetGame().IsClient() || !GetGame().IsMultiplayer());
    }

    // -----------------------
    //  ИНИТ ДЛЯ ШУМА + LED OFF
    // -----------------------
    override void EEInit()
    {
        super.EEInit();

        // при спавне гасим LED
        SFP_UpdateLed(false);

        if (GetGame().IsServer())
        {
            if (!m_SFPRadioNoiseSystem)
                m_SFPRadioNoiseSystem = GetGame().GetNoiseSystem();

            if (m_SFPRadioNoiseSystem)
            {
                // грузим шумы из cfgVehicles SFP_Radio NoiseRadioXxx
                if (!m_SFPRadioNoiseQuiet)
                {
                    m_SFPRadioNoiseQuiet = new NoiseParams();
                    m_SFPRadioNoiseQuiet.LoadFromPath("cfgVehicles " + GetType() + " NoiseRadioQuiet");
                }

                if (!m_SFPRadioNoiseNormal)
                {
                    m_SFPRadioNoiseNormal = new NoiseParams();
                    m_SFPRadioNoiseNormal.LoadFromPath("cfgVehicles " + GetType() + " NoiseRadioNormal");
                }

                if (!m_SFPRadioNoiseLoud)
                {
                    m_SFPRadioNoiseLoud = new NoiseParams();
                    m_SFPRadioNoiseLoud.LoadFromPath("cfgVehicles " + GetType() + " NoiseRadioLoud");
                }
            }
        }
    }

    // ==============================
    //   ТЕКСТУРНЫЙ LED (селекция "led")
    // ==============================
    void SFP_UpdateLed(bool is_on)
    {
        if (!GetGame())
            return;

        // только клиент / SP
        if (GetGame().IsServer() && GetGame().IsMultiplayer())
            return;

        int idx = GetHiddenSelectionIndex(SFP_SELECTION_NAME_LED);
        if (idx < 0)
            return;

        if (is_on)
            SetObjectTexture(idx, SFP_COLOR_LED_LIT);
        else
            SetObjectTexture(idx, SFP_COLOR_LED_OFF);
    }

    // ==============================
    //        ВСПОМОГАТЕЛЬНОЕ
    // ==============================

    // ищем любую кассету, основанную на SFP_Cassette_Base
    SFP_Cassette_Base FindInsertedCassette()
    {
        int count = GetInventory().AttachmentCount();
        for (int i = 0; i < count; i++)
        {
            EntityAI att = GetInventory().GetAttachmentFromIndex(i);

            SFP_Cassette_Base cassette;
            if (Class.CastTo(cassette, att))
                return cassette;
        }
        return null;
    }

    bool SFP_HasCassette()
    {
        return FindInsertedCassette() != null;
    }

    protected bool CanPlaySFPCassette()
    {
        if (!IsClient())
            return false;

        if (!GetCompEM())
            return false;

        // есть батарейка и радио включено
        if (!GetCompEM().IsWorking())
            return false;

        if (!SFP_HasCassette())
            return false;

        return true;
    }

    // ==============================
    //         ЗВУК КАССЕТЫ
    // ==============================

    protected void StartSFPCassette()
    {
        if (!CanPlaySFPCassette())
            return;

        // In MP, client receives current soundset from server via RPC.
        // If we don't have it yet, request and wait.
        if (m_SFPCurrentSoundSet == string.Empty)
        {
			SFP_ClientRequestTrackInfo();
			return;
        }

        string soundSet = m_SFPCurrentSoundSet;
        if (soundSet == "")
            return;

        if (!m_SFPCassettePlaying)
        {
            if (PlaySoundSetLoop(m_SFPCassetteSound, soundSet, 0.1, 0.2))
            {
                m_SFPCassettePlaying = true;
                if (m_SFPCassetteSound)
                    m_SFPCassetteSound.SetSoundVolume(m_SFPCassetteVolume);
            }
        }
    }

    protected void StopSFPCassette()
    {
        if (!IsClient())
            return;

        if (!m_SFPCassettePlaying)
            return;

        StopSoundSet(m_SFPCassetteSound);
        m_SFPCassetteSound   = null;
        m_SFPCassettePlaying = false;
    }

    void SFP_UpdateCassetteSound()
    {
        if (!IsClient())
            return;

        bool should_play = m_SFPCassetteSwitch && CanPlaySFPCassette();

        if (should_play && !m_SFPCassettePlaying)
        {
            StartSFPCassette();
        }
        else if (!should_play && m_SFPCassettePlaying)
        {
            StopSFPCassette();
        }

        if (m_SFPCassettePlaying && m_SFPCassetteSound)
        {
            m_SFPCassetteSound.SetSoundVolume(m_SFPCassetteVolume);
        }
    }

    // ==============================
    //   ВЫБОР ПАРАМЕТРОВ ШУМА
    // ==============================
    NoiseParams SFP_GetCurrentNoiseParams()
    {
        if (!m_SFPRadioNoiseQuiet || !m_SFPRadioNoiseNormal || !m_SFPRadioNoiseLoud)
            return null;

        float vol = m_SFPCassetteVolume; // 0.0 .. 1.0

        if (vol < 0.34)
            return m_SFPRadioNoiseQuiet;   // тихо
        else if (vol < 0.67)
            return m_SFPRadioNoiseNormal;  // средне

        return m_SFPRadioNoiseLoud;        // громко
    }

    // ==============================
    //      СЕТЕВАЯ ЛОГИКА
    // ==============================

    void SFP_SetCassetteEnabled(bool enable) // сервер
    {
        m_SFPCassetteSwitch = enable;

		if (GetGame() && GetGame().IsServer())
		{
			if (enable)
			{
				SFP_ServerSendTrackInfo();
				SFP_ServerArmAutoNextFromCurrentTrack();
			}
			else
			{
				GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(this.SFP_ServerAutoNextTimer);
				m_SFPServerAutoNextArmed = false;
			}
		}

        SetSynchDirty();
    }

    bool SFP_IsCassetteEnabled()
    {
        return m_SFPCassetteSwitch;
    }

    bool SFP_CanIncreaseVolume()
    {
        return m_SFPCassetteVolume < 1.0;
    }

    bool SFP_CanDecreaseVolume()
    {
        return m_SFPCassetteVolume > 0.0;
    }

    void SFP_ChangeCassetteVolume(float delta) // сервер
    {
        float new_vol = m_SFPCassetteVolume + delta;

        if (new_vol < 0.0)
            new_vol = 0.0;
        if (new_vol > 1.0)
            new_vol = 1.0;

        m_SFPCassetteVolume = new_vol;
        SetSynchDirty();
    }

    float SFP_GetCassetteVolume()
    {
        return m_SFPCassetteVolume;
    }

    // сервер: помечаем, что трек сменился во время воспроизведения
    void SFP_MarkTrackChanged()
    {
        if (!GetGame() || !GetGame().IsServer())
            return;

        m_SFPTrackChangeCounter++;
		SFP_ServerSendTrackInfo();
		SFP_ServerArmAutoNextFromCurrentTrack();
        SetSynchDirty();
    }

    override void OnVariablesSynchronized()
    {
        super.OnVariablesSynchronized();

        // Обновляем звук (клиент)
        SFP_UpdateCassetteSound();

        // Проверяем, не сменился ли трек (только на клиенте)
        if (IsClient())
        {
            if (m_SFPTrackChangeCounter != m_SFPLastTrackChangeCounter)
            {
                m_SFPLastTrackChangeCounter = m_SFPTrackChangeCounter;

				// Обычно сервер присылает новый soundset по RPC сразу при смене трека.
				// Но если RPC не дошёл/не успел — запрашиваем.
				if (m_SFPCurrentSoundSet == string.Empty)
				{
					m_SFPPendingRestartAfterTrackInfo = true;
					SFP_ClientRequestTrackInfo();
				}
				else
				{
					if (m_SFPCassetteSwitch && CanPlaySFPCassette())
					{
						StopSFPCassette();
						StartSFPCassette();
					}
				}
            }
        }

        // LED по состоянию энергии
        bool working = (GetCompEM() && GetCompEM().IsWorking());
        SFP_UpdateLed(working);
    }

    // ==============================
    //   СОХРАНЕНИЕ / ЗАГРУЗКА
    // ==============================

    override void OnStoreSave(ParamsWriteContext ctx)
    {
        super.OnStoreSave(ctx);
        ctx.Write(m_SFPCassetteVolume);
    }

    override bool OnStoreLoad(ParamsReadContext ctx, int version)
    {
        if (!super.OnStoreLoad(ctx, version))
            return false;

        float stored_vol;
        if (ctx.Read(stored_vol))
        {
            if (stored_vol < 0.0)
                stored_vol = 0.0;
            if (stored_vol > 1.0)
                stored_vol = 1.0;

            m_SFPCassetteVolume = stored_vol;
        }
        else
        {
            m_SFPCassetteVolume = 1.0;
        }

        return true;
    }

    // ==============================
    //    ЭНЕРГИЯ / ЛАМПА / ШУМ
    // ==============================

    override void OnWorkStart()
    {
        super.OnWorkStart();
        SFP_UpdateCassetteSound();
        m_SFPNoiseTimer = 0.0;

		// server: arm auto-next if cassette is enabled
		if (GetGame() && GetGame().IsServer())
		{
			SFP_ServerSendTrackInfo();
			SFP_ServerArmAutoNextFromCurrentTrack();
		}

        // включаем текстурный LED
        SFP_UpdateLed(true);

        // включаем динамический свет (только на клиенте или в SP)
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer())
        {
            if (!m_PowerLight)
            {
                // создаём light и цепляем на memory point "power_light"
                m_PowerLight = SFP_RadioPowerLight.Cast(ScriptedLightBase.CreateLight(SFP_RadioPowerLight, "0 0 0"));
                if (m_PowerLight)
                {
                    m_PowerLight.AttachOnMemoryPoint(this, "power_light");
                }
            }
        }
    }

    override void OnWorkStop()
    {
        super.OnWorkStop();
        StopSFPCassette();
        m_SFPNoiseTimer = 0.0;
		if (GetGame() && GetGame().IsServer())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(this.SFP_ServerAutoNextTimer);
			m_SFPServerAutoNextArmed = false;
		}

        // выключаем текстурный LED
        SFP_UpdateLed(false);

        // гасим динамический свет (только на клиенте или в SP)
        if (!GetGame().IsServer() || !GetGame().IsMultiplayer())
        {
            if (m_PowerLight)
            {
                m_PowerLight.Destroy();
                m_PowerLight = null;
            }
        }
    }

    // вызывается EnergyManager'ом примерно каждую секунду
    override void OnWork(float consumed_energy)
    {
        super.OnWork(consumed_energy);

        if (!GetGame().IsServer())
            return;

        // шум только когда кассета логически включена (Play)
        if (!m_SFPCassetteSwitch)
            return;

        if (!m_SFPRadioNoiseSystem)
            return;

        NoiseParams curNoise = SFP_GetCurrentNoiseParams();
        if (!curNoise)
            return;

        m_SFPNoiseTimer += 1.0;

        if (m_SFPNoiseTimer >= SFP_NOISE_INTERVAL)
        {
            m_SFPNoiseTimer = 0.0;
            MiscGameplayFunctions.GenerateAINoiseAtPositionEx(this, SFP_NOISE_INTERVAL, curNoise);
        }

    }

    override void EEItemAttached(EntityAI item, string slot_name)
    {
        super.EEItemAttached(item, slot_name);

        SFP_Cassette_Base cassette;
        if (Class.CastTo(cassette, item))
        {
            SFP_UpdateCassetteSound();
			if (GetGame() && GetGame().IsServer())
			{
				SFP_ServerSendTrackInfo();
				SFP_ServerArmAutoNextFromCurrentTrack();
			}
        }
    }

    override void EEItemDetached(EntityAI item, string slot_name)
    {
        super.EEItemDetached(item, slot_name);

        SFP_Cassette_Base cassette;
        if (Class.CastTo(cassette, item))
        {
            SFP_UpdateCassetteSound();
			m_SFPCurrentSoundSet = string.Empty;
			if (GetGame() && GetGame().IsServer())
			{
				GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(this.SFP_ServerAutoNextTimer);
				m_SFPServerAutoNextArmed = false;
			}
        }
    }

    // ==============================
    //          ЭКШЕНЫ
    // ==============================

    override void SetActions()
    {
        super.SetActions();

        // Возвращаем базовые Take/Drop, но убираем ненужную настройку радиостанций.
        RemoveAction(ActionTuneRadioStation);

        AddAction(ActionSFP_PlayCassette);
        AddAction(ActionSFP_StopCassette);
        AddAction(ActionSFP_CassetteVolumeUp);
        AddAction(ActionSFP_CassetteVolumeDown);
        AddAction(ActionSFP_CassetteNextTrack);
        AddAction(ActionSFP_CassettePrevTrack);
    }
}
