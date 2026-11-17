class RFFSHeli_Blackhawk_Cargo extends RFFSHeli_Blackhawk
{
	
	void RFFSHeli_Blackhawk_Cargo() 
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
	
	void ~RFFSHeli_Blackhawk_Cargo()
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








