class CfgPatches
{
	class TGK_Casual_Boots
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Characters",
			"DZ_Characters_Shoes"
		};
	};
};
class CfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class TGK_Casual_Boots_ColorBase: Clothing
	{
		displayName="$STR_Casual_Boots01";
		descriptionShort="$STR_Casual_Boots02";
		model="TGK_CasualSoldier\set\Boots\models\Boots_g.p3d";
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
		class ClothingTypes
		{
			male="TGK_CasualSoldier\set\Boots\models\Boots_m.p3d";
			female="TGK_CasualSoldier\set\Boots\models\Boots_m.p3d";
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
								"TGK_CasualSoldier\set\Boots\data\Salomon.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_CasualSoldier\set\Boots\data\Salomon.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_CasualSoldier\set\Boots\data\Salomon_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_CasualSoldier\set\Boots\data\Salomon_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_CasualSoldier\set\Boots\data\Salomon_destruct.rvmat"
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
	class TGK_Casual_Boots_Black: TGK_Casual_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_CasualSoldier\set\Boots\data\Salomon_Black.paa"
		};
	};
	class TGK_Casual_Boots_Gray: TGK_Casual_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_CasualSoldier\set\Boots\data\Salomon_co.paa"
		};
	};
	class TGK_Casual_Boots_Tan: TGK_Casual_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_CasualSoldier\set\Boots\data\Salomon_Tan.paa"
		};
	};
};
