modded class OOTS_AnomalyBase
{
	protected const ref array<typename> DAMAGEABLE_TYPES = {Ammunition_Base, PlayerBase, DayZCreature, OOTS_Item_Bolt, MagazineStorage}; //, "ZombieBase", "AnimalBase"

	protected ref AnomalyDamageManager m_AreaDamage;
	protected float m_RechargeTime = 1;
	protected float m_ActiveTime = 1;
	protected AnomalyDmgCfgJson m_DmgCfg;
	
	override void EEInit()
	{
		super.EEInit();
		/* SetLifetimeMax(10800);
		SetLifetime(10800); */
		
		m_DmgCfg = AnomalyDmgCfg.GetAnomaly(GetType());
		CreateAreaDamage();
	}
	
	override void EEDelete(EntityAI parent)
	{
		DestroyAreaDamage();
		
		super.EEDelete(parent);
	}

	float GetDmgCoef()
	{
		if(m_DmgCfg)
			return m_DmgCfg.GetDmgCoef();
		return 1;
	}
	
	void CreateAreaDamage()
	{
		//destroy area damage if some already exists
		DestroyAreaDamage();
	}
	
	void DestroyAreaDamage()
	{
		if ( m_AreaDamage ) 
		{
			m_AreaDamage.Destroy();
		}
	}
	
	void SendRPC()
	{
		ref Param1<float> p = new Param1<float>(m_ActiveTime + m_RechargeTime);
		GetGame().RPCSingleParam(this, OOTS_AnomalyBase.RPC_ACTIVE, p, true);
	}

	override void PostAreaDamageActions() 
	{
		DestroyAreaDamage();
		SendRPC();
		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( CreateAreaDamage, (m_ActiveTime + m_RechargeTime) * 1000, false);
	}
	
	/*protected array<typename> DamagebleStrToType(array<string> string_types)
	{
		array<typename> types = new array<typename>;
		foreach(string type: string_types)
		{
			types.Insert(type.ToType());
		}
		
		return types;
	}*/
}


//Electra
modded class Anomaly_Electra
{
	void Anomaly_Electra()
	{
		m_RechargeTime = 2.5;
		m_ActiveTime = 1.5;
	}
	
	override void CreateAreaDamage()
	{
		super.CreateAreaDamage();
		
		//create new area damage
		m_AreaDamage = new AnomalyDamageManager(this);
		m_AreaDamage.SetDamageComponentType(AreaDamageComponentTypes.HITZONE);
		m_AreaDamage.SetDamageableTypes(DAMAGEABLE_TYPES);
		//m_AreaDamage.SetExtents("-2 0 -2", "2 2 2");
		m_AreaDamage.SetHitZones({"Torso","LeftHand","LeftLeg","LeftFoot","RightHand","RightLeg","RightFoot"});
		m_AreaDamage.SetAmmoName("ElectraDmg");
		m_AreaDamage.Spawn();
		m_AreaDamage.SetCylinderExtents(2, 1.5);
		m_AreaDamage.SetSource("Electra");
		
	}
}

//Studen
modded class Anomaly_Studen
{
	
	void Anomaly_Studen()
	{
		m_RechargeTime = 1;
		m_ActiveTime = 0;
	}
	
	override void CreateAreaDamage()
	{
		super.CreateAreaDamage();
		
		//create new area damage
		m_AreaDamage = new AnomalyDamageManager(this);
		m_AreaDamage.SetDamageComponentType(AreaDamageComponentTypes.HITZONE);
		m_AreaDamage.SetDamageableTypes(DAMAGEABLE_TYPES);
		//m_AreaDamage.SetExtents("-2 0 -2", "2 2 2");
		m_AreaDamage.SetHitZones({"Torso","LeftHand","LeftLeg","LeftFoot","RightHand","RightLeg","RightFoot"});
		m_AreaDamage.SetAmmoName("StudenDmg");
		m_AreaDamage.Spawn();
		m_AreaDamage.SetCylinderExtents(1.8, 1);
		m_AreaDamage.SetSource("Studen");
	}
	
}

//Tramplin
modded class Anomaly_Tramplin
{
	void Anomaly_Tramplin()
	{
		m_RechargeTime = 3;
		m_ActiveTime = 2;
	}
	
	override void CreateAreaDamage()
	{
		super.CreateAreaDamage();
		
		//create new area damage
		m_AreaDamage = new AnomalyDamageManager(this);
		m_AreaDamage.SetDamageComponentType(AreaDamageComponentTypes.HITZONE);
		m_AreaDamage.SetDamageableTypes(DAMAGEABLE_TYPES);
		//m_AreaDamage.SetExtents("-2 0 -2", "2 1 2");
		m_AreaDamage.SetHitZones({"LeftLeg","LeftFoot","RightLeg","RightFoot"});
		m_AreaDamage.SetAmmoName("TramplinDmg");
		m_AreaDamage.Spawn();
		m_AreaDamage.SetCylinderExtents(1.8, 1);
		m_AreaDamage.SetSource("Tramplin");
	}
}

//Voronka
modded class Anomaly_Voronka
{
	private const int CHARGING_TIME = 2;
	private bool m_Phase_1 = false;
	
	void Anomaly_Voronka()
	{
		m_RechargeTime = 3;
		m_ActiveTime = 5;
	}
	
	override void CreateAreaDamage()
	{
		super.CreateAreaDamage();
		
		//create new area damage
		m_AreaDamage = new AnomalyDamageManager(this);
		m_AreaDamage.SetDamageComponentType(AreaDamageComponentTypes.HITZONE);
		m_AreaDamage.SetDamageableTypes(DAMAGEABLE_TYPES);
		//m_AreaDamage.SetExtents("-3 0 -3", "3 1 3");
		m_AreaDamage.SetHitZones({"Torso","LeftHand","LeftLeg","LeftFoot","RightHand","RightLeg","RightFoot"});
		m_AreaDamage.SetAmmoName("ZeroFakeDmg");
		m_AreaDamage.Spawn();
		m_AreaDamage.SetCylinderExtents(2.5, 1.5);
		m_AreaDamage.SetSource("Voronka");
		
		m_Phase_1 = true;
	}
	
	override void PostAreaDamageActions()
	{
		if(m_Phase_1)
		{
			super.PostAreaDamageActions();
			m_Phase_1 = false;
			GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( CreateChargedAreaDamage, CHARGING_TIME * 1000, false);
		}
		else
		{
			DestroyAreaDamage();
		}
		
	}
	
	private void CreateChargedAreaDamage()
	{
		m_AreaDamage = new AnomalyDamageManager(this);
		m_AreaDamage.SetDamageComponentType(AreaDamageComponentTypes.HITZONE);
		m_AreaDamage.SetDamageableTypes(DAMAGEABLE_TYPES);
		//m_AreaDamage.SetExtents("-2.5 0 -2.5", "2.5 2.5 2.5");
		m_AreaDamage.SetHitZones({"Torso","LeftHand","LeftLeg","LeftFoot","RightHand","RightLeg","RightFoot"});
		m_AreaDamage.SetAmmoName("VoronkaDmg");
		m_AreaDamage.Spawn();
		m_AreaDamage.SetCylinderExtents(3, 3);
		m_AreaDamage.SetSource("Voronka");
		
		GetGame().RPCSingleParam(this, Anomaly_Voronka.RPC_ACTIVE_2, null, true);
		
		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( DestroyAreaDamage, 1000, false);
	}
}

//Zharka
modded class Anomaly_Zharka
{
	ref array<Object> nearest_objects = new array<Object>;
	
	const float PARAM_HEAT_RADIUS = 10.0;
	const float PARAM_HEAT_THROUGH_AIR_COEF = 0.28; //0.035;
	const float TEMPERATURE = 1226.0;
	
	protected ref Timer m_HeatingTimer;
	
	void Anomaly_Zharka()
	{
		m_RechargeTime = 1;
		m_ActiveTime = 10;
		
		m_HeatingTimer = new Timer ( CALL_CATEGORY_SYSTEM );
	}
	
	override void CreateAreaDamage()
	{
		super.CreateAreaDamage();
		
		//create new area damage
		m_AreaDamage = new AnomalyDamageManager(this);
		m_AreaDamage.SetDamageComponentType(AreaDamageComponentTypes.HITZONE);
		m_AreaDamage.SetDamageableTypes(DAMAGEABLE_TYPES);
		//m_AreaDamage.SetExtents("-2 0 -2", "2 2 2");
		m_AreaDamage.SetHitZones({"Torso","LeftHand","LeftLeg","LeftFoot","RightHand","RightLeg","RightFoot"});
		m_AreaDamage.SetAmmoName("ZharkaDmg");
		m_AreaDamage.Spawn();
		m_AreaDamage.SetCylinderExtents(1.7, 2.5);
		m_AreaDamage.SetSource("Zharka");
	}
	
	override void PostAreaDamageActions()
	{
		super.PostAreaDamageActions();
		
		m_HeatingTimer.Run( 2, this, "TransferHeatToNearPlayers", NULL, true );
		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( StopHeating, m_ActiveTime * 1000, false);
	}
	
	private void StopHeating()
	{
		m_HeatingTimer.Stop();
		nearest_objects.Clear();
	}
	
	private void TransferHeatToNearPlayers()
	{		
		float distance;
		  
		GetGame().GetObjectsAtPosition ( GetPosition(), PARAM_HEAT_RADIUS, nearest_objects, null ); 
		  
		for ( int i = 0; i < nearest_objects.Count(); i++ )
		{
			Object nearest_object = nearest_objects.Get(i);
			  
			if ( nearest_object.IsInherited( PlayerBase ) )
			{
				PlayerBase player = PlayerBase.Cast( nearest_object );
				distance = vector.Distance( player.GetPosition(), GetPosition() );
				distance = Math.Max( distance, 0.1 ); //min distance cannot be 0 (division by zero)
				  
				float temperature = TEMPERATURE * ( PARAM_HEAT_THROUGH_AIR_COEF / distance );
				player.AddToEnvironmentTemperature( temperature );
			}
			else if ( nearest_object != this && nearest_object.IsInherited( ItemBase ) && nearest_object.GetParent() == null )
			{
				ItemBase item = ItemBase.Cast( nearest_object );
				float wetness = item.GetWet();
					  
				if ( wetness > 0 )
				{
					distance = vector.Distance( item.GetPosition(), GetPosition() );
					distance = Math.Max( distance, 0.1 ); //min distance cannot be 0 (division by zero)
					  
					wetness = wetness * ( PARAM_HEAT_THROUGH_AIR_COEF / distance );
					wetness = Math.Clamp( wetness, item.GetWetMin(), item.GetWetMax() );
					item.AddWet( -wetness );
				}
			}
		}
	}
}

//teleport
modded class Anomaly_Teleport_Entry
{	
	static const int ACTIVE_TIME = 5;
	private ref array<Anomaly_Teleport_Output> m_Outputs;
	
	void Anomaly_Teleport_Entry()
	{
		m_RechargeTime = 0;
		m_ActiveTime = ACTIVE_TIME;
		
		m_Outputs = new array<Anomaly_Teleport_Output>;
	}
	
	override void EEDelete(EntityAI parent)
	{	
		for(int i = 0; i < m_Outputs.Count(); i++)
		{
			Anomaly_Teleport_Output output = m_Outputs.Get(i);
			if(output)
			{
				GetGame().ObjectDelete(output);
			}
		}
		
		m_Outputs.Clear();
		
		super.EEDelete(parent);
	}
	
	override void EEInit()
	{
		//super.EEInit();

		Anomaly_Teleport_Entry.CreateVisionObject(GetPosition());
		
		GetAnomalyTeleportManager().RegisterEntry(this);
	}
	
	static void CreateVisionObject(vector pos)
	{
		Object v_object = GetGame().CreateObject("SmallStone", pos, false);
		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( RemoveVisionObject, 3000, false, v_object);
	}
	
	static void RemoveVisionObject(Object v_object)
	{
		GetGame().ObjectDelete(v_object);
	}
	
	override void CreateAreaDamage()
	{
		super.CreateAreaDamage();
		
		//create new area damage
		m_AreaDamage = new AnomalyDamageManager(this);
		m_AreaDamage.SetDamageComponentType(AreaDamageComponentTypes.HITZONE);
		m_AreaDamage.SetDamageableTypes(DAMAGEABLE_TYPES);
		//m_AreaDamage.SetExtents("-3 0 -3", "3 1 3");
		m_AreaDamage.SetHitZones({"Torso","LeftHand","LeftLeg","LeftFoot","RightHand","RightLeg","RightFoot"});
		m_AreaDamage.SetAmmoName("ZeroFakeDmg");
		m_AreaDamage.Spawn();
		m_AreaDamage.SetCylinderExtents(2, 2);
		m_AreaDamage.SetSource("Teleport");
	}
	
	override void PostAreaDamageActions()
	{
		if(m_Outputs.Count() == 0)
			return;
		
		super.PostAreaDamageActions();
	}
	
	void AddOutput(Anomaly_Teleport_Output teleport)
	{
		m_Outputs.Insert(teleport);
		if(m_Outputs.Count() == 1)
		{
			CreateAreaDamage();
			m_Show = true;
			SetSynchDirty();
		}
	}
	
	void Teleport(EntityAI entity)
	{
		if(m_Outputs.Count() > 0)
		{
			Anomaly_Teleport_Output teleport = m_Outputs.GetRandomElement();
			if(teleport)
			{
				Param1<float> p = new Param1<float>(Anomaly_Teleport_Entry.ACTIVE_TIME);
				GetGame().RPCSingleParam(teleport, OOTS_AnomalyBase.RPC_ACTIVE, p, true);
					
				//teleport				
				entity.SetPosition(teleport.GetPosition());
			}
		}
	}
	
	bool HasOutput()
	{
		if(m_Outputs.Count() > 0)
			return true;
		
		return false;
	}
}

modded class Anomaly_Teleport_Output
{	
	void Anomaly_Teleport_Output()
	{
		
	}
	
	override void EEInit()
	{
		super.EEInit();
		
		Anomaly_Teleport_Entry.CreateVisionObject(GetPosition());
		
		GetAnomalyTeleportManager().RegisterOutput(this);
	}
}

modded class Anomaly_Teleport_Dot
{
	override void EEInit()
	{
		super.EEInit();
		
		Anomaly_Teleport_Entry.CreateVisionObject(GetPosition());
		
		GetAnomalyTeleportManager().RegisterDot(this);
	}
}

modded class Anomaly_EMP_Blast
{
	private const int EMP_RADIUS = 7;
	
	void Anomaly_EMP_Blast()
	{
		m_RechargeTime = 2.5;
		m_ActiveTime = 1.5;
	}
	
	override void CreateAreaDamage()
	{
		super.CreateAreaDamage();
		
		//create new area damage
		m_AreaDamage = new AnomalyDamageManager(this);
		m_AreaDamage.SetDamageComponentType(AreaDamageComponentTypes.HITZONE);
		m_AreaDamage.SetDamageableTypes(DAMAGEABLE_TYPES);
		//m_AreaDamage.SetExtents("-2 0 -2", "2 2 2");
		m_AreaDamage.SetHitZones({"Torso","LeftHand","LeftLeg","LeftFoot","RightHand","RightLeg","RightFoot"});
		m_AreaDamage.SetAmmoName("EmpDmg");
		m_AreaDamage.Spawn();
		m_AreaDamage.SetCylinderExtents(3, 2);
		m_AreaDamage.SetSource("EMP_Blast");
		
	}
	void EMPBlast()
	{
		
		//Param1<float> p = new Param1<float>(m_ActiveTime);
		//GetGame().RPCSingleParam(this, OOTS_AnomalyBase.RPC_ACTIVE, p, true);
		
		array<Object> objects = new array<Object>;
		GetGame().GetObjectsAtPosition3D(GetPosition(), EMP_RADIUS, objects, null);
		
		if(objects.Count() > 0)
		{
			foreach(Object f_object: objects)
			{
				EntityAI entity;
				if(Class.CastTo(entity, f_object))
				{
					InspectItem(entity);
				}
			}
		}
		
		/* GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( RemoveObject, (m_ActiveTime + m_RechargeTime) * 1000, false); */
	}
	
	private void InspectItem(EntityAI parent)
	{
		if(!parent)
			return;

		if(parent.HasEnergyManager())
		{
			parent.GetCompEM().SetEnergy(0);
			parent.OnEnergyConsumed();
		}

		EntityAI entity;
		
		if(parent.IsMan())
		{
			PlayerBase player;
			if(Class.CastTo(player, parent))
			{
				entity = player.GetItemInHands();
				InspectItem(entity);
			}
		}

		if(parent.GetInventory())
		{
			if(parent.GetInventory().AttachmentCount() > 0)
			{
				for(int i = 0; i < parent.GetInventory().AttachmentCount(); i++)
				{
					entity = parent.GetInventory().GetAttachmentFromIndex(i);
					InspectItem(entity);
				}
			}
			
			if(parent.HasAnyCargo())
			{
				CargoBase cargo = parent.GetInventory().GetCargo();
				for(int j = 0; j < cargo.GetItemCount(); j++)
				{
					entity = cargo.GetItem(j);
					InspectItem(entity);
				}
			}
		}
	}
}

modded class Anomaly_Steklo
{
	void Anomaly_Steklo()
	{
		m_RechargeTime = 2;
		m_ActiveTime = 3;
	}
	
	override void CreateAreaDamage()
	{
		super.CreateAreaDamage();
		
		//create new area damage
		m_AreaDamage = new AnomalyDamageManager(this);
		m_AreaDamage.SetDamageComponentType(AreaDamageComponentTypes.HITZONE);
		m_AreaDamage.SetDamageableTypes(DAMAGEABLE_TYPES);
		//m_AreaDamage.SetExtents("-2 0 -2", "2 2 2");
		m_AreaDamage.SetHitZones({"Torso","LeftHand","LeftLeg","LeftFoot","RightHand","RightLeg","RightFoot"});
		m_AreaDamage.SetAmmoName("StekloDmg");
		m_AreaDamage.Spawn();
		m_AreaDamage.SetCylinderExtents(5, 5);
		m_AreaDamage.SetSource("Steklo");
	}
}

modded class Anomaly_DeadZone
{
	void Anomaly_DeadZone()
	{
		m_RechargeTime = 2;
		m_ActiveTime = 10;
	}
	
	override void CreateAreaDamage()
	{
		super.CreateAreaDamage();
		
		//create new area damage
		m_AreaDamage = new AnomalyDamageManager(this);
		m_AreaDamage.SetDamageComponentType(AreaDamageComponentTypes.HITZONE);
		m_AreaDamage.SetDamageableTypes(DAMAGEABLE_TYPES);
		//m_AreaDamage.SetExtents("-2 0 -2", "2 2 2");
		m_AreaDamage.SetHitZones({"Torso","LeftHand","LeftLeg","LeftFoot","RightHand","RightLeg","RightFoot"});
		m_AreaDamage.SetAmmoName("DeadZoneDmg");
		m_AreaDamage.Spawn();
		m_AreaDamage.SetCylinderExtents(10, 10);
		m_AreaDamage.SetSource("DeadZone");
	}
}

modded class Anomaly_Zastoy
{
	void Anomaly_Zastoy()
	{
		m_RechargeTime = 2;
		m_ActiveTime = 5;
	}
	
	override void CreateAreaDamage()
	{
		super.CreateAreaDamage();
		
		//create new area damage
		m_AreaDamage = new AnomalyDamageManager(this);
		m_AreaDamage.SetDamageComponentType(AreaDamageComponentTypes.HITZONE);
		m_AreaDamage.SetDamageableTypes(DAMAGEABLE_TYPES);
		//m_AreaDamage.SetExtents("-2 0 -2", "2 1 2");
		m_AreaDamage.SetHitZones({"Torso","LeftHand","LeftLeg","LeftFoot","RightHand","RightLeg","RightFoot"});
		m_AreaDamage.SetAmmoName("ZastoyDmg");
		m_AreaDamage.Spawn();
		m_AreaDamage.SetCylinderExtents(2, 2);
		m_AreaDamage.SetSource("Zastoy");
	}
}

modded class Anomaly_Snow
{
	private const int SNOW_RADIUS = 10;
	
	void Anomaly_Snow()
	{
		m_RechargeTime = 3;
		m_ActiveTime = 10;
	}
	
	override void CreateAreaDamage()
	{
		super.CreateAreaDamage();
		
		//create new area damage
		m_AreaDamage = new AnomalyDamageManager(this);
		m_AreaDamage.SetDamageComponentType(AreaDamageComponentTypes.HITZONE);
		m_AreaDamage.SetDamageableTypes(DAMAGEABLE_TYPES);
		//m_AreaDamage.SetExtents("-2 0 -2", "2 2 2");
		m_AreaDamage.SetHitZones({"Torso","LeftHand","LeftLeg","LeftFoot","RightHand","RightLeg","RightFoot"});
		m_AreaDamage.SetAmmoName("SnowDmg");
		m_AreaDamage.Spawn();
		m_AreaDamage.SetCylinderExtents(10, 10);
		m_AreaDamage.SetSource("Snow");
	}
	
	override void PostAreaDamageActions()
	{
		super.PostAreaDamageActions();
		
		array<Object> nearest_objects = new array<Object>;
		
		GetGame().GetObjectsAtPosition3D ( GetPosition(), SNOW_RADIUS, nearest_objects, null ); 

		for ( int i = 0; i < nearest_objects.Count(); i++ )
		{
			Object nearest_object = nearest_objects.Get(i);
			  
			if ( nearest_object.IsInherited( PlayerBase ) )
			{
				PlayerBase player = PlayerBase.Cast( nearest_object );
				if (player)
				{
					for ( int j = 0; j < player.GetInventory().AttachmentCount(); j++ )
					{
						ItemBase attachment = ItemBase.Cast(player.GetInventory().GetAttachmentFromIndex(j));
						if (attachment)
						{
							attachment.SetWetMax();
						}
					}
				}
			}
			else if ( nearest_object != this && nearest_object.IsInherited( ItemBase ) && nearest_object.GetParent() == null )
			{
				ItemBase item = ItemBase.Cast( nearest_object );
				if (item)
				{
					item.SetWetMax();
				}
			}
		}
	}
}

modded class Anomaly_Par
{
	ref array<Object> nearest_objects = new array<Object>;
	
	const float PARAM_HEAT_RADIUS = 10.0;
	const float PARAM_HEAT_THROUGH_AIR_COEF = 0.28; //0.035;
	const float TEMPERATURE = 1226.0;
	
	protected ref Timer m_HeatingTimer;
	
	void Anomaly_Par()
	{
		m_RechargeTime = 1;
		m_ActiveTime = 10;
		
		m_HeatingTimer = new Timer ( CALL_CATEGORY_SYSTEM );
	}
	
	override void CreateAreaDamage()
	{
		super.CreateAreaDamage();
		
		//create new area damage
		m_AreaDamage = new AnomalyDamageManager(this);
		m_AreaDamage.SetDamageComponentType(AreaDamageComponentTypes.HITZONE);
		m_AreaDamage.SetDamageableTypes(DAMAGEABLE_TYPES);
		//m_AreaDamage.SetExtents("-2 0 -2", "2 2 2");
		m_AreaDamage.SetHitZones({"Torso","LeftHand","LeftLeg","LeftFoot","RightHand","RightLeg","RightFoot"});
		m_AreaDamage.SetAmmoName("ParDmg");
		m_AreaDamage.Spawn();
		m_AreaDamage.SetCylinderExtents(1.5, 2.5);
		m_AreaDamage.SetSource("Par");
	}
	
	override void PostAreaDamageActions()
	{
		super.PostAreaDamageActions();
		
		m_HeatingTimer.Run( 2, this, "TransferHeatToNearPlayers", NULL, true );
		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( StopHeating, m_ActiveTime * 1000, false);
	}
	
	private void StopHeating()
	{
		m_HeatingTimer.Stop();
		nearest_objects.Clear();
	}
	
	private void TransferHeatToNearPlayers()
	{		
		float distance;
		  
		GetGame().GetObjectsAtPosition ( GetPosition(), PARAM_HEAT_RADIUS, nearest_objects, null ); 
		  
		for ( int i = 0; i < nearest_objects.Count(); i++ )
		{
			Object nearest_object = nearest_objects.Get(i);
			  
			if ( nearest_object.IsInherited( PlayerBase ) )
			{
				PlayerBase player = PlayerBase.Cast( nearest_object );
				distance = vector.Distance( player.GetPosition(), GetPosition() );
				distance = Math.Max( distance, 0.1 ); //min distance cannot be 0 (division by zero)
				  
				float temperature = TEMPERATURE * ( PARAM_HEAT_THROUGH_AIR_COEF / distance );
				player.AddToEnvironmentTemperature( temperature );
			}
			else if ( nearest_object != this && nearest_object.IsInherited( ItemBase ) && nearest_object.GetParent() == null )
			{
				ItemBase item = ItemBase.Cast( nearest_object );
				float wetness = item.GetWet();
					  
				if ( wetness > 0 )
				{
					distance = vector.Distance( item.GetPosition(), GetPosition() );
					distance = Math.Max( distance, 0.1 ); //min distance cannot be 0 (division by zero)
					  
					wetness = wetness * ( PARAM_HEAT_THROUGH_AIR_COEF / distance );
					wetness = Math.Clamp( wetness, item.GetWetMin(), item.GetWetMax() );
					item.AddWet( -wetness );
				}
			}
		}
	}
}

modded class Anomaly_Groza
{
	private const int GROZA_RADIUS = 10;
	
	void Anomaly_Groza()
	{
		m_RechargeTime = 3;
		m_ActiveTime = 10;
	}
	
	override void CreateAreaDamage()
	{
		super.CreateAreaDamage();
		
		//create new area damage
		m_AreaDamage = new AnomalyDamageManager(this);
		m_AreaDamage.SetDamageComponentType(AreaDamageComponentTypes.HITZONE);
		m_AreaDamage.SetDamageableTypes(DAMAGEABLE_TYPES);
		//m_AreaDamage.SetExtents("-2 0 -2", "2 2 2");
		m_AreaDamage.SetHitZones({"Torso","LeftHand","LeftLeg","LeftFoot","RightHand","RightLeg","RightFoot"});
		m_AreaDamage.SetAmmoName("GrozaDmg");
		m_AreaDamage.Spawn();
		m_AreaDamage.SetCylinderExtents(10, 10);
		m_AreaDamage.SetSource("Groza");
		
	}
	override void PostAreaDamageActions()
	{
		super.PostAreaDamageActions();
		
		array<Object> nearest_objects = new array<Object>;
		
		GetGame().GetObjectsAtPosition3D ( GetPosition(), GROZA_RADIUS, nearest_objects, null ); 

		for ( int i = 0; i < nearest_objects.Count(); i++ )
		{
			Object nearest_object = nearest_objects.Get(i);
			  
			if ( nearest_object.IsInherited( PlayerBase ) )
			{
				PlayerBase player = PlayerBase.Cast( nearest_object );
				if (player)
				{
					for ( int j = 0; j < player.GetInventory().AttachmentCount(); j++ )
					{
						ItemBase attachment = ItemBase.Cast(player.GetInventory().GetAttachmentFromIndex(j));
						if (attachment)
						{
							attachment.SetWetMax();
						}
					}
				}
			}
			else if ( nearest_object != this && nearest_object.IsInherited( ItemBase ) && nearest_object.GetParent() == null )
			{
				ItemBase item = ItemBase.Cast( nearest_object );
				if (item)
				{
					item.SetWetMax();
				}
			}
		}
	}
}

modded class Anomaly_BlackHole
{
	const float BLACKHOLE_RADIUS = 30;
	private ref array<Object> m_Objects;
	
	private BlackHoleTrigger m_SuckTrigger;
	
	void Anomaly_BlackHole()
	{
		m_RechargeTime = 3;
		m_ActiveTime = 5;
		
		m_Objects = new array<Object>;
	}
	
	override void CreateAreaDamage()
	{
		super.CreateAreaDamage();
	
		m_Objects.Clear();
			
		//create new area damage
		m_AreaDamage = new AnomalyDamageManager(this);
		m_AreaDamage.SetDamageComponentType(AreaDamageComponentTypes.HITZONE);
		m_AreaDamage.SetDamageableTypes(DAMAGEABLE_TYPES);
		//m_AreaDamage.SetExtents("-2 0 -2", "2 2 2");
		m_AreaDamage.SetHitZones({"Torso","LeftHand","LeftLeg","LeftFoot","RightHand","RightLeg","RightFoot"});
		m_AreaDamage.SetAmmoName("BlackHoleDmg");
		m_AreaDamage.Spawn();
		m_AreaDamage.SetCylinderExtents(33, 3);
		m_AreaDamage.SetSource("BlackHole");
		
	}
	
	
	
	override void PostAreaDamageActions()
	{
		//Print("Start");
		
		super.PostAreaDamageActions();
	
		CreateSuckTrigger();
		
		FindObjects();
		
		vector force;
		vector pos = GetPosition();
		pos[1] = pos[1] + 3.3;
		
		
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(PullHandler, 75, true, pos);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(FindObjects, 1000, true);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(PullHandlerStop, m_ActiveTime * 1000, false);
	}
	
	private void FindObjects()
	{
		array<Object> nearest_objects = new array<Object>;
		
		GetGame().GetObjectsAtPosition3D( GetPosition(), 75, nearest_objects, null ); 

		for ( int i = 0; i < nearest_objects.Count(); i++ )
		{
			Object nearest_object = nearest_objects.Get(i);
			
			if(!nearest_object.IsAlive())
				continue;
			
			if(nearest_object.IsMan() || nearest_object.IsDayZCreature())
			{
				if(m_Objects.Find(nearest_object) == INDEX_NOT_FOUND)
				{
					//PrintFormat("Entity is %1, Weight is %2", nearest_object.GetType(), EntityAI.Cast(nearest_object).GetWeight());
					m_Objects.Insert(nearest_object);
				}
			}
		}
	}
	
	private void PullHandler(vector pos)
	{
		vector force;
		foreach(Object obj: m_Objects)
		{
			force = vector.Direction(obj.GetPosition(), pos).Normalized() * 333;
			force[1] = force[1] * 8;
			dBodyApplyImpulse(obj, force);
		}
	}
	
	private void PullHandlerStop()
	{
		//Print("Stop");
		GetGame().ObjectDelete(m_SuckTrigger);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(PullHandler);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(FindObjects);
	}
	
	private void CreateSuckTrigger()
	{
		vector pos = GetPosition();
		pos[1] = pos[1] + 1;
		m_SuckTrigger = BlackHoleTrigger.Cast(GetGame().CreateObject("BlackHoleTrigger", pos));
		if(m_SuckTrigger)
		{
			m_SuckTrigger.SetParent(this);
			m_SuckTrigger.SetCollisionCylinder(1.2, 3.3);
		}
	}
	
	void OnSucked(Object object)
	{
		m_Objects.RemoveItemUnOrdered(object);
	
		vector pos = GetPosition();
		
		for(int i = 0; i < 3; i++)
		{
			if(i == 1)
				continue;
			
			int sign = 1;
			if(Math.RandomBool())
				sign = -1;
			
			pos[i] = pos[i] + (sign * Math.RandomIntInclusive(333, 1000));
		}
		
		//GetGame().SurfaceY()
		
		object.SetPosition(pos);
		
		object.PlaceOnSurface();
	}
}

modded class Anomaly_Atom
{
	
	void Anomaly_Atom()
	{
		m_RechargeTime = 5;
		m_ActiveTime = 5;
	}
	
	override void CreateAreaDamage()
	{
		super.CreateAreaDamage();
		
		//create new area damage
		m_AreaDamage = new AnomalyDamageManager(this);
		m_AreaDamage.SetDamageComponentType(AreaDamageComponentTypes.HITZONE);
		m_AreaDamage.SetDamageableTypes(DAMAGEABLE_TYPES);
		//m_AreaDamage.SetExtents("-2 0 -2", "2 1 2");
		m_AreaDamage.SetHitZones({"LeftLeg","LeftFoot","RightLeg","RightFoot"});
		m_AreaDamage.SetAmmoName("AtomDmg");
		m_AreaDamage.Spawn();
		m_AreaDamage.SetCylinderExtents(4, 4);
		m_AreaDamage.SetSource("Atom");
	}
	
		/* vector force;
		vector pos = GetPosition();
		pos[1] = pos[1] + 3.3; */
		
			/* force = vector.Direction(obj.GetPosition(), pos).Normalized() * 333;
			force[1] = force[1] * 8;
			dBodyApplyImpulse(obj, force); */
	
	override void PostAreaDamageActions()
	{
		super.PostAreaDamageActions();
		
		array<Object> nearest_objects = new array<Object>;
		
		GetGame().GetObjectsAtPosition3D( GetPosition(), 40, nearest_objects, null ); 
		
		vector pos = GetPosition();
		pos[1] = 0;
		
		for ( int i = 0; i < nearest_objects.Count(); i++ )
		{
			Object nearest_object = nearest_objects.Get(i);
			
			if(!nearest_object.IsAlive())
				continue;
			
			if(nearest_object.IsMan() || nearest_object.IsDayZCreature())
			{
				nearest_object.ProcessDirectDamage(DT_CUSTOM, this, "", "MeleeCraftedHeavy_3", "0 0 0", 0.5);
				ApplyImpulse(nearest_object, pos, 20, 2000);
			}
		}
	}
	private void ApplyImpulse(Object object, vector position, float angle = 45, float power = 1000)
	{
		vector objpos = object.GetPosition();
		objpos[1] = 0;
		vector dir = vector.Direction(position, objpos).Normalized();
		vector force = vector.RotateAroundZeroDeg(dir, "1 0 1", angle) * power;
		dBodyApplyImpulse(object, force);
	}
}

modded class Anomaly_Lince
{
	void Anomaly_Lince()
	{
		m_RechargeTime = 5;
		m_ActiveTime = 5;
	}
	
	override void CreateAreaDamage()
	{
		super.CreateAreaDamage();
		
		//create new area damage
		m_AreaDamage = new AnomalyDamageManager(this);
		m_AreaDamage.SetDamageComponentType(AreaDamageComponentTypes.HITZONE);
		m_AreaDamage.SetDamageableTypes(DAMAGEABLE_TYPES);
		//m_AreaDamage.SetExtents("-2 0 -2", "2 1 2");
		m_AreaDamage.SetHitZones({"Torso","LeftHand","LeftLeg","LeftFoot","RightHand","RightLeg","RightFoot"});
		m_AreaDamage.SetAmmoName("");
		m_AreaDamage.Spawn();
		m_AreaDamage.SetCylinderExtents(2, 2);
		m_AreaDamage.SetSource("Lince");
	}
}

modded class Anomaly_LinceAgr
{
	void Anomaly_LinceAgr()
	{
		m_RechargeTime = 5;
		m_ActiveTime = 5;
	}
	
	override void CreateAreaDamage()
	{
		super.CreateAreaDamage();
		
		//create new area damage
		m_AreaDamage = new AnomalyDamageManager(this);
		m_AreaDamage.SetDamageComponentType(AreaDamageComponentTypes.HITZONE);
		m_AreaDamage.SetDamageableTypes(DAMAGEABLE_TYPES);
		//m_AreaDamage.SetExtents("-2 0 -2", "2 1 2");
		m_AreaDamage.SetHitZones({"Torso","LeftHand","RightHand"});
		m_AreaDamage.SetAmmoName("LinceAgrDmg");
		m_AreaDamage.Spawn();
		m_AreaDamage.SetCylinderExtents(2, 2);
		m_AreaDamage.SetSource("LinceAgr");
	}
}

modded class Anomaly_Holera
{
	void Anomaly_Holera()
	{
		m_RechargeTime = 5;
		m_ActiveTime = 5;
	}
	
	override void CreateAreaDamage()
	{
		super.CreateAreaDamage();
		
		//create new area damage
		m_AreaDamage = new AnomalyDamageManager(this);
		m_AreaDamage.SetDamageComponentType(AreaDamageComponentTypes.HITZONE);
		m_AreaDamage.SetDamageableTypes(DAMAGEABLE_TYPES);
		//m_AreaDamage.SetExtents("-2 0 -2", "2 1 2");
		m_AreaDamage.SetHitZones({"Torso","LeftHand","LeftLeg","LeftFoot","RightHand","RightLeg","RightFoot"});
		m_AreaDamage.SetAmmoName("HoleraDmg");
		m_AreaDamage.Spawn();
		m_AreaDamage.SetCylinderExtents(2, 2);
		m_AreaDamage.SetSource("Holera");
	}
}


modded class Anomaly_FireShtorm
{
	void Anomaly_FireShtorm()
	{
		m_RechargeTime = 2.5;
		m_ActiveTime = 5;
	}
	
	override void CreateAreaDamage()
	{
		super.CreateAreaDamage();
		
		//create new area damage
		m_AreaDamage = new AnomalyDamageManager(this);
		m_AreaDamage.SetDamageComponentType(AreaDamageComponentTypes.HITZONE);
		m_AreaDamage.SetDamageableTypes(DAMAGEABLE_TYPES);
		//m_AreaDamage.SetExtents("-2 0 -2", "2 2 2");
		m_AreaDamage.SetHitZones({"Torso","LeftHand","LeftLeg","LeftFoot","RightHand","RightLeg","RightFoot"});
		m_AreaDamage.SetAmmoName("FireShtormDmg");
		m_AreaDamage.Spawn();
		m_AreaDamage.SetCylinderExtents(3, 3);
		m_AreaDamage.SetSource("FireShtorm");
		
	}
}

modded class Anomaly_Lomka
{
	void Anomaly_Lomka()
	{
		m_RechargeTime = 2.5;
		m_ActiveTime = 5;
	}
	
	override void CreateAreaDamage()
	{
		super.CreateAreaDamage();
		
		//create new area damage
		m_AreaDamage = new AnomalyDamageManager(this);
		m_AreaDamage.SetDamageComponentType(AreaDamageComponentTypes.HITZONE);
		m_AreaDamage.SetDamageableTypes(DAMAGEABLE_TYPES);
		//m_AreaDamage.SetExtents("-2 0 -2", "2 2 2");
		m_AreaDamage.SetHitZones({"Torso","LeftHand","LeftLeg","LeftFoot","RightHand","RightLeg","RightFoot"});
		m_AreaDamage.SetAmmoName("LomkaDmg");
		m_AreaDamage.Spawn();
		m_AreaDamage.SetCylinderExtents(2, 1.5);
		m_AreaDamage.SetSource("Lomka");
		
	}
}

modded class Anomaly_Roy
{
	void Anomaly_Roy()
	{
		m_RechargeTime = 2.5;
		m_ActiveTime = 5;
	}
	
	override void CreateAreaDamage()
	{
		super.CreateAreaDamage();
		
		//create new area damage
		m_AreaDamage = new AnomalyDamageManager(this);
		m_AreaDamage.SetDamageComponentType(AreaDamageComponentTypes.HITZONE);
		m_AreaDamage.SetDamageableTypes(DAMAGEABLE_TYPES);
		//m_AreaDamage.SetExtents("-2 0 -2", "2 2 2");
		m_AreaDamage.SetHitZones({"Torso","LeftHand","LeftLeg","LeftFoot","RightHand","RightLeg","RightFoot"});
		m_AreaDamage.SetAmmoName("RoyDmg");
		m_AreaDamage.Spawn();
		m_AreaDamage.SetCylinderExtents(2, 1.5);
		m_AreaDamage.SetSource("Roy");
		
	}
}

modded class Anomaly_Flash
{
	void Anomaly_Flash()
	{
		m_RechargeTime = 2.5;
		m_ActiveTime = 5;
	}
	
	override void CreateAreaDamage()
	{
		super.CreateAreaDamage();
		
		//create new area damage
		m_AreaDamage = new AnomalyDamageManager(this);
		m_AreaDamage.SetDamageComponentType(AreaDamageComponentTypes.HITZONE);
		m_AreaDamage.SetDamageableTypes(DAMAGEABLE_TYPES);
		//m_AreaDamage.SetExtents("-2 0 -2", "2 2 2");
		m_AreaDamage.SetHitZones({"Torso","LeftHand","LeftLeg","LeftFoot","RightHand","RightLeg","RightFoot"});
		m_AreaDamage.SetAmmoName("FlashDmg");
		m_AreaDamage.Spawn();
		m_AreaDamage.SetCylinderExtents(2, 1.5);
		m_AreaDamage.SetSource("Flash");
		
	}
}

modded class Anomaly_FireIskra
{
	void Anomaly_FireIskra()
	{
		m_RechargeTime = 2.5;
		m_ActiveTime = 5;
	}
	
	override void CreateAreaDamage()
	{
		super.CreateAreaDamage();
		
		//create new area damage
		m_AreaDamage = new AnomalyDamageManager(this);
		m_AreaDamage.SetDamageComponentType(AreaDamageComponentTypes.HITZONE);
		m_AreaDamage.SetDamageableTypes(DAMAGEABLE_TYPES);
		//m_AreaDamage.SetExtents("-2 0 -2", "2 2 2");
		m_AreaDamage.SetHitZones({"Torso","LeftHand","LeftLeg","LeftFoot","RightHand","RightLeg","RightFoot"});
		m_AreaDamage.SetAmmoName("FireIskraDmg");
		m_AreaDamage.Spawn();
		m_AreaDamage.SetCylinderExtents(2, 1.5);
		m_AreaDamage.SetSource("FireIskra");
		
	}
}

modded class Anomaly_MlechniPut
{
	void Anomaly_MlechniPut()
	{
		m_RechargeTime = 2.5;
		m_ActiveTime = 5;
	}
	
	override void CreateAreaDamage()
	{
		super.CreateAreaDamage();
		
		//create new area damage
		m_AreaDamage = new AnomalyDamageManager(this);
		m_AreaDamage.SetDamageComponentType(AreaDamageComponentTypes.HITZONE);
		m_AreaDamage.SetDamageableTypes(DAMAGEABLE_TYPES);
		//m_AreaDamage.SetExtents("-2 0 -2", "2 2 2");
		m_AreaDamage.SetHitZones({"Torso","LeftHand","LeftLeg","LeftFoot","RightHand","RightLeg","RightFoot"});
		m_AreaDamage.SetAmmoName("FireIskraDmg");
		m_AreaDamage.Spawn();
		m_AreaDamage.SetCylinderExtents(2, 1.5);
		m_AreaDamage.SetSource("MlechniPut");
		
	}
}
