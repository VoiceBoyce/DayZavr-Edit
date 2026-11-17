//AUTHOR: Borizz.k aka [G.P]NOIZZ(MC2) https://vk.com/borizzk
//Special for ALEX.P Steam: https://steamcommunity.com/profiles/76561198179608217 , VK: https://vk.com/id317192524 
//22.02.2021

class Interceptor extends CarScript
{
	/* protected CamoNet 		m_CamoNet; */
	protected ref UniversalTemperatureSource m_UTSource;
	protected ref UniversalTemperatureSourceSettings m_UTSSettings;
	protected ref UniversalTemperatureSourceLambdaEngine m_UTSLEngine;
	//**
	protected ref EffVehicleSmoke m_exhaustFxR;
	
	protected int m_exhaustPtcFxR;
	protected vector m_exhaustPtcPosR;
	protected vector m_exhaustPtcDirR;
	
	//*** SECOND EXHAUST ***//
	bool m_IsSecondExhaustPresent = false;
	//*** SECOND EXHAUST END ***//

	void Interceptor()
	{
		m_dmgContactCoef = 0.0010;
		m_EngineStartOK 		= "Interceptor_engine_start_SoundSet";
		m_EngineStartBattery 	= "Interceptor_engine_failed_start_battery_SoundSet";
		m_EngineStartPlug 		= "Interceptor_engine_failed_start_sparkplugs_SoundSet";
		m_EngineStartFuel 		= "Interceptor_engine_failed_start_fuel_SoundSet";
		m_EngineStopFuel 		= "Interceptor_engine_stop_fuel_SoundSet";

		m_CarDoorOpenSound 		= "Truck_01_door_open_SoundSet";
		m_CarDoorCloseSound 	= "Truck_01_door_close_SoundSet";
		
		m_CarHornShortSoundName = "Offroad_Horn_Short_SoundSet";
		m_CarHornLongSoundName	= "Offroad_Horn_SoundSet";
		m_HornComponent.RegisterSound(VehicleHornMode.SHORT, "Offroad_02_Horn_Short_SoundSet");
		m_HornComponent.RegisterSound(VehicleHornMode.LONG, "Offroad_02_Horn_SoundSet");
		m_LightsComponent.RegisterLight("Front", new VehicleLightData(new InterceptorLightProfileFront()));
		m_LightsComponent.RegisterLight("Rear", new VehicleLightData(new InterceptorLightProfileRear()));
		
		SetEnginePos("0 0.7 1.2");
		//*** SECOND EXHAUST ***//
		if 	( MemoryPointExists("ptcExhaust1_end") )
		{
			m_exhaustPtcPosR = GetMemoryPointPos("ptcExhaust1_end");
			if ( MemoryPointExists("ptcExhaust1_start") )
			{
				vector exhaustStartR = GetMemoryPointPos("ptcExhaust1_start");
				
				if (m_exhaustPtcPosR && exhaustStartR)
				{
					m_IsSecondExhaustPresent = true;

					vector tempOriR = vector.Direction( exhaustStartR, m_exhaustPtcPosR);

					m_exhaustPtcDirR[0] = -tempOriR[2];
					m_exhaustPtcDirR[1] = tempOriR[1];
					m_exhaustPtcDirR[2] = tempOriR[0];
				
					m_exhaustPtcDirR = m_exhaustPtcDirR.Normalized().VectorToAngles();
				}
			}
		}
		//*** SECOND EXHAUST END ***//
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
	
	override void OnEngineStart()
	{
		super.OnEngineStart();

		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			m_UTSource.SetDefferedActive(true, 20.0);
		}
	}
	
	override void OnEngineStop()
	{
		super.OnEngineStop();

		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			m_UTSource.SetDefferedActive(false, 10.0);
		}
	}
	
	/* override void EOnPostSimulate(IEntity other, float timeSlice)
	{
		super.EOnPostSimulate(other, timeSlice);
		
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			if (m_UTSource.IsActive())
			{
				m_UTSource.Update(m_UTSSettings, m_UTSLEngine);
			}
		}
	} */
	
	//*** SECOND EXHAUST ***//
	override void EEDelete(EntityAI parent)
	{
		super. EEDelete(parent);
		if ( GetGame().IsClient() || !GetGame().IsMultiplayer() )
		{
			if (m_IsSecondExhaustPresent)
			{
				if ( SEffectManager.IsEffectExist( m_exhaustPtcFxR ) )
				{
					SEffectManager.Stop( m_exhaustPtcFxR );
				}
			}
		}
	}

	override void EOnPostSimulate(IEntity other, float timeSlice)
	{
		float m_localTime = m_Time + timeSlice ;

		super.EOnPostSimulate(other, timeSlice);
		
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			if (m_UTSource.IsActive())
			{
				m_UTSource.Update(m_UTSSettings, m_UTSLEngine);
			}
		}

		if ( m_localTime >= GameConstants.CARS_FLUIDS_TICK );
		{
			if ( EngineIsOn() )
			{
				if ( GetGame().IsClient() || !GetGame().IsMultiplayer() )
				{
					if (m_IsSecondExhaustPresent)
					{
						if ( !SEffectManager.IsEffectExist( m_exhaustPtcFxR ) )
						{
							m_exhaustFxR = new EffExhaustSmoke();
							m_exhaustPtcFxR = SEffectManager.PlayOnObject( m_exhaustFxR, this, m_exhaustPtcPosR, m_exhaustPtcDirR );
						}
						m_exhaustFxR.SetParticleStateLight();
					}
				}
			}
			else
			{
				if ( GetGame().IsClient() || !GetGame().IsMultiplayer() )
				{
					if (m_IsSecondExhaustPresent)
					{
						if ( SEffectManager.IsEffectExist( m_exhaustPtcFxR ) )
						{
							SEffectManager.Stop(m_exhaustPtcFxR);
						}
					}
				}
			}
		}
	}
	//*** SECOND EXHAUST END ***//

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
	
	override float GetTransportCameraDistance()
	{
		return 5.0;
	}
	
	override vector GetTransportCameraOffset()
	{
		return "0 1.4 0";
	}

	override int GetAnimInstance()
	{
		return VehicleAnimInstances.SEDAN;
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
	
	override bool CanReleaseAttachment(EntityAI attachment)
	{
		if (!super.CanReleaseAttachment(attachment))
		{
			return false;
		}
		
		if (EngineIsOn() || GetCarDoorsState("Interceptor_Doors_hood") == CarDoorState.DOORS_CLOSED)
		{
			string attType = attachment.GetType();
			if (attType == "CarRadiator" || attType == "CarBattery" || attType == "SparkPlug")
			{
				return false;
			}
		}

		return true;
	}
	
	override protected bool CanManipulateSpareWheel(string slotSelectionName)
	{
		return GetCarDoorsState("Interceptor_Doors_coDriver") != CarDoorState.DOORS_CLOSED;
	}

	override bool CanDisplayAttachmentCategory( string category_name )
	{
		if (!super.CanDisplayAttachmentCategory(category_name))
		{
			return false;
		}
	
		category_name.ToLower();		
		if (category_name.Contains("engine"))
		{
			if (GetCarDoorsState("Interceptor_Doors_hood") == CarDoorState.DOORS_CLOSED)
			{
				return false;
			}
		}
				
		return true;
	}
	
	override bool CanDisplayCargo()
	{
		if (!super.CanDisplayCargo())
		{
			return false;
		}
		
		if (GetCarDoorsState("Interceptor_Doors_Driver") == CarDoorState.DOORS_CLOSED)
		{
			return false;
		}
		
		return true;
	}

	override int GetCarDoorsState(string slotType)
	{
		CarDoor carDoor;
		
		Class.CastTo(carDoor, FindAttachmentBySlotName(slotType));
		if (!carDoor)
		{
			return CarDoorState.DOORS_MISSING;
		}

		switch (slotType)
		{
		case "Interceptor_Doors_Driver":
			return TranslateAnimationPhaseToCarDoorState("DoorsDriver");
			break;
			
		case "Interceptor_Doors_coDriver":
			return TranslateAnimationPhaseToCarDoorState("DoorsCoDriver");
			break;
	
		case "Interceptor_Doors_hood":
			return TranslateAnimationPhaseToCarDoorState("DoorsHood");
			break;
		
		}

		return CarDoorState.DOORS_MISSING;
	}	

	override bool CrewCanGetThrough( int posIdx )
	{
		switch( posIdx )
		{
			case 0:
				if ( GetCarDoorsState( "Interceptor_Doors_Driver" ) == CarDoorState.DOORS_CLOSED )
					return false;

				return true;
			break;
			
			case 1:
				if ( GetCarDoorsState( "Interceptor_Doors_coDriver" ) == CarDoorState.DOORS_CLOSED )
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
			return "Interceptor_Doors_Driver";
		break;
		case 1:
			return "Interceptor_Doors_coDriver";
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
		case "doors_hood":
			return "DoorsHood";
		
		}

		return "";
	}

	override bool IsVitalGlowPlug()
	{
		return false;
	}
	
	override bool NeedElectricitySourceDevice()
	{
		return true;
	}

	override EntityAI GetElectricitySourceDevice()
	{
		super.GetElectricitySourceDevice();

		return GetInventory().FindAttachment(CarBattery.SLOT_ID);
	}

	override string GetElectricitySourceDeviceType()
	{
		return "CarBattery";
	}

		override bool IsVitalSparkPlug()
	{
		return false;
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

	override void OnDebugSpawn()
	{
		EntityAI entity;
		EntityAI ent;
		ItemBase container;
		
		if ( Class.CastTo(entity, this) )
		{
			entity.GetInventory().CreateInInventory( "Interceptor_Doors_Driver" );
			entity.GetInventory().CreateInInventory( "Interceptor_Doors_coDriver" );
			entity.GetInventory().CreateInInventory( "Interceptor_Doors_hood" );	
			
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );

			entity.GetInventory().CreateInInventory( "CarBattery" );
			entity.GetInventory().CreateInInventory( "SparkPlug" );
			entity.GetInventory().CreateInInventory( "CarRadiator" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			//-----IN CAR CARGO
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );
			entity.GetInventory().CreateInInventory( "CarBattery" );
			entity.GetInventory().CreateInInventory( "SparkPlug" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			entity.GetInventory().CreateInInventory( "CarRadiator" );
			//--
			ent = entity.GetInventory().CreateInInventory( "Blowtorch" );
			entity = ent.GetInventory().CreateInInventory( "LargeGasCanister" );
			//--
			entity.GetInventory().CreateInInventory( "CanisterGasoline" );
			ent = entity.GetInventory().CreateInInventory( "CanisterGasoline" );
			if ( Class.CastTo(container, ent) )
			{
				container.SetLiquidType(LIQUID_WATER, true);
			}
			ent = entity.GetInventory().CreateInInventory( "Blowtorch" );
			if ( ent )
			{
				entity = ent.GetInventory().CreateInInventory( "LargeGasCanister" );
			}
		};

		Fill( CarFluid.FUEL, 50 );
		Fill( CarFluid.COOLANT, 6.0 );
		Fill( CarFluid.OIL, 4.0 );
	}
	/* override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);
		
		if ( item.IsKindOf ( "CamoNet" ) ) 
		{
			m_CamoNet = CamoNet.Cast(item);
			SetAnimationPhase( "Camonet", 0 );
			AddProxyPhysics( "camonet" );
		}
	}

	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);
				
		if ( item.IsKindOf ( "CamoNet" ) ) 
		{
			m_CamoNet = null;
			SetAnimationPhase( "Camonet", 1 );
			RemoveProxyPhysics( "camonet" );
		}
	} */
	
}

class Interceptor_blue extends Interceptor
{

	override void OnDebugSpawn()
	{
		EntityAI entity;
		EntityAI ent;
		ItemBase container;
		
		if ( Class.CastTo(entity, this) )
		{
			entity.GetInventory().CreateInInventory( "Interceptor_Doors_Driver_blue" );
			entity.GetInventory().CreateInInventory( "Interceptor_Doors_coDriver_blue" );
			entity.GetInventory().CreateInInventory( "Interceptor_Doors_hood_blue" );	
			
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );

			entity.GetInventory().CreateInInventory( "CarBattery" );
			entity.GetInventory().CreateInInventory( "SparkPlug" );
			entity.GetInventory().CreateInInventory( "CarRadiator" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			//-----IN CAR CARGO
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );
			entity.GetInventory().CreateInInventory( "CarBattery" );
			entity.GetInventory().CreateInInventory( "SparkPlug" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			entity.GetInventory().CreateInInventory( "CarRadiator" );
			//--
			ent = entity.GetInventory().CreateInInventory( "Blowtorch" );
			entity = ent.GetInventory().CreateInInventory( "LargeGasCanister" );
			//--
			entity.GetInventory().CreateInInventory( "CanisterGasoline" );
			ent = entity.GetInventory().CreateInInventory( "CanisterGasoline" );
			if ( Class.CastTo(container, ent) )
			{
				container.SetLiquidType(LIQUID_WATER, true);
			}
			ent = entity.GetInventory().CreateInInventory( "Blowtorch" );
			if ( ent )
			{
				entity = ent.GetInventory().CreateInInventory( "LargeGasCanister" );
			}
		};

		Fill( CarFluid.FUEL, 50 );
		Fill( CarFluid.COOLANT, 6.0 );
		Fill( CarFluid.OIL, 4.0 );
	}
};

class Interceptor_red extends Interceptor
{

	override void OnDebugSpawn()
	{
		EntityAI entity;
		EntityAI ent;
		ItemBase container;
		
		if ( Class.CastTo(entity, this) )
		{
			entity.GetInventory().CreateInInventory( "Interceptor_Doors_Driver_red" );
			entity.GetInventory().CreateInInventory( "Interceptor_Doors_coDriver_red" );
			entity.GetInventory().CreateInInventory( "Interceptor_Doors_hood_red" );	
			
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );

			entity.GetInventory().CreateInInventory( "CarBattery" );
			entity.GetInventory().CreateInInventory( "SparkPlug" );
			entity.GetInventory().CreateInInventory( "CarRadiator" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			//-----IN CAR CARGO
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );
			entity.GetInventory().CreateInInventory( "CarBattery" );
			entity.GetInventory().CreateInInventory( "SparkPlug" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			entity.GetInventory().CreateInInventory( "CarRadiator" );
			//--
			ent = entity.GetInventory().CreateInInventory( "Blowtorch" );
			entity = ent.GetInventory().CreateInInventory( "LargeGasCanister" );
			//--
			entity.GetInventory().CreateInInventory( "CanisterGasoline" );
			ent = entity.GetInventory().CreateInInventory( "CanisterGasoline" );
			if ( Class.CastTo(container, ent) )
			{
				container.SetLiquidType(LIQUID_WATER, true);
			}
			ent = entity.GetInventory().CreateInInventory( "Blowtorch" );
			if ( ent )
			{
				entity = ent.GetInventory().CreateInInventory( "LargeGasCanister" );
			}
		};

		Fill( CarFluid.FUEL, 50 );
		Fill( CarFluid.COOLANT, 6.0 );
		Fill( CarFluid.OIL, 4.0 );
	}
};

class Interceptor_yellow extends Interceptor
{

	override void OnDebugSpawn()
	{
		EntityAI entity;
		EntityAI ent;
		ItemBase container;
		
		if ( Class.CastTo(entity, this) )
		{
			entity.GetInventory().CreateInInventory( "Interceptor_Doors_Driver_yellow" );
			entity.GetInventory().CreateInInventory( "Interceptor_Doors_coDriver_yellow" );
			entity.GetInventory().CreateInInventory( "Interceptor_Doors_hood_yellow" );	
			
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );

			entity.GetInventory().CreateInInventory( "CarBattery" );
			entity.GetInventory().CreateInInventory( "SparkPlug" );
			entity.GetInventory().CreateInInventory( "CarRadiator" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			//-----IN CAR CARGO
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );
			entity.GetInventory().CreateInInventory( "CarBattery" );
			entity.GetInventory().CreateInInventory( "SparkPlug" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			entity.GetInventory().CreateInInventory( "CarRadiator" );
			//--
			ent = entity.GetInventory().CreateInInventory( "Blowtorch" );
			entity = ent.GetInventory().CreateInInventory( "LargeGasCanister" );
			//--
			entity.GetInventory().CreateInInventory( "CanisterGasoline" );
			ent = entity.GetInventory().CreateInInventory( "CanisterGasoline" );
			if ( Class.CastTo(container, ent) )
			{
				container.SetLiquidType(LIQUID_WATER, true);
			}
			ent = entity.GetInventory().CreateInInventory( "Blowtorch" );
			if ( ent )
			{
				entity = ent.GetInventory().CreateInInventory( "LargeGasCanister" );
			}
		};

		Fill( CarFluid.FUEL, 50 );
		Fill( CarFluid.COOLANT, 6.0 );
		Fill( CarFluid.OIL, 4.0 );
	}
};

class Interceptor_Black extends Interceptor
{

	override void OnDebugSpawn()
	{
		EntityAI entity;
		EntityAI ent;
		ItemBase container;
		
		if ( Class.CastTo(entity, this) )
		{
			entity.GetInventory().CreateInInventory( "Interceptor_Doors_Driver_Black" );
			entity.GetInventory().CreateInInventory( "Interceptor_Doors_coDriver_Black" );
			entity.GetInventory().CreateInInventory( "Interceptor_Doors_hood_Black" );	
			
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );

			entity.GetInventory().CreateInInventory( "CarBattery" );
			entity.GetInventory().CreateInInventory( "SparkPlug" );
			entity.GetInventory().CreateInInventory( "CarRadiator" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			//-----IN CAR CARGO
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );
			entity.GetInventory().CreateInInventory( "CarBattery" );
			entity.GetInventory().CreateInInventory( "SparkPlug" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			entity.GetInventory().CreateInInventory( "CarRadiator" );
			//--
			ent = entity.GetInventory().CreateInInventory( "Blowtorch" );
			entity = ent.GetInventory().CreateInInventory( "LargeGasCanister" );
			//--
			entity.GetInventory().CreateInInventory( "CanisterGasoline" );
			ent = entity.GetInventory().CreateInInventory( "CanisterGasoline" );
			if ( Class.CastTo(container, ent) )
			{
				container.SetLiquidType(LIQUID_WATER, true);
			}
			ent = entity.GetInventory().CreateInInventory( "Blowtorch" );
			if ( ent )
			{
				entity = ent.GetInventory().CreateInInventory( "LargeGasCanister" );
			}
		};

		Fill( CarFluid.FUEL, 50 );
		Fill( CarFluid.COOLANT, 6.0 );
		Fill( CarFluid.OIL, 4.0 );
	}
};

class Interceptor_Sonic extends Interceptor
{

	override void OnDebugSpawn()
	{
		EntityAI entity;
		EntityAI ent;
		ItemBase container;
		
		if ( Class.CastTo(entity, this) )
		{
			entity.GetInventory().CreateInInventory( "Interceptor_Doors_Driver_Sonic" );
			entity.GetInventory().CreateInInventory( "Interceptor_Doors_coDriver_Sonic" );
			entity.GetInventory().CreateInInventory( "Interceptor_Doors_hood_Sonic" );	
			
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );

			entity.GetInventory().CreateInInventory( "CarBattery" );
			entity.GetInventory().CreateInInventory( "SparkPlug" );
			entity.GetInventory().CreateInInventory( "CarRadiator" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			//-----IN CAR CARGO
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );
			entity.GetInventory().CreateInInventory( "CarBattery" );
			entity.GetInventory().CreateInInventory( "SparkPlug" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			entity.GetInventory().CreateInInventory( "CarRadiator" );
			//--
			ent = entity.GetInventory().CreateInInventory( "Blowtorch" );
			entity = ent.GetInventory().CreateInInventory( "LargeGasCanister" );
			//--
			entity.GetInventory().CreateInInventory( "CanisterGasoline" );
			ent = entity.GetInventory().CreateInInventory( "CanisterGasoline" );
			if ( Class.CastTo(container, ent) )
			{
				container.SetLiquidType(LIQUID_WATER, true);
			}
			ent = entity.GetInventory().CreateInInventory( "Blowtorch" );
			if ( ent )
			{
				entity = ent.GetInventory().CreateInInventory( "LargeGasCanister" );
			}
		};

		Fill( CarFluid.FUEL, 50 );
		Fill( CarFluid.COOLANT, 6.0 );
		Fill( CarFluid.OIL, 4.0 );
	}
};

class Interceptor_skull extends Interceptor
{

	override void OnDebugSpawn()
	{
		EntityAI entity;
		EntityAI ent;
		ItemBase container;
		
		if ( Class.CastTo(entity, this) )
		{
			entity.GetInventory().CreateInInventory( "Interceptor_Doors_Driver_skull" );
			entity.GetInventory().CreateInInventory( "Interceptor_Doors_coDriver_skull" );
			entity.GetInventory().CreateInInventory( "Interceptor_Doors_hood_skull" );	
			
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );

			entity.GetInventory().CreateInInventory( "CarBattery" );
			entity.GetInventory().CreateInInventory( "SparkPlug" );
			entity.GetInventory().CreateInInventory( "CarRadiator" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			//-----IN CAR CARGO
			entity.GetInventory().CreateInInventory( "Interceptor_Wheel" );
			entity.GetInventory().CreateInInventory( "CarBattery" );
			entity.GetInventory().CreateInInventory( "SparkPlug" );
			entity.GetInventory().CreateInInventory( "HeadlightH7" );
			entity.GetInventory().CreateInInventory( "CarRadiator" );
			//--
			ent = entity.GetInventory().CreateInInventory( "Blowtorch" );
			entity = ent.GetInventory().CreateInInventory( "LargeGasCanister" );
			//--
			entity.GetInventory().CreateInInventory( "CanisterGasoline" );
			ent = entity.GetInventory().CreateInInventory( "CanisterGasoline" );
			if ( Class.CastTo(container, ent) )
			{
				container.SetLiquidType(LIQUID_WATER, true);
			}
			ent = entity.GetInventory().CreateInInventory( "Blowtorch" );
			if ( ent )
			{
				entity = ent.GetInventory().CreateInInventory( "LargeGasCanister" );
			}
		};

		Fill( CarFluid.FUEL, 50 );
		Fill( CarFluid.COOLANT, 6.0 );
		Fill( CarFluid.OIL, 4.0 );
	}
};
