modded class HideZoneTrigger: ManTrigger
{
	private bool m_MessageEnabled = false;
	private string m_MessageEntry = "";
	private string m_MessageLeave = "";
	private float m_Tick = 0;
	
	private EntityAI m_Parent;
	
	
	void ~HideZoneTrigger()
	{
		if(m_Parent)
			GetGame().ObjectDelete(m_Parent);
	}
	
	void SetParent(EntityAI parent)
	{
		m_Parent = parent;
	}
	
	void SetMessages(bool enabled, string entry, string leave)
	{
		m_MessageEnabled = enabled;
		m_MessageEntry = entry;
		m_MessageLeave = leave;
	}
	
	/*override protected bool CanAddObjectAsInsider(Object object)
	{
		PlayerBase player;
		if(Class.CastTo(player, object))
		{
			return true;	
		}
		
		return false;
	}
	
	override protected void OnEnterServerEvent(TriggerInsider insider)
	{
		PlayerBase player;
		if(Class.CastTo(player, insider.GetObject()))
		{
			player.SetBlowOutHidden(true, this);
			if(!player.IsMonolith())
			{
				SendRPCMessage(player, m_MessageEntry);
			}
		}
	}
	
	override protected void OnStayServerEvent(TriggerInsider insider, float deltaTime)
	{
		PlayerBase player;
		if(Class.CastTo(player, insider.GetObject()))
		{
			player.SetBlowOutHidden(true);
		}
	}
	
	override protected void OnLeaveServerEvent(TriggerInsider insider)
	{
		PlayerBase player;
		if(Class.CastTo(player, insider.GetObject()))
		{
			player.SetBlowOutHidden(false, this);
			if(!player.IsMonolith())
			{				
				SendRPCMessage(player, m_MessageLeave);
			}
		}
	}
	
	protected void SendRPCMessage(PlayerBase player, string message)
	{
		if(m_MessageEnabled)
		{
			Param1<string> data = new Param1<string>(message);
			GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, data, true, player.GetIdentity());
		}
	}*/
}

modded class BlowOut_HideZoneBase
{
	protected HideZoneTrigger m_Trigger;
	
	void ~BlowOut_HideZoneBase()
	{
		
	}
	
	override void EEInit()
	{
		m_Trigger = HideZoneTrigger.Cast( GetGame().CreateObject("HideZoneTrigger", GetPosition()) );
		if(m_Trigger)
		{
			m_Trigger.SetParent(this);
			array<vector> extents = GetTriggerObjectExtents();
			m_Trigger.SetExtents(extents[0], extents[1]);
			
			GetBlowOutManager().RegisterHideZoneTrigger(m_Trigger);
		}
	}
	
	protected array<vector> GetTriggerObjectExtents()
	{
		return {"-2 -2 -2", "2 2 2"};
	}

}
modded class BlowOut_HideZone_2m
{
	override protected array<vector> GetTriggerObjectExtents()
	{
		return {"-2 -2 -2", "2 2 2"};
	}
}
modded class BlowOut_HideZone_5m
{
	override protected array<vector> GetTriggerObjectExtents()
	{
		return {"-5 -5 -5", "5 5 5"};
	}
}
modded class BlowOut_HideZone_10m
{
	override protected array<vector> GetTriggerObjectExtents()
	{
		return {"-10 -10 -10", "10 10 10"};
	}
}

modded class BlowOut_DebugGlasses
{
	override void OnWasAttached(EntityAI parent, int slot_id)
	{
		super.OnWasAttached(parent, slot_id);
		
		PlayerBase player;
		if(Class.CastTo(player, parent))
		{
			GetBlowOutManager().DebugHideZoneTrigger(player, true);
		}
	}
	
	override void OnWasDetached(EntityAI parent, int slot_id)
	{
		super.OnWasDetached(parent, slot_id);
		
		PlayerBase player;
		if(Class.CastTo(player, parent))
		{
			GetBlowOutManager().DebugHideZoneTrigger(player, false);
		}
	}
}