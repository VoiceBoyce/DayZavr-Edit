modded class PlayerBase
{
	protected ref BlowOutHandler m_BlowOutHanlder;

	protected int m_PsiBlockTime = 0;
	protected float m_PsiBlockTick = 0;

	protected bool m_IsTramlingJumping = false;
	
	override void Init()
	{
		super.Init();
		
		m_BlowOutHanlder = new BlowOutHandler(this);
	}
	
	BlowOutHandler GetBlowOutHandler()
	{
		return m_BlowOutHanlder;
	}
	
	override void OnConnect()
	{
		super.OnConnect();
		
		GetBlowOutManager().PlayerOnConnect(this);
	}

	override void OnDisconnect()
	{
		GetGame().RPCSingleParam(this, PlayerRPCs.END_SESSION, null, true, GetIdentity());

		GetBlowOutHandler().OnDisconect();
		
		super.OnDisconnect();
	}
	
	override void EEKilled( Object killer )
	{
		GetGame().RPCSingleParam(this, PlayerRPCs.END_SESSION, null, true, GetIdentity());
		
		super.EEKilled(killer);
	}
	
	override void OnScheduledTick(float deltaTime)
	{
		super.OnScheduledTick(deltaTime);

		if(GetBlowOutHandler())
			GetBlowOutHandler().OnTick(deltaTime);

		if(m_PsiBlockTime > 0)
		{
			m_PsiBlockTick += deltaTime;
			if(m_PsiBlockTick >= 1)
			{
				m_PsiBlockTime--;
				
				if(m_PsiBlockTime > 0)
				{
					if(!GetNotifiersManager().FindNotifier(eNotifiers.NTF_PILLS).IsActive())
						GetNotifiersManager().ActivateByType(eNotifiers.NTF_PILLS);
				}
				else
				{
					if(GetNotifiersManager().FindNotifier(eNotifiers.NTF_PILLS).IsActive())
						GetNotifiersManager().DeactivateByType(eNotifiers.NTF_PILLS);
				}
				
				m_PsiBlockTick = 0;
			}
		}
	}
	
	//trampin jump
	void SetAnomalyTramplinJump()
	{
		m_IsTramlingJumping = true;
	}

	override void OnCommandFallFinish()
	{
		super.OnCommandFallFinish();
		
		if(m_IsTramlingJumping)
		{
			int chance = Math.RandomIntInclusive(0, 100);
			if(chance < 80)
			{
				array<string> zones = {"RightLeg", "LeftLeg"};
				SetHealth(zones.GetRandomElement(), "Health", 0);
				ProcessDirectDamage(DT_CUSTOM, this, "", "FallDamage", WorldToModel(GetPosition()), Math.RandomIntInclusive(4, 8));
			}
			else
			{
				ProcessDirectDamage(DT_CUSTOM, this, "", "FallDamage", WorldToModel(GetPosition()), Math.RandomIntInclusive(2, 4));	
			}
			
			m_IsTramlingJumping = false;
		}
	}
	
	void UpdateBlowoutNetSyncParams(bool is_monolith, bool is_item_protected, bool is_psi_helmet)
	{
		m_AmIMonolith = is_monolith;
		m_PsiHelmet = is_psi_helmet;
		m_IsItemProtected = is_item_protected;
		
		SetSynchDirty();
	}
	
	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);
		
		GetBlowOutHandler().ItemAttachmentEvent();
		
		/*if(!GetBlowOutManager())
			return;

		bool am_i_monolith = GetBlowOutManager().IsMonolith(this);
		bool psi_helmet = GetBlowOutManager().IsPsiHelmet(this);
		bool is_item_protected = GetBlowOutManager().IsItemProtected(this);
		
		if(m_AmIMonolith != am_i_monolith || m_PsiHelmet != psi_helmet || m_IsItemProtected != is_item_protected)
		{
			m_AmIMonolith = am_i_monolith;
			m_PsiHelmet = psi_helmet;
			m_IsItemProtected = is_item_protected;
		
			SetSynchDirty();
		}
		
		
		if((m_AmIMonolith && am_i_monolith) && (m_PsiHelmet && psi_helmet))
			return;
		
		GetBlowOutHandler().SetIsMonolith(am_i_monolith);
		
		m_AmIMonolith = am_i_monolith;
		m_PsiHelmet = psi_helmet;
			
		SetSynchDirty();*/
	}
	
	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);

		GetBlowOutHandler().ItemAttachmentEvent();
		
		/*bool am_i_monolith = GetBlowOutManager().IsMonolith(this);
		bool psi_helmet = GetBlowOutManager().IsPsiHelmet(this);
		bool is_item_protected = GetBlowOutManager().IsItemProtected(this);
		
		if((m_AmIMonolith && am_i_monolith) && (m_PsiHelmet && psi_helmet))
			return;
		
		GetBlowOutHandler().SetIsMonolith(am_i_monolith);
		
		m_AmIMonolith = am_i_monolith;
		m_PsiHelmet = psi_helmet;
			
		SetSynchDirty();*/
	}

	void SetBlowOutHidden(bool state, bool from_trigger = false)
	{
		//GetBlowOutHandler().SetHiddenByTrigger(from_trigger && state);
		GetBlowOutHandler().SetHidden(state);

		GetBlowOutManager().SendPlayerHideZoneState(this, state);

	}

	void SetPsiInfluenceState(bool state)
	{
		if(m_PsiInfluence == state)
			return;
		
		m_PsiInfluence = state;
		
		PlayerIdentity identity = GetIdentity();
		if(identity)
			GetGame().RPCSingleParam(this, RPC_PSI_INFLUENCE, new Param1<bool>(m_PsiInfluence), true, identity);
	}
	
	bool IsPsiBlocked()
	{
		if(m_PsiBlockTime > 0 || m_PsiHelmet)
			return true;
		
		return false;
	}
	
	void PsiBlock(int secs)
	{
		m_PsiBlockTime = secs;
		if(!GetNotifiersManager().FindNotifier(eNotifiers.NTF_PILLS).IsActive())
			GetNotifiersManager().ActivateByType(eNotifiers.NTF_PILLS);
	}
	
	void BlowOutBlock(int secs)
	{
		GetBlowOutHandler().SetProtection(secs);
	}
	
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		switch(rpc_type)
		{
			case PlayerBase.RPC_HIDEZONE:
				Param1<bool> hidezonedata = new Param1<bool>(false);
				if(ctx.Read(hidezonedata))
				{
					SetBlowOutHidden(hidezonedata.param1, true);
				}
			break;
		}
	}
}