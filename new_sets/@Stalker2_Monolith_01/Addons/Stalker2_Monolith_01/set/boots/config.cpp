class CfgPatches
{
	class Stalker_2_Monolith_Balaclava
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
	class Stalker_2_Monolith_Boots_ColorBase: CombatBoots_ColorBase
	{
		displayName="$STR_Stalker_2_Monolith_Boots_01";
		descriptionShort="$STR_Stalker_2_Monolith_Boots_02";
		model="Stalker2_Monolith_01\set\boots\Boots_g.p3d";
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
			male="Stalker2_Monolith_01\set\boots\Boots_m.p3d";
			female="Stalker2_Monolith_01\set\boots\Boots_m.p3d";
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
								"Stalker2_Monolith_01\set\data\Monolith.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"Stalker2_Monolith_01\set\data\Monolith.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"Stalker2_Monolith_01\set\data\Monolith_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"Stalker2_Monolith_01\set\data\Monolith_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"Stalker2_Monolith_01\set\data\Monolith_destruct.rvmat"
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
	class S2_Monolith_01_Boots: Stalker_2_Monolith_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\data\Monolith_co.paa"
		};
	};
	class S2_Monolith_01_Boots_ClearSky: Stalker_2_Monolith_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\data\Monolith_ClearSky.paa"
		};
	};
	class S2_Monolith_01_Boots_Duty: Stalker_2_Monolith_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\data\Monolith_duty.paa"
		};
	};
	class S2_Monolith_01_Boots_Freedom: Stalker_2_Monolith_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\data\Monolith_Freedom.paa"
		};
	};
	class S2_Monolith_01_Boots_Korpus: Stalker_2_Monolith_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\data\Monolith_Korpus.paa"
		};
	};
	class S2_Monolith_01_Boots_Merc: Stalker_2_Monolith_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\data\Monolith_Merc.paa"
		};
	};
	class S2_Monolith_01_Boots_MSOP: Stalker_2_Monolith_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\data\Monolith_MSOP.paa"
		};
	};
	class S2_Monolith_01_Boots_Neutrals: Stalker_2_Monolith_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\data\Monolith_Neutrals.paa"
		};
	};
	class S2_Monolith_01_Boots_Warta: Stalker_2_Monolith_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\data\Monolith_Warta.paa"
		};
	};
};
