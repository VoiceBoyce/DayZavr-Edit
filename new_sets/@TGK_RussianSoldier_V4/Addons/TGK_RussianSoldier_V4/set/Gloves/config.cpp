class CfgPatches
{
	class TGK_RussianSolider_V4_Gloves
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters",
			"DZ_Scripts",
			"DZ_Characters_Gloves"
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
	class TGK_RussianSolider_V4_Gloves_Colorbase: TacticalGloves_ColorBase
	{
		scope=0;
		displayName="$STR_TGK_RussianSolider_V4_Gloves_01";
		descriptionShort="$STR_TGK_RussianSolider_V4_Gloves_02";
		model="TGK_RussianSoldier_V4\set\Gloves\models\Gloves_g.p3d";
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
			male="TGK_RussianSoldier_V4\set\Gloves\models\Gloves_m.p3d";
			female="TGK_RussianSoldier_V4\set\Gloves\models\Gloves_m.p3d";
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
								"TGK_RussianSoldier_V4\set\Gloves\data\T_Glove3.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_RussianSoldier_V4\set\Gloves\data\T_Glove3.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_RussianSoldier_V4\set\Gloves\data\T_Glove3_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_RussianSoldier_V4\set\Gloves\data\T_Glove3_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_RussianSoldier_V4\set\Gloves\data\T_Glove3_destruct.rvmat"
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
	class TGK_RussianSolider_V4_Gloves_ColorW: TGK_RussianSolider_V4_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Gloves\data\T_Glove3_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Gloves_CamoA: TGK_RussianSolider_V4_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Gloves\data\T_Gloves3A_CamoA_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Gloves_CamoB: TGK_RussianSolider_V4_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Gloves\data\T_Gloves3A_CamoB_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Gloves_CamoC: TGK_RussianSolider_V4_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Gloves\data\T_Gloves3A_CamoC_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Gloves_CamoD: TGK_RussianSolider_V4_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Gloves\data\T_Gloves3A_CamoD_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Gloves_CamoE: TGK_RussianSolider_V4_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Gloves\data\T_Gloves3A_CamoE_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Gloves_CamoF: TGK_RussianSolider_V4_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Gloves\data\T_Gloves3A_CamoF_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Gloves_CamoG: TGK_RussianSolider_V4_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Gloves\data\T_Gloves3A_CamoG_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Gloves_CamoH: TGK_RussianSolider_V4_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Gloves\data\T_Gloves3A_CamoH_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Gloves_CamoL: TGK_RussianSolider_V4_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Gloves\data\T_Gloves3A_CamoI_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Gloves_CamoJ: TGK_RussianSolider_V4_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Gloves\data\T_Gloves3A_CamoJ_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Gloves_CamoK: TGK_RussianSolider_V4_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Gloves\data\T_Gloves3A_CamoK_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Gloves_ColorA: TGK_RussianSolider_V4_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Gloves\data\T_Gloves3A_ColorA_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Gloves_ColorB: TGK_RussianSolider_V4_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Gloves\data\T_Gloves3A_ColorB_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Gloves_ColorC: TGK_RussianSolider_V4_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Gloves\data\T_Gloves3A_ColorC_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Gloves_ColorD: TGK_RussianSolider_V4_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Gloves\data\T_Gloves3A_ColorD_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Gloves_ColorE: TGK_RussianSolider_V4_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Gloves\data\T_Gloves3A_ColorE_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Gloves_ColorF: TGK_RussianSolider_V4_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Gloves\data\T_Gloves3A_ColorF_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Gloves_ColorG: TGK_RussianSolider_V4_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Gloves\data\T_Gloves3A_ColorG_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Gloves_ColorH: TGK_RussianSolider_V4_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Gloves\data\T_Gloves3A_ColorH_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Gloves_ColorL: TGK_RussianSolider_V4_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Gloves\data\T_Gloves3A_ColorI_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Gloves_ColorJ: TGK_RussianSolider_V4_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Gloves\data\T_Gloves3A_ColorJ_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Gloves_ColorK: TGK_RussianSolider_V4_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Gloves\data\T_Gloves3A_ColorK_D.paa"
		};
	};
};
