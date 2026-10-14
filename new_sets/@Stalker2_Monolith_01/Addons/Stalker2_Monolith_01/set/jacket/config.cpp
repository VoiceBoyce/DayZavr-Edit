class CfgPatches
{
	class Stalker_2_Monolith_Top
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters",
			"DZ_Scripts",
			"DZ_Characters_Tops"
		};
	};
};
class CfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class GorkaEJacket_ColorBase;
	class Stalker_2_Monolith_Top_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_Stalker_2_Monolith_Top_01";
		descriptionShort="$STR_Stalker_2_Monolith_Top_02";
		model="Stalker2_Monolith_01\set\jacket\jacket_g.p3d";
		inventorySlot[]=
		{
			"Body"
		};
		itemInfo[]=
		{
			"Clothing",
			"Body"
		};
		attachments[]=
		{
			"Magpouch_a",
			"Magpouch_b",
			"Magpouch_c",
			"GranadePouch_a",
			"GranadePouch_b",
			"GranadePouch_c",
			"LegPouch"
		};
		weight=5000;
		itemSize[]={4,6};
		quickBarBonus=2;
		ragQuantity=3;
		varWetMax=0.249;
		heatIsolation=0.40000001;
		itemsCargoSize[]={6,10};
		repairableWithKits[]={2,3};
		repairCosts[]={75,50};
		hiddenSelections[]=
		{
			"camo"
		};
		rotationFlags=17;
		class ClothingTypes
		{
			male="Stalker2_Monolith_01\set\jacket\jacket_m.p3d";
			female="Stalker2_Monolith_01\set\jacket\jacket_m.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=700;
					transferToAttachmentsCoef=0.5;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"Stalker2_Monolith_01\set\data\Monolith.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"Stalker2_Monolith_01\set\data\Monolith.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"Stalker2_Monolith_01\set\data\Monolith_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"Stalker2_Monolith_01\set\data\Monolith_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"Stalker2_Monolith_01\set\data\Monolith_destruct.rvmat"
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
						damage=0.55000001;
					};
					class Blood
					{
						damage=0.55000001;
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
						damage=0.34999999;
					};
					class Blood
					{
						damage=0.34999999;
					};
					class Shock
					{
						damage=0.15000001;
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
						damage=0.44999999;
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
						damage=0.44999999;
					};
					class Blood
					{
						damage=0.1;
					};
					class Shock
					{
						damage=0.44999999;
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
	class S2_Monolith_01_Top: Stalker_2_Monolith_Top_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\data\Monolith_co.paa"
		};
	};
	class S2_Monolith_01_Top_ClearSky: Stalker_2_Monolith_Top_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\data\Monolith_ClearSky.paa"
		};
	};
	class S2_Monolith_01_Top_Duty: Stalker_2_Monolith_Top_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\data\Monolith_duty.paa"
		};
	};
	class S2_Monolith_01_Top_Freedom: Stalker_2_Monolith_Top_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\data\Monolith_Freedom.paa"
		};
	};
	class S2_Monolith_01_Top_Korpus: Stalker_2_Monolith_Top_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\data\Monolith_Korpus.paa"
		};
	};
	class S2_Monolith_01_Top_Merc: Stalker_2_Monolith_Top_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\data\Monolith_Merc.paa"
		};
	};
	class S2_Monolith_01_Top_MSOP: Stalker_2_Monolith_Top_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\data\Monolith_MSOP.paa"
		};
	};
	class S2_Monolith_01_Top_Neutrals: Stalker_2_Monolith_Top_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\data\Monolith_Neutrals.paa"
		};
	};
	class S2_Monolith_01_Top_Warta: Stalker_2_Monolith_Top_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\data\Monolith_Warta.paa"
		};
	};
};
