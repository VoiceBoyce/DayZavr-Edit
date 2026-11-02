//ammo, magazines
modded class Ammunition_Base
{
	void Ammunition_Base()
	{
		SetFlags(GetFlags() | EntityFlags.TOUCHTRIGGERS, false);
	}
}

modded class MagazineStorage
{
	void MagazineStorage()
	{
		SetFlags(GetFlags() | EntityFlags.TOUCHTRIGGERS, false);
	}
}

class OOTS_Item_Bolt extends ItemBase
{
	void OOTS_Item_Bolt()
	{
		SetFlags(GetFlags() | EntityFlags.TOUCHTRIGGERS, false);
	}
}


class AnomalyDamageTrigger: AreaDamageTriggerBase
{
	void AnomalyDamageTrigger()
	{
		SetEventMask(GetEventMask() | EntityEvent.CONTACT | EntityEvent.TOUCH);
		SetFlags(EntityFlags.TRIGGER, false);
		m_insiders = new array<ref TriggerInsider>;
	}
	
	override void EOnTouch(IEntity other, int extra)
	{
		ContactEventHandler(other);
	}
	
	override void EOnContact(IEntity other, Contact extra)
	{
		ContactEventHandler(other);
	}
	
	protected void ContactEventHandler(IEntity other)
	{
		Object obj;
		if(Class.CastTo(obj, other) && CanAddObjectAsInsider(obj))
		{
			//PrintFormat("[AnomalyDamageTrigger][ContactEventHandler]: Obj is %1", obj.GetType());
		}
	}
}

class AnomalyDamageManager: AreaDamageOnce
{
	protected string dmgsource;

	void AnomalyDamageManager(EntityAI parent)
	{
		m_ParentObject		= parent;
		
		m_DamageComponent	= new AreaDamageComponent(this);
		
		m_ExtentMin 		= vector.Zero;
		m_ExtentMax 		= vector.Zero;
		m_AreaPosition		= parent.GetPosition();
		m_AreaOrientation	= parent.GetOrientation();
		
		//m_TriggerBaseClass 	= "AnomalyDamageTrigger";
		m_TriggerBaseClass  = "AreaDamageTriggerBase";
	}
	
	override protected bool CreateDamageTriggerEx()
	{
		int flags = ECE_NONE;

		/*if ( GetGame().IsMultiplayer() && GetGame().IsServer() )
			flags = ECE_CREATEPHYSICS;
		else
			flags = ECE_LOCAL;*/

		if (Class.CastTo(m_AreaDamageTrigger, GetGame().CreateObjectEx( m_TriggerBaseClass, m_AreaPosition, flags )))
		{
			m_AreaDamageTrigger.SetOrientation( m_AreaOrientation );
			//m_AreaDamageTrigger.SetCollisionCylinder(m_Radius, m_Height);
			m_AreaDamageTrigger.SetAreaDamageManager(this);
			
			return true;
		}
		
		return false;
	}
	
	void SetSource(string source)
	{
		dmgsource = source;
		
		m_AreaDamageTrigger.SetEventMask(m_AreaDamageTrigger.GetEventMask() | EntityEvent.TOUCH);
	}
	
	void SetCylinderExtents(float radius, float heigth)
	{
		m_AreaDamageTrigger.SetCollisionCylinder(radius, heigth);
	}
	
	override bool OnEvaluateDamage(TriggerInsider insider, float damageCoef)
	{		
		OOTS_AnomalyBase anomaly = OOTS_AnomalyBase.Cast(m_ParentObject);
		if(anomaly)
		{
			damageCoef = anomaly.GetDmgCoef();
		}
		
		if(m_DamageComponent.OnEvaluateDamage(insider, damageCoef))
		{
			Object object = insider.GetObject();
			EntityAI eai = EntityAI.Cast(object);
			
			if( AnomalyTypeCheck(object, "OOTS_Item_Bolt") || AnomalyTypeCheck(object, "MagazineStorage") || AnomalyTypeCheck(object, "Ammunition_Base")) //case bolt
			{
				if(dmgsource == "Tramplin" || dmgsource == "Voronka" || dmgsource == "Electra") //in this anomalies
				{
					vector item_jump = Vector(Math.RandomIntInclusive(-30, 30), Math.RandomIntInclusive(100, 150), Math.RandomIntInclusive(-30, 30));
					dBodyApplyImpulse(eai, item_jump); //bolt gets an impulse
				}
				
				if(dmgsource == "Teleport")
				{
					//GetAnomalyTeleportManager().TeleportEntity(GetParentObject(), eai);
					Anomaly_Teleport_Entry.Cast( GetParentObject() ).Teleport(eai);
				}			
			}
			else if(AnomalyTypeCheck(object, "DayZPlayer"))
			{
				Man mantarget = Man.Cast(object);
				PlayerBase player = PlayerBase.Cast(object);
						
				if( dmgsource == "Tramplin" ) //in this anomaly
				{
					//player.GetStatStamina().Set( 0 ); //sets stamina to 0
					if(!mantarget.IsInTransport()) //if not in transport (to prevent vehicle bugs)
					{
						//set player to jump this high
						player.SetAnomalyTramplinJump();
						vector tramplin_vector = GetJumpVector();
						dBodyApplyImpulse(eai, tramplin_vector);
					};
				}
				else if( dmgsource == "Studen" || dmgsource == "Voronka" ) //in this anomalies
				{
					player.GetStatStamina().Set( 0 ); //sets stamina to 0
				}
				else if( dmgsource == "Electra" )
				{

				}
				else if( dmgsource == "FireShtorm" )
				{

				}
				else if( dmgsource == "Lomka" )
				{

				}
				else if( dmgsource == "Roy" )
				{

				}
				else if( dmgsource == "Flash" )
				{

				}
				else if( dmgsource == "Zharka" )
				{
					player.GetStatHeatComfort().Set( 40 );
				}
				else if( dmgsource == "Teleport" )
				{
					//GetAnomalyTeleportManager().TeleportEntity(GetParentObject(), eai);
					Anomaly_Teleport_Entry.Cast( GetParentObject() ).Teleport(eai);
				}
				else if( dmgsource == "EMP_Blast" )
				{
					Anomaly_EMP_Blast.Cast( GetParentObject() ).EMPBlast();
				}
				else if( dmgsource == "Steklo" )
				{
					player.SetHealth01("", "Blood", 0.62);
				}
				else if( dmgsource == "DeadZone" )
				{

				}
				else if( dmgsource == "Zastoy" )
				{

				}
				else if( dmgsource == "Snow" )
				{
					player.GetStatStamina().Set( 0 );
				}
				else if( dmgsource == "Par" )
				{
					player.GetStatStamina().Set( 0 );
					player.GetStatHeatComfort().Set( 80 );
				}
				else if( dmgsource == "Groza" )
				{

				}
				else if( dmgsource == "Atom" )
				{

				}
				else if( dmgsource == "Lince" )
				{
					player.SetHealth01("", "Health", 1);
					player.SetHealth01("", "Blood", 1);
					player.GetStatEnergy().Set( 5000 );
					player.GetStatWater().Set( 5000 );
				}
				
				else if( dmgsource == "LinceAgr" )
				{
					player.SetHealth01("", "Health", 0.4);
					player.SetHealth01("", "Blood", 0.6);
					player.SetHealth01("", "Shock", 0.5);
					player.GetStatEnergy().Set( 0 );
					player.GetStatWater().Set( 0 );
					player.GetStatHeatComfort().Set( 80 );
				}
				
				else if( dmgsource == "Holera" )
				{
					player.SetHealth01("", "Shock", 0.5);
					player.GetStatEnergy().Set( 0 );
					player.GetStatToxicity().Set( 200 );
				}
				
			}
			else if(AnomalyTypeCheck(object, "AnimalBase") || AnomalyTypeCheck(object, "ZombieBase"))
			{
				if( dmgsource == "Tramplin" ) //in this anomaly
				{
					dBodyApplyImpulse(eai, GetJumpVector());
					eai.ProcessDirectDamage(DT_CUSTOM, eai, "", "FallDamage", eai.WorldToModel(eai.GetPosition()), Math.RandomIntInclusive(5, 10));
				}
				
				else if( dmgsource == "Teleport" )
				{
					//GetAnomalyTeleportManager().TeleportEntity(GetParentObject(), eai);
					Anomaly_Teleport_Entry.Cast( GetParentObject() ).Teleport(eai);
				}
			}
			
			return true;
		}
		
		return false;
						
	}
	
	private bool AnomalyTypeCheck(Object object, string type)
	{
		if(object.GetType() == type || object.IsKindOf(type) || object.IsInherited(type.ToType()))
			return true;
		
		return false;
	}
	
	private vector GetJumpVector()
	{
		vector v = vector.Zero;

		for(int i = 0; i < 3; i++)
		{
			if(i == 1)
			{
				v[i] = Math.RandomIntInclusive(1200, 1600);
			}
			else
			{
				v[i] = Math.RandomIntInclusive(-600, 600);
			}
			
		}

		return v;
	}
}
