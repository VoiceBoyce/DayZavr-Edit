modded class ModItemRegisterCallbacks
{
    override void RegisterOneHanded(DayZPlayerType pType, DayzPlayerItemBehaviorCfg pBehavior)
    {
        super.RegisterOneHanded(pType, pBehavior);
		
        //pType.AddItemInHandsProfileIK("MSFZ_Crystal_Gold",     			"dz/anims/workspaces/player/player_main/player_main_1h.asi",                    pBehavior, "dz/anims/anm/player/ik/gear/book.anm");
        //pType.AddItemInHandsProfileIK("MSFZ_Crystal_Brass",             "dz/anims/workspaces/player/player_main/player_main_1h.asi", 			        pBehavior, "dz/anims/anm/player/ik/gear/book.anm");
        //pType.AddItemInHandsProfileIK("MSFZ_Crystal_Cooper",            "dz/anims/workspaces/player/player_main/player_main_1h.asi", 			        pBehavior, "dz/anims/anm/player/ik/gear/book.anm");
        //pType.AddItemInHandsProfileIK("MSFZ_Crystal_Iron",              "dz/anims/workspaces/player/player_main/player_main_1h.asi", 			        pBehavior, "dz/anims/anm/player/ik/gear/book.anm");
        //pType.AddItemInHandsProfileIK("MSFZ_Crystal_Silver",            "dz/anims/workspaces/player/player_main/player_main_1h.asi", 			        pBehavior, "dz/anims/anm/player/ik/gear/book.anm");
        //pType.AddItemInHandsProfileIK("MSFZ_Crystal_Blue",         		"dz/anims/workspaces/player/player_main/player_main_1h.asi", 			        pBehavior, "dz/anims/anm/player/ik/gear/book.anm");
        //pType.AddItemInHandsProfileIK("MSFZ_Crystal_Amethyst", 			"dz/anims/workspaces/player/player_main/player_main_1h.asi", 			        pBehavior, "dz/anims/anm/player/ik/gear/book.anm");
        //pType.AddItemInHandsProfileIK("MSFZ_Crystal_Mithril", 			"dz/anims/workspaces/player/player_main/player_main_1h.asi", 			        pBehavior, "dz/anims/anm/player/ik/gear/book.anm");
        //pType.AddItemInHandsProfileIK("MSFZ_Crystal_Obsidian", 			"dz/anims/workspaces/player/player_main/player_main_1h.asi", 			        pBehavior, "dz/anims/anm/player/ik/gear/book.anm");
        //pType.AddItemInHandsProfileIK("MSFZ_Crystal_Pink", 				"dz/anims/workspaces/player/player_main/player_main_1h.asi", 			        pBehavior, "dz/anims/anm/player/ik/gear/book.anm");
        //pType.AddItemInHandsProfileIK("MSFZ_Crystal_Stone", 			"dz/anims/workspaces/player/player_main/player_main_1h.asi", 			        pBehavior, "dz/anims/anm/player/ik/gear/book.anm");
        //pType.AddItemInHandsProfileIK("MSFZ_Crystal_Damascus", 			"dz/anims/workspaces/player/player_main/player_main_1h.asi", 			        pBehavior, "dz/anims/anm/player/ik/gear/book.anm");
        pType.AddItemInHandsProfileIK("MSFZ_Crystal_Pyrite", 			"dz/anims/workspaces/player/player_main/player_main_1h.asi", 			        pBehavior, "dz/anims/anm/player/ik/gear/book.anm");
    };    
};
