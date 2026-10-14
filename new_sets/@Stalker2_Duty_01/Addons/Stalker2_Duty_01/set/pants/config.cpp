class CfgPatches
{
	class S2_Duty_01
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
	class Stalker_2_Duty_01_Pants_Colorbase: GorkaPants_ColorBase
	{
		scope=0;
		displayName="$STR_Duty_01_Pants_01";
		descriptionShort="$STR_Duty_01_Pants_02";
		model="Stalker2_Duty_01\set\pants\models\Pants_g.p3d";
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
		attachments[]=
		{
			"Leg_small_pouches_a",
			"Leg_small_pouches_b",
			"MapPouch",
			"Small_Pouch",
			"Small_Pouch_System",
			"VestGrenadeA",
			"VestGrenadeB"
		};
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
			"personality"
		};
		rotationFlags=17;
		class ClothingTypes
		{
			male="Stalker2_Duty_01\set\pants\models\Pants_m.p3d";
			female="Stalker2_Duty_01\set\pants\models\Pants_m.p3d";
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
								"Stalker2_Duty_01\set\pants\data\pants.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"Stalker2_Duty_01\set\pants\data\pants.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"Stalker2_Duty_01\set\pants\data\pants_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"Stalker2_Duty_01\set\pants\data\pants_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"Stalker2_Duty_01\set\pants\data\pants_destruct.rvmat"
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
	class Stalker_2_Duty_01_Pants: Stalker_2_Duty_01_Pants_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\pants\data\pants_co.paa"
		};
	};
	class Stalker_2_Duty_01_Pants_ClearSky: Stalker_2_Duty_01_Pants_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\pants\data\pants_ClearSky.paa"
		};
	};
	class Stalker_2_Duty_01_Pants_Korpus: Stalker_2_Duty_01_Pants_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\pants\data\pants_Korpus.paa"
		};
	};
	class Stalker_2_Duty_01_Pants_Monolith: Stalker_2_Duty_01_Pants_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\pants\data\pants_Monolith.paa"
		};
	};
	class Stalker_2_Duty_01_Pants_MSOP: Stalker_2_Duty_01_Pants_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\pants\data\pants_MSOP.paa"
		};
	};
	class Stalker_2_Duty_01_Pants_Warta: Stalker_2_Duty_01_Pants_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\pants\data\pants_Warta.paa"
		};
	};
};
