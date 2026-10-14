class CfgPatches
{
	class Vp_SET16
	{
		units[]=
		{
			"Vp_SET16_Vest_Black"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters",
			"DZ_Characters_Vests"
		};
	};
};
class cfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class Vp_SET16_Vest_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_Heavy_Vest";
		descriptionShort="";
		model="set_heavy\set\Vest\model\vest_g.p3d";
		attachments[]={};
		inventorySlot[]=
		{
			"Vest"
		};
		itemInfo[]=
		{
			"Clothing",
			"Vest"
		};
		quickBarBonus=2;
		weight=3000;
		itemSize[]={4,4};
		varWetMax=0.050000001;
		heatIsolation=0.5;
		itemsCargoSize[]={10,3};
		repairableWithKits[]={3,8};
		repairCosts[]={75,50};
		hiddenSelections[]=
		{
			"karabin",
			"pah_vest",
			"vest",
			"vest_relief",
			"vorotnik_vest"
		};
		class ClothingTypes
		{
			male="set_heavy\set\Vest\model\vest_mf.p3d";
			female="set_heavy\set\Vest\model\vest_mf.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=1000;
					transferToAttachmentsCoef=0.5;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"set_heavy\set\Vest\data\karabin.rvmat",
								"set_heavy\set\Vest\data\pah.rvmat",
								"set_heavy\set\Vest\data\vest.rvmat",
								"set_heavy\set\Vest\data\vest_relief.rvmat",
								"set_heavy\set\Vest\data\vorotnik.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"set_heavy\set\Vest\data\karabin.rvmat",
								"set_heavy\set\Vest\data\pah.rvmat",
								"set_heavy\set\Vest\data\vest.rvmat",
								"set_heavy\set\Vest\data\vest_relief.rvmat",
								"set_heavy\set\Vest\data\vorotnik.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"set_heavy\set\Vest\data\karabin_damage.rvmat",
								"set_heavy\set\Vest\data\pah_damage.rvmat",
								"set_heavy\set\Vest\data\vest_damage.rvmat",
								"set_heavy\set\Vest\data\vest_relief_damage.rvmat",
								"set_heavy\set\Vest\data\vorotnik_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"set_heavy\set\Vest\data\karabin_damage.rvmat",
								"set_heavy\set\Vest\data\pah_damage.rvmat",
								"set_heavy\set\Vest\data\vest_damage.rvmat",
								"set_heavy\set\Vest\data\vest_relief_damage.rvmat",
								"set_heavy\set\Vest\data\vorotnik_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"set_heavy\set\Vest\data\karabin_destruct.rvmat",
								"set_heavy\set\Vest\data\pah_destruct.rvmat",
								"set_heavy\set\Vest\data\vest_destruct.rvmat",
								"set_heavy\set\Vest\data\vest_relief_destruct.rvmat",
								"set_heavy\set\Vest\data\vorotnik_destruct.rvmat"
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
						damage=0.2;
					};
					class Blood
					{
						damage=0;
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
						damage=0.2;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0;
					};
				};
				class Infected
				{
					class Health
					{
						damage=0.2;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage=0.2;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0;
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
	class Vp_SET16_Vest_Black: Vp_SET16_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"set_heavy\set\Vest\data\karabin.paa",
			"set_heavy\set\Vest\data\pah_vest.paa",
			"set_heavy\set\Vest\data\vest.paa",
			"set_heavy\set\Vest\data\vest_relief.paa",
			"set_heavy\set\Vest\data\vorotnik_vest.paa"
		};
	};
};
