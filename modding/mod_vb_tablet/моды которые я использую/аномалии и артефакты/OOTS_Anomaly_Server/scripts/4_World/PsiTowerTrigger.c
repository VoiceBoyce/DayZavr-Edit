class PsiTowerTrigger: ManTrigger
{
	private float m_Tick = 0;
	private const int DAMAGE_VALUE = 10;
	private const int DAMAGE_TIME = 5;

	override protected bool CanAddObjectAsInsider(Object object)
	{
		if(PlayerBase.Cast(object))
			return true;
		
		return false;
	}

	override void OnEnterServerEvent(TriggerInsider insider)
	{
		Object object = insider.GetObject();
		if(!object)
			return;
		
		PlayerBase player;
		if(Class.CastTo(player, object))
		{
			player.SetPsiInfluenceState(true);
		}
	}
	
	override void OnStayServerEvent(TriggerInsider insider, float deltaTime)
	{
		PlayerBase player;
		if(Class.CastTo(player, insider.GetObject()))
		{
			if(!player.IsMonolith())
			{
				m_Tick += deltaTime;
				if(m_Tick >= DAMAGE_TIME)
				{
					if(!player.IsPsiBlocked())
					{
						player.AddHealth("", "Health", -DAMAGE_VALUE);
						player.RequestSoundEvent(EPlayerSoundEventID.TAKING_DMG_LIGHT);
						if(player.GetHealth("", "Health") <= 5)
						{
							player.SetHealth("", "Shock", 5);
						}
					}
					
					m_Tick = 0;
				}
			}
		}
	}
	
	override void OnLeaveServerEvent(TriggerInsider insider)
	{
		Object object = insider.GetObject();
		if(!object)
			return;
		
		PlayerBase player;
		if(Class.CastTo(player, object))
		{
			player.SetPsiInfluenceState(false);
		}
		
		/*PlayerBase player;
		if(Class.CastTo(player, insider.GetObject()))
		{
			player.SetPsiInfluenceState(false);
		}*/
	}
	
	override bool ShouldRemoveInsider(TriggerInsider insider)
	{
		PlayerBase player = PlayerBase.Cast(insider.GetObject());
		if(player && !player.IsAlive())
		{
			return true;
		}
		
		return false;
	}
}