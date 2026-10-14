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
	class BalaclavaMask_ColorBase;
	class TGK_SetComplete_EOD_Specialist_Mask_Colorbase: BalaclavaMask_ColorBase
	{
		scope=0;
		displayName="$STR_EOD_Specialist_Mask01";
		descriptionShort="$STR_EOD_Specialist_Mask02";
		inventorySlot[]=
		{
			"Mask"
		};
		itemSize[]={2,2};
		rotationFlags=17;
		model="SetComplete_EOD_Specialist\set\Mask\Scarf_g.p3d";
		hiddenSelections[]=
		{
			"scarf"
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
								"SetComplete_EOD_Specialist\set\data\scarf\scarf.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"SetComplete_EOD_Specialist\set\data\scarf\scarf.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"SetComplete_EOD_Specialist\set\data\scarf\scarf_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"SetComplete_EOD_Specialist\set\data\scarf\scarf_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"SetComplete_EOD_Specialist\set\data\scarf\scarf_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		class ClothingTypes
		{
			male="SetComplete_EOD_Specialist\set\Mask\Scarf_m.p3d";
			female="SetComplete_EOD_Specialist\set\Mask\Scarf_m.p3d";
		};
	};
	class TGK_EOD_Specialist_Mask_Orange: TGK_SetComplete_EOD_Specialist_Mask_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"SetComplete_EOD_Specialist\set\data\scarf\scarf_co.paa"
		};
	};
	class TGK_EOD_Specialist_Mask_Black: TGK_SetComplete_EOD_Specialist_Mask_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"SetComplete_EOD_Specialist\set\data\scarf\scarf_Black_co.paa"
		};
	};
};
