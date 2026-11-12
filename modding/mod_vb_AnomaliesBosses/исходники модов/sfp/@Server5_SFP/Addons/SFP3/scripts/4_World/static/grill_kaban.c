
class SFP_Boar_grill_campfire extends CrashBase
{
	EffectSound 		m_AmbientSoundLoop;
	const string		LOOP_SOUND = "HeavyFire_SoundSet";
	FireplaceLight 	m_TreeLight;
	//----------------------------------------------------------
	override void EEInit()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
		}
	}
	//----------------------------------------------------------
	void SFP_Boar_grill_campfire()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			m_ParticleEfx = ParticleManager.GetInstance().PlayOnObject(ParticleList.SFP_FIRE01_GRILL, this, Vector(0, 0.200, 0));
			m_TreeLight = FireplaceLight.Cast( ScriptedLightBase.CreateLight( FireplaceLight, "0 0 0" ) );
			m_TreeLight.AttachOnMemoryPoint( this, "light" );
		}
	}
	//----------------------------------------------------------
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
	//----------------------------------------------------------
};
