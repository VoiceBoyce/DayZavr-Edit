class CfgPatches
{
	class Vp_Set6
	{
		units[]=
		{
			"Vp_Set6_Pants_Black",
			"Vp_Set6_Pants_Silver",
			"Vp_Set6_Pants_Silver_Away"
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
	class Vp_Set6_Pants_ColorBase: Clothing
	{
		scope=0;
		displayName="Штаны";
		descriptionShort="";
		model="set_cod2\set\pants\model\pants_g.p3d";
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
		itemsCargoSize[]={8,5};
		repairableWithKits[]={3,8};
		repairCosts[]={75,50};
		hiddenSelections[]=
		{
			"camo",
			"personality"
		};
		class ClothingTypes
		{
			male="set_cod2\set\pants\model\pants_m.p3d";
			female="set_cod2\set\pants\model\pants_f.p3d";
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
								"set_cod2\set\pants\data\pants.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"set_cod2\set\pants\data\pants.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"set_cod2\set\pants\data\pants_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"set_cod2\set\pants\data\pants_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"set_cod2\set\pants\data\pants_destruct.rvmat"
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
	class Vp_Set6_Pants_Black: Vp_Set6_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"set_cod2\set\pants\data\pants_co.paa"
		};
	};
	class Vp_Set6_Pants_Silver: Vp_Set6_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"set_cod2\set\pants\data\pants_silver.paa"
		};
	};
	class Vp_Set6_Pants_Silver_Away: Vp_Set6_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"set_cod2\set\pants\data\pants_silver_away.paa"
		};
	};
};
