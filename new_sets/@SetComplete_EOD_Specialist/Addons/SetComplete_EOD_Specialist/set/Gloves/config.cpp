class CfgPatches
{
	class TGK_SetComplete_EOD_Specialist
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
	class Clothing;
	class TGK_SetComplete_EOD_Specialist_Gloves_Colorbase: Clothing
	{
		scope=0;
		displayName="$STR_EOD_Specialist_Gloves01";
		descriptionShort="$STR_EOD_Specialist_Gloves02";
		model="SetComplete_EOD_Specialist\set\Gloves\Gloves_g.p3d";
		inventorySlot[]=
		{
			"Gloves"
		};
		simulation="clothing";
		itemInfo[]=
		{
			"Clothing",
			"Gloves"
		};
		weight=450;
		itemSize[]={2,2};
		varWetMax=0.048999999;
		heatIsolation=0.75;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		hiddenSelections[]=
		{
			"gloves"
		};
		class ClothingTypes
		{
			male="SetComplete_EOD_Specialist\set\Gloves\Gloves_m.p3d";
			female="SetComplete_EOD_Specialist\set\Gloves\Gloves_m.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=150;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"SetComplete_EOD_Specialist\set\data\gloves\gloves.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"SetComplete_EOD_Specialist\set\data\gloves\gloves.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"SetComplete_EOD_Specialist\set\data\gloves\gloves_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"SetComplete_EOD_Specialist\set\data\gloves\gloves_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"SetComplete_EOD_Specialist\set\data\gloves\gloves_destruct.rvmat"
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
	class TGK_EOD_Specialist_Gloves_Green: TGK_SetComplete_EOD_Specialist_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"SetComplete_EOD_Specialist\set\data\gloves\gloves_co.paa"
		};
	};
	class TGK_EOD_Specialist_Gloves_Black: TGK_SetComplete_EOD_Specialist_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"SetComplete_EOD_Specialist\set\data\gloves\gloves_BaO.paa"
		};
	};
};
