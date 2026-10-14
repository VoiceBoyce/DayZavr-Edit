class CfgPatches
{
	class Vp_SET13
	{
		units[]=
		{
			"Vp_SET13_Mask_Black"
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
	class Vp_SET13_Mask_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_mask";
		descriptionShort="";
		model="Set_Oni\set\Mask\model\Mask_g.p3d";
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
		itemSize[]={3,2};
		varWetMax=0.050000001;
		heatIsolation=0.5;
		repairableWithKits[]={3,8};
		repairCosts[]={75,50};
		hiddenSelections[]=
		{
			"mask",
			"ebalo"
		};
		headSelectionsToHide[]=
		{
			"Clipping_Balaclava"
		};
		class ClothingTypes
		{
			male="Set_Oni\set\Mask\model\Mask_mf.p3d";
			female="Set_Oni\set\Mask\model\Mask_mf.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=1500;
					transferToAttachmentsCoef=0.5;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"Set_Oni\set\Mask\data\Mask.rvmat",
								"Set_Oni\set\Mask\data\Ebalo.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"Set_Oni\set\Mask\data\Mask.rvmat",
								"Set_Oni\set\Mask\data\Ebalo.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"Set_Oni\set\Mask\data\Mask_damage.rvmat",
								"Set_Oni\set\Mask\data\Ebalo_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"Set_Oni\set\Mask\data\Mask_damage.rvmat",
								"Set_Oni\set\Mask\data\Ebalo_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"Set_Oni\set\Mask\data\Mask_destruct.rvmat",
								"Set_Oni\set\Mask\data\Ebalo_destruct.rvmat"
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
	class Vp_SET13_Mask_Black: Vp_SET13_Mask_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Set_Oni\set\Mask\data\Mask_co.paa",
			"Set_Oni\set\Mask\data\Ebalo_co.paa"
		};
	};
};
