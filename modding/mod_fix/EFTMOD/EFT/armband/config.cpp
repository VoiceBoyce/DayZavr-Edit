class CfgPatches
{
	class DZ_Characters_Tops
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Characters"
		};
	};
};

class CfgVehicles
{
	class Inventory_Base;
	class Clothing_Base;
    class Clothing: Clothing_Base{};
	class Armband_ColorBase: Clothing{};
	
	class EFT_armband: Armband_ColorBase
	{
		displayName="#EFT_armband_displayName";
		descriptionShort="#EFT_armband_descriptionShort";
		model="EFT\armband\EFT_armband_g.p3d";
		inventorySlot[]=
		{
			"Armband"
		};
		itemInfo[]=
		{
			"Clothing",
			"Armband"
		};
		rotationFlags=34;
		weight=80;
		itemSize[]={1,1};
		ragQuantity=1;
		varWetMax=1;
		absorbency=0.80000001;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		scope=0;
		hiddenSelections[]=
		{
			"zbytek"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=100;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"EFT\armband\Data\armband.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"EFT\armband\Data\armband.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"EFT\armband\Data\armband_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"EFT\armband\Data\armband_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"EFT\armband\Data\armband_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		class ClothingTypes
		{
			male="EFT\armband\EFT_armband.p3d";
			female="EFT\armband\EFT_armband.p3d";
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem
				{
					soundSet="Shirt_pickup_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="Shirt_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	
	class EFT_armband_white: EFT_armband
    {
        itemSize[]={1,1};
        scope=2;
        hiddenSelectionsTextures[]=
        {
            "EFT\armband\data\armband_white.paa",
            "EFT\armband\data\armband_white.paa",
            "EFT\armband\data\armband_white.paa",
            "EFT\armband\data\armband_white.paa",
            "EFT\armband\data\armband_white.paa",
            "EFT\armband\data\armband_white.paa",
            "EFT\armband\data\armband_white.paa",
            "EFT\armband\data\armband_white.paa",
            "EFT\armband\data\armband_white.paa"
        };
    };
	class EFT_armband_alpha: EFT_armband
    {
        itemSize[]={1,1};
        scope=2;
        hiddenSelectionsTextures[]=
        {
            "EFT\armband\data\armband_alpha.paa",
            "EFT\armband\data\armband_alpha.paa",
            "EFT\armband\data\armband_alpha.paa",
            "EFT\armband\data\armband_alpha.paa",
            "EFT\armband\data\armband_alpha.paa",
            "EFT\armband\data\armband_alpha.paa",
            "EFT\armband\data\armband_alpha.paa",
            "EFT\armband\data\armband_alpha.paa",
            "EFT\armband\data\armband_alpha.paa"
        };
    };
	class EFT_armband_bear: EFT_armband
    {
        itemSize[]={1,1};
        scope=2;
        hiddenSelectionsTextures[]=
        {
            "EFT\armband\data\armband_bear.paa",
            "EFT\armband\data\armband_bear.paa",
            "EFT\armband\data\armband_bear.paa",
            "EFT\armband\data\armband_bear.paa",
            "EFT\armband\data\armband_bear.paa",
            "EFT\armband\data\armband_bear.paa",
            "EFT\armband\data\armband_bear.paa",
            "EFT\armband\data\armband_bear.paa",
            "EFT\armband\data\armband_bear.paa"
        };
    };
	class EFT_armband_blue: EFT_armband
    {
        itemSize[]={1,1};
        scope=2;
        
        hiddenSelectionsTextures[]=
        {
            "EFT\armband\data\armband_blue.paa",
            "EFT\armband\data\armband_blue.paa",
            "EFT\armband\data\armband_blue.paa",
            "EFT\armband\data\armband_blue.paa",
            "EFT\armband\data\armband_blue.paa",
            "EFT\armband\data\armband_blue.paa",
            "EFT\armband\data\armband_blue.paa",
            "EFT\armband\data\armband_blue.paa",
            "EFT\armband\data\armband_blue.paa"
        };
    };
	class EFT_armband_dead: EFT_armband
    {
        itemSize[]={1,1};
        scope=2;
        
        hiddenSelectionsTextures[]=
        {
            "EFT\armband\data\armband_dead.paa",
            "EFT\armband\data\armband_dead.paa",
            "EFT\armband\data\armband_dead.paa",
            "EFT\armband\data\armband_dead.paa",
            "EFT\armband\data\armband_dead.paa",
            "EFT\armband\data\armband_dead.paa",
            "EFT\armband\data\armband_dead.paa",
            "EFT\armband\data\armband_dead.paa",
            "EFT\armband\data\armband_dead.paa"
        };
    };
	class EFT_armband_evasion: EFT_armband
    {
        itemSize[]={1,1};
        scope=2;
        
        hiddenSelectionsTextures[]=
        {
            "EFT\armband\data\armband_evasion.paa",
            "EFT\armband\data\armband_evasion.paa",
            "EFT\armband\data\armband_evasion.paa",
            "EFT\armband\data\armband_evasion.paa",
            "EFT\armband\data\armband_evasion.paa",
            "EFT\armband\data\armband_evasion.paa",
            "EFT\armband\data\armband_evasion.paa",
            "EFT\armband\data\armband_evasion.paa",
            "EFT\armband\data\armband_evasion.paa"
        };
    };
	class EFT_armband_green: EFT_armband
    {
        itemSize[]={1,1};
        scope=2;
        
        hiddenSelectionsTextures[]=
        {
            "EFT\armband\data\armband_green.paa",
            "EFT\armband\data\armband_green.paa",
            "EFT\armband\data\armband_green.paa",
            "EFT\armband\data\armband_green.paa",
            "EFT\armband\data\armband_green.paa",
            "EFT\armband\data\armband_green.paa",
            "EFT\armband\data\armband_green.paa",
            "EFT\armband\data\armband_green.paa",
            "EFT\armband\data\armband_green.paa"
        };
    };
	class EFT_armband_kibaarms: EFT_armband
    {
        itemSize[]={1,1};
        scope=2;
        
        hiddenSelectionsTextures[]=
        {
            "EFT\armband\data\armband_kibaarms.paa",
            "EFT\armband\data\armband_kibaarms.paa",
            "EFT\armband\data\armband_kibaarms.paa",
            "EFT\armband\data\armband_kibaarms.paa",
            "EFT\armband\data\armband_kibaarms.paa",
            "EFT\armband\data\armband_kibaarms.paa",
            "EFT\armband\data\armband_kibaarms.paa",
            "EFT\armband\data\armband_kibaarms.paa",
            "EFT\armband\data\armband_kibaarms.paa"
        };
    };
	class EFT_armband_labs: EFT_armband
    {
        itemSize[]={1,1};
        scope=2;
        
        hiddenSelectionsTextures[]=
        {
            "EFT\armband\data\armband_labs.paa",
            "EFT\armband\data\armband_labs.paa",
            "EFT\armband\data\armband_labs.paa",
            "EFT\armband\data\armband_labs.paa",
            "EFT\armband\data\armband_labs.paa",
            "EFT\armband\data\armband_labs.paa",
            "EFT\armband\data\armband_labs.paa",
            "EFT\armband\data\armband_labs.paa",
            "EFT\armband\data\armband_labs.paa"
        };
    };
	class EFT_armband_red: EFT_armband
    {
        itemSize[]={1,1};
        scope=2;
        
        hiddenSelectionsTextures[]=
        {
            "EFT\armband\data\armband_red.paa",
            "EFT\armband\data\armband_red.paa",
            "EFT\armband\data\armband_red.paa",
            "EFT\armband\data\armband_red.paa",
            "EFT\armband\data\armband_red.paa",
            "EFT\armband\data\armband_red.paa",
            "EFT\armband\data\armband_red.paa",
            "EFT\armband\data\armband_red.paa",
            "EFT\armband\data\armband_red.paa"
        };
    };
	class EFT_armband_revals: EFT_armband
    {
        itemSize[]={1,1};
        scope=2;
        
        hiddenSelectionsTextures[]=
        {
            "EFT\armband\data\armband_revals.paa",
            "EFT\armband\data\armband_revals.paa",
            "EFT\armband\data\armband_revals.paa",
            "EFT\armband\data\armband_revals.paa",
            "EFT\armband\data\armband_revals.paa",
            "EFT\armband\data\armband_revals.paa",
            "EFT\armband\data\armband_revals.paa",
            "EFT\armband\data\armband_revals.paa",
            "EFT\armband\data\armband_revals.paa"
        };
    };
	class EFT_armband_russia: EFT_armband
    {
        itemSize[]={1,1};
        scope=2;
        
        hiddenSelectionsTextures[]=
        {
            "EFT\armband\data\armband_russia.paa",
            "EFT\armband\data\armband_russia.paa",
            "EFT\armband\data\armband_russia.paa",
            "EFT\armband\data\armband_russia.paa",
            "EFT\armband\data\armband_russia.paa",
            "EFT\armband\data\armband_russia.paa",
            "EFT\armband\data\armband_russia.paa",
            "EFT\armband\data\armband_russia.paa",
            "EFT\armband\data\armband_russia.paa"
        };
    };
	class EFT_armband_terragroup: EFT_armband
    {
        itemSize[]={1,1};
        scope=2;
        
        hiddenSelectionsTextures[]=
        {
            "EFT\armband\data\armband_terragroup.paa",
            "EFT\armband\data\armband_terragroup.paa",
            "EFT\armband\data\armband_terragroup.paa",
            "EFT\armband\data\armband_terragroup.paa",
            "EFT\armband\data\armband_terragroup.paa",
            "EFT\armband\data\armband_terragroup.paa",
            "EFT\armband\data\armband_terragroup.paa",
            "EFT\armband\data\armband_terragroup.paa",
            "EFT\armband\data\armband_terragroup.paa"
        };
    };
	class EFT_armband_un: EFT_armband
    {
        itemSize[]={1,1};
        scope=2;
        
        hiddenSelectionsTextures[]=
        {
            "EFT\armband\data\armband_un.paa",
            "EFT\armband\data\armband_un.paa",
            "EFT\armband\data\armband_un.paa",
            "EFT\armband\data\armband_un.paa",
            "EFT\armband\data\armband_un.paa",
            "EFT\armband\data\armband_un.paa",
            "EFT\armband\data\armband_un.paa",
            "EFT\armband\data\armband_un.paa",
            "EFT\armband\data\armband_un.paa"
        };
    };
	class EFT_armband_usec: EFT_armband
    {
        itemSize[]={1,1};
        scope=2;
        
        hiddenSelectionsTextures[]=
        {
            "EFT\armband\data\armband_usec.paa",
            "EFT\armband\data\armband_usec.paa",
            "EFT\armband\data\armband_usec.paa",
            "EFT\armband\data\armband_usec.paa",
            "EFT\armband\data\armband_usec.paa",
            "EFT\armband\data\armband_usec.paa",
            "EFT\armband\data\armband_usec.paa",
            "EFT\armband\data\armband_usec.paa",
            "EFT\armband\data\armband_usec.paa"
        };
    };
};