modded class PlayerBase
{
	protected EntityAI m_DRL_CurrentWorkbench;

	void DRL_SetCurrentWorkbench(EntityAI wb)
	{
		m_DRL_CurrentWorkbench = wb;
	}

	EntityAI DRL_GetCurrentWorkbench()
	{
		return m_DRL_CurrentWorkbench;
	}
	
	void DRL_CollectItemsByClass(string classname, float minHealthFrac, out array<ItemBase> outItems)
	{
		outItems = new array<ItemBase>();
		if (classname == "") return;

		int minLevel = minHealthFrac;
		minLevel = Math.Clamp(minLevel, 0, 3);
	
		array<EntityAI> items = new array<EntityAI>();
		GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
		
		EntityAI wb = DRL_GetCurrentWorkbench();
		if (wb && wb.GetInventory())
		{
			wb.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
		}
	
		for (int i = 0; i < items.Count(); i++)
		{
			ItemBase ib = ItemBase.Cast(items[i]);
			if (!ib) continue;
			
			InventoryLocation il = new InventoryLocation();
			if (ib.GetInventory() && ib.GetInventory().GetCurrentInventoryLocation(il))
			{
				if (il.GetType() == InventoryLocationType.ATTACHMENT)
				{
					EntityAI parentEnt = il.GetParent();
					if (parentEnt == this)
						continue;
				}
			}
			
			if (!ib.IsKindOf(classname)) continue;
			if (ib.IsRuined()) continue;

			int lvl = ib.GetHealthLevel();
			if (lvl > minLevel) continue;
	
			outItems.Insert(ib);
		}
	}
}