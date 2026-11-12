//New russian helicopter crash site
/*class Wreck_Mi8_Crashed extends CrashBase
{
	void Wreck_Mi8_Crashed()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			m_ParticleEfx = ParticleManager.GetInstance().PlayOnObject(ParticleList.SMOKING_HELI_WRECK, this, Vector(2, 0, -5));SFP_ANOMALY_MOLNYA_Part SFP_OGNEMET_PART
		}
	}
}
*/
class SFP_PartPrizraki extends CrashBase
{
	void SFP_PartPrizraki()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			m_ParticleEfx = ParticleManager.GetInstance().PlayOnObject(ParticleList.SFP_PSI_PRIZRAKI, this, Vector(0, 0, 0));
		}
	}
}
class SFP_PartPrizraki_mutant extends CrashBase
{
	void SFP_PartPrizraki_mutant()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			m_ParticleEfx = ParticleManager.GetInstance().PlayOnObject(ParticleList.SFP_PSI_PRIZRAKI_MUTANT, this, Vector(0, 0, 0));
		}
	}
}
class SFP_PartCrowFly extends CrashBase
{
	void SFP_PartCrowFly()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			m_ParticleEfx = ParticleManager.GetInstance().PlayOnObject(ParticleList.SFP_CROW_FLY_PART, this, Vector(0, 0, 0));
		}
	}
}
class SFP_Siga_smoke extends CrashBase
{
	void SFP_Siga_smoke()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			m_ParticleEfx = ParticleManager.GetInstance().PlayOnObject(ParticleList.SMOKING_BARREL_STEAM_SMALL, this, Vector(0, 0.165, 0));
		}
	}
	
	
	override void EEDelete( EntityAI parent )
	{
		if ( m_ParticleEfx )
			m_ParticleEfx.Stop();
	
	}
}