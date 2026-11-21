class CfgPatches
{
	class KOD_Bipod_RIS
	{
		units[]=
		{
			"KOD_RIS_Bipod"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Firearms",
			"KOD_Bipod",
			"DZ_Scripts"
		};
	};
};
class cfgVehicles
{
	class KOD_Bipod_Base;
	class KOD_RIS_Bipod: KOD_Bipod_Base
	{
		scope=2;
		displayName="Сошки RIS";
		descriptionShort="Быстросъемные сошки для RIS-планки. Значительно уменьшают отдачу при стрельбе лёжа.";
		model="KOD_weapons\attachments\RISBipod\RISBipod.p3d";
		weight=200;
		itemSize[]={2,3};
		inventorySlot[]=
		{
			"KODRISBipod"
		};
		recoilDecrease=0.40000001;
		hiddenSelections[]=
		{
			"color"
		};
		hiddenSelectionsTextures[]=
		{
			"kod_weapons\weapons\gevar\textures\bipod_co.paa"
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
								"kod_weapons\weapons\gevar\textures\bipod.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"kod_weapons\weapons\gevar\textures\bipod.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"kod_weapons\weapons\gevar\textures\bipod_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"kod_weapons\weapons\gevar\textures\bipod_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"kod_weapons\weapons\gevar\textures\bipod_destruct.rvmat"
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
	};
};
class CfgNonAIVehicles
{
	class ProxyAttachment;
	class ProxyRISBipod: ProxyAttachment
	{
		scope=0;
		inventorySlot="KODRISBipod";
		model="KOD_weapons\attachments\RISBipod\RISBipod.p3d";
	};
};
class CfgSlots
{
	class Slot_KODRISBipod
	{
		name="KODRISBipod";
		displayName="Сошки/Рукоятка RIS";
		GhostIcon="set:kod_icons image:BipodGrip";
	};
};
