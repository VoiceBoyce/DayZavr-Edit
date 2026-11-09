// Грозовая туча Тролля: из неё выходят Ричары.
// Сервер (MSFZ_Mifical_Pack_Server, Mutants/Troll.c) ставит тучу рядом с игроком, при появлении
// каждого Ричара вызывает MSFZ_Strike(), перед удалением — MSFZ_Dissipate().
// Клиент показывает туман с молниями, гул грозы, разряды, гром и вспышку света.
// Частицы и звуки взяты из аномалии «Гроза» (OOTS), туча уменьшена с 35 до 14 м.
// Сама туча не наносит урона: это только портал.

// Короткая вспышка молнии
class MSFZ_TrollStormFlash extends PointLightBase
{
	void MSFZ_TrollStormFlash()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(35);
		SetBrightnessTo(8);
		SetDiffuseColor(0.65, 0.75, 1.0);
		SetAmbientColor(0.65, 0.75, 1.0);
		SetFlareVisible(false);
		SetCastShadow(false);
		SetFadeOutTime(0.3);
		SetLifetime(0.25);
	}
};

class MSFZ_TrollStorm extends House
{
	protected const string MSFZ_SOUND_LOOP    = "MSFZ_TrollStorm_Loop_SoundSet";
	protected const string MSFZ_SOUND_STRIKE  = "MSFZ_TrollStorm_Strike_SoundSet";
	protected const string MSFZ_SOUND_THUNDER = "MSFZ_TrollStorm_Thunder_SoundSet";
	protected const int    MSFZ_ACTIVE_MS     = 2500; // сколько горят разряды после последнего удара, мс
	protected const int    MSFZ_STRIKES_MAX   = 63;   // счётчик ударов идёт по кругу

	// Синхронизируется с сервера
	protected int  m_MSFZ_Strikes;      // сколько раз ударила молния (вышел Ричар)
	protected bool m_MSFZ_Dissipating;  // туча рассеивается

	// Только клиент
	protected int         m_MSFZ_StrikesShown = -1; // -1: туча на клиенте ещё не запущена
	protected bool        m_MSFZ_DissipateShown;
	protected Particle    m_MSFZ_IdleFx;
	protected Particle    m_MSFZ_ActiveFx;
	protected EffectSound m_MSFZ_LoopSound;

	void MSFZ_TrollStorm()
	{
		RegisterNetSyncVariableInt("m_MSFZ_Strikes", 0, MSFZ_STRIKES_MAX);
		RegisterNetSyncVariableBool("m_MSFZ_Dissipating");

		// К этому моменту клиент получит начальные значения синхронизации
		if (!GetGame().IsDedicatedServer())
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(MSFZ_ClientStart, 50, false);
	}

	// ---------- Сервер ----------

	// Удар молнии: из тучи вышел Ричар
	void MSFZ_Strike()
	{
		m_MSFZ_Strikes++;
		if (m_MSFZ_Strikes > MSFZ_STRIKES_MAX)
			m_MSFZ_Strikes = 1;
		SetSynchDirty();
	}

	// Туча перестаёт клубиться и тихо уходит, затем сервер её удаляет
	void MSFZ_Dissipate()
	{
		m_MSFZ_Dissipating = true;
		SetSynchDirty();
	}

	// ---------- Клиент ----------

	protected void MSFZ_ClientStart()
	{
		if (IsSetForDeletion())
			return;

		m_MSFZ_StrikesShown = m_MSFZ_Strikes;

		// игрок подошёл, когда туча уже уходит
		if (m_MSFZ_Dissipating)
		{
			m_MSFZ_DissipateShown = true;
			return;
		}

		m_MSFZ_IdleFx = Particle.PlayOnObject(ParticleList.MSFZ_TROLLSTORM_IDLE, this);
		PlaySoundSetLoop(m_MSFZ_LoopSound, MSFZ_SOUND_LOOP, 1.5, 3.0);

		// туча только что поднялась: разряд и гром сразу, а не тем, кто подошёл позже
		if (m_MSFZ_Strikes == 0)
			MSFZ_PlayStrike(true);
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();

		// туча на клиенте ещё не запущена: MSFZ_ClientStart возьмёт текущие значения сам
		if (m_MSFZ_StrikesShown < 0)
			return;

		if (m_MSFZ_Strikes != m_MSFZ_StrikesShown)
		{
			m_MSFZ_StrikesShown = m_MSFZ_Strikes;
			// гром через удар, иначе раскаты сливаются в сплошной гул
			MSFZ_PlayStrike(m_MSFZ_Strikes % 2 == 1);
		}

		if (m_MSFZ_Dissipating && !m_MSFZ_DissipateShown)
		{
			m_MSFZ_DissipateShown = true;
			MSFZ_StopEffects();
		}
	}

	protected void MSFZ_PlayStrike(bool thunder)
	{
		vector pos = GetPosition();
		ScriptCallQueue queue = GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY);

		// Разряды горят, пока идут удары, и гаснут через MSFZ_ACTIVE_MS после последнего.
		// Треск разрядов (10 с) запускается один раз на серию ударов.
		queue.Remove(MSFZ_StopActive);
		if (!m_MSFZ_ActiveFx)
		{
			m_MSFZ_ActiveFx = Particle.PlayOnObject(ParticleList.MSFZ_TROLLSTORM_ACTIVE, this);
			EffectSound strike = SEffectManager.PlaySound(MSFZ_SOUND_STRIKE, pos);
			if (strike)
				strike.SetAutodestroy(true);
		}
		queue.CallLater(MSFZ_StopActive, MSFZ_ACTIVE_MS, false);

		if (thunder)
		{
			EffectSound thunderSound = SEffectManager.PlaySound(MSFZ_SOUND_THUNDER, pos);
			if (thunderSound)
				thunderSound.SetAutodestroy(true);
		}

		ScriptedLightBase.CreateLight(MSFZ_TrollStormFlash, pos + "0 6 0");
	}

	protected void MSFZ_StopActive()
	{
		if (m_MSFZ_ActiveFx)
		{
			m_MSFZ_ActiveFx.Stop();
			m_MSFZ_ActiveFx = null;
		}
	}

	protected void MSFZ_StopEffects()
	{
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(MSFZ_StopActive);
		MSFZ_StopActive();

		if (m_MSFZ_IdleFx)
		{
			m_MSFZ_IdleFx.Stop();
			m_MSFZ_IdleFx = null;
		}

		// гул затихает плавно (fade out 3 с), эффект с автоудалением уничтожится сам
		if (m_MSFZ_LoopSound)
			StopSoundSet(m_MSFZ_LoopSound);
	}

	override void EEDelete(EntityAI parent)
	{
		if (!GetGame().IsDedicatedServer())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(MSFZ_ClientStart);
			MSFZ_StopEffects();
		}

		super.EEDelete(parent);
	}
};
