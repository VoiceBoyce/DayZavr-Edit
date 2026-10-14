class CfgPatches
{
	class Vp_SET20
	{
		units[]=
		{
			"Vp_SET20_Mask"
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
class CfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class Vp_SET20_Mask_ColorBase: Clothing
	{
		displayName="$STR_Legion_Mask";
		descriptionShort="";
		model="set_legion\set\Mask\model\balaclava_g.p3d";
		inventorySlot[]=
		{
			"Mask"
		};
		itemInfo[]=
		{
			"Clothing",
			"Mask"
		};
		rotationFlags=17;
		weight=500;
		itemSize[]={2,2};
		ragQuantity=2;
		varWetMax=0.5;
		heatIsolation=0.5;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		headSelectionsToHide[]=
		{
			"Clipping_Balaclava"
		};
		hiddenSelections[]=
		{
			"balaclava"
		};
		class ClothingTypes
		{
			male="set_legion\set\Mask\model\balaclava_mf.p3d";
			female="set_legion\set\Mask\model\balaclava_mf.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=300;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"set_legion\set\Mask\data\balaclava.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"set_legion\set\Mask\data\balaclava.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"set_legion\set\Mask\data\balaclava_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"set_legion\set\Mask\data\balaclava_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"set_legion\set\Mask\data\balaclava_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		class Protection
		{
			biological=0.25;
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
	class Vp_SET20_Mask: Vp_SET20_Mask_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"set_legion\set\Mask\data\balaclava_co.paa"
		};
	};
};
