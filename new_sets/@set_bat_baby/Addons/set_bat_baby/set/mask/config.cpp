class CfgPatches
{
	class Vp_Set18
	{
		units[]=
		{
			"Vp_Set18_Mask_White"
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
	class Vp_Set18_Mask_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_set_bat_baby_Mask";
		descriptionShort="";
		model="\set_bat_baby\set\mask\model\mask_g.p3d";
		attachments[]={};
		inventorySlot[]=
		{
			"Mask"
		};
		itemInfo[]=
		{
			"Clothing",
			"Mask"
		};
		ragQuantity=6;
		quickBarBonus=2;
		weight=310;
		itemSize[]={3,3};
		varWetMax=0.050000001;
		heatIsolation=0.5;
		repairableWithKits[]={3,8};
		repairCosts[]={75,50};
		hiddenSelections[]=
		{
			"mask1",
			"mask2",
			"mask3"
		};
		headSelectionsToHide[]=
		{
			"Clipping_Balaclava"
		};
		class ClothingTypes
		{
			male="\set_bat_baby\set\mask\model\mask_mf.p3d";
			female="\set_bat_baby\set\mask\model\mask_mf.p3d";
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
								"\set_bat_baby\set\mask\data\mask1.rvmat",
								"\set_bat_baby\set\mask\data\mask2.rvmat",
								"\set_bat_baby\set\mask\data\mask3.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"\set_bat_baby\set\mask\data\mask1.rvmat",
								"\set_bat_baby\set\mask\data\mask2.rvmat",
								"\set_bat_baby\set\mask\data\mask3.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"\set_bat_baby\set\mask\data\mask1_damage.rvmat",
								"\set_bat_baby\set\mask\data\mask2_damage.rvmat",
								"\set_bat_baby\set\mask\data\mask3_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"\set_bat_baby\set\mask\data\mask1_damage.rvmat",
								"\set_bat_baby\set\mask\data\mask2_damage.rvmat",
								"\set_bat_baby\set\mask\data\mask3_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"\set_bat_baby\set\mask\data\mask1_destruct.rvmat",
								"\set_bat_baby\set\mask\data\mask2_destruct.rvmat",
								"\set_bat_baby\set\mask\data\mask3_destruct.rvmat"
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
						damage=0;
					};
					class Shock
					{
						damage=0.89999998;
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
						damage=0;
					};
					class Shock
					{
						damage=0.89999998;
					};
				};
				class Infected
				{
					class Health
					{
						damage=0.75;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.89999998;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage=0.75;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.89999998;
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
	class Vp_Set18_Mask_White: Vp_Set18_Mask_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"\set_bat_baby\set\mask\data\mask1_co.paa",
			"\set_bat_baby\set\mask\data\mask2_co.paa",
			"\set_bat_baby\set\mask\data\mask3_co.paa"
		};
	};
};
