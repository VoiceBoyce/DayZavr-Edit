class CfgPatches
{
	class VP_SET18
	{
		units[]=
		{
			"VP_SET18_Belt_Black"
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
	class VP_SET18_Belt_ColorBase: Clothing
	{
		scope=0;
		RankLevel=3;
		displayName="$STR_set_bat_baby_Belt";
		descriptionShort="";
		model="\set_bat_baby\set\belt\model\belt_g.p3d";
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
		itemsCargoSize[]={10,4};
		quickBarBonus=2;
		varWetMax=0.048999999;
		heatIsolation=0.5;
		weight=550;
		repairableWithKits[]={5,3};
		repairCosts[]={30,25};
		hiddenSelections[]=
		{
			"belt",
			"kobura",
			"podkladka_left_leg",
			"sumka"
		};
		class ClothingTypes
		{
			male="\set_bat_baby\set\belt\model\belt_mf.p3d";
			female="\set_bat_baby\set\belt\model\belt_mf.p3d";
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
								"\set_bat_baby\set\belt\data\belt.rvmat",
								"\set_bat_baby\set\belt\data\kobura.rvmat",
								"\set_bat_baby\set\belt\data\podkladka_left_leg.rvmat",
								"\set_bat_baby\set\belt\data\sumka.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"\set_bat_baby\set\belt\data\belt.rvmat",
								"\set_bat_baby\set\belt\data\kobura.rvmat",
								"\set_bat_baby\set\belt\data\podkladka_left_leg.rvmat",
								"\set_bat_baby\set\belt\data\sumka.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"\set_bat_baby\set\belt\data\belt_damage.rvmat",
								"\set_bat_baby\set\belt\data\kobura_damage.rvmat",
								"\set_bat_baby\set\belt\data\podkladka_left_leg_damage.rvmat",
								"\set_bat_baby\set\belt\data\sumka_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"\set_bat_baby\set\belt\data\belt_damage.rvmat",
								"\set_bat_baby\set\belt\data\kobura_damage.rvmat",
								"\set_bat_baby\set\belt\data\podkladka_left_leg_damage.rvmat",
								"\set_bat_baby\set\belt\data\sumka_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"\set_bat_baby\set\belt\data\belt_destruct.rvmat",
								"\set_bat_baby\set\belt\data\kobura_destruct.rvmat",
								"\set_bat_baby\set\belt\data\podkladka_left_leg_destruct.rvmat",
								"\set_bat_baby\set\belt\data\sumka_destruct.rvmat"
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
						damage=0.75;
					};
					class Blood
					{
						damage=0.60000002;
					};
					class Shock
					{
						damage=0.75;
					};
				};
				class Melee
				{
					class Health
					{
						damage=0.75;
					};
					class Blood
					{
						damage=0.60000002;
					};
					class Shock
					{
						damage=0.75;
					};
				};
				class Infected
				{
					class Health
					{
						damage=0.75;
					};
					class Blood
					{
						damage=0.60000002;
					};
					class Shock
					{
						damage=0.75;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage=0.75;
					};
					class Blood
					{
						damage=0.60000002;
					};
					class Shock
					{
						damage=0.75;
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
	class VP_SET18_Belt_Black: VP_SET18_Belt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"\set_bat_baby\set\belt\data\belt_co.paa",
			"\set_bat_baby\set\belt\data\kobura_co.paa",
			"\set_bat_baby\set\belt\data\podkladka_left_leg_co.paa",
			"\set_bat_baby\set\belt\data\sumka_co.paa"
		};
	};
};
