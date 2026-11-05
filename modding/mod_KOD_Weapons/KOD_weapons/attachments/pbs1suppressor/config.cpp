class CfgPatches
{
	class KOD_Weapons_PBS1Suppressor
	{
		units[]=
		{
			"KOD_PBS1Suppressor"
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
	class KOD_PBS1Suppressor: ItemSuppressor
	{
		scope=2;
		displayName="Глушитель ПБС-1";
		descriptionShort="$STR_cfgVehicles_AK_Suppressor1";
		model="KOD_weapons\attachments\PBS1Suppressor\pbs1_suppressor.p3d";
		rotationFlags=17;
		reversed=0;
		weight=100;
		itemSize[]={3,1};
		itemModelLength=0.23;
		barrelArmor=2250;
		lootCategory="Attachments";
		lootTag[]=
		{
			"Military_east"
		};
		inventorySlot[]=
		{
			"weaponMuzzleAK"
		};
		simulation="ItemSuppressor";
		dispersionModifier=-0.00050000002;
		dispersionCondition="true";
		noiseShootModifier=-0.89999998;
		recoilModifier[]={0.89999998,0.89999998,0.89999998};
		swayModifier[]={1.3,1.3,1.3};
		selectionFireAnim="zasleh";
		soundIndex=1;
		muzzlePos="usti hlavne";
		hiddenSelections[]=
		{
			"color"
		};
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\PBS1Suppressor\data\pbs1_co.paa"
		};
		class Particles
		{
			class OnFire
			{
				class MuzzleFlashAssault
				{
					overrideParticle="weapon_shot_akm_02";
					onlyWithinHealthLabel[]={0,3};
				};
			};
			class OnOverheating
			{
				shotsToStartOverheating=2;
				maxOverheatingValue=20;
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
					hitpoints=6;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\attachments\PBS1Suppressor\data\pbs1.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\attachments\PBS1Suppressor\data\pbs1.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\attachments\PBS1Suppressor\data\pbs1_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\attachments\PBS1Suppressor\data\pbs1_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\attachments\PBS1Suppressor\data\pbs1_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
};
