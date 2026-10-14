class CfgPatches
{
	class Vp_SET18
	{
		units[]=
		{
			"Vp_SET18_Vest_Black"
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
	class Vp_SET18_Vest_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_set_bat_baby_Vest";
		descriptionShort="";
		model="\set_bat_baby\set\vest\model\vest_g.p3d";
		attachments[]=
		{
			"Magazine",
			"Magazine2",
			"Magazine3"
		};
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
		weight=10000;
		itemSize[]={4,4};
		varWetMax=0.050000001;
		heatIsolation=1;
		itemsCargoSize[]={10,4};
		repairableWithKits[]={3,8};
		repairCosts[]={75,50};
		hiddenSelections[]=
		{
			"vest1",
			"vest2",
			"vest3",
			"vest4",
			"vest5",
			"vest6",
			"vest7",
			"vest8"
		};
		class ClothingTypes
		{
			male="\set_bat_baby\set\vest\model\vest_mf.p3d";
			female="\set_bat_baby\set\vest\model\vest_mf.p3d";
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
								"\set_bat_baby\set\vest\data\vest1.rvmat",
								"\set_bat_baby\set\vest\data\vest2.rvmat",
								"\set_bat_baby\set\vest\data\vest3.rvmat",
								"\set_bat_baby\set\vest\data\vest4.rvmat",
								"\set_bat_baby\set\vest\data\vest5.rvmat",
								"\set_bat_baby\set\vest\data\vest6.rvmat",
								"\set_bat_baby\set\vest\data\vest7.rvmat",
								"\set_bat_baby\set\vest\data\vest8.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"\set_bat_baby\set\vest\data\vest1.rvmat",
								"\set_bat_baby\set\vest\data\vest2.rvmat",
								"\set_bat_baby\set\vest\data\vest3.rvmat",
								"\set_bat_baby\set\vest\data\vest4.rvmat",
								"\set_bat_baby\set\vest\data\vest5.rvmat",
								"\set_bat_baby\set\vest\data\vest6.rvmat",
								"\set_bat_baby\set\vest\data\vest7.rvmat",
								"\set_bat_baby\set\vest\data\vest8.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"\set_bat_baby\set\vest\data\vest1_damage.rvmat",
								"\set_bat_baby\set\vest\data\vest2_damage.rvmat",
								"\set_bat_baby\set\vest\data\vest3_damage.rvmat",
								"\set_bat_baby\set\vest\data\vest4_damage.rvmat",
								"\set_bat_baby\set\vest\data\vest5_damage.rvmat",
								"\set_bat_baby\set\vest\data\vest6_damage.rvmat",
								"\set_bat_baby\set\vest\data\vest7_damage.rvmat",
								"\set_bat_baby\set\vest\data\vest8_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"\set_bat_baby\set\vest\data\vest1_damage.rvmat",
								"\set_bat_baby\set\vest\data\vest2_damage.rvmat",
								"\set_bat_baby\set\vest\data\vest3_damage.rvmat",
								"\set_bat_baby\set\vest\data\vest4_damage.rvmat",
								"\set_bat_baby\set\vest\data\vest5_damage.rvmat",
								"\set_bat_baby\set\vest\data\vest6_damage.rvmat",
								"\set_bat_baby\set\vest\data\vest7_damage.rvmat",
								"\set_bat_baby\set\vest\data\vest8_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"\set_bat_baby\set\vest\data\vest1_destruct.rvmat",
								"\set_bat_baby\set\vest\data\vest2_destruct.rvmat",
								"\set_bat_baby\set\vest\data\vest3_destruct.rvmat",
								"\set_bat_baby\set\vest\data\vest4_destruct.rvmat",
								"\set_bat_baby\set\vest\data\vest5_destruct.rvmat",
								"\set_bat_baby\set\vest\data\vest6_destruct.rvmat",
								"\set_bat_baby\set\vest\data\vest7_destruct.rvmat",
								"\set_bat_baby\set\vest\data\vest8_destruct.rvmat"
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
						damage=0.10000001;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.10000002;
					};
				};
				class Melee
				{
					class Health
					{
						damage=0.1;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.1;
					};
				};
				class Infected
				{
					class Health
					{
						damage=0.050000001;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.0099999998;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage=0.1;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.25999999;
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
	class Vp_SET18_Vest_Black: Vp_SET18_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"\set_bat_baby\set\vest\data\vest1_co.paa",
			"\set_bat_baby\set\vest\data\vest2_co.paa",
			"\set_bat_baby\set\vest\data\vest3_co.paa",
			"\set_bat_baby\set\vest\data\vest4_co.paa",
			"\set_bat_baby\set\vest\data\vest5_co.paa",
			"\set_bat_baby\set\vest\data\vest6_co.paa",
			"\set_bat_baby\set\vest\data\vest7_co.paa",
			"\set_bat_baby\set\vest\data\vest8_co.paa"
		};
	};
};
