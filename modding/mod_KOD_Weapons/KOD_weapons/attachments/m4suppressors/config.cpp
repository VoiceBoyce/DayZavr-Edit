class CfgPatches
{
	class KOD_Weapons_M4Suppressor
	{
		units[]=
		{
			"KOD_M4Suppressor_NoCover",
			"KOD_M4Suppressor_Black",
			"KOD_M4Suppressor_Tan",
			"KOD_M4Suppressor_Camo"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Muzzles"
		};
	};
};
class cfgVehicles
{
	class ItemSuppressor;
	class KOD_M4Suppressor_Base: ItemSuppressor
	{
		scope=0;
		displayName="Глушитель AR";
		descriptionShort="$STR_cfgVehicles_M4_Suppressor1";
		model="KOD_weapons\attachments\M4Suppressors\m4suppressor.p3d";
		rotationFlags=17;
		reversed=0;
		weight=100;
		itemSize[]={4,1};
		itemModelLength=0.2;
		barrelArmor=3500;
		lootCategory="Attachments";
		lootTag[]=
		{
			"Military_east"
		};
		inventorySlot[]=
		{
			"weaponMuzzleM4"
		};
		simulation="ItemSuppressor";
		dispersionModifier=-0.00050000002;
		dispersionCondition="true";
		noiseShootModifier=-0.94999999;
		recoilModifier[]={0.85000002,0.85000002,0.85000002};
		swayModifier[]={1.1,1.1,1.1};
		selectionFireAnim="zasleh";
		soundIndex=1;
		muzzlePos="usti hlavne";
		hiddenSelections[]=
		{
			"color1",
			"color2"
		};
		class Particles
		{
			class OnFire
			{
				class MuzzleFlashAssault
				{
					overrideParticle="weapon_shot_fnx_02";
					onlyWithinHealthLabel[]={0,3};
				};
			};
			class OnOverheating
			{
				shotsToStartOverheating=10;
				maxOverheatingValue=30;
				overheatingDecayInterval=1;
				class SmokingBarrel
				{
					overrideParticle="smoking_barrel_small";
					onlyWithinRainLimits[]={0,0.2};
				};
				class SmokingBarrelHotSteamSmall
				{
					overrideParticle="smoking_barrel_steam_small";
					positionOffset[]={0,0.1,0};
					onlyWithinRainLimits[]={0.2,1};
				};
			};
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=5;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\attachments\M4Suppressors\data\suppressor1.rvmat",
								"KOD_weapons\attachments\M4Suppressors\data\suppressor2.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\attachments\M4Suppressors\data\suppressor1.rvmat",
								"KOD_weapons\attachments\M4Suppressors\data\suppressor2.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\attachments\M4Suppressors\data\suppressor1_damage.rvmat",
								"KOD_weapons\attachments\M4Suppressors\data\suppressor2_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\attachments\M4Suppressors\data\suppressor1_damage.rvmat",
								"KOD_weapons\attachments\M4Suppressors\data\suppressor2_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\attachments\M4Suppressors\data\suppressor1_destruct.rvmat",
								"KOD_weapons\attachments\M4Suppressors\data\suppressor2_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_M4Suppressor_NoCover: KOD_M4Suppressor_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\M4Suppressors\data\suppressor1_co.paa"
		};
	};
	class KOD_M4Suppressor_Black: KOD_M4Suppressor_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\M4Suppressors\data\suppressor1_co.paa",
			"KOD_weapons\attachments\M4Suppressors\data\suppressor2_black_co.paa"
		};
	};
	class KOD_M4Suppressor_Tan: KOD_M4Suppressor_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\M4Suppressors\data\suppressor1_co.paa",
			"KOD_weapons\attachments\M4Suppressors\data\suppressor2_tan_co.paa"
		};
	};
	class KOD_M4Suppressor_Camo: KOD_M4Suppressor_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\M4Suppressors\data\suppressor1_co.paa",
			"KOD_weapons\attachments\M4Suppressors\data\suppressor2_multicam_co.paa"
		};
	};
};
