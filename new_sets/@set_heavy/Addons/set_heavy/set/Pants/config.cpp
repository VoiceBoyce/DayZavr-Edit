class CfgPatches
{
	class Vp_SET16
	{
		units[]=
		{
			"Vp_SET16_Pants_Black"
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
	class Vp_SET16_Pants_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_Heavy_Pants";
		descriptionShort="";
		model="set_heavy\set\Pants\model\pants_g.p3d";
		attachments[]={};
		inventorySlot[]=
		{
			"Legs"
		};
		itemInfo[]=
		{
			"Clothing",
			"Legs"
		};
		quickBarBonus=2;
		weight=310;
		ragQuantity=6;
		itemSize[]={3,2};
		varWetMax=0.050000001;
		heatIsolation=0.5;
		itemsCargoSize[]={10,4};
		repairableWithKits[]={5,2};
		repairCosts[]={75,50};
		hiddenSelections[]=
		{
			"fast_kobura",
			"fast_krep",
			"fast2_kobura",
			"karmani_na_krep",
			"kobura",
			"koleno",
			"krep_magaz",
			"magaz_krep",
			"pants",
			"podklaka_kobura",
			"personality"
		};
		class ClothingTypes
		{
			male="set_heavy\set\Pants\model\pants_m.p3d";
			female="set_heavy\set\Pants\model\pants_f.p3d";
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
								"set_heavy\set\Pants\data\fast.rvmat",
								"set_heavy\set\Pants\data\fast_krep.rvmat",
								"set_heavy\set\Pants\data\fast2.rvmat",
								"set_heavy\set\Pants\data\karmani_na.rvmat",
								"set_heavy\set\Pants\data\kobura.rvmat",
								"set_heavy\set\Pants\data\koleno.rvmat",
								"set_heavy\set\Pants\data\krep_magaz.rvmat",
								"set_heavy\set\Pants\data\magaz.rvmat",
								"set_heavy\set\Pants\data\pants.rvmat",
								"set_heavy\set\Pants\data\podkladka.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"set_heavy\set\Pants\data\fast.rvmat",
								"set_heavy\set\Pants\data\fast_krep.rvmat",
								"set_heavy\set\Pants\data\fast2.rvmat",
								"set_heavy\set\Pants\data\karmani_na.rvmat",
								"set_heavy\set\Pants\data\kobura.rvmat",
								"set_heavy\set\Pants\data\koleno.rvmat",
								"set_heavy\set\Pants\data\krep_magaz.rvmat",
								"set_heavy\set\Pants\data\magaz.rvmat",
								"set_heavy\set\Pants\data\pants.rvmat",
								"set_heavy\set\Pants\data\podkladka.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"set_heavy\set\Pants\data\fast_damage.rvmat",
								"set_heavy\set\Pants\data\fast_krep_damage.rvmat",
								"set_heavy\set\Pants\data\fast2_damage.rvmat",
								"set_heavy\set\Pants\data\karmani_na_damage.rvmat",
								"set_heavy\set\Pants\data\kobura_damage.rvmat",
								"set_heavy\set\Pants\data\koleno_damage.rvmat",
								"set_heavy\set\Pants\data\krep_magaz_damage.rvmat",
								"set_heavy\set\Pants\data\magaz_damage.rvmat",
								"set_heavy\set\Pants\data\pants_damage.rvmat",
								"set_heavy\set\Pants\data\podkladka_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"set_heavy\set\Pants\data\fast_damage.rvmat",
								"set_heavy\set\Pants\data\fast_krep_damage.rvmat",
								"set_heavy\set\Pants\data\fast2_damage.rvmat",
								"set_heavy\set\Pants\data\karmani_na_damage.rvmat",
								"set_heavy\set\Pants\data\kobura_damage.rvmat",
								"set_heavy\set\Pants\data\koleno_damage.rvmat",
								"set_heavy\set\Pants\data\krep_magaz_damage.rvmat",
								"set_heavy\set\Pants\data\magaz_damage.rvmat",
								"set_heavy\set\Pants\data\pants_damage.rvmat",
								"set_heavy\set\Pants\data\podkladka_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"set_heavy\set\Pants\data\fast_destruct.rvmat",
								"set_heavy\set\Pants\data\fast_krep_destruct.rvmat",
								"set_heavy\set\Pants\data\fast2_destruct.rvmat",
								"set_heavy\set\Pants\data\karmani_na_destruct.rvmat",
								"set_heavy\set\Pants\data\kobura_destruct.rvmat",
								"set_heavy\set\Pants\data\koleno_destruct.rvmat",
								"set_heavy\set\Pants\data\krep_magaz_destruct.rvmat",
								"set_heavy\set\Pants\data\magaz_destruct.rvmat",
								"set_heavy\set\Pants\data\pants_destruct.rvmat",
								"set_heavy\set\Pants\data\podkladka_destruct.rvmat"
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
						damage=0.89999998;
					};
					class Blood
					{
						damage=0.89999998;
					};
					class Shock
					{
						damage=1;
					};
				};
				class Melee
				{
					class Health
					{
						damage=0.89999998;
					};
					class Blood
					{
						damage=0.89999998;
					};
					class Shock
					{
						damage=1;
					};
				};
				class Infected
				{
					class Health
					{
						damage=0.89999998;
					};
					class Blood
					{
						damage=0.89999998;
					};
					class Shock
					{
						damage=1;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage=0.89999998;
					};
					class Blood
					{
						damage=0.89999998;
					};
					class Shock
					{
						damage=1;
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
					soundSet="Shirt_pickup_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="Shirt_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class Vp_SET16_Pants_Black: Vp_SET16_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"set_heavy\set\Pants\data\fast_kobura.paa",
			"set_heavy\set\Pants\data\fast_krep.paa",
			"set_heavy\set\Pants\data\fast2_kobura.paa",
			"set_heavy\set\Pants\data\karmani_na_krep.paa",
			"set_heavy\set\Pants\data\kobura.paa",
			"set_heavy\set\Pants\data\koleno.paa",
			"set_heavy\set\Pants\data\krep_magaz.paa",
			"set_heavy\set\Pants\data\magaz_krep.paa",
			"set_heavy\set\Pants\data\pants.paa",
			"set_heavy\set\Pants\data\podkladka.paa"
		};
	};
};
