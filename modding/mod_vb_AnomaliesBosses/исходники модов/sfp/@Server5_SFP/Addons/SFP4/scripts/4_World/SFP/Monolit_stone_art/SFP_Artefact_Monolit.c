// ============================================================================
// МОНОЛИТ — артефакт, который «мутит разум»
//
// Важно по твоим требованиям:
//  - ТОЛЬКО этот класс (SFP_Artefact_Monolit) читает MonolitStoneSFP.json
//  - Type2 (SFP_Artefact_Monolit_T2) остаётся "как есть" и НЕ зависит от JSON
//
// Клиентские эффекты:
//   1) Партикл:  ParticleList.SFP_MONOLIT_ARTE_PART (как в ваниле у разбитого вертолёта)
//   2) Свет:     SFP_MonolitStoneLight (как ванильный костёр/большой огонь)
//   3) PPE+Звук: персонально для игрока через SFP_MonolitStoneClientManager
//
// Свет ДОЛЖЕН идти из memory point "monolit_light" (SFP_MonolitStone_Config.LIGHT_MEMORYPOINT)
// ============================================================================

class SFP_Artefact_Monolit extends House
{
	#ifndef SERVER
	protected Particle m_ParticleEfx;
	protected SFP_MonolitStoneLight m_Light;
	#endif

	override void EEInit()
	{
		super.EEInit();

		// Сервер: гарантируем создание/чтение JSON (по требованию)
		if (GetGame().IsServer())
		{
			SFP_MonolitStoneSFPSettings.Get();
		}

		#ifndef SERVER
		// Регистрируемся в клиентском менеджере (он считает дистанцию и даёт PPE/звук игроку)
		SFP_MonolitStoneClientManager.GetInstance().RegisterStone(this);


		string memPointName = SFP_MonolitStone_Config.LIGHT_MEMORYPOINT;
		// Партикл
		// Ванильный паттерн (см. Wreck_Mi8_Crashed): PlayOnObject(ParticleList.CONST, this, localPos)
		// Важно: использовать CONST из ParticleList (а не GetParticleIDByName), чтобы не зависеть от filename
		if (!m_ParticleEfx && !GetGame().IsDedicatedServer())
		{
			vector localPos = Vector(0, 0, 0);

			if (MemoryPointExists(memPointName))
				localPos = GetMemoryPointPos(memPointName);

			m_ParticleEfx = ParticleManager.GetInstance().PlayOnObject(ParticleList.SFP_MONOLIT_ARTE_PART, this, localPos);
		}

		// Свет — делаем 1:1 как ванильный Bonfire:
		// CreateLight(0,0,0) -> AttachOnMemoryPoint -> FadeBrightness/Radius to значения
		if (!m_Light)
		{
			m_Light = SFP_MonolitStoneLight.Cast(ScriptedLightBase.CreateLight(SFP_MonolitStoneLight, Vector(0,0,0), 1));

			if (!m_Light)
			{
				#ifdef DIAG_DEVELOPER
				Print("[SFP Monolit] CreateLight(SFP_MonolitStoneLight) failed for " + GetType());
				#endif
			}
			else
			{


				if (MemoryPointExists(memPointName))
				{
					m_Light.AttachOnMemoryPoint(this, memPointName);
				}
				else
				{
					#ifdef DIAG_DEVELOPER
				Print("[SFP Monolit] Memory point not found on model: " + memPointName + " for " + GetType());
				#endif
					m_Light.AttachOnObject(this, Vector(0,0,0), Vector(0,0,0));
				}

				// Радиус света берём из JSON ТОЛЬКО для этого типа камня
				float targetRadius = SFP_MonolitStone_Config.LIGHT_RADIUS;
				SFP_MonolitStoneSFPSettings st = SFP_MonolitStoneSFPSettings.Get();
				if (st && st.m_LightRadius_M > 0.1)
					targetRadius = st.m_LightRadius_M;

				m_Light.FadeBrightnessTo(SFP_MonolitStone_Config.LIGHT_BRIGHTNESS, 0);
				m_Light.FadeRadiusTo(targetRadius, 0);
			}
		}
		#endif
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);

		#ifndef SERVER
		SFP_MonolitStoneClientManager.GetInstance().UnregisterStone(this);

		if (m_Light)
		{
			m_Light.FadeOut();
			m_Light = null;
		}

		if (m_ParticleEfx)
		{
			m_ParticleEfx.Stop();
			m_ParticleEfx = null;
		}
		#endif
	}

	#ifndef SERVER
	// Точка, от которой считается дистанция (предпочтительно memory point "monolit_light")
	vector GetEffectOriginWS()
	{
		string memPointName = SFP_MonolitStone_Config.LIGHT_MEMORYPOINT;
		if (MemoryPointExists(memPointName))
			return ModelToWorld(GetMemoryPointPos(memPointName));
		return GetPosition();
	}
	#endif
}