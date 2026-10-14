class CfgPatches
{
	class TGK_RussianSolider_V4_Helmet
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters",
			"DZ_Scripts",
			"DZ_Characters_Headgear"
		};
	};
};
class cfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class Mich2001Helmet;
	class TGK_RussianSolider_V4_Helmet_ColorBase: Mich2001Helmet
	{
		scope=0;
		displayName="$STR_TGK_RussianSolider_V4_Helmet_01";
		descriptionShort="$STR_TGK_RussianSolider_V4_Helmet_02";
		model="TGK_RussianSoldier_V4\set\Helmet\Helmet_g.p3d";
		attachments[]={};
		repairableWithKits[]={8};
		repairCosts[]={25};
		inventorySlot[]=
		{
			"Headgear",
			"Helmet"
		};
		simulation="clothing";
		vehicleClass="Clothing";
		itemInfo[]=
		{
			"Clothing",
			"Headgear"
		};
		weight=1300;
		quickBarBonus=0;
		itemSize[]={4,4};
		noMask=0;
		noNVStrap=1;
		noGlasses=1;
		varWetMax=0.1;
		heatIsolation=0.34999999;
		headSelectionsToHide[]=
		{
			"Clipping_Mich2001"
		};
		hiddenSelections[]=
		{
			"camo"
		};
		rotationFlags=8;
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
								"TGK_RussianSoldier_V4\set\Balaclava\data\T_Headgear.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_RussianSoldier_V4\set\Balaclava\data\T_Headgear.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_RussianSoldier_V4\set\Balaclava\data\T_Headgear_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_RussianSoldier_V4\set\Balaclava\data\T_Headgear_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_RussianSoldier_V4\set\Balaclava\data\T_Headgear_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		class ClothingTypes
		{
			male="TGK_RussianSoldier_V4\set\Helmet\Helmet_m.p3d";
			female="TGK_RussianSoldier_V4\set\Helmet\Helmet_m.p3d";
		};
		class Protection
		{
			biological=0.25;
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
	class TGK_RussianSolider_V4_Helmet_CamoA: TGK_RussianSolider_V4_Helmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Balaclava\data\T_HeadGear_CamoA_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Helmet_CamoB: TGK_RussianSolider_V4_Helmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Balaclava\data\T_HeadGear_CamoB_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Helmet_CamoC: TGK_RussianSolider_V4_Helmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Balaclava\data\T_HeadGear_CamoC_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Helmet_CamoD: TGK_RussianSolider_V4_Helmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Balaclava\data\T_HeadGear_CamoD_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Helmet_CamoE: TGK_RussianSolider_V4_Helmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Balaclava\data\T_HeadGear_CamoE_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Helmet_CamoF: TGK_RussianSolider_V4_Helmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Balaclava\data\T_HeadGear_CamoF_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Helmet_CamoG: TGK_RussianSolider_V4_Helmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Balaclava\data\T_HeadGear_CamoG_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Helmet_CamoH: TGK_RussianSolider_V4_Helmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Balaclava\data\T_HeadGear_CamoH_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Helmet_CamoL: TGK_RussianSolider_V4_Helmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Balaclava\data\T_HeadGear_CamoI_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Helmet_CamoJ: TGK_RussianSolider_V4_Helmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Balaclava\data\T_HeadGear_CamoJ_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Helmet_CamoK: TGK_RussianSolider_V4_Helmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Balaclava\data\T_HeadGear_CamoK_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Helmet_ColorA: TGK_RussianSolider_V4_Helmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Balaclava\data\T_HeadGear_ColorA_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Helmet_ColorB: TGK_RussianSolider_V4_Helmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Balaclava\data\T_HeadGear_ColorB_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Helmet_ColorC: TGK_RussianSolider_V4_Helmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Balaclava\data\T_HeadGear_ColorC_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Helmet_ColorD: TGK_RussianSolider_V4_Helmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Balaclava\data\T_HeadGear_ColorD_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Helmet_ColorE: TGK_RussianSolider_V4_Helmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Balaclava\data\T_HeadGear_ColorE_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Helmet_ColorF: TGK_RussianSolider_V4_Helmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Balaclava\data\T_HeadGear_ColorF_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Helmet_ColorG: TGK_RussianSolider_V4_Helmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Balaclava\data\T_HeadGear_ColorG_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Helmet_ColorH: TGK_RussianSolider_V4_Helmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Balaclava\data\T_HeadGear_ColorH_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Helmet_ColorL: TGK_RussianSolider_V4_Helmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Balaclava\data\T_HeadGear_ColorI_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Helmet_ColorJ: TGK_RussianSolider_V4_Helmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Balaclava\data\T_HeadGear_ColorJ_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Helmet_ColorK: TGK_RussianSolider_V4_Helmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Balaclava\data\T_HeadGear_ColorK_D.paa"
		};
	};
	class TGK_RussianSolider_V4_Helmet_ColorW: TGK_RussianSolider_V4_Helmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Balaclava\data\T_Headgear_D.paa"
		};
	};
};
