class CfgPatches
{
	class TGK_Casual_Pouch1
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Magazines",
			"DZ_Characters_Vests",
			"DZ_Weapons_Firearms",
			"DZ_Data",
			"DZ_Characters",
			"DZ_Scripts"
		};
		magazines[]={};
	};
};
class cfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class Container_Base;
	class TGK_Casual_Pouch1_base: Container_Base
	{
		scope=0;
		displayName="$STR_Casual_Pouch01";
		descriptionShort="$STR_Casual_Pouch02";
		model="TGK_CasualSoldier\set\Items\Pouch\models\Pouch.p3d";
		itemSize[]={2,2};
		itemsCargoSize[]={6,4};
		inventorySlot[]=
		{
			"Pouch2"
		};
		hiddenSelections[]=
		{
			"camo"
		};
		weight=400;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
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
								"TGK_CasualSoldier\set\Items\Pouch\data\Pouch.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_CasualSoldier\set\Items\Pouch\data\Pouch.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_CasualSoldier\set\Items\Pouch\data\Pouch_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_CasualSoldier\set\Items\Pouch\data\Pouch_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_CasualSoldier\set\Items\Pouch\data\Pouch_destruct.rvmat"
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
	class TGK_Casual_Pouch1_Black: TGK_Casual_Pouch1_base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_CasualSoldier\set\Items\Pouch\data\Pouch_Black.paa"
		};
	};
	class TGK_Casual_Pouch1_CamoDesert: TGK_Casual_Pouch1_base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_CasualSoldier\set\Items\Pouch\data\Pouch_CamoDesert.paa"
		};
	};
	class TGK_Casual_Pouch1_Green: TGK_Casual_Pouch1_base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_CasualSoldier\set\Items\Pouch\data\Pouch_Green.paa"
		};
	};
	class TGK_Casual_Pouch1_Tan: TGK_Casual_Pouch1_base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_CasualSoldier\set\Items\Pouch\data\Pouch_Tan.paa"
		};
	};
};
class CfgNonAIVehicles
{
	class ProxyAttachment;
	class ProxyPouch: ProxyAttachment
	{
		scope=2;
		inventorySlot="Pouch2";
		model="TGK_CasualSoldier\set\Items\Pouch\models\Pouch.p3d";
	};
	class ProxyMedPouch: ProxyAttachment
	{
		scope=2;
		inventorySlot="MedPouch";
		model="TGK_CasualSoldier\set\Items\MedPouch\models\MedPouch.p3d";
	};
};
class CfgSlots
{
	class Slot_Pouch2
	{
		name="Pouch2";
		displayName="$STR_Casual_Pouch01";
		ghostIcon="";
	};
	class Slot_MedPouch
	{
		name="MedPouch";
		displayName="$STR_Casual_MedPouch01";
		ghostIcon="";
	};
};
