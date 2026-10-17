modded class SlotsIcon
{
	protected Widget m_DRL_ItemBlock;
	protected ItemBase m_DRL_CurrentLockedItem;

	void ~SlotsIcon()
	{
		DRL_DestroyItemBlock();
	}

	protected void DRL_DestroyItemBlock()
	{
		if (m_DRL_ItemBlock)
		{
			m_DRL_ItemBlock.Unlink();
			m_DRL_ItemBlock = null;
		}
		m_DRL_CurrentLockedItem = null;
	}

	override void Init(EntityAI obj, bool reservation = false)
	{
		super.Init(obj, reservation);
		DRL_UpdateItemBlock();
	}

	override void SetItemPreview()
	{
		super.SetItemPreview();
		DRL_UpdateItemBlock();
	}

	override void ClearRemainSelected()
	{
		super.ClearRemainSelected();
		DRL_DestroyItemBlock();
	}

	override void UpdateInterval()
	{
		super.UpdateInterval();
		DRL_UpdateItemBlock();
	}

	void DRL_UpdateItemBlock()
	{
		EntityAI obj = m_Obj;
		if (!obj)
		{
			obj = m_Item;
		}

		ItemBase ib = ItemBase.Cast(obj);
		if (m_DRL_CurrentLockedItem != ib)
		{
			DRL_DestroyItemBlock();
			m_DRL_CurrentLockedItem = ib;
		}

		bool shouldShow = (ib && ib.DRL_IsCraftLocked());

		if (shouldShow)
		{
			if (!m_DRL_ItemBlock)
			{
				Widget parent = GetMainWidget();
				if (parent)
				{
					m_DRL_ItemBlock = GetGame().GetWorkspace().CreateWidgets("DRL_CraftMenu/UI/layout/ItemBlock.layout", parent);
					if (m_DRL_ItemBlock)
					{
						m_DRL_ItemBlock.SetPos(0, 0, false);
					}
				}
			}
			if (m_DRL_ItemBlock)
			{
				m_DRL_ItemBlock.Show(true);
			}
		}
		else if (m_DRL_ItemBlock)
		{
			DRL_DestroyItemBlock();
		}
	}
}

modded class Icon
{
	protected Widget m_DRL_ItemBlock;
	protected ItemBase m_DRL_CurrentLockedItem;

	void ~Icon()
	{
		DRL_DestroyItemBlock();
	}

	protected void DRL_DestroyItemBlock()
	{
		if (m_DRL_ItemBlock)
		{
			m_DRL_ItemBlock.Unlink();
			m_DRL_ItemBlock = null;
		}
		m_DRL_CurrentLockedItem = null;
	}

	override void InitEx(EntityAI obj, bool refresh = true)
	{
		super.InitEx(obj, refresh);
		DRL_UpdateItemBlock();
	}

	override void SetItemPreview()
	{
		super.SetItemPreview();
		DRL_UpdateItemBlock();
	}

	override void FullScreen()
	{
		super.FullScreen();
		DRL_UpdateItemBlock();
	}

	override void DestroyWhiteBackground()
	{
		super.DestroyWhiteBackground();
		DRL_UpdateItemBlock();
	}

	override void UpdateInterval()
	{
		super.UpdateInterval();
		DRL_UpdateItemBlock();
	}

	void DRL_UpdateItemBlock()
	{
		EntityAI obj = m_Obj;
		if (!obj)
		{
			obj = m_Item;
		}

		ItemBase ib = ItemBase.Cast(obj);
		if (m_DRL_CurrentLockedItem != ib)
		{
			DRL_DestroyItemBlock();
			m_DRL_CurrentLockedItem = ib;
		}

		bool shouldShow = (ib && ib.DRL_IsCraftLocked());

		if (shouldShow)
		{
			if (!m_DRL_ItemBlock)
			{
				Widget parent = GetMainWidget();
				if (parent)
				{
					m_DRL_ItemBlock = GetGame().GetWorkspace().CreateWidgets("DRL_CraftMenu/UI/layout/ItemBlock.layout", parent);
					if (m_DRL_ItemBlock)
					{
						m_DRL_ItemBlock.SetPos(0, 0, false);
					}
				}
			}
			if (m_DRL_ItemBlock)
			{
				m_DRL_ItemBlock.Show(true);
			}
		}
		else if (m_DRL_ItemBlock)
		{
			DRL_DestroyItemBlock();
		}
	}
}
