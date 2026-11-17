class RFFSHeli_AS350 extends RFFSHeli_base
{
	//Exhaust smoke points
	protected ref RFFSHeli_HeliConfig m_heli_config;

	//string c_slidedoor_open_soundset;
	//string c_slidedoor_close_soundset;

	void RFFSHeli_AS350() 
	{
		c_heli_wreck_model = "RFFSHeli_AS350_wreck";
		//c_startup_sound_length = 26960.0;		//Length in miliseconds of the heli startup sound
		//c_shutdown_sound_length = 28090.0;	//Length in miliseconds of the heli shutdown sound35.667;
		//c_startup_soundset = "RFFSHeli_AS350_Startup_SoundSet";
		//c_engine_soundset = "RFFSHeli_AS350_Engine_SoundSet";
		//c_shutdown_soundset = "RFFSHeli_AS350_Shutdown_SoundSet";
		//c_slidedoor_open_soundset = "RFFSHeli_AS350_SlideDoor_Open_SoundSet";
		//c_slidedoor_close_soundset = "RFFSHeli_AS350_SlideDoor_Close_SoundSet";
		
		DetermineGauges();
	}
	
	void ~RFFSHeli_AS350()
	{
		//Gotta clean up sounds if/when helicopter is deleted
		if (m_heli_startup_wave) m_heli_startup_wave.Stop();
		if (m_heli_shutdown_wave) m_heli_shutdown_wave.Stop();
		if (m_heli_engine_wave) m_heli_engine_wave.Stop();
		if (m_heli_enginedamaged_wave) m_heli_enginedamaged_wave.Stop();
		if (m_heli_klaxon_wave) m_heli_klaxon_wave.Stop();
		if (m_engine_badlydamaged_smoke) m_engine_badlydamaged_smoke.Stop();
		if (m_engine_ruined_smoke) m_engine_ruined_smoke.Stop();

		if (GetGame() && GetGame().IsServer() && c_print_damage_debug && this)
			RFFSLog().Log("DESPAWN", "" + this + " despawned from " + this.GetPosition());
	}

	//Set the 3PP camera distance
	override float GetTransportCameraDistance()			
	{
		return 18.0;
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
			float BankTurnCoeff = 130.0;
			float SpeedFactor = 100.0;
			float AntiTorqueThrustRate = 100.0;
			float CyclicThrustRate = 100.0;
			float CollectiveThrustRate = 100.0;
			float MaximumRotationalRate = 100.0;
			float CyclicDampeningRate = 100.0;
			
			//Retrieve and apply the Heli Config file settings
			string heli_type = "RFFSHeli_AS350";
			
			RFFSHeli_HeliConfigData DefaultHeliConfigData = new RFFSHeli_HeliConfigData(FuelCapacity, FuelConsumptionRate, TrainerMode, HasHydraulics, SimpleCollective, MaximumAirspeed, MaximumAltitude, MaximumClimbRate, AerodynamicDrag, BankTurnCoeff, SpeedFactor, AntiTorqueThrustRate, CyclicThrustRate, CollectiveThrustRate, MaximumRotationalRate, CyclicDampeningRate);

			m_heli_config = new RFFSHeli_HeliConfig(heli_type, DefaultHeliConfigData);
			ApplyHeliConfig(heli_type, m_heli_config);	

		}
	}

	override void AttachHeliParts()
	{
		if (!m_heli_parts_added)
		{
			EntityAI entity_AS350;
			if ( Class.CastTo(entity_AS350, this) )
			{
				entity_AS350.GetInventory().CreateAttachment( "RFFSHeli_AS350_Driverdoor" );
				entity_AS350.GetInventory().CreateAttachment( "RFFSHeli_AS350_Codriverdoor" );
				entity_AS350.GetInventory().CreateAttachment( "RFFSHeli_AS350_Cargo1" );
				entity_AS350.GetInventory().CreateAttachment( "RFFSHeli_AS350_Cargo2" );
				m_heli_parts_added = true;
				m_attachement_timer = GetGame().GetTime();
			}			
		}

	}

	//Update animation values for cockpit gauges specific to this heli
	override void UpdateCockpitGauges()		
	{
		super.UpdateCockpitGauges();
		
		//Had to do a second needle for AGL as it is a 2 phase scale
		if (m_agl_altitude_ft > 100.0)
		{
			SetAnimationPhase("agl_ft_sm_hide", 1);
			SetAnimationPhase("agl_ft_lg_hide", 0);
		}
		else
		{
			SetAnimationPhase("agl_ft_sm_hide", 0);
			SetAnimationPhase("agl_ft_lg_hide", 1);			
		}

		//Had to do 3 needles for Vertical Speed (VS) as it is a 2 phase scale in both directions		
		if (m_vertical_speed_fpm > 2000.0)
		{
			SetAnimationPhase("vs_ft_hide", 1);
			SetAnimationPhase("vs_ft_up_hide", 0);		
		}
		if (m_vertical_speed_fpm < -2000.0)
		{
			SetAnimationPhase("vs_ft_hide", 1);
			SetAnimationPhase("vs_ft_down_hide", 0);		
		}
		if (m_vertical_speed_fpm < 2000.0001 && m_vertical_speed_fpm > -2000.0001)
		{
			SetAnimationPhase("vs_ft_hide", 0);
			SetAnimationPhase("vs_ft_up_hide", 1);	
			SetAnimationPhase("vs_ft_down_hide", 1);		
		}			
		SetSynchDirty();		
	}

	//Make sure that non-removalbe attachment can't be removed
	override bool CanReleaseAttachment( EntityAI attachment )											
	{
		if( !super.CanReleaseAttachment( attachment ) )				
			return false;

		string attType = attachment.GetType();
		if( attType.Contains("RFFSHeli_AS350")) return false;

		return true;																	
	}
	
	override int GetCarDoorsState( string slotType )   
	{
		CarDoor carDoor;
		Class.CastTo( carDoor, FindAttachmentBySlotName( slotType ) );  
		if ( !carDoor ) return CarDoorState.DOORS_MISSING;							
	
		switch( slotType )
		{
			case "AS350_doors_driver":								
				if ( GetAnimationPhase("DoorsDriver") > 0.5 )			
					return CarDoorState.DOORS_OPEN;						
				else
					return CarDoorState.DOORS_CLOSED;					
			break;
			
			case "AS350_doors_codriver":
				if ( GetAnimationPhase("DoorsCoDriver") > 0.5 )
					return CarDoorState.DOORS_OPEN;
				else
					return CarDoorState.DOORS_CLOSED;
			break;
				
			case "AS350_doors_cargo1":
				if ( GetAnimationPhase("DoorsCargo1") > 0.5 )
					return CarDoorState.DOORS_OPEN;
				else
					return CarDoorState.DOORS_CLOSED;
			break;
				
			case "AS350_doors_cargo2":
				if ( GetAnimationPhase("DoorsCargo2") > 0.5 )
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
				if ( GetCarDoorsState( "AS350_doors_driver" ) == CarDoorState.DOORS_CLOSED )  
					return false;																  

				return true;																	  
			break;
			
			case 1:
				if ( GetCarDoorsState( "AS350_doors_codriver" ) == CarDoorState.DOORS_CLOSED )
					return false;

				return true;
			break;
			
			case 2:
				if ( GetCarDoorsState( "AS350_doors_cargo1" ) == CarDoorState.DOORS_CLOSED )
					return false;

				return true;				
			break;
			
			case 3:
				return false;
			break;
			
			case 4:
				return false;
			break;
			
			case 5:
				if ( GetCarDoorsState( "AS350_doors_cargo2" ) == CarDoorState.DOORS_CLOSED )
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
		case 5:
			return "doors_cargo2";
		break;	

		}
		return super.GetDoorSelectionNameFromSeatPos(posIdx);
	}
	
	override string GetDoorInvSlotNameFromSeatPos(int posIdx)
	{
		switch( posIdx )
		{
		case 0:
			return "AS350_doors_driver";
		break;
		case 1:
			return "AS350_doors_codriver";
		break;
		
		case 2:
			return "AS350_doors_cargo1";
		break;
	
		case 5:
			return "AS350_doors_cargo2";
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

			case 2:
				return "pos_cargo1";
			break;
			
			case 5:
				return "pos_cargo4";
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
		case "doors_cargo1":
			return "DoorsCargo1";
		case "doors_cargo2":
			return "DoorsCargo2";
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
			return "seat_con_1_3";
		break;
		case "seat_cargo3":
			return "seat_con_2_2";
		break;
		case "seat_cargo4":
			return "seat_con_2_3";
		break;
		}
		
		return "";
	}

	override bool CanReachSeatFromSeat( int currentSeat, int nextSeat )
	{
//		switch( currentSeat )
//		{
//		case 0:
//			if ( nextSeat == 1)
//				return true;
//			break;
//		case 1:
//			if ( nextSeat == 0 )
//				return true;
//			break;
//		case 2:
//			if ( nextSeat == 3 )
//				return true;
//			break;
//		case 3:
//			if ( nextSeat == 2 || nextSeat == 4 )
//				return true;
//			break;
//		case 4:
//			if ( nextSeat == 3 || nextSeat == 5 )
//				return true;
//			break;
//		case 5:
//			if ( nextSeat == 4 )
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
			return DayZPlayerConstants.CMD_ACTIONMOD_DRIVER_DOOR_OPEN;
		case 3:
			return DayZPlayerConstants.CMD_ACTIONMOD_DRIVER_DOOR_OPEN;
		case 4:
			return DayZPlayerConstants.CMD_ACTIONMOD_CODRIVER_DOOROPEN;
		case 5:
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
			return DayZPlayerConstants.CMD_ACTIONMOD_DRIVER_DOOR_CLOSE;
		case 3:
			return DayZPlayerConstants.CMD_ACTIONMOD_DRIVER_DOOR_CLOSE;
		case 4:
			return DayZPlayerConstants.CMD_ACTIONMOD_CODRIVER_DOORCLOSE;
		case 5:
			return DayZPlayerConstants.CMD_ACTIONMOD_CODRIVER_DOORCLOSE;


		}
		return 0;
	}


	override bool CanReachDoorsFromSeat( string pDoorsSelection, int pCurrentSeat )
	{
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

		case 5:
			if (pDoorsSelection == "DoorsCargo2")
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
		
		if ( GetCarDoorsState("AS350_doors_driver") != CarDoorState.DOORS_OPEN && GetCarDoorsState( "AS350_doors_codriver" ) != CarDoorState.DOORS_OPEN && GetCarDoorsState( "AS350_doors_cargo1" ) != CarDoorState.DOORS_OPEN && GetCarDoorsState( "AS350_doors_cargo2" ) != CarDoorState.DOORS_OPEN )
			return false;
		
		return true;
	}

	override float OnSound( CarSoundCtrl ctrl, float oldValue )
	{
		switch ( ctrl )
		{
			case CarSoundCtrl.DOORS:
				float newValue = 0;
				
				if ( GetCarDoorsState( "AS350_doors_driver" ) == CarDoorState.DOORS_CLOSED )
					newValue += 0.25;

				if ( GetCarDoorsState( "AS350_doors_codriver" ) == CarDoorState.DOORS_CLOSED )
					newValue += 0.25;
				
				if ( GetCarDoorsState( "AS350_doors_cargo1" ) == CarDoorState.DOORS_CLOSED )
					newValue += 0.25;
				
				if ( GetCarDoorsState( "AS350_doors_cargo2" ) == CarDoorState.DOORS_CLOSED )
					newValue += 0.25;
				
				if ( newValue > 1 )
					newValue = 1;
			
				return newValue;
			break;
		}
		return oldValue;
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
			entity.GetInventory().CreateAttachment( "HeadlightH7" );
		};

		Fill( CarFluid.FUEL, c_fuel_max );
		Fill( CarFluid.OIL, 4.0 );
		if (m_engine_badlydamaged_smoke) m_engine_badlydamaged_smoke.Stop();
		SetBladesNoBlur();
	}
};

class RFFSHeli_AS350_wreck extends Wreck_UH1Y
{

}

class RFFSHeli_AS350_Driverdoor extends CarDoor {}

class RFFSHeli_AS350_Codriverdoor extends CarDoor {};
class RFFSHeli_AS350_Cargo1 extends CarDoor {};
class RFFSHeli_AS350_Cargo2 extends CarDoor {};






