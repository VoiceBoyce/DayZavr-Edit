class CfgPatches
{
	class Stalker_2_Monolith_Helmet
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters",
			"DZ_Scripts",
			"DZ_Characters_Headgear"
		};
	};
};
class cfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class Mich2001Helmet;
	class Stalker_2_Monolith_Helmet_Colorbase: Mich2001Helmet
	{
		scope=0;
		displayName="$STR_Stalker_2_Monolith_Helmet_01";
		descriptionShort="$STR_Stalker_2_Monolith_Helmet_02";
		model="Stalker2_Monolith_01\set\helmet\models\Helmet_g.p3d";
		repairableWithKits[]={8};
		repairCosts[]={25};
		inventorySlot[]=
		{
			"Headgear",
			"Helmet"
		};
		attachments[]={};
		simulation="clothing";
		vehicleClass="Clothing";
		itemInfo[]=
		{
			"Clothing",
			"Headgear"
		};
		weight=1300;
		quickBarBonus=0;
		itemSize[]={4,4};
		noMask=0;
		noNVStrap=1;
		noGlasses=1;
		varWetMax=0.1;
		heatIsolation=0.34999999;
		headSelectionsToHide[]=
		{
			"Clipping_Mich2001"
		};
		hiddenSelections[]=
		{
			"camo",
			"glass"
		};
		rotationFlags=8;
		class ClothingTypes
		{
			male="Stalker2_Monolith_01\set\helmet\models\Helmet_m.p3d";
			female="Stalker2_Monolith_01\set\helmet\models\Helmet_m.p3d";
		};
		class Protection
		{
			biological=1;
			chemical=1;
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
								"Stalker2_Monolith_01\set\helmet\data\Helmet.rvmat",
								"Stalker2_Monolith_01\set\helmet\data\glass\Helmet.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"Stalker2_Monolith_01\set\helmet\data\Helmet.rvmat",
								"Stalker2_Monolith_01\set\helmet\data\glass\Helmet.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"Stalker2_Monolith_01\set\helmet\data\Helmet_damage.rvmat",
								"Stalker2_Monolith_01\set\helmet\data\glass\Helmet_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"Stalker2_Monolith_01\set\helmet\data\Helmet_damage.rvmat",
								"Stalker2_Monolith_01\set\helmet\data\glass\Helmet_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"Stalker2_Monolith_01\set\helmet\data\Helmet_destruct.rvmat",
								"Stalker2_Monolith_01\set\helmet\data\glass\Helmet_destruct.rvmat"
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
						damage=0.31;
					};
					class Blood
					{
						damage=0.31;
					};
					class Shock
					{
						damage=0.31;
					};
				};
				class Melee
				{
					class Health
					{
						damage=0.31;
					};
					class Blood
					{
						damage=0.31;
					};
					class Shock
					{
						damage=0.31;
					};
				};
				class Infected
				{
					class Health
					{
						damage=0.31;
					};
					class Blood
					{
						damage=0.31;
					};
					class Shock
					{
						damage=0.31;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage=0.31;
					};
					class Blood
					{
						damage=0.31;
					};
					class Shock
					{
						damage=0.31;
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
					soundSet="pickUpPot_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="BallisticHelmet_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class S2_Monolith_01_Helmet: Stalker_2_Monolith_Helmet_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\helmet\data\Helmet_ca.paa",
			"Stalker2_Monolith_01\set\helmet\data\Helmet_ca.paa"
		};
	};
	class S2_Monolith_01_Helmet_ClearSky: Stalker_2_Monolith_Helmet_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\helmet\data\Helmet_ClearSky.paa",
			"Stalker2_Monolith_01\set\helmet\data\Helmet_ca.paa"
		};
	};
	class S2_Monolith_01_Helmet_Duty: Stalker_2_Monolith_Helmet_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\helmet\data\Helmet_Duty.paa",
			"Stalker2_Monolith_01\set\helmet\data\Helmet_ca.paa"
		};
	};
	class S2_Monolith_01_Helmet_Freedom: Stalker_2_Monolith_Helmet_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\helmet\data\Helmet_Freedom.paa",
			"Stalker2_Monolith_01\set\helmet\data\Helmet_ca.paa"
		};
	};
	class S2_Monolith_01_Helmet_Korpus: Stalker_2_Monolith_Helmet_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\helmet\data\Helmet_Korpus.paa",
			"Stalker2_Monolith_01\set\helmet\data\Helmet_ca.paa"
		};
	};
	class S2_Monolith_01_Helmet_Merc: Stalker_2_Monolith_Helmet_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\helmet\data\Helmet_Merc.paa",
			"Stalker2_Monolith_01\set\helmet\data\Helmet_ca.paa"
		};
	};
	class S2_Monolith_01_Helmet_MSOP: Stalker_2_Monolith_Helmet_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\helmet\data\Helmet_MSOP.paa",
			"Stalker2_Monolith_01\set\helmet\data\Helmet_ca.paa"
		};
	};
	class S2_Monolith_01_Helmet_Neutrals: Stalker_2_Monolith_Helmet_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\helmet\data\Helmet_Neutrals.paa",
			"Stalker2_Monolith_01\set\helmet\data\Helmet_ca.paa"
		};
	};
	class S2_Monolith_01_Helmet_Warta1: Stalker_2_Monolith_Helmet_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\helmet\data\Helmet_Warta1.paa",
			"Stalker2_Monolith_01\set\helmet\data\Helmet_ca.paa"
		};
	};
	class S2_Monolith_01_Helmet_Warta2: Stalker_2_Monolith_Helmet_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\helmet\data\Helmet_Warta2.paa",
			"Stalker2_Monolith_01\set\helmet\data\Helmet_ca.paa"
		};
	};
};
