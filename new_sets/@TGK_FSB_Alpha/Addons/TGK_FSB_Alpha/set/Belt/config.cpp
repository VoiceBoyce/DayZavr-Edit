class CfgPatches
{
	class TGK_FSB_ALPHA
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
	class MilitaryBelt;
	class TGK_FSB_ALPHA_Belt_ColorBase: MilitaryBelt
	{
		scope=0;
		displayName="$STR_FSB_ALPHA_Belt_01";
		descriptionShort="$STR_FSB_ALPHA_Belt_02";
		model="TGK_FSB_Alpha\set\Belt\Belt_g.p3d";
		attachments[]=
		{
			"DumpPouch"
		};
		vehicleClass="Clothing";
		simulation="clothing";
		inventorySlot="Hips";
		itemInfo[]=
		{
			"Clothing",
			"Hips"
		};
		itemSize[]={4,1};
		quickBarBonus=2;
		varWetMax=0.34999999;
		heatIsolation=0.15000001;
		weight=550;
		repairableWithKits[]={5,3};
		repairCosts[]={30,25};
		rotationFlags=16;
		lootCategory="Crafted";
		hiddenSelections[]=
		{
			"camo"
		};
		class ClothingTypes
		{
			male="TGK_FSB_Alpha\set\Belt\Belt_m.p3d";
			female="TGK_FSB_Alpha\set\Belt\Belt_m.p3d";
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
								"TGK_FSB_Alpha\set\Belt\data\WarbeltVector.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_FSB_Alpha\set\Belt\data\WarbeltVector.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_FSB_Alpha\set\Belt\data\WarbeltVector_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_FSB_Alpha\set\Belt\data\WarbeltVector_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_FSB_Alpha\set\Belt\data\WarbeltVector_destruct.rvmat"
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
	class TGK_FSB_ALPHA_Belt_Alpine: TGK_FSB_ALPHA_Belt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Belt\data\WarbeltVector_Alpine.paa"
		};
	};
	class TGK_FSB_ALPHA_Belt_Black: TGK_FSB_ALPHA_Belt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Belt\data\WarbeltVector_Black.paa"
		};
	};
	class TGK_FSB_ALPHA_Belt_Black_MC: TGK_FSB_ALPHA_Belt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Belt\data\WarbeltVector_Black_MC.paa"
		};
	};
	class TGK_FSB_ALPHA_Belt_MC: TGK_FSB_ALPHA_Belt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Belt\data\WarbeltVector_MC.paa"
		};
	};
	class TGK_FSB_ALPHA_Belt_ODGreen: TGK_FSB_ALPHA_Belt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Belt\data\WarbeltVector_ODGreen.paa"
		};
	};
	class TGK_FSB_ALPHA_Belt_Tropic_MC: TGK_FSB_ALPHA_Belt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Belt\data\WarbeltVector_Tropic_MC.paa"
		};
	};
};
