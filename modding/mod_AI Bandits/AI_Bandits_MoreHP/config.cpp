class CfgPatches
{
	class AI_Bandits_MoreHP
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters",
			"AI_Bandits"
		};
	};
};
class CfgMods
{
	class AI_Bandits_MoreHP
	{	
		
		dir = "AI_Bandits_MoreHP";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "AI_Bandits_MoreHP";
		author = "DayZavr";
		authorID = "";
		version = 1;
		extra = 0;		
		type = "mod";
	};
};

class CfgVehicles
{
	class BanditAI_Adam;
	class BanditAI_Boris;
	class BanditAI_Cyril;
	class BanditAI_Denis;
	
	
	class BanditAI_Adam_300HP: BanditAI_Adam 
	{
        class DamageSystem 
		{
            class GlobalHealth 
			{
                class Health 
				{ 
				hitpoints = 300; 
				};
            };
            class DamageZones 
			{
                class Head		{ hitpoints = 90; transferToGlobalCoef = 1; };
                class Torso		{ hitpoints = 300; transferToGlobalCoef = 0; };
                class LeftArm	{ hitpoints = 300; transferToGlobalCoef = 0; };
                class RightArm	{ hitpoints = 300; transferToGlobalCoef = 0; };
                class LeftLeg	{ hitpoints = 300; transferToGlobalCoef = 0; };
                class RightLeg	{ hitpoints = 300; transferToGlobalCoef = 0; };
            };
        };
    };
	class BanditAI_Boris_300HP: BanditAI_Boris 
	{
        class DamageSystem 
		{
            class GlobalHealth 
			{
                class Health 
				{ 
				hitpoints = 300; 
				};
            };
            class DamageZones 
			{
                class Head		{ hitpoints = 90; transferToGlobalCoef = 1; };
                class Torso		{ hitpoints = 300; transferToGlobalCoef = 0; };
                class LeftArm	{ hitpoints = 300; transferToGlobalCoef = 0; };
                class RightArm	{ hitpoints = 300; transferToGlobalCoef = 0; };
                class LeftLeg	{ hitpoints = 300; transferToGlobalCoef = 0; };
                class RightLeg	{ hitpoints = 300; transferToGlobalCoef = 0; };
            };
        };
    };
	class BanditAI_Cyril_300HP: BanditAI_Cyril 
	{
        class DamageSystem 
		{
            class GlobalHealth 
			{
                class Health 
				{ 
				hitpoints = 300; 
				};
            };
            class DamageZones 
			{
                class Head		{ hitpoints = 90; transferToGlobalCoef = 1; };
                class Torso		{ hitpoints = 300; transferToGlobalCoef = 0; };
                class LeftArm	{ hitpoints = 300; transferToGlobalCoef = 0; };
                class RightArm	{ hitpoints = 300; transferToGlobalCoef = 0; };
                class LeftLeg	{ hitpoints = 300; transferToGlobalCoef = 0; };
                class RightLeg	{ hitpoints = 300; transferToGlobalCoef = 0; };
            };
        };
    };
	class BanditAI_Denis_300HP: BanditAI_Denis 
	{
        class DamageSystem 
		{
            class GlobalHealth 
			{
                class Health 
				{ 
				hitpoints = 300; 
				};
            };
            class DamageZones 
			{
                class Head		{ hitpoints = 90; transferToGlobalCoef = 1; };
                class Torso		{ hitpoints = 300; transferToGlobalCoef = 0; };
                class LeftArm	{ hitpoints = 300; transferToGlobalCoef = 0; };
                class RightArm	{ hitpoints = 300; transferToGlobalCoef = 0; };
                class LeftLeg	{ hitpoints = 300; transferToGlobalCoef = 0; };
                class RightLeg	{ hitpoints = 300; transferToGlobalCoef = 0; };
            };
        };
    };
	
	
	class BanditAI_Adam_500HP: BanditAI_Adam 
	{
        class DamageSystem 
		{
            class GlobalHealth 
			{
                class Health 
				{ 
				hitpoints = 500; 
				};
            };
            class DamageZones 
			{
                class Head		{ hitpoints = 150; transferToGlobalCoef = 1; };
                class Torso		{ hitpoints = 500; transferToGlobalCoef = 0; };
                class LeftArm	{ hitpoints = 500; transferToGlobalCoef = 0; };
                class RightArm	{ hitpoints = 500; transferToGlobalCoef = 0; };
                class LeftLeg	{ hitpoints = 500; transferToGlobalCoef = 0; };
                class RightLeg	{ hitpoints = 500; transferToGlobalCoef = 0; };
            };
        };
    };
	class BanditAI_Boris_500HP: BanditAI_Boris 
	{
        class DamageSystem 
		{
            class GlobalHealth 
			{
                class Health 
				{ 
				hitpoints = 500; 
				};
            };
            class DamageZones 
			{
                class Head		{ hitpoints = 150; transferToGlobalCoef = 1; };
                class Torso		{ hitpoints = 500; transferToGlobalCoef = 0; };
                class LeftArm	{ hitpoints = 500; transferToGlobalCoef = 0; };
                class RightArm	{ hitpoints = 500; transferToGlobalCoef = 0; };
                class LeftLeg	{ hitpoints = 500; transferToGlobalCoef = 0; };
                class RightLeg	{ hitpoints = 500; transferToGlobalCoef = 0; };
            };
        };
    };
	class BanditAI_Cyril_500HP: BanditAI_Cyril 
	{
        class DamageSystem 
		{
            class GlobalHealth 
			{
                class Health 
				{ 
				hitpoints = 500; 
				};
            };
            class DamageZones 
			{
                class Head		{ hitpoints = 150; transferToGlobalCoef = 1; };
                class Torso		{ hitpoints = 500; transferToGlobalCoef = 0; };
                class LeftArm	{ hitpoints = 500; transferToGlobalCoef = 0; };
                class RightArm	{ hitpoints = 500; transferToGlobalCoef = 0; };
                class LeftLeg	{ hitpoints = 500; transferToGlobalCoef = 0; };
                class RightLeg	{ hitpoints = 500; transferToGlobalCoef = 0; };
            };
        };
    };
	class BanditAI_Denis_500HP: BanditAI_Denis 
	{
        class DamageSystem 
		{
            class GlobalHealth 
			{
                class Health 
				{ 
				hitpoints = 500; 
				};
            };
            class DamageZones 
			{
                class Head		{ hitpoints = 150; transferToGlobalCoef = 1; };
                class Torso		{ hitpoints = 500; transferToGlobalCoef = 0; };
                class LeftArm	{ hitpoints = 500; transferToGlobalCoef = 0; };
                class RightArm	{ hitpoints = 500; transferToGlobalCoef = 0; };
                class LeftLeg	{ hitpoints = 500; transferToGlobalCoef = 0; };
                class RightLeg	{ hitpoints = 500; transferToGlobalCoef = 0; };
            };
        };
    };
	
	
	class BanditAI_Adam_700HP: BanditAI_Adam 
	{
        class DamageSystem 
		{
            class GlobalHealth 
			{
                class Health 
				{ 
				hitpoints = 700; 
				};
            };
            class DamageZones 
			{
                class Head		{ hitpoints = 210; transferToGlobalCoef = 1; };
                class Torso		{ hitpoints = 700; transferToGlobalCoef = 0; };
                class LeftArm	{ hitpoints = 700; transferToGlobalCoef = 0; };
                class RightArm	{ hitpoints = 700; transferToGlobalCoef = 0; };
                class LeftLeg	{ hitpoints = 700; transferToGlobalCoef = 0; };
                class RightLeg	{ hitpoints = 700; transferToGlobalCoef = 0; };
            };
        };
    };
	class BanditAI_Boris_700HP: BanditAI_Boris 
	{
        class DamageSystem 
		{
            class GlobalHealth 
			{
                class Health 
				{ 
				hitpoints = 700; 
				};
            };
            class DamageZones 
			{
                class Head		{ hitpoints = 210; transferToGlobalCoef = 1; };
                class Torso		{ hitpoints = 700; transferToGlobalCoef = 0; };
                class LeftArm	{ hitpoints = 700; transferToGlobalCoef = 0; };
                class RightArm	{ hitpoints = 700; transferToGlobalCoef = 0; };
                class LeftLeg	{ hitpoints = 700; transferToGlobalCoef = 0; };
                class RightLeg	{ hitpoints = 700; transferToGlobalCoef = 0; };
            };
        };
    };
	class BanditAI_Cyril_700HP: BanditAI_Cyril 
	{
        class DamageSystem 
		{
            class GlobalHealth 
			{
                class Health 
				{ 
				hitpoints = 700; 
				};
            };
            class DamageZones 
			{
                class Head		{ hitpoints = 210; transferToGlobalCoef = 1; };
                class Torso		{ hitpoints = 700; transferToGlobalCoef = 0; };
                class LeftArm	{ hitpoints = 700; transferToGlobalCoef = 0; };
                class RightArm	{ hitpoints = 700; transferToGlobalCoef = 0; };
                class LeftLeg	{ hitpoints = 700; transferToGlobalCoef = 0; };
                class RightLeg	{ hitpoints = 700; transferToGlobalCoef = 0; };
            };
        };
    };
	class BanditAI_Denis_700HP: BanditAI_Denis 
	{
        class DamageSystem 
		{
            class GlobalHealth 
			{
                class Health 
				{ 
				hitpoints = 700; 
				};
            };
            class DamageZones 
			{
                class Head		{ hitpoints = 210; transferToGlobalCoef = 1; };
                class Torso		{ hitpoints = 700; transferToGlobalCoef = 0; };
                class LeftArm	{ hitpoints = 700; transferToGlobalCoef = 0; };
                class RightArm	{ hitpoints = 700; transferToGlobalCoef = 0; };
                class LeftLeg	{ hitpoints = 700; transferToGlobalCoef = 0; };
                class RightLeg	{ hitpoints = 700; transferToGlobalCoef = 0; };
            };
        };
    };
	
	
	class BanditAI_Adam_1000HP: BanditAI_Adam 
	{
        class DamageSystem 
		{
            class GlobalHealth 
			{
                class Health 
				{ 
				hitpoints = 1000; 
				};
            };
            class DamageZones 
			{
                class Head		{ hitpoints = 300; transferToGlobalCoef = 1; };
                class Torso		{ hitpoints = 1000; transferToGlobalCoef = 0; };
                class LeftArm	{ hitpoints = 1000; transferToGlobalCoef = 0; };
                class RightArm	{ hitpoints = 1000; transferToGlobalCoef = 0; };
                class LeftLeg	{ hitpoints = 1000; transferToGlobalCoef = 0; };
                class RightLeg	{ hitpoints = 1000; transferToGlobalCoef = 0; };
            };
        };
    };
	class BanditAI_Boris_1000HP: BanditAI_Boris 
	{
        class DamageSystem 
		{
            class GlobalHealth 
			{
                class Health 
				{ 
				hitpoints = 1000; 
				};
            };
            class DamageZones 
			{
                class Head		{ hitpoints = 300; transferToGlobalCoef = 1; };
                class Torso		{ hitpoints = 1000; transferToGlobalCoef = 0; };
                class LeftArm	{ hitpoints = 1000; transferToGlobalCoef = 0; };
                class RightArm	{ hitpoints = 1000; transferToGlobalCoef = 0; };
                class LeftLeg	{ hitpoints = 1000; transferToGlobalCoef = 0; };
                class RightLeg	{ hitpoints = 1000; transferToGlobalCoef = 0; };
            };
        };
    };
	class BanditAI_Cyril_1000HP: BanditAI_Cyril 
	{
        class DamageSystem 
		{
            class GlobalHealth 
			{
                class Health 
				{ 
				hitpoints = 1000; 
				};
            };
            class DamageZones 
			{
                class Head		{ hitpoints = 300; transferToGlobalCoef = 1; };
                class Torso		{ hitpoints = 1000; transferToGlobalCoef = 0; };
                class LeftArm	{ hitpoints = 1000; transferToGlobalCoef = 0; };
                class RightArm	{ hitpoints = 1000; transferToGlobalCoef = 0; };
                class LeftLeg	{ hitpoints = 1000; transferToGlobalCoef = 0; };
                class RightLeg	{ hitpoints = 1000; transferToGlobalCoef = 0; };
            };
        };
    };
	class BanditAI_Denis_1000HP: BanditAI_Denis 
	{
        class DamageSystem 
		{
            class GlobalHealth 
			{
                class Health 
				{ 
				hitpoints = 1000; 
				};
            };
            class DamageZones 
			{
                class Head		{ hitpoints = 300; transferToGlobalCoef = 1; };
                class Torso		{ hitpoints = 1000; transferToGlobalCoef = 0; };
                class LeftArm	{ hitpoints = 1000; transferToGlobalCoef = 0; };
                class RightArm	{ hitpoints = 1000; transferToGlobalCoef = 0; };
                class LeftLeg	{ hitpoints = 1000; transferToGlobalCoef = 0; };
                class RightLeg	{ hitpoints = 1000; transferToGlobalCoef = 0; };
            };
        };
    };
	
	
	class BanditAI_Adam_1500HP: BanditAI_Adam 
	{
        class DamageSystem 
		{
            class GlobalHealth 
			{
                class Health 
				{ 
				hitpoints = 1500; 
				};
            };
            class DamageZones 
			{
                class Head		{ hitpoints = 450; transferToGlobalCoef = 1; };
                class Torso		{ hitpoints = 1500; transferToGlobalCoef = 0; };
                class LeftArm	{ hitpoints = 1500; transferToGlobalCoef = 0; };
                class RightArm	{ hitpoints = 1500; transferToGlobalCoef = 0; };
                class LeftLeg	{ hitpoints = 1500; transferToGlobalCoef = 0; };
                class RightLeg	{ hitpoints = 1500; transferToGlobalCoef = 0; };
            };
        };
    };
	class BanditAI_Boris_1500HP: BanditAI_Boris 
	{
        class DamageSystem 
		{
            class GlobalHealth 
			{
                class Health 
				{ 
				hitpoints = 1500; 
				};
            };
            class DamageZones 
			{
                class Head		{ hitpoints = 450; transferToGlobalCoef = 1; };
                class Torso		{ hitpoints = 1500; transferToGlobalCoef = 0; };
                class LeftArm	{ hitpoints = 1500; transferToGlobalCoef = 0; };
                class RightArm	{ hitpoints = 1500; transferToGlobalCoef = 0; };
                class LeftLeg	{ hitpoints = 1500; transferToGlobalCoef = 0; };
                class RightLeg	{ hitpoints = 1500; transferToGlobalCoef = 0; };
            };
        };
    };
	class BanditAI_Cyril_1500HP: BanditAI_Cyril 
	{
        class DamageSystem 
		{
            class GlobalHealth 
			{
                class Health 
				{ 
				hitpoints = 1500; 
				};
            };
            class DamageZones 
			{
                class Head		{ hitpoints = 450; transferToGlobalCoef = 1; };
                class Torso		{ hitpoints = 1500; transferToGlobalCoef = 0; };
                class LeftArm	{ hitpoints = 1500; transferToGlobalCoef = 0; };
                class RightArm	{ hitpoints = 1500; transferToGlobalCoef = 0; };
                class LeftLeg	{ hitpoints = 1500; transferToGlobalCoef = 0; };
                class RightLeg	{ hitpoints = 1500; transferToGlobalCoef = 0; };
            };
        };
    };
	class BanditAI_Denis_1500HP: BanditAI_Denis 
	{
        class DamageSystem 
		{
            class GlobalHealth 
			{
                class Health 
				{ 
				hitpoints = 1500; 
				};
            };
            class DamageZones 
			{
                class Head		{ hitpoints = 450; transferToGlobalCoef = 1; };
                class Torso		{ hitpoints = 1500; transferToGlobalCoef = 0; };
                class LeftArm	{ hitpoints = 1500; transferToGlobalCoef = 0; };
                class RightArm	{ hitpoints = 1500; transferToGlobalCoef = 0; };
                class LeftLeg	{ hitpoints = 1500; transferToGlobalCoef = 0; };
                class RightLeg	{ hitpoints = 1500; transferToGlobalCoef = 0; };
            };
        };
    };
	
	
	
	class BanditAI_Adam_3000HP: BanditAI_Adam 
	{
        class DamageSystem 
		{
            class GlobalHealth 
			{
                class Health 
				{ 
				hitpoints = 3000; 
				};
            };
            class DamageZones 
			{
                class Head		{ hitpoints = 900; transferToGlobalCoef = 1; };
                class Torso		{ hitpoints = 3000; transferToGlobalCoef = 0; };
                class LeftArm	{ hitpoints = 3000; transferToGlobalCoef = 0; };
                class RightArm	{ hitpoints = 3000; transferToGlobalCoef = 0; };
                class LeftLeg	{ hitpoints = 3000; transferToGlobalCoef = 0; };
                class RightLeg	{ hitpoints = 3000; transferToGlobalCoef = 0; };
            };
        };
    };
	class BanditAI_Boris_3000HP: BanditAI_Boris 
	{
        class DamageSystem 
		{
            class GlobalHealth 
			{
                class Health 
				{ 
				hitpoints = 3000; 
				};
            };
            class DamageZones 
			{
                class Head		{ hitpoints = 900; transferToGlobalCoef = 1; };
                class Torso		{ hitpoints = 3000; transferToGlobalCoef = 0; };
                class LeftArm	{ hitpoints = 3000; transferToGlobalCoef = 0; };
                class RightArm	{ hitpoints = 3000; transferToGlobalCoef = 0; };
                class LeftLeg	{ hitpoints = 3000; transferToGlobalCoef = 0; };
                class RightLeg	{ hitpoints = 3000; transferToGlobalCoef = 0; };
            };
        };
    };
	class BanditAI_Cyril_3000HP: BanditAI_Cyril 
	{
        class DamageSystem 
		{
            class GlobalHealth 
			{
                class Health 
				{ 
				hitpoints = 3000; 
				};
            };
            class DamageZones 
			{
                class Head		{ hitpoints = 900; transferToGlobalCoef = 1; };
                class Torso		{ hitpoints = 3000; transferToGlobalCoef = 0; };
                class LeftArm	{ hitpoints = 3000; transferToGlobalCoef = 0; };
                class RightArm	{ hitpoints = 3000; transferToGlobalCoef = 0; };
                class LeftLeg	{ hitpoints = 3000; transferToGlobalCoef = 0; };
                class RightLeg	{ hitpoints = 3000; transferToGlobalCoef = 0; };
            };
        };
    };
	class BanditAI_Denis_3000HP: BanditAI_Denis 
	{
        class DamageSystem 
		{
            class GlobalHealth 
			{
                class Health 
				{ 
				hitpoints = 3000; 
				};
            };
            class DamageZones 
			{
                class Head		{ hitpoints = 900; transferToGlobalCoef = 1; };
                class Torso		{ hitpoints = 3000; transferToGlobalCoef = 0; };
                class LeftArm	{ hitpoints = 3000; transferToGlobalCoef = 0; };
                class RightArm	{ hitpoints = 3000; transferToGlobalCoef = 0; };
                class LeftLeg	{ hitpoints = 3000; transferToGlobalCoef = 0; };
                class RightLeg	{ hitpoints = 3000; transferToGlobalCoef = 0; };
            };
        };
    };
	
	
	
	class BanditAI_Adam_5000HP: BanditAI_Adam 
	{
        class DamageSystem 
		{
            class GlobalHealth 
			{
                class Health 
				{ 
				hitpoints = 5000; 
				};
            };
            class DamageZones 
			{
                class Head		{ hitpoints = 1500; transferToGlobalCoef = 1; };
                class Torso		{ hitpoints = 5000; transferToGlobalCoef = 0; };
                class LeftArm	{ hitpoints = 5000; transferToGlobalCoef = 0; };
                class RightArm	{ hitpoints = 5000; transferToGlobalCoef = 0; };
                class LeftLeg	{ hitpoints = 5000; transferToGlobalCoef = 0; };
                class RightLeg	{ hitpoints = 5000; transferToGlobalCoef = 0; };
            };
        };
    };
	class BanditAI_Boris_5000HP: BanditAI_Boris 
	{
        class DamageSystem 
		{
            class GlobalHealth 
			{
                class Health 
				{ 
				hitpoints = 5000; 
				};
            };
            class DamageZones 
			{
                class Head		{ hitpoints = 1500; transferToGlobalCoef = 1; };
                class Torso		{ hitpoints = 5000; transferToGlobalCoef = 0; };
                class LeftArm	{ hitpoints = 5000; transferToGlobalCoef = 0; };
                class RightArm	{ hitpoints = 5000; transferToGlobalCoef = 0; };
                class LeftLeg	{ hitpoints = 5000; transferToGlobalCoef = 0; };
                class RightLeg	{ hitpoints = 5000; transferToGlobalCoef = 0; };
            };
        };
    };
	class BanditAI_Cyril_5000HP: BanditAI_Cyril 
	{
        class DamageSystem 
		{
            class GlobalHealth 
			{
                class Health 
				{ 
				hitpoints = 5000; 
				};
            };
            class DamageZones 
			{
                class Head		{ hitpoints = 1500; transferToGlobalCoef = 1; };
                class Torso		{ hitpoints = 5000; transferToGlobalCoef = 0; };
                class LeftArm	{ hitpoints = 5000; transferToGlobalCoef = 0; };
                class RightArm	{ hitpoints = 5000; transferToGlobalCoef = 0; };
                class LeftLeg	{ hitpoints = 5000; transferToGlobalCoef = 0; };
                class RightLeg	{ hitpoints = 5000; transferToGlobalCoef = 0; };
            };
        };
    };
	class BanditAI_Denis_5000HP: BanditAI_Denis 
	{
        class DamageSystem 
		{
            class GlobalHealth 
			{
                class Health 
				{ 
				hitpoints = 5000; 
				};
            };
            class DamageZones 
			{
                class Head		{ hitpoints = 1500; transferToGlobalCoef = 1; };
                class Torso		{ hitpoints = 5000; transferToGlobalCoef = 0; };
                class LeftArm	{ hitpoints = 5000; transferToGlobalCoef = 0; };
                class RightArm	{ hitpoints = 5000; transferToGlobalCoef = 0; };
                class LeftLeg	{ hitpoints = 5000; transferToGlobalCoef = 0; };
                class RightLeg	{ hitpoints = 5000; transferToGlobalCoef = 0; };
            };
        };
    };
};