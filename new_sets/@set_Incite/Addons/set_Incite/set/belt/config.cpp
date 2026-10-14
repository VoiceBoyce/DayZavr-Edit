class CfgPatches
{
	class VP_SET17
	{
		units[]=
		{
			"VP_SET17_Belt_Black"
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
	class VP_SET17_Belt_ColorBase: Clothing
	{
		scope=0;
		RankLevel=3;
		displayName="$STR_set_Incite_Belt";
		descriptionShort="";
		model="\set_Incite\set\belt\model\belt_g.p3d";
		attachments[]=
		{
			"VestGrenadeA",
			"VestGrenadeB",
			"magazine",
			"magazine2"
		};
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
			"bandage_belt",
			"belt",
			"kobura",
			"magaz_belt",
			"magazi_belt",
			"podkladki_belt",
			"sumka_belt",
			"sumka_belt2",
			"zalypa_belt",
			"zalypa_belt2"
		};
		class ClothingTypes
		{
			male="\set_Incite\set\belt\model\belt_mf.p3d";
			female="\set_Incite\set\belt\model\belt_mf.p3d";
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
								"\set_Incite\set\belt\data\bandage_belt.rvmat",
								"\set_Incite\set\belt\data\belt.rvmat",
								"\set_Incite\set\belt\data\kobura.rvmat",
								"\set_Incite\set\belt\data\magaz_belt.rvmat",
								"\set_Incite\set\belt\data\magazi_belt.rvmat",
								"\set_Incite\set\belt\data\podkladki_belt.rvmat",
								"\set_Incite\set\belt\data\sumka_belt.rvmat",
								"\set_Incite\set\belt\data\sumka_belt2.rvmat",
								"\set_Incite\set\belt\data\zalypa_belt.rvmat",
								"\set_Incite\set\belt\data\zalypa_belt2.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"\set_Incite\set\belt\data\bandage_belt.rvmat",
								"\set_Incite\set\belt\data\belt.rvmat",
								"\set_Incite\set\belt\data\kobura.rvmat",
								"\set_Incite\set\belt\data\magaz_belt.rvmat",
								"\set_Incite\set\belt\data\magazi_belt.rvmat",
								"\set_Incite\set\belt\data\podkladki_belt.rvmat",
								"\set_Incite\set\belt\data\sumka_belt.rvmat",
								"\set_Incite\set\belt\data\sumka_belt2.rvmat",
								"\set_Incite\set\belt\data\zalypa_belt.rvmat",
								"\set_Incite\set\belt\data\zalypa_belt2.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"\set_Incite\set\belt\data\bandage_belt_damage.rvmat",
								"\set_Incite\set\belt\data\belt_damage.rvmat",
								"\set_Incite\set\belt\data\kobura_damage.rvmat",
								"\set_Incite\set\belt\data\magaz_belt_damage.rvmat",
								"\set_Incite\set\belt\data\magazi_belt_damage.rvmat",
								"\set_Incite\set\belt\data\podkladki_belt_damage.rvmat",
								"\set_Incite\set\belt\data\sumka_belt_damage.rvmat",
								"\set_Incite\set\belt\data\sumka_belt2_damage.rvmat",
								"\set_Incite\set\belt\data\zalypa_belt_damage.rvmat",
								"\set_Incite\set\belt\data\zalypa_belt2_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"\set_Incite\set\belt\data\bandage_belt_damage.rvmat",
								"\set_Incite\set\belt\data\belt_damage.rvmat",
								"\set_Incite\set\belt\data\kobura_damage.rvmat",
								"\set_Incite\set\belt\data\magaz_belt_damage.rvmat",
								"\set_Incite\set\belt\data\magazi_belt_damage.rvmat",
								"\set_Incite\set\belt\data\podkladki_belt_damage.rvmat",
								"\set_Incite\set\belt\data\sumka_belt_damage.rvmat",
								"\set_Incite\set\belt\data\sumka_belt2_damage.rvmat",
								"\set_Incite\set\belt\data\zalypa_belt_damage.rvmat",
								"\set_Incite\set\belt\data\zalypa_belt2_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"\set_Incite\set\belt\data\bandage_belt_destruct.rvmat",
								"\set_Incite\set\belt\data\belt_destruct.rvmat",
								"\set_Incite\set\belt\data\kobura_destruct.rvmat",
								"\set_Incite\set\belt\data\magaz_belt_destruct.rvmat",
								"\set_Incite\set\belt\data\magazi_belt_destruct.rvmat",
								"\set_Incite\set\belt\data\podkladki_belt_destruct.rvmat",
								"\set_Incite\set\belt\data\sumka_belt_destruct.rvmat",
								"\set_Incite\set\belt\data\sumka_belt2_destruct.rvmat",
								"\set_Incite\set\belt\data\zalypa_belt_destruct.rvmat",
								"\set_Incite\set\belt\data\zalypa_belt2_destruct.rvmat"
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
	class VP_SET17_Belt_Black: VP_SET17_Belt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"\set_Incite\set\belt\data\bandage_belt_co.paa",
			"\set_Incite\set\belt\data\belt_co.paa",
			"\set_Incite\set\belt\data\kobura_co.paa",
			"\set_Incite\set\belt\data\magaz_belt_co.paa",
			"\set_Incite\set\belt\data\magazi_belt_co.paa",
			"\set_Incite\set\belt\data\podkladki_belt_co.paa",
			"\set_Incite\set\belt\data\sumka_belt_co.paa",
			"\set_Incite\set\belt\data\sumka_belt2_co.paa",
			"\set_Incite\set\belt\data\zalypa_belt_co.paa",
			"\set_Incite\set\belt\data\zalypa_belt2_co.paa"
		};
	};
};
