class CfgPatches
{
	class Vp_SET13
	{
		units[]=
		{
			"Vp_SET13_Vest_Black"
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
	class Vp_SET13_Vest_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_VEST";
		descriptionShort="";
		model="Set_Oni\set\Vest\model\Vest_g.p3d";
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
			"camo",
			"karman",
			"karman_ra",
			"magaz",
			"ra",
			"zalypa1"
		};
		class ClothingTypes
		{
			male="Set_Oni\set\Vest\model\Vest_mf.p3d";
			female="Set_Oni\set\Vest\model\Vest_mf.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=1500;
					transferToAttachmentsCoef=0.5;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"Set_Oni\set\Vest\data\Vest.rvmat",
								"Set_Oni\set\Vest\data\Karman.rvmat",
								"Set_Oni\set\Vest\data\Karman_ra.rvmat",
								"Set_Oni\set\Vest\data\Mag.rvmat",
								"Set_Oni\set\Vest\data\ra.rvmat",
								"Set_Oni\set\Vest\data\zalypa_ra.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"Set_Oni\set\Vest\data\Vest.rvmat",
								"Set_Oni\set\Vest\data\Karman.rvmat",
								"Set_Oni\set\Vest\data\Karman_ra.rvmat",
								"Set_Oni\set\Vest\data\Mag.rvmat",
								"Set_Oni\set\Vest\data\ra.rvmat",
								"Set_Oni\set\Vest\data\zalypa_ra.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"Set_Oni\set\Vest\data\Vest_damage.rvmat",
								"Set_Oni\set\Vest\data\Karman_damage.rvmat",
								"Set_Oni\set\Vest\data\Karman_ra_damage.rvmat",
								"Set_Oni\set\Vest\data\Mag_damage.rvmat",
								"Set_Oni\set\Vest\data\ra_damage.rvmat",
								"Set_Oni\set\Vest\data\zalypa_ra_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"Set_Oni\set\Vest\data\Vest_damage.rvmat",
								"Set_Oni\set\Vest\data\Karman_damage.rvmat",
								"Set_Oni\set\Vest\data\Karman_ra_damage.rvmat",
								"Set_Oni\set\Vest\data\Mag_damage.rvmat",
								"Set_Oni\set\Vest\data\ra_damage.rvmat",
								"Set_Oni\set\Vest\data\zalypa_ra_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"Set_Oni\set\Vest\data\Vest_destruct.rvmat",
								"Set_Oni\set\Vest\data\Karman_destruct.rvmat",
								"Set_Oni\set\Vest\data\Karman_ra_destruct.rvmat",
								"Set_Oni\set\Vest\data\Mag_destruct.rvmat",
								"Set_Oni\set\Vest\data\ra_destruct.rvmat",
								"Set_Oni\set\Vest\data\zalypa_ra_destruct.rvmat"
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
	class Vp_SET13_Vest_Black: Vp_SET13_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Set_Oni\set\Vest\data\Vest_co.paa",
			"Set_Oni\set\Vest\data\Karman_co.paa",
			"Set_Oni\set\Vest\data\Karma_ra_co.paa",
			"Set_Oni\set\Vest\data\Mag_co.paa",
			"Set_Oni\set\Vest\data\ra_co.paa",
			"Set_Oni\set\Vest\data\zalypa_ra_co.paa"
		};
	};
};
