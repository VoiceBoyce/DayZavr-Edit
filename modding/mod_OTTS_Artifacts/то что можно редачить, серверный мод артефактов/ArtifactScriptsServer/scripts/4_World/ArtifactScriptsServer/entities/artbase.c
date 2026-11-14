modded class DD_ArtifactBase
{
	protected const string CONTAINER_TYPE = "OOTS_ART_CASE_BASE";
	protected bool m_IsInContainer = false;

	protected ref map<string, float> m_Absolutes;
	protected ref map<string, float> m_Relatives;
	
	protected PlayerBase m_Player;
	
	override void EEInit()
	{
		super.EEInit();
		
		//Init();
	}
	
	protected void Init()
	{
		m_Absolutes = new map<string, float>;
		m_Relatives = new map<string, float>;
	}
	
	override void EEItemLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
	{
		super.EEItemLocationChanged(oldLoc, newLoc);
		
		EntityAI old_parent = oldLoc.GetParent();
		EntityAI new_parent = newLoc.GetParent();
		
		if(old_parent == new_parent)
			return;
		
		PlayerBase old_player;
		PlayerBase new_player;
		
		if(old_parent)
		{
			if(old_parent.IsItemBase())
				ItemBase.Cast(old_parent).RemoveArt(this);
			
			old_player = PlayerBase.Cast(old_parent.GetHierarchyRootPlayer());
		}
		
		bool new_parent_is_container = false;
		if(new_parent)
		{
			if(new_parent.IsItemBase())
			{
				ItemBase.Cast(new_parent).AddArt(this);
				new_parent_is_container = IsParentContainer(new_parent);
			}
			
			new_player = PlayerBase.Cast(new_parent.GetHierarchyRootPlayer());
		}

		//register container state switch
		if(new_player && old_player == new_player)
		{
			if(new_parent_is_container != m_IsInContainer)
			{
				if(!new_parent_is_container)
					Activate(new_player);
				else
					Deactivate(new_player);
			}
		}
		
		if(old_player != new_player)
		{
			if(!new_parent_is_container)
			{
				if(new_player)
					Activate(new_player);
				
				if(old_player && !m_IsInContainer)
					Deactivate(old_player);
			}
			else
			{
				if(old_player && !m_IsInContainer)
					Deactivate(old_player);
			}	
		}
		
		m_IsInContainer = new_parent_is_container;

	}
	
	void OnParentLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
	{
		EntityAI old_parent = oldLoc.GetParent();
		EntityAI new_parent = newLoc.GetParent();
		
		PlayerBase old_player;
		PlayerBase new_player;
		
		if(old_parent)
			old_player = PlayerBase.Cast(old_parent.GetHierarchyRootPlayer());
		
		if(new_parent)
			new_player = PlayerBase.Cast(new_parent.GetHierarchyRootPlayer());
		
		if(!m_IsInContainer)
			OnArtPlayerChanged(old_player, new_player);
	}
	
	private void OnArtPlayerChanged(PlayerBase old_player, PlayerBase new_player)
	{
		if(old_player == new_player)
			return;
		
		if(old_player)
		{
			Deactivate(old_player);
		}
		
		if(new_player)
		{
			Activate(new_player);
		}
	}
	
	protected bool IsParentContainer(EntityAI parent)
	{
		if(parent && parent.IsItemBase())
		{
			if(parent.GetType() == CONTAINER_TYPE || parent.IsKindOf(CONTAINER_TYPE))
				return true;
		}
		
		return false;
	}
	
	protected void Activate(PlayerBase player)
	{
		//PrintFormat("[%1][Activate]: ", GetType());
		
		if(!m_Absolutes && !m_Relatives)
			Init();
		
		m_Player = player;
		
		ArtModifierBase modifier;
		for(int i = 0; i < m_Absolutes.Count(); i++)
		{
			modifier = player.GetArtManager().GetModifier(m_Absolutes.GetKey(i));
			if(modifier)
			{
				modifier.AddAbsolute(m_Absolutes.GetElement(i));
			}
		}
		
		for(int j = 0; j < m_Relatives.Count(); j++)
		{
			modifier = player.GetArtManager().GetModifier(m_Relatives.GetKey(j));
			if(modifier)
			{
				modifier.AddRelative(this, m_Relatives.GetElement(j));
			}
		}
	}
	
	protected void Deactivate(PlayerBase player)
	{
		//PrintFormat("[%1][Deactivate]: ", GetType());
	
		ArtModifierBase modifier;
		for(int i = 0; i < m_Absolutes.Count(); i++)
		{
			modifier = player.GetArtManager().GetModifier(m_Absolutes.GetKey(i));
			if(modifier)
			{
				modifier.AddAbsolute(-m_Absolutes.GetElement(i));
			}
		}
		
		for(int j = 0; j < m_Relatives.Count(); j++)
		{
			modifier = player.GetArtManager().GetModifier(m_Relatives.GetKey(j));
			if(modifier)
			{
				modifier.RemoveRelative(this);
			}
		}
		m_Player = null;
		
	}
	override void EEDelete(EntityAI parent)
    {
		if(m_Player)
		{
			Deactivate(m_Player);
		}
		super.EEDelete(parent);
	}
}

/* modded class ArtDebug
{
	override void Init()
	{
		super.Init();
		
		//m_Absolutes.Set("Health", 0.46);
		//m_Relatives.Set("Health", 1.2);
		
		//m_Absolutes.Set("Water", -0.2);
		//m_Absolutes.Set("Energy", -0.8);
		
		m_Absolutes.Set("Shock", -10);
		
		
		m_Absolutes.Set("Blood", 1.15);
		
		m_Absolutes.Set("Bleeds", 0.5);
		
		m_Absolutes.Set("Stamina", 0.2); //this is stamina percent coef, e.g. -50% per tick
	}
} */