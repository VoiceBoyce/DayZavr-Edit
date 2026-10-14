class CfgPatches
{
	class TGK_RussianSolider_V3
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
	class BalaclavaMask_ColorBase;
	class TGK_RussianSolider_V3_Balacalava_ColorBase: BalaclavaMask_ColorBase
	{
		scope=0;
		displayName="$STR_TGK_RussianSolider_V3_Balaclava_01";
		descriptionShort="$STR_TGK_RussianSolider_V3_Balaclava_02";
		inventorySlot[]=
		{
			"Mask"
		};
		itemInfo[]=
		{
			"Clothing",
			"Mask"
		};
		rotationFlags=17;
		weight=300;
		itemSize[]={2,2};
		ragQuantity=2;
		varWetMax=0.34999999;
		heatIsolation=0.5;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		model="TGK_RussianSoldier_V3\set\balaclava\models\balaclava_g.p3d";
		headSelectionsToHide[]=
		{
			"Clipping_Gasmask",
			"Clipping_Balaclava_3holes"
		};
		hiddenSelections[]=
		{
			"camo"
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
								"TGK_RussianSoldier_V3\set\balaclava\data\T_Headgear.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_RussianSoldier_V3\set\balaclava\data\T_Headgear.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_RussianSoldier_V3\set\balaclava\data\T_Headgear_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_RussianSoldier_V3\set\balaclava\data\T_Headgear_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_RussianSoldier_V3\set\balaclava\data\T_Headgear_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		class ClothingTypes
		{
			male="TGK_RussianSoldier_V3\set\balaclava\models\balaclava_m.p3d";
			female="TGK_RussianSoldier_V3\set\balaclava\models\balaclava_m.p3d";
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
	class TGK_RussianSolider_V3_Balacalava_CamoA: TGK_RussianSolider_V3_Balacalava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\balaclava\data\T_HeadGear_CamoA_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Balacalava_CamoB: TGK_RussianSolider_V3_Balacalava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\balaclava\data\T_HeadGear_CamoB_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Balacalava_CamoC: TGK_RussianSolider_V3_Balacalava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\balaclava\data\T_HeadGear_CamoC_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Balacalava_CamoD: TGK_RussianSolider_V3_Balacalava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\balaclava\data\T_HeadGear_CamoD_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Balacalava_CamoE: TGK_RussianSolider_V3_Balacalava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\balaclava\data\T_HeadGear_CamoE_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Balacalava_CamoF: TGK_RussianSolider_V3_Balacalava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\balaclava\data\T_HeadGear_CamoF_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Balacalava_CamoG: TGK_RussianSolider_V3_Balacalava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\balaclava\data\T_HeadGear_CamoG_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Balacalava_CamoH: TGK_RussianSolider_V3_Balacalava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\balaclava\data\T_HeadGear_CamoH_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Balacalava_CamoL: TGK_RussianSolider_V3_Balacalava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\balaclava\data\T_HeadGear_CamoI_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Balacalava_CamoJ: TGK_RussianSolider_V3_Balacalava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\balaclava\data\T_HeadGear_CamoJ_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Balacalava_CamoK: TGK_RussianSolider_V3_Balacalava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\balaclava\data\T_HeadGear_CamoK_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Balacalava_ColorA: TGK_RussianSolider_V3_Balacalava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\balaclava\data\T_HeadGear_ColorA_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Balacalava_ColorB: TGK_RussianSolider_V3_Balacalava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\balaclava\data\T_HeadGear_ColorB_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Balacalava_ColorC: TGK_RussianSolider_V3_Balacalava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\balaclava\data\T_HeadGear_ColorC_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Balacalava_ColorD: TGK_RussianSolider_V3_Balacalava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\balaclava\data\T_HeadGear_ColorD_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Balacalava_ColorE: TGK_RussianSolider_V3_Balacalava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\balaclava\data\T_HeadGear_ColorE_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Balacalava_ColorF: TGK_RussianSolider_V3_Balacalava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\balaclava\data\T_HeadGear_ColorF_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Balacalava_ColorG: TGK_RussianSolider_V3_Balacalava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\balaclava\data\T_HeadGear_ColorG_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Balacalava_ColorH: TGK_RussianSolider_V3_Balacalava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\balaclava\data\T_HeadGear_ColorH_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Balacalava_ColorL: TGK_RussianSolider_V3_Balacalava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\balaclava\data\T_HeadGear_ColorI_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Balacalava_ColorJ: TGK_RussianSolider_V3_Balacalava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\balaclava\data\T_HeadGear_ColorJ_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Balacalava_ColorK: TGK_RussianSolider_V3_Balacalava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\balaclava\data\T_HeadGear_ColorK_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Balacalava_ColorW: TGK_RussianSolider_V3_Balacalava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\balaclava\data\T_Headgear_D.paa"
		};
	};
};
