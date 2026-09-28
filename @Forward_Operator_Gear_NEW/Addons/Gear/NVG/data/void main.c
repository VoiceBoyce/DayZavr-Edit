void main()
{
	CreateHive();
	GetHive().InitOffline();
	int year, month, day, hour, minute;
	GetGame().GetWorld().GetDate( year, month, day, hour, minute );

	//GetCEApi().ExportProxyData("0 0 0", 100000); //Loot

	//Change here the dates for whatever months you desire
    if ( month < 12 )
    {
    	year = 2031;
        month = 6;
        day = 1;
		
		GetGame().GetWorld().SetDate( year, month, day, hour, minute );
	}
} 

class CustomMission: MissionServer
{
	void SetRandomHealth(EntityAI itemEnt)
	{
		if ( itemEnt )
		{
			float rndHlt = Math.RandomFloat( 0.45, 0.65 );
			itemEnt.SetHealth01( "", "", rndHlt );
		}
	}

	/*
	override PlayerBase CreateCharacter(PlayerIdentity identity, vector pos, ParamsReadContext ctx, string characterName)
	{
		Entity playerEnt;
		playerEnt = GetGame().CreatePlayer( identity, characterName, pos, 0, "NONE" );
		Class.CastTo( m_player, playerEnt );

		GetGame().SelectPlayer( identity, m_player );

		return m_player;
	}
	*/

	override void OnInit()
	{
		super.OnInit();

		// this piece of code is recommended otherwise event system is switched on automatically and runs from default values
		// comment this whole bloctsck if NOT using Namalsk Survival

		if ( m_EventManagerServer )
		{
			// enable/disable event system, min time between events, max time between events, max number of events at the same time
			m_EventManagerServer.OnInitServer( false, 550, 1000, 0 );
			// registering events and their probability
			m_EventManagerServer.RegisterEvent( Aurora, 0 );
			m_EventManagerServer.RegisterEvent( Blizzard, 0 );
			m_EventManagerServer.RegisterEvent( ExtremeCold, 0 );
			m_EventManagerServer.RegisterEvent( SnowfallE, 0 );
			m_EventManagerServer.RegisterEvent( EVRStorm, 0 );
			m_EventManagerServer.RegisterEvent( HeavyFog, 0 );
		}
	}

	override PlayerBase CreateCharacter(PlayerIdentity identity, vector pos, ParamsReadContext ctx, string characterName)
	{
		string newChar = characterName;

		if (newChar == "SurvivorF_Oakley" && identity.GetPlainId() != "76561198067728169")
		{
			newChar = GetGame().CreateRandomPlayer();
			while (newChar == "SurvivorF_Oakley")
			{
				newChar = GetGame().CreateRandomPlayer();
			}
		}

		Entity playerEnt;
		playerEnt = GetGame().CreatePlayer( identity, newChar, pos, 0, "NONE" );
		Class.CastTo( m_player, playerEnt );

		GetGame().SelectPlayer( identity, m_player );

		return m_player;
	}

	override void StartingEquipSetup(PlayerBase player, bool clothesChosen)
	{
		EntityAI itemClothing;
		EntityAI itemEnt;
		ItemBase itemBs;
		Magazine itemMag;
		EntityAI gasMask;
		EntityAI itemBag;
		
		float rand;
		
		itemClothing = player.FindAttachmentBySlotName( "Body" );
		if ( itemClothing )
		{
			SetRandomHealth( itemClothing );
			
			itemEnt = itemClothing.GetInventory().CreateInInventory( "BandageDressing" );
			player.SetQuickBarEntityShortcut(itemEnt, 10);
			
			itemEnt = player.GetInventory().CreateInInventory("Chemlight_White");
			player.SetQuickBarEntityShortcut(itemEnt, 2);
			
			itemEnt = player.GetInventory().CreateInInventory("HuntingKnife");
			player.SetQuickBarEntityShortcut(itemEnt, 1);
			
			itemEnt = player.GetInventory().CreateInInventory("TCL_Highpoint_C9_Black");
			player.SetQuickBarEntityShortcut(itemEnt, 0);
			
			itemEnt = player.GetInventory().CreateInInventory("TCL_Mag_Highpoint_8Rnd");
			if ( Class.CastTo(itemBs, itemEnt ) )
				itemBs.SetQuantity(15);
			
			itemEnt = player.GetInventory().CreateInInventory("TCL_Mag_Highpoint_8Rnd");
			if ( Class.CastTo(itemBs, itemEnt ) )
				itemBs.SetQuantity(15);
			
			itemEnt = player.GetInventory().CreateInInventory("TCL_Mag_Highpoint_8Rnd");
			if ( Class.CastTo(itemBs, itemEnt ) )
				itemBs.SetQuantity(15);
			
			itemEnt = player.GetInventory().CreateInInventory("Paper");
			if ( Class.CastTo(itemBs, itemEnt ) )
				itemBs.SetQuantity(4);
									
			itemBag = player.GetInventory().CreateInInventory("CFRP_Backpack3");
		}

		itemClothing = player.FindAttachmentBySlotName( "Legs" );
		if ( itemClothing )
		{
			SetRandomHealth( itemClothing );
		}
			
		itemClothing = player.FindAttachmentBySlotName( "Feet" );
		if ( itemClothing )
			SetRandomHealth( itemClothing );
		
			player.GetInventory().CreateInInventory("ChernarusMap");    // added items
			player.GetInventory().CreateInInventory("Battery9V");    // added items
			player.GetInventory().CreateInInventory("Battery9V");
			player.GetInventory().CreateInInventory("Battery9V");
			player.GetInventory().CreateInInventory("PersonalRadio");    // added items
			player.GetInventory().CreateInInventory("Pen_Black");    // added items	
			player.GetInventory().CreateInInventory("Compass");    // added items
			player.GetInventory().CreateInInventory("PeachesCan");    // added items
			player.GetInventory().CreateInInventory("SodaCan_Cola");    // added items
			player.GetInventory().CreateInInventory("SodaCan_Fronta");    // added items
			
			
			player.GetStatWater().Add(4400);
			player.GetStatEnergy().Add(4400);
	}
};

Mission CreateCustomMission(string path)
{
	return new CustomMission();
}