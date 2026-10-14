class CfgPatches
{
	class Stalker_2_Monolith_02_Top
	{
		units[]={};
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
class CfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class GorkaEJacket_ColorBase;
	class Stalker_2_Monolith_02_Top_ColorBase: GorkaEJacket_ColorBase
	{
		scope=0;
		displayName="$STR_Stalker_2_Monolith_02_Top_01";
		descriptionShort="$STR_Stalker_2_Monolith_02_Top_02";
		model="Stalker2_Monolith_02\set\jacket\models\jacket_g.p3d";
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
			"Stalker_2_Monolith_02_Pouches1"
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
			"camo_backpack",
			"camo_glass",
			"camo_hood",
			"camo_jacket"
		};
		rotationFlags=17;
		class ClothingTypes
		{
			male="Stalker2_Monolith_02\set\jacket\models\jacket_m.p3d";
			female="Stalker2_Monolith_02\set\jacket\models\jacket_m.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=600;
					transferToAttachmentsCoef=0.5;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"Stalker2_Monolith_02\set\backpack\data\Backpack.rvmat",
								"Stalker2_Monolith_02\set\jacket\data\glass\Glass.rvmat",
								"Stalker2_Monolith_02\set\jacket\data\hood\Hood.rvmat",
								"Stalker2_Monolith_02\set\jacket\data\Top.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"Stalker2_Monolith_02\set\backpack\data\Backpack.rvmat",
								"Stalker2_Monolith_02\set\jacket\data\glass\Glass.rvmat",
								"Stalker2_Monolith_02\set\jacket\data\hood\Hood.rvmat",
								"Stalker2_Monolith_02\set\jacket\data\Top.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"Stalker2_Monolith_02\set\backpack\data\Backpack_damage.rvmat",
								"Stalker2_Monolith_02\set\jacket\data\glass\Glass_damage.rvmat",
								"Stalker2_Monolith_02\set\jacket\data\hood\Hood_damage.rvmat",
								"Stalker2_Monolith_02\set\jacket\data\Top_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"Stalker2_Monolith_02\set\backpack\data\Backpack_damage.rvmat",
								"Stalker2_Monolith_02\set\jacket\data\glass\Glass_damage.rvmat",
								"Stalker2_Monolith_02\set\jacket\data\hood\Hood_damage.rvmat",
								"Stalker2_Monolith_02\set\jacket\data\Top_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"Stalker2_Monolith_02\set\backpack\data\Backpack_destruct.rvmat",
								"Stalker2_Monolith_02\set\jacket\data\glass\Glass_destruct.rvmat",
								"Stalker2_Monolith_02\set\jacket\data\hood\Hood_destruct.rvmat",
								"Stalker2_Monolith_02\set\jacket\data\Top_destruct.rvmat"
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
	class S2_Monolith_02_Top: Stalker_2_Monolith_02_Top_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_02\set\backpack\data\Backpack_co.paa",
			"Stalker2_Monolith_02\set\jacket\data\glass\Glass_ca.paa",
			"Stalker2_Monolith_02\set\jacket\data\hood\Hood.paa",
			"Stalker2_Monolith_02\set\jacket\data\Top_co.paa"
		};
	};
};
