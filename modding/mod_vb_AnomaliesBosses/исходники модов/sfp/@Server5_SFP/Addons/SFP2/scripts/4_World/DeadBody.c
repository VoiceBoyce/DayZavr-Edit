
//class FH_Structures_goloVorotaLong /*класснейм обьекта*/ extends House /*родительский класс */
//{
//	private SoundOnVehicle	m_LoopSoundEntity; //не трогать
//	void FH_Structures_goloVorotaLong() // void *класснейм 
//	{
//		SoundFireLightStart();		//запуск скрипта звука
//	}
//	void SoundFireLightStart() // сам скрипт звук
//	{
//		m_LoopSoundEntity = PlaySoundLoop("FH_goloWall_Sound", 100); //звук, 50 по моему это громкость
//	}
//}
//-------------------------------------------------------------------------------------------------------------------------------------------пустой звук кар кар без мух
class SFP_Crow_sound extends House
{
	//Particle 			m_ParticleEfx;
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "SFP_Crow_KarKarOdin_SoundSet";
	
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
			//m_ParticleEfx = ParticleManager.GetInstance().PlayOnObject(ParticleList.ENV_SWARMING_FLIES, this, Vector(0, 0, -0.9));
		}
	}
	
	override void EEDelete(EntityAI parent)
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			
			if ( m_AmbientSoundLoop )
				SEffectManager.DestroyEffect(m_AmbientSoundLoop);
		}
	}
};
class SFP_DeadBodyBasic_SeatCrow extends House
{
	//Particle 			m_ParticleEfx;
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "SFP_DeadBodyCrowMyx_SoundSet";
	
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
			//m_ParticleEfx = ParticleManager.GetInstance().PlayOnObject(ParticleList.ENV_SWARMING_FLIES, this, Vector(0, 0, -0.9));
		}
	}
	
	override void EEDelete(EntityAI parent)
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			
			if ( m_AmbientSoundLoop )
				SEffectManager.DestroyEffect(m_AmbientSoundLoop);
		}
	}
};
class SFP_DeadBodyBasic2_SeatCrow extends SFP_DeadBodyBasic_SeatCrow{};
class SFP_DeadBodyKuski_Crow extends SFP_DeadBodyBasic_SeatCrow{};
class SFP_DeadBodyRazbrosCrow extends SFP_DeadBodyBasic_SeatCrow{};
//-------------------------------------------------------------------------------------------------------------------------------------------Подрубил звук только мух
class SFP_DeadBodySkined_Rope extends House
{
	//Particle 			m_ParticleEfx;
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "SFP_Myx_roi_SoundSet";
	
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
			//m_ParticleEfx = ParticleManager.GetInstance().PlayOnObject(ParticleList.ENV_SWARMING_FLIES, this, Vector(0.000, 1.298, -0.055));
		}
	}
	
	override void EEDelete(EntityAI parent)
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			
			if ( m_AmbientSoundLoop )
				SEffectManager.DestroyEffect(m_AmbientSoundLoop);
		}
	}
};
class SFP_DeadBodyKuski extends SFP_DeadBodySkined_Rope{};
class SFP_DeadBodyLeza extends SFP_DeadBodySkined_Rope{};	
class SFP_DeadBodySkined_Leza extends SFP_DeadBodySkined_Rope{};	
class SFP_OldBarrelDeadBody_2 extends SFP_DeadBodySkined_Rope{};
class SFP_DeadBodyRazbros extends SFP_DeadBodySkined_Rope{};
//-------------------------------------------------------------------------------------------------------------------------------------------подрублен дым
class SFP_DeadBodyBurned1_smoke extends House
{
	Particle 			m_ParticleEfx;
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			
			m_ParticleEfx = ParticleManager.GetInstance().PlayOnObject(ParticleList.SMOKING_CAR_ENGINE, this, Vector(0, 0, 0));
		}
	}
	
};
class SFP_DeadBodyBurned2_smoke extends SFP_DeadBodyBurned1_smoke{};

class SFP_OldBarrelDeadBody_1Steam extends House
{
	Particle 			m_ParticleEfx;
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			
			m_ParticleEfx = ParticleManager.GetInstance().PlayOnObject(ParticleList.HATCHBACK_COOLANT_OVERHEATING, this, Vector(0, 0, 0));
		}
	}
	override void EEDelete( EntityAI parent )
	{
		if ( m_ParticleEfx )
			m_ParticleEfx.Stop();
		
	}
};