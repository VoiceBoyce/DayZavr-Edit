class CfgPatches
{
	class TGK_RussianSolider_V4_Pants
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters",
			"DZ_Scripts",
			"DZ_Characters_Pants"
		};
	};
};
class CfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class GorkaPants_ColorBase;
	class TGK_RussianSolider_V4_Pants_ColorBase: GorkaPants_ColorBase
	{
		scope=0;
		displayName="$STR_TGK_RussianSolider_V3_Pants_01";
		descriptionShort="$STR_TGK_RussianSolider_V3_Pants_02";
		model="TGK_RussianSoldier_V4\set\Pants\models\Pants_g.p3d";
		inventorySlot[]=
		{
			"Legs"
		};
		itemInfo[]=
		{
			"Clothing",
			"Legs"
		};
		weight=1000;
		itemSize[]={4,6};
		quickBarBonus=2;
		varWetMax=0.249;
		heatIsolation=0.40000001;
		itemsCargoSize[]={10,5};
		repairableWithKits[]={2,3};
		repairCosts[]={75,50};
		hiddenSelections[]=
		{
			"camo",
			"pads"
		};
		rotationFlags=17;
		class ClothingTypes
		{
			male="TGK_RussianSoldier_V4\set\Pants\models\Pants_m.p3d";
			female="TGK_RussianSoldier_V4\set\Pants\models\Pants_m.p3d";
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
								"TGK_RussianSoldier_V4\set\Pants\data\T_Pants.rvmat",
								"TGK_RussianSoldier_V4\set\Boots\data\Boots.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_RussianSoldier_V4\set\Pants\data\T_Pants.rvmat",
								"TGK_RussianSoldier_V4\set\Boots\data\Boots.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_RussianSoldier_V4\set\Pants\data\T_Pants_damage.rvmat",
								"TGK_RussianSoldier_V4\set\Boots\data\Boots_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_RussianSoldier_V4\set\Pants\data\T_Pants_damage.rvmat",
								"TGK_RussianSoldier_V4\set\Boots\data\Boots_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_RussianSoldier_V4\set\Pants\data\T_Pants_destruct.rvmat",
								"TGK_RussianSoldier_V4\set\Boots\data\Boots_destruct.rvmat"
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
	class TGK_RussianSolider_V4_Pants_CamoA: TGK_RussianSolider_V4_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Pants\data\T_Pants_CamoA_D.paa",
			"TGK_RussianSoldier_V4\set\Boots\data\Boots_Black.paa"
		};
	};
	class TGK_RussianSolider_V4_Pants_CamoB: TGK_RussianSolider_V4_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Pants\data\T_Pants_CamoB_D.paa",
			"TGK_RussianSoldier_V4\set\Boots\data\Boots_Orange.paa"
		};
	};
	class TGK_RussianSolider_V4_Pants_CamoC: TGK_RussianSolider_V4_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Pants\data\T_Pants_CamoC_D.paa",
			"TGK_RussianSoldier_V4\set\Boots\data\Boots_Green2.paa"
		};
	};
	class TGK_RussianSolider_V4_Pants_CamoD: TGK_RussianSolider_V4_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Pants\data\T_Pants_CamoD_D.paa",
			"TGK_RussianSoldier_V4\set\Boots\data\Boots_Green3.paa"
		};
	};
	class TGK_RussianSolider_V4_Pants_CamoE: TGK_RussianSolider_V4_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Pants\data\T_Pants_CamoE_D.paa",
			"TGK_RussianSoldier_V4\set\Boots\data\Boots_Black.paa"
		};
	};
	class TGK_RussianSolider_V4_Pants_CamoF: TGK_RussianSolider_V4_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Pants\data\T_Pants_CamoF_D.paa",
			"TGK_RussianSoldier_V4\set\Boots\data\Boots_Black.paa"
		};
	};
	class TGK_RussianSolider_V4_Pants_CamoG: TGK_RussianSolider_V4_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Pants\data\T_Pants_CamoG_D.paa",
			"TGK_RussianSoldier_V4\set\Boots\data\Boots_Green1.paa"
		};
	};
	class TGK_RussianSolider_V4_Pants_CamoH: TGK_RussianSolider_V4_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Pants\data\T_Pants_CamoH_D.paa",
			"TGK_RussianSoldier_V4\set\Boots\data\Boots_Green2.paa"
		};
	};
	class TGK_RussianSolider_V4_Pants_CamoL: TGK_RussianSolider_V4_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Pants\data\T_Pants_CamoI_D.paa",
			"TGK_RussianSoldier_V4\set\Boots\data\Boots_Green1.paa"
		};
	};
	class TGK_RussianSolider_V4_Pants_CamoJ: TGK_RussianSolider_V4_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Pants\data\T_Pants_CamoJ_D.paa",
			"TGK_RussianSoldier_V4\set\Boots\data\Boots_Orange.paa"
		};
	};
	class TGK_RussianSolider_V4_Pants_ColorA: TGK_RussianSolider_V4_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Pants\data\T_Pants_ColorA_D.paa",
			"TGK_RussianSoldier_V4\set\Boots\data\Boots_Tan1.paa"
		};
	};
	class TGK_RussianSolider_V4_Pants_ColorB: TGK_RussianSolider_V4_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Pants\data\T_Pants_ColorB_D.paa",
			"TGK_RussianSoldier_V4\set\Boots\data\Boots_Tan2.paa"
		};
	};
	class TGK_RussianSolider_V4_Pants_ColorC: TGK_RussianSolider_V4_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Pants\data\T_Pants_ColorC_D.paa",
			"TGK_RussianSoldier_V4\set\Boots\data\Boots_Tan3.paa"
		};
	};
	class TGK_RussianSolider_V4_Pants_ColorD: TGK_RussianSolider_V4_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Pants\data\T_Pants_ColorD_D.paa",
			"TGK_RussianSoldier_V4\set\Boots\data\Boots_Green1.paa"
		};
	};
	class TGK_RussianSolider_V4_Pants_ColorE: TGK_RussianSolider_V4_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Pants\data\T_Pants_ColorE_D.paa",
			"TGK_RussianSoldier_V4\set\Boots\data\Boots_Green2.paa"
		};
	};
	class TGK_RussianSolider_V4_Pants_ColorF: TGK_RussianSolider_V4_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Pants\data\T_Pants_ColorF_D.paa",
			"TGK_RussianSoldier_V4\set\Boots\data\Boots_Green3.paa"
		};
	};
	class TGK_RussianSolider_V4_Pants_ColorG: TGK_RussianSolider_V4_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Pants\data\T_Pants_ColorG_D.paa",
			"TGK_RussianSoldier_V4\set\Boots\data\Boots_Tan4.paa"
		};
	};
	class TGK_RussianSolider_V4_Pants_ColorH: TGK_RussianSolider_V4_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Pants\data\T_Pants_ColorH_D.paa",
			"TGK_RussianSoldier_V4\set\Boots\data\Boots_Orange.paa"
		};
	};
	class TGK_RussianSolider_V4_Pants_ColorL: TGK_RussianSolider_V4_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Pants\data\T_Pants_ColorI_D.paa",
			"TGK_RussianSoldier_V4\set\Boots\data\Boots_Black1.paa"
		};
	};
	class TGK_RussianSolider_V4_Pants_ColorJ: TGK_RussianSolider_V4_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Pants\data\T_Pants_ColorJ_D.paa",
			"TGK_RussianSoldier_V4\set\Boots\data\Boots_Blue.paa"
		};
	};
	class TGK_RussianSolider_V4_Pants_ColorK: TGK_RussianSolider_V4_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Pants\data\T_Pants_ColorK_D.paa",
			"TGK_RussianSoldier_V4\set\Boots\data\Boots_Black.paa"
		};
	};
	class TGK_RussianSolider_V4_Pants_ColorW: TGK_RussianSolider_V4_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Pants\data\T_Pants_D.paa",
			"TGK_RussianSoldier_V4\set\Boots\data\Boots_Gray.paa"
		};
	};
	class TGK_RussianSolider_V4_Pants_Gorka_ColorA: TGK_RussianSolider_V4_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Pants\data\T_Pants_Gorka_ColorA_D.paa",
			"TGK_RussianSoldier_V4\set\Boots\data\Boots_Tan1.paa"
		};
	};
	class TGK_RussianSolider_V4_Pants_Gorka_ColorB: TGK_RussianSolider_V4_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Pants\data\T_Pants_Gorka_ColorB_D.paa",
			"TGK_RussianSoldier_V4\set\Boots\data\Boots_Tan2.paa"
		};
	};
	class TGK_RussianSolider_V4_Pants_Gorka_ColorC: TGK_RussianSolider_V4_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Pants\data\T_Pants_Gorka_ColorC_D.paa",
			"TGK_RussianSoldier_V4\set\Boots\data\Boots_Tan3.paa"
		};
	};
	class TGK_RussianSolider_V4_Pants_Gorka_ColorD: TGK_RussianSolider_V4_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Pants\data\T_Pants_Gorka_ColorD_D.paa",
			"TGK_RussianSoldier_V4\set\Boots\data\Boots_Green1.paa"
		};
	};
	class TGK_RussianSolider_V4_Pants_Gorka_ColorE: TGK_RussianSolider_V4_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Pants\data\T_Pants_Gorka_ColorE_D.paa",
			"TGK_RussianSoldier_V4\set\Boots\data\Boots_Green2.paa"
		};
	};
	class TGK_RussianSolider_V4_Pants_Gorka_ColorF: TGK_RussianSolider_V4_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Pants\data\T_Pants_Gorka_ColorF_D.paa",
			"TGK_RussianSoldier_V4\set\Boots\data\Boots_Green3.paa"
		};
	};
	class TGK_RussianSolider_V4_Pants_Gorka_ColorG: TGK_RussianSolider_V4_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Pants\data\T_Pants_Gorka_ColorG_D.paa",
			"TGK_RussianSoldier_V4\set\Boots\data\Boots_Tan4.paa"
		};
	};
	class TGK_RussianSolider_V4_Pants_Gorka_ColorH: TGK_RussianSolider_V4_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Pants\data\T_Pants_Gorka_ColorH_D.paa",
			"TGK_RussianSoldier_V4\set\Boots\data\Boots_Orange.paa"
		};
	};
	class TGK_RussianSolider_V4_Pants_Gorka_ColorL: TGK_RussianSolider_V4_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Pants\data\T_Pants_Gorka_ColorI_D.paa",
			"TGK_RussianSoldier_V4\set\Boots\data\Boots_Black1.paa"
		};
	};
	class TGK_RussianSolider_V4_Pants_Gorka_ColorJ: TGK_RussianSolider_V4_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Pants\data\T_Pants_Gorka_ColorJ_D.paa",
			"TGK_RussianSoldier_V4\set\Boots\data\Boots_Blue.paa"
		};
	};
	class TGK_RussianSolider_V4_Pants_Gorka_ColorK: TGK_RussianSolider_V4_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V4\set\Pants\data\T_Pants_Gorka_ColorK_D.paa",
			"TGK_RussianSoldier_V4\set\Boots\data\Boots_Black.paa"
		};
	};
};
