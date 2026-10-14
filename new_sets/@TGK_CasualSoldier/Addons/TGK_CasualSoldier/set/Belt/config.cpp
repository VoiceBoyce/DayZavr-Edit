class CfgPatches
{
	class TGK_Casual_Belt
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
	class TGK_Casual_Belt_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_Casual_Belt";
		descriptionShort="$STR_Casual_Belt02";
		model="TGK_CasualSoldier\set\Belt\models\Belt_g.p3d";
		attachments[]=
		{
			"Pouch2",
			"MedPouch"
		};
		vehicleClass="Clothing";
		simulation="clothing";
		inventorySlot="Hips";
		itemInfo[]=
		{
			"Clothing",
			"Hips"
		};
		itemSize[]={5,1};
		quickBarBonus=2;
		varWetMax=0.048999999;
		heatIsolation=0.075000003;
		weight=550;
		repairableWithKits[]={5,3};
		repairCosts[]={30,25};
		lootCategory="Crafted";
		hiddenSelections[]=
		{
			"camo"
		};
		class ClothingTypes
		{
			male="TGK_CasualSoldier\set\Belt\models\Belt_m.p3d";
			female="TGK_CasualSoldier\set\Belt\models\Belt_m.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=100;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"TGK_CasualSoldier\set\Belt\data\Belt.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_CasualSoldier\set\Belt\data\Belt.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_CasualSoldier\set\Belt\data\Belt_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_CasualSoldier\set\Belt\data\Belt_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_CasualSoldier\set\Belt\data\Belt_destruct.rvmat"
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
					soundSet="WorkingGloves_pickup_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="WorkingGloves_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class TGK_Casual_Belt_Black: TGK_Casual_Belt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_CasualSoldier\set\Belt\data\Belt_Black.paa"
		};
	};
	class TGK_Casual_Belt_Green: TGK_Casual_Belt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_CasualSoldier\set\Belt\data\Belt_Green.paa"
		};
	};
	class TGK_Casual_Belt_Tan: TGK_Casual_Belt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_CasualSoldier\set\Belt\data\Belt_Tan.paa"
		};
	};
};
