class CfgPatches
{
	class FOG_Slammer_Stuff
	{
		units[]=
		{
			"FOG_SOCOM_Slammer_Black"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Weapons_Melee"
		};
	};
};
class CfgVehicles
{
	class SledgeHammer;
	class FOG_SOCOM_Slammer_Base: SledgeHammer
	{
		scope=0;
		debug_ItemCategory=2;
		displayName="SOCOM Slammer";
		descriptionShort="SOCOM Slammer — тактический кувалдоподобный инструмент из закалённой хромованадиевой стали с ударной головкой 6 кг. Противоотскоковая рукоятка с демпфером и гравитационно-оптимизированная форма обеспечивают сокрушительную силу удара при пробивании преград | The SOCOM Slammer is a sledgehammer-style breaching tool forged from tempered chromium-vanadium steel with 6kg impact head. Anti-vibration handle with shock absorber and gravity-optimized design deliver devastating kinetic energy for barrier penetration.";
		model="\FOG_MOD\Weapons\Melee\SF_Slammer\the_slammer.p3d";
		itemInfo[]=
		{
			"SledgeHammer"
		};
		rotationFlags=12;
		weight=5000;
		itemSize[]={2,6};
		itemBehaviour=2;
		repairableWithKits[]={5};
		repairCosts[]={25};
		openItemSpillRange[]={30,50};
		inventorySlot[]=
		{
			"Shoulder",
			"Melee",
			"Backpack_1"
		};
		hiddenSelections[]=
		{
			"camo"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Weapons\Melee\SF_Slammer\data\the_slammer.rvmat"
		};
		isMeleeWeapon=1;
		build_action_type=16;
		dismantle_action_type=16;
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
								"FOG_MOD\Weapons\Melee\SF_Slammer\data\the_slammer.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Weapons\Melee\SF_Slammer\data\the_slammer.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Weapons\Melee\SF_Slammer\data\the_slammer_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Weapons\Melee\SF_Slammer\data\the_slammer_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Weapons\Melee\SF_Slammer\data\the_slammer_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		class MeleeModes
		{
			class Default
			{
				ammo="MeleeSledgeHammer";
				range=1.8;
			};
			class Heavy
			{
				ammo="MeleeSledgeHammer_Heavy";
				range=1.8;
			};
			class Sprint
			{
				ammo="MeleeSledgeHammer_Heavy";
				range=3.7;
			};
		};
		soundImpactType="metal";
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickup_light
				{
					soundSet="hatchet_pickup_light_SoundSet";
					id=796;
				};
				class pickup
				{
					soundSet="hatchet_pickup_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="crowbar_drop_SoundSet";
					id=898;
				};
				class SledgeWoodHammer_loop
				{
					soundSet="SledgeWoodHammer_loop_SoundSet";
					id=1117;
				};
				class SledgeWoodHammer_end
				{
					soundSet="SledgeWoodHammer_end_SoundSet";
					id=1118;
				};
				class ShoulderR_Hide
				{
					soundset="ShoulderR_Hide_SoundSet";
					id=1210;
				};
				class ShoulderR_Show
				{
					soundset="ShoulderR_Show_SoundSet";
					id=1211;
				};
			};
		};
	};
	class FOG_SOCOM_Slammer_Black: FOG_SOCOM_Slammer_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Weapons\Melee\SF_Slammer\data\slammer_black_co.paa"
		};
	};
};
