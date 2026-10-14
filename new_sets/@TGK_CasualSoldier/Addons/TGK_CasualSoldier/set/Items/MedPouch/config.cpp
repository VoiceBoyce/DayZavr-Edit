class CfgPatches
{
	class TGK_Casual_MedPouch
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters_Vests",
			"DZ_Characters_Backpacks",
			"DZ_Characters_Headgear",
			"DZ_Scripts",
			"DZ_Characters",
			"DZ_Gear_Drinks",
			"DZ_Gear_Medical",
			"DZ_Gear_Containers",
			"DZ_Radio",
			"DZ_Gear_Optics",
			"DZ_Characters_Glasses"
		};
		magazines[]={};
	};
};
class cfgVehicles
{
	class Container_Base;
	class TGK_Casual_MedPouch_base: Container_Base
	{
		scope=0;
		displayName="$STR_Casual_MedPouch01";
		descriptionShort="$STR_Casual_MedPouch02";
		model="TGK_CasualSoldier\set\Items\MedPouch\models\MedPouch.p3d";
		itemSize[]={2,3};
		itemsCargoSize[]={6,4};
		inventorySlot[]=
		{
			"MedPouch"
		};
		hiddenSelections[]=
		{
			"camo"
		};
		weight=200;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		randomQuantity=2;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=50;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"TGK_CasualSoldier\set\Items\MedPouch\data\MedPouch.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_CasualSoldier\set\Items\MedPouch\data\MedPouch.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_CasualSoldier\set\Items\MedPouch\data\MedPouch_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_CasualSoldier\set\Items\MedPouch\data\MedPouch_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_CasualSoldier\set\Items\MedPouch\data\MedPouch_destruct.rvmat"
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
					soundSet="pickUpCourierBag_Light_SoundSet";
					id=796;
				};
				class pickUpItem
				{
					soundSet="pickUpCourierBag_SoundSet";
					id=797;
				};
			};
		};
	};
	class TGK_Casual_MedPouch_Black: TGK_Casual_MedPouch_base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"\TGK_CasualSoldier\set\Items\MedPouch\data\MedPouch_Black.paa"
		};
	};
	class TGK_Casual_MedPouch_Tan: TGK_Casual_MedPouch_base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"\TGK_CasualSoldier\set\Items\MedPouch\data\MedPouch_Tan.paa"
		};
	};
};
