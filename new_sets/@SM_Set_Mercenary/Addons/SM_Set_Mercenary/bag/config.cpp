class CfgPatches
{
	class SM_Characters_Backpacks_Mercenary
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
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
	class SM_Mercenary_Bag_ColorBase: Clothing
	{
		displayName="$STR_mercenary_bag";
		descriptionShort="$STR_mercenary_opisanie";
		model="\SM_Set_Mercenary\bag\models\bag_g.p3d";
		attachments[]=
		{
			"Shoulder1"
		};
		debug_ItemCategory=9;
		inventorySlot[]=
		{
			"Back"
		};
		itemInfo[]=
		{
			"Clothing",
			"Back"
		};
		rotationFlags=2;
		itemSize[]={4,4};
		itemsCargoSize[]={8,8};
		weight=1000;
		varWetMax=0.79000002;
		heatIsolation=0.2;
		repairableWithKits[]={5,3};
		repairCosts[]={20,25};
		hiddenSelections[]=
		{
			"camo"
		};
		class ClothingTypes
		{
			male="\SM_Set_Mercenary\bag\models\bag_m.p3d";
			female="\SM_Set_Mercenary\bag\models\bag_f.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=120;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"SM_Set_Mercenary\bag\data\Bag.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"SM_Set_Mercenary\bag\data\Bag.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"SM_Set_Mercenary\bag\data\Bag_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"SM_Set_Mercenary\bag\data\Bag_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"SM_Set_Mercenary\bag\data\Bag_destruct.rvmat"
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
				class pickUpItem_Light
				{
					soundSet="pickUpBackPack_Plastic_Light_SoundSet";
					id=796;
				};
				class pickUpItem
				{
					soundSet="pickUpBackPack_Plastic_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="taloonbag_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class SM_Mercenary_Bag_Black_and_Blue: SM_Mercenary_Bag_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"SM_Set_Mercenary\bag\data\Bag_Black.paa"
		};
	};
	class SM_Mercenary_Bag_Black_and_White: SM_Mercenary_Bag_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"SM_Set_Mercenary\bag\data\Bag_White.paa"
		};
	};
	class SM_Mercenary_Bag_Red_and_Yellow: SM_Mercenary_Bag_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"SM_Set_Mercenary\bag\data\Bag_Yellow.paa"
		};
	};
};
