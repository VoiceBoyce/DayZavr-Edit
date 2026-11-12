modded class OOTS_item_PsyTower
{
	PsiTowerTrigger m_PsiTrigger;
	protected const float PSI_RADIUS = 150;
	
	override void EEInit()
	{
		super.EEInit();
		
		//request
		SetPsiActive(true);
	}
	
	override void EEKilled(Object killer)
	{
		SetPsiActive(false);
		
		super.EEKilled(killer);
		
	}
	
	override void EEDelete(EntityAI parent)
	{
		SetPsiActive(false);
		
		super.EEDelete(parent);
	}
	
	void EndPsiEmmition()
	{
		SetPsiActive(false);
	}
	
	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		
		ctx.Write(m_IsActive);
	}
	
	override bool OnStoreLoad (ParamsReadContext ctx, int version)
	{
		if ( !super.OnStoreLoad(ctx, version) )
			return false;
		
		bool is_active = false;
		if ( !ctx.Read( is_active ) )
			return false;
		SetPsiActive(is_active);
		
		return true;
	}
	
	/*override void OnPlacementComplete( Man player, vector position = "0 0 0", vector orientation = "0 0 0" )
	{
		super.OnPlacementComplete(player, position, orientation);
		
		SetPsiActive(true);
	}*/
	
	protected void SetPsiActive(bool state)
	{
		if(m_IsActive == state)
			return;
		
		m_IsActive = state;
		
		if(m_IsActive)
		{
			m_PsiTrigger = PsiTowerTrigger.Cast( GetGame().CreateObject("PsiTowerTrigger", GetPosition()) );
			if(m_PsiTrigger)
			{	
				m_PsiTrigger.SetCollisionSphere(PSI_RADIUS);
			}
		}
		else
		{
			if(m_PsiTrigger)
				GetGame().ObjectDelete( m_PsiTrigger );
			
		}

		//Sync To client
		SetSynchDirty();
	}
}