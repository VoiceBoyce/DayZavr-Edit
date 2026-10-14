class CfgPatches
{
	class SM_VP_Multi_VestBag_Set
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data"
		};
	};
};
class cfgVehicles
{
	class Clothing;
	class SM_VP_Multi_Vest_ColorBase: Clothing
	{
		scope=0;
		displayName="Бронежилет Мульти";
		descriptionShort="Бронежилет с рюкзаком от сета Мульти.";
		model="set_mulit\vest\model\vest_g.p3d";
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
		noUNLVest=0;
		weight=3000;
		itemSize[]={4,4};
		itemsCargoSize[]={8,5};
		varWetMax=0.050000001;
		heatIsolation=0.5;
		repairableWithKits[]={3,8};
		repairCosts[]={75,50};
		hiddenSelections[]=
		{
			"black",
			"camo",
			"karman_left",
			"karman_right",
			"magaz_big",
			"magaz_small",
			"palka",
			"pipka",
			"Plita",
			"radio",
			"spina",
			"zalypa1",
			"zalypa2"
		};
		class ClothingTypes
		{
			male="set_mulit\vest\model\vest_m.p3d";
			female="set_mulit\vest\model\vest_f.p3d";
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
								"set_mulit\vest\data\back.rvmat",
								"set_mulit\vest\data\vest.rvmat",
								"set_mulit\vest\data\karman_left.rvmat",
								"set_mulit\vest\data\karman_right.rvmat",
								"set_mulit\vest\data\magaz_big.rvmat",
								"set_mulit\vest\data\small_mag.rvmat",
								"set_mulit\vest\data\palka.rvmat",
								"set_mulit\vest\data\pipka.rvmat",
								"set_mulit\vest\data\plita.rvmat",
								"set_mulit\vest\data\radio.rvmat",
								"set_mulit\vest\data\spine.rvmat",
								"set_mulit\vest\data\zalypa1.rvmat",
								"set_mulit\vest\data\zalypa2.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"set_mulit\vest\data\back.rvmat",
								"set_mulit\vest\data\vest.rvmat",
								"set_mulit\vest\data\karman_left.rvmat",
								"set_mulit\vest\data\karman_right.rvmat",
								"set_mulit\vest\data\magaz_big.rvmat",
								"set_mulit\vest\data\small_mag.rvmat",
								"set_mulit\vest\data\palka.rvmat",
								"set_mulit\vest\data\pipka.rvmat",
								"set_mulit\vest\data\plita.rvmat",
								"set_mulit\vest\data\radio.rvmat",
								"set_mulit\vest\data\spine.rvmat",
								"set_mulit\vest\data\zalypa1.rvmat",
								"set_mulit\vest\data\zalypa2.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"set_mulit\vest\data\back_damage.rvmat",
								"set_mulit\vest\data\vest_damage.rvmat",
								"set_mulit\vest\data\karman_left_damage.rvmat",
								"set_mulit\vest\data\karman_right_damage.rvmat",
								"set_mulit\vest\data\magaz_big_damage.rvmat",
								"set_mulit\vest\data\small_mag_damage.rvmat",
								"set_mulit\vest\data\palka_damage.rvmat",
								"set_mulit\vest\data\pipka_damage.rvmat",
								"set_mulit\vest\data\plita_damage.rvmat",
								"set_mulit\vest\data\radio_damage.rvmat",
								"set_mulit\vest\data\spine_damage.rvmat",
								"set_mulit\vest\data\zalypa1_damage.rvmat",
								"set_mulit\vest\data\zalypa2_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"set_mulit\vest\data\back_damage.rvmat",
								"set_mulit\vest\data\vest_damage.rvmat",
								"set_mulit\vest\data\karman_left_damage.rvmat",
								"set_mulit\vest\data\karman_right_damage.rvmat",
								"set_mulit\vest\data\magaz_big_damage.rvmat",
								"set_mulit\vest\data\small_mag_damage.rvmat",
								"set_mulit\vest\data\palka_damage.rvmat",
								"set_mulit\vest\data\pipka_damage.rvmat",
								"set_mulit\vest\data\plita_damage.rvmat",
								"set_mulit\vest\data\radio_damage.rvmat",
								"set_mulit\vest\data\spine_damage.rvmat",
								"set_mulit\vest\data\zalypa1_damage.rvmat",
								"set_mulit\vest\data\zalypa2_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"set_mulit\vest\data\back_destruct.rvmat",
								"set_mulit\vest\data\vest_destruct.rvmat",
								"set_mulit\vest\data\karman_left_destruct.rvmat",
								"set_mulit\vest\data\karman_right_destruct.rvmat",
								"set_mulit\vest\data\magaz_big_destruct.rvmat",
								"set_mulit\vest\data\small_mag_destruct.rvmat",
								"set_mulit\vest\data\palka_destruct.rvmat",
								"set_mulit\vest\data\pipka_destruct.rvmat",
								"set_mulit\vest\data\plita_destruct.rvmat",
								"set_mulit\vest\data\radio_destruct.rvmat",
								"set_mulit\vest\data\spine_destruct.rvmat",
								"set_mulit\vest\data\zalypa1_destruct.rvmat",
								"set_mulit\vest\data\zalypa2_destruct.rvmat"
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
						damage=0.60000002;
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
						damage=0.25;
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
						damage=0.25;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage=0.5;
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
	class SM_VP_Multi_Vest: SM_VP_Multi_Vest_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"set_mulit\vest\data\back_co.paa",
			"set_mulit\vest\data\vest_co.paa",
			"set_mulit\vest\data\karman_left_co.paa",
			"set_mulit\vest\data\karman_right_co.paa",
			"set_mulit\vest\data\magaz_big_co.paa",
			"set_mulit\vest\data\small_mag_co.paa",
			"set_mulit\vest\data\palka_co.paa",
			"set_mulit\vest\data\pipka_co.paa",
			"set_mulit\vest\data\plita_co.paa",
			"set_mulit\vest\data\radio_co.paa",
			"set_mulit\vest\data\spine_co.paa",
			"set_mulit\vest\data\zalypa1.paa",
			"set_mulit\vest\data\zalypa2.paa"
		};
	};
};
