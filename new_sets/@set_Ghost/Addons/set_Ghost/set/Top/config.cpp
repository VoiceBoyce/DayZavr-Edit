class CfgPatches
{
	class Vp_SET14
	{
		units[]=
		{
			"Vp_SET14_Top_Black"
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
	class Vp_SET14_Top_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_Ghost_Top";
		descriptionShort="";
		model="set_Ghost\set\Top\model\Top_g.p3d";
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
			"camo"
		};
		class ClothingTypes
		{
			male="set_Ghost\set\Top\model\Top_mf.p3d";
			female="set_Ghost\set\Top\model\Top_mf.p3d";
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
								"set_Ghost\set\Top\data\Top.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"set_Ghost\set\Top\data\Top.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"set_Ghost\set\Top\data\Top_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"set_Ghost\set\Top\data\Top_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"set_Ghost\set\Top\data\Top_destruct.rvmat"
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
	class Vp_SET14_Top_Black: Vp_SET14_Top_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"set_Ghost\set\Top\data\Top_co.paa"
		};
	};
};
