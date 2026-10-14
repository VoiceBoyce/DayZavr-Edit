class CfgPatches
{
	class TGK_Casual_Hat
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters",
			"DZ_Scripts",
			"DZ_Characters_Headgear"
		};
	};
};
class cfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class TGK_Casual_Hat_base: Clothing
	{
		scope=0;
		displayName="$STR_Casual_Hat01";
		descriptionShort="$STR_Casual_Hat02";
		model="TGK_CasualSoldier\set\Hat\models\Hat_g.p3d";
		attachments[]={};
		repairableWithKits[]={8};
		repairCosts[]={25};
		inventorySlot[]=
		{
			"Headgear",
			"Helmet"
		};
		simulation="clothing";
		vehicleClass="Clothing";
		itemInfo[]=
		{
			"Clothing",
			"Headgear"
		};
		weight=700;
		quickBarBonus=0;
		itemSize[]={4,4};
		varWetMax=0.048999999;
		heatIsolation=0.5;
		hiddenSelections[]=
		{
			"hat",
			"goggles",
			"glass"
		};
		headSelectionsToHide[]=
		{
			"Clipping_Mich2001"
		};
		class ClothingTypes
		{
			male="TGK_CasualSoldier\set\Hat\models\Hat_m.p3d";
			female="TGK_CasualSoldier\set\Hat\models\Hat_m.p3d";
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
								"TGK_CasualSoldier\set\Hat\data\Hat.rvmat",
								"TGK_CasualSoldier\set\Hat\data\goggles\Googles.rvmat",
								"TGK_CasualSoldier\set\Hat\data\goggles\glass\glass.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_CasualSoldier\set\Hat\data\Hat.rvmat",
								"TGK_CasualSoldier\set\Hat\data\goggles\Googles.rvmat",
								"TGK_CasualSoldier\set\Hat\data\goggles\glass\glass.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_CasualSoldier\set\Hat\data\Hat_damage.rvmat",
								"TGK_CasualSoldier\set\Hat\data\goggles\Googles_damage.rvmat",
								"TGK_CasualSoldier\set\Hat\data\goggles\glass\glass_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_CasualSoldier\set\Hat\data\Hat_damage.rvmat",
								"TGK_CasualSoldier\set\Hat\data\goggles\Googles_damage.rvmat",
								"TGK_CasualSoldier\set\Hat\data\goggles\glass\glass_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_CasualSoldier\set\Hat\data\Hat_destruct.rvmat",
								"TGK_CasualSoldier\set\Hat\data\goggles\Googles_destruct.rvmat",
								"TGK_CasualSoldier\set\Hat\data\goggles\glass\glass_destruct.rvmat"
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
					soundSet="pickUpPot_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="BallisticHelmet_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class TGK_Casual_Hat_Black: TGK_Casual_Hat_base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_CasualSoldier\set\Hat\data\Hat_Black.paa",
			"TGK_CasualSoldier\set\Hat\data\goggles\Googles_Black.paa",
			"TGK_CasualSoldier\set\Hat\data\goggles\glass\glass_co.paa"
		};
	};
	class TGK_Casual_Hat_Gray: TGK_Casual_Hat_base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_CasualSoldier\set\Hat\data\Hat_Gray.paa",
			"TGK_CasualSoldier\set\Hat\data\goggles\Googles_Black.paa",
			"TGK_CasualSoldier\set\Hat\data\goggles\glass\glass_co.paa"
		};
	};
	class TGK_Casual_Hat_Green: TGK_Casual_Hat_base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_CasualSoldier\set\Hat\data\Hat_Green.paa",
			"TGK_CasualSoldier\set\Hat\data\goggles\Googles_Black.paa",
			"TGK_CasualSoldier\set\Hat\data\goggles\glass\glass_co.paa"
		};
	};
	class TGK_Casual_Hat_Tan: TGK_Casual_Hat_base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_CasualSoldier\set\Hat\data\Hat_Tan.paa",
			"TGK_CasualSoldier\set\Hat\data\goggles\Googles_Tan.paa",
			"TGK_CasualSoldier\set\Hat\data\goggles\glass\glass_co.paa"
		};
	};
};
