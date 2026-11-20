class CfgPatches
{
	class KOD_Weapons_AKM
	{
		units[]={};
		weapons[]=
		{
			"KOD_AKM"
		};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Firearms",
			"DZ_Weapons_Magazines"
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
	class KOD_AKM_Base: Rifle_Base
	{
		scope=0;
		displayName="$STR_cfgWeapons_AKM0";
		descriptionShort="$STR_cfgWeapons_AKM1";
		model="KOD_weapons\weapons\AKM\ak.p3d";
		attachments[]=
		{
			"weaponWrap",
			"weaponOptics",
			"weaponMuzzleAK",
			"KODSticker1",
			"KODSticker2",
			"KODSticker3"
		};
		itemSize[]={8,3};
		weight=2540;
		absorbency=0;
		repairableWithKits[]={1};
		repairCosts[]={25};
		PPDOFProperties[]={1,0.5,20,170,4,10};
		WeaponLength=0.85906702;
		initSpeedMultiplier=0.89999998;
		barrelArmor=4.4439998;
		chamberSize=1;
		chamberedRound="";
		chamberableFrom[]=
		{
			"Ammo_762x39",
			"Ammo_762x39Tracer"
		};
		magazines[]=
		{
			"Mag_AKM_30Rnd",
			"Mag_AKM_Palm30Rnd",
			"Mag_AKM_Palm30Rnd_Black",
			"Mag_AKM_Palm30Rnd_Green",
			"Mag_AKM_Drum75Rnd",
			"Mag_AKM_Drum75Rnd_Green",
			"Mag_AKM_Drum75Rnd_Black",
			"Mag_AKM_Drum_TEST"
		};
		ejectType=1;
		recoilModifier[]={0.60000002,0.60000002,0.60000002};
		swayModifier[]={2,2,1.05};
		simpleHiddenSelections[]=
		{
			"hide_barrel"
		};
		hiddenSelections[]=
		{
			"color1",
			"color2",
			"color3",
			"color4"
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
				"DZVR_KOD_AKM_Shot_SoundSet",
				"AK74_Shot_1st_iterior_SoundSet",
				"AK74_Tail_SoundSet",
				"AK74_InteriorTail_SoundSet",
				"AK74_Slapback_SoundSet",
				"AK74_Tail_2D_SoundSet"
			};
			soundSetShotExt[]=
			{
				
				{
					"DZVR_KOD_AKALPHA_Silencer_SoundSet",
					"AK74_silencerTail_SoundSet",
					"AK74_silencerInteriorTail_SoundSet"
				},
				
				{
					"AK74_1st_silencerHomeMade_SoundSet",
					"AK74_silencerHomeMadeTail_SoundSet",
					"AK74_silencerInteriorHomeMadeTail_SoundSet"
				}
			};
			reloadTime=0.125;
			dispersion=0.0015;
			magazineSlot="magazine";
		};
		class FullAuto: Mode_FullAuto
		{
			soundSetShot1st[]=
			{
				"DZVR_KOD_AKM_Shot_SoundSet",
				"AK74_Shot_1st_iterior_SoundSet"
			};
			soundSetShot[]=
			{
				"DZVR_KOD_AKM_Shot_SoundSet",
				"AK74_Shot_iterior_SoundSet",
				"AK74_Tail_SoundSet",
				"AK74_InteriorTail_SoundSet",
				"AK74_Slapback_SoundSet",
				"AK74_Tail_2D_SoundSet"
			};
			soundSetShotExt1st[]=
			{
				
				{
					"DZVR_KOD_AKALPHA_Silencer_SoundSet"
				},
				
				{
					"AK74_1st_silencerHomeMade_SoundSet"
				}
			};
			soundSetShotExt[]=
			{
				
				{
					"DZVR_KOD_AKALPHA_Silencer_SoundSet",
					"AK74_silencerTail_SoundSet",
					"AK74_silencerInteriorTail_SoundSet"
				},
				
				{
					"AK74_silencerHomeMade_SoundSet",
					"AK74_silencerHomeMadeTail_SoundSet",
					"AK74_silencerInteriorHomeMadeTail_SoundSet"
				}
			};
			reloadTime=0.1;
			dispersion=0.0015;
			magazineSlot="magazine";
		};
		class OpticsInfo: OpticsInfoRifle
		{
			memoryPointCamera="eye";
			discreteDistance[]={100,200,300,400,500,600,700,800,900,1000};
			discreteDistanceInitIndex=0;
			modelOptics="-";
			distanceZoomMin=100;
			distanceZoomMax=1000;
		};
		class Particles
		{
			class OnFire
			{
				class SmokeCloud
				{
					overrideParticle="weapon_shot_winded_smoke";
				};
				class MuzzleFlash
				{
					overrideParticle="weapon_shot_akm_01";
					ignoreIfSuppressed=1;
					illuminateWorld=1;
					positionOffset[]={-0.050000001,0,0};
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
				maxOverheatingValue=50;
				shotsToStartOverheating=10;
				overheatingDecayInterval=1;
				class SmokingBarrel1
				{
					overrideParticle="smoking_barrel_small";
					onlyWithinOverheatLimits[]={0,0.5};
					positionOffset[]={0.1,0,0};
					onlyWithinRainLimits[]={0,0.2};
				};
				class SmokingBarrelHot1
				{
					overrideParticle="smoking_barrel";
					onlyWithinOverheatLimits[]={0.5,0.80000001};
					positionOffset[]={0.1,0,0};
					onlyWithinRainLimits[]={0,0.2};
				};
				class SmokingBarrelHot3
				{
					overrideParticle="smoking_barrel_heavy";
					onlyWithinOverheatLimits[]={0.80000001,1};
					positionOffset[]={0.1,0,0};
					onlyWithinRainLimits[]={0,0.2};
				};
				class SmokingBarrelHotSteamSmall
				{
					overrideParticle="smoking_barrel_steam_small";
					positionOffset[]={0.30000001,0,0};
					onlyWithinRainLimits[]={0.2,0.5};
				};
				class SmokingBarrelHotSteam
				{
					overrideParticle="smoking_barrel_steam";
					positionOffset[]={0.30000001,0,0};
					onlyWithinRainLimits[]={0.5,1};
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
		class NoiseShoot
		{
			strength=82;
			type="shot";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=400;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\AKM\data\ak_01.rvmat",
								"KOD_weapons\weapons\AKM\data\ak_02.rvmat",
								"KOD_weapons\weapons\AKM\data\ak_03.rvmat",
								"KOD_weapons\weapons\AKM\data\ak_04.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\AKM\data\ak_01.rvmat",
								"KOD_weapons\weapons\AKM\data\ak_02.rvmat",
								"KOD_weapons\weapons\AKM\data\ak_03.rvmat",
								"KOD_weapons\weapons\AKM\data\ak_04.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\AKM\data\ak_01_damage.rvmat",
								"KOD_weapons\weapons\AKM\data\ak_02_damage.rvmat",
								"KOD_weapons\weapons\AKM\data\ak_03_damage.rvmat",
								"KOD_weapons\weapons\AKM\data\ak_04_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\AKM\data\ak_01_damage.rvmat",
								"KOD_weapons\weapons\AKM\data\ak_02_damage.rvmat",
								"KOD_weapons\weapons\AKM\data\ak_03_damage.rvmat",
								"KOD_weapons\weapons\AKM\data\ak_04_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\AKM\data\ak_01_destruct.rvmat",
								"KOD_weapons\weapons\AKM\data\ak_02_destruct.rvmat",
								"KOD_weapons\weapons\AKM\data\ak_03_destruct.rvmat",
								"KOD_weapons\weapons\AKM\data\ak_04_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_AKM: KOD_AKM_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\AKM\data\ak_01_co.paa",
			"KOD_weapons\weapons\AKM\data\ak_02_co.paa",
			"KOD_weapons\weapons\AKM\data\ak_03_co.paa",
			"KOD_weapons\weapons\AKM\data\ak_04_co.paa"
		};
	};
};
