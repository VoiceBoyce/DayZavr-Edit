class CfgPatches
{
	class Stalker_2_Monolith_02_Boots
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
	class Stalker_2_02_Monolith_Boots_ColorBase: CombatBoots_ColorBase
	{
		displayName="$STR_Stalker_2_Monolith_02_Boots_01";
		descriptionShort="$STR_Stalker_2_Monolith_02_Boots_02";
		model="Stalker2_Monolith_02\set\boots\models\boots_g.p3d";
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
			male="Stalker2_Monolith_02\set\boots\models\boots_m.p3d";
			female="Stalker2_Monolith_02\set\boots\models\boots_m.p3d";
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
								"Stalker2_Monolith_02\set\boots\data\Boots.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"Stalker2_Monolith_02\set\boots\data\Boots.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"Stalker2_Monolith_02\set\boots\data\Boots_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"Stalker2_Monolith_02\set\boots\data\Boots_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"Stalker2_Monolith_02\set\boots\data\Boots_destruct.rvmat"
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
	class S2_Monolith_02_Boots: Stalker_2_02_Monolith_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_02\set\boots\data\Shoes_co.paa"
		};
	};
};
