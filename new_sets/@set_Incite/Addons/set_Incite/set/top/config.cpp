class CfgPatches
{
	class Vp_SET17
	{
		units[]=
		{
			"Vp_SET17_Top_Black"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters"
		};
	};
};
class cfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class Vp_SET17_Top_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_set_Incite_Top";
		descriptionShort="";
		model="\set_Incite\set\top\model\top_g.p3d";
		attachments[]={};
		inventorySlot[]=
		{
			"Body"
		};
		itemInfo[]=
		{
			"Clothing",
			"Body"
		};
		quickBarBonus=2;
		weight=310;
		itemSize[]={3,2};
		varWetMax=0.050000001;
		heatIsolation=0.5;
		itemsCargoSize[]={10,4};
		repairableWithKits[]={5,2};
		repairCosts[]={75,50};
		hiddenSelections[]=
		{
			"camo",
			"gorlovina",
			"krep_top_pants",
			"nadpleshniki",
			"napleshniki_relief",
			"predpleshie"
		};
		class ClothingTypes
		{
			male="\set_Incite\set\top\model\top_mf.p3d";
			female="\set_Incite\set\top\model\top_mf.p3d";
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
								"\set_Incite\set\top\data\top.rvmat",
								"\set_Incite\set\top\data\gorlovina.rvmat",
								"\set_Incite\set\top\data\krep_top.rvmat",
								"\set_Incite\set\top\data\nadpleshniki.rvmat",
								"\set_Incite\set\top\data\napleshniki_relief.rvmat",
								"\set_Incite\set\top\data\predpleshie.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"\set_Incite\set\top\data\top.rvmat",
								"\set_Incite\set\top\data\gorlovina.rvmat",
								"\set_Incite\set\top\data\krep_top.rvmat",
								"\set_Incite\set\top\data\nadpleshniki.rvmat",
								"\set_Incite\set\top\data\napleshniki_relief.rvmat",
								"\set_Incite\set\top\data\predpleshie.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"\set_Incite\set\top\data\top_damage.rvmat",
								"\set_Incite\set\top\data\gorlovina_damage.rvmat",
								"\set_Incite\set\top\data\krep_top_damage.rvmat",
								"\set_Incite\set\top\data\nadpleshniki_damage.rvmat",
								"\set_Incite\set\top\data\napleshniki_relief_damage.rvmat",
								"\set_Incite\set\top\data\predpleshie_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"\set_Incite\set\top\data\top_damage.rvmat",
								"\set_Incite\set\top\data\gorlovina_damage.rvmat",
								"\set_Incite\set\top\data\krep_top_damage.rvmat",
								"\set_Incite\set\top\data\nadpleshniki_damage.rvmat",
								"\set_Incite\set\top\data\napleshniki_relief_damage.rvmat",
								"\set_Incite\set\top\data\predpleshie_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"\set_Incite\set\top\data\top_destruct.rvmat",
								"\set_Incite\set\top\data\gorlovina_destruct.rvmat",
								"\set_Incite\set\top\data\krep_top_destruct.rvmat",
								"\set_Incite\set\top\data\nadpleshniki_destruct.rvmat",
								"\set_Incite\set\top\data\napleshniki_relief_destruct.rvmat",
								"\set_Incite\set\top\data\predpleshie_destruct.rvmat"
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
	class Vp_SET17_Top_Black: Vp_SET17_Top_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"\set_Incite\set\top\data\top_co.paa",
			"\set_Incite\set\top\data\gorlovina_co.paa",
			"\set_Incite\set\top\data\krep_top_pants_co.paa",
			"\set_Incite\set\top\data\nadpleshniki_co.paa",
			"\set_Incite\set\top\data\napleshniki_relief_co.paa",
			"\set_Incite\set\top\data\predpleshie_co.paa"
		};
	};
};
