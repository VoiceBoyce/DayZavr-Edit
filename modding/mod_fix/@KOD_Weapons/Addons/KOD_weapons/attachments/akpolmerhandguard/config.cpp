class CfgPatches
{
	class KOD_Weapons_AKPolymerHandguard
	{
		units[]=
		{
			"KOD_AK_PolymerHndgrd_Black",
			"KOD_AK_PolymerHndgrd_FDE"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Supports"
		};
	};
};
class cfgVehicles
{
	class AK_RailHndgrd;
	class KOD_AK_PolymerHndgrd_Base: AK_RailHndgrd
	{
		scope=0;
		displayName="Полим. Цевьё АК";
		descriptionShort="Облегчённое полимерное цевьё с боковой планкой крепления для автоматов на базе АК.";
		model="KOD_weapons\attachments\AKPolmerHandguard\ak_polymerhandguard.p3d";
		rotationFlags=17;
		reversed=0;
		weight=300;
		itemSize[]={2,1};
		inventorySlot[]=
		{
			"weaponHandguardAK"
		};
		hasRailFunctionality=1;
		recoilModifier[]={0.87,0.87,0.87};
		swayModifier[]={0.44999999,0.44999999,0.44999999};
		hiddenSelections[]=
		{
			"color"
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
								"KOD_weapons\attachments\AKPolmerHandguard\data\handguard.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\attachments\AKPolmerHandguard\data\handguard.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\attachments\AKPolmerHandguard\data\handguard_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\attachments\AKPolmerHandguard\data\handguard_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\attachments\AKPolmerHandguard\data\handguard_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		isMeleeWeapon=1;
		class MeleeModes
		{
			class Default
			{
				ammo="MeleeLightBlunt";
				range=1;
			};
			class Heavy
			{
				ammo="MeleeLightBlunt_Heavy";
				range=1;
			};
			class Sprint
			{
				ammo="MeleeLightBlunt_Heavy";
				range=2.8;
			};
		};
		soundImpactType="plastic";
	};
	class KOD_AK_PolymerHndgrd_Black: KOD_AK_PolymerHndgrd_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\AKPolmerHandguard\data\handguard_black_co.paa"
		};
	};
	class KOD_AK_PolymerHndgrd_FDE: KOD_AK_PolymerHndgrd_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\AKPolmerHandguard\data\handguard_fde_co.paa"
		};
	};
};
