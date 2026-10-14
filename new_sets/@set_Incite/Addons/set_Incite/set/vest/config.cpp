class CfgPatches
{
	class Vp_SET17
	{
		units[]=
		{
			"Vp_SET17_Vest_Black"
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
	class Vp_SET17_Vest_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_set_Incite_Vest";
		descriptionShort="";
		model="\set_Incite\set\vest\model\vest_g.p3d";
		attachments[]=
		{
			"Magazine",
			"Magazine2",
			"Magazine3",
			"Magazine4",
			"Magazine5",
			"Magazine6"
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
		heatIsolation=0.5;
		itemsCargoSize[]={10,4};
		repairableWithKits[]={3,8};
		repairCosts[]={75,50};
		hiddenSelections[]=
		{
			"backpack",
			"karmani_vest",
			"krep_knife_vest",
			"ruka_knief_vest",
			"small_magazi_vest",
			"vest",
			"vest_podushki",
			"vest_relief",
			"vest1"
		};
		class ClothingTypes
		{
			male="\set_Incite\set\vest\model\vest_mf.p3d";
			female="\set_Incite\set\vest\model\vest_mf.p3d";
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
								"\set_Incite\set\vest\data\backpack.rvmat",
								"\set_Incite\set\vest\data\karmani_vest.rvmat",
								"\set_Incite\set\vest\data\krep_knife_vest.rvmat",
								"\set_Incite\set\vest\data\ruka_knief_vest.rvmat",
								"\set_Incite\set\vest\data\small_magazi_vest.rvmat",
								"\set_Incite\set\vest\data\vest.rvmat",
								"\set_Incite\set\vest\data\vest_podushki.rvmat",
								"\set_Incite\set\vest\data\vest_relief.rvmat",
								"\set_Incite\set\vest\data\vest1.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"\set_Incite\set\vest\data\backpack.rvmat",
								"\set_Incite\set\vest\data\karmani_vest.rvmat",
								"\set_Incite\set\vest\data\krep_knife_vest.rvmat",
								"\set_Incite\set\vest\data\ruka_knief_vest.rvmat",
								"\set_Incite\set\vest\data\small_magazi_vest.rvmat",
								"\set_Incite\set\vest\data\vest.rvmat",
								"\set_Incite\set\vest\data\vest_podushki.rvmat",
								"\set_Incite\set\vest\data\vest_relief.rvmat",
								"\set_Incite\set\vest\data\vest1.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"\set_Incite\set\vest\data\backpack_damage.rvmat",
								"\set_Incite\set\vest\data\karmani_vest_damage.rvmat",
								"\set_Incite\set\vest\data\krep_knife_vest_damage.rvmat",
								"\set_Incite\set\vest\data\ruka_knief_vest_damage.rvmat",
								"\set_Incite\set\vest\data\small_magazi_vest_damage.rvmat",
								"\set_Incite\set\vest\data\vest_damage.rvmat",
								"\set_Incite\set\vest\data\vest_podushki_damage.rvmat",
								"\set_Incite\set\vest\data\vest_relief_damage.rvmat",
								"\set_Incite\set\vest\data\vest1_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"\set_Incite\set\vest\data\backpack_damage.rvmat",
								"\set_Incite\set\vest\data\karmani_vest_damage.rvmat",
								"\set_Incite\set\vest\data\krep_knife_vest_damage.rvmat",
								"\set_Incite\set\vest\data\ruka_knief_vest_damage.rvmat",
								"\set_Incite\set\vest\data\small_magazi_vest_damage.rvmat",
								"\set_Incite\set\vest\data\vest_damage.rvmat",
								"\set_Incite\set\vest\data\vest_podushki_damage.rvmat",
								"\set_Incite\set\vest\data\vest_relief_damage.rvmat",
								"\set_Incite\set\vest\data\vest1_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"\set_Incite\set\vest\data\backpack_destruct.rvmat",
								"\set_Incite\set\vest\data\karmani_vest_destruct.rvmat",
								"\set_Incite\set\vest\data\krep_knife_vest_destruct.rvmat",
								"\set_Incite\set\vest\data\ruka_knief_vest_destruct.rvmat",
								"\set_Incite\set\vest\data\small_magazi_vest_destruct.rvmat",
								"\set_Incite\set\vest\data\vest_destruct.rvmat",
								"\set_Incite\set\vest\data\vest_podushki_destruct.rvmat",
								"\set_Incite\set\vest\data\vest_relief_destruct.rvmat",
								"\set_Incite\set\vest\data\vest1_destruct.rvmat"
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
	class Vp_SET17_Vest_Black: Vp_SET17_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"\set_Incite\set\vest\data\backpack_co.paa",
			"\set_Incite\set\vest\data\karmani_vest_co.paa",
			"\set_Incite\set\vest\data\krep_knife_vest_co.paa",
			"\set_Incite\set\vest\data\ruka_knief_vest_co.paa",
			"\set_Incite\set\vest\data\small_magazi_vest_co.paa",
			"\set_Incite\set\vest\data\vest_co.paa",
			"\set_Incite\set\vest\data\vest_podushki_co.paa",
			"\set_Incite\set\vest\data\vest_relief_co.paa",
			"\set_Incite\set\vest\data\vest1_co.paa"
		};
	};
};
