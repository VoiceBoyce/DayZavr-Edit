class CfgPatches
{
	class TGK_RussianSolider_V3_TOP
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters",
			"DZ_Scripts",
			"DZ_Characters_Tops"
		};
	};
};
class CfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class GorkaEJacket_ColorBase;
	class TGK_RussianSolider_V3_TOP_ColorBase: GorkaEJacket_ColorBase
	{
		scope=0;
		displayName="$STR_TGK_RussianSolider_V3_TOP_01";
		descriptionShort="$STR_TGK_RussianSolider_V3_TOP_02";
		model="TGK_RussianSoldier_V3\set\top\models\Top_g.p3d";
		inventorySlot[]=
		{
			"Body"
		};
		itemInfo[]=
		{
			"Clothing",
			"Body"
		};
		weight=1000;
		itemSize[]={4,6};
		quickBarBonus=2;
		ragQuantity=3;
		varWetMax=0.249;
		heatIsolation=0.40000001;
		itemsCargoSize[]={10,5};
		repairableWithKits[]={2,3};
		repairCosts[]={75,50};
		hiddenSelections[]=
		{
			"camo"
		};
		rotationFlags=17;
		class ClothingTypes
		{
			male="TGK_RussianSoldier_V3\set\top\models\Top_m.p3d";
			female="TGK_RussianSoldier_V3\set\top\models\Top_m.p3d";
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
								"TGK_RussianSoldier_V3\set\top\data\T_Jacket_CamoA.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_RussianSoldier_V3\set\top\data\T_Jacket_CamoA.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_RussianSoldier_V3\set\top\data\T_Jacket_CamoA_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_RussianSoldier_V3\set\top\data\T_Jacket_CamoA_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_RussianSoldier_V3\set\top\data\T_Jacket_CamoA_destruct.rvmat"
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
						damage=0.94999999;
					};
					class Blood
					{
						damage=0.94999999;
					};
					class Shock
					{
						damage=0;
					};
				};
				class Melee
				{
					class Health
					{
						damage=0.94999999;
					};
					class Blood
					{
						damage=0.94999999;
					};
					class Shock
					{
						damage=0.44999999;
					};
				};
				class Infected
				{
					class Health
					{
						damage=0.1;
					};
					class Blood
					{
						damage=0.5;
					};
					class Shock
					{
						damage=0.15000001;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage=1;
					};
					class Blood
					{
						damage=0.1;
					};
					class Shock
					{
						damage=0.1;
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
	class TGK_RussianSolider_V3_TOP_CamoA: TGK_RussianSolider_V3_TOP_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\top\data\T_Jacket_CamoA_D.paa"
		};
	};
	class TGK_RussianSolider_V3_TOP_CamoB: TGK_RussianSolider_V3_TOP_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\top\data\T_Jacket_CamoB_D.paa"
		};
	};
	class TGK_RussianSolider_V3_TOP_CamoC: TGK_RussianSolider_V3_TOP_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\top\data\T_Jacket_CamoC_D.paa"
		};
	};
	class TGK_RussianSolider_V3_TOP_CamoD: TGK_RussianSolider_V3_TOP_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\top\data\T_Jacket_CamoD_D.paa"
		};
	};
	class TGK_RussianSolider_V3_TOP_CamoE: TGK_RussianSolider_V3_TOP_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\top\data\T_Jacket_CamoE_D.paa"
		};
	};
	class TGK_RussianSolider_V3_TOP_CamoF: TGK_RussianSolider_V3_TOP_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\top\data\T_Jacket_CamoF_D.paa"
		};
	};
	class TGK_RussianSolider_V3_TOP_CamoG: TGK_RussianSolider_V3_TOP_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\top\data\T_Jacket_CamoG_D.paa"
		};
	};
	class TGK_RussianSolider_V3_TOP_CamoH: TGK_RussianSolider_V3_TOP_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\top\data\T_Jacket_CamoH_D.paa"
		};
	};
	class TGK_RussianSolider_V3_TOP_CamoL: TGK_RussianSolider_V3_TOP_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\top\data\T_Jacket_CamoI_D.paa"
		};
	};
	class TGK_RussianSolider_V3_TOP_CamoJ: TGK_RussianSolider_V3_TOP_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\top\data\T_Jacket_CamoJ_D.paa"
		};
	};
	class TGK_RussianSolider_V3_TOP_CamoK: TGK_RussianSolider_V3_TOP_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\top\data\T_Jacket_CamoK_D.paa"
		};
	};
	class TGK_RussianSolider_V3_TOP_ColorA: TGK_RussianSolider_V3_TOP_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\top\data\T_Jacket_ColorA_D.paa"
		};
	};
	class TGK_RussianSolider_V3_TOP_ColorB: TGK_RussianSolider_V3_TOP_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\top\data\T_Jacket_ColorB_D.paa"
		};
	};
	class TGK_RussianSolider_V3_TOP_ColorC: TGK_RussianSolider_V3_TOP_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\top\data\T_Jacket_ColorC_D.paa"
		};
	};
	class TGK_RussianSolider_V3_TOP_ColorD: TGK_RussianSolider_V3_TOP_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\top\data\T_Jacket_ColorD_D.paa"
		};
	};
	class TGK_RussianSolider_V3_TOP_ColorE: TGK_RussianSolider_V3_TOP_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\top\data\T_Jacket_ColorE_D.paa"
		};
	};
	class TGK_RussianSolider_V3_TOP_ColorF: TGK_RussianSolider_V3_TOP_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\top\data\T_Jacket_ColorF_D.paa"
		};
	};
	class TGK_RussianSolider_V3_TOP_ColorG: TGK_RussianSolider_V3_TOP_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\top\data\T_Jacket_ColorG_D.paa"
		};
	};
	class TGK_RussianSolider_V3_TOP_ColorH: TGK_RussianSolider_V3_TOP_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\top\data\T_Jacket_ColorH_D.paa"
		};
	};
	class TGK_RussianSolider_V3_TOP_ColorL: TGK_RussianSolider_V3_TOP_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\top\data\T_Jacket_ColorI_D.paa"
		};
	};
	class TGK_RussianSolider_V3_TOP_ColorJ: TGK_RussianSolider_V3_TOP_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\top\data\T_Jacket_ColorJ_D.paa"
		};
	};
	class TGK_RussianSolider_V3_TOP_Gorka_ColorA: TGK_RussianSolider_V3_TOP_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\top\data\T_Jacket_Gorka_ColorA_D.paa"
		};
	};
	class TGK_RussianSolider_V3_TOP_Gorka_ColorC: TGK_RussianSolider_V3_TOP_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\top\data\T_Jacket_Gorka_ColorC_D.paa"
		};
	};
	class TGK_RussianSolider_V3_TOP_Gorka_ColorB: TGK_RussianSolider_V3_TOP_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\top\data\T_Jacket_Gorka_ColorB_D.paa"
		};
	};
	class TGK_RussianSolider_V3_TOP_Gorka_ColorE: TGK_RussianSolider_V3_TOP_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\top\data\T_Jacket_Gorka_ColorE_D.paa"
		};
	};
	class TGK_RussianSolider_V3_TOP_Gorka_ColorF: TGK_RussianSolider_V3_TOP_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\top\data\T_Jacket_Gorka_ColorF_D.paa"
		};
	};
	class TGK_RussianSolider_V3_TOP_Gorka_ColorG: TGK_RussianSolider_V3_TOP_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\top\data\T_Jacket_Gorka_ColorG_D.paa"
		};
	};
	class TGK_RussianSolider_V3_TOP_Gorka_ColorH: TGK_RussianSolider_V3_TOP_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\top\data\T_Jacket_Gorka_ColorH_D.paa"
		};
	};
	class TGK_RussianSolider_V3_TOP_Gorka_ColorL: TGK_RussianSolider_V3_TOP_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\top\data\T_Jacket_Gorka_ColorI_D.paa"
		};
	};
	class TGK_RussianSolider_V3_TOP_Gorka_ColorJ: TGK_RussianSolider_V3_TOP_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\top\data\T_Jacket_Gorka_ColorJ_D.paa"
		};
	};
	class TGK_RussianSolider_V3_TOP_Gorka_ColorK: TGK_RussianSolider_V3_TOP_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\top\data\T_Jacket_Gorka_ColorK_D.paa"
		};
	};
	class TGK_RussianSolider_V3_TOP_Gorka_ColorW: TGK_RussianSolider_V3_TOP_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\top\data\T_Jacket_Gorka_D.paa"
		};
	};
};
