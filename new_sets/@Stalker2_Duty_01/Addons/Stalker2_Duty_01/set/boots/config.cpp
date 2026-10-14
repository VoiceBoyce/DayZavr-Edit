class CfgPatches
{
	class S2_Duty_01
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
	class Stalker_2_Duty_01_Boots_Colorbase: CombatBoots_ColorBase
	{
		displayName="$STR_Duty_01_Boots_01";
		descriptionShort="$STR_Duty_01_Boots_02";
		model="Stalker2_Duty_01\set\boots\Boots_g.p3d";
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
			male="Stalker2_Duty_01\set\boots\Boots_m.p3d";
			female="Stalker2_Duty_01\set\boots\Boots_m.p3d";
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
								"Stalker2_Duty_01\set\top\data\Top.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"Stalker2_Duty_01\set\top\data\Top.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"Stalker2_Duty_01\set\top\data\Top_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"Stalker2_Duty_01\set\top\data\Top_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"Stalker2_Duty_01\set\top\data\Top_destruct.rvmat"
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
	class Stalker_2_Duty_01_Boots: Stalker_2_Duty_01_Boots_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\top\data\Top_co.paa"
		};
	};
	class Stalker_2_Duty_01_Boots_ClearSky: Stalker_2_Duty_01_Boots_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\top\data\Top_ClearSky.paa"
		};
	};
	class Stalker_2_Duty_01_Boots_Korpus: Stalker_2_Duty_01_Boots_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\top\data\Top_Korpus.paa"
		};
	};
	class Stalker_2_Duty_01_Boots_Monolith: Stalker_2_Duty_01_Boots_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\top\data\Top_Monolith.paa"
		};
	};
	class Stalker_2_Duty_01_Boots_MSOP: Stalker_2_Duty_01_Boots_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\top\data\Top_MSOP.paa"
		};
	};
	class Stalker_2_Duty_01_Boots_Warta: Stalker_2_Duty_01_Boots_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\top\data\Top_Warta.paa"
		};
	};
};
