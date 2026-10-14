class CfgPatches
{
	class Vp_SET15
	{
		units[]=
		{
			"Vp_SET15_Vest_Black"
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
	class Vp_SET15_Vest_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_Elegant_Vest";
		descriptionShort="";
		model="set_Elegant\set\Vest\model\vest_g.p3d";
		attachments[]=
		{
			"pistol"
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
		weight=3000;
		itemSize[]={4,4};
		varWetMax=0.050000001;
		heatIsolation=0.5;
		repairableWithKits[]={3,8};
		repairCosts[]={75,50};
		hiddenSelections[]=
		{
			"karman",
			"magaz_vest",
			"planshet",
			"vest"
		};
		class ClothingTypes
		{
			male="set_Elegant\set\Vest\model\vest_mf.p3d";
			female="set_Elegant\set\Vest\model\vest_mf.p3d";
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
								"set_Elegant\set\Vest\data\karman.rvmat",
								"set_Elegant\set\Vest\data\Magaz.rvmat",
								"set_Elegant\set\Vest\data\planshet.rvmat",
								"set_Elegant\set\Vest\data\vest.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"set_Elegant\set\Vest\data\karman.rvmat",
								"set_Elegant\set\Vest\data\Magaz.rvmat",
								"set_Elegant\set\Vest\data\planshet.rvmat",
								"set_Elegant\set\Vest\data\vest.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"set_Elegant\set\Vest\data\karman_damage.rvmat",
								"set_Elegant\set\Vest\data\Magaz_damage.rvmat",
								"set_Elegant\set\Vest\data\planshet_damage.rvmat",
								"set_Elegant\set\Vest\data\vest_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"set_Elegant\set\Vest\data\karman_damage.rvmat",
								"set_Elegant\set\Vest\data\Magaz_damage.rvmat",
								"set_Elegant\set\Vest\data\planshet_damage.rvmat",
								"set_Elegant\set\Vest\data\vest_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"set_Elegant\set\Vest\data\karman_destruct.rvmat",
								"set_Elegant\set\Vest\data\Magaz_destruct.rvmat",
								"set_Elegant\set\Vest\data\planshet_destruct.rvmat",
								"set_Elegant\set\Vest\data\vest_destruct.rvmat"
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
	class Vp_SET15_Vest_Black: Vp_SET15_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"set_Elegant\set\Vest\data\karman_vest.paa",
			"set_Elegant\set\Vest\data\Magaz_vest.paa",
			"set_Elegant\set\Vest\data\planshet_co.paa",
			"set_Elegant\set\Vest\data\vest_co.paa"
		};
	};
};
