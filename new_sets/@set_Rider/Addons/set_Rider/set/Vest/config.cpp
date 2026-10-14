class CfgPatches
{
	class Vp_SET12
	{
		units[]=
		{
			"Vp_SET12_Vest_Black"
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
	class Vp_SET12_Vest_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_Vest";
		descriptionShort="";
		model="set_Rider\set\Vest\Vest_g.p3d";
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
		itemsCargoSize[]={5,5};
		repairableWithKits[]={3,8};
		repairCosts[]={75,50};
		hiddenSelections[]=
		{
			"fast",
			"fast2",
			"karman",
			"karman_ra",
			"monster2",
			"Plita",
			"ra",
			"verevka",
			"Vest"
		};
		class ClothingTypes
		{
			male="set_Rider\set\Vest\Vest_mf.p3d";
			female="set_Rider\set\Vest\Vest_mf.p3d";
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
								"set_Rider\set\Vest\data\fast.rvmat",
								"set_Rider\set\Vest\data\fast2.rvmat",
								"set_Rider\set\Vest\data\Karman.rvmat",
								"set_Rider\set\Vest\data\Karman_ra.rvmat",
								"set_Rider\set\Vest\data\Monster2.rvmat",
								"set_Rider\set\Vest\data\Plita.rvmat",
								"set_Rider\set\Vest\data\ra.rvmat",
								"set_Rider\set\Vest\data\Verevka.rvmat",
								"set_Rider\set\Vest\data\Vest.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"set_Rider\set\Vest\data\fast.rvmat",
								"set_Rider\set\Vest\data\fast2.rvmat",
								"set_Rider\set\Vest\data\Karman.rvmat",
								"set_Rider\set\Vest\data\Karman_ra.rvmat",
								"set_Rider\set\Vest\data\Monster2.rvmat",
								"set_Rider\set\Vest\data\Plita.rvmat",
								"set_Rider\set\Vest\data\ra.rvmat",
								"set_Rider\set\Vest\data\Verevka.rvmat",
								"set_Rider\set\Vest\data\Vest.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"set_Rider\set\Vest\data\fast_damage.rvmat",
								"set_Rider\set\Vest\data\fast2_damage.rvmat",
								"set_Rider\set\Vest\data\Karman_damage.rvmat",
								"set_Rider\set\Vest\data\Karman_ra_damage.rvmat",
								"set_Rider\set\Vest\data\Monster2_damage.rvmat",
								"set_Rider\set\Vest\data\Plita_damage.rvmat",
								"set_Rider\set\Vest\data\ra_damage.rvmat",
								"set_Rider\set\Vest\data\Verevka_damage.rvmat",
								"set_Rider\set\Vest\data\Vest_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"set_Rider\set\Vest\data\fast_damage.rvmat",
								"set_Rider\set\Vest\data\fast2_damage.rvmat",
								"set_Rider\set\Vest\data\Karman_damage.rvmat",
								"set_Rider\set\Vest\data\Karman_ra_damage.rvmat",
								"set_Rider\set\Vest\data\Monster2_damage.rvmat",
								"set_Rider\set\Vest\data\Plita_damage.rvmat",
								"set_Rider\set\Vest\data\ra_damage.rvmat",
								"set_Rider\set\Vest\data\Verevka_damage.rvmat",
								"set_Rider\set\Vest\data\Vest_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"set_Rider\set\Vest\data\fast_destruct.rvmat",
								"set_Rider\set\Vest\data\fast2_destruct.rvmat",
								"set_Rider\set\Vest\data\Karman_destruct.rvmat",
								"set_Rider\set\Vest\data\Karman_ra_destruct.rvmat",
								"set_Rider\set\Vest\data\Monster2_destruct.rvmat",
								"set_Rider\set\Vest\data\Plita_destruct.rvmat",
								"set_Rider\set\Vest\data\ra_destruct.rvmat",
								"set_Rider\set\Vest\data\Verevka_destruct.rvmat",
								"set_Rider\set\Vest\data\Vest_destruct.rvmat"
							}
						},
						"_destruct"
					};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage=0.30000001;
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
						damage=0.30000001;
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
						damage=0.30000001;
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
						damage=0.30000001;
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
	class Vp_SET12_Vest_Black: Vp_SET12_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"set_Rider\set\Vest\data\fast_co.paa",
			"set_Rider\set\Vest\data\fast2_co.paa",
			"set_Rider\set\Vest\data\Karman_co.paa",
			"set_Rider\set\Vest\data\Karman_ra_co.paa",
			"set_Rider\set\Vest\data\Monster2_co.paa",
			"set_Rider\set\Vest\data\Plita_co.paa",
			"set_Rider\set\Vest\data\ra_co.paa",
			"set_Rider\set\Vest\data\Verevka_co.paa",
			"set_Rider\set\Vest\data\Vest_co.paa"
		};
	};
};
