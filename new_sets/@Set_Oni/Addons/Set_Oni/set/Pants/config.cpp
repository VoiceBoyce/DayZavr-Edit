class CfgPatches
{
	class Vp_SET13
	{
		units[]=
		{
			"Vp_SET13_Pants_Black"
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
	class Vp_SET13_Pants_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_trousers";
		descriptionShort="";
		model="Set_Oni\set\Pants\model\Pants_g.p3d";
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
		itemsCargoSize[]={10,5};
		repairableWithKits[]={5,2};
		repairCosts[]={75,50};
		hiddenSelections[]=
		{
			"camo"
		};
		class ClothingTypes
		{
			male="Set_Oni\set\Pants\model\Pants_mf.p3d";
			female="Set_Oni\set\Pants\model\Pants_mf.p3d";
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
								"Set_Oni\set\Pants\data\Pants.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"Set_Oni\set\Pants\data\Pants.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"Set_Oni\set\Pants\data\Pants_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"Set_Oni\set\Pants\data\Pants_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"Set_Oni\set\Pants\data\Pants_destruct.rvmat"
							}
						}
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
	class Vp_SET13_Pants_Black: Vp_SET13_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Set_Oni\set\Pants\data\Pants_co.paa"
		};
	};
};
