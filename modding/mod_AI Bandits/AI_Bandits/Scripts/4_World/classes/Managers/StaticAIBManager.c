
	
		/*
			posrot = config.NPCLocations.Get(i).triggerpos;
			loc = new TStringArray;
			posrot.Split( "|", loc );
			pos = loc.Get(0);
			rot = loc.Get(1);
			if (!rot)
			{
				pos = config.NPCLocations.Get(i).triggerpos;
				rot = "0 0 0";
			}
			AIBLogger.Log("Created sound trigger " + config.NPCLocations.Get(i).name + " at " + config.NPCLocations.Get(i).triggerpos);
			//
			//trigger = NPCTalkTrigger.Cast(GetGame().CreateObject("NPCTalkTrigger", pos.ToVector()));
			//trigger.SetOrientation(rot.ToVector());
			mins = config.NPCLocations.Get(i).triggermins.ToVector();
			maxs = config.NPCLocations.Get(i).triggermaxs.ToVector();
			radius = config.NPCLocations.Get(i).triggerradius;
			if (radius == 0)
			{
				trigger.SetExtents(mins, maxs);
			} else {
				trigger.SetCollisionCylinder(radius, 1.8);
			} 
			trigger.SetTriggerName(config.NPCLocations.Get(i).name);
			trigger.SetTriggerNPCid(config.NPCLocations.Get(i).npcid);
			trigger.SetTriggerNotification(config.NPCLocations.Get(i).notification);
			trigger.SetTriggerNotificationTime(config.NPCLocations.Get(i).notificationtime);
			trigger.SetSounds(config.NPCLocations.Get(i).sounds);
			trigger.SetEmotes(config.NPCLocations.Get(i).emotes);
			trigger.SetLastTriggerTime(-1 * config.NPCLocations.Get(i).triggercooldown);
			trigger.SetTriggerCooldown(config.NPCLocations.Get(i).triggercooldown);
			NPCTriggers.Insert(trigger);
			*/
class StaticAIBManager
{

	protected static ref StaticAIBManager Instance;
	protected ref array<AIBTalkTrigger> NPCTriggers = new array<AIBTalkTrigger>;
	
	static StaticAIBManager GetInstance()
	{
		if (!Instance)
        {
            Instance = new StaticAIBManager();
        }
		return Instance;
	}
	
	static void ClearInstance()
	{
		Instance = null;
	}
	
	void StaticAIBManager()
    {
		AIBLogger.Log( "AI Bandits started!" );
		GetRPCManager().AddRPC( "AIBandits", "StopTalk", this, SingleplayerExecutionType.Client );
		StaticAIBConfig config = GetDayZGame().GetStaticAIBConfig();
		AIBTalkTrigger trigger;
		vector mins, maxs;
		float radius;
		string posrot, pos, rot;
		TStringArray loc;
		for ( int i=0; i < config.NPCLocations.Count(); i++)
		{
			
			posrot = config.NPCLocations.Get(i).triggerpos;
			loc = new TStringArray;
			posrot.Split( "|", loc );
			pos = loc.Get(0);
			rot = loc.Get(1);
			if (!rot)
			{
				pos = config.NPCLocations.Get(i).triggerpos;
				rot = "0 0 0";
			}
			AIBLogger.Log("Created sound trigger " + config.NPCLocations.Get(i).name + " at " + config.NPCLocations.Get(i).triggerpos);
			//
			trigger = AIBTalkTrigger.Cast(GetGame().CreateObject("AIBTalkTrigger", pos.ToVector()));
			trigger.SetOrientation(rot.ToVector());
			mins = config.NPCLocations.Get(i).triggermins.ToVector();
			maxs = config.NPCLocations.Get(i).triggermaxs.ToVector();
			radius = config.NPCLocations.Get(i).triggerradius;
			if (radius == 0)
			{
				trigger.SetExtents(mins, maxs);
			} else {
				trigger.SetCollisionCylinder(radius, 1.8);
			} 
			trigger.SetTriggerName(config.NPCLocations.Get(i).name);
			trigger.SetTriggerNPCid(config.NPCLocations.Get(i).npcid);
			trigger.SetSounds(config.NPCLocations.Get(i).sounds);
			trigger.SetEmotes(config.NPCLocations.Get(i).emotes);
			trigger.SetLastTriggerTime(-1 * config.NPCLocations.Get(i).triggercooldown);
			trigger.SetTriggerCooldown(config.NPCLocations.Get(i).triggercooldown);
			NPCTriggers.Insert(trigger);
			
		}
	
	}
	
	void StopTalk( CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target )
    {
        Param1< vector > data;
        if ( !ctx.Read( data ) ) return;
        
        if( type == CallType.Server )
        {
			for ( int j=0; j < NPCTriggers.Count(); j++)
			{
				if (NPCTriggers.Get(j).GetPosition() == data.param1)
				{
					if (NPCTriggers.Get(j).GetAssignedNPC())
					{
						AIBLogger.Debug( "" + NPCTriggers.Get(j).GetAssignedNPC() + " stop talking at trigger " + NPCTriggers.Get(j).GetTriggerName() );
						NPCTriggers.Get(j).GetAssignedNPC().SetTalkNPC(false);
						NPCTriggers.Get(j).GetAssignedNPC().ShouldPlayEmoteID(0);
					}
					
				}
			}
		}
	}

}
			

class StaticAIBManagerLate
{

	protected static ref StaticAIBManagerLate Instance;
	
	static StaticAIBManagerLate GetInstance()
	{
		if (!Instance)
        {
            Instance = new StaticAIBManagerLate();
        }
		return Instance;
	}
	
	static void ClearInstance()
	{
		Instance = null;
	}
	
	void StaticAIBManagerLate()
    {
		StaticAIBConfig config = GetDayZGame().GetStaticAIBConfig();
		string posrot, pos, rot, item;
		TStringArray loc;
		for ( int i=0; i < config.NPCLocations.Count(); i++)
		{
			if (config.NPCLocations.Get(i).npcclass && config.NPCLocations.Get(i).npcpos)
			{
				posrot = config.NPCLocations.Get(i).npcpos;
				loc = new TStringArray;
				posrot.Split( "|", loc );
				pos = loc.Get(0);
				rot = loc.Get(1);
				if (!rot)
				{
					pos = config.NPCLocations.Get(i).npcpos;
					rot = "0 0 0";
				}
				
				AIBLogger.Log("Spawned Static NPC " + config.NPCLocations.Get(i).npcclass + " at " + config.NPCLocations.Get(i).npcpos);
				InfectedBanditBase npc = InfectedBanditBase.Cast(GetGame().CreateObject(config.NPCLocations.Get(i).npcclass, pos.ToVector(), false, true));
				npc.SetOrientation(rot.ToVector());
				npc.SetPose(config.NPCLocations.Get(i).npcpose);
				npc.SetNPCID(config.NPCLocations.Get(i).npcid);
				if (config.NPCLocations.Get(i).npcloadout.Count() > 0)
				{
					for ( int j=0; j < config.NPCLocations.Get(i).npcloadout.Count(); j++)
					npc.GetInventory().CreateAttachment(config.NPCLocations.Get(i).npcloadout.Get(j));
				}
				
				item = config.NPCLocations.Get(i).npcitem;
				
				if (item.Contains("Predefined_"))
				{
					for ( int k = 0; k < config.PredefinedWeapons.Count(); k++)
					{
						if (config.PredefinedWeapons.Get(k).name == item)
						{
							string wpclass = config.PredefinedWeapons.Get(k).weapon;
							TStringArray wpattach = config.PredefinedWeapons.Get(k).attachments;
							npc.SetupWeapon(wpclass, wpattach);
						}
					}
				}
				else
				{
					npc.SetupWeapon(item, null);
				}
					
			}
		}
	}
}