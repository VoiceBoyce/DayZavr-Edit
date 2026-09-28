class CfgPatches
{
	class FOG_MOD_BoltCutters
	{
		units[]=
		{
			"FOG_BoltCutters"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Weapons_Melee",
			"DZ_Gear_Tools"
		};
	};
};
class CfgVehicles
{
	class Lockpick;
	class FOG_BoltCutters_Base: Lockpick
	{
		scope=0;
		displayName="BoltCutters";
		descriptionShort="Болторезы. Позволяют освобождать людей от наручников, открывать запертые двери и обезвреживать взрывчатку. | Bolt cutters, has the ability to unrestrain individuals, open locked doors, and disarm explosives.";
		itemsize[]={2,4};
		allowOwnedCargoManipulation=1;
		Model="\FOG_MOD\Gear\MISC\BoltCutters\FOG_BoltCutters.p3d";
		inventorySlot[]=
		{
			"Backpack_1"
		};
		weight=300;
		hiddenSelections[]=
		{
			"camo"
		};
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
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
								"FOG_MOD\Gear\MISC\BoltCutters\data\Bolt_Cutters.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Gear\MISC\BoltCutters\data\Bolt_Cutters.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Gear\MISC\BoltCutters\data\Bolt_Cutters.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Gear\MISC\BoltCutters\data\Bolt_Cutters.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Gear\MISC\BoltCutters\data\Bolt_Cutters.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class FOG_BoltCutters: FOG_BoltCutters_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\MISC\BoltCutters\data\Bolt_Cutters_co.paa"
		};
	};
};
