class RFFSHeli_Apache extends RFFSHeli_base
{
	//Exhaust smoke points
	protected vector m_exhaustPtcPos2;			
	protected vector m_exhaustPtcDir2;			
	protected ref EffVehicleSmoke m_exhaustFx2;
	protected int m_exhaustPtcFx2;

	protected ref RFFSHeli_HeliConfig m_heli_config;
	
	void RFFSHeli_Apache() 
	{
		c_startup_soundset = "RFFSHeli_Apache_Startup_SoundSet";
		c_engine_soundset = "RFFSHeli_Apache_Engine_SoundSet";
		c_shutdown_soundset = "RFFSHeli_Apache_Shutdown_SoundSet";
		c_startup_sound_length = 15000.0;	
		c_shutdown_sound_length = 25750.0;
		c_heli_wreck_model = "RFFSHeli_Apache_wreck";

		DetermineGauges();
		
		if 	( MemoryPointExists("ptcExhaust_end2") )
		{
			m_exhaustPtcPos2 = GetMemoryPointPos("ptcExhaust_end2");
			if ( MemoryPointExists("ptcExhaust_start2") )
			{
				vector exhaustStart2 = GetMemoryPointPos("ptcExhaust_start2");
				vector tempOri2 = vector.Direction( exhaustStart2, m_exhaustPtcPos2);

				m_exhaustPtcDir2[0] = -tempOri2[2];
				m_exhaustPtcDir2[1] = tempOri2[1];
				m_exhaustPtcDir2[2] = tempOri2[0];
				
				m_exhaustPtcDir2 = m_exhaustPtcDir2.Normalized().VectorToAngles();
			}
		}
		else
		{
			m_exhaustPtcPos2 = "0 0 0";
			m_exhaustPtcDir2 = "1 1 1";
		}
	}
	
	void ~RFFSHeli_Apache()
	{
		//Gotta clean up sounds if/when helicopter is deleted
		if (m_heli_startup_wave) m_heli_startup_wave.Stop();
		if (m_heli_shutdown_wave) m_heli_shutdown_wave.Stop();
		if (m_heli_engine_wave) m_heli_engine_wave.Stop();
		if (m_heli_enginedamaged_wave) m_heli_enginedamaged_wave.Stop();
		if (m_heli_klaxon_wave) m_heli_klaxon_wave.Stop();
		if (m_engine_badlydamaged_smoke) m_engine_badlydamaged_smoke.Stop();
		if (m_engine_ruined_smoke) m_engine_ruined_smoke.Stop();
		if (m_exhaustPtcFx2 && SEffectManager.IsEffectExist(m_exhaustPtcFx2)) SEffectManager.Stop(m_exhaustPtcFx2);		

		if (GetGame() && GetGame().IsServer() && c_print_damage_debug && this)
			RFFSLog().Log("DESPAWN", "" + this + " despawned from " + this.GetPosition());
	}

	//Set the 3PP camera distance
	override float GetTransportCameraDistance()			
	{
		return 20.0;
	}

	override void EEInit()
	{
		super.EEInit();
		if ( GetGame().IsServer())
		{
			//Default config settings for this heli
			float FuelCapacity = 495.0;
			float FuelConsumptionRate = 1.24998;
			bool TrainerMode = false;
			bool HasHydraulics = true;
			bool SimpleCollective = false;
			float MaximumAirspeed = 268.54;
			float MaximumAltitude = 1250.0;
			float MaximumClimbRate = 720.0;
			float AerodynamicDrag = 100.0;
			float BankTurnCoeff = 100.0;
			float SpeedFactor = 100.0;
			float AntiTorqueThrustRate = 100.0;
			float CyclicThrustRate = 100.0;
			float CollectiveThrustRate = 100.0;
			float MaximumRotationalRate = 100.0;
			float CyclicDampeningRate = 100.0;
		
			//Retrieve and apply the Heli Config file settings
			string heli_type = "RFFSHeli_Apache";
			
			RFFSHeli_HeliConfigData DefaultHeliConfigData = new RFFSHeli_HeliConfigData(FuelCapacity, FuelConsumptionRate, TrainerMode, HasHydraulics, SimpleCollective, MaximumAirspeed, MaximumAltitude, MaximumClimbRate, AerodynamicDrag, BankTurnCoeff, SpeedFactor, AntiTorqueThrustRate, CyclicThrustRate, CollectiveThrustRate, MaximumRotationalRate, CyclicDampeningRate);

			m_heli_config = new RFFSHeli_HeliConfig(heli_type, DefaultHeliConfigData);
			ApplyHeliConfig(heli_type, m_heli_config);	
			
		}

	}

	override void AttachHeliParts()
	{

		if (!m_heli_parts_added)
		{
			EntityAI entity_Apache;
			if ( Class.CastTo(entity_Apache, this) )
			{
				entity_Apache.GetInventory().CreateAttachment( "RFFSHeli_Apache_Driverdoor" );
				entity_Apache.GetInventory().CreateAttachment( "RFFSHeli_Apache_Codriverdoor" );
				m_heli_parts_added = true;
				m_attachement_timer = GetGame().GetTime();
			}			
		}

	}
	
	override void EnginesRunningAnimation ( float time )
	{
		super.EnginesRunningAnimation ( time );
		
		if ((GetGame().IsClient() || !GetGame().IsMultiplayer()))
		{
			if ( m_exhaustPtcPos2 != "0 0 0")
			{
				if ( !SEffectManager.IsEffectExist( m_exhaustPtcFx2 ))
				{
					m_exhaustFx2 = new EffExhaustSmoke();
					m_exhaustPtcFx2 = SEffectManager.PlayOnObject( m_exhaustFx2, this, m_exhaustPtcPos2, m_exhaustPtcDir2 );
				}
				m_exhaustFx2.SetParticleStateLight();
			}
		}
	}

	override void ShutdownAnimationStarting ( float time )
	{
		super.ShutdownAnimationStarting(time);
		if ((GetGame().IsClient() || !GetGame().IsMultiplayer()))
			if (SEffectManager.IsEffectExist(m_exhaustPtcFx2)) SEffectManager.Stop(m_exhaustPtcFx2);
	}


	//Update animation values for cockpit gauges specific to this heli
	override void UpdateCockpitGauges()		
	{
		super.UpdateCockpitGauges();
	
	}

	//Make sure that non-removalbe attachment can't be removed
	override bool CanReleaseAttachment( EntityAI attachment )											
	{
		if( !super.CanReleaseAttachment( attachment ) )				
			return false;

		string attType = attachment.GetType();
		if( attType.Contains("RFFSHeli_Apache") )return false;

		return true;																	
	}
	
	override int GetCarDoorsState( string slotType )   
	{
		CarDoor carDoor;
		Class.CastTo( carDoor, FindAttachmentBySlotName( slotType ) );  
		if ( !carDoor ) return CarDoorState.DOORS_MISSING;							
	
		switch( slotType )
		{
			case "apache_doors_driver":								
				if ( GetAnimationPhase("DoorsDriver") > 0.5 )			
					return CarDoorState.DOORS_OPEN;						
				else
					return CarDoorState.DOORS_CLOSED;					
			break;
			
			case "apache_doors_codriver":
				if ( GetAnimationPhase("DoorsCoDriver") > 0.5 )
					return CarDoorState.DOORS_OPEN;
				else
					return CarDoorState.DOORS_CLOSED;
			break;
		}
		return CarDoorState.DOORS_MISSING;						
	}

	
	override int GetSeatIndexFromDoor( string pDoorSelection )
	{
		//Potientially could be fixed some other way, currently follows the unfortunate pattern that CanReachDoorsFromSeat has created
		switch (pDoorSelection)
		{
			case "DoorsDriver":
				return 0;
				break;
			case "DoorsCoDriver":
				return 1;
				break;
		}
		return -1;
	}

	
	override bool CrewCanGetThrough( int posIdx )				
	{
		switch( posIdx )
		{
			case 0:
				if ( GetCarDoorsState( "apache_doors_driver" ) == CarDoorState.DOORS_CLOSED )  
					return false;																  

				return true;																	  
			break;
			
			case 1:
				if ( GetCarDoorsState( "apache_doors_codriver" ) == CarDoorState.DOORS_CLOSED )
					return false;

				return true;
			break;
		}

		return false;
	}
	
	override string GetDoorSelectionNameFromSeatPos(int posIdx)				
	{
		switch( posIdx )
		{
		case 0:															 	
			return "doors_driver";											
		break;
		case 1:
			return "doors_codriver";
		break;
		}
		return super.GetDoorSelectionNameFromSeatPos(posIdx);
	}
	
	override string GetDoorInvSlotNameFromSeatPos(int posIdx)
	{
		switch( posIdx )
		{
		case 0:
			return "apache_doors_driver";
		break;
		case 1:
			return "apache_doors_codriver";
		break;
		}
		return super.GetDoorInvSlotNameFromSeatPos(posIdx);
	}

	override string GetSeatExitLocation(int playerPosition)
	{
		switch( playerPosition )
		{
			case 0:
				return "pos_driver";
			break;
			
			case 1:
				return "pos_codriver";
			break;
		}

		return super.GetSeatExitLocation(playerPosition);
	}	
	
	override string GetAnimSourceFromSelection( string selection )
	{
		switch( selection )
		{
		case "doors_driver":
			return "DoorsDriver";
		case "doors_codriver":
			return "DoorsCoDriver";
		}

		return "";
	}
	
	override string GetDoorConditionPointFromSelection( string selection )
	{
		switch( selection )
		{
		case "seat_driver":
			return "seat_con_1_1";
		break;
		case "seat_codriver":
			return "seat_con_2_1";
		break;
		}
		
		return "";
	}

	override bool CanReachSeatFromSeat( int currentSeat, int nextSeat )
	{
//		switch( currentSeat )
//		{
//		case 0:
//			if ( nextSeat == 1 )
//				return true;
//			break;
//		case 1:
//			if ( nextSeat == 0)
//				return true;
//			break;
//		}
		
		return false;
	}

	override int GetSeatAnimationType( int posIdx )
	{
		switch( posIdx )
		{
		case 0:
			return DayZPlayerConstants.VEHICLESEAT_CODRIVER;
		case 1:
			return DayZPlayerConstants.VEHICLESEAT_CODRIVER;
		}
		return 0;
	}

	override int GetDoorOpenAnimationType( int posIdx )
	{
		switch( posIdx )
		{
		case 0:
			return DayZPlayerConstants.CMD_ACTIONMOD_CODRIVER_DOOROPEN;
		case 1:
			return DayZPlayerConstants.CMD_ACTIONMOD_CODRIVER_DOOROPEN;
		}
		return 0;
	}

	override int GetDoorCloseAnimationType( int posIdx )
	{
		switch( posIdx )
		{
		case 0:
			return DayZPlayerConstants.CMD_ACTIONMOD_CODRIVER_DOORCLOSE;
		case 1:
			return DayZPlayerConstants.CMD_ACTIONMOD_CODRIVER_DOORCLOSE;
		}
		return 0;
	}

	override bool CanReachDoorsFromSeat( string pDoorsSelection, int pCurrentSeat )
	{
		//Print("Current Seat: " + pCurrentSeat + " Door Selection: " + pDoorsSelection);
		switch( pCurrentSeat )
		{
		case 0:
			if (pDoorsSelection == "DoorsDriver")
			{
				return true;
			}
		break;
		case 1:
			if (pDoorsSelection == "DoorsCoDriver")
			{
				return true;
			}
		break;
		}
		
		return false;		
	}

	override bool IsAreaAtDoorFree( int currentSeat, float maxAllowedObjHeight = 2.0, float horizontalExtents = 0.5, float playerHeight = 1.7 )
	{
		return super.IsAreaAtDoorFree( currentSeat, maxAllowedObjHeight, horizontalExtents, playerHeight );
	}

//#ifdef SURVIVOR_ANIMS	
//	override int GetAnimInstance()
//	{
//		return 14;  
//		return VehicleAnimInstances.HELICOPTER; 
//	}
//#endif

	override bool CanDisplayCargo()
	{
		if ( !super.CanDisplayCargo() )
			return false;
		
		if ( GetCarDoorsState("apache_doors_driver") != CarDoorState.DOORS_OPEN && GetCarDoorsState( "apache_doors_codriver" ) != CarDoorState.DOORS_OPEN)
			return false;
		
		return true;
	}
	
	override float OnSound( CarSoundCtrl ctrl, float oldValue )
	{
		switch ( ctrl )
		{
			case CarSoundCtrl.DOORS:
				float newValue = 0;
				
				if ( GetCarDoorsState( "apache_doors_driver" ) == CarDoorState.DOORS_CLOSED )
					newValue += 0.25;

				if ( GetCarDoorsState( "apache_doors_codriver" ) == CarDoorState.DOORS_CLOSED )
					newValue += 0.25;
			
				if ( newValue > 1 )
					newValue = 1;
			
				return newValue;
			break;
		}
		return oldValue;
	}

	//Add everything needed to fly if this was spawned by an admin
	override void OnDebugSpawn()				
	{
		EntityAI entity;
		
		if ( Class.CastTo(entity, this) )
		{
			entity.GetInventory().CreateAttachment( "RFFSHeli_aviation_battery" );
			entity.GetInventory().CreateAttachment( "RFFSHeli_igniter_plug" );
			entity.GetInventory().CreateAttachment( "RFFSHeli_hydraulic_hoses" );
			entity.GetInventory().CreateAttachment( "RFFSHeli_wiring_harness" );
			entity.GetInventory().CreateAttachment( "HeadlightH7" );
		};

		Fill( CarFluid.FUEL, c_fuel_max );
		Fill( CarFluid.OIL, 4.0);
		if (m_engine_badlydamaged_smoke) m_engine_badlydamaged_smoke.Stop();
		SetBladesNoBlur();
	}

};
class RFFSHeli_Apache_Driverdoor extends CarDoor {};
class RFFSHeli_Apache_Codriverdoor extends CarDoor {};

class RFFSHeli_Apache_Wreck extends Wreck_UH1Y
{

}






