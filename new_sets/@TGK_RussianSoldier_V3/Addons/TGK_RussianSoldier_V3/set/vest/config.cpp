class CfgPatches
{
	class TGK_RussianSolider_V3_Vest
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters",
			"DZ_Scripts",
			"DZ_Characters_Vests"
		};
	};
};
class cfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class PlateCarrierVest;
	class TGK_RussianSolider_V3_Vest_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_TGK_RussianSolider_V3_VEST_01";
		descriptionShort="$STR_TGK_RussianSolider_V3_VEST_02";
		model="TGK_RussianSoldier_V3\set\vest\models\Vest_g.p3d";
		inventorySlot[]=
		{
			"Vest"
		};
		itemInfo[]=
		{
			"Clothing",
			"Vest"
		};
		weight=3000;
		itemSize[]={4,4};
		quickBarBonus=1;
		varWetMax=0.5;
		heatIsolation=0.80000001;
		repairableWithKits[]={3,8};
		repairCosts[]={75,50};
		visibilityModifier=0.94999999;
		hiddenSelections[]=
		{
			"camo"
		};
		class ClothingTypes
		{
			male="TGK_RussianSoldier_V3\set\vest\models\Vest_m.p3d";
			female="TGK_RussianSoldier_V3\set\vest\models\Vest_m.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=500;
					transferToAttachmentsCoef=0.5;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"TGK_RussianSoldier_V3\set\vest\data\T_Vest.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_RussianSoldier_V3\set\vest\data\T_Vest.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_RussianSoldier_V3\set\vest\data\T_Vest_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_RussianSoldier_V3\set\vest\data\T_Vest_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_RussianSoldier_V3\set\vest\data\T_Vest_destruct.rvmat"
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
					soundSet="SmershVest_pickup_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="SmershVest_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class TGK_RussianSolider_V3_Vest_CamoA: TGK_RussianSolider_V3_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\vest\data\T_Vest_CamoA_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Vest_CamoB: TGK_RussianSolider_V3_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\vest\data\T_Vest_CamoB_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Vest_CamoC: TGK_RussianSolider_V3_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\vest\data\T_Vest_CamoC_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Vest_CamoD: TGK_RussianSolider_V3_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\vest\data\T_Vest_CamoD_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Vest_CamoE: TGK_RussianSolider_V3_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\vest\data\T_Vest_CamoE_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Vest_CamoF: TGK_RussianSolider_V3_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\vest\data\T_Vest_CamoF_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Vest_CamoG: TGK_RussianSolider_V3_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\vest\data\T_Vest_CamoG_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Vest_CamoH: TGK_RussianSolider_V3_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\vest\data\T_Vest_CamoH_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Vest_CamoL: TGK_RussianSolider_V3_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\vest\data\T_Vest_CamoI_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Vest_CamoJ: TGK_RussianSolider_V3_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\vest\data\T_Vest_CamoJ_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Vest_CamoK: TGK_RussianSolider_V3_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\vest\data\T_Vest_CamoK_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Vest_ColorA: TGK_RussianSolider_V3_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\vest\data\T_Vest_ColorA_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Vest_ColorB: TGK_RussianSolider_V3_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\vest\data\T_Vest_ColorB_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Vest_ColorC: TGK_RussianSolider_V3_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\vest\data\T_Vest_ColorC_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Vest_ColorD: TGK_RussianSolider_V3_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\vest\data\T_Vest_ColorD_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Vest_ColorE: TGK_RussianSolider_V3_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\vest\data\T_Vest_ColorE_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Vest_ColorF: TGK_RussianSolider_V3_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\vest\data\T_Vest_ColorF_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Vest_ColorG: TGK_RussianSolider_V3_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\vest\data\T_Vest_ColorG_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Vest_ColorH: TGK_RussianSolider_V3_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\vest\data\T_Vest_ColorH_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Vest_ColorL: TGK_RussianSolider_V3_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\vest\data\T_Vest_ColorI_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Vest_ColorJ: TGK_RussianSolider_V3_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\vest\data\T_Vest_ColorJ_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Vest_ColorK: TGK_RussianSolider_V3_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\vest\data\T_Vest_ColorK_D.paa"
		};
	};
	class TGK_RussianSolider_V3_Vest_ColorW: TGK_RussianSolider_V3_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\vest\data\T_Vest_D.paa"
		};
	};
};
