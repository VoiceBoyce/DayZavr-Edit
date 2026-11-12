class HideZoneTriggerCenter_Light extends PointLightBase
{		
	void HideZoneTriggerCenter_Light()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo( 0.8 );
		SetBrightnessTo( 8 );
		SetCastShadow(false);
		SetFadeOutTime(3);
		SetDiffuseColor(1, 0, 0);
		SetAmbientColor(0.2, 0.5, 1.0);
		SetFlareVisible(true);
		SetFlickerAmplitude(0.5);
		SetFlickerSpeed(0.2);
		SetDancingShadowsMovementSpeed(0.3);
		SetDancingShadowsAmplitude(0.3);
	}
}

class HideZoneTriggerBound_Light extends PointLightBase
{		
	void HideZoneTriggerBound_Light()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo( 0.8 );
		SetBrightnessTo( 8 );
		SetCastShadow(false);
		SetFadeOutTime(3);
		SetDiffuseColor(0, 1, 0);
		SetAmbientColor(0.2, 0.5, 1.0);
		SetFlareVisible(true);
		SetFlickerAmplitude(0.5);
		SetFlickerSpeed(0.5);
		SetDancingShadowsMovementSpeed(0.3);
		SetDancingShadowsAmplitude(0.3);
	}
}

class HideZoneTrigger: ManTrigger
{
	protected ref array<PointLightBase> m_DebugLights;
	protected ref array<EntityAI> m_DebugStones;
	
	void HideZoneTrigger()
	{
		
	}
	
	void ~HideZoneTrigger()
	{
		RemoveDebugLight();
	}
	
	protected void RemoveDebugLight()
	{
		if(m_DebugLights && m_DebugLights.Count() > 0)
		{
			foreach(PointLightBase light: m_DebugLights)
			{
				if ( light ) light.FadeOut();
			}
			
			m_DebugLights.Clear();
		}
		
		if(m_DebugStones && m_DebugStones.Count() > 0)
		{
			foreach(EntityAI stone: m_DebugStones)
			{
				if ( stone ) GetGame().ObjectDelete(stone);
			}
			
			m_DebugStones.Clear();
		}
	}
	
	protected void CreateDebugLight()
	{
		if(!m_DebugLights)
			m_DebugLights = new array<PointLightBase>;
		
		if(!m_DebugStones)
			m_DebugStones = new array<EntityAI>;
		
		m_DebugLights.Clear();
		
		vector center = GetWorldPosition();
		
		vector minmax[2];
		GetCollisionBox(minmax);
		
		vector min = minmax[0];
		vector max = minmax[1];
		
		vector orientation = GetOrientation();
		
		//center light
		HideZoneTriggerCenter_Light center_light = HideZoneTriggerCenter_Light.Cast( ScriptedLightBase.CreateLight(HideZoneTriggerCenter_Light, center, 0.5) );
		center_light.AttachOnObject(this, "0 0 0", orientation);
		
		m_DebugLights.Insert(center_light);
		
		EntityAI center_stone = AttachDebugStone(vector.Zero, orientation);
		if(center_stone)
			m_DebugStones.Insert(center_stone);
		
		array<vector> light_points = GenerateDebugVectorArray(min, max);
		
		foreach(vector dot: light_points)
		{
			HideZoneTriggerBound_Light light = HideZoneTriggerBound_Light.Cast( ScriptedLightBase.CreateLight(HideZoneTriggerBound_Light, center, 0.5) );
			light.AttachOnObject(this, dot, orientation);
			
			m_DebugLights.Insert(light);
			
			EntityAI stone = AttachDebugStone(dot, orientation);
			if(stone)
				m_DebugStones.Insert(stone);
		}
	}
	
	protected EntityAI AttachDebugStone(vector pos, vector ori)
	{
		EntityAI stone = EntityAI.Cast( GetGame().CreateObject("BlowOut_DebugStone", pos, true, false, false) );
		if(stone)
		{
			stone.SetOrientation(ori);
			stone.SetPosition(pos);
				
			AddChild(stone, -1, false);
			stone.Update();
			
			return stone;
		}
		
		return null;
	}
	
	protected array<vector> GenerateDebugVectorArray(vector min, vector max)
	{
		array<vector> points = new array<vector>;
		
		//up zero
		points.Insert(Vector(0, max[1], 0));
		
		//up central
		points.Insert(Vector(max[0], max[1], 0));
		points.Insert(Vector(min[0], max[1], 0));
		points.Insert(Vector(0, max[1], max[2]));
		points.Insert(Vector(0, max[1], min[2]));
		
		//down zero
		points.Insert(Vector(0, min[1], 0));
		
		//down central
		points.Insert(Vector(max[0], min[1], 0));
		points.Insert(Vector(min[0], min[1], 0));
		points.Insert(Vector(0, min[1], max[2]));
		points.Insert(Vector(0, min[1], min[2]));
		
		//forward face zero
		points.Insert(Vector(max[0], 0, max[2]));
		points.Insert(Vector(min[0], 0, max[2]));
		points.Insert(Vector(0, 0, max[2]));

		//backward face zero
		points.Insert(Vector(max[0], 0, min[2]));
		points.Insert(Vector(min[0], 0, min[2]));
		points.Insert(Vector(0, 0, min[2]));
		
		
		//right and left side face zero
		points.Insert(Vector(max[0], 0, 0));
		points.Insert(Vector(min[0], 0, 0));
		
		//forward face
		points.Insert(max);
		points.Insert(Vector(max[0], min[1], max[2]));
		points.Insert(Vector(min[0], min[1], max[2]));
		points.Insert(Vector(min[0], max[1], max[2]));
		
		//backward face
		points.Insert(min);
		points.Insert(Vector(min[0], max[1], min[2]));
		points.Insert(Vector(max[0], max[1], min[2]));
		points.Insert(Vector(max[0], min[1], min[2]));

		return points;
	}
	
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		
		switch (rpc_type)
		{
				// BODY STAGING - server => client synchronization of skinned state.
			case HideZoneTriggerRPCs.DEBUG:
				Param1<bool> data = new Param1<bool>(false);
				if (ctx.Read(data))
				{
					if(data.param1)
						CreateDebugLight();
					else
						RemoveDebugLight();
				}
			break;
		}
	}
	
	override protected bool CanAddObjectAsInsider(Object object)
	{
		if(GetGame().IsServer())
			return false;
		
		PlayerBase player;
		if(Class.CastTo(player, object) && player == PlayerBase.Cast(GetGame().GetPlayer()) && !player.IsMonolith())
			return true;

		return false;
	}
	
	override void OnEnterClientEvent(TriggerInsider insider)
	{
		super.OnEnterClientEvent(insider);
		PlayerBase player = PlayerBase.Cast(insider.GetObject());
		player.SendHideZoneRPC(true, true);
	}
	
	override void OnLeaveClientEvent(TriggerInsider insider)
	{
		super.OnLeaveClientEvent(insider);
		PlayerBase player = PlayerBase.Cast(insider.GetObject());
		player.SendHideZoneRPC(false, true);
	}
}


class BlowOut_HideZoneBase: House{};
class BlowOut_HideZone_2m: BlowOut_HideZoneBase{};
class BlowOut_HideZone_5m: BlowOut_HideZoneBase{};
class BlowOut_HideZone_10m: BlowOut_HideZoneBase{};

class BlowOut_DebugGlasses: AviatorGlasses{};

class BlowOut_DebugStone: ItemBase
{
	override bool IsHealthVisible()
	{
		return false;
	}
	
	override bool IsInventoryVisible()
	{
		return false;
	}
	
	override void SetActions()
	{
		return;
	}
};