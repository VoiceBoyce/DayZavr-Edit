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
	class TGK_SetComplete_EOD_Specialist_Helmet_Colorbase: Clothing
	{
		scope=0;
		displayName="$STR_EOD_Specialist_Helmet01";
		descriptionShort="$STR_EOD_Specialist_Helmet02";
		model="SetComplete_EOD_Specialist\set\Helmet\Helmet_g.p3d";
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
		weight=2500;
		quickBarBonus=0;
		itemSize[]={4,4};
		varWetMax=0.048999999;
		heatIsolation=0.85000002;
		hiddenSelections[]=
		{
			"belt",
			"helmet_strap",
			"clips",
			"def_2_ear",
			"glass",
			"helm",
			"helm1",
			"manda",
			"patch_golova",
			"patch_golova2",
			"pnv"
		};
		headSelectionsToHide[]=
		{
			"Clipping_Mich2001"
		};
		class ClothingTypes
		{
			male="SetComplete_EOD_Specialist\set\Helmet\Helmet_m.p3d";
			female="SetComplete_EOD_Specialist\set\Helmet\Helmet_m.p3d";
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
								"SetComplete_EOD_Specialist\set\data\belt\Belt.rvmat",
								"SetComplete_EOD_Specialist\set\data\helmet_strap\helmet_strap.rvmat",
								"SetComplete_EOD_Specialist\set\data\clips\clips.rvmat",
								"SetComplete_EOD_Specialist\set\data\def_2_ear\def_2_ear.rvmat",
								"SetComplete_EOD_Specialist\set\data\glass\glass.rvmat",
								"SetComplete_EOD_Specialist\set\data\helm\helm.rvmat",
								"SetComplete_EOD_Specialist\set\data\helm1\helm1.rvmat",
								"SetComplete_EOD_Specialist\set\data\manda\manda.rvmat",
								"SetComplete_EOD_Specialist\set\data\patch_golova\patch_golova.rvmat",
								"SetComplete_EOD_Specialist\set\data\patch_golova2\patch_golova2.rvmat",
								"SetComplete_EOD_Specialist\set\data\pnv\pnv.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"SetComplete_EOD_Specialist\set\data\belt\Belt.rvmat",
								"SetComplete_EOD_Specialist\set\data\helmet_strap\helmet_strap.rvmat",
								"SetComplete_EOD_Specialist\set\data\clips\clips.rvmat",
								"SetComplete_EOD_Specialist\set\data\def_2_ear\def_2_ear.rvmat",
								"SetComplete_EOD_Specialist\set\data\glass\glass.rvmat",
								"SetComplete_EOD_Specialist\set\data\helm\helm.rvmat",
								"SetComplete_EOD_Specialist\set\data\helm1\helm1.rvmat",
								"SetComplete_EOD_Specialist\set\data\manda\manda.rvmat",
								"SetComplete_EOD_Specialist\set\data\patch_golova\patch_golova.rvmat",
								"SetComplete_EOD_Specialist\set\data\patch_golova2\patch_golova2.rvmat",
								"SetComplete_EOD_Specialist\set\data\pnv\pnv.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"SetComplete_EOD_Specialist\set\data\belt\Belt_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\helmet_strap\helmet_strap_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\clips\clips_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\def_2_ear\def_2_ear_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\glass\glass_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\helm\helm_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\helm1\helm1_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\manda\manda_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\patch_golova\patch_golova_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\patch_golova2\patch_golova2_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\pnv\pnv_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"SetComplete_EOD_Specialist\set\data\belt\Belt_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\helmet_strap\helmet_strap_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\clips\clips_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\def_2_ear\def_2_ear_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\glass\glass_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\helm\helm_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\helm1\helm1_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\manda\manda_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\patch_golova\patch_golova_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\patch_golova2\patch_golova2_damage.rvmat",
								"SetComplete_EOD_Specialist\set\data\pnv\pnv_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"SetComplete_EOD_Specialist\set\data\belt\Belt_destruct.rvmat",
								"SetComplete_EOD_Specialist\set\data\helmet_strap\helmet_strap_destruct.rvmat",
								"SetComplete_EOD_Specialist\set\data\clips\clips_destruct.rvmat",
								"SetComplete_EOD_Specialist\set\data\def_2_ear\def_2_ear_destruct.rvmat",
								"SetComplete_EOD_Specialist\set\data\glass\glass_destruct.rvmat",
								"SetComplete_EOD_Specialist\set\data\helm\helm_destruct.rvmat",
								"SetComplete_EOD_Specialist\set\data\helm1\helm1_destruct.rvmat",
								"SetComplete_EOD_Specialist\set\data\manda\manda_destruct.rvmat",
								"SetComplete_EOD_Specialist\set\data\patch_golova\patch_golova_destruct.rvmat",
								"SetComplete_EOD_Specialist\set\data\patch_golova2\patch_golova2_destruct.rvmat",
								"SetComplete_EOD_Specialist\set\data\pnv\pnv_destruct.rvmat"
							}
						}
					};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage=0.31;
					};
					class Blood
					{
						damage=0.31;
					};
					class Shock
					{
						damage=0.31;
					};
				};
				class Melee
				{
					class Health
					{
						damage=0.31;
					};
					class Blood
					{
						damage=0.31;
					};
					class Shock
					{
						damage=0.31;
					};
				};
				class Infected
				{
					class Health
					{
						damage=0.31;
					};
					class Blood
					{
						damage=0.31;
					};
					class Shock
					{
						damage=0.31;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage=0.31;
					};
					class Blood
					{
						damage=0.31;
					};
					class Shock
					{
						damage=0.31;
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
	class TGK_EOD_Specialist_Helmet_Green: TGK_SetComplete_EOD_Specialist_Helmet_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"SetComplete_EOD_Specialist\set\data\belt\Belt_co.paa",
			"SetComplete_EOD_Specialist\set\data\helmet_strap\helmet_strap_co.paa",
			"SetComplete_EOD_Specialist\set\data\clips\clips_co.paa",
			"SetComplete_EOD_Specialist\set\data\def_2_ear\def_2_ear_co.paa",
			"SetComplete_EOD_Specialist\set\data\glass\glass_ca.paa",
			"SetComplete_EOD_Specialist\set\data\helm\helm_co.paa",
			"SetComplete_EOD_Specialist\set\data\helm1\helm1_co.paa",
			"SetComplete_EOD_Specialist\set\data\manda\manda_co.paa",
			"SetComplete_EOD_Specialist\set\data\patch_golova\patch_golova_co.paa",
			"SetComplete_EOD_Specialist\set\data\patch_golova2\patch_golova2_co.paa",
			"SetComplete_EOD_Specialist\set\data\pnv\pnv_co.paa"
		};
	};
	class TGK_EOD_Specialist_Helmet_Black: TGK_SetComplete_EOD_Specialist_Helmet_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"SetComplete_EOD_Specialist\set\data\belt\Belt_BaO.paa",
			"SetComplete_EOD_Specialist\set\data\helmet_strap\helmet_strap_Bao.paa",
			"SetComplete_EOD_Specialist\set\data\clips\clips_co.paa",
			"SetComplete_EOD_Specialist\set\data\def_2_ear\def_2_ear_co.paa",
			"SetComplete_EOD_Specialist\set\data\glass\glass_Bao_ca.paa",
			"SetComplete_EOD_Specialist\set\data\helm\helm_Bao.paa",
			"SetComplete_EOD_Specialist\set\data\helm1\helm1_Bao.paa",
			"SetComplete_EOD_Specialist\set\data\manda\manda_Bao.paa",
			"SetComplete_EOD_Specialist\set\data\patch_golova\patch_golova_co.paa",
			"SetComplete_EOD_Specialist\set\data\patch_golova2\patch_golova2_co.paa",
			"SetComplete_EOD_Specialist\set\data\pnv\pnv_co.paa"
		};
	};
};
