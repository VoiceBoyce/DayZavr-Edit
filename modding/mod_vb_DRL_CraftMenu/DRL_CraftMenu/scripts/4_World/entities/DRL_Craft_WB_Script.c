class DRL_Craft_WB : ItemBase
{
    protected int m_DRL_CraftSoundId = 0;
    protected int m_DRL_CraftSoundIdPrev = -1;
    protected EffectSound m_DRL_CraftSoundEffect;

    void DRL_Craft_WB()
    {
        RegisterNetSyncVariableInt("m_DRL_CraftSoundId", 0, 63);
    }

    void ~DRL_Craft_WB()
    {
        SEffectManager.DestroyEffect(m_DRL_CraftSoundEffect);
    }

    override void EEDelete(EntityAI parent)
    {
        super.EEDelete(parent);
        SEffectManager.DestroyEffect(m_DRL_CraftSoundEffect);
    }

    void DRL_StartCraftSound(int soundId)
    {
        if (g_Game.IsServer() || !g_Game.IsMultiplayer())
        {
            m_DRL_CraftSoundId = soundId;
            SetSynchDirty();
        }
    }

    void DRL_StopCraftSound()
    {
        if (g_Game.IsServer() || !g_Game.IsMultiplayer())
        {
            m_DRL_CraftSoundId = 0;
            SetSynchDirty();
        }
    }

    override void OnVariablesSynchronized()
    {
        super.OnVariablesSynchronized();
        DRL_UpdateCraftSoundClient();
    }

    protected void DRL_UpdateCraftSoundClient()
    {
        if (m_DRL_CraftSoundId == m_DRL_CraftSoundIdPrev)
        {
            return;
        }

        m_DRL_CraftSoundIdPrev = m_DRL_CraftSoundId;

        if (m_DRL_CraftSoundId > 0)
        {
            string soundSet = DRL_CraftSoundHelper.GetSoundSet(m_DRL_CraftSoundId);
            if (soundSet != "")
            {
                if (m_DRL_CraftSoundEffect)
                {
                    StopSoundSet(m_DRL_CraftSoundEffect);
                }
                PlaySoundSetLoop(m_DRL_CraftSoundEffect, soundSet, 0.3, 0.3);
            }
        }
        else
        {
            if (m_DRL_CraftSoundEffect)
            {
                StopSoundSet(m_DRL_CraftSoundEffect);
            }
        }
    }

    override void SetActions()
    {
        super.SetActions();
		
        AddAction(ActionOpenDRLWorkbench);
        AddAction(ActionDeconstructDRLWorkbench);
    }

    override bool CanPutInCargo (EntityAI parent)
	{
		return false;
	}

    override bool CanPutIntoHands (EntityAI parent)
	{	
		return false;
	}
	
	bool NeedsScrewdriverForDeconstruct()
    {
        return true;
    }

	bool DRL_CanCraftResult(string resultCls, PlayerBase player, out string blockReason)
	{
		blockReason = "";
		return true;
	}

	void DRL_OnCraftStart(string resultCls, PlayerBase player)
	{
	}

	void DRL_OnCraftStartEx(int recipeId, string resultCls, PlayerBase player)
	{
		DRL_OnCraftStart(resultCls, player);
	}

	protected EntityAI TryCreateInAttachmentsCargo(EntityAI parent, string resultCls)
	{
		if (!parent || !parent.GetInventory()) return null;

		int count = parent.GetInventory().AttachmentCount();
		for (int i = 0; i < count; i++)
		{
			EntityAI att = parent.GetInventory().GetAttachmentFromIndex(i);
			if (!att || !att.GetInventory()) continue;

			EntityAI created = att.GetInventory().CreateInInventory(resultCls);
			if (created) return created;

			if (att.GetInventory().AttachmentCount() > 0)
			{
				created = TryCreateInAttachmentsCargo(att, resultCls);
				if (created) return created;
			}
		}
		return null;
	}

	EntityAI DRL_CreateCraftResult(string resultCls, PlayerBase player, bool groundOnly, vector customSpawnPos = "0 0 0")
	{
		if (resultCls == "") return null;

		EntityAI root = GetHierarchyRoot();
		if (!root || root.IsInherited(Man))
		{
			root = this;
		}

		if (!groundOnly)
		{
			if (GetInventory())
			{
				EntityAI inWb = GetInventory().CreateInInventory(resultCls);
				if (inWb) return inWb;
			}

			if (root && root != this && root.GetInventory())
			{
				EntityAI inRoot = root.GetInventory().CreateInInventory(resultCls);
				if (inRoot) return inRoot;
			}

			EntityAI inAtt = TryCreateInAttachmentsCargo(this, resultCls);
			if (inAtt) return inAtt;

			if (root && root != this)
			{
				inAtt = TryCreateInAttachmentsCargo(root, resultCls);
				if (inAtt) return inAtt;
			}
		}

		EntityAI baseObj = root;
		if (!baseObj || baseObj.IsInherited(Man))
		{
			baseObj = this;
		}

		vector pos = baseObj.GetPosition();
		vector dir = baseObj.GetDirection();
		if (baseObj.IsInherited(Man) && player)
		{
			pos = player.GetPosition();
			dir = player.GetDirection();
		}

		dir.Normalize();

		if (GetGame().IsKindOf(resultCls, "Transport"))
		{
			vector vehPos = customSpawnPos;
			if (vehPos == "0 0 0" || vector.DistanceSq(vehPos, pos) > 1225.0)
			{
				vehPos = pos + Vector(dir[0] * 4.5, 0.2, dir[2] * 4.5);
			}

			int vehFlags = ECE_CREATEPHYSICS | ECE_UPDATEPATHGRAPH | ECE_KEEPHEIGHT;
			EntityAI veh = EntityAI.Cast(GetGame().CreateObjectEx(resultCls, vehPos, vehFlags));
			if (veh)
			{
				veh.SetOrientation(baseObj.GetOrientation());
				dBodyApplyImpulse(veh, vector.Up);
				return veh;
			}
		}

		vector dropPos = pos;
		if (player && baseObj != player)
		{
			dropPos = pos + Vector(dir[0] * 0.3, 0.05, dir[2] * 0.3);
		}

		int iFlags = ECE_SETUP | ECE_KEEPHEIGHT | ECE_PLACE_ON_SURFACE;
		EntityAI groundItem = EntityAI.Cast(GetGame().CreateObjectEx(resultCls, dropPos, iFlags));
		if (groundItem)
		{
			groundItem.SetOrientation(baseObj.GetOrientation());
			return groundItem;
		}

		return null;
	}

	void DRL_OnCraftFinish(string resultCls, EntityAI result, PlayerBase player)
	{
	}
};

class DRL_Craft_Workbench extends DRL_Craft_WB
{
	override bool NeedsScrewdriverForDeconstruct()
    {
        return false;
    }
};

class DRL_Craft_Garden extends DRL_Craft_WB
{
	override bool NeedsScrewdriverForDeconstruct()
    {
        return false;
    }
};

class DRL_Craft_Cook extends DRL_Craft_WB
{
	override bool NeedsScrewdriverForDeconstruct()
    {
        return false;
    }
};

class DRL_Craft_Build extends DRL_Craft_WB
{
	override bool NeedsScrewdriverForDeconstruct()
    {
        return false;
    }
};

class DRL_Craft_Welding_Machine extends DRL_Craft_WB
{
	override bool CanPutIntoHands (EntityAI parent)
	{	
		return true;
	}
};

class DRL_Craft_Circular extends DRL_Craft_WB
{
	override bool CanPutIntoHands (EntityAI parent)
	{	
		return true;
	}
};

class DRL_Craft_NoteBook extends DRL_Craft_WB
{
	override bool CanPutIntoHands (EntityAI parent)
	{	
		return true;
	}
};

class DRL_Craft_Microscope extends DRL_Craft_WB
{
	override bool CanPutIntoHands (EntityAI parent)
	{	
		return true;
	}
};

class DRL_Craft_Microwave extends DRL_Craft_WB
{
	override bool CanPutIntoHands (EntityAI parent)
	{	
		return true;
	}
};

class DRL_Craft_Sewing extends DRL_Craft_WB
{
	override bool CanPutIntoHands (EntityAI parent)
	{	
		return true;
	}
};

class DRL_Craft_WB1: DRL_Craft_WB{};
class DRL_Craft_WB2: DRL_Craft_WB{};
class DRL_Craft_WB3: DRL_Craft_WB{};
class DRL_Craft_WB4: DRL_Craft_WB{};
class DRL_Craft_WB5: DRL_Craft_WB{};
class DRL_Craft_WB6: DRL_Craft_WB{};
class DRL_Craft_WB7: DRL_Craft_WB{};
class DRL_Craft_WB8: DRL_Craft_WB{};
class DRL_Craft_WB9: DRL_Craft_WB{};

class DRL_Craft_WB_Static : DRL_Craft_WB
{
    override void SetActions()
    {
        super.SetActions();
        RemoveAction(ActionDeconstructDRLWorkbench);
    }

    override bool CanPutInCargo(EntityAI parent)
    {
        return false;
    }

    override bool CanPutIntoHands(EntityAI parent)
    {
        return false;
    }
};

