class CfgPatches
{
	class Vp_SET14
	{
		units[]=
		{
			"Vp_SET14_Vest_Black"
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
	class Vp_SET14_Vest_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_Ghost_Vest";
		descriptionShort="";
		model="set_Ghost\set\Vest\model\Vest_g.p3d";
		attachments[]=
		{
			"magazine",
			"magazine2"
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
		itemsCargoSize[]={10,4};
		repairableWithKits[]={3,8};
		repairCosts[]={75,50};
		hiddenSelections[]=
		{
			"vest1",
			"vest2",
			"vest3",
			"vest4",
			"vest6",
			"vest7",
			"vest10",
			"vest11",
			"vest12",
			"vest13",
			"vest14",
			"vest15",
			"vest16",
			"vest17",
			"vest19",
			"vest20",
			"vest21",
			"vest22",
			"vest23",
			"vest24"
		};
		class ClothingTypes
		{
			male="set_Ghost\set\Vest\model\Vest_mf.p3d";
			female="set_Ghost\set\Vest\model\Vest_mf.p3d";
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
								"set_Ghost\set\Vest\data\Vest1.rvmat",
								"set_Ghost\set\Vest\data\Vest2.rvmat",
								"set_Ghost\set\Vest\data\Vest3.rvmat",
								"set_Ghost\set\Vest\data\Vest4.rvmat",
								"set_Ghost\set\Vest\data\vest6.rvmat",
								"set_Ghost\set\Vest\data\vest7.rvmat",
								"set_Ghost\set\Vest\data\Vest10.rvmat",
								"set_Ghost\set\Vest\data\Vest11.rvmat",
								"set_Ghost\set\Vest\data\Vest12.rvmat",
								"set_Ghost\set\Vest\data\Vest13.rvmat",
								"set_Ghost\set\Vest\data\Vest14.rvmat",
								"set_Ghost\set\Vest\data\Vest15.rvmat",
								"set_Ghost\set\Vest\data\Vest16.rvmat",
								"set_Ghost\set\Vest\data\Vest17.rvmat",
								"set_Ghost\set\Vest\data\Vest19.rvmat",
								"set_Ghost\set\Vest\data\Vest20.rvmat",
								"set_Ghost\set\Vest\data\Vest21.rvmat",
								"set_Ghost\set\Vest\data\Vest22.rvmat",
								"set_Ghost\set\Vest\data\Vest23.rvmat",
								"set_Ghost\set\Vest\data\Vest24.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"set_Ghost\set\Vest\data\Vest1.rvmat",
								"set_Ghost\set\Vest\data\Vest2.rvmat",
								"set_Ghost\set\Vest\data\Vest3.rvmat",
								"set_Ghost\set\Vest\data\Vest4.rvmat",
								"set_Ghost\set\Vest\data\vest6.rvmat",
								"set_Ghost\set\Vest\data\vest7.rvmat",
								"set_Ghost\set\Vest\data\Vest10.rvmat",
								"set_Ghost\set\Vest\data\Vest11.rvmat",
								"set_Ghost\set\Vest\data\Vest12.rvmat",
								"set_Ghost\set\Vest\data\Vest13.rvmat",
								"set_Ghost\set\Vest\data\Vest14.rvmat",
								"set_Ghost\set\Vest\data\Vest15.rvmat",
								"set_Ghost\set\Vest\data\Vest16.rvmat",
								"set_Ghost\set\Vest\data\Vest17.rvmat",
								"set_Ghost\set\Vest\data\Vest19.rvmat",
								"set_Ghost\set\Vest\data\Vest20.rvmat",
								"set_Ghost\set\Vest\data\Vest21.rvmat",
								"set_Ghost\set\Vest\data\Vest22.rvmat",
								"set_Ghost\set\Vest\data\Vest23.rvmat",
								"set_Ghost\set\Vest\data\Vest24.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"set_Ghost\set\Vest\data\Vest1_damage.rvmat",
								"set_Ghost\set\Vest\data\Vest2_damage.rvmat",
								"set_Ghost\set\Vest\data\Vest3_damage.rvmat",
								"set_Ghost\set\Vest\data\Vest4_damage.rvmat",
								"set_Ghost\set\Vest\data\vest6_damage.rvmat",
								"set_Ghost\set\Vest\data\vest7_damage.rvmat",
								"set_Ghost\set\Vest\data\Vest10_damage.rvmat",
								"set_Ghost\set\Vest\data\Vest11_damage.rvmat",
								"set_Ghost\set\Vest\data\Vest12_damage.rvmat",
								"set_Ghost\set\Vest\data\Vest13_damage.rvmat",
								"set_Ghost\set\Vest\data\Vest14_damage.rvmat",
								"set_Ghost\set\Vest\data\Vest15_damage.rvmat",
								"set_Ghost\set\Vest\data\Vest16_damage.rvmat",
								"set_Ghost\set\Vest\data\Vest17_damage.rvmat",
								"set_Ghost\set\Vest\data\Vest19_damage.rvmat",
								"set_Ghost\set\Vest\data\Vest20_damage.rvmat",
								"set_Ghost\set\Vest\data\Vest21_damage.rvmat",
								"set_Ghost\set\Vest\data\Vest22_damage.rvmat",
								"set_Ghost\set\Vest\data\Vest23_damage.rvmat",
								"set_Ghost\set\Vest\data\Vest24_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"set_Ghost\set\Vest\data\Vest1_damage.rvmat",
								"set_Ghost\set\Vest\data\Vest2_damage.rvmat",
								"set_Ghost\set\Vest\data\Vest3_damage.rvmat",
								"set_Ghost\set\Vest\data\Vest4_damage.rvmat",
								"set_Ghost\set\Vest\data\vest6_damage.rvmat",
								"set_Ghost\set\Vest\data\vest7_damage.rvmat",
								"set_Ghost\set\Vest\data\Vest10_damage.rvmat",
								"set_Ghost\set\Vest\data\Vest11_damage.rvmat",
								"set_Ghost\set\Vest\data\Vest12_damage.rvmat",
								"set_Ghost\set\Vest\data\Vest13_damage.rvmat",
								"set_Ghost\set\Vest\data\Vest14_damage.rvmat",
								"set_Ghost\set\Vest\data\Vest15_damage.rvmat",
								"set_Ghost\set\Vest\data\Vest16_damage.rvmat",
								"set_Ghost\set\Vest\data\Vest17_damage.rvmat",
								"set_Ghost\set\Vest\data\Vest19_damage.rvmat",
								"set_Ghost\set\Vest\data\Vest20_damage.rvmat",
								"set_Ghost\set\Vest\data\Vest21_damage.rvmat",
								"set_Ghost\set\Vest\data\Vest22_damage.rvmat",
								"set_Ghost\set\Vest\data\Vest23_damage.rvmat",
								"set_Ghost\set\Vest\data\Vest24_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"set_Ghost\set\Vest\data\Vest1_destruct.rvmat",
								"set_Ghost\set\Vest\data\Vest2_destruct.rvmat",
								"set_Ghost\set\Vest\data\Vest3_destruct.rvmat",
								"set_Ghost\set\Vest\data\Vest4_destruct.rvmat",
								"set_Ghost\set\Vest\data\vest6_destruct.rvmat",
								"set_Ghost\set\Vest\data\vest7_destruct.rvmat",
								"set_Ghost\set\Vest\data\Vest10_destruct.rvmat",
								"set_Ghost\set\Vest\data\Vest11_destruct.rvmat",
								"set_Ghost\set\Vest\data\Vest12_destruct.rvmat",
								"set_Ghost\set\Vest\data\Vest13_destruct.rvmat",
								"set_Ghost\set\Vest\data\Vest14_destruct.rvmat",
								"set_Ghost\set\Vest\data\Vest15_destruct.rvmat",
								"set_Ghost\set\Vest\data\Vest16_destruct.rvmat",
								"set_Ghost\set\Vest\data\Vest17_destruct.rvmat",
								"set_Ghost\set\Vest\data\Vest19_destruct.rvmat",
								"set_Ghost\set\Vest\data\Vest20_destruct.rvmat",
								"set_Ghost\set\Vest\data\Vest21_destruct.rvmat",
								"set_Ghost\set\Vest\data\Vest22_destruct.rvmat",
								"set_Ghost\set\Vest\data\Vest23_destruct.rvmat",
								"set_Ghost\set\Vest\data\Vest24_destruct.rvmat"
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
	class Vp_SET14_Vest_Black: Vp_SET14_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"set_Ghost\set\Vest\data\Vest1_co.paa",
			"set_Ghost\set\Vest\data\Vest2_co.paa",
			"set_Ghost\set\Vest\data\Vest3_co.paa",
			"set_Ghost\set\Vest\data\Vest4_co.paa",
			"set_Ghost\set\Vest\data\vest6_co.paa",
			"set_Ghost\set\Vest\data\vest7_co.paa",
			"set_Ghost\set\Vest\data\Vest10_co.paa",
			"set_Ghost\set\Vest\data\Vest11_co.paa",
			"set_Ghost\set\Vest\data\Vest12_co.paa",
			"set_Ghost\set\Vest\data\Vest13_co.paa",
			"set_Ghost\set\Vest\data\Vest14_co.paa",
			"set_Ghost\set\Vest\data\Vest15_co.paa",
			"set_Ghost\set\Vest\data\Vest16_co.paa",
			"set_Ghost\set\Vest\data\Vest17_co.paa",
			"set_Ghost\set\Vest\data\Vest19_co.paa",
			"set_Ghost\set\Vest\data\Vest20_co.paa",
			"set_Ghost\set\Vest\data\Vest21_co.paa",
			"set_Ghost\set\Vest\data\Vest22_co.paa",
			"set_Ghost\set\Vest\data\Vest23_co.paa",
			"set_Ghost\set\Vest\data\Vest24_co.paa"
		};
	};
};
class CfgSlots
{
	class Slot_Magaz1
	{
		name="Magaz1";
		displayName="Magazin";
		ghostIcon="set:dayz_inventory image:magazine";
		autoAttach="true";
	};
	class Slot_Magaz2
	{
		name="Magaz2";
		displayName="Magazin";
		ghostIcon="set:dayz_inventory image:magazine";
		autoAttach="true";
	};
};
