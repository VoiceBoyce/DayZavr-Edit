class CfgPatches
{
	class Stalker_2_Monolith_02_Pants
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
	class GorkaPants_ColorBase;
	class Stalker_2_Monolith_02_Pants_ColorBase: GorkaPants_ColorBase
	{
		scope=0;
		displayName="$STR_Stalker_2_Monolith_02_Pants_01";
		descriptionShort="$STR_Stalker_2_Monolith_02_Pants_02";
		model="Stalker2_Monolith_02\set\pants\models\pants_g.p3d";
		inventorySlot[]=
		{
			"Legs"
		};
		itemInfo[]=
		{
			"Clothing",
			"Legs"
		};
		attachments[]={};
		weight=2000;
		itemSize[]={4,6};
		quickBarBonus=2;
		varWetMax=0.249;
		heatIsolation=0.40000001;
		itemsCargoSize[]={10,6};
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
			male="Stalker2_Monolith_02\set\pants\models\pants_m.p3d";
			female="Stalker2_Monolith_02\set\pants\models\pants_m.p3d";
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
								"Stalker2_Monolith_02\set\pants\data\Pants.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"Stalker2_Monolith_02\set\pants\data\Pants.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"Stalker2_Monolith_02\set\pants\data\Pants_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"Stalker2_Monolith_02\set\pants\data\Pants_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"Stalker2_Monolith_02\set\pants\data\Pants_destruct.rvmat"
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
	class S2_Monolith_02_Pants: Stalker_2_Monolith_02_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_02\set\pants\data\Pants_co.paa"
		};
	};
};
