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
	class TGK_SetComplete_EOD_Specialist_Belt_Colorbase: Clothing
	{
		scope=0;
		displayName="$STR_EOD_Specialist_Belt01";
		descriptionShort="$STR_EOD_Specialist_Belt02";
		model="SetComplete_EOD_Specialist\set\Belt\Belt_g.p3d";
		vehicleClass="Clothing";
		simulation="clothing";
		inventorySlot="Hips";
		itemInfo[]=
		{
			"Clothing",
			"Hips"
		};
		attachments[]=
		{
			"Pistol"
		};
		itemSize[]={5,2};
		quickBarBonus=2;
		varWetMax=0.048999999;
		heatIsolation=0.5;
		weight=860;
		itemsCargoSize[]={10,3};
		repairableWithKits[]={5,3};
		repairCosts[]={30,25};
		lootCategory="Crafted";
		hiddenSelections[]=
		{
			"belt",
			"belt_medical",
			"belt_stap",
			"clips",
			"def_1",
			"def_groin",
			"granad_pocket",
			"grenade",
			"helmet_strap",
			"holster",
			"karm3",
			"tourniq",
			"velcro"
		};
		class ClothingTypes
		{
			male="SetComplete_EOD_Specialist\set\Belt\Belt_m.p3d";
			female="SetComplete_EOD_Specialist\set\Belt\Belt_m.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=220;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"SetComplete_EOD_Specialist\set\data\belt\Belt.rvmat",
								"SetComplete_EOD_Specialist\set\data\belt_medical\Belt_medical.rvmat",
								"SetComplete_EOD_Specialist\set\data\belt_stap\belt_stap.rvmat",
								"SetComplete_EOD_Specialist\set\data\clips\clips.rvmat",
								"SetComplete_EOD_Specialist\set\data\def_1\def_1.rvmat",
								"SetComplete_EOD_Specialist\set\data\def_3_groin\def_3_groin.rvmat",
								"SetComplete_EOD_Specialist\set\data\granad_pocket\granad_pocket.rvmat",
								"SetComplete_EOD_Specialist\set\data\granad\granad.rvmat",
								"SetComplete_EOD_Specialist\set\data\helmet_strap\helmet_strap.rvmat",
								"SetComplete_EOD_Specialist\set\data\holster\holster.rvmat",
								"SetComplete_EOD_Specialist\set\data\karm3\karm3.rvmat",
								"SetComplete_EOD_Specialist\set\data\tourniq\tourniq.rvmat",
								"SetComplete_EOD_Specialist\set\data\velcro\velcro.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"SetComplete_EOD_Specialist\set\data\belt\Belt.rvmat",
								"SetComplete_EOD_Specialist\set\data\belt_medical\Belt_medical.rvmat",
								"SetComplete_EOD_Specialist\set\data\belt_stap\belt_stap.rvmat",
								"SetComplete_EOD_Specialist\set\data\clips\clips.rvmat",
								"SetComplete_EOD_Specialist\set\data\def_1\def_1.rvmat",
								"SetComplete_EOD_Specialist\set\data\def_3_groin\def_3_groin.rvmat",
								"SetComplete_EOD_Specialist\set\data\granad_pocket\granad_pocket.rvmat",
								"SetComplete_EOD_Specialist\set\data\granad\granad.rvmat",
								"SetComplete_EOD_Specialist\set\data\helmet_strap\helmet_strap.rvmat",
								"SetComplete_EOD_Specialist\set\data\holster\holster.rvmat",
								"SetComplete_EOD_Specialist\set\data\karm3\karm3.rvmat",
								"SetComplete_EOD_Specialist\set\data\tourniq\tourniq.rvmat",
								"SetComplete_EOD_Specialist\set\data\velcro\velcro.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"SetComplete_EOD_Specialist\set\data\belt\Belt_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\belt_medical\Belt_medical_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\belt_stap\belt_stap_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\clips\clips_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\def_1\def_1_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\def_3_groin\def_3_groin_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\granad_pocket\granad_pocket_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\granad\granad_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\helmet_strap\helmet_strap_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\holster\holster_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\karm3\karm3_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\tourniq\tourniq_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\velcro\velcro_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"SetComplete_EOD_Specialist\set\data\belt\Belt_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\belt_medical\Belt_medical_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\belt_stap\belt_stap_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\clips\clips_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\def_1\def_1_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\def_3_groin\def_3_groin_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\granad_pocket\granad_pocket_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\granad\granad_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\helmet_strap\helmet_strap_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\holster\holster_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\karm3\karm3_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\tourniq\tourniq_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\velcro\velcro_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"SetComplete_EOD_Specialist\set\data\belt\Belt_destruct.rvmat",
								"SetComplete_EOD_Specialist\set\data\belt_medical\Belt_medical_destruct.rvmat",
								"SetComplete_EOD_Specialist\set\data\belt_stap\belt_stap_destruct.rvmat",
								"SetComplete_EOD_Specialist\set\data\clips\clips_destruct.rvmat",
								"SetComplete_EOD_Specialist\set\data\def_1\def_1_destruct.rvmat",
								"SetComplete_EOD_Specialist\set\data\def_3_groin\def_3_groin_destruct.rvmat",
								"SetComplete_EOD_Specialist\set\data\granad_pocket\granad_pocket_destruct.rvmat",
								"SetComplete_EOD_Specialist\set\data\granad\granad_destruct.rvmat",
								"SetComplete_EOD_Specialist\set\data\helmet_strap\helmet_strap_destruct.rvmat",
								"SetComplete_EOD_Specialist\set\data\holster\holster_destruct.rvmat",
								"SetComplete_EOD_Specialist\set\data\karm3\karm3_destruct.rvmat",
								"SetComplete_EOD_Specialist\set\data\tourniq\tourniq_destruct.rvmat",
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
	class TGK_EOD_Specialist_Belt_Green: TGK_SetComplete_EOD_Specialist_Belt_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"SetComplete_EOD_Specialist\set\data\belt\Belt_co.paa",
			"SetComplete_EOD_Specialist\set\data\belt_medical\Belt_medical_co.paa",
			"SetComplete_EOD_Specialist\set\data\belt_stap\belt_stap_co.paa",
			"SetComplete_EOD_Specialist\set\data\clips\clips_co.paa",
			"SetComplete_EOD_Specialist\set\data\def_1\def_1_co.paa",
			"SetComplete_EOD_Specialist\set\data\def_3_groin\def_3_groin_co.paa",
			"SetComplete_EOD_Specialist\set\data\granad_pocket\granad_pocket_co.paa",
			"SetComplete_EOD_Specialist\set\data\granad\granad_co.paa",
			"SetComplete_EOD_Specialist\set\data\helmet_strap\helmet_strap_co.paa",
			"SetComplete_EOD_Specialist\set\data\holster\holster_co.paa",
			"SetComplete_EOD_Specialist\set\data\karm3\karm3_co.paa",
			"SetComplete_EOD_Specialist\set\data\tourniq\tourniq_co.paa",
			"SetComplete_EOD_Specialist\set\data\velcro\velcro_co.paa"
		};
	};
	class TGK_EOD_Specialist_Belt_Black: TGK_SetComplete_EOD_Specialist_Belt_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"SetComplete_EOD_Specialist\set\data\belt\Belt_BaO.paa",
			"SetComplete_EOD_Specialist\set\data\belt_medical\Belt_medical_BaO.paa",
			"SetComplete_EOD_Specialist\set\data\belt_stap\belt_stap_co.paa",
			"SetComplete_EOD_Specialist\set\data\clips\clips_co.paa",
			"SetComplete_EOD_Specialist\set\data\def_1\def_1_BaO.paa",
			"SetComplete_EOD_Specialist\set\data\def_3_groin\def_3_groin_Bao.paa",
			"SetComplete_EOD_Specialist\set\data\granad_pocket\granad_pocket_Bao.paa",
			"SetComplete_EOD_Specialist\set\data\granad\granad_co.paa",
			"SetComplete_EOD_Specialist\set\data\helmet_strap\helmet_strap_Bao.paa",
			"SetComplete_EOD_Specialist\set\data\holster\holster_Bao.paa",
			"SetComplete_EOD_Specialist\set\data\karm3\karm3_Bao.paa",
			"SetComplete_EOD_Specialist\set\data\tourniq\tourniq_Bao.paa",
			"SetComplete_EOD_Specialist\set\data\velcro\velcro_Bao.paa"
		};
	};
};
