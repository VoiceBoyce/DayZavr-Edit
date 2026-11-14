modded class ItemBase
{
	private ref array<DD_ArtifactBase> m_Arts;
	
	override void InitItemVariables()
	{
		super.InitItemVariables();
	}
	
	override void EEItemLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
	{
		
		super.EEItemLocationChanged(oldLoc, newLoc);
		
		int art_count = 0;
		if(m_Arts)
			art_count = m_Arts.Count();
		
		//PrintFormat("[ItemBase][%1]: EEItemLocationChanged - Arts count is %2", GetType(), art_count);
		
		if(m_Arts && m_Arts.Count() > 0)
		{
			EntityAI old_parent = oldLoc.GetParent();
			EntityAI new_parent = newLoc.GetParent();
			
			if(old_parent == new_parent)
				return;
			
			foreach(DD_ArtifactBase art: m_Arts)
			{
				if(art)
				{
					if(old_parent)
					{
						if(old_parent.IsItemBase())
							ItemBase.Cast(old_parent).RemoveArt(art);
					}
					
					if(new_parent)
					{
						if(new_parent.IsItemBase())
							ItemBase.Cast(new_parent).AddArt(art);
					}
					
					art.OnParentLocationChanged(oldLoc, newLoc);
				}
			}
		}
	}

	void AddArt(DD_ArtifactBase art)
	{
		//PrintFormat("[ItemBase][%1]: AddArt - Art is %2", GetType(), art.GetType());
		
		if(!m_Arts)
			m_Arts = new array<DD_ArtifactBase>;
		
		int index = m_Arts.Find(art);
		if(index > INDEX_NOT_FOUND)
			return;
		
		m_Arts.Insert(art);
		
		AddArtToParent(art);
	}
	
	void RemoveArt(DD_ArtifactBase art)
	{
		//PrintFormat("[ItemBase][%1]: RemoveArt - Art is %2", GetType(), art.GetType());
		
		int index = m_Arts.Find(art);
		if(index > INDEX_NOT_FOUND)
		{
			m_Arts.Remove(index);
		}

		RemoveArtFromParent(art);
		
		if(m_Arts.Count() == 0)
			delete m_Arts;
	}
	
	private void AddArtToParent(DD_ArtifactBase art)
	{
		EntityAI entity = GetHierarchyParent();
		if(entity && entity.IsItemBase())
		{
			ItemBase.Cast(entity).AddArt(art);
			
		}
	}
	
	private void RemoveArtFromParent(DD_ArtifactBase art)
	{
		EntityAI entity = GetHierarchyParent();
		if(entity && entity.IsItemBase())
		{
			ItemBase.Cast(entity).RemoveArt(art);
		}
	}
}