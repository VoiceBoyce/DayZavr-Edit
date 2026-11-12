class BlowOutHandler
{
	private PlayerBase m_Player;
	
	private int m_Phase;
	
	private bool m_IsHidden;
	private bool m_IsMonolith;
	private bool m_IsHiddenByTrigger;
	private bool m_IsItemProtected;
	private bool m_IsPsiHelmet;
	
	private float m_DamagePerTick = 0;
	private float m_DamageTimer = 0;
	private float m_DamageTick = 0;
	
	private int m_ProtectedTime;
	private float m_ProtectedTick;
	
	private bool m_CheckHiddenZoneWhenActive = false;
	private bool m_CheckRoofAsHiddenZone;
	private float m_CheckRoofAsHiddenZoneTick = 0;
	
	private bool m_IsPsiStorm;
	
	void BlowOutHandler(PlayerBase player)
	{
		m_Player = player;
	}
	
	void SetDamageParams(float time, float value)
	{
		m_DamageTimer = time;
		m_DamagePerTick = value;
	}
	
	void SetPhase(int phase, int phase_duration, bool psistorm = false)
	{
		if(m_Phase == phase)
			return;
		
		m_Phase = phase;
		
		m_IsPsiStorm = psistorm;
		
		Param2<int, int> phase_data = new Param2<int, int>(m_Phase, phase_duration);
		
		int rpc_type = PlayerRPCs.BLOWOUT_PHASE;
		if(psistorm)
			rpc_type = PlayerRPCs.PSISTORM_PHASE;
		
		GetGame().RPCSingleParam(m_Player, rpc_type, phase_data, true, m_Player.GetIdentity());
	}
	
	//==== Events =====
	void ItemAttachmentEvent()
	{
		bool is_monolith;
		bool is_item_protected;
		bool is_psi_helmet;
		GetBlowOutManager().CheckProtectetion(m_Player, is_monolith, is_item_protected, is_psi_helmet);
		
		if(m_IsMonolith != is_monolith || m_IsItemProtected != is_item_protected || m_IsPsiHelmet != is_psi_helmet)
		{
			m_IsMonolith = is_monolith;
			m_IsItemProtected  = is_item_protected;
			m_IsPsiHelmet = is_psi_helmet;
			
			m_Player.UpdateBlowoutNetSyncParams(m_IsMonolith, m_IsItemProtected, m_IsPsiHelmet);
		}
	}
	//=================
	
	//==== Protection =====
	bool IsProtected()
	{
		return m_ProtectedTime > 0;
	}
	
	void SetProtection(int time)
	{
		m_ProtectedTime += time;
		if(m_ProtectedTime > 600)
		{
			m_Player.SetHealth("GlobalHealth", "Shock", 5);
		}
	}
	
	void CheckHiddenUnderRoofs(bool state)
	{
		m_CheckRoofAsHiddenZone = state;
	}
	
	bool IsHidden()
	{
		return m_IsHidden;
	}
	
	void CheckHiddenWhenActive(bool state)
	{
		m_CheckHiddenZoneWhenActive = state;
	}
	
	void SetHiddenByTrigger(bool state)
	{
		m_IsHiddenByTrigger = state;
	}
	
	void SetHidden(bool state)
	{
		m_IsHidden = state;
	}
	
	private void CheckHiddenIsUnderRoof(float delta)
	{
		if(m_CheckHiddenZoneWhenActive && m_Phase == BlowOutPhases.COOLDOWN)
			return;
		
		m_CheckRoofAsHiddenZoneTick += delta;
		if(m_CheckRoofAsHiddenZoneTick >= 2)
		{
			bool is_hidden = m_Player.m_Environment.IsUnderRoof();
			if(is_hidden != m_IsHidden)
				m_Player.SetBlowOutHidden(is_hidden);
				
			m_CheckRoofAsHiddenZoneTick = 0;
		}
	}
	
	//=====================
	
	void OnTick(float delta)
	{
		/*if(m_CheckRoofAsHiddenZone && !m_IsHiddenByTrigger)
		{
			CheckHiddenIsUnderRoof(delta);
		}*/
		
		if(IsProtected())
		{
			m_ProtectedTick += delta;
			if(m_ProtectedTick >= 1)
			{
				m_ProtectedTime--;
				
				if(m_ProtectedTime > 0)
				{
					if(!m_Player.GetNotifiersManager().FindNotifier(eNotifiers.NTF_PILLS).IsActive())
						m_Player.GetNotifiersManager().ActivateByType(eNotifiers.NTF_PILLS);
				}
				else
				{
					if(m_Player.GetNotifiersManager().FindNotifier(eNotifiers.NTF_PILLS).IsActive())
						m_Player.GetNotifiersManager().DeactivateByType(eNotifiers.NTF_PILLS);
				}
				
				m_ProtectedTick = 0;
			}
		}
		
		
		if(m_IsPsiStorm)
		{
			HandlePsiStorm(delta);
			return;
		}
		
		if(m_Phase == BlowOutPhases.ACTIVE && !m_IsMonolith && !m_IsItemProtected)
		{
			m_DamageTick += delta;
			if(m_DamageTick >= m_DamageTimer)
			{
				int random = Math.RandomIntInclusive(0, 100);
				/* PrintFormat("Blowout: m_IsHidden %1, random %2, damagepertick %3, damagetimer %4", m_IsHidden, random, m_DamagePerTick, m_DamageTimer); */
				if(m_IsHidden)
				{
					float dmg = random;
					if(random <= 12)
					{
						m_Player.AddHealth("GlobalHealth", "Health", -(dmg/39));
						//m_Player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_VOMIT);
					}
					else
					{
						m_Player.AddHealth("GlobalHealth", "Health", -(dmg/65));
						m_Player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_COUGH);
					}
				}
				else
				{
					if(!IsProtected())
					{
						m_Player.AddHealth("", "Health", -m_DamagePerTick);
					}
					
					if(random <= 6)
					{
						m_Player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_VOMIT);
					}
					else
					{
						m_Player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_COUGH);
					}
				}
				
				m_DamageTick = 0;
			}
		}
	}
	
	void OnDisconect()
	{
		if(m_Phase == BlowOutPhases.END && !m_IsMonolith)
		{
			m_Player.SetHealth("GlobalHealth", "Shock", 100);
		}
	}
	
	
	void SetIsItemProtected(bool state)
	{
		
	}
	
	//==== monolith =====
	bool IsMonolith()
	{
		return m_IsMonolith;
	}
	
	void SetIsMonolith(bool state)
	{
		m_IsMonolith = state;
	}
	
	bool DefineIsMonolithItem(ItemBase item)
	{
		return GetBlowOutManager().IsMonolithItem(item);
	}
	
	//===================
	
	//===== PSI STRORM=====
	void HandlePsiStorm(float delta)
	{
		if(m_Phase == BlowOutPhases.ACTIVE && !m_IsMonolith)
		{
			m_DamageTick += delta;
			if(m_DamageTick >= m_DamageTimer)
			{
				int random = Math.RandomIntInclusive(0, 100);
				/* PrintFormat("PsiStorm: m_IsHidden %1, random %2, damagepertick %3, damagetimer %4", m_IsHidden, random, m_DamagePerTick, m_DamageTimer); */
				if(!m_IsHidden)
				{
					m_Player.AddHealth("", "Health", -m_DamagePerTick);
				}
				else
				{
					float dmg = random;
					if(random <= 10)
					{	
						m_Player.AddHealth("GlobalHealth", "Health", -(dmg/39));
					}
					else
					{
						m_Player.AddHealth("GlobalHealth", "Health", -(dmg/65));
					}
				}
				
				m_DamageTick = 0;
			}
		}
	}
}