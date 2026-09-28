class CfgPatches
{
	class FOG_MOD_ZynCan
	{
		units[]=
		{
			"FOG_ZynCan_WinterGreen"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data"
		};
	};
};
class CfgVehicles
{
	class Inventory_Base;
	class FOG_ZynCan_Base: Inventory_Base
	{
		scope=0;
		displayName="ZynCan 6MG";
		descriptionShort="Баночка ZynPouches для никотинового перекура. | A can of ZynPouches for when you need that nicotine fix.";
		itemsize[]={2,2};
		allowOwnedCargoManipulation=1;
		Model="\FOG_MOD\Gear\MISC\ZYNCan\FOG_ZynCan.p3d";
		inventorySlot[]={};
		weight=100;
		hiddenSelections[]=
		{
			"camo"
		};
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
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
								"FOG_MOD\Gear\MISC\ZYNCan\data\ZynCan.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Gear\MISC\ZYNCan\data\ZynCan.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Gear\MISC\ZYNCan\data\ZynCan.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Gear\MISC\ZYNCan\data\ZynCan.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Gear\MISC\ZYNCan\data\ZynCan.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class FOG_ZynCan_WinterGreen: FOG_ZynCan_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\MISC\ZYNCan\data\ZynCan_co.paa"
		};
	};
};
