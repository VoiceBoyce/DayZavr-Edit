class CfgPatches
{
	class Vp_Set6
	{
		units[]=
		{
			"Vp_Set6_vest2_Silver",
			"Vp_Set6_vest2_Silver_Away"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters",
			"DZ_Characters_vests",
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
	class Vp_Set6_vest2_ColorBase: Clothing
	{
		scope=0;
		displayName="Бронежилет";
		descriptionShort="";
		model="set_cod2\set\vest2\model\vest2_g.p3d";
		attachments[]={};
		inventorySlot[]=
		{
			"vest"
		};
		itemInfo[]=
		{
			"Clothing",
			"vest"
		};
		quickBarBonus=2;
		weight=6000;
		itemSize[]={4,4};
		varWetMax=0.050000001;
		heatIsolation=0.5;
		repairableWithKits[]={3,8};
		repairCosts[]={75,50};
		hiddenSelections[]=
		{
			"camo",
			"plita"
		};
		class ClothingTypes
		{
			male="set_cod2\set\vest2\model\vest2_m.p3d";
			female="set_cod2\set\vest2\model\vest2_f.p3d";
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
								"set_cod2\set\vest2\data\vest2.rvmat",
								"set_cod2\set\vest2\data\plita.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"set_cod2\set\vest2\data\vest2.rvmat",
								"set_cod2\set\vest2\data\plita.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"set_cod2\set\vest2\data\vest2_damage.rvmat",
								"set_cod2\set\vest2\data\plita_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"set_cod2\set\vest2\data\vest2_damage.rvmat",
								"set_cod2\set\vest2\data\plita_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"set_cod2\set\vest2\data\vest2_destruct.rvmat",
								"set_cod2\set\vest2\data\plita_destruct.rvmat"
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
						damage=0.1;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.1;
					};
				};
				class Melee
				{
					class Health
					{
						damage=0.1;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.1;
					};
				};
				class Infected
				{
					class Health
					{
						damage=0.050000001;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.0099999998;
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
						damage=0;
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
					soundSet="Smershvest2_pickup_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="Smershvest2_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class Vp_Set6_vest2_Silver: Vp_Set6_vest2_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"set_cod2\set\vest2\data\vest2_co.paa",
			"set_cod2\set\vest2\data\plita_co.paa"
		};
	};
	class Vp_Set6_vest2_Silver_Away: Vp_Set6_vest2_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"set_cod2\set\vest2\data\vest2_silver_away.paa",
			"set_cod2\set\vest2\data\plita_silver_away.paa"
		};
	};
};
