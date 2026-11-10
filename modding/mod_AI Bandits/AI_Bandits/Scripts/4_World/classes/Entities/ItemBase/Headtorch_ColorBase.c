modded class Headtorch_ColorBase
{	
	override void CheckParent()
	{
		EntityAI owner = GetHierarchyParent();
		InfectedBanditBase bandit = InfectedBanditBase.Cast(owner);
		// Spawn a battery in the headtorch if it's attached on a zombie and switch it on
		if (bandit)
		{
			GetInventory().CreateAttachment("Battery9V");
			GetCompEM().SwitchOn();
		}
		super.CheckParent();
	}
	
	override void CreateHeadtorchLight()
	{
		
		super.CreateHeadtorchLight();
		
		if (!m_Light)
			m_Light = HeadtorchLight.Cast(ScriptedLightBase.CreateLight( HeadtorchLight, "0 0 0"));
		
		OnLightCreated();
		
		m_Light.PerformVisibilityCheck(this);
		
		EntityAI owner = GetHierarchyParent();
		
		InfectedBanditBase bandit = InfectedBanditBase.Cast(owner);
		if (bandit)
		{
			int slot_id = InventorySlots.GetSlotIdFromString("Eyewear");
			EntityAI item_EAI = owner.GetInventory().FindAttachment(slot_id);
			ItemBase item_IB = ItemBase.Cast(item_EAI);
			
			if (item_IB == this)
			{
				AttachLightOnHead(owner);
			}
		}
	}

	
	
	override void AttachLightOnHead(EntityAI person)
	{
		InfectedBanditBase bandit = InfectedBanditBase.Cast(person);

		if (bandit)
		{
			if (m_Light.GetParent())
				m_Light.DetachFromParent();
			
			m_Light.SetPosition(m_OnHeadLocalPos);
			m_Light.SetOrientation(m_OnHeadLocalOri);
			
			person.AddChild(m_Light, 17);
		}
		super.AttachLightOnHead(person);
	}
		
	override void OnWasDetached(EntityAI parent, int slot_id)
	{
		super.OnWasDetached(parent, slot_id);
		InfectedBanditBase bandit = InfectedBanditBase.Cast(parent);
		if (bandit)
		{
			if (m_Light)
			{
				m_Light.DetachFromParent();
				m_Light.AttachOnMemoryPoint(this, m_OffHeadLightPoint, m_OffHeadLightTarget);
			}
		}
	}
	
};