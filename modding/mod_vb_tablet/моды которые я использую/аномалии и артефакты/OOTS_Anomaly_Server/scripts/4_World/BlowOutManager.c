class BlowOutManager: PluginBase
{
	private const string 						CONFIG_PATH = "$profile:\\AnomalySpawner\\BlowOutConfig.json";
	private float								m_MainTick = 0;
	private float 								m_SecTick = 0;
	private float 								m_CurPhaseDuration;
	
	
	private ref array<string> 					m_MonolithItems = new array<string>;
	private ref array<string>					m_ProtectItems = new array<string>;
	private bool 								m_HideZoneMessageEnabled = false;
	private bool 								m_UseHideZoneNotification = false;
	private string 								m_HideZoneNotificationTitle = "Сообщение";
	private string 								m_HideZoneNotificationIcon = "set:ccgui_enforce image:MapRadar";
	private float 								m_HideZoneNotificationShowTime = 3;
	private string 								m_HideZoneMessageEntry = "Здесь ты укроешься от выброса";
	private string 								m_HideZoneMessageLeave = "Ты покинул укрытие от выброса";
	private float 								m_UnconsciousTime = 5; //time player is unconscious
	
	private int 								m_NextBlowOut = 0; //time to next blowout in mins
	private int 								m_CoolDownInterval = 100;
	private int 								m_CoolDownSigma = 20;
	private int 								m_WarningTime = 5;
	private int 								m_LifeTime = 4;
	private int 								m_StepPhase = 0;
	private float 								m_DamageTime = 10; //how offen player must be damaged in secs
	private float 								m_DamageValue = 10;
	private bool 								m_HideZonesWhenActive = false;
	private bool 								m_HideZonesIsUnderRoofs = false;
	private ref array<HideZoneTrigger> 			m_HideZones;
	private ref array<HideZoneTrigger> 			m_RegisteredHideZones;
	private ref array<ref HideZoneConfig> 		m_HideZonesConfig;
	private int m_Hour;
	
	private bool 								m_EnablePsiTowers = true;
	private ref array<string> 					m_PsiHelmets = new array<string>;
	private ref array<ref PsiTowerConfig> 		m_PsiTowersConfig; 
	
	//sounds
	private ref array<string> 					m_AlertItemsTypes = new array<string>;
	private ref array<EntityAI> 				m_AlertItems = new array<EntityAI>;
	
	
	//
	private bool 								m_ManualBlowOut;
	
	//psistorm
	private ref PsiStormConfig 					m_PsiStormCfg;
	private ref PsiStormHandler 				m_PsiStormHandler; 
	
	void BlowOutManager()
	{
		ReadConfig();
		
		
	}
	
	void MissionLoaded()
	{
		GetNextBlowOutTime();
		
		if(!m_HideZonesWhenActive)
		{
			CreateHideZoneTriggers();
		}
		
		if(m_EnablePsiTowers)
		{
			CreatePsiTowers();
		}
	}
	
	//config reader
	private void ReadConfig()
	{
		BlowOutConfig config;
		if( FileExist(CONFIG_PATH) )
		{
			JsonFileLoader<BlowOutConfig>.JsonLoadFile(CONFIG_PATH, config);
			if(config)
			{
				m_CoolDownInterval = config.interval;
				m_CoolDownSigma = config.sigma;
				m_WarningTime = config.warning;
				m_LifeTime = config.lifetime;
				m_DamageTime = config.damagetime;
				m_DamageValue = config.damagevalue;
				m_HideZonesIsUnderRoofs = config.hidezoneisunderroofs;
				m_HideZonesWhenActive = config.hidezoneswhenactive;
				m_HideZonesConfig = config.hidezones;
				m_UnconsciousTime = config.unconscioustime;
				m_PsiStormCfg = config.psistorm;
				
				foreach(string type: config.alertitems)
				{
					m_AlertItemsTypes.Insert(type);	
				}
				foreach(string item: config.monolithitems)
				{
					m_MonolithItems.Insert(item);
				}
				
				foreach(string p_item: config.protectitems)
				{
					m_ProtectItems.Insert(p_item);
				}
				
				m_HideZoneMessageEnabled = config.enablehidezonemessage;
				m_HideZoneMessageEntry = config.hidezoneentrymessage;
				m_HideZoneMessageLeave = config.hidezoneleavemessage;
				
				m_UseHideZoneNotification = config.usehidezonenotification;
				m_HideZoneNotificationTitle = config.hidezonenotificationtitle;
				m_HideZoneNotificationIcon = config.hidezonenotificationicon;
				m_HideZoneNotificationShowTime = config.hidezonenotificationshowtime;
				
				m_EnablePsiTowers = config.enablepsitowers;
				foreach(string psiHelmet: config.psihelmets)
				{
					m_PsiHelmets.Insert(psiHelmet);
				}
				m_PsiTowersConfig = config.psitowers;
				
			}
			else
			{
				PrintFormat("[BlowOut]: Occured some errors while reading config. Please, check your config is correct");
			}
		}
		else
		{
			Print("[BlowOut]: Creating default config...");
			config = CreateDefaultConfig();
			JsonFileLoader<BlowOutConfig>.JsonSaveFile(CONFIG_PATH, config);
		}
	}
	
	private BlowOutConfig CreateDefaultConfig()
	{
		BlowOutConfig config = new BlowOutConfig();
		config.interval = m_CoolDownInterval;
		config.sigma = m_CoolDownSigma;
		config.warning = m_WarningTime;
		config.lifetime = m_LifeTime;
		config.damagetime = m_DamageTime;
		config.damagevalue = m_DamageValue;
		config.hidezoneswhenactive = m_HideZonesWhenActive;
		config.hidezones = m_HideZonesConfig;
		config.unconscioustime = m_UnconsciousTime;
		
		config.psistorm = new PsiStormConfig;
		config.psistorm.chance = 0.5;
		config.psistorm.damagetime = 10;
		config.psistorm.damagetime = 4;
		
		array<string> ai = {"PersonalRadio"};
		config.alertitems = ai;
		array<string> mi = new array<string>;
		mi.Insert("Monolith_Shevron_1");
		mi.Insert("Monolith_Shevron_2");
		config.monolithitems = mi;
		
		config.protectitems = {"Mich2001Helmet", "WorkingGloves_Black"};
		
		config.enablehidezonemessage = m_HideZoneMessageEnabled;
		config.hidezoneentrymessage = m_HideZoneMessageEntry;
		config.hidezoneleavemessage = m_HideZoneMessageLeave;
		
		array<ref HideZoneConfig> hidezones = new array<ref HideZoneConfig>;
		//array<string> shapes = {"BOX", "SPHERE", "CYLINDER"};
		for(int i = 0; i < 3; i++)
		{
			HideZoneConfig hzc = new HideZoneConfig();
			hzc.position = GenerateVector( i + 1 );
			hzc.radius = 10;
			hzc.height = 5;
			//hzc.shape = shapes[i];
			//hzc.direction = "0 0 0";
			//hzc.minvector = GenerateVector( - (i + 2) );
			//hzc.maxvector = GenerateVector( (i + 2) );
			hidezones.Insert(hzc);
		}
		
		config.hidezones = hidezones;
		
		config.enablepsitowers = true;
		
		array<string> ph = new array<string>;
		ph.Insert("Psi_helmet_1");
		ph.Insert("Psi_helmet_2");
		config.psihelmets = ph;
		
		array<ref PsiTowerConfig> psitowers = new array<ref PsiTowerConfig>;
		for(int j = 0; j < 3; j++)
		{
			PsiTowerConfig ptc = new PsiTowerConfig();
			ptc.position = GenerateVector( j + 1 );
			ptc.radius = 10;
			ptc.height = 5;
			psitowers.Insert(ptc);
		}
		
		config.psitowers = psitowers;
		
		return config;
	}
	
	vector GenerateVector(int num)
	{
		vector v = vector.Zero;
		for(int i = 0; i < 3; i++)
		{
			v[i] = num;
		}
		
		return v;
	}
	
	//blowout handler
	int GetPhaseDuration(int phase)
	{
		if(m_PsiStormHandler)
		{
			return m_PsiStormHandler.GetCurPhaseDuration();
		}
		
		int phase_time;
		
		switch(phase)
		{
			case BlowOutPhases.WARNING:
				phase_time = m_WarningTime;
			break;
			case BlowOutPhases.ACTIVE:
				phase_time = m_LifeTime;
			break;
			case BlowOutPhases.END:
				phase_time = m_UnconsciousTime;
			break;
		}
		
		return phase_time - m_CurPhaseDuration;
	}
	
	void GetNextBlowOutTime()
	{
		m_NextBlowOut = m_CoolDownInterval + Math.RandomIntInclusive(-m_CoolDownSigma, m_CoolDownSigma);
	}
	
	void GetParams(out float next_blow_out, out float warning_time, out float life_time, out float unc_time)
	{
		next_blow_out = m_NextBlowOut;
		warning_time = m_WarningTime;
		life_time = m_LifeTime;
		unc_time = m_UnconsciousTime;
	}
	
	float GetMainTick()
	{
		return m_MainTick;
	}
	
	bool HideZonesWhenActive()
	{
		return m_HideZonesWhenActive;
	}
	
	void PlayerOnConnect(PlayerBase player)
	{
		if(player)
		{
			float dmg_time = m_DamageTime;
			float dmg_value = m_DamageValue;
			int phase = m_StepPhase;
			bool psi_storm = false;
			
			
			if(m_PsiStormHandler)
			{
				dmg_time = m_PsiStormCfg.GetDamageTime();
				dmg_value = m_PsiStormCfg.GetDamageValue();
				phase = m_PsiStormHandler.GetPhase();
				psi_storm = true;
			}
			
			BlowOutHandler handler = player.GetBlowOutHandler();
			if(handler)
			{
				handler.SetDamageParams(dmg_time, dmg_value);
				handler.SetPhase(phase, GetPhaseDuration(phase), psi_storm);
				handler.CheckHiddenUnderRoofs(m_HideZonesIsUnderRoofs);
				handler.CheckHiddenWhenActive(m_HideZonesWhenActive);
			}
		}
	}
	
	/*void PlayerOnDisconnect(PlayerBase player)
	{
		if(player)
		{
			GetGame().RPCSingleParam(player, PlayerRPCs.END_SESSION, null, true, player.GetIdentity());
			if(m_StepPhase == BlowOutPhases.END && !player.IsMonolith())
			{
				player.SetHealth("GlobalHealth", "Shock", 100);
			}
		}
	}*/
	
	void OnPsiStormEnd()
	{
		m_PsiStormHandler = null;
		
		//Main timer reset
		m_MainTick = 0;
		
		GetNextBlowOutTime();
	}
	
	void SetPhase(int phase, bool psi_storm = false)
	{
		
		if(!psi_storm && m_PsiStormHandler)
		{
			return;
		}
		
		if(psi_storm && m_StepPhase == BlowOutPhases.COOLDOWN && phase == BlowOutPhases.WARNING && !m_PsiStormHandler)
		{
			m_PsiStormHandler = new PsiStormHandler(this, m_PsiStormCfg);
			return;
		}
		
		if(psi_storm && m_PsiStormHandler)
		{
			m_PsiStormHandler.SetPhase(phase);
			return;
		}
		
		switch(phase)
		{
			case BlowOutPhases.COOLDOWN:
			break;
			case BlowOutPhases.WARNING:
				m_ManualBlowOut = true;
				if(m_StepPhase == BlowOutPhases.COOLDOWN)
				{
					m_MainTick = (m_NextBlowOut - m_WarningTime) + 0.02;
				}
			break;
			case BlowOutPhases.ACTIVE:
				if(m_StepPhase == BlowOutPhases.WARNING)
					m_MainTick = m_NextBlowOut + 0.02;
			break;
			case BlowOutPhases.END:
				if(m_StepPhase == BlowOutPhases.ACTIVE)
					m_MainTick = m_NextBlowOut + m_LifeTime + 0.02;
			break;
		}
	}
	
	override void OnUpdate(float delta_time)
	{
		if(m_PsiStormHandler)
		{
			m_PsiStormHandler.OnUpdate(delta_time);
			return;
		}
		
		m_MainTick += delta_time / 60;
		
		if(m_StepPhase == BlowOutPhases.WARNING || m_StepPhase == BlowOutPhases.ACTIVE)
			m_CurPhaseDuration += delta_time / 60;
		
		//warning
		if(m_MainTick >= m_NextBlowOut - m_WarningTime && m_MainTick < m_NextBlowOut)
		{
			
			
			if(m_StepPhase != BlowOutPhases.WARNING)
			{
				if(!m_ManualBlowOut && m_PsiStormCfg.ShouldHappen())
				{
					m_PsiStormHandler = new PsiStormHandler(this, m_PsiStormCfg);
					return;
				}
				
				m_ManualBlowOut = false;
				
				PrintFormat("[BlowOut]: Warning - %1 mins, MissionTime - %2", m_MainTick, GetMissionTime());
				Warning(BlowOutPhases.WARNING);
				
				if(m_HideZonesWhenActive)
				{
					CreateHideZoneTriggers();
				}
				
				m_CurPhaseDuration = 0;
			}

			m_StepPhase = BlowOutPhases.WARNING;
		}
		else if(m_MainTick >= m_NextBlowOut && m_MainTick < m_NextBlowOut + m_LifeTime) //start
		{
			
			if(m_StepPhase != BlowOutPhases.ACTIVE)
			{
				PrintFormat("[BlowOut]: Active - %1 mins, MissionTime - %2", m_MainTick, GetMissionTime());
				Active(BlowOutPhases.ACTIVE);
				
				m_CurPhaseDuration = 0;
			}
			
			/*m_SecTick += delta_time;
			if(m_SecTick >= m_DamageTime)
			{
				//PrintFormat("[BlowOut]: DamagePlayer - %1 mins, %2 secs, MissionTime - %3 mins", m_MainTick, m_SecTick, GetGame().GetTime()/60000);
				DamagePlayers();
				m_SecTick = 0;
			}*/

			m_StepPhase = BlowOutPhases.ACTIVE;
		}
		else if(m_MainTick >= m_NextBlowOut + m_LifeTime) //end
		{
			PrintFormat("[BlowOut]: End - %1 mins, MissionTime - %2", m_MainTick, GetMissionTime());
			End(BlowOutPhases.END);
			
			m_CurPhaseDuration = 0;
			
			//Respawn anomalies and artefact triggers
			AnomalySpawner.ClearAndReconfigure();
			
			if(m_HideZonesWhenActive)
			{
				RemoveHideZoneTriggers();
			}
			
			//Main timer reset
			m_MainTick = 0;
			
			GetNextBlowOutTime();
			
			m_SecTick = 0;
			m_StepPhase = BlowOutPhases.END;
		}
		
		//UnconsciousTime
		if(m_StepPhase == BlowOutPhases.END)
		{
			m_SecTick += delta_time;
			if(m_SecTick >= m_UnconsciousTime)
			{
				PrintFormat("[BlowOut]: Final - MissionTime - %1", GetMissionTime());
				StandUp(BlowOutPhases.COOLDOWN);
				m_StepPhase = BlowOutPhases.COOLDOWN;
				m_SecTick = 0;
				//Main timer reset
				m_MainTick = 0;
				m_CurPhaseDuration = 0;
				m_ManualBlowOut = false;
			}
		}	
	}
	
	//tools
	void SendAlertItemsRPC(int phase, bool psi_storm = false)
	{
		if(m_AlertItems.Count() > 0)
		{
			ItemBase item;
			int sound_index = Math.RandomIntInclusive(0, 2);
			foreach(EntityAI entity: m_AlertItems)
			{
				if(entity && Class.CastTo(item, entity))
				{
					item.SetBOSound(phase, sound_index, psi_storm);
				}
			}
		}
	}
	
	string GetMissionTime()
	{
		int time = GetGame().GetTime();
		
		int secs = time/1000;
		int msecs = time % 1000;
		int mins = secs/60;
		secs = secs % 60;
		int hours = mins/60;
		mins = mins % 60;
		
		return hours.ToString() + ":" + GetTimeStr(mins) + ":" + GetTimeStr(secs) + ":" + GetTimeStr(msecs, true);
	}
	
	string GetTimeStr(int num, bool ms = false)
	{
		if(ms)
		{
			if(num < 10)
			{
				return "00" + num.ToString();
			}
			else if(num < 100)
			{
				return "0" + num.ToString();
			}
		}
		else
		{
			if(num < 10)
			{
				return "0" + num.ToString();
			}
		}
		
		return num.ToString();
	}
	
	//phases functions
	void Warning(int phase)
	{
		GetGame().GetWeather().SetWindSpeed( 0.75 );								
		GetGame().GetWeather().SetStorm( 1, 0.75, 10);
		GetGame().GetWeather().GetOvercast().Set( 0.75, 0, 1000 );
		
		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);
		foreach(Man man: players)
		{
			PlayerBase player;
			if(Class.CastTo(player, man))
			{
				//player.SetBlowOutPhase(phase);
				player.GetBlowOutHandler().SetPhase(phase, GetPhaseDuration(phase));
			}
		}
		
		
		SendAlertItemsRPC(phase);
	}
	
	void PlayerPhase(PlayerBase player, int monolith_rpc_type, int stalker_rpc_type, int phase)
	{
		//player.SetBlowOutPhase(phase);
		
		player.GetBlowOutHandler().SetPhase(phase, GetPhaseDuration(phase));
	}
	
	void Active(int phase)
	{
		GetGame().GetWeather().SetWindSpeed( 0.85 );
		GetGame().GetWeather().SetStorm( 1, 0.85, 1);
		GetGame().GetWeather().GetOvercast().Set( 0.95, 0, 1000 );
		
		int year;
		int month;
		int day;
		int hour;
		int minute;
		GetGame().GetWorld().GetDate(year, month, day, hour, minute);
		
		m_Hour = hour;
		
		GetGame().GetWorld().SetDate(year, month, day, 20, minute);
		
		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);
		foreach(Man man: players)
		{
			PlayerBase player;
			if(Class.CastTo(player, man))
			{
				//player.SetBlowOutPhase(phase);
				player.GetBlowOutHandler().SetPhase(phase, GetPhaseDuration(phase));
			}
		}
		
		SendAlertItemsRPC(phase);
	}
	
	//TODO Damage Make in PlayerBase
	/*void DamagePlayers()
	{
		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);
		foreach(Man man: players)
		{
			PlayerBase player;
			if(Class.CastTo(player, man))
			{
				if(!IsMonolith(player))
				{
					int random = Math.RandomIntInclusive(1, 100);
					if(player.IsBlowOutHidden())
					{
						float dmg = random;
						if(random <= 13)
						{
							player.AddHealth("", "Health", -(dmg/39));
							player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_VOMIT);
						}
						else
						{
							player.AddHealth("", "Health", -(dmg/65));
							player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_COUGH);
						}
					}
					else
					{
						if(!player.IsBlowOutProtected())
						{
							player.AddHealth("", "Health", -m_DamageValue);
						}
						if(random <= 7)
						{
							player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_VOMIT);
						}
						else
						{
							player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_COUGH);
						}
					}
				}
			}
		}
	}*/
	
	void End(int phase)
	{
		int year;
		int month;
		int day;
		int hour;
		int minute;
		GetGame().GetWorld().GetDate(year, month, day, hour, minute);
		
		GetGame().GetWorld().SetDate(year, month, day, m_Hour, minute);
		
		GetGame().GetWeather().SetWindSpeed( 0.1 );
		GetGame().GetWeather().SetStorm( 0, 0.9, 5);
		GetGame().GetWeather().GetOvercast().Set( 0.1, 0, 1000 );
		
		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);
		foreach(Man man: players)
		{
			PlayerBase player;
			if(Class.CastTo(player, man))
			{
				if(!player.IsMonolith() && m_UnconsciousTime > 0)
				{
					player.SetHealth("GlobalHealth", "Shock", 0);
				}
				
				//player.SetBlowOutPhase(phase);
				player.GetBlowOutHandler().SetPhase(phase, GetPhaseDuration(phase));
			}
		}
	}
	
	void StandUp(int phase)
	{
		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);
		foreach(Man man: players)
		{
			PlayerBase player;
			if(Class.CastTo(player, man))
			{
				/* if(m_HideZonesWhenActive)
				{
					player.SetBlowOutHidden(false);
				} */
				
				if(!player.IsMonolith())
				{
					player.SetHealth("GlobalHealth", "Shock", 100);
				}
				
				player.GetBlowOutHandler().SetPhase(phase, GetPhaseDuration(phase));
				//player.SetBlowOutPhase(phase);
			}
		}
		
		SendAlertItemsRPC(phase);
	}
	
	void CheckProtectetion(Man player, out bool is_monolith, out bool item_protected, out bool psi_helmet)
	{
		for(int i = 0; i < player.GetInventory().AttachmentCount(); i++)
		{
			ItemBase attachment	= ItemBase.Cast(player.GetInventory().GetAttachmentFromIndex(i));
			if(attachment)
			{
				if(!is_monolith && m_MonolithItems.Find(attachment.GetType()) > INDEX_NOT_FOUND)
				{
					is_monolith = true;
				}
				
				if(!item_protected && m_ProtectItems.Find(attachment.GetType()) > INDEX_NOT_FOUND)
				{
					item_protected = true;
				}
				
				if(!psi_helmet)
				{
					if(m_PsiHelmets.Find(attachment.GetType()) > INDEX_NOT_FOUND)
					{
						psi_helmet = true;
					}
							
					if(attachment.GetInventory() && attachment.GetInventory().AttachmentCount() > 0)
					{
						for(int j = 0; j < attachment.GetInventory().AttachmentCount(); j++)
						{
							ItemBase sub_attachment	= ItemBase.Cast(attachment.GetInventory().GetAttachmentFromIndex(j));
							if(sub_attachment)
							{
								if(m_PsiHelmets.Find(sub_attachment.GetType()) > INDEX_NOT_FOUND)
								{
									psi_helmet = true;
								}
							}
						}
					}
				}
			}
		}
	}
	
	
	bool IsMonolithItem(ItemBase item)
	{
		if(m_MonolithItems && m_MonolithItems.Find(item.GetType()) > INDEX_NOT_FOUND)
			return true;
		
		return false;
	}
	
	bool IsMonolith(PlayerBase player)
	{
		if(m_MonolithItems && m_MonolithItems.Count() > 0)
		{
			int count = player.GetInventory().AttachmentCount();
			for(int i = 0; i < count; i++)
			{
				ItemBase attachment	= ItemBase.Cast(player.GetInventory().GetAttachmentFromIndex(i));
				if(attachment)
				{
					if(m_MonolithItems.Find(attachment.GetType()) >= 0)
					{
						return true;
					}
				}
			}
		}
		
		return false;
	}
	
	bool IsItemProtected(PlayerBase player)
	{
		if(m_ProtectItems && m_ProtectItems.Count() > 0)
		{
			int count = player.GetInventory().AttachmentCount();
			for(int i = 0; i < count; i++)
			{
				ItemBase attachment	= ItemBase.Cast(player.GetInventory().GetAttachmentFromIndex(i));
				if(attachment)
				{
					if(m_ProtectItems.Find(attachment.GetType()) >= INDEX_NOT_FOUND)
					{
						return true;
					}
				}
			}
		}
		
		return false;
	}
	
	bool IsPsiHelmet(PlayerBase player)
	{
		if(m_PsiHelmets && m_PsiHelmets.Count() > 0)
		{
			int count = player.GetInventory().AttachmentCount();
			for(int i = 0; i < count; i++)
			{
				ItemBase attachment	= ItemBase.Cast(player.GetInventory().GetAttachmentFromIndex(i));
				if(attachment)
				{
					if(m_PsiHelmets.Find(attachment.GetType()) >= 0)
						return true;
					
					if(attachment.GetInventory() && attachment.GetInventory().AttachmentCount() > 0)
					{
						for(int j = 0; j < attachment.GetInventory().AttachmentCount(); j++)
						{
							ItemBase sub_attachment	= ItemBase.Cast(attachment.GetInventory().GetAttachmentFromIndex(j));
							if(sub_attachment)
							{
								if(m_PsiHelmets.Find(sub_attachment.GetType()) >= 0)
									return true;
							}
						}
					}
				}
			}
		}
		return false;
	}
	
	//hide zones
	void SendPlayerHideZoneState(PlayerBase player, bool state)
	{
		if(player.IsMonolith())
			return;
		
		if(!m_HideZoneMessageEnabled)
			return;
		
		if(m_HideZonesWhenActive)
        {
            if(m_PsiStormHandler)
            {
                if(m_PsiStormHandler.GetPhase() == BlowOutPhases.COOLDOWN)
                {
                    return;
                }
            }
            else if(m_StepPhase == BlowOutPhases.COOLDOWN)
            {
                return;
            }
        }
		
		string message;
		if(state)
			message = m_HideZoneMessageEntry;
		else
			message = m_HideZoneMessageLeave;

		if(m_UseHideZoneNotification && m_HideZoneNotificationTitle.Length() > 0)
		{
			NotificationSystem.SendNotificationToPlayerExtended(player, m_HideZoneNotificationShowTime, m_HideZoneNotificationTitle, message, m_HideZoneNotificationIcon);
		}
		else
			player.MessageAction(message);
		
		
	}
	
	void CreateHideZoneTriggers()
	{
		if(m_HideZonesConfig.Count() > 0)
		{
			if(m_HideZonesWhenActive)
			{
				m_HideZones = new array<HideZoneTrigger>;
			}
			
			foreach(HideZoneConfig hidezone: m_HideZonesConfig)
			{
				HideZoneTrigger trigger = HideZoneTrigger.Cast( GetGame().CreateObject("HideZoneTrigger", hidezone.position ));
				if(trigger)
				{
					trigger.SetMessages(m_HideZoneMessageEnabled, m_HideZoneMessageEntry, m_HideZoneMessageLeave);
					//trigger.SetCollisionCylinder(hidezone.radius, hidezone.height);
					vector min = vector.Zero;
					vector max = vector.Zero;
					
					for(int i = 0; i < 3; i++)
					{
						if(i == 1)
						{
							min[i] = -hidezone.height;
							max[i] = hidezone.height;
						}
						else
						{
							min[i] = -hidezone.radius;
							max[i] = hidezone.radius;
						}
					}
					
					trigger.SetExtents(min, max);
					
					if(m_HideZonesWhenActive)
					{
						m_HideZones.Insert(trigger);
					}
				}
			}
		}
	}

	void RemoveHideZoneTriggers()
	{
		if(m_HideZones && m_HideZones.Count() > 0)
		{
			foreach(HideZoneTrigger trigger: m_HideZones)
			{
				if(trigger)
				{	
					GetGame().ObjectDelete(trigger);
				}
				
			}
			
			m_HideZones.Clear();
			delete m_HideZones;
		}
		
		if(m_RegisteredHideZones && m_RegisteredHideZones.Count() > 0)
		{
			foreach(HideZoneTrigger registered_trigger: m_RegisteredHideZones)
			{
				if(registered_trigger)
				{	
					GetGame().ObjectDelete(registered_trigger);
				}
			}
			
			m_RegisteredHideZones.Clear();
			delete m_RegisteredHideZones;
		}
	}
	
	void RegisterHideZoneTrigger(HideZoneTrigger trigger)
	{
		if(!m_RegisteredHideZones)
			m_RegisteredHideZones = new array<HideZoneTrigger>;
		
		if(m_HideZoneMessageEnabled)
			trigger.SetMessages(m_HideZoneMessageEnabled, m_HideZoneMessageEntry, m_HideZoneMessageLeave);
		
		m_RegisteredHideZones.Insert(trigger);
	}
	
	void DebugHideZoneTrigger(PlayerBase player, bool state)
	{
		Param1<bool> t_data = new Param1<bool>(state);
		if(m_HideZones && m_HideZones.Count() > 0)
		{
			foreach(HideZoneTrigger trigger: m_HideZones)
			{
				if(trigger)
				{	
					GetGame().RPCSingleParam(trigger, HideZoneTriggerRPCs.DEBUG, t_data, true, player.GetIdentity());
				}
				
			}
		}
		
		if(m_RegisteredHideZones && m_RegisteredHideZones.Count() > 0)
		{
			foreach(HideZoneTrigger registered_trigger: m_RegisteredHideZones)
			{
				if(registered_trigger)
				{	
					GetGame().RPCSingleParam(registered_trigger, HideZoneTriggerRPCs.DEBUG, t_data, true, player.GetIdentity());
				}
			}
		}
	}
	
	//PsiTowers
	void CreatePsiTowers()
	{
		if(m_PsiTowersConfig && m_PsiTowersConfig.Count() > 0)
		{
			foreach(PsiTowerConfig tower: m_PsiTowersConfig)
			{
				PsiTowerTrigger trigger = PsiTowerTrigger.Cast( GetGame().CreateObjectEx("PsiTowerTrigger", tower.position, ECE_NONE) ); // GetGame().CreateObject("PsiTowerTrigger", tower.position)
				if(trigger)
				{
					trigger.SetOrientation(Vector(0, 0, 0));
					trigger.SetCollisionCylinder(tower.radius, tower.height);
					trigger.Update();
				}
			}
		}
	}
	
	bool IsSoundItem(EntityAI entity)
	{
		if(m_AlertItemsTypes.Count() > 0 && m_AlertItemsTypes.Find(entity.GetType()) > -1)
		{
			m_AlertItems.Insert(entity);
			return true;
		}
		return false;
	}
}

class BlowOutConfig
{
	int interval;
	int sigma;
	int warning;
	int lifetime;
	float damagetime;
	float damagevalue;
	float unconscioustime;
	ref PsiStormConfig psistorm;
	ref array<string> alertitems;
	ref array<string> monolithitems;
	ref array<string> protectitems;
	bool enablehidezonemessage;
	bool usehidezonenotification;
	string hidezonenotificationtitle = "Сообщение";
	string hidezonenotificationicon = "set:ccgui_enforce image:MapRadar";
	float hidezonenotificationshowtime = 3;
	string hidezoneentrymessage;
	string hidezoneleavemessage;
	bool hidezoneisunderroofs;
	bool hidezoneswhenactive;
	ref array<ref HideZoneConfig> hidezones;
	bool enablepsitowers;
	ref array<string> psihelmets;
	ref array<ref PsiTowerConfig> psitowers;
}

class HideZoneConfig
{
	vector position;
	//string shape;
	//vector direction;
	//vector minvector;
	//vector maxvector;
	float radius;
	float height;
}

class PsiTowerConfig
{
	vector position;
	float radius;
	float height;
}

class PsiStormConfig
{
	float chance;
	float damagetime;
	float damagevalue;
	
	bool ShouldHappen()
	{
		if(chance >= Math.RandomFloat01())
			return true;
		
		return false;
	}
	
	float GetDamageTime()
	{
		return damagetime;
	}
	
	float GetDamageValue()
	{
		return damagevalue;
	}
}

/*class PsiEmitConfig
{
	ref array<string> monolith_items;
	ref array<string> psi_helmets;
}*/

BlowOutManager GetBlowOutManager()
{
	return BlowOutManager.Cast(GetPluginManager().GetPluginByType(BlowOutManager));
}