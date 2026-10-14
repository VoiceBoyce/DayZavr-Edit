class CfgPatches
{
	class TGK_Casual_Backpack
	{
		units[]={};
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
	class TGK_Casual_Backpack_base: Clothing
	{
		displayName="$STR_Casual_Backpack";
		descriptionShort="$STR_Casual_Backpack02";
		model="TGK_CasualSoldier\set\Backpack\models\Backpack_g.p3d";
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
		rotationFlags=16;
		weight=1500;
		itemSize[]={4,3};
		itemsCargoSize[]={7,4};
		varWetMax=0.5;
		heatIsolation=0.5;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		soundAttType="Outdoor";
		randomQuantity=3;
		hiddenSelections[]=
		{
			"camo"
		};
		class ClothingTypes
		{
			male="TGK_CasualSoldier\set\Backpack\models\Backpack_m.p3d";
			female="TGK_CasualSoldier\set\Backpack\models\Backpack_m.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=100;
					transferToAttachmentsCoef=0.5;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"TGK_CasualSoldier\set\Backpack\data\Backpack.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_CasualSoldier\set\Backpack\data\Backpack.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_CasualSoldier\set\Backpack\data\Backpack_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_CasualSoldier\set\Backpack\data\Backpack_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_CasualSoldier\set\Backpack\data\Backpack_destruct.rvmat"
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
	class TGK_Casual_Backpack_Black: TGK_Casual_Backpack_base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_CasualSoldier\set\Backpack\data\Backpack_Black.paa"
		};
	};
	class TGK_Casual_Backpack_Blue: TGK_Casual_Backpack_base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_CasualSoldier\set\Backpack\data\Backpack_Blue.paa"
		};
	};
	class TGK_Casual_Backpack_CamoDesert: TGK_Casual_Backpack_base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_CasualSoldier\set\Backpack\data\Backpack_CamoDesert.paa"
		};
	};
	class TGK_Casual_Backpack_Green: TGK_Casual_Backpack_base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_CasualSoldier\set\Backpack\data\Backpack_Green.paa"
		};
	};
	class TGK_Casual_Backpack_Red: TGK_Casual_Backpack_base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_CasualSoldier\set\Backpack\data\Backpack_Red.paa"
		};
	};
	class TGK_Casual_Backpack_Tan: TGK_Casual_Backpack_base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_CasualSoldier\set\Backpack\data\Backpack_Tan.paa"
		};
	};
};
