class CfgPatches
{
	class KOD_Weapons_MP5SD
	{
		units[]={};
		weapons[]=
		{
			"KOD_MP5SD"
		};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Firearms"
		};
	};
};
class Mode_Safe;
class Mode_SemiAuto;
class Mode_Burst;
class Mode_FullAuto;
class OpticsInfoRifle;
class cfgWeapons
{
	class Rifle_Base;
	class KOD_MP5SD_Base: Rifle_Base
	{
		scope=0;
		displayName="MP5SD";
		descriptionShort="$STR_KOD_MP5SD";
		model="KOD_weapons\weapons\MP5SD\mp5sd.p3d";
		attachments[]=
		{
			"weaponWrap",
			"KODSticker1",
			"KODSticker2",
			"KODSticker3"
		};
		itemSize[]={6,3};
		weight=3400;
		absorbency=0;
		repairableWithKits[]={1};
		repairCosts[]={30,25};
		PPDOFProperties[]={1,0.75,40,180,4,10};
		WeaponLength=0.68326902;
		initSpeedMultiplier=1;
		chamberSize=1;
		chamberedRound="";
		chamberableFrom[]=
		{
			"Ammo_9x19"
		};
		magazines[]=
		{
			"Mag_MP5_15Rnd",
			"Mag_MP5_30Rnd"
		};
		barrelArmor=2.4000001;
		ejectType=1;
		recoilModifier[]={0.69999999,0.69999999,0.69999999};
		swayModifier[]={1.2,1.2,1};
		hiddenSelections[]=
		{
			"color1",
			"color2",
			"color3"
		};
		modes[]=
		{
			"FullAuto",
			"SemiAuto"
		};
		class SemiAuto: Mode_SemiAuto
		{
			soundSetShot[]=
			{
				"PB6P9_silencerPro_SoundSet",
				"PB6P9_silencerTail_SoundSet",
				"PB6P9_silencerInteriorTail_SoundSet"
			};
			reloadTime=0.125;
			dispersion=0.0024999999;
			magazineSlot="magazine";
		};
		class FullAuto: Mode_FullAuto
		{
			soundSetShot[]=
			{
				"PB6P9_silencerPro_SoundSet",
				"PB6P9_silencerTail_SoundSet",
				"PB6P9_silencerInteriorTail_SoundSet"
			};
			reloadTime=0.059999999;
			dispersion=0.0024999999;
			magazineSlot="magazine";
		};
		class NoiseShoot
		{
			strength=3;
			type="shot";
		};
		class OpticsInfo: OpticsInfoRifle
		{
			memoryPointCamera="eye";
			modelOptics="-";
			distanceZoomMin=25;
			distanceZoomMax=150;
			discreteDistance[]={25,50,100,150};
			discreteDistanceInitIndex=0;
		};
		class Particles
		{
			class OnFire
			{
				class MuzzleFlash
				{
					overrideParticle="weapon_shot_fnx_02";
				};
				class SmokeCloud
				{
					overrideParticle="weapon_shot_winded_smoke_small";
				};
				class ChamberSmokeBurst
				{
					overrideParticle="weapon_shot_chamber_smoke";
					overridePoint="Nabojnicestart";
					overrideDirectionPoint="Nabojniceend";
				};
			};
			class OnOverheating
			{
				shotsToStartOverheating=5;
				maxOverheatingValue=15;
				overheatingDecayInterval=1;
				class SmokingBarrel
				{
					overrideParticle="smoking_barrel_small";
					onlyWithinOverheatLimits[]={0,0.80000001};
					onlyWithinRainLimits[]={0,0.2};
				};
				class SmokingBarrelHotSteam
				{
					overrideParticle="smoking_barrel_steam_small";
					positionOffset[]={0.1,0.02,0};
					onlyWithinOverheatLimits[]={0,1};
					onlyWithinRainLimits[]={0.2,1};
				};
				class OpenChamberSmoke
				{
					onlyIfBoltIsOpen=1;
					overrideParticle="smoking_barrel_small";
					overridePoint="Nabojnicestart";
				};
			};
			class OnBulletCasingEject
			{
				class ChamberSmokeRaise
				{
					overrideParticle="weapon_shot_chamber_smoke_raise";
					overridePoint="Nabojnicestart";
				};
			};
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=200;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\MP5SD\data\body.rvmat",
								"KOD_weapons\weapons\MP5SD\data\hndgrd_a2.rvmat",
								"KOD_weapons\weapons\MP5SD\data\stck_a1.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\MP5SD\data\body.rvmat",
								"KOD_weapons\weapons\MP5SD\data\hndgrd_a2.rvmat",
								"KOD_weapons\weapons\MP5SD\data\stck_a1.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\MP5SD\data\body_damage.rvmat",
								"KOD_weapons\weapons\MP5SD\data\hndgrd_a2_damage.rvmat",
								"KOD_weapons\weapons\MP5SD\data\stck_a1_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\MP5SD\data\body_damage.rvmat",
								"KOD_weapons\weapons\MP5SD\data\hndgrd_a2_damage.rvmat",
								"KOD_weapons\weapons\MP5SD\data\stck_a1_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\MP5SD\data\body_destruct.rvmat",
								"KOD_weapons\weapons\MP5SD\data\hndgrd_a2_destruct.rvmat",
								"KOD_weapons\weapons\MP5SD\data\stck_a1_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_MP5SD: KOD_MP5SD_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\MP5SD\data\body_co.paa",
			"KOD_weapons\weapons\MP5SD\data\hndgrd_a2_co.paa",
			"KOD_weapons\weapons\MP5SD\data\stck_a1_co.paa"
		};
	};
};
