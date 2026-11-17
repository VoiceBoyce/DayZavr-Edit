class Candy_MustFrank extends CarScript
{
	protected ref UniversalTemperatureSource m_UTSource;
	protected ref UniversalTemperatureSourceSettings m_UTSSettings;
	protected ref UniversalTemperatureSourceLambdaEngine m_UTSLEngine;
	

	void Candy_MustFrank()
	{
		m_dmgContactCoef = 0.000;
		m_EngineStartOK         = "Candy_MustFrank_engine_ext_start_SoundSet";
		m_EngineStartBattery 	= "offroad_engine_failed_start_battery_SoundSet";
		m_EngineStartPlug 		= "offroad_engine_failed_start_sparkplugs_SoundSet";
		m_EngineStartFuel 		= "offroad_engine_failed_start_fuel_SoundSet";
		m_EngineStopFuel 		= "offroad_engine_stop_fuel_SoundSet";

		m_CarDoorOpenSound = "Candy_MustFrank_door_open_SoundSet";
		m_CarDoorCloseSound = "Candy_MustFrank_door_close_SoundSet";
		m_CarSeatShiftInSound 	= "Offroad_SeatShiftIn_SoundSet";
		m_CarSeatShiftOutSound 	= "Offroad_SeatShiftOut_SoundSet";
		
		m_CarHornShortSoundName = "Offroad_Horn_Short_SoundSet";
		m_CarHornLongSoundName	= "Offroad_Horn_SoundSet";
		m_HornComponent.RegisterSound(VehicleHornMode.SHORT, "Offroad_Horn_Short_SoundSet");
		m_HornComponent.RegisterSound(VehicleHornMode.LONG, "Offroad_Horn_SoundSet");
		m_LightsComponent.RegisterLight("Front", new VehicleLightData(new OffroadHatchbackLightProfileFront()));
		m_LightsComponent.RegisterLight("Rear", new VehicleLightData(new OffroadHatchbackLightProfileRear()));
		
	}
	
	override void EEInit()
	{		
		super.EEInit();
		
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
 			m_UTSSettings 					= new UniversalTemperatureSourceSettings();
			m_UTSSettings.m_ManualUpdate 	= true;
			m_UTSSettings.m_TemperatureMin	= 0;
			m_UTSSettings.m_TemperatureMax	= 30;
			m_UTSSettings.m_RangeFull		= 0.5;
			m_UTSSettings.m_RangeMax		= 2;
			m_UTSSettings.m_TemperatureCap	= 25;
			
			m_UTSLEngine					= new UniversalTemperatureSourceLambdaEngine();
			m_UTSource						= new UniversalTemperatureSource(this, m_UTSSettings, m_UTSLEngine);
		}		
	}
	
	override float GetActionDistanceCoolant()
	{
		return 6.0;
	}
	
	override float GetActionDistanceFuel()
	{
		return 4.0;
	}

	override float GetActionDistanceOil()
	{
		return 4.0;
	}
	
	override float GetActionDistanceBrakes()
	{
		return 4.0;
	}
	
	override int GetAnimInstance()
	{
		return VehicleAnimInstances.SEDAN;
	}

	override float GetTransportCameraDistance()
	{
		return 5.0;
	}
	
	override vector GetTransportCameraOffset()
	{
		return "0 1.5 0";
	}
	
	override protected bool CanManipulateSpareWheel(string slotSelectionName)
	{
		return GetCarDoorsState("Candy_MustFrank_Doors_Trunk") != CarDoorState.DOORS_CLOSED;
	}
	override int GetSeatAnimationType( int posIdx )
	{
		switch( posIdx )
		{
		case 0:
			return DayZPlayerConstants.VEHICLESEAT_DRIVER;
		case 1:
			return DayZPlayerConstants.VEHICLESEAT_CODRIVER;
		}

		return 0;
	}
	
	int SELECTION_ID_light_Offroad = GetHiddenSelectionIndex("light_offroad");
    int SELECTION_ID_light_Offroad2 = GetHiddenSelectionIndex("light_offroad2");
	
	override protected void TailLightsShineOn()
	{
		super.TailLightsShineOn();

		string material = ConfigGetString("OffroadReflectorMatOn");
		
		if (material)
		{
			SetObjectMaterial(SELECTION_ID_light_Offroad, material);
		}
		string material2 = ConfigGetString("OffroadReflectorMatOn2");
		
		if (material2)
		{
			SetObjectMaterial(SELECTION_ID_light_Offroad2, material2);
		}
	};

	override protected void TailLightsShineOff()
	{
		super.TailLightsShineOff();

		string material = ConfigGetString("OffroadReflectorMatOff");
		
		if (material)
		{
			SetObjectMaterial(SELECTION_ID_light_Offroad, material);
		}
		
		string material2 = ConfigGetString("OffroadReflectorMatOff2");
		
		if (material2)
		{
			SetObjectMaterial(SELECTION_ID_light_Offroad2, material2);
		}
	};
	
	override bool CanDisplayCargo()
	{
		if ( !super.CanDisplayCargo() )
			return false;
		
		if ( GetCarDoorsState("Candy_MustFrank_Doors_Trunk") == CarDoorState.DOORS_CLOSED )
			return false;
		
		return true;
	}
	
	override bool CanReleaseAttachment( EntityAI attachment )
	{
		if( !super.CanReleaseAttachment( attachment ) )
			return false;
		
		string attType = attachment.GetType();
		
		if ( EngineIsOn() || GetCarDoorsState("") == CarDoorState.DOORS_CLOSED )
		{
			if ( attType == "CarRadiator" || attType == "CarBattery" || attType == "SparkPlug"  )
				return false;
		}
		
		return true;
	}	
	
	
	override int GetCarDoorsState( string slotType )
	{
		CarDoor carDoor;

		Class.CastTo( carDoor, FindAttachmentBySlotName( slotType ) );
		if ( !carDoor )
			return CarDoorState.DOORS_MISSING;
	
		switch( slotType )
		{
			case "Candy_MustFrank_Doors_Driver":
				if ( GetAnimationPhase("DoorsDriver") > 0.5 )
					return CarDoorState.DOORS_OPEN;
				else
					return CarDoorState.DOORS_CLOSED;

			break;
			
			case "Candy_MustFrank_Doors_CoDriver":
				if ( GetAnimationPhase("DoorsCoDriver") > 0.5 )
					return CarDoorState.DOORS_OPEN;
				else
					return CarDoorState.DOORS_CLOSED;

			break;
			
			case "Candy_MustFrank_Doors_Trunk":
				if ( GetAnimationPhase("DoorsTrunk") > 0.5 )
					return CarDoorState.DOORS_OPEN;
				else
					return CarDoorState.DOORS_CLOSED;
			break;
			
			case "Candy_MustFrank_Doors_Trunk0":
				if ( GetAnimationPhase("DoorsTrunk0") > 0.5 )
					return CarDoorState.DOORS_OPEN;
				else
					return CarDoorState.DOORS_CLOSED;
			break;
			
			case "Candy_MustFrank_Doors_Hood":
				if ( GetAnimationPhase("DoorsHood") > 0.5 )
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
				if ( GetCarDoorsState( "Candy_MustFrank_Doors_Driver" ) == CarDoorState.DOORS_CLOSED )
					return false;

				return true;
			break;
			
			case 1:
				if ( GetCarDoorsState( "Candy_MustFrank_Doors_CoDriver" ) == CarDoorState.DOORS_CLOSED )
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
			return "Candy_MustFrank_Doors_Driver";
		break;
		case 1:
			return "Candy_MustFrank_Doors_CoDriver";
		break;
		}
		
		return super.GetDoorInvSlotNameFromSeatPos(posIdx);
	}

	override float OnSound(CarSoundCtrl ctrl, float oldValue)
	{
		float tempCap = 0.0;

		switch (ctrl)
		{
		case CarSoundCtrl.DOORS:
			float newValue = 0;
			if (GetCarDoorsState("Candy_MustFrank_Doors_Driver") == CarDoorState.DOORS_CLOSED)
			{
				newValue += 0.5;
			}

			if (GetCarDoorsState("Candy_MustFrank_Doors_CoDriver") == CarDoorState.DOORS_CLOSED)
			{
				newValue += 0.5;
			}
		
			 if (GetHealthLevel("WindowFront") == GameConstants.STATE_RUINED)
            {
                newValue -= 0.6;
            }

            if (GetHealthLevel("WindowLR") == GameConstants.STATE_RUINED)
            {
                newValue -= 0.2;
            }
        
            if (GetHealthLevel("WindowRR") == GameConstants.STATE_RUINED)
            {
                newValue -= 0.2;
            }

			return Math.Clamp(newValue, 0, 1);
		break;
		}

		return super.OnSound(ctrl, oldValue);
	}
	
	override string GetAnimSourceFromSelection( string selection )
	{
		switch( selection )
		{
		case "doors_driver":
			return "DoorsDriver";
		case "doors_codriver":
			return "DoorsCoDriver";
		case "doors_trunk":
			return "DoorsTrunk";
		case "doors_trunk0":
			return "DoorsTrunk0";
		case "doors_hood":
			return "DoorsHood";
		}

		return "";
	}

	override bool NeedElectricitySourceDevice() { return true; }
	override EntityAI GetElectricitySourceDevice() { super.GetElectricitySourceDevice(); return GetInventory().FindAttachment(CarBattery.SLOT_ID); }
	override string GetElectricitySourceDeviceType() { return "CarBattery"; }

	override bool IsVitalGlowPlug()
	{
		return false;
	}
	
	override bool IsVitalRadiator()
	{
		return false;
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
		switch( currentSeat )
		{
		case 0:
			if ( nextSeat == 1 )
				return false;
			break;
		case 1:
			if ( nextSeat == 0 )
				return false;
			break;
		}
		
		return false;
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
		}
		
		return false;		
	}

	int ATTClothingSlot1 = InventorySlots.GetSlotIdFromString("Truck_01_WoodenCrate1");
	int ATTClothingSlot2 = InventorySlots.GetSlotIdFromString("Truck_01_WoodenCrate2");
	
    override void EEItemAttached(EntityAI item, string slot_name)
    {
        super.EEItemAttached(item, slot_name);
        
          if(slot_name == "Truck_01_WoodenCrate1")
          {
              GetInventory().SetSlotLock(ATTClothingSlot1, true);
          }
          else if(slot_name == "Truck_01_WoodenCrate2")
          {
              GetInventory().SetSlotLock(ATTClothingSlot2, true);
          }
              
    };

	
	override void OnDebugSpawn()
	{
		EntityAI entity;
		
		if ( Class.CastTo(entity, this) )
		{
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_Driver" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_CoDriver" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_Trunk" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_Trunk0" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_Hood" );
			entity.GetInventory().CreateInInventory( "Zavr_CrateAVTO" );
			entity.GetInventory().CreateInInventory( "CarBattery" );
			entity.GetInventory().CreateInInventory( "SparkPlug" );
			entity.GetInventory().CreateInInventory( "CarRadiator" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
		};

		Fill( CarFluid.FUEL, 50 );
		Fill( CarFluid.COOLANT, 6.0 );
		Fill( CarFluid.OIL, 4.0 );
	}
    
}

class Candy_MustFrank_White extends Candy_MustFrank
{
	override void OnDebugSpawn()
	{
		EntityAI entity;
		
		if ( Class.CastTo(entity, this) )
		{
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_Driver_White" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_CoDriver_White" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_Trunk_White" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_Trunk0_White" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_Hood_White" );
			entity.GetInventory().CreateInInventory( "CarBattery" );
			entity.GetInventory().CreateInInventory( "SparkPlug" );
			entity.GetInventory().CreateInInventory( "Zavr_CrateAVTO" );
			entity.GetInventory().CreateInInventory( "CarRadiator" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
		};

		Fill( CarFluid.FUEL, 50 );
		Fill( CarFluid.COOLANT, 6.0 );
		Fill( CarFluid.OIL, 4.0 );
	}
}

class Candy_MustFrank_NoGun extends Candy_MustFrank
{
	override void OnDebugSpawn()
	{
		EntityAI entity;
		
		if ( Class.CastTo(entity, this) )
		{
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_Driver" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_CoDriver" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_Trunk" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_Trunk0" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_Hood" );
			entity.GetInventory().CreateInInventory( "CarBattery" );
			entity.GetInventory().CreateInInventory( "SparkPlug" );
			entity.GetInventory().CreateInInventory( "Zavr_CrateAVTO" );
			entity.GetInventory().CreateInInventory( "CarRadiator" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
		};

		Fill( CarFluid.FUEL, 50 );
		Fill( CarFluid.COOLANT, 6.0 );
		Fill( CarFluid.OIL, 4.0 );
	}
}

class Candy_MustFrank_NoGun_White extends Candy_MustFrank_NoGun
{
	override void OnDebugSpawn()
	{
		EntityAI entity;
		
		if ( Class.CastTo(entity, this) )
		{
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_Driver_White" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_CoDriver_White" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_Trunk_White" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_Trunk0_White" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_Hood_White" );
			entity.GetInventory().CreateInInventory( "CarBattery" );
			entity.GetInventory().CreateInInventory( "Zavr_CrateAVTO" );
			entity.GetInventory().CreateInInventory( "SparkPlug" );
			entity.GetInventory().CreateInInventory( "CarRadiator" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
		};

		Fill( CarFluid.FUEL, 50 );
		Fill( CarFluid.COOLANT, 6.0 );
		Fill( CarFluid.OIL, 4.0 );
	}
}




class Candy_MustFrank_NoGun_White_icecoffee extends Candy_MustFrank_NoGun
{
	override void OnDebugSpawn()
	{
		EntityAI entity;
		
		if ( Class.CastTo(entity, this) )
		{
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_Driver_White" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_CoDriver_White" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_Trunk_White" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_Trunk0_White" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_Hood_White" );
			entity.GetInventory().CreateInInventory( "CarBattery" );
			entity.GetInventory().CreateInInventory( "Zavr_CrateAVTO" );
			entity.GetInventory().CreateInInventory( "Zavr_CrateAVTO" );
			entity.GetInventory().CreateInInventory( "SparkPlug" );
			entity.GetInventory().CreateInInventory( "CarRadiator" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
		};

		Fill( CarFluid.FUEL, 50 );
		Fill( CarFluid.COOLANT, 6.0 );
		Fill( CarFluid.OIL, 4.0 );
	}
}



class Candy_MustFrank_konstanting extends Candy_MustFrank
{
	override void OnDebugSpawn()
	{
		EntityAI entity;
		
		if ( Class.CastTo(entity, this) )
		{
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_Driver_konstanting" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_CoDriver_konstanting" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_Trunk_konstanting" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_Trunk0_konstanting" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_Hood_konstanting" );
			entity.GetInventory().CreateInInventory( "CarBattery" );
			entity.GetInventory().CreateInInventory( "Zavr_CrateAVTO" );
			entity.GetInventory().CreateInInventory( "Zavr_CrateAVTO" );
			entity.GetInventory().CreateInInventory( "SparkPlug" );
			entity.GetInventory().CreateInInventory( "CarRadiator" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
		};

		Fill( CarFluid.FUEL, 50 );
		Fill( CarFluid.COOLANT, 6.0 );
		Fill( CarFluid.OIL, 4.0 );
	}
}




class Candy_MustFrank_777VOR777 extends Candy_MustFrank
{
	override void OnDebugSpawn()
	{
		EntityAI entity;
		
		if ( Class.CastTo(entity, this) )
		{
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_Driver_777VOR777" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_CoDriver_777VOR777" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_Trunk_777VOR777" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_Trunk0_777VOR777" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_Hood_777VOR777" );
			entity.GetInventory().CreateInInventory( "CarBattery" );
			entity.GetInventory().CreateInInventory( "Zavr_CrateAVTO" );
			entity.GetInventory().CreateInInventory( "SparkPlug" );
			entity.GetInventory().CreateInInventory( "CarRadiator" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
		};

		Fill( CarFluid.FUEL, 50 );
		Fill( CarFluid.COOLANT, 6.0 );
		Fill( CarFluid.OIL, 4.0 );
	}
}



class Candy_MustFrank_xVOLKOLAKx extends Candy_MustFrank
{
	override void OnDebugSpawn()
	{
		EntityAI entity;
		
		if ( Class.CastTo(entity, this) )
		{
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Wheel" );
			
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_Driver" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_CoDriver" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_Trunk" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_Trunk0" );
			entity.GetInventory().CreateInInventory( "Candy_MustFrank_Doors_Hood" );
			entity.GetInventory().CreateInInventory( "CarBattery" );
			entity.GetInventory().CreateInInventory( "Zavr_CrateAVTO" );
			entity.GetInventory().CreateInInventory( "Zavr_CrateAVTO" );
			entity.GetInventory().CreateInInventory( "SparkPlug" );
			entity.GetInventory().CreateInInventory( "CarRadiator" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
		};

		Fill( CarFluid.FUEL, 50 );
		Fill( CarFluid.COOLANT, 6.0 );
		Fill( CarFluid.OIL, 4.0 );
	}
}