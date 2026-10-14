class CfgPatches
{
	class TGK_RussianSolider_V3
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Characters",
			"DZ_Characters_Shoes",
			"DZ_Scripts"
		};
	};
};
class CfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class CombatBoots_ColorBase;
	class TGK_RussianSolider_V3_Boots_ColorBase: CombatBoots_ColorBase
	{
		displayName="$STR_TGK_RussianSolider_V3_Boots_01";
		descriptionShort="$STR_TGK_RussianSolider_V3_Boots_02";
		model="TGK_RussianSoldier_V3\set\boots\models\Boots_g.p3d";
		inventorySlot[]=
		{
			"Feet"
		};
		itemInfo[]=
		{
			"Clothing",
			"Feet"
		};
		itemSize[]={4,2};
		weight=1200;
		durability=0.34999999;
		varWetMax=0.25;
		heatIsolation=0.64999998;
		visibilityModifier=0.89999998;
		repairableWithKits[]={3};
		repairCosts[]={25};
		soundAttType="Sneakers";
		soundImpactType="default";
		hiddenSelections[]=
		{
			"camo"
		};
		rotationFlags=16;
		class ClothingTypes
		{
			male="TGK_RussianSoldier_V3\set\boots\models\Boots_m.p3d";
			female="TGK_RussianSoldier_V3\set\boots\models\Boots_m.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=500;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"TGK_RussianSoldier_V3\set\boots\data\Boots.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_RussianSoldier_V3\set\boots\data\Boots.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_RussianSoldier_V3\set\boots\data\Boots_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_RussianSoldier_V3\set\boots\data\Boots_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_RussianSoldier_V3\set\boots\data\Boots_destruct.rvmat"
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
					soundSet="AthleticShoes_pickup_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="AthleticShoes_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class TGK_RussianSolider_V3_Boots_BlackA: TGK_RussianSolider_V3_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\boots\data\Boots_Black.paa"
		};
	};
	class TGK_RussianSolider_V3_Boots_BlackB: TGK_RussianSolider_V3_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\boots\data\Boots_Black1.paa"
		};
	};
	class TGK_RussianSolider_V3_Boots_Blue: TGK_RussianSolider_V3_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\boots\data\Boots_Blue.paa"
		};
	};
	class TGK_RussianSolider_V3_Boots_Gray: TGK_RussianSolider_V3_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\boots\data\Boots_Gray.paa"
		};
	};
	class TGK_RussianSolider_V3_Boots_GreenA: TGK_RussianSolider_V3_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\boots\data\Boots_Green1.paa"
		};
	};
	class TGK_RussianSolider_V3_Boots_GreenB: TGK_RussianSolider_V3_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\boots\data\Boots_Green2.paa"
		};
	};
	class TGK_RussianSolider_V3_Boots_GreenC: TGK_RussianSolider_V3_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\boots\data\Boots_Green3.paa"
		};
	};
	class TGK_RussianSolider_V3_Boots_Orange: TGK_RussianSolider_V3_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\boots\data\Boots_Orange.paa"
		};
	};
	class TGK_RussianSolider_V3_Boots_TanA: TGK_RussianSolider_V3_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\boots\data\Boots_Tan1.paa"
		};
	};
	class TGK_RussianSolider_V3_Boots_TanB: TGK_RussianSolider_V3_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\boots\data\Boots_Tan2.paa"
		};
	};
	class TGK_RussianSolider_V3_Boots_TanC: TGK_RussianSolider_V3_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\boots\data\Boots_Tan3.paa"
		};
	};
	class TGK_RussianSolider_V3_Boots_TanD: TGK_RussianSolider_V3_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V3\set\boots\data\Boots_Tan4.paa"
		};
	};
};
