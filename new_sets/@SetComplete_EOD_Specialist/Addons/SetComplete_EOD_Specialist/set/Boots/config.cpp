class CfgPatches
{
	class TGK_SetComplete_EOD_Specialist
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
	class TGK_SetComplete_EOD_Specialist_Boots_Colorbase: Clothing
	{
		displayName="$STR_EOD_Specialist_Boots01";
		descriptionShort="$STR_EOD_Specialist_Boots02";
		model="SetComplete_EOD_Specialist\set\Boots\Boots_g.p3d";
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
			male="SetComplete_EOD_Specialist\set\Boots\Boots_m.p3d";
			female="SetComplete_EOD_Specialist\set\Boots\Boots_m.p3d";
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
								"SetComplete_EOD_Specialist\set\data\boots\Boots.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"SetComplete_EOD_Specialist\set\data\boots\Boots.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"SetComplete_EOD_Specialist\set\data\boots\Boots_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"SetComplete_EOD_Specialist\set\data\boots\Boots_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"SetComplete_EOD_Specialist\set\data\boots\Boots_destruct.rvmat"
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
	class TGK_EOD_Specialist_Boots: TGK_SetComplete_EOD_Specialist_Boots_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"SetComplete_EOD_Specialist\set\data\boots\Boots_co.paa"
		};
	};
};
