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
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class TGK_SetComplete_EOD_Specialist_Backpack_Colorbase: Clothing
	{
		displayName="$STR_EOD_Specialist_Backpack01";
		descriptionShort="$STR_EOD_Specialist_Backpack02";
		model="SetComplete_EOD_Specialist\set\backpack\Backpack_g.p3d";
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
		varWetMax=0.25;
		heatIsolation=0.25;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		soundAttType="Outdoor";
		randomQuantity=3;
		hiddenSelections[]=
		{
			"backpack",
			"clips",
			"velcro"
		};
		class ClothingTypes
		{
			male="SetComplete_EOD_Specialist\set\backpack\Backpack_m.p3d";
			female="SetComplete_EOD_Specialist\set\backpack\Backpack_m.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=200;
					transferToAttachmentsCoef=0.5;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"SetComplete_EOD_Specialist\set\data\backpack\backpack.rvmat",
								"SetComplete_EOD_Specialist\set\data\clips\clips.rvmat",
								"SetComplete_EOD_Specialist\set\data\velcro\velcro.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"SetComplete_EOD_Specialist\set\data\backpack\backpack.rvmat",
								"SetComplete_EOD_Specialist\set\data\clips\clips.rvmat",
								"SetComplete_EOD_Specialist\set\data\velcro\velcro.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"SetComplete_EOD_Specialist\set\data\backpack\backpack_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\clips\clips_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\velcro\velcro_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"SetComplete_EOD_Specialist\set\data\backpack\backpack_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\clips\clips_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\velcro\velcro_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"SetComplete_EOD_Specialist\set\data\backpack\backpack_destruct.rvmat",
								"SetComplete_EOD_Specialist\set\data\clips\clips_destruct.rvmat",
								"SetComplete_EOD_Specialist\set\data\velcro\velcro_destruct.rvmat"
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
	class TGK_EOD_Specialist_Backpack_Green: TGK_SetComplete_EOD_Specialist_Backpack_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"SetComplete_EOD_Specialist\set\data\backpack\backpack_co.paa",
			"SetComplete_EOD_Specialist\set\data\clips\clips_co.paa",
			"SetComplete_EOD_Specialist\set\data\velcro\velcro_co.paa"
		};
	};
	class TGK_EOD_Specialist_Backpack_Black: TGK_SetComplete_EOD_Specialist_Backpack_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"SetComplete_EOD_Specialist\set\data\backpack\backpack_BaO.paa",
			"SetComplete_EOD_Specialist\set\data\clips\clips_co.paa",
			"SetComplete_EOD_Specialist\set\data\velcro\velcro_Bao.paa"
		};
	};
};
