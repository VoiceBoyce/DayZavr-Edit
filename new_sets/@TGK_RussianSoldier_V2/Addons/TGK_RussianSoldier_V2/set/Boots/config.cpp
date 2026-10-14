class CfgPatches
{
	class TGK_RussianSoldier_v2
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
	class TGK_RussianSoldier_v2_Boots_ColorBase: Clothing
	{
		displayName="$STR_RusV2_Boots01";
		descriptionShort="$STR_RusV2_Boots02";
		model="TGK_RussianSoldier_V2\set\Boots\models\Boots_g.p3d";
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
		varWetMax=1;
		heatIsolation=0.80000001;
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
			male="TGK_RussianSoldier_V2\set\Boots\models\Boots_m.p3d";
			female="TGK_RussianSoldier_V2\set\Boots\models\Boots_m.p3d";
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
								"TGK_RussianSoldier_V2\set\Boots\data\Boots.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_RussianSoldier_V2\set\Boots\data\Boots.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_RussianSoldier_V2\set\Boots\data\Boots_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_RussianSoldier_V2\set\Boots\data\Boots_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_RussianSoldier_V2\set\Boots\data\Boots_destruct.rvmat"
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
	class TGK_RussianSoldier_v2_Boots_Black: TGK_RussianSoldier_v2_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Boots\data\Boots_co.paa"
		};
	};
};
