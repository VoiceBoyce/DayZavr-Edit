class CfgPatches
{
	class Vp_Set11
	{
		units[]=
		{
			"Vp_Set11_MotoHelmet"
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
	class Vp_Set11_MotoHelmet_ColorBase: Clothing
	{
		scope=0;
		RankLevel=3;
		displayName="$STR_Helmet";
		descriptionShort="";
		model="set_moto\set\helmet\model\moto_helmet_g.p3d";
		attachments[]={};
		inventorySlot[]=
		{
			"Headgear"
		};
		simulation="clothing";
		vehicleClass="Clothing";
		itemInfo[]=
		{
			"Clothing",
			"Helmet"
		};
		ragQuantity=6;
		quickBarBonus=2;
		weight=310;
		itemSize[]={4,4};
		varWetMax=0.050000001;
		heatIsolation=1;
		repairableWithKits[]={3,8};
		repairCosts[]={25};
		hiddenSelections[]=
		{
			"camo",
			"vizor",
			"zalypa1"
		};
		headSelectionsToHide[]=
		{
			"Clipping_Balaclava"
		};
		noPants=1;
		class ClothingTypes
		{
			male="set_moto\set\helmet\model\moto_helmet_mf.p3d";
			female="set_moto\set\helmet\model\moto_helmet_mf.p3d";
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
								"set_moto\set\helmet\data\Helmet.rvmat",
								"set_moto\set\helmet\data\Vizor.rvmat",
								"set_moto\set\helmet\data\zalypa1.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"set_moto\set\helmet\data\Helmet.rvmat",
								"set_moto\set\helmet\data\Vizor.rvmat",
								"set_moto\set\helmet\data\zalypa1.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"set_moto\set\helmet\data\Helmet_damage.rvmat",
								"set_moto\set\helmet\data\Vizor_damage.rvmat",
								"set_moto\set\helmet\data\zalypa1_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"set_moto\set\helmet\data\Helmet_damage.rvmat",
								"set_moto\set\helmet\data\Vizor_damage.rvmat",
								"set_moto\set\helmet\data\zalypa1_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"set_moto\set\helmet\data\Helmet_destruct.rvmat",
								"set_moto\set\helmet\data\Vizor_destruct.rvmat",
								"set_moto\set\helmet\data\zalypa1_destruct.rvmat"
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
						damage=0.5;
					};
					class Blood
					{
						damage=0;
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
						damage=0.5;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0;
					};
				};
				class Infected
				{
					class Health
					{
						damage=0.5;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage=0.5;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0;
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
	class Vp_Set11_MotoHelmet: Vp_Set11_MotoHelmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"set_moto\set\helmet\data\Helmet_co.paa",
			"set_moto\set\helmet\data\Vizor_co.paa",
			"set_moto\set\helmet\data\zalypa1_co.paa"
		};
	};
};
