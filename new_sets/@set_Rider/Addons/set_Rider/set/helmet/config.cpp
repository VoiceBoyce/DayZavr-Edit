class CfgPatches
{
	class Vp_SET12
	{
		units[]=
		{
			"Vp_SET12_MotoHelmet"
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
	class Vp_SET12_MotoHelmet_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_Cross helmet";
		descriptionShort="";
		model="set_Rider\set\helmet\Helmet_g.p3d";
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
			"Helmet",
			"Mask"
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
			"1hole",
			"camo",
			"vizor",
			"vizor2"
		};
		headSelectionsToHide[]=
		{
			"Clipping_Balaclava"
		};
		noPants=1;
		class ClothingTypes
		{
			male="set_Rider\set\helmet\Helmet_mf.p3d";
			female="set_Rider\set\helmet\Helmet_mf.p3d";
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
								"set_Rider\set\helmet\data\Mask.rvmat",
								"set_Rider\set\helmet\data\Helmet.rvmat",
								"set_Rider\set\helmet\data\Vizor.rvmat",
								"set_Rider\set\helmet\data\Vizor2.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"set_Rider\set\helmet\data\Mask.rvmat",
								"set_Rider\set\helmet\data\Helmet.rvmat",
								"set_Rider\set\helmet\data\Vizor.rvmat",
								"set_Rider\set\helmet\data\Vizor2.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"set_Rider\set\helmet\data\Mask_damage.rvmat",
								"set_Rider\set\helmet\data\Helmet_damage.rvmat",
								"set_Rider\set\helmet\data\Vizor_damage.rvmat",
								"set_Rider\set\helmet\data\Vizor2_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"set_Rider\set\helmet\data\Mask_damage.rvmat",
								"set_Rider\set\helmet\data\Helmet_damage.rvmat",
								"set_Rider\set\helmet\data\Vizor_damage.rvmat",
								"set_Rider\set\helmet\data\Vizor2_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"set_Rider\set\helmet\data\Mask_destruct.rvmat",
								"set_Rider\set\helmet\data\Helmet_destruct.rvmat",
								"set_Rider\set\helmet\data\Vizor_destruct.rvmat",
								"set_Rider\set\helmet\data\Vizor2_destruct.rvmat"
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
	class Vp_SET12_MotoHelmet: Vp_SET12_MotoHelmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"set_Rider\set\helmet\data\Mask_co.paa",
			"set_Rider\set\helmet\data\Helmet_co.paa",
			"set_Rider\set\helmet\data\Vizor_co.paa",
			"set_Rider\set\helmet\data\Vizor2_ca.paa"
		};
	};
};
