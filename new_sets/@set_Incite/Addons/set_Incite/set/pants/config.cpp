class CfgPatches
{
	class Vp_SET17
	{
		units[]=
		{
			"Vp_SET17_Pants_Black"
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
	class Vp_SET17_Pants_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_set_Incite_Pants";
		descriptionShort="";
		model="\set_Incite\set\pants\model\pants_g.p3d";
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
			"camo",
			"koleno",
			"krep_top_pants",
			"personality"
		};
		class ClothingTypes
		{
			male="\set_Incite\set\pants\model\pants_m.p3d";
			female="\set_Incite\set\pants\model\pants_f.p3d";
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
								"\set_Incite\set\pants\data\Pants.rvmat",
								"\set_Incite\set\pants\data\koleno.rvmat",
								"\set_Incite\set\pants\data\krep_top_pants.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"\set_Incite\set\pants\data\Pants.rvmat",
								"\set_Incite\set\pants\data\koleno.rvmat",
								"\set_Incite\set\pants\data\krep_top_pants.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"\set_Incite\set\pants\data\Pants_damage.rvmat",
								"\set_Incite\set\pants\data\koleno_damage.rvmat",
								"\set_Incite\set\pants\data\krep_top_pants_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"\set_Incite\set\pants\data\Pants_damage.rvmat",
								"\set_Incite\set\pants\data\koleno_damage.rvmat",
								"\set_Incite\set\pants\data\krep_top_pants_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"\set_Incite\set\pants\data\Pants_destruct.rvmat",
								"\set_Incite\set\pants\data\koleno_destruct.rvmat",
								"\set_Incite\set\pants\data\krep_top_pants_destruct.rvmat"
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
	class Vp_SET17_Pants_Black: Vp_SET17_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"\set_Incite\set\pants\data\Pants_co.paa",
			"\set_Incite\set\pants\data\koleno_co.paa",
			"\set_Incite\set\pants\data\krep_top_pants_co.paa"
		};
	};
};
