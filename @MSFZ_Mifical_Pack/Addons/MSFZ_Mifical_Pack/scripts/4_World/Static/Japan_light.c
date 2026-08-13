class MSFZ_Japan_light extends House
{
	protected Particle 		m_Particle;
	EffectSound 		m_AmbientSoundLoop;
	Japan_light_light	m_TreeLight
	const string		LOOP_SOUND = "Japan_light_SoundSet";
	void MSFZ_Japan_light()
	{				
		if ( !GetGame().IsMultiplayer() || GetGame().IsClient() )
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(SpawnParticle, 100, false);
			m_TreeLight = Japan_light_light.Cast( ScriptedLightBase.CreateLight( Japan_light_light, "0 0 0" ) );
			m_TreeLight.AttachOnMemoryPoint( this, "Japan_light_axis" );
			PlaySoundSetLoop( m_AmbientSoundLoop, LOOP_SOUND, 0, 0 );
		}
	}
	void ~MSFZ_Japan_light(){}
	override void EEDelete(EntityAI parent)
	{
		if ( !GetGame().IsMultiplayer() || GetGame().IsClient() )
		{
			if ( m_Particle )
				m_Particle.Stop();
			if ( m_TreeLight )
				m_TreeLight.Destroy();
			if ( m_AmbientSoundLoop )
				SEffectManager.DestroyEffect(m_AmbientSoundLoop);
		}
	}
	void SpawnParticle()
	{
		m_Particle = Particle.PlayOnObject(ParticleList.MSFZ_Japan_light, this);
	}
};