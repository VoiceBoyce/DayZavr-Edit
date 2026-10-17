modded class ModItemRegisterCallbacks
{
	override void RegisterOneHanded( DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior )
	{
		super.RegisterOneHanded( pType, pBehavior );
		pType.AddItemInHandsProfileIK("DRL_Craft_Blueprint", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/paper.anm");
		pType.AddItemInHandsProfileIK("DRL_Craft_Research_Book", "dz/anims/workspaces/player/player_main/player_main_1h.asi", pBehavior, "dz/anims/anm/player/ik/gear/book.anm");
	}

	override void RegisterTwoHanded( DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior )
	{
		super.RegisterTwoHanded( pType, pBehavior );
		pType.AddItemInHandsProfileIK("DRL_Craft_Grinder","dz/anims/workspaces/player/player_main/props/player_main_2h_pot.asi",pBehavior,"dz/anims/anm/player/ik/two_handed/CookingPot.anm");
		
		pType.AddItemInHandsProfileIK("DRL_Craft_Vise","dz/anims/workspaces/player/player_main/props/player_main_2h_pot.asi",pBehavior,"dz/anims/anm/player/ik/two_handed/CookingPot.anm");
		
		pType.AddItemInHandsProfileIK("DRL_Craft_Anvil","dz/anims/workspaces/player/player_main/props/player_main_2h_pot.asi",pBehavior,"dz/anims/anm/player/ik/two_handed/CookingPot.anm");
		
		pType.AddItemInHandsProfileIK("DRL_Craft_Drill","dz/anims/workspaces/player/player_main/props/player_main_2h_pot.asi",pBehavior,"dz/anims/anm/player/ik/two_handed/CookingPot.anm");
		
		pType.AddItemInHandsProfileIK("DRL_Craft_Micro","dz/anims/workspaces/player/player_main/props/player_main_2h_pot.asi",pBehavior,"dz/anims/anm/player/ik/two_handed/CookingPot.anm");
		
		pType.AddItemInHandsProfileIK("DRL_Craft_lab_bunsen","dz/anims/workspaces/player/player_main/props/player_main_2h_pot.asi",pBehavior,"dz/anims/anm/player/ik/two_handed/CookingPot.anm");
		
		pType.AddItemInHandsProfileIK("DRL_Craft_Circular","dz/anims/workspaces/player/player_main/props/player_main_2h_pot.asi",pBehavior,"dz/anims/anm/player/ik/two_handed/CookingPot.anm");
		
		pType.AddItemInHandsProfileIK("DRL_Craft_Microscope","dz/anims/workspaces/player/player_main/props/player_main_2h_pot.asi",pBehavior,"dz/anims/anm/player/ik/two_handed/CookingPot.anm");
		
		pType.AddItemInHandsProfileIK("DRL_Craft_NoteBook","dz/anims/workspaces/player/player_main/props/player_main_2h_pot.asi",pBehavior,"dz/anims/anm/player/ik/two_handed/CookingPot.anm");
		
		pType.AddItemInHandsProfileIK("DRL_Craft_Microwave","dz/anims/workspaces/player/player_main/props/player_main_2h_pot.asi",pBehavior,"dz/anims/anm/player/ik/two_handed/CookingPot.anm");
		
		pType.AddItemInHandsProfileIK("DRL_Craft_Sewing","dz/anims/workspaces/player/player_main/props/player_main_2h_pot.asi",pBehavior,"dz/anims/anm/player/ik/two_handed/CookingPot.anm");
		
		pType.AddItemInHandsProfileIK("DRL_Craft_Welding_Machine","dz/anims/workspaces/player/player_main/props/player_main_2h_pot.asi",pBehavior,"dz/anims/anm/player/ik/two_handed/CookingPot.anm");
		
		pType.AddItemInHandsProfileIK("DRL_Craft_Box","dz/anims/workspaces/player/player_main/props/player_main_2h_pot.asi",pBehavior,"dz/anims/anm/player/ik/two_handed/CookingPot.anm");
		
		pType.AddItemInHandsProfileIK("DRL_Craft_Sewing_Table","dz/anims/workspaces/player/player_main/props/player_main_2h_pot.asi",pBehavior,"dz/anims/anm/player/ik/two_handed/CookingPot.anm");
		
		pType.AddItemInHandsProfileIK("DRL_Craft_Table","dz/anims/workspaces/player/player_main/props/player_main_2h_pot.asi",pBehavior,"dz/anims/anm/player/ik/two_handed/CookingPot.anm");
		
		pType.AddItemInHandsProfileIK("DRL_Craft_Car_Jack","dz/anims/workspaces/player/player_main/props/player_main_2h_pot.asi",pBehavior,"dz/anims/anm/player/ik/two_handed/CookingPot.anm");
	};
};