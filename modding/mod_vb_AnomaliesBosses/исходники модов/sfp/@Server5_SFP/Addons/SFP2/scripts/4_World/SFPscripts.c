
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
class SFP_Mutant_Snork extends ZombieMaleBase
{
	override bool CanBeSkinned()
	{
		return true;
	}
	
}
class SFP_Mutant_Snork2 extends SFP_Mutant_Snork {};
class SFP_SFP_NPC_Invalid01_voice extends House
{
	
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "SFP_invalid_diag_SoundSet";
	
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
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
//---------------------------------------------------------------------------------------------
class SFP_Saharov_Voice extends House
{
	
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "SFP_Saharov_diag_SoundSet";
	
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
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
//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
class SFP_NPC_NaemFemale_Quest_voice extends House
{
	
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "SFP_NaemFemale_QuestDI_SoundSet";
	
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
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
//---------------------------------------------------------------------------------------------
class SFP_NPC_NaemFemale_Traider_voice extends House
{
	
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "SFP_NaemFemale_TraiderDI_SoundSet";
	
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
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
//---------------------------------------------------------------------------------------------
class SFP_NPC_Azot_voice extends House
{
	
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "SFP_Azot_diag_SoundSet";
	
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
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
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
class SFP_NPC_Barman_voice_v2 extends House
{
	
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "SFP_BarmenDiag_SoundSet";
	
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
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
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
class SFP_NPC_Sidor_voice_v2 extends House
{
	
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "SFP_SidorV2_diag_SoundSet";
	
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
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
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
class SFP_NPC_Sidor_voice_v1 extends House
{
	
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "SFP_Sidor_diag_SoundSet";
	
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
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
//---------------------------------------------------------------------------------------------------------------
class SFP_Pripat_street_light3 extends House
{
	SFP_LightLamp_Light 	m_Light;
	SFP_LightLamp_point 	m_TreeLight;
	SFP_LightLamp_Light 	m_Light2;
	SFP_LightLamp_point 	m_TreeLight2;
	SFP_LightLamp_Light 	m_Light3;
	SFP_LightLamp_point 	m_TreeLight3;
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "";
	
	void SFP_Pripat_street_light3()
	{
		if ( !GetGame().IsServer()  ||  !GetGame().IsMultiplayer() )
		{
			m_TreeLight = SFP_LightLamp_point.Cast( ScriptedLightBase.CreateLight( SFP_LightLamp_point, "0 0 0" ) );
			m_TreeLight.AttachOnMemoryPoint( this, "reflector" );
			m_Light = SFP_LightLamp_Light.Cast( ScriptedLightBase.CreateLight(SFP_LightLamp_Light, "0 0 0", 0) );
			m_Light.AttachOnMemoryPoint(this, "beamStart", "beamEnd");
			
			m_TreeLight2 = SFP_LightLamp_point.Cast( ScriptedLightBase.CreateLight( SFP_LightLamp_point, "0 0 0" ) );
			m_TreeLight2.AttachOnMemoryPoint( this, "reflector2" );
			m_Light2 = SFP_LightLamp_Light.Cast( ScriptedLightBase.CreateLight(SFP_LightLamp_Light, "0 0 0", 0) );
			m_Light2.AttachOnMemoryPoint(this, "beamStart2", "beamEnd2");
			
			m_TreeLight3 = SFP_LightLamp_point.Cast( ScriptedLightBase.CreateLight( SFP_LightLamp_point, "0 0 0" ) );
			m_TreeLight3.AttachOnMemoryPoint( this, "reflector3" );
			m_Light3 = SFP_LightLamp_Light.Cast( ScriptedLightBase.CreateLight(SFP_LightLamp_Light, "0 0 0", 0) );
			m_Light3.AttachOnMemoryPoint(this, "beamStart3", "beamEnd3");
		}
	}
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
		}
	}
	
	override void EEDelete(EntityAI parent)
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			if ( m_Light )
				m_Light.Destroy();
			if ( m_Light2 )
				m_Light2.Destroy();
			if ( m_Light3 )
				m_Light3.Destroy();
			if ( m_TreeLight )
				m_TreeLight.Destroy();
			if ( m_TreeLight2 )
				m_TreeLight2.Destroy();
			if ( m_TreeLight3 )
				m_TreeLight3.Destroy();
			if ( m_AmbientSoundLoop )
				SEffectManager.DestroyEffect(m_AmbientSoundLoop);
		}
	}
	
}
//------------------------------------------------------------------------------------------------
class SFP_Pripat_street_light2 extends House
{
	SFP_LightLamp_Light 	m_Light;
	SFP_LightLamp_point 	m_TreeLight;
	SFP_LightLamp_Light 	m_Light2;
	SFP_LightLamp_point 	m_TreeLight2;
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "";
	
	void SFP_Pripat_street_light2()
	{
		if ( !GetGame().IsServer()  ||  !GetGame().IsMultiplayer() )
		{
			m_TreeLight = SFP_LightLamp_point.Cast( ScriptedLightBase.CreateLight( SFP_LightLamp_point, "0 0 0" ) );
			m_TreeLight.AttachOnMemoryPoint( this, "reflector" );
			m_Light = SFP_LightLamp_Light.Cast( ScriptedLightBase.CreateLight(SFP_LightLamp_Light, "0 0 0", 0) );
			m_Light.AttachOnMemoryPoint(this, "beamStart", "beamEnd");
			
			m_TreeLight2 = SFP_LightLamp_point.Cast( ScriptedLightBase.CreateLight( SFP_LightLamp_point, "0 0 0" ) );
			m_TreeLight2.AttachOnMemoryPoint( this, "reflector2" );
			m_Light2 = SFP_LightLamp_Light.Cast( ScriptedLightBase.CreateLight(SFP_LightLamp_Light, "0 0 0", 0) );
			m_Light2.AttachOnMemoryPoint(this, "beamStart2", "beamEnd2");
		}
	}
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
		}
	}
	
	override void EEDelete(EntityAI parent)
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			if ( m_Light )
				m_Light.Destroy();
			if ( m_Light2 )
				m_Light2.Destroy();
			if ( m_TreeLight )
				m_TreeLight.Destroy();
			if ( m_TreeLight2 )
				m_TreeLight2.Destroy();
			if ( m_AmbientSoundLoop )
				SEffectManager.DestroyEffect(m_AmbientSoundLoop);
		}
	}
	
}
//------------------------------------------------------------------------------------------------
class SFP_Pripat_street_light1 extends House
{
	SFP_LightLamp_Light 	m_Light;
	SFP_LightLamp_point 	m_TreeLight;
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "";
	
	void SFP_Pripat_street_light1()
	{
		if ( !GetGame().IsServer()  ||  !GetGame().IsMultiplayer() )
		{
			m_TreeLight = SFP_LightLamp_point.Cast( ScriptedLightBase.CreateLight( SFP_LightLamp_point, "0 0 0" ) );
			m_TreeLight.AttachOnMemoryPoint( this, "reflector" );
			m_Light = SFP_LightLamp_Light.Cast( ScriptedLightBase.CreateLight(SFP_LightLamp_Light, "0 0 0", 0) );
			m_Light.AttachOnMemoryPoint(this, "beamStart", "beamEnd");
			
		}
	}
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
		}
	}
	
	override void EEDelete(EntityAI parent)
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			if ( m_Light )
				m_Light.Destroy();
			if ( m_TreeLight )
				m_TreeLight.Destroy();
			if ( m_AmbientSoundLoop )
				SEffectManager.DestroyEffect(m_AmbientSoundLoop);
		}
	}
	
}
//------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
class SFP_ANM_Krovosos extends ZombieMaleBase
{
	override bool CanBeSkinned()
	{
		return true;
	}
	
}
class SFP_ANM_Krovosos_Boloto extends SFP_ANM_Krovosos {};
class SFP_ANM_Krovosos_Dark extends SFP_ANM_Krovosos {};
class SFP_Piyavka extends SFP_ANM_Krovosos {};
//---------------------------------------------------------------------------------------------
class SFP_ANM_BIG_Krovosos extends ZombieMaleBase
{
	override bool CanBeSkinned()
	{
		return true;
	}
	
	protected float         m_Scale = 1.25;
	void SFP_ANM_BIG_Krovosos()
	{
		SetEventMask(EntityEvent.POSTFRAME);
        RegisterNetSyncVariableFloat("m_Scale");
	}
    override void EOnPostFrame(IEntity other, int extra) // | EntityEvent.POSTFRAME
    {
        vector mat[4];
        GetTransform(mat);

        mat[0] = mat[0].Normalized() * m_Scale;
        mat[1] = mat[1].Normalized() * m_Scale;
        mat[2] = mat[2].Normalized() * m_Scale;

        SetTransform(mat);
    }  
}
class SFP_ANM_BIG_Krovosos_Boloto extends SFP_ANM_BIG_Krovosos {};
class SFP_ANM_BIG_Krovosos_Dark extends SFP_ANM_BIG_Krovosos {};
class SFP_Barmen2_NPC_Voice extends House
{
	
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "SFP_BarmenDiag_SoundSet";
	
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
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
//---------------------------------------------------------------------------------------------
class SFP_NaemFemale_NPC_traider extends House
{
	
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "SFP_NaemFemale_TraiderDI_SoundSet";
	
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
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
//---------------------------------------------------------------------------------------------
class SFP_NaemFemale_NPC_quest extends House
{
	
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "SFP_NaemFemale_QuestDI_SoundSet";
	
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
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
//---------------------------------------------------------------------------------------------
class SFP_Saharov_NPC_Voice extends House
{
	
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "SFP_Saharov_diag_SoundSet";
	
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
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
//---------------------------------------------------------------------------------------------
/*
class SFP_Artefact_Monolit extends House
{
	Particle 			m_ParticleEfx;
	EffectSound 		m_AmbientSoundLoop;
	SFP_MonolitStone_Light	m_TreeLight
	const string		LOOP_SOUND = "SFP_AnomalyMonolitStone_SoundSet";
	
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
			m_ParticleEfx = ParticleManager.GetInstance().PlayOnObject(ParticleList.SFP_MONOLIT_ARTE_PART, this, Vector(0, 0, 0.000));
			m_TreeLight = SFP_MonolitStone_Light.Cast( ScriptedLightBase.CreateLight( SFP_MonolitStone_Light, "0 0 0" ) );
			m_TreeLight.AttachOnMemoryPoint( this, "monolit_light" );
		}
	}
	
	override void EEDelete(EntityAI parent)
	{
		if ( !GetGame().IsDedicatedServer() )
		{
	
			if ( m_AmbientSoundLoop )
				SEffectManager.DestroyEffect(m_AmbientSoundLoop);
			if ( m_ParticleEfx )
				m_ParticleEfx.Stop();
			if ( m_TreeLight )
				m_TreeLight.Destroy();
		}
	}
};*/
//---------------------------------------------------------

//------------------------------------
class SFP_AntiRad_Kvarc extends House //антирад кварц
{
	SFP_AntiRad_Kvarc_Light	m_TreeLight
	SFP_AntiRad_Kvarc_Light	m_TreeLight2
	SFP_AntiRad_Kvarc_Light	m_TreeLight3
	SFP_AntiRad_Kvarc_Light	m_TreeLight4
	SFP_AntiRad_Kvarc_Light	m_TreeLight5
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "";
	
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
			m_TreeLight = SFP_AntiRad_Kvarc_Light.Cast( ScriptedLightBase.CreateLight( SFP_AntiRad_Kvarc_Light, "0 0 0" ) );
			m_TreeLight.AttachOnMemoryPoint( this, "svet1" );
			m_TreeLight2 = SFP_AntiRad_Kvarc_Light.Cast( ScriptedLightBase.CreateLight( SFP_AntiRad_Kvarc_Light, "0 0 0" ) );
			m_TreeLight2.AttachOnMemoryPoint( this, "svet2" );
			m_TreeLight3 = SFP_AntiRad_Kvarc_Light.Cast( ScriptedLightBase.CreateLight( SFP_AntiRad_Kvarc_Light, "0 0 0" ) );
			m_TreeLight3.AttachOnMemoryPoint( this, "svet3" );
			m_TreeLight4 = SFP_AntiRad_Kvarc_Light.Cast( ScriptedLightBase.CreateLight( SFP_AntiRad_Kvarc_Light, "0 0 0" ) );
			m_TreeLight4.AttachOnMemoryPoint( this, "svet4" );
			m_TreeLight5 = SFP_AntiRad_Kvarc_Light.Cast( ScriptedLightBase.CreateLight( SFP_AntiRad_Kvarc_Light, "0 0 0" ) );
			m_TreeLight5.AttachOnMemoryPoint( this, "svet5" );
		
		}
	}
	
	override void EEDelete(EntityAI parent)
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			
			if ( m_AmbientSoundLoop )
				SEffectManager.DestroyEffect(m_AmbientSoundLoop);
			if ( m_TreeLight )
				m_TreeLight.Destroy();
			if ( m_TreeLight2 )
				m_TreeLight2.Destroy();
			if ( m_TreeLight3 )
				m_TreeLight3.Destroy();
			if ( m_TreeLight4 )
				m_TreeLight4.Destroy();
			if ( m_TreeLight5 )
				m_TreeLight5.Destroy();
		}
	}
};
//---------------------------------------------------------------------------------------------------------------------------
class SFP_AntiRad_Vorota extends House //антирад ворота
{
	Particle 			m_ParticleEfx;
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "SFP_AntiRadSpray_SoundSet";
	
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
			m_ParticleEfx = ParticleManager.GetInstance().PlayOnObject(ParticleList.SFP_ANTIRAD_VOROTA_PART, this, Vector(0, 0, 0));
		}
	}
	
	override void EEDelete(EntityAI parent)
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			
			if ( m_AmbientSoundLoop )
				SEffectManager.DestroyEffect(m_AmbientSoundLoop);
			if ( m_ParticleEfx )
				m_ParticleEfx.Stop();
		}
	}
};
//---------------------------------------------------------------------------------------------------------------------------
class SFP_Lampatka_static extends House
{
	Particle 			m_ParticleEfx;
	EffectSound 		m_AmbientSoundLoop;
	SFP_LAMPATKA_Light	m_TreeLight
	const string		LOOP_SOUND = "SFP_Lampatka_power_SoundSet";
	
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
			m_ParticleEfx = ParticleManager.GetInstance().PlayOnObject(ParticleList.SFP_LAMPATKA_FIRE, this, Vector(-0.1, 0.090, 0.000));
			m_TreeLight = SFP_LAMPATKA_Light.Cast( ScriptedLightBase.CreateLight( SFP_LAMPATKA_Light, "0 0 0" ) );
			m_TreeLight.AttachOnMemoryPoint( this, "Fire_lampatka" );
		}
	}
	
	override void EEDelete(EntityAI parent)
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			
			if ( m_AmbientSoundLoop )
				SEffectManager.DestroyEffect(m_AmbientSoundLoop);
			if ( m_ParticleEfx )
				m_ParticleEfx.Stop();
			if ( m_TreeLight )
				m_TreeLight.Destroy();
		}
	}
};
//---------------------------------------------------------------------------------------------------------------------------
class SFP_Stalker_Mangal_voice extends House
{
	Particle 			m_ParticleEfx;
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "SFP_Mangal_maso_shipenie_SoundSet";
	
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
			m_ParticleEfx = ParticleManager.GetInstance().PlayOnObject(ParticleList.SFP_SMOKE_MANGAL_PART, this, Vector(-0.050, 0.288, -0.506));
		}
	}
	
	override void EEDelete(EntityAI parent)
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			
			if ( m_AmbientSoundLoop )
				SEffectManager.DestroyEffect(m_AmbientSoundLoop);
			if ( m_ParticleEfx )
				m_ParticleEfx.Stop();
			
		}
	}
};
//-------------------------------------------------------------------------------------------
class SFP_GologenLamp_Ice extends House
{
	SFP_GologenLampIceLight 	m_Light;
	SFP_GologenLampIceLightZvezda	m_TreeLight
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "";
	
	void SFP_GologenLamp_Ice()
	{
		if ( !GetGame().IsServer()  ||  !GetGame().IsMultiplayer() )
		{
			m_TreeLight = SFP_GologenLampIceLightZvezda.Cast( ScriptedLightBase.CreateLight( SFP_GologenLampIceLightZvezda, "0 0 0" ) );
			m_TreeLight.AttachOnMemoryPoint( this, "reflector" );
			m_Light = SFP_GologenLampIceLight.Cast( ScriptedLightBase.CreateLight(SFP_GologenLampIceLight, "0 0 0", 0) );
			m_Light.AttachOnMemoryPoint(this, "beamStart", "beamEnd");
			
		}
	}
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
		}
	}
	
	override void EEDelete(EntityAI parent)
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			if ( m_Light )
				m_Light.Destroy();
			if ( m_AmbientSoundLoop )
				SEffectManager.DestroyEffect(m_AmbientSoundLoop);
		}
	}
	
}
//---------------------------------------------------------------------------------------------------------------------------
class SFP_GologenLamp extends House
{
	SFP_GologenLampLight 	m_Light;
	SFP_GologenLampLightZvezda	m_TreeLight
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "";
	
	void SFP_GologenLamp()
	{
		if ( !GetGame().IsServer()  ||  !GetGame().IsMultiplayer() )
		{
			m_TreeLight = SFP_GologenLampLightZvezda.Cast( ScriptedLightBase.CreateLight( SFP_GologenLampLightZvezda, "0 0 0" ) );
			m_TreeLight.AttachOnMemoryPoint( this, "reflector" );
			m_Light = SFP_GologenLampLight.Cast( ScriptedLightBase.CreateLight(SFP_GologenLampLight, "0 0 0", 0) );
			m_Light.AttachOnMemoryPoint(this, "beamStart", "beamEnd");
			
		}
	}
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
		}
	}
	
	override void EEDelete(EntityAI parent)
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			if ( m_Light )
				m_Light.Destroy();
			if ( m_AmbientSoundLoop )
				SEffectManager.DestroyEffect(m_AmbientSoundLoop);
		}
	}
	
}
//---------------------------------------------------------------------------------------------
class SFP_ShawermaGrill extends House
{
	
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "SFP_GrillMasoSound_SoundSet";
	
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
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
//---------------------------------------------------------------------------------------------
class SFP_NPC_Lesnik_Voice extends House
{
	
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "SFP_Forester_SoundSet";
	
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
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
//---------------------------------------------------------------------------------------------
class SFP_NCP_Sidor_Voice extends House
{
	
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "SFP_Sidor_diag_SoundSet";
	
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
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
//---------------------------------------------------------------------------------------------------------------
class SFP_NCP_Barmen_voice extends House
{
	
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "SFP_BarmenDiag_SoundSet";
	
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
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
//---------------------------------------------------------------------------------------------------------------
class SFP_USSR_Radio_BarSound100rengen extends House
{
	
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "SFP_RadioPlay100rengen_SoundSet";
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
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
//---------------------------------------------------------------------------------------------------------------
class SFP_USSR_Radio_HolodBar extends House
{
	
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "SFP_RadioPlayHolodBar_SoundSet";
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
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
//---------------------------------------------------------------------------------------------------------------
class SFP_VinylPlayer_KISH_Sound extends House
{
	
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "SFP_VinylRecord_KISH_SoundSet";
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
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
//------------------------------------------------------------------------------------------------------------------------------------------------------ворота
//---------------------------------------------------------------------------------------------------------------
class SFP_USSR_Radio_SidorBunker extends House
{
	
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "SFP_RadioPlaySidorBunker_SoundSet";
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
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
//------------------------------------------------------------------------------------------------------------------------------------------------------ворота

class SFP_StalkerGitara_NPC extends House
{
	
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "SFP_GitaraNPC_SoundSet";
	
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
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
class SFP_StalkerGitaraP_NPC extends SFP_StalkerGitara_NPC {};
class SFP_StalkerGitaraP2_NPC extends SFP_StalkerGitara_NPC {};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
class SFP_USSR_Radio_Sound extends House
{
	
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "SFP_RadioPlayList_SoundSet";
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
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
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
class SFP_Bankomat extends House
{
	
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "SFP_BankomatWork_SoundSet";
	
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
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
//----------------------------------------------------------------------------------------------------------------------------------------------------------
/*class SFP_LightLamp_stolb extends House //свет прожектор
{
	SFP_LightLamp_Light 	m_Light;
	static vector 	m_LightLocalPosition 	= "-0.004 7.418 -1.268"; // We can't use GetMemoryPointPos() on this object, so we need to remember light position like this instead.
	static vector 	m_LightLocalOrientation = "-90 -90 30";
	void SFP_LightLamp_stolb()
	{
		
		if ( !GetGame().IsDedicatedServer() )
		{
			m_Light = SFP_LightLamp_Light.Cast( ScriptedLightBase.CreateLight( SFP_LightLamp_Light, "0 0 0") );
			m_Light.AttachOnObject(this, m_LightLocalPosition, m_LightLocalOrientation);
			
		}
	}
	
};*/
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class SFP_LightLamp_stolb extends House
{
	SFP_LightLamp_Light 	m_Light;
	SFP_LightLamp_point 	m_TreeLight;
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "";
	
	void SFP_LightLamp_stolb()
	{
		if ( !GetGame().IsServer()  ||  !GetGame().IsMultiplayer() )
		{
			m_TreeLight = SFP_LightLamp_point.Cast( ScriptedLightBase.CreateLight( SFP_LightLamp_point, "0 0 0" ) );
			m_TreeLight.AttachOnMemoryPoint( this, "reflector" );
			m_Light = SFP_LightLamp_Light.Cast( ScriptedLightBase.CreateLight(SFP_LightLamp_Light, "0 0 0", 0) );
			m_Light.AttachOnMemoryPoint(this, "beamStart", "beamEnd");
			
		}
	}
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
		}
	}
	
	override void EEDelete(EntityAI parent)
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			if ( m_Light )
				m_Light.Destroy();
			if ( m_TreeLight )
				m_TreeLight.Destroy();
			if ( m_AmbientSoundLoop )
				SEffectManager.DestroyEffect(m_AmbientSoundLoop);
		}
	}
	
}
class SFP_LightLamp_WodStolb extends SFP_LightLamp_stolb{};
class SFP_LightLamp_WodStolb2 extends SFP_LightLamp_stolb{};
class SFP_LightLamp_Mini extends SFP_LightLamp_stolb{};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class SFP_LightLamp_Only extends House//сама лампа, идет свет не уголовой а рассеянный
{
	SFP_LightLampOnly_point 	m_TreeLight;
	
	
	void SFP_LightLamp_Only()
	{
		if ( !GetGame().IsServer()  ||  !GetGame().IsMultiplayer() )
		{
			m_TreeLight = SFP_LightLampOnly_point.Cast( ScriptedLightBase.CreateLight( SFP_LightLampOnly_point, "0 0 0" ) );
			m_TreeLight.AttachOnMemoryPoint( this, "reflector" );
		}
	}
	
	override void EEDelete(EntityAI parent)
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			if ( m_TreeLight )
				m_TreeLight.Destroy();
			
		}
	}
	
}
class SFP_LightLamp_Only1 extends SFP_LightLamp_Only{};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class SFP_LightLampLenin extends House//сама лампа, идет свет не уголовой а рассеянный
{
	SFP_LightLampLenin_point 	m_TreeLight;
	
	
	void SFP_LightLampLenin()
	{
		if ( !GetGame().IsServer()  ||  !GetGame().IsMultiplayer() )
		{
			m_TreeLight = SFP_LightLampLenin_point.Cast( ScriptedLightBase.CreateLight( SFP_LightLampLenin_point, "0 0 0" ) );
			m_TreeLight.AttachOnMemoryPoint( this, "reflector" );
		}
	}
	
	override void EEDelete(EntityAI parent)
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			if ( m_TreeLight )
				m_TreeLight.Destroy();
			
		}
	}
	
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------
class SFP_Svetofor_RED extends House
{
	SFP_svetoforRED_Light 	m_Light;
	SFP_svetoforRED_LightZvezda 	m_TreeLight;
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "";
	
	void SFP_Svetofor_RED()
	{
		if ( !GetGame().IsServer()  ||  !GetGame().IsMultiplayer() )
		{
			m_TreeLight = SFP_svetoforRED_LightZvezda.Cast( ScriptedLightBase.CreateLight( SFP_svetoforRED_LightZvezda, "0 0 0" ) );
			m_TreeLight.AttachOnMemoryPoint( this, "rbeamStart" );
			m_Light = SFP_svetoforRED_Light.Cast( ScriptedLightBase.CreateLight(SFP_svetoforRED_Light, "0 0 0", 0) );
			m_Light.AttachOnMemoryPoint(this, "rbeamStart", "rbeamEnd");
			
		}
	}
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
		}
	}
	
	override void EEDelete(EntityAI parent)
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			if ( m_Light )
				m_Light.Destroy();
			if ( m_TreeLight )
				m_TreeLight.Destroy();
			if ( m_AmbientSoundLoop )
				SEffectManager.DestroyEffect(m_AmbientSoundLoop);
		}
	}
	
}
class SFP_Svetofor_GREEN extends House
{
	SFP_svetoforGREEN_Light 	m_Light;
	SFP_svetoforGREEN_LightZvezda 	m_TreeLight;
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "";
	
	void SFP_Svetofor_GREEN()
	{
		if ( !GetGame().IsServer()  ||  !GetGame().IsMultiplayer() )
		{
			m_TreeLight = SFP_svetoforGREEN_LightZvezda.Cast( ScriptedLightBase.CreateLight( SFP_svetoforGREEN_LightZvezda, "0 0 0" ) );
			m_TreeLight.AttachOnMemoryPoint( this, "GbeamStart" );
			m_Light = SFP_svetoforGREEN_Light.Cast( ScriptedLightBase.CreateLight(SFP_svetoforGREEN_Light, "0 0 0", 0) );
			m_Light.AttachOnMemoryPoint(this, "GbeamStart", "GbeamEnd");
			
		}
	}
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
		}
	}
	
	override void EEDelete(EntityAI parent)
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			if ( m_Light )
				m_Light.Destroy();
			if ( m_TreeLight )
				m_TreeLight.Destroy();
			if ( m_AmbientSoundLoop )
				SEffectManager.DestroyEffect(m_AmbientSoundLoop);
		}
	}	
}
class SFP_Old_TV_Sound extends House
{
	SFP_OldTV_Light 	m_Light;
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "SFP_TV_Glith_SoundSet";
	
	void SFP_Old_TV_Sound()
	{
		if ( !GetGame().IsServer()  ||  !GetGame().IsMultiplayer() )
		{
			m_Light = SFP_OldTV_Light.Cast( ScriptedLightBase.CreateLight(SFP_OldTV_Light, "0 0 0", 0) );
			m_Light.AttachOnMemoryPoint(this, "TVbeamStart", "TVbeamEnd");
			
		}
	}
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
		}
	}
	
	override void EEDelete(EntityAI parent)
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			if ( m_Light )
				m_Light.Destroy();
			if ( m_AmbientSoundLoop )
				SEffectManager.DestroyEffect(m_AmbientSoundLoop);
		}
	}
	
}
class SFP_Old_TV_ps1 extends House
{
	SFP_OldTV_Light 	m_Light;
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "SFP_TV_gameSound_SoundSet";
	
	void SFP_Old_TV_ps1()
	{
		if ( !GetGame().IsServer()  ||  !GetGame().IsMultiplayer() )
		{
			m_Light = SFP_OldTV_Light.Cast( ScriptedLightBase.CreateLight(SFP_OldTV_Light, "0 0 0", 0) );
			m_Light.AttachOnMemoryPoint(this, "TVbeamStart", "TVbeamEnd");
			
		}
	}
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
		}
	}
	
	override void EEDelete(EntityAI parent)
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			if ( m_Light )
				m_Light.Destroy();
			if ( m_AmbientSoundLoop )
				SEffectManager.DestroyEffect(m_AmbientSoundLoop);
		}
	}
	
}
class SFP_Old_TV extends House
{
	SFP_OldTV_Light 	m_Light;
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "";
	
	void SFP_Old_TV()
	{
		if ( !GetGame().IsServer()  ||  !GetGame().IsMultiplayer() )
		{
			m_Light = SFP_OldTV_Light.Cast( ScriptedLightBase.CreateLight(SFP_OldTV_Light, "0 0 0", 0) );
			m_Light.AttachOnMemoryPoint(this, "TVbeamStart", "TVbeamEnd");
			
		}
	}
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
		}
	}
	
	override void EEDelete(EntityAI parent)
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			if ( m_Light )
				m_Light.Destroy();
			if ( m_AmbientSoundLoop )
				SEffectManager.DestroyEffect(m_AmbientSoundLoop);
		}
	}
	
}