class CfgPatches
{
	class VP_SET6
	{
		units[]=
		{
			"Belt_SET6_Black",
			"Belt_SET6_Silver",
			"Belt_SET6_Silver_Away"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters",
			"DZ_Scripts"
		};
	};
};
class cfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class Belt_SET6_ColorBase: Clothing
	{
		scope=0;
		displayName="Разгрузочная система на пояс";
		descriptionShort="";
		model="set_cod2\set\belt\model\belt_g.p3d";
		attachments[]={};
		vehicleClass="Clothing";
		simulation="clothing";
		inventorySlot[]=
		{
			"Hips"
		};
		itemInfo[]=
		{
			"Clothing",
			"Hips"
		};
		itemSize[]={5,5};
		itemsCargoSize[]={4,10};
		quickBarBonus=2;
		varWetMax=0.048999999;
		heatIsolation=0.5;
		weight=550;
		repairableWithKits[]={5,3};
		repairCosts[]={30,25};
		lootCategory="Crafted";
		hiddenSelections[]=
		{
			"camo",
			"fast",
			"kabura",
			"pounsh1",
			"pounsh2"
		};
		hiddenSelectionsTextures[]=
		{
			"set_cod2\set\belt\data\belt_co.paa",
			"set_cod2\set\belt\data\fast_co.paa",
			"set_cod2\set\belt\data\kabura_co.paa",
			"set_cod2\set\belt\data\left_poush_co.paa",
			"set_cod2\set\belt\data\right_poush_co.paa"
		};
		class ClothingTypes
		{
			male="set_cod2\set\belt\model\belt_m.p3d";
			female="set_cod2\set\belt\model\belt_f.p3d";
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
								"set_cod2\set\belt\data\belt.rvmat",
								"set_cod2\set\belt\data\fast.rvmat",
								"set_cod2\set\belt\data\kabura.rvmat",
								"set_cod2\set\belt\data\left_poush.rvmat",
								"set_cod2\set\belt\data\right_poush.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"set_cod2\set\belt\data\belt.rvmat",
								"set_cod2\set\belt\data\fast.rvmat",
								"set_cod2\set\belt\data\kabura.rvmat",
								"set_cod2\set\belt\data\left_poush.rvmat",
								"set_cod2\set\belt\data\right_poush.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"set_cod2\set\belt\data\belt_damage.rvmat",
								"set_cod2\set\belt\data\fast_damage.rvmat",
								"set_cod2\set\belt\data\kabura_damage.rvmat",
								"set_cod2\set\belt\data\left_poush_damage.rvmat",
								"set_cod2\set\belt\data\right_poush_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"set_cod2\set\belt\data\belt_damage.rvmat",
								"set_cod2\set\belt\data\fast_damage.rvmat",
								"set_cod2\set\belt\data\kabura_damage.rvmat",
								"set_cod2\set\belt\data\left_poush_damage.rvmat",
								"set_cod2\set\belt\data\right_poush_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"set_cod2\set\belt\data\belt_destruct.rvmat",
								"set_cod2\set\belt\data\fast_destruct.rvmat",
								"set_cod2\set\belt\data\kabura_destruct.rvmat",
								"set_cod2\set\belt\data\left_poush_destruct.rvmat",
								"set_cod2\set\belt\data\right_poush_destruct.rvmat"
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
						damage=0.85000002;
					};
					class Blood
					{
						damage=0.80000001;
					};
					class Shock
					{
						damage=0.80000001;
					};
				};
				class Melee
				{
					class Health
					{
						damage=0.85000002;
					};
					class Blood
					{
						damage=0.80000001;
					};
					class Shock
					{
						damage=0.80000001;
					};
				};
				class Infected
				{
					class Health
					{
						damage=0.85000002;
					};
					class Blood
					{
						damage=0.80000001;
					};
					class Shock
					{
						damage=0.80000001;
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
						damage=0.2;
					};
					class Shock
					{
						damage=0.2;
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
					soundSet="WorkingGloves_pickup_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="WorkingGloves_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class Belt_SET6_Black: Belt_SET6_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"set_cod2\set\belt\data\belt_co.paa",
			"set_cod2\set\belt\data\fast_co.paa",
			"set_cod2\set\belt\data\kabura_co.paa",
			"set_cod2\set\belt\data\left_poush_co.paa",
			"set_cod2\set\belt\data\right_poush_co.paa"
		};
	};
	class Belt_SET6_Silver: Belt_SET6_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"set_cod2\set\belt\data\belt_co.paa",
			"set_cod2\set\belt\data\fast_co.paa",
			"set_cod2\set\belt\data\kabura_silver.paa",
			"set_cod2\set\belt\data\left_poush_silver.paa",
			"set_cod2\set\belt\data\right_poush_silver.paa"
		};
	};
	class Belt_SET6_Silver_Away: Belt_SET6_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"set_cod2\set\belt\data\belt_co.paa",
			"set_cod2\set\belt\data\fast_co.paa",
			"set_cod2\set\belt\data\kabura_silver.paa",
			"set_cod2\set\belt\data\left_poush_silver.paa",
			"set_cod2\set\belt\data\right_poush_silver.paa"
		};
	};
};
