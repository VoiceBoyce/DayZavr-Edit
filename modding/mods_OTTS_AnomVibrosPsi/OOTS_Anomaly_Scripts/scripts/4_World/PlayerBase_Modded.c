modded class PlayerBase
{
	protected bool 								m_AmIMonolith = false;
	protected bool 								m_IsItemProtected = false;
	
	protected int 								m_BlowOutPhase = BlowOutPhases.COOLDOWN;
	protected int 								m_BlowOutClientPhase = BlowOutPhases.COOLDOWN;
	
	protected int 								m_PsiStormPhase = BlowOutPhases.COOLDOWN;
	protected int 								m_PsiStormClientPhase = BlowOutPhases.COOLDOWN;
	
	protected float 							m_Tick = 0;
	protected int 								m_TickTime = 0;
	
	protected bool 								m_PsiInfluence = false;
	protected bool 								m_PsiHelmet = false;
	
	protected ref CamEffects 					m_CamEffects;
	protected EffectSound 						m_PsiInfluenceLoop;
	
	protected bool 								m_IsUnderRoof;
	protected float 							m_IsUnderRoofTick;
	protected bool 								m_InTrigger;
	
	protected ref PsiStormHandlerClient 		m_PsiStormHandlerClient;
	protected ref BlowOutHandlerClient 			m_BlowOutHandlerClient;

	static const int RPC_HIDEZONE = 342155;
	static const int RPC_PSI_INFLUENCE = 34215699;
	
	override void Init()
	{
		super.Init();
		if(GetGame().IsClient())
		{
			m_CamEffects = new CamEffects();
			ClearScreen();
		}
		
		RegisterNetSyncVariableBool("m_AmIMonolith"); //am I monolith
		RegisterNetSyncVariableBool("m_IsItemProtected");
		//RegisterNetSyncVariableBool("m_PsiInfluence");
		RegisterNetSyncVariableBool("m_PsiHelmet");
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		
		//ManagePsiInfluence();
	}
	
	override void OnDamageDestroyed(int oldLevel)
	{
		super.OnDamageDestroyed(oldLevel);
		
		#ifndef SERVER
		StopAllEffects();
		#endif;
	}
	
	bool IsMonolith()
	{
		return m_AmIMonolith;
	}
	
	CamEffects GetCamEffects()
	{
		return m_CamEffects;
	}
	
	protected void ManageBlowOutClient(int phase_duration)
	{
		if(m_BlowOutClientPhase == m_BlowOutPhase)
			return;
		
		if(!m_BlowOutHandlerClient)
		{
			m_BlowOutHandlerClient = new BlowOutHandlerClient(this);
			m_BlowOutHandlerClient.OnHiddenStateChanged(IsBlowOutHidden());
		}
			
		
		m_BlowOutClientPhase = m_BlowOutPhase;
		
		if(m_BlowOutHandlerClient)
		{
			m_BlowOutHandlerClient.SetPhaseDuration(phase_duration);
			m_BlowOutHandlerClient.OnPhaseSwitched(m_BlowOutClientPhase);
		}
	}
	
	protected void ManagePsiInfluence()
	{
		if(m_PsiStormHandlerClient)
		{
			m_PsiStormHandlerClient.SetPsiInfluenceState(m_PsiInfluence);
		}
		if(m_PsiStormPhase == BlowOutPhases.ACTIVE)
		{
			/*if(m_PsiInfluenceLoop && m_PsiInfluenceLoop.IsSoundPlaying())
			{
				StopSoundSet( m_PsiInfluenceLoop );
			}*/
			
			return;
		}
		
		if(m_PsiInfluence && !m_AmIMonolith)
		{
			if(m_PsiInfluenceLoop == NULL || !m_PsiInfluenceLoop.IsSoundPlaying())
			{
				PlaySoundSetLoop( m_PsiInfluenceLoop, "PsiInfluence_SoundSet", 0.5, 1.5 );
			}
			
			int psi_influence_type = PPERequester_OOTS_Anomaly_Client.PSI_INFLUENCE;
			
			if(m_PsiHelmet)
			{
				//m_PsiInfluenceLoop.SetSoundVolume(0.05);
				GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(SetSoundVolume, 600, false, m_PsiInfluenceLoop, 0.05);
				psi_influence_type = PPERequester_OOTS_Anomaly_Client.PROTECTED_PSI_INFLUENCE;
			}	
			else if(IsBlowOutHidden())
			{
				//m_PsiInfluenceLoop.SetSoundVolume(0.33);
				GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(SetSoundVolume, 600, false, m_PsiInfluenceLoop, 0.33);
			}	
			else
			{
				//m_PsiInfluenceLoop.SetSoundVolume(1);
				GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(SetSoundVolume, 600, false, m_PsiInfluenceLoop, 1);
			}	
				
			if(m_BlowOutPhase != BlowOutPhases.ACTIVE)
				m_CamEffects.SetInfluence(psi_influence_type);
		}
		else
		{
			StopSoundSet( m_PsiInfluenceLoop );
			
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ClearScreen, 100, false, false);
		}
	}
	
	void OnBlowOutHandlerDone()
	{
		m_BlowOutHandlerClient = null;
		
		ClearScreen(true);
		
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ManagePsiInfluence, 300, false);
	}
	
	protected void ManagePsiStormClient(int phase_duration)
	{
		if(m_AmIMonolith)
			return;
		
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(ClearScreen);
		
		if(m_PsiStormPhase == m_PsiStormClientPhase)
			return;
		
		if(!m_PsiStormHandlerClient)
		{
			m_PsiStormHandlerClient = new PsiStormHandlerClient(this);
			m_PsiStormHandlerClient.SetPsiInfluenceState(m_PsiInfluence);
			m_PsiStormHandlerClient.OnHiddenStateChanged(IsBlowOutHidden());
		}
			
		
		m_PsiStormClientPhase = m_PsiStormPhase;
		
		//stopping psi influence
		if(m_PsiStormClientPhase == BlowOutPhases.ACTIVE && m_PsiInfluenceLoop && m_PsiInfluenceLoop.IsSoundPlaying())
		{
			StopSoundSet( m_PsiInfluenceLoop );
		}
		
		if(m_PsiStormHandlerClient)
		{
			m_PsiStormHandlerClient.SetPhaseDuration(phase_duration);
			m_PsiStormHandlerClient.OnPhaseSwitched();
		}
	}
	
	void OnPsiHandlerDone()
	{
		m_PsiStormHandlerClient = null;
		
		ClearScreen(true);
		
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ManagePsiInfluence, 300, false);
	}
	
	int GetPsiStormPhase()
	{
		return m_PsiStormClientPhase;
	}
	
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		switch(rpc_type)
		{
			case PlayerRPCs.END_SESSION:
				StopAllEffects();
			break;
			case PlayerRPCs.BLOWOUT_PHASE:
				Param2<int, int> phase_data = new Param2<int, int>(0, 0);
				if(ctx.Read(phase_data))
				{
					m_BlowOutPhase = phase_data.param1;
					ManageBlowOutClient(phase_data.param2);
				}
			break;
			case PlayerRPCs.PSISTORM_PHASE:
				Param2<int, int> psi_phase_data = new Param2<int, int>(0, 0);
				if(ctx.Read(psi_phase_data))
				{
					m_PsiStormPhase = psi_phase_data.param1;
					ManagePsiStormClient(psi_phase_data.param2);
				}
			break;
			case RPC_PSI_INFLUENCE:
				Param1<bool> psi_influence_data = new Param1<bool>(false);
				if(ctx.Read(psi_influence_data))
				{
					m_PsiInfluence = psi_influence_data.param1;
					ManagePsiInfluence();
				}
			break;
		}
	}
	
	void SendHideZoneRPC(bool state, bool from_trigger)
	{
		m_InTrigger = state && from_trigger;
		
		GetGame().RPCSingleParam(this, PlayerBase.RPC_HIDEZONE, new Param1<bool>(state), true);
		
		if(m_PsiStormHandlerClient)
		{
			m_PsiStormHandlerClient.OnHiddenStateChanged(state);
		}
		
		if(m_BlowOutHandlerClient)
		{
			m_BlowOutHandlerClient.OnHiddenStateChanged(state);
		}
	}
	
	bool IsBlowOutHidden()
	{
		return m_InTrigger || m_IsUnderRoof;
	}
	
	void CheckIsUnderRoof(float delta)
	{
		if(m_InTrigger)
			return;
		
		m_IsUnderRoofTick += delta;
		if(m_IsUnderRoofTick >= 3)
		{
			float hitFraction;
			vector hitPosition, hitNormal;
			vector from = GetPosition();
			vector to = from + "0 25 0";
			Object hitObject;
			PhxInteractionLayers collisionLayerMask = PhxInteractionLayers.BUILDING;
			
			bool is_under_roof = DayZPhysics.RayCastBullet(from, to, collisionLayerMask, null, hitObject, hitPosition, hitNormal, hitFraction);
			
			if(is_under_roof != m_IsUnderRoof)
			{
				SendHideZoneRPC(is_under_roof, false);
				
				m_IsUnderRoof = is_under_roof;
			}
			
			m_IsUnderRoofTick = 0;
		}
	}
	
	override void OnScheduledTick(float deltaTime)
	{
		super.OnScheduledTick(deltaTime);
		
		if(GetGame().IsClient())
		{
			CheckIsUnderRoof(deltaTime);
			
			if(GetPsiStormPhase() != BlowOutPhases.ACTIVE)
				m_CamEffects.OnUpdate(deltaTime);
			
			if(m_PsiStormHandlerClient)
			{
				m_PsiStormHandlerClient.OnUpdate(deltaTime);
				return;
			}
			
			if(m_BlowOutHandlerClient)
			{
				m_BlowOutHandlerClient.OnUpdate(deltaTime);
				return;
			}
			
			/*if(m_BlowOutPhase != BlowOutPhases.ACTIVE && m_PsiInfluence && !m_AmIMonolith) //(m_BlowOutPhase == BlowOutPhases.COOLDOWN || m_BlowOutPhase == BlowOutPhases.WARNING) && 
			{
				int psi_influence_type = PPERequester_OOTS_Anomaly_Client.PSI_INFLUENCE;
				if(m_PsiHelmet)
					psi_influence_type = PPERequester_OOTS_Anomaly_Client.PROTECTED_PSI_INFLUENCE;
				
				m_CamEffects.SetInfluence(psi_influence_type);
					
			}*/
			
			
		}
	}
	
	void SetSoundVolume(EffectSound sound, float volume)
	{
		if(sound)
			sound.SetSoundVolume(volume);
	}
		
	void ClearScreen(bool effect_stop = false)
	{
		if(!m_CamEffects)
			m_CamEffects = new CamEffects();
		
		m_CamEffects.SetInfluence(PPERequester_OOTS_Anomaly_Client.NONE_INFLUENCE, effect_stop);
	}
	

	private void StopAllEffects()
	{
		ClearScreen(true);
		
		if(m_PsiStormHandlerClient)
			m_PsiStormHandlerClient = null;
		
		if(m_BlowOutHandlerClient)
			m_BlowOutHandlerClient = null;
		
		if(m_PsiInfluenceLoop)
			StopSoundSet( m_PsiInfluenceLoop );
	}
	
	override void SetActions()
	{
		super.SetActions();

		AddAction(ActionDecodePsiTower);
	}
	
}