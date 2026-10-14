class CfgPatches
{
	class TGK_RussianSoldier_v4_Vest
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
	class TGK_RussianSoldier_V4_Vest_ColorBase: PlateCarrierVest
	{
		scope=0;
		displayName="$STR_RusV4_Vest01";
		descriptionShort="$STR_RusV4_Vest02";
		model="TGK_RussianSoldier_V4\set\Vest\models\Vest_g.p3d";
		inventorySlot[]=
		{
			"Vest"
		};
		itemInfo[]=
		{
			"Clothing",
			"Vest"
		};
		weight=7500;
		itemSize[]={4,4};
		quickBarBonus=1;
		varWetMax=0.49000001;
		heatIsolation=0.80000001;
		repairableWithKits[]={3,8};
		repairCosts[]={75,50};
		visibilityModifier=0.94999999;
		hiddenSelections[]=
		{
			"camo",
			"camo1"
		};
		class ClothingTypes
		{
			male="TGK_RussianSoldier_V4\set\Vest\models\Vest_m.p3d";
			female="TGK_RussianSoldier_V4\set\Vest\models\Vest_m.p3d";
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
								"TGK_RussianSoldier_V4\set\Vest\data\T_Vest_CamoA.rvmat",
								"TGK_RussianSoldier_V4\set\Vest\data\camo1\T_Vest.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_RussianSoldier_V4\set\Vest\data\T_Vest_CamoA.rvmat",
								"TGK_RussianSoldier_V4\set\Vest\data\camo1\T_Vest.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_RussianSoldier_V4\set\Vest\data\T_Vest_CamoA_damage.rvmat",
								"TGK_RussianSoldier_V4\set\Vest\data\camo1\T_Vest_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_RussianSoldier_V4\set\Vest\data\T_Vest_CamoA_damage.rvmat",
								"TGK_RussianSoldier_V4\set\Vest\data\camo1\T_Vest_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_RussianSoldier_V4\set\Vest\data\T_Vest_CamoA_destruct.rvmat",
								"TGK_RussianSoldier_V4\set\Vest\data\camo1\T_Vest_destruct.rvmat"
							}
						}
					};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage=0.31;
					};
					class Blood
					{
						damage=0.31;
					};
					class Shock
					{
						damage=0.31;
					};
				};
				class Melee
				{
					class Health
					{
						damage=0.20999999;
					};
					class Blood
					{
						damage=0.20999999;
					};
					class Shock
					{
						damage=0.20999999;
					};
				};
				class Infected
				{
					class Health
					{
						damage=0.20999999;
					};
					class Blood
					{
						damage=0.20999999;
					};
					class Shock
					{
						damage=0.20999999;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage=0.20999999;
					};
					class Blood
					{
						damage=0.20999999;
					};
					class Shock
					{
						damage=0.20999999;
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
	class TGK_RussianSoldier_V4_Vest_CamoA_D: TGK_RussianSoldier_V4_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Vest\data\T_Vest_CamoA_D.paa",
			"TGK_RussianSoldier_V4\set\Vest\data\camo1\T_Vest_CamoA_D.paa"
		};
	};
	class TGK_RussianSoldier_V4_Vest_CamoB_D: TGK_RussianSoldier_V4_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Vest\data\T_Vest_CamoB_D.paa",
			"TGK_RussianSoldier_V4\set\Vest\data\camo1\T_Vest_CamoB_D.paa"
		};
	};
	class TGK_RussianSoldier_V4_Vest_CamoC_D: TGK_RussianSoldier_V4_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Vest\data\T_Vest_CamoC_D.paa",
			"TGK_RussianSoldier_V4\set\Vest\data\camo1\T_Vest_CamoC_D.paa"
		};
	};
	class TGK_RussianSoldier_V4_Vest_CamoD_D: TGK_RussianSoldier_V4_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Vest\data\T_Vest_CamoD_D.paa",
			"TGK_RussianSoldier_V4\set\Vest\data\camo1\T_Vest_CamoD_D.paa"
		};
	};
	class TGK_RussianSoldier_V4_Vest_CamoE_D: TGK_RussianSoldier_V4_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Vest\data\T_Vest_CamoE_D.paa",
			"TGK_RussianSoldier_V4\set\Vest\data\camo1\T_Vest_CamoE_D.paa"
		};
	};
	class TGK_RussianSoldier_V4_Vest_CamoF_D: TGK_RussianSoldier_V4_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Vest\data\T_Vest_CamoF_D.paa",
			"TGK_RussianSoldier_V4\set\Vest\data\camo1\T_Vest_CamoF_D.paa"
		};
	};
	class TGK_RussianSoldier_V4_Vest_CamoG_D: TGK_RussianSoldier_V4_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Vest\data\T_Vest_CamoG_D.paa",
			"TGK_RussianSoldier_V4\set\Vest\data\camo1\T_Vest_CamoG_D.paa"
		};
	};
	class TGK_RussianSoldier_V4_Vest_CamoH_D: TGK_RussianSoldier_V4_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Vest\data\T_Vest_CamoH_D.paa",
			"TGK_RussianSoldier_V4\set\Vest\data\camo1\T_Vest_CamoH_D.paa"
		};
	};
	class TGK_RussianSoldier_V4_Vest_CamoI_D: TGK_RussianSoldier_V4_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Vest\data\T_Vest_CamoI_D.paa",
			"TGK_RussianSoldier_V4\set\Vest\data\camo1\T_Vest_CamoI_D.paa"
		};
	};
	class TGK_RussianSoldier_V4_Vest_CamoJ_D: TGK_RussianSoldier_V4_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Vest\data\T_Vest_CamoJ_D.paa",
			"TGK_RussianSoldier_V4\set\Vest\data\camo1\T_Vest_CamoJ_D.paa"
		};
	};
	class TGK_RussianSoldier_V4_Vest_CamoK_D: TGK_RussianSoldier_V4_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Vest\data\T_Vest_CamoK_D.paa",
			"TGK_RussianSoldier_V4\set\Vest\data\camo1\T_Vest_CamoK_D.paa"
		};
	};
	class TGK_RussianSoldier_V4_Vest_ColorA_D: TGK_RussianSoldier_V4_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Vest\data\T_Vest_ColorA_D.paa",
			"TGK_RussianSoldier_V4\set\Vest\data\camo1\T_Vest_ColorA_D.paa"
		};
	};
	class TGK_RussianSoldier_V4_Vest_ColorB_D: TGK_RussianSoldier_V4_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Vest\data\T_Vest_ColorB_D.paa",
			"TGK_RussianSoldier_V4\set\Vest\data\camo1\T_Vest_ColorB_D.paa"
		};
	};
	class TGK_RussianSoldier_V4_Vest_ColorC_D: TGK_RussianSoldier_V4_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Vest\data\T_Vest_ColorC_D.paa",
			"TGK_RussianSoldier_V4\set\Vest\data\camo1\T_Vest_ColorC_D.paa"
		};
	};
	class TGK_RussianSoldier_V4_Vest_ColorD_D: TGK_RussianSoldier_V4_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Vest\data\T_Vest_ColorD_D.paa",
			"TGK_RussianSoldier_V4\set\Vest\data\camo1\T_Vest_ColorD_D.paa"
		};
	};
	class TGK_RussianSoldier_V4_Vest_ColorE_D: TGK_RussianSoldier_V4_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Vest\data\T_Vest_ColorE_D.paa",
			"TGK_RussianSoldier_V4\set\Vest\data\camo1\T_Vest_ColorE_D.paa"
		};
	};
	class TGK_RussianSoldier_V4_Vest_ColorF_D: TGK_RussianSoldier_V4_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Vest\data\T_Vest_ColorF_D.paa",
			"TGK_RussianSoldier_V4\set\Vest\data\camo1\T_Vest_ColorF_D.paa"
		};
	};
	class TGK_RussianSoldier_V4_Vest_ColorG_D: TGK_RussianSoldier_V4_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Vest\data\T_Vest_ColorG_D.paa",
			"TGK_RussianSoldier_V4\set\Vest\data\camo1\T_Vest_ColorG_D.paa"
		};
	};
	class TGK_RussianSoldier_V4_Vest_ColorH_D: TGK_RussianSoldier_V4_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Vest\data\T_Vest_ColorH_D.paa",
			"TGK_RussianSoldier_V4\set\Vest\data\camo1\T_Vest_ColorH_D.paa"
		};
	};
	class TGK_RussianSoldier_V4_Vest_ColorI_D: TGK_RussianSoldier_V4_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Vest\data\T_Vest_ColorI_D.paa",
			"TGK_RussianSoldier_V4\set\Vest\data\camo1\T_Vest_ColorI_D.paa"
		};
	};
	class TGK_RussianSoldier_V4_Vest_ColorJ_D: TGK_RussianSoldier_V4_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Vest\data\T_Vest_ColorJ_D.paa",
			"TGK_RussianSoldier_V4\set\Vest\data\camo1\T_Vest_ColorJ_D.paa"
		};
	};
	class TGK_RussianSoldier_V4_Vest_ColorK_D: TGK_RussianSoldier_V4_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Vest\data\T_Vest_ColorK_D.paa",
			"TGK_RussianSoldier_V4\set\Vest\data\camo1\T_Vest_ColorK_D.paa"
		};
	};
	class TGK_RussianSoldier_V4_Vest_ColorW_D: TGK_RussianSoldier_V4_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Vest\data\T_Vest_D.paa",
			"TGK_RussianSoldier_V4\set\Vest\data\camo1\T_Vest_D.paa"
		};
	};
};
