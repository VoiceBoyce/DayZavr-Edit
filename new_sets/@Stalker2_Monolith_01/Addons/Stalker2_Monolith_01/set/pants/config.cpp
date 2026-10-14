class CfgPatches
{
	class Stalker_2_Monolith_Pants
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters",
			"DZ_Scripts",
			"DZ_Characters_Pants"
		};
	};
};
class CfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class GorkaPants_ColorBase;
	class Stalker_2_Monolith_Pants_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_Stalker_2_Monolith_Pants_01";
		descriptionShort="$STR_Stalker_2_Monolith_Pants_02";
		model="Stalker2_Monolith_01\set\pants\pants_g.p3d";
		inventorySlot[]=
		{
			"Legs"
		};
		itemInfo[]=
		{
			"Clothing",
			"Legs"
		};
		attachments[]=
		{
			"LegPouch",
			"GranadePouch_a",
			"GranadePouch_b"
		};
		weight=1000;
		itemSize[]={4,6};
		quickBarBonus=2;
		varWetMax=0.249;
		heatIsolation=0.40000001;
		itemsCargoSize[]={10,5};
		repairableWithKits[]={2,3};
		repairCosts[]={75,50};
		hiddenSelections[]=
		{
			"camo",
			"personality"
		};
		rotationFlags=17;
		class ClothingTypes
		{
			male="Stalker2_Monolith_01\set\pants\pants_m.p3d";
			female="Stalker2_Monolith_01\set\pants\pants_m.p3d";
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
								"Stalker2_Monolith_01\set\data\Pants.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"Stalker2_Monolith_01\set\data\Pants.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"Stalker2_Monolith_01\set\data\Pants_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"Stalker2_Monolith_01\set\data\Pants_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"Stalker2_Monolith_01\set\data\Pants_destruct.rvmat"
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
						damage=0.75;
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
						damage=0.75;
					};
					class Blood
					{
						damage=0.75;
					};
					class Shock
					{
						damage=0.44999999;
					};
				};
				class Infected
				{
					class Health
					{
						damage=0.1;
					};
					class Blood
					{
						damage=0.5;
					};
					class Shock
					{
						damage=0.15000001;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage=1;
					};
					class Blood
					{
						damage=0.1;
					};
					class Shock
					{
						damage=0.1;
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
	class S2_Monolith_01_Pants: Stalker_2_Monolith_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\data\Monolith_co.paa"
		};
	};
	class S2_Monolith_01_Pants_ClearSky: Stalker_2_Monolith_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\data\Monolith_ClearSky.paa"
		};
	};
	class S2_Monolith_01_Pants_Duty: Stalker_2_Monolith_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\data\Monolith_duty.paa"
		};
	};
	class S2_Monolith_01_Pants_Freedom: Stalker_2_Monolith_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\data\Monolith_Freedom.paa"
		};
	};
	class S2_Monolith_01_Pants_Korpus: Stalker_2_Monolith_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\data\Monolith_Korpus.paa"
		};
	};
	class S2_Monolith_01_Pants_Merc: Stalker_2_Monolith_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\data\Monolith_Merc.paa"
		};
	};
	class S2_Monolith_01_Pants_MSOP: Stalker_2_Monolith_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\data\Monolith_MSOP.paa"
		};
	};
	class S2_Monolith_01_Pants_Neutrals: Stalker_2_Monolith_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\data\Monolith_Neutrals.paa"
		};
	};
	class S2_Monolith_01_Pants_Warta: Stalker_2_Monolith_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\data\Monolith_Warta.paa"
		};
	};
};
