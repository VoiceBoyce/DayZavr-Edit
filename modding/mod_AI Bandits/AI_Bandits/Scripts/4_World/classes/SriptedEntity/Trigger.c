class AIBTalkTrigger extends Trigger
{
    protected string m_TriggerName;
	protected int m_NPCid;
	protected string m_TriggerNotification;
	protected int m_TriggerNotificationTime;
	protected ref TStringArray m_SoundList;
	protected ref TStringArray m_EmoteList;
	protected int m_TriggerCooldown;
	protected int m_LastTriggerTime;
	protected int m_Whistle;
	protected InfectedBanditBase m_AssignedNPC = NULL;
	protected ref EffectSound m_WhistleSound;

	void AIBTalkTrigger()
	{
		RegisterNetSyncVariableInt("m_Whistle");

	}

	void StopTalkingNPC()
	{
		//Print("STOP TALKING");
		GetRPCManager().SendRPC( "AIBandits", "StopTalk", new Param1< vector >( this.GetPosition() ) );
	}

	void AdditionalAction()
	{
		// Override this, if you need custom actions serverside
	}


	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		//GetGame().GetMission().OnEvent(ChatMessageEventTypeID, new ChatMessageEventParams(CCDirect, "", "Vars synchronized", ""));
		if (m_Whistle>0)
		{
			WhistleSoundPlay(m_Whistle);
		}
    }

	void WhistleSoundPlay(int index)
	{
		// Озвучка из мода убрана. Если звука нет, NPC просто молчит, а не роняет клиент на null
		string soundSet = "Talking_SoundSet" + index.ToString();
		if (!GetGame().ConfigIsExisting("CfgSoundSets " + soundSet))
		{
			ResetWhistle();
			StopTalkingNPC();
			return;
		}

		m_WhistleSound = SEffectManager.PlaySoundOnObject( soundSet, this );
		if (!m_WhistleSound)
		{
			ResetWhistle();
			StopTalkingNPC();
			return;
		}
		m_WhistleSound.Event_OnSoundWaveEnded.Insert(StopTalkingNPC);
		m_WhistleSound.SetSoundAutodestroy(true);
		ResetWhistle();
		// in prison react to player voice
		// when they say something npc will say shut up

	}

	void BlowWhistle(int index)
	{
		m_Whistle = index;
		SetSynchDirty();
	}

	void ResetWhistle()
	{
		m_Whistle = 0;
		SetSynchDirty();
	}


	void SetTriggerName(string name)
	{
		m_TriggerName = name;
	}

	string GetTriggerName()
	{
		return m_TriggerName;
	}

	void SetTriggerNPCid(int id)
	{
		m_NPCid = id;
	}

	int GetTriggerNPCid()
	{
		return m_NPCid;
	}

	void SetTriggerNotification(string notification)
	{
		m_TriggerNotification = notification;
	}

	string GetTriggerNotification()
	{
		return m_TriggerNotification;
	}

	void SetTriggerNotificationTime(int notificationtime)
	{
		m_TriggerNotificationTime = notificationtime;
	}

	int GetTriggerNotificationTime()
	{
		return m_TriggerNotificationTime;
	}

	void SetSounds(TStringArray sounds)
	{
		m_SoundList = sounds;
	}

	TStringArray GetSounds()
	{
		return m_SoundList;
	}

	void SetEmotes(TStringArray emotes)
	{
		m_EmoteList = emotes;
	}

	TStringArray GetEmotes()
	{
		return m_EmoteList;
	}

	void SetTriggerCooldown(int cooldown)
	{
		m_TriggerCooldown = cooldown;
	}

	void SetLastTriggerTime(int time)
	{
		m_LastTriggerTime = time;
	}

	bool CanTriggerAction(int time)
	{
		if (time > m_LastTriggerTime+m_TriggerCooldown)
		{
			m_LastTriggerTime = time;
			return true;
		}

		return false;
	}

	void AssignNPC(InfectedBanditBase npcsurvivor)
	{
		m_AssignedNPC = npcsurvivor;
	}

	InfectedBanditBase GetAssignedNPC()
	{
		return m_AssignedNPC;
	}

    override bool CanAddObjectAsInsider(Object object)
    {
        PlayerBase player = PlayerBase.Cast(object);
		InfectedBanditBase npcsurvivor = InfectedBanditBase.Cast(object);
        if (player || npcsurvivor)
			return true;
		return false;
		//return player != null;
    }

	void DetectNPC(vector position)
	{
		array<Object> objects_in_vicinity = new array<Object>;
		array<CargoBase> proxyCargos = new array<CargoBase>;
		Object object_in_radius;
		GetGame().GetObjectsAtPosition3D(position, 1.1, objects_in_vicinity, proxyCargos);
		for ( int j = 0; j < objects_in_vicinity.Count(); j++ )
		{
			object_in_radius = objects_in_vicinity.Get(j);

			if (object_in_radius.IsInherited(InfectedBanditBase))
			{
				AssignNPC(InfectedBanditBase.Cast(object_in_radius));
			}
		}

	}

	override void OnEnter(Object obj)
    {
		//SPBLogger.Log( "SpawnerBubaku ENTER" );
		//Print("ENTER Object " + obj);
		// pozici npc vezmu z triggeru a poslu mu prikaz
		if ( GetGame().IsServer())
		{
			// porovnat s ulozenym casem tiku, kdyz je vetsi nez cooldown ulozit novy a povolit akci
			//GetGame().CreateObject("Seachest", obj.GetPosition() );
			//GetGame().CreateObject("ZmbM_ClerkFat_White", obj.GetPosition(), false, true, true );
			//SPBLogger.Log( "Triggered " + GetTriggerName() + " time " + GetGame().GetTime()/1000);

			DetectNPC(this.GetPosition());

			InfectedBanditBase npcsurvivor;
			Class.CastTo(npcsurvivor,obj);
			//Print("npc survivor " + obj);
			if (npcsurvivor)
			{
				if ( vector.Distance(npcsurvivor.GetPosition(),this.GetPosition()) < 1.1 )
				{
					AssignNPC(npcsurvivor);
					npcsurvivor.SetNPCID(GetTriggerNPCid());
					if (GetTriggerNPCid() >0)
					{
						AIBLogger.Log("Assigned trigger " + GetTriggerName() + " to NPC " + npcsurvivor.GetType() + " id " + GetTriggerNPCid());
					}
					else
					{
						AIBLogger.Log("Assigned trigger " + GetTriggerName() + " to NPC " + npcsurvivor.GetType());
					}

				}
			}
			if (CanTriggerAction(GetGame().GetTime()/1000))
			{
				if (PlayerBase.Cast(obj))
				{
					if (PlayerBase.Cast(obj).GetIdentity())
					{
						if (PlayerBase.Cast(obj).GetIdentity().GetName())
						{
							//SPBLogger.Log("Can trigger action " + GetGame().GetTime()/1000 + " last " + m_LastTriggerTime + " trigger delay " + m_TriggerCooldown);
							SetLastTriggerTime(GetGame().GetTime()/1000);
							//SpawniBubaky();
							BlowWhistle(GetSounds().GetRandomElement().ToInt());
							AdditionalAction(); //Thx Inkihh
							if(GetAssignedNPC())
							{
								AIBLogger.Debug("NPC " + GetAssignedNPC() + " should do emote and talk");
								GetAssignedNPC().SetTalkNPC(true);
								GetAssignedNPC().ShouldPlayEmoteID(GetEmotes().GetRandomElement().ToInt());
							}
							if (GetTriggerNotification() != "")
							{
								NotificationSystem.SendNotificationToPlayerExtended(Man.Cast(obj), GetTriggerNotificationTime(), GetTriggerName(), GetTriggerNotification());
							}
							AIBLogger.Debug( PlayerBase.Cast(obj).GetIdentity().GetName() + " triggered " + GetTriggerName());
							GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( ResetWhistle, 1000, false );
						}
					}
				}
			}
		}
    }
};

class BanditSniperTrigger extends Trigger
{
	protected InfectedBanditBase m_AssignedNPC = NULL;

	void AssignNPC(InfectedBanditBase npcsurvivor)
	{
		m_AssignedNPC = npcsurvivor;
	}

	InfectedBanditBase GetAssignedNPC()
	{
		return m_AssignedNPC;
	}

	override bool CanAddObjectAsInsider(Object object)
    {
        PlayerBase player = PlayerBase.Cast(object);
		if (player && player.IsAlive())
		{
			return true;
		}
		return false;
    }

	override void OnEnter(Object obj)
    {
		PlayerBase player = PlayerBase.Cast(obj);
		if (GetGame().IsServer() && player && GetAssignedNPC())
		{
			GetAssignedNPC().AddSniperTriggerTarget(player);
		}
	}

	override void OnLeave(Object obj)
	{
		PlayerBase player = PlayerBase.Cast(obj);
		if (GetGame().IsServer() && player && GetAssignedNPC())
		{
			GetAssignedNPC().RemoveSniperTriggerTarget(player);
		}
	}
};
/*
modded class Apple
{
	void Apple()
	{
		SetEventMask(EntityEvent.POSTFRAME);

	}

	override void EOnPostFrame(IEntity other, int extra)
    {
        float scale = 4;
        vector transform[4];
        GetTransform(transform);

        transform[0] = transform[0] * scale;
        transform[1] = transform[1] * scale;
        transform[2] = transform[2] * scale;

        SetTransform(transform);
        Update();
    }

}
*/

