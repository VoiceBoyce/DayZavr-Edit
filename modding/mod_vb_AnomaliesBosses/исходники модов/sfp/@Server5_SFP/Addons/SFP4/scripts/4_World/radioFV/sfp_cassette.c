// БАЗОВЫЙ КЛАСС ДЛЯ ВСЕХ КАССЕТ
class SFP_Cassette_Base extends Inventory_Base
{
	protected int m_SFPTrackIndex; // текущий выбранный трек (0..N-1)

	void SFP_Cassette_Base()
	{
		m_SFPTrackIndex = 0;
		RegisterNetSyncVariableInt("m_SFPTrackIndex");
	}

	// --- Переопределяется в наследниках ---

	// количество треков на кассете
	int SFP_GetTrackCount()
	{
		return 1;
	}

	// вернуть soundset по индексу
	string SFP_GetSoundSetByIndex(int index)
	{
		return string.Empty;
	}

	// отображаемое название трека по индексу
	string SFP_GetTrackNameByIndex(int index)
	{
		return string.Empty;
	}

	// длительность трека (секунды). 0 = не переключать автоматически (будет играть по кругу)
	float SFP_GetTrackDurationSecondsByIndex(int index)
	{
		return 0.0;
	}

	// --- API ---

	// текущий soundset с учётом выбранного трека
	string SFP_GetCurrentSoundSet()
	{
		int count = SFP_GetTrackCount();
		if (count <= 0)
			return string.Empty;

		int idx = m_SFPTrackIndex;
		if (idx < 0 || idx >= count)
			idx = 0;

		return SFP_GetSoundSetByIndex(idx);
	}

	// совместимость: старый метод (если кто-то ещё его вызывает)
	string GetCassetteSoundSet()
	{
		return SFP_GetCurrentSoundSet();
	}

	// текущее имя трека (для сообщений игроку)
	string SFP_GetCurrentTrackName()
	{
		int count = SFP_GetTrackCount();
		if (count <= 0)
			return string.Empty;

		int idx = m_SFPTrackIndex;
		if (idx < 0 || idx >= count)
			idx = 0;

		return SFP_GetTrackNameByIndex(idx);
	}

	float SFP_GetCurrentTrackDurationSeconds()
	{
		int count = SFP_GetTrackCount();
		if (count <= 0)
			return 0.0;

		int idx = m_SFPTrackIndex;
		if (idx < 0 || idx >= count)
			idx = 0;

		return SFP_GetTrackDurationSecondsByIndex(idx);
	}

	int SFP_GetTrackIndex()
	{
		return m_SFPTrackIndex;
	}

	void SFP_SetTrackIndex(int idx)
	{
		int count = SFP_GetTrackCount();
		if (count <= 0)
		{
			m_SFPTrackIndex = 0;
			return;
		}

		if (idx < 0)
			idx = count - 1;
		else if (idx >= count)
			idx = 0;

		m_SFPTrackIndex = idx;
		SetSynchDirty();
	}

	void SFP_NextTrack()
	{
		SFP_SetTrackIndex(m_SFPTrackIndex + 1);
	}

	void SFP_PrevTrack()
	{
		SFP_SetTrackIndex(m_SFPTrackIndex - 1);
	}

	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		ctx.Write(m_SFPTrackIndex);
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;

		int idx;
		if (ctx.Read(idx))
		{
			m_SFPTrackIndex = idx;
		}
		else
		{
			m_SFPTrackIndex = 0;
		}

		return true;
	}
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionOsmotrItem);
	}
}

// ------------------------------------------------------------
//  CONFIG-DRIVEN CASSETTE
//  - Track list (soundsets, names, durations) is defined in JSON:
//    $profile:SFP/Radio/SFP_RadioConfig.json
//
//  Notes:
//   - In MP, the SERVER uses JSON. Clients receive CURRENT soundset via RPC
//     from SFP_Radio, so clients don't need the JSON to play correctly.
// ------------------------------------------------------------
class SFP_Cassette_Config extends SFP_Cassette_Base
{
	protected bool SFP_CanUseConfig()
	{
		if (!GetGame())
			return false;

		// don't touch $profile on pure MP clients (they may not have the server config)
		if (GetGame().IsClient() && GetGame().IsMultiplayer())
			return false;

		return true;
	}

	override int SFP_GetTrackCount()
	{
		if (!SFP_CanUseConfig())
			return 0;

		SFP_RadioConfig cfg = SFP_RadioConfig.Get();
		if (!cfg)
			return 0;
		return cfg.GetTrackCount(GetType());
	}

	override string SFP_GetSoundSetByIndex(int index)
	{
		if (!SFP_CanUseConfig())
			return string.Empty;

		SFP_RadioConfig cfg = SFP_RadioConfig.Get();
		if (!cfg)
			return string.Empty;

		SFP_RadioTrackCfg t = cfg.GetTrack(GetType(), index);
		if (!t)
			return string.Empty;
		return t.SoundSet;
	}

	override string SFP_GetTrackNameByIndex(int index)
	{
		if (!SFP_CanUseConfig())
			return string.Empty;

		SFP_RadioConfig cfg = SFP_RadioConfig.Get();
		if (!cfg)
			return string.Empty;

		SFP_RadioTrackCfg t = cfg.GetTrack(GetType(), index);
		if (!t)
			return string.Empty;
		return t.Name;
	}

	override float SFP_GetTrackDurationSecondsByIndex(int index)
	{
		if (!SFP_CanUseConfig())
			return 0.0;

		SFP_RadioConfig cfg = SFP_RadioConfig.Get();
		if (!cfg)
			return 0.0;

		SFP_RadioTrackCfg t = cfg.GetTrack(GetType(), index);
		if (!t)
			return 0.0;
		return t.DurationSeconds;
	}
}
//-------------------------------------------------------------------------------------------------------------------------
// КАССЕТА: Sidor Bunker (tracks are configured in JSON)
class SFP_radio_cassette_01 extends SFP_Cassette_Config {}
//---------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------
// КАССЕТА 02 (tracks are configured in JSON)
class SFP_radio_cassette_02 extends SFP_Cassette_Config {}
//---------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------
// КАССЕТА 03 (tracks are configured in JSON)
class SFP_radio_cassette_03 extends SFP_Cassette_Config {}
//---------------------------------------------------------------------------------------------------------------------------
// КАССЕТА: Clear Sky Bar (configured in JSON)
class SFP_radio_cassette_ClearSkyBar extends SFP_Cassette_Config {}
