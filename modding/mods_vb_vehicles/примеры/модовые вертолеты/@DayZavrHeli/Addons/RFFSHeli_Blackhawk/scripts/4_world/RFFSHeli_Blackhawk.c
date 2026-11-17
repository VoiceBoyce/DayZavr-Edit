class RFFSHeli_Blackhawk extends RFFSHeli_base
{
	//Exhaust smoke points
	protected vector m_exhaustPtcPos2;			
	protected vector m_exhaustPtcDir2;			
	protected ref EffVehicleSmoke m_exhaustFx2;
	protected int m_exhaustPtcFx2;

	protected ref RFFSHeli_HeliConfig m_heli_config;
	
	void RFFSHeli_Blackhawk() 
	{
		c_startup_soundset = "RFFSHeli_Blackhawk_Startup_SoundSet";
		c_engine_soundset = "RFFSHeli_Blackhawk_Engine_SoundSet";
		c_shutdown_soundset = "RFFSHeli_Blackhawk_Shutdown_SoundSet";
		//c_startup_sound_length = 14000.0;	
		//c_shutdown_sound_length = 16000.0;
		c_heli_wreck_model = "RFFSHeli_Blackhawk_wreck";

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
	
	void ~RFFSHeli_Blackhawk()
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
		return 17.0;
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
			float BankTurnCoeff = 140.0;
			float SpeedFactor = 100.0;
			float AntiTorqueThrustRate = 80.0;
			float CyclicThrustRate = 55.0;
			float CollectiveThrustRate = 100.0;
			float MaximumRotationalRate = 85.0;
			float CyclicDampeningRate = 45.0;

			//Retrieve and apply the Heli Config file settings
			string heli_type = "RFFSHeli_Blackhawk";
			
			RFFSHeli_HeliConfigData DefaultHeliConfigData = new RFFSHeli_HeliConfigData(FuelCapacity, FuelConsumptionRate, TrainerMode, HasHydraulics, SimpleCollective, MaximumAirspeed, MaximumAltitude, MaximumClimbRate, AerodynamicDrag, BankTurnCoeff, SpeedFactor, AntiTorqueThrustRate, CyclicThrustRate, CollectiveThrustRate, MaximumRotationalRate, CyclicDampeningRate);

			m_heli_config = new RFFSHeli_HeliConfig(heli_type, DefaultHeliConfigData);
			ApplyHeliConfig(heli_type, m_heli_config);	

		}
	}

	override void AttachHeliParts()
	{
		if (!m_heli_parts_added)
		{
			EntityAI entity_Blackhawk;
			if ( Class.CastTo(entity_Blackhawk, this) )
			{
				entity_Blackhawk.GetInventory().CreateAttachment( "RFFSHeli_Blackhawk_Driverdoor" );
				entity_Blackhawk.GetInventory().CreateAttachment( "RFFSHeli_Blackhawk_Codriverdoor" );
				entity_Blackhawk.GetInventory().CreateAttachment( "RFFSHeli_Blackhawk_Cargo1" );
				entity_Blackhawk.GetInventory().CreateAttachment( "RFFSHeli_Blackhawk_Cargo1a" );
				entity_Blackhawk.GetInventory().CreateAttachment( "RFFSHeli_Blackhawk_Cargo2" );
				entity_Blackhawk.GetInventory().CreateAttachment( "RFFSHeli_Blackhawk_Cargo2a" );
				entity_Blackhawk.GetInventory().CreateAttachment( "RFFSHeli_Blackhawk_Cargo3" );
				entity_Blackhawk.GetInventory().CreateAttachment( "RFFSHeli_Blackhawk_Cargo4" );
				
				m_heli_parts_added = true;
				m_attachement_timer = GetGame().GetTime();
			}			
		}		
	}
	
	override void SpawnHeliCrash(vector position, vector orientation)
	{
		//Give it a 180 degree spin
		float x_axis = orientation[0] + 180;
		if (x_axis > 360) x_axis -= 360;
		orientation[0] = x_axis;
		orientation[1] = 0;
		orientation[2] = 0;

		//Make sure that the wreck spawns on the surface
		vector newposition = position;
		newposition[1] = GetGame().SurfaceY(newposition[0], newposition[2]);
		
		//Spawn in the proper crash site class
		Wreck_UH1Y crashsite = Wreck_UH1Y.Cast(GetGame().CreateObjectEx(c_heli_wreck_model, newposition, ECE_PLACE_ON_SURFACE));
		crashsite.SetPosition(newposition);		
		crashsite.SetOrientation(orientation);		
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
			if (m_exhaustPtcFx2 && SEffectManager.IsEffectExist(m_exhaustPtcFx2)) SEffectManager.Stop(m_exhaustPtcFx2);
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
		if( attType.Contains("RFFSHeli_Blackhawk") )return false;

		return true;																	
	}
	
	override int GetCarDoorsState( string slotType )   
	{
		CarDoor carDoor;
		Class.CastTo( carDoor, FindAttachmentBySlotName( slotType ) );  
		if ( !carDoor ) return CarDoorState.DOORS_MISSING;							
	
		switch( slotType )
		{
			case "blackhawk_doors_driver":								
				if ( GetAnimationPhase("DoorsDriver") > 0.5 )			
					return CarDoorState.DOORS_OPEN;						
				else
					return CarDoorState.DOORS_CLOSED;					
			break;
			
			case "blackhawk_doors_codriver":
				if ( GetAnimationPhase("DoorsCoDriver") > 0.5 )
					return CarDoorState.DOORS_OPEN;
				else
					return CarDoorState.DOORS_CLOSED;
			break;
				
			case "blackhawk_doors_cargo1":
				if ( GetAnimationPhase("DoorsCargo1") > 0.5 )
					return CarDoorState.DOORS_OPEN;
				else
					return CarDoorState.DOORS_CLOSED;
			break;
				
			case "blackhawk_doors_cargo2":
				if ( GetAnimationPhase("DoorsCargo2") > 0.5 )
					return CarDoorState.DOORS_OPEN;
				else
					return CarDoorState.DOORS_CLOSED;
			break;
				
			case "blackhawk_doors_cargo3":
				if ( GetAnimationPhase("DoorsCargo3") > 0.5 )
					return CarDoorState.DOORS_OPEN;
				else
					return CarDoorState.DOORS_CLOSED;
			break;
				
			case "blackhawk_doors_cargo4":
				if ( GetAnimationPhase("DoorsCargo4") > 0.5 )
					return CarDoorState.DOORS_OPEN;
				else
					return CarDoorState.DOORS_CLOSED;
			break;
		}
		return CarDoorState.DOORS_MISSING;						
	}
	
	override bool CrewCanGetThrough( int posIdx )				
	{
		switch( posIdx )
		{
			case 0:
				if ( GetCarDoorsState( "blackhawk_doors_driver" ) == CarDoorState.DOORS_CLOSED )  
					return false;																  

				return true;																	  
			break;
			
			case 1:
				if ( GetCarDoorsState( "blackhawk_doors_codriver" ) == CarDoorState.DOORS_CLOSED )
					return false;

				return true;
			break;
		
			case 2:
				if ( GetCarDoorsState( "blackhawk_doors_cargo1" ) == CarDoorState.DOORS_CLOSED )
					return false;

				return true;
			break;
		
			case 3:
				if ( GetCarDoorsState( "blackhawk_doors_cargo2" ) == CarDoorState.DOORS_CLOSED )
					return false;

				return true;
			break;
		
			case 4:
				if ( GetCarDoorsState( "blackhawk_doors_cargo3" ) == CarDoorState.DOORS_CLOSED )
					return false;

				return true;
			break;
		
			case 5:
				if ( GetCarDoorsState( "blackhawk_doors_cargo3" ) == CarDoorState.DOORS_CLOSED )
					return false;

				return true;
			break;
		
			case 6:
				if ( GetCarDoorsState( "blackhawk_doors_cargo4" ) == CarDoorState.DOORS_CLOSED )
					return false;

				return true;
			break;
			
			case 7:
				if ( GetCarDoorsState( "blackhawk_doors_cargo4" ) == CarDoorState.DOORS_CLOSED )
					return false;

				return true;
			break;
		
			case 8:
				if ( GetCarDoorsState( "blackhawk_doors_cargo3" ) == CarDoorState.DOORS_CLOSED )
					return false;

				return true;
			break;
		
			case 9:
				if ( GetCarDoorsState( "blackhawk_doors_cargo3" ) == CarDoorState.DOORS_CLOSED )
					return false;

				return true;
			break;
		
			case 10:
				if ( GetCarDoorsState( "blackhawk_doors_cargo4" ) == CarDoorState.DOORS_CLOSED )
					return false;

				return true;
			break;
		
			case 11:
				if ( GetCarDoorsState( "blackhawk_doors_cargo4" ) == CarDoorState.DOORS_CLOSED )
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
        case 2:
			return "doors_cargo1";
		break;
		case 3:
			return "doors_cargo2";
		break;	
        case 4:
			return "doors_cargo3";
		break;
        case 5:
			return "doors_cargo3";
		break;
		case 6:
			return "doors_cargo4";
		break;
		case 7:
			return "doors_cargo4";
		break;
        case 8:
			return "doors_cargo3";
		break;
        case 9:
			return "doors_cargo3";
		break;
		case 10:
			return "doors_cargo4";
		break;
		case 11:
			return "doors_cargo4";
		break;	
		}
		return super.GetDoorSelectionNameFromSeatPos(posIdx);
	}
	
	override string GetDoorInvSlotNameFromSeatPos(int posIdx)
	{
		switch( posIdx )
		{
		case 0:
			return "blackhawk_doors_driver";
		break;
		
		case 1:
			return "blackhawk_doors_codriver";
		break;
		
		case 2:
			return "blackhawk_doors_cargo1";
		break;
		
		case 3:
			return "blackhawk_doors_cargo2";
		break;
		
		case 4:
			return "blackhawk_doors_cargo3";
		break;
		
		case 5:
			return "blackhawk_doors_cargo3";
		break;
		
		case 6:
			return "blackhawk_doors_cargo4";
		break;
		
		case 7:
			return "blackhawk_doors_cargo4";
		break;
		
		case 8:
			return "blackhawk_doors_cargo3";
		break;
		
		case 9:
			return "blackhawk_doors_cargo3";
		break;
		
		case 10:
			return "blackhawk_doors_cargo4";
		break;
		
		case 11:
			return "blackhawk_doors_cargo4";
		break;
		}
		return super.GetDoorInvSlotNameFromSeatPos(posIdx);
	}
	
	override string GetAnimSourceFromSelection( string selection )
	{
		switch( selection )
		{
		case "doors_driver":
			return "DoorsDriver";
		case "doors_codriver":
			return "DoorsCoDriver";
		case "doors_cargo1":
			return "DoorsCargo1";
		case "doors_cargo2":
			return "DoorsCargo2";
		case "doors_cargo3":
			return "DoorsCargo3";
		case "doors_cargo4":
			return "DoorsCargo4";
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
		case "seat_cargo1":
			return "seat_con_1_2";
		break;
		case "seat_cargo2":
			return "seat_con_2_2";
		break;
		case "seat_cargo3":
			return "seat_con_1_3";
		break;
		case "seat_cargo4":
			return "seat_con_1_4";
		break;
		case "seat_cargo5":
			return "seat_con_2_3";
		break;
		case "seat_cargo6":
			return "seat_con_2_4";
		break;
		case "seat_cargo7":
			return "seat_con_1_5";
		break;
		case "seat_cargo8":
			return "seat_con_1_6";
		break;
		case "seat_cargo9":
			return "seat_con_2_5";
		break;
		case "seat_cargo10":
			return "seat_con_2_6";
		break;
		}
		
		return "";
	}

	override bool CanReachSeatFromSeat( int currentSeat, int nextSeat )
	{
//		switch( currentSeat )
//		{
//		case 0:
//			if ( nextSeat == 1 || nextSeat == 2 || nextSeat == 3)
//				return true;
//			break;
//		case 1:
//			if ( nextSeat == 0 || nextSeat == 2 || nextSeat == 3)
//				return true;
//			break;
//		case 2:
//			if ( nextSeat == 3  || nextSeat == 0 || nextSeat == 1)
//				return true;
//			break;
//		case 3:
//			if ( nextSeat == 2  || nextSeat == 0 || nextSeat == 1)
//				return true;
//			break;
//		case 4:
//			if ( nextSeat == 5 || nextSeat == 6 || nextSeat == 7 || nextSeat == 2 || nextSeat == 3)
//				return true;
//			break;
//		case 5:
//			if ( nextSeat == 4 || nextSeat ==  6 || nextSeat == 7 || nextSeat == 2 || nextSeat == 3)
//				return true;
//			break;
//		case 6:
//			if (  nextSeat == 4 || nextSeat == 5 || nextSeat ==  7 || nextSeat == 2 || nextSeat == 3)
//				return true;
//			break;
//		case 7:
//			if ( nextSeat ==  4 || nextSeat == 5 || nextSeat == 6 || nextSeat == 2 || nextSeat == 3)
//				return true;
//			break;
//			
//		case 8:
//			if ( nextSeat == 9)
//				return true;
//			break;
//		case 9:
//			if ( nextSeat == 8 || nextSeat == 10)
//				return true;
//			break;
//		case 10:
//			if ( nextSeat == 9 || nextSeat == 11)
//				return true;
//			break;
//		case 11:
//			if ( nextSeat == 10)
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
//#ifdef SURVIVOR_ANIMS	
//			return DayZPlayerConstants.VEHICLESEAT_DRIVER;
//#else
			return DayZPlayerConstants.VEHICLESEAT_PASSENGER_L;
//#endif
		case 1:
			return DayZPlayerConstants.VEHICLESEAT_CODRIVER;
		case 2:
			return DayZPlayerConstants.VEHICLESEAT_PASSENGER_L;
		case 3:
			return DayZPlayerConstants.VEHICLESEAT_PASSENGER_R;
		case 4:
			return DayZPlayerConstants.VEHICLESEAT_PASSENGER_L;
		case 5:
			return DayZPlayerConstants.VEHICLESEAT_PASSENGER_L;
		case 6:
			return DayZPlayerConstants.VEHICLESEAT_PASSENGER_R;
		case 7:
			return DayZPlayerConstants.VEHICLESEAT_PASSENGER_R;
		case 8:
			return DayZPlayerConstants.VEHICLESEAT_PASSENGER_R;
		case 9:
			return DayZPlayerConstants.VEHICLESEAT_PASSENGER_R;
		case 10:
			return DayZPlayerConstants.VEHICLESEAT_PASSENGER_L;
		case 11:
			return DayZPlayerConstants.VEHICLESEAT_PASSENGER_L;
		}
		return 0;
	}

	override int GetDoorOpenAnimationType( int posIdx )
	{
		switch( posIdx )
		{
		case 0:
			return DayZPlayerConstants.CMD_ACTIONMOD_DRIVER_DOOR_OPEN;
		case 1:
			return DayZPlayerConstants.CMD_ACTIONMOD_CODRIVER_DOOROPEN;
		case 2:
			return DayZPlayerConstants.CMD_ACTIONMOD_CODRIVER_DOOROPEN;
		case 3:
			return DayZPlayerConstants.CMD_ACTIONMOD_DRIVER_DOOR_OPEN;
		case 4:
			return DayZPlayerConstants.CMD_ACTIONMOD_DRIVER_DOOR_OPEN;
		case 5:
			return DayZPlayerConstants.CMD_ACTIONMOD_DRIVER_DOOR_OPEN;
		case 6:
			return DayZPlayerConstants.CMD_ACTIONMOD_CODRIVER_DOOROPEN;
		case 7:
			return DayZPlayerConstants.CMD_ACTIONMOD_CODRIVER_DOOROPEN;
		case 8:
			return DayZPlayerConstants.CMD_ACTIONMOD_CODRIVER_DOOROPEN;
		case 9:
			return DayZPlayerConstants.CMD_ACTIONMOD_CODRIVER_DOOROPEN;
		case 10:
			return DayZPlayerConstants.CMD_ACTIONMOD_DRIVER_DOOR_OPEN;
		case 11:
			return DayZPlayerConstants.CMD_ACTIONMOD_DRIVER_DOOR_OPEN;
		}
		return 0;
	}

	override int GetDoorCloseAnimationType( int posIdx )
	{
		switch( posIdx )
		{
		case 0:
			return DayZPlayerConstants.CMD_ACTIONMOD_DRIVER_DOOR_CLOSE;
		case 1:
			return DayZPlayerConstants.CMD_ACTIONMOD_CODRIVER_DOORCLOSE;
		case 2:
			return DayZPlayerConstants.CMD_ACTIONMOD_CODRIVER_DOORCLOSE;
		case 3:
			return DayZPlayerConstants.CMD_ACTIONMOD_DRIVER_DOOR_CLOSE;
		case 4:
			return DayZPlayerConstants.CMD_ACTIONMOD_DRIVER_DOOR_CLOSE;
		case 5:
			return DayZPlayerConstants.CMD_ACTIONMOD_DRIVER_DOOR_CLOSE;
		case 6:
			return DayZPlayerConstants.CMD_ACTIONMOD_CODRIVER_DOORCLOSE;
		case 7:
			return DayZPlayerConstants.CMD_ACTIONMOD_CODRIVER_DOORCLOSE;
		case 8:
			return DayZPlayerConstants.CMD_ACTIONMOD_CODRIVER_DOORCLOSE;
		case 9:
			return DayZPlayerConstants.CMD_ACTIONMOD_CODRIVER_DOORCLOSE;
		case 10:
			return DayZPlayerConstants.CMD_ACTIONMOD_DRIVER_DOOR_CLOSE;
		case 11:
			return DayZPlayerConstants.CMD_ACTIONMOD_DRIVER_DOOR_CLOSE;
		}
		return 0;
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
			
			case 2:
				return "pos_cargo1";
			break;
			
			case 3:
				return "pos_cargo2";
			break;
			
			case 4:
				return "pos_cargo4";
			break;
			
			case 5:
				return "pos_cargo4";
			break;
			
			case 6:
				return "pos_cargo5";
			break;
			
			case 7:
				return "pos_cargo5";
			break;
			
			case 8:
				return "pos_cargo4";
			break;
			
			case 9:
				return "pos_cargo4";
			break;
			
			case 10:
				return "pos_cargo5";
			break;
			
			case 11:
				return "pos_cargo5";
			break;
		}

		return super.GetSeatExitLocation(playerPosition);
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
		case 2:
			if (pDoorsSelection == "DoorsCargo1")
			{
				return true;
			}
		break;
		case 3:
			if (pDoorsSelection == "DoorsCargo2")
			{
				return true;
			}
		break;
		case 4:
			if (pDoorsSelection == "DoorsCargo3")
			{
				return true;
			}
		break;
		case 5:
			if (pDoorsSelection == "DoorsCargo3")
			{
				return true;
			}
		break;
		case 6:
			if (pDoorsSelection == "DoorsCargo4")
			{
				return true;
			}
		break;
		case 7:
			if (pDoorsSelection == "DoorsCargo4")
			{
				return true;
			}
		break;
		case 8:
			if (pDoorsSelection == "DoorsCargo3")
			{
				return true;
			}
		break;
		case 9:
			if (pDoorsSelection == "DoorsCargo3")
			{
				return true;
			}
		break;
		case 10:
			if (pDoorsSelection == "DoorsCargo4")
			{
				return true;
			}
		break;
		case 11:
			if (pDoorsSelection == "DoorsCargo4")
			{
				return true;
			}
		break;
		}
		
		return false;		
	}
	
	override bool CanDisplayCargo()
	{
		if ( !super.CanDisplayCargo() )
			return false;
		
		if ( GetCarDoorsState("blackhawk_doors_driver") != CarDoorState.DOORS_OPEN && GetCarDoorsState( "blackhawk_doors_codriver" ) != CarDoorState.DOORS_OPEN && GetCarDoorsState( "blackhawk_doors_cargo1" ) != CarDoorState.DOORS_OPEN && GetCarDoorsState( "blackhawk_doors_cargo2" ) != CarDoorState.DOORS_OPEN && GetCarDoorsState( "blackhawk_doors_cargo3" ) != CarDoorState.DOORS_OPEN && GetCarDoorsState( "blackhawk_doors_cargo4" ) != CarDoorState.DOORS_OPEN)
			return false;
		
		return true;
	}
	
	override float OnSound( CarSoundCtrl ctrl, float oldValue )
	{
		switch ( ctrl )
		{
			case CarSoundCtrl.DOORS:
				float newValue = 0;
				
				if ( GetCarDoorsState( "blackhawk_doors_driver" ) == CarDoorState.DOORS_CLOSED )
					newValue += 0.25;

				if ( GetCarDoorsState( "blackhawk_doors_codriver" ) == CarDoorState.DOORS_CLOSED )
					newValue += 0.25;
				
				if ( GetCarDoorsState( "blackhawk_doors_cargo1" ) == CarDoorState.DOORS_CLOSED )
					newValue += 0.25;
				
				if ( GetCarDoorsState( "blackhawk_doors_cargo2" ) == CarDoorState.DOORS_CLOSED )
					newValue += 0.25;
				
				if ( GetCarDoorsState( "blackhawk_doors_cargo3" ) == CarDoorState.DOORS_CLOSED )
					newValue += 0.25;
				
				if ( GetCarDoorsState( "blackhawk_doors_cargo4" ) == CarDoorState.DOORS_CLOSED )
					newValue += 0.25;
				
				if ( newValue > 1 )
					newValue = 1;
			
				return newValue;
			break;
		}
		return oldValue;
	};

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
			entity.GetInventory().CreateAttachment( "HeadlightH7" );
		};

		Fill( CarFluid.FUEL, c_fuel_max );
		Fill( CarFluid.OIL, 4.0);
		if (m_engine_badlydamaged_smoke) m_engine_badlydamaged_smoke.Stop();
		SetBladesNoBlur();
	}
	


	override int GetAnimInstance()
	{
//#ifdef SURVIVOR_ANIMS	
//		return 14;  
//		return VehicleAnimInstances.HELICOPTER; 

//#else
		return VehicleAnimInstances.S120;  
						   // Tweak ^^^^^^^^^ for different seating postions, search for vehicleaniminstances.c in your p drive
//#endif
	};

	override void SetActions()
	{
		//AddAction(ActionAnimateCarSelection); not needed now
		AddAction(ActionSwitchSeats);

		super.SetActions();
	}	
};
class RFFSHeli_Blackhawk_DriverDoor extends CarDoor {};
class RFFSHeli_Blackhawk_CodriverDoor extends CarDoor {};
class RFFSHeli_Blackhawk_Cargo1 extends CarDoor {};
class RFFSHeli_Blackhawk_Cargo2 extends CarDoor {};
class RFFSHeli_Blackhawk_Cargo3 extends CarDoor {};
class RFFSHeli_Blackhawk_Cargo4 extends CarDoor {};

class RFFSHeli_Blackhawk_Wreck extends Wreck_UH1Y {};







