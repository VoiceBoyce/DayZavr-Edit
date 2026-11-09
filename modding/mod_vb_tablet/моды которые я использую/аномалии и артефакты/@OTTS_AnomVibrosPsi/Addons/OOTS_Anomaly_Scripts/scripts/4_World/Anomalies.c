class OOTS_AnomalyBase: House
{
	static const int RPC_ACTIVE = 1;
	protected Particle 	m_Effect;
	protected SoundOnVehicle m_SEngine;
	protected PointLightBase m_IdleLight;
	protected PointLightBase m_ActiveLight;
	
	void OOTS_AnomalyBase()
	{
		SetIdle();
	}
	
	
	override void EEDelete(EntityAI parent)
	{
		if(GetGame().IsClient())
		{
			VisualsAndSoundsOff();
		}
		
		super.EEDelete(parent);
	}
	
	protected void VisualsAndSoundsOff()
	{
		if ( m_Effect ) m_Effect.Stop();
		
		if ( m_IdleLight ) m_IdleLight.FadeOut();
		if ( m_ActiveLight ) m_ActiveLight.FadeOut();
		
		GetGame().ObjectDelete( m_SEngine );
		m_SEngine = NULL;
	}
	
	protected void SetActive(int active_time)
	{
		//delete previous sound, light and partirle
		VisualsAndSoundsOff();
		
		//set sound, light and particle
		
		//set idle
		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( SetIdle, active_time, false);
	}
	
	protected void SetIdle()
	{
		//delete previous sound, light and partirle
		VisualsAndSoundsOff();

		//set sound, light and particle
	}
	
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		switch(rpc_type)
		{
			
			case OOTS_AnomalyBase.RPC_ACTIVE:
			{
				ref Param1<float> p = new Param1<float>(1);
				if (ctx.Read(p))
				{
					SetActive((p.param1 * 1000)); // - sender.GetPingAvg()
				}
				break;
			}
		}
	}
		
}


//Electra
class Anomaly_Electra: OOTS_AnomalyBase
{
	override void SetIdle()
	{
		super.SetIdle();
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{
			m_IdleLight = Anomaly_Electra_Idle_Light.Cast( ScriptedLightBase.CreateLight(Anomaly_Electra_Idle_Light, "0 1 0", 0.5) );
			m_IdleLight.AttachOnObject(this, "0 1 0", "0 0 0");
					
			m_SEngine = PlaySoundLoop("electra_idle", 30);
				
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_ELECTRA_IDLE, this);
		}
	}
	
	override void SetActive(int active_time)
	{
		super.SetActive(active_time);
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{
			m_ActiveLight = Anomaly_Electra_Active_Light.Cast( ScriptedLightBase.CreateLight(Anomaly_Electra_Active_Light, "0 1 0", 0) );
			m_ActiveLight.AttachOnObject(this, "0 1 0", "0 0 0");
					
			m_SEngine = PlaySound("electra_active", 100);
					
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_ELECTRA_ACTIVE, this);
		}
	}
}


//Studen
class Anomaly_Studen: OOTS_AnomalyBase
{
	override void SetIdle()
	{
		super.SetIdle();
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{
			/* m_IdleLight = Anomaly_Studen_Idle_Light.Cast( ScriptedLightBase.CreateLight(Anomaly_Studen_Idle_Light, "0 1 0", 0.5) );
			m_IdleLight.AttachOnObject(this, "0 1 0", "0 0 0"); */
					
			m_SEngine = PlaySoundLoop("studen_idle", 20);
				
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_STUDEN_IDLE, this);
		}
	}
	
	override void SetActive(int active_time)
	{
		super.SetActive(active_time);
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{
			/* m_ActiveLight = Anomaly_Studen_Active_Light.Cast( ScriptedLightBase.CreateLight(Anomaly_Studen_Active_Light, "0 2 0", 0) );
			m_ActiveLight.AttachOnObject(this, "0 2 0", "0 0 0"); */
				
			m_SEngine = PlaySound("studen_active", 100);
				
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_STUDEN_ACTIVE, this);
		}
	}
}

//Tramplin
class Anomaly_Tramplin: OOTS_AnomalyBase
{
	override void SetIdle()
	{
		super.SetIdle();
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{	
			m_SEngine = PlaySoundLoop("tramplin_idle", 50);
				
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_TRAMPLIN_IDLE, this);
		}
	}
	
	override void SetActive(int active_time)
	{
		super.SetActive(active_time);
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{		
			m_SEngine = PlaySound("tramplin_active", 75);
					
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_TRAMPLIN_ACTIVE, this);
		}
	}
}

//Voronka
class Anomaly_Voronka: OOTS_AnomalyBase
{
	static const int RPC_ACTIVE_2 = 2;
	
	override void SetIdle()
	{
		super.SetIdle();
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{			
			m_SEngine = PlaySoundLoop("voronka_idle", 30);
				
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_VORONKA_IDLE, this);
		}
	}
	
	override void SetActive(int active_time)
	{
		super.SetActive(active_time);
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{			
			
			m_SEngine = PlaySound("voronka_active", 80);
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_VORONKA_ACTIVE_1, this);
		}
	}
	
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{	
		super.OnRPC(sender, rpc_type, ctx);
		switch(rpc_type)
		{
			
			case Anomaly_Voronka.RPC_ACTIVE_2:
			{
				if ( m_Effect ) m_Effect.Stop();
				m_Effect = Particle.PlayOnObject(ParticleList.OOTS_VORONKA_ACTIVE_2, this);
				break;
			}
		}
	}
}

//Zharka
class Anomaly_Zharka: OOTS_AnomalyBase
{
	override void SetIdle()
	{
		super.SetIdle();
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{		
			m_SEngine = PlaySoundLoop("zharka_idle", 50);
				
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_ZHARKA_IDLE, this);
		}
	}
	
	override void SetActive(int active_time)
	{
		super.SetActive(active_time);
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{
			m_ActiveLight = Anomaly_Zharka_Active_Light.Cast( ScriptedLightBase.CreateLight(Anomaly_Zharka_Active_Light, "0 2 0", 0.5) );
			m_ActiveLight.AttachOnObject(this, "0 2 0", "0 0 0");
					
			m_SEngine = PlaySound("zharka_active", 75);
					
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_ZHARKA_ACTIVE, this);
		}
	}
}

//teleport
class Anomaly_Teleport_Entry: OOTS_AnomalyBase
{
	private bool m_Show = false;
	
	void Anomaly_Teleport_Entry()
	{
		RegisterNetSyncVariableBool("m_Show");
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		
		if(m_Show)
			SetIdle();
	}
		
	override void SetIdle()
	{
		super.SetIdle();
		if ((!GetGame().IsServer() || !GetGame().IsMultiplayer()) && m_Show)
		{	
			m_IdleLight = Anomaly_Electra_Idle_Light.Cast( ScriptedLightBase.CreateLight(Anomaly_Electra_Idle_Light, "0 1 0", 0.5) );
			m_IdleLight.AttachOnObject(this, "0 1 0", "0 0 0");
			
			m_SEngine = PlaySoundLoop("teleport_idle", 30);
				
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_TELEPORT_IDLE, this);

		}
	}
	
	override void SetActive(int active_time)
	{
		super.SetActive(active_time);
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{
			m_ActiveLight = Anomaly_Electra_Active_Light.Cast( ScriptedLightBase.CreateLight(Anomaly_Electra_Active_Light, "0 1 0", 0.5) );
			m_ActiveLight.AttachOnObject(this, "0 1 0", "0 0 0");
	
			m_SEngine = PlaySound("teleport_work", 100);
					
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_TELEPORT_ACTIVE, this);
		}
	}
}

class Anomaly_Teleport_Output: OOTS_AnomalyBase
{

	override void SetActive(int active_time)
	{
		super.SetActive(active_time);
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{
			m_ActiveLight = Anomaly_Electra_Active_Light.Cast( ScriptedLightBase.CreateLight(Anomaly_Electra_Active_Light, "0 1 0", 0.5) );
			m_ActiveLight.AttachOnObject(this, "0 1 0", "0 0 0");
	
			m_SEngine = PlaySound("teleport_incoming", 100);
					
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_TELEPORT_ACTIVE, this);
		}
	}
}

class Anomaly_Teleport_Dot: OOTS_AnomalyBase
{
	
}

class Anomaly_EMP_Blast: OOTS_AnomalyBase
{
	override void SetIdle()
	{
		super.SetIdle();
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{
			m_IdleLight = Anomaly_Electra_Idle_Light.Cast( ScriptedLightBase.CreateLight(Anomaly_Electra_Idle_Light, "0 1 0", 0.5) );
			m_IdleLight.AttachOnObject(this, "0 1 0", "0 0 0");
					
			m_SEngine = PlaySoundLoop("emp_blast_IDLE", 30);
				
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_EMP_BLAST_IDLE, this);
		}
	}
	
	override void SetActive(int active_time)
	{
		super.SetActive(active_time);
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{
			m_ActiveLight = Anomaly_Electra_Active_Light.Cast( ScriptedLightBase.CreateLight(Anomaly_Electra_Active_Light, "0 1 0", 0) );
			m_ActiveLight.AttachOnObject(this, "0 1 0", "0 0 0");
					
			m_SEngine = PlaySound("emp_blast", 100);
					
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_EMP_BLAST_ACTIVE, this);
		}
	}
}


class Anomaly_Steklo: OOTS_AnomalyBase
{
	override void SetIdle()
	{
		super.SetIdle();
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{
			m_SEngine = PlaySoundLoop("Steklo_IDLE", 5);
				
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_STEKLO_IDLE, this);
		}
	}
	
	override void SetActive(int active_time)
	{
		super.SetActive(active_time);
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{	
			m_SEngine = PlaySoundLoop("Steklo_ACTIVE", 30);
					
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_STEKLO_ACTIVE, this);
		}
	}
}

class Anomaly_DeadZone: OOTS_AnomalyBase
{
	override void SetIdle()
	{
		super.SetIdle();
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{
			m_SEngine = PlaySoundLoop("DeadZone_IDLE", 35);
				
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_DEATH_ZONE_IDLE, this);
		}
	}
	
	override void SetActive(int active_time)
	{
		super.SetActive(active_time);
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{	
			m_SEngine = PlaySound("DeadZone_ACTIVE", 100);
					
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_DEATH_ZONE_ACTIVE, this);
		}
	}
}

class Anomaly_Zastoy: OOTS_AnomalyBase
{
	
	override void SetIdle()
	{
		super.SetIdle();
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{			
			m_SEngine = PlaySoundLoop("Zastoy_idle", 25);
				
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_ZASTOY_IDLE, this);
		}
	}
	
	override void SetActive(int active_time)
	{
		super.SetActive(active_time);
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{			
			
			m_SEngine = PlaySound("Zastoy_active", 35);
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_ZASTOY_ACTIVE, this);
		}
	}
}

class Anomaly_Snow: OOTS_AnomalyBase
{
	
	override void SetIdle()
	{
		super.SetIdle();
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{			
			m_SEngine = PlaySoundLoop("Snow_idle", 40);
				
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_SNOW_IDLE, this);
		}
	}
	
	override void SetActive(int active_time)
	{
		super.SetActive(active_time);
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{			
			
			m_SEngine = PlaySound("Snow_active", 40);
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_SNOW_ACTIVE, this);
		}
	}
}

class Anomaly_Par: OOTS_AnomalyBase
{
	override void SetIdle()
	{
		super.SetIdle();
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{		
			m_SEngine = PlaySoundLoop("Par_idle", 40);
				
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_PAR_IDLE, this);
		}
	}
	
	override void SetActive(int active_time)
	{
		super.SetActive(active_time);
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{
			
			m_SEngine = PlaySound("Par_active", 70);
			
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_PAR_ACTIVE, this);
		}
	}
}

class Anomaly_Groza: OOTS_AnomalyBase
{
	override void SetIdle()
	{
		super.SetIdle();
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{		
			m_SEngine = PlaySoundLoop("Groza_idle", 35);
				
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_GROZA_IDLE, this);
		}
	}
	
	override void SetActive(int active_time)
	{
		super.SetActive(active_time);
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{
			
			m_SEngine = PlaySound("Groza_active", 70);
			
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_GROZA_ACTIVE, this);
		}
	}
}

class Anomaly_BlackHole: OOTS_AnomalyBase
{
	override void SetIdle()
	{
		super.SetIdle();
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{
			m_IdleLight = Anomaly_BlackHole_Idle_Light.Cast( ScriptedLightBase.CreateLight(Anomaly_BlackHole_Idle_Light, "0 1 0", 0.5) );
			m_IdleLight.AttachOnObject(this, "0 1 0", "0 0 0");
					
			m_SEngine = PlaySoundLoop("BlackHole_IDLE", 20);
				
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_BLACKHOLE_IDLE, this);
		}
	}
	
	override void SetActive(int active_time)
	{
		super.SetActive(active_time);
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{
			m_ActiveLight = Anomaly_BlackHole_Active_Light.Cast( ScriptedLightBase.CreateLight(Anomaly_BlackHole_Active_Light, "0 1 0", 0) );
			m_ActiveLight.AttachOnObject(this, "0 1 0", "0 0 0");
					
			m_SEngine = PlaySound("BlackHole_ACTIVE", 70);
					
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_BLACKHOLE_ACTIVE, this);
		}
	}
}

class Anomaly_Atom: OOTS_AnomalyBase
{
	
	override void SetIdle()
	{
		super.SetIdle();
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{			
			m_SEngine = PlaySoundLoop("Atom_IDLE", 25);
				
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_ATOM_IDLE, this);
		}
	}
	
	override void SetActive(int active_time)
	{
		super.SetActive(active_time);
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{			
			
			m_SEngine = PlaySound("Atom_ACTIVE", 35);
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_ATOM_ACTIVE, this);
		}
	}
}

class Anomaly_Lince: OOTS_AnomalyBase
{
	
	override void SetIdle()
	{
		super.SetIdle();
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{			
			m_SEngine = PlaySoundLoop("Lince_idle", 25);
				
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_LINCE_IDLE, this);
		}
	}
	
	override void SetActive(int active_time)
	{
		super.SetActive(active_time);
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{			
			
			m_SEngine = PlaySound("Lince_active", 35);
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_LINCE_ACTIVE, this);
		}
	}
}

class Anomaly_LinceAgr: OOTS_AnomalyBase
{
	
	override void SetIdle()
	{
		super.SetIdle();
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{			
			m_SEngine = PlaySoundLoop("Lince_idle", 25);
				
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_LINCE_IDLE, this);
		}
	}
	
	override void SetActive(int active_time)
	{
		super.SetActive(active_time);
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{			
			
			m_SEngine = PlaySound("LinceAgr_Active", 35);
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_LINCEAGR_ACTIVE, this);
		}
	}
}

class Anomaly_Holera: OOTS_AnomalyBase
{
	
	override void SetIdle()
	{
		super.SetIdle();
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{		
			m_IdleLight = Anomaly_Holera_Idle_Light.Cast( ScriptedLightBase.CreateLight(Anomaly_Holera_Idle_Light, "0 1 0", 0.5) );
			m_IdleLight.AttachOnObject(this, "0 1 0", "0 0 0");
			
			m_SEngine = PlaySoundLoop("Holera_idle", 25);
				
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_HOLERA_IDLE, this);
		}
	}
	
	override void SetActive(int active_time)
	{
		super.SetActive(active_time);
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{		
			m_ActiveLight = Anomaly_Holera_Active_Light.Cast( ScriptedLightBase.CreateLight(Anomaly_Holera_Active_Light, "0 2 0", 0) );
			m_ActiveLight.AttachOnObject(this, "0 2 0", "0 0 0");
			
			m_SEngine = PlaySound("Holera_Active", 35);
			
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_HOLERA_ACTIVE, this);
		}
	}
}

//new
class Anomaly_FireShtorm: OOTS_AnomalyBase
{
	override void SetIdle()
	{
		super.SetIdle();
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{
					
			m_SEngine = PlaySoundLoop("electra_idle", 30);
				
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_FIRE_SHTORM_IDLE, this);
		}
	}
	
	override void SetActive(int active_time)
	{
		super.SetActive(active_time);
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{
					
			m_SEngine = PlaySound("electra_active", 100);
					
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_FIRE_SHTORM_ACTIVE, this);
		}
	}
}

class Anomaly_Lomka: OOTS_AnomalyBase
{
	override void SetIdle()
	{
		super.SetIdle();
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{
					
			m_SEngine = PlaySoundLoop("electra_idle", 30);
				
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_LOMKA_IDLE, this);
		}
	}
	
	override void SetActive(int active_time)
	{
		super.SetActive(active_time);
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{
					
			m_SEngine = PlaySound("electra_active", 100);
					
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_LOMKA_ACTIVE, this);
		}
	}
}

class Anomaly_Roy: OOTS_AnomalyBase
{
	override void SetIdle()
	{
		super.SetIdle();
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{
					
			m_SEngine = PlaySoundLoop("roy_idle", 25);
				
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_ROY_IDLE, this);
		}
	}
	
	override void SetActive(int active_time)
	{
		super.SetActive(active_time);
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{
					
			m_SEngine = PlaySound("roy_active", 60);
					
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_ROY_ACTIVE, this);
		}
	}
}

class Anomaly_Flash: OOTS_AnomalyBase
{
	override void SetIdle()
	{
		super.SetIdle();
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{
					
			m_SEngine = PlaySoundLoop("flash_idle", 15);
				
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_FLASH_IDLE, this);
		}
	}
	
	override void SetActive(int active_time)
	{
		super.SetActive(active_time);
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{
					
			m_SEngine = PlaySound("flash_active", 100);
					
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_FLASH_ACTIVE, this);
		}
	}
}

class Anomaly_FireIskra: OOTS_AnomalyBase
{
	override void SetIdle()
	{
		super.SetIdle();
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{
					
			m_SEngine = PlaySoundLoop("fireIskra_idle", 15);
				
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_FIRE_ISKRA_IDLE, this);
		}
	}
	
	override void SetActive(int active_time)
	{
		super.SetActive(active_time);
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{
					
			m_SEngine = PlaySound("fireIskra_active", 100);
					
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_FIRE_ISKRA_ACTIVE, this);
		}
	}
}

class Anomaly_MlechniPut: OOTS_AnomalyBase
{
	override void SetIdle()
	{
		super.SetIdle();
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{
		
			m_SEngine = PlaySoundLoop("mlechniPut_idle", 25);
		
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_MLECHNIPUT_IDLE, this);
		}
	}
	
	override void SetActive(int active_time)
	{
		super.SetActive(active_time);
		if (!GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{
			
			m_SEngine = PlaySound("mlechniPut_active", 70);
			
			m_Effect = Particle.PlayOnObject(ParticleList.OOTS_MLECHNIPUT_ACTIVE, this);
		}
	}
}