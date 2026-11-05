class CfgPatches
{
	class KOD_Weapons_DTKPutnik
	{
		units[]=
		{
			"KOD_DTKPutnik_Black",
			"KOD_DTKPutnik_White",
			"KOD_DTKPutnik_Tan"
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
	class KOD_DTKPutnik_Base: ItemSuppressor
	{
		scope=0;
		displayName="ДТК Путник";
		descriptionShort="$STR_cfgVehicles_AK_Suppressor1";
		model="KOD_weapons\attachments\DTKPutnik\putnik.p3d";
		rotationFlags=17;
		reversed=0;
		weight=50;
		itemSize[]={2,2};
		itemModelLength=0.1;
		barrelArmor=3500;
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
		noiseShootModifier=-0.89999998;
		recoilModifier[]={0.80000001,0.80000001,0.80000001};
		swayModifier[]={1,1,1};
		selectionFireAnim="zasleh";
		soundIndex=1;
		muzzlePos="usti hlavne";
		hiddenSelections[]=
		{
			"color"
		};
		class Particles
		{
			class OnFire
			{
				class MuzzleFlash
				{
					overrideParticle="weapon_shot_fnx_02";
					onlyWithinHealthLabel[]={0,3};
				};
			};
			class OnOverheating
			{
				shotsToStartOverheating=2;
				maxOverheatingValue=10;
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
								"KOD_weapons\attachments\DTKPutnik\data\putnik.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\attachments\DTKPutnik\data\putnik.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\attachments\DTKPutnik\data\putnik_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\attachments\DTKPutnik\data\putnik_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\attachments\DTKPutnik\data\putnik_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_DTKPutnik_Black: KOD_DTKPutnik_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\DTKPutnik\data\putnik_black_co.paa"
		};
	};
	class KOD_DTKPutnik_White: KOD_DTKPutnik_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\DTKPutnik\data\putnik_white_co.paa"
		};
	};
	class KOD_DTKPutnik_Tan: KOD_DTKPutnik_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\attachments\DTKPutnik\data\putnik_tan_co.paa"
		};
	};
};
