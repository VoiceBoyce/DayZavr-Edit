class CfgPatches
{
	class TGK_S2_Warta
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
	class TGK_S2_WartaBoots_Colorbase: Clothing
	{
		displayName="$STR_WartaBoots_01";
		descriptionShort="$STR_WartaBoots_02";
		model="TGK_S2_Warta\set\shoes\models\Boots_g.p3d";
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
		weight=1000;
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
			male="TGK_S2_Warta\set\shoes\models\Boots_m.p3d";
			female="TGK_S2_Warta\set\shoes\models\Boots_m.p3d";
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
								"TGK_S2_Warta\set\shoes\data\Shoes.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_S2_Warta\set\shoes\data\Shoes.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_S2_Warta\set\shoes\data\Shoes_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_S2_Warta\set\shoes\data\Shoes_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_S2_Warta\set\shoes\data\Shoes_destruct.rvmat"
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
	class TGK_S2_WartaBoots: TGK_S2_WartaBoots_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_S2_Warta\set\shoes\data\Shoes_co.paa"
		};
	};
};
