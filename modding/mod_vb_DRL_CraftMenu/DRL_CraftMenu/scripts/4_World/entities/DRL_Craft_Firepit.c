class DRL_Craft_Firepit : DRL_Craft_WB
{
	protected bool m_IsCraftActive;
	protected FireplaceLight m_Light;
	protected Particle m_ParticleFire;

	void DRL_Craft_Firepit()
	{
		RegisterNetSyncVariableBool("m_IsCraftActive");
	}

	void ~DRL_Craft_Firepit()
	{
		StopFireEffects();
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		StopFireEffects();
	}

	override bool NeedsScrewdriverForDeconstruct()
	{
		return false;
	}

	override void DRL_OnCraftStart(string resultCls, PlayerBase player)
	{
		super.DRL_OnCraftStart(resultCls, player);
		m_IsCraftActive = true;
		SetSynchDirty();
	}

	override void DRL_OnCraftFinish(string resultCls, EntityAI result, PlayerBase player)
	{
		super.DRL_OnCraftFinish(resultCls, result, player);
		m_IsCraftActive = false;
		SetSynchDirty();
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		UpdateFireEffectsClient();
	}

	protected void UpdateFireEffectsClient()
	{
		if (m_IsCraftActive)
		{
			StartFireEffects();
		}
		else
		{
			StopFireEffects();
		}
	}

	protected void StartFireEffects()
	{
		if (!m_Light)
		{
			m_Light = FireplaceLight.Cast(ScriptedLightBase.CreateLight(FireplaceLight, GetPosition()));
			if (m_Light)
			{
				m_Light.AttachOnMemoryPoint(this, "light");
			}
		}

		if (!m_ParticleFire)
		{
			vector flarePos = GetMemoryPointPos("flare_pos");
			m_ParticleFire = ParticleManager.GetInstance().PlayOnObject(ParticleList.CAMP_NORMAL_FIRE, this, flarePos);
		}
	}

	protected void StopFireEffects()
	{
		if (m_Light)
		{
			m_Light.FadeOut(0.5);
			m_Light = null;
		}

		if (m_ParticleFire)
		{
			m_ParticleFire.Stop();
			m_ParticleFire = null;
		}
	}
};

class DRL_Craft_Firepit_Static : DRL_Craft_Firepit
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
