class RF_ESCAPE_FROM_DAYZ_Roadflare_Static: Roadflare {

    override bool CanPutInCargo (EntityAI parent) { return false; }
    override bool CanPutIntoHands(EntityAI parent) { return false; }
}

modded class Roadflare {

    // MARK: - Override Methods --------------------------------------------------------------------------

    override void UpdateActiveParticles() {
        string flareType = this.GetType();
        if (!((flareType == "RF_ESCAPE_FROM_DAYZ_Roadflare") || (flareType == "RF_ESCAPE_FROM_DAYZ_Roadflare_Static"))) {
            super.UpdateActiveParticles();
            return;
        }

		if (GetGame().IsDedicatedServer())
			return;
		
		switch (m_BurningState) {
			case RoadflareBurningState.NOT_BURNING: {
				DestroyAllParticles();
				break;
            }
				
			case RoadflareBurningState.INITIAL_BURN: {
				if (!m_ParInitialFire) {
					DestroyAllParticles();
					m_ParInitialFire = ParticleManager.GetInstance().PlayOnObject(ParticleList.RF_ESCAPE_FROM_DAYZ_ROADFLARE_BURNING_INIT, this, m_FlameLocalPos);
					m_ParInitialFire.SetWiggle(10, 0.3);
				}
				break;
            }
				
			case RoadflareBurningState.MAIN_BURN: {
				if (!m_ParMainFire) {
					m_ParMainFire = ParticleManager.GetInstance().PlayOnObject(ParticleList.RF_ESCAPE_FROM_DAYZ_ROADFLARE_BURNING_MAIN, this, m_FlameLocalPos);
					m_ParMainFire.SetWiggle(7, 0.3);
				}
				DestroyParticleEx(m_ParInitialFire);
				break;	
            }
				
			case RoadflareBurningState.FINAL_BURN: {
				if (!m_ParFinalFire) {
					DestroyAllParticles();
					m_ParFinalFire = ParticleManager.GetInstance().PlayOnObject(ParticleList.RF_ESCAPE_FROM_DAYZ_ROADFLARE_BURNING_ENDING, this, m_FlameLocalPos);
					m_ParFinalFire.SetWiggle(4, 0.3);
				}
				break;
            }
				
			case RoadflareBurningState.SMOKE_ONLY: {
				if (!m_ParJustSmoke) {
					DestroyAllParticles();
					m_ParJustSmoke = ParticleManager.GetInstance().PlayOnObject(ParticleList.RF_ESCAPE_FROM_DAYZ_ROADFLARE_BURNING_SMOKE, this, m_FlameLocalPos);
					m_ParJustSmoke.SetWiggle(2, 0.3);
				}
				break;
            }
		}
	}
}