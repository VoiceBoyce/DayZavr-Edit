class CfgPatches
{
	class TGK_RussianSoldier_v2
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
	class TGK_RussianSoldier_v2_Gloves_Colorbase: TacticalGloves_ColorBase
	{
		scope=0;
		displayName="$STR_RusSold_v2_Gloves_01";
		descriptionShort="$STR_RusSold_v2_Gloves_02";
		model="TGK_RussianSoldier_V2\set\Gloves\models\Gloves_g.p3d";
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
			male="TGK_RussianSoldier_V2\set\Gloves\models\Gloves_m.p3d";
			female="TGK_RussianSoldier_V2\set\Gloves\models\Gloves_m.p3d";
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
								"TGK_RussianSoldier_V2\set\Gloves\data\Combat_gloves.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_RussianSoldier_V2\set\Gloves\data\Combat_gloves.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_RussianSoldier_V2\set\Gloves\data\Combat_gloves_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_RussianSoldier_V2\set\Gloves\data\Combat_gloves_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_RussianSoldier_V2\set\Gloves\data\Combat_gloves_destruct.rvmat"
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
	class TGK_RussianSoldier_v2_Gloves_Alpine: TGK_RussianSoldier_v2_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Gloves\data\Gloves_Alpine.paa"
		};
	};
	class TGK_RussianSoldier_v2_Gloves_Black: TGK_RussianSoldier_v2_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Gloves\data\Gloves_Black.paa"
		};
	};
	class TGK_RussianSoldier_v2_Gloves_Black_MC: TGK_RussianSoldier_v2_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Gloves\data\Gloves_Black_MC.paa"
		};
	};
	class TGK_RussianSoldier_v2_Gloves_MC: TGK_RussianSoldier_v2_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Gloves\data\Gloves_MC.paa"
		};
	};
	class TGK_RussianSoldier_v2_Gloves_EMR: TGK_RussianSoldier_v2_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Gloves\data\Gloves_EMR.paa"
		};
	};
};
