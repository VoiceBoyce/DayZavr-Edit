class CfgPatches
{
	class TGK_FSB_ALPHA
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters",
			"DZ_Scripts"
		};
	};
};
class cfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class TacticalGloves_ColorBase;
	class TGK_FSB_ALPHA_Gloves_Colorbase: TacticalGloves_ColorBase
	{
		scope=0;
		displayName="$STR_FSB_ALPHA_Gloves_01";
		descriptionShort="$STR_FSB_ALPHA_Gloves_02";
		model="TGK_FSB_Alpha\set\Gloves\models\Gloves_g.p3d";
		inventorySlot[]=
		{
			"Gloves"
		};
		simulation="clothing";
		itemInfo[]=
		{
			"Clothing",
			"Gloves"
		};
		weight=450;
		itemSize[]={2,2};
		varWetMax=1;
		heatIsolation=0.5;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		rotationFlags=17;
		hiddenSelections[]=
		{
			"camo",
			"personality"
		};
		class ClothingTypes
		{
			male="TGK_FSB_Alpha\set\Gloves\models\Gloves_m.p3d";
			female="TGK_FSB_Alpha\set\Gloves\models\Gloves_m.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=200;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"TGK_FSB_Alpha\set\Gloves\data\Top.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_FSB_Alpha\set\Gloves\data\Top.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_FSB_Alpha\set\Gloves\data\Top_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_FSB_Alpha\set\Gloves\data\Top_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_FSB_Alpha\set\Gloves\data\Top_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem
				{
					soundSet="WorkingGloves_pickup_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="WorkingGloves_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class TGK_FSB_ALPHA_Gloves_Alpine: TGK_FSB_ALPHA_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Gloves\data\gloves_Alpine.paa"
		};
	};
	class TGK_FSB_ALPHA_Gloves_Black: TGK_FSB_ALPHA_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Gloves\data\gloves_Black.paa"
		};
	};
	class TGK_FSB_ALPHA_Gloves_Black_MC: TGK_FSB_ALPHA_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Gloves\data\gloves_Black_MC.paa"
		};
	};
	class TGK_FSB_ALPHA_Gloves_MC: TGK_FSB_ALPHA_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Gloves\data\gloves_MC.paa"
		};
	};
	class TGK_FSB_ALPHA_Gloves_ODGreen: TGK_FSB_ALPHA_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Gloves\data\gloves_ODGreen.paa"
		};
	};
	class TGK_FSB_ALPHA_Gloves_Tropic_MC: TGK_FSB_ALPHA_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Gloves\data\gloves_Tropic_MC.paa"
		};
	};
};
