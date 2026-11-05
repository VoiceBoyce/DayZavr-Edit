class CfgPatches
{
	class KOD_Weapons_AEK971
	{
		units[]={};
		weapons[]=
		{
			"KOD_AEK971"
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
	class KOD_AEK971_Base: Rifle_Base
	{
		scope=0;
		displayName="AEK-971";
		descriptionShort="$STR_KOD_AEK971";
		model="KOD_weapons\weapons\AEK\aek.p3d";
		attachments[]=
		{
			"weaponWrap",
			"weaponOpticsAK",
			"weaponMuzzleAK",
			"KODSticker1",
			"KODSticker2",
			"KODSticker3"
		};
		itemSize[]={8,3};
		weight=3530;
		absorbency=0;
		repairableWithKits[]={1};
		repairCosts[]={30,25};
		PPDOFProperties[]={1,0.5,50,160,4,10};
		WeaponLength=0.66000003;
		barrelArmor=3.743;
		initSpeedMultiplier=1;
		chamberSize=1;
		chamberedRound="";
		chamberableFrom[]=
		{
			"Ammo_545x39",
			"Ammo_545x39Tracer"
		};
		magazines[]=
		{
			"Mag_AK74_30Rnd",
			"Mag_AK74_30Rnd_Black",
			"Mag_AK74_30Rnd_Green",
			"Mag_AK74_45Rnd"
		};
		ejectType=1;
		recoilModifier[]={0.5,0.60000002,0.44999999};
		swayModifier[]={1.8,1.8,1.1};
		simpleHiddenSelections[]=
		{
			"hide_barrel"
		};
		hiddenSelections[]=
		{
			"color"
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
				"AK74_Shot_1st_SoundSet",
				"AK74_Shot_1st_iterior_SoundSet",
				"AK74_Tail_SoundSet",
				"AK74_InteriorTail_SoundSet",
				"AK74_Slapback_SoundSet",
				"AK74_Tail_2D_SoundSet"
			};
			soundSetShotExt[]=
			{
				
				{
					"AK74_1st_silencer_SoundSet",
					"AK74_silencerTail_SoundSet",
					"AK74_silencerInteriorTail_SoundSet"
				},
				
				{
					"AK74_1st_silencerHomeMade_SoundSet",
					"AK74_silencerHomeMadeTail_SoundSet",
					"AK74_silencerInteriorHomeMadeTail_SoundSet"
				}
			};
			reloadTime=0.075000003;
			recoil="recoil_AK74";
			recoilProne="recoil_AK74_prone";
			dispersion=0.0020000001;
			magazineSlot="magazine";
		};
		class FullAuto: Mode_FullAuto
		{
			soundSetShot1st[]=
			{
				"AK74_Shot_1st_SoundSet",
				"AK74_Shot_1st_iterior_SoundSet"
			};
			soundSetShot[]=
			{
				"AK74_Shot_SoundSet",
				"AK74_Shot_iterior_SoundSet",
				"AK74_Tail_SoundSet",
				"AK74_InteriorTail_SoundSet",
				"AK74_Slapback_SoundSet",
				"AK74_Tail_2D_SoundSet"
			};
			soundSetShotExt1st[]=
			{
				
				{
					"AK74_1st_silencer_SoundSet"
				},
				
				{
					"AK74_1st_silencerHomeMade_SoundSet"
				}
			};
			soundSetShotExt[]=
			{
				
				{
					"AK74_silencer_SoundSet",
					"AK74_silencerTail_SoundSet",
					"AK74_silencerInteriorTail_SoundSet"
				},
				
				{
					"AK74_silencerHomeMade_SoundSet",
					"AK74_silencerHomeMadeTail_SoundSet",
					"AK74_silencerInteriorHomeMadeTail_SoundSet"
				}
			};
			reloadTime=0.057999998;
			recoil="recoil_AK74";
			recoilProne="recoil_AK74_prone";
			dispersion=0.0020000001;
			magazineSlot="magazine";
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
					overrideParticle="weapon_shot_ak74_01";
					ignoreIfSuppressed=1;
					illuminateWorld=1;
					positionOffset[]={-0.1,0,0};
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
				maxOverheatingValue=40;
				shotsToStartOverheating=5;
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
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=350;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\AEK\data\aek.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\AEK\data\aek.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\AEK\data\aek_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\AEK\data\aek_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\AEK\data\aek_destruct.rvmat"
							}
						}
					};
				};
			};
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
	};
	class KOD_AEK971: KOD_AEK971_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\AEK\data\aek_co.paa"
		};
	};
};
