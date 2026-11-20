class CfgPatches
{
	class KOD_Weapons_Mk18
	{
		units[]={};
		weapons[]=
		{
			"KOD_Mk18"
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
	class KOD_Mk18_Base: Rifle_Base
	{
		scope=0;
		displayName="MK18";
		descriptionShort="Винтовка на базе М4 с низкопрофильным газоотводным блоком и комплектом направляющих для аксессуаров. Испольует патрон 5,56х45 мм НАТО.";
		weight=3500;
		model="KOD_weapons\weapons\Mk18\mk18.p3d";
		absorbency=0;
		repairableWithKits[]={1};
		repairCosts[]={25};
		PPDOFProperties[]={1,0.60000002,50,200,4,10};
		WeaponLength=1.1;
		initSpeedMultiplier=0.92000002;
		attachments[]=
		{
			"weaponWrap",
			"weaponOptics",
			"weaponFlashlight",
			"weaponMuzzleM4",
			"KODRISBipod",
			"KODSticker1",
			"KODSticker2",
			"KODSticker3"
		};
		itemSize[]={8,3};
		chamberSize=1;
		barrelArmor=1.2;
		chamberedRound="";
		chamberableFrom[]=
		{
			"Ammo_556x45",
			"Ammo_556x45Tracer"
		};
		magazines[]=
		{
			"Mag_STANAG_30Rnd",
			"Mag_STANAGCoupled_30Rnd",
			"Mag_STANAG_60Rnd",
			"Mag_CMAG_10Rnd",
			"Mag_CMAG_20Rnd",
			"Mag_CMAG_30Rnd",
			"Mag_CMAG_40Rnd",
			"Mag_CMAG_10Rnd_Green",
			"Mag_CMAG_20Rnd_Green",
			"Mag_CMAG_30Rnd_Green",
			"Mag_CMAG_40Rnd_Green",
			"Mag_CMAG_10Rnd_Black",
			"Mag_CMAG_100Rnd_Black",
			"Mag_CMAG_20Rnd_Black",
			"Mag_CMAG_30Rnd_Black",
			"Mag_CMAG_40Rnd_Black"
		};
		ejectType=1;
		recoilModifier[]={0.75,0.75,0.75};
		swayModifier[]={1.7,1.7,1.2};
		simpleHiddenSelections[]=
		{
			"hide_barrel",
			"hide_ironsights"
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
				"DZVR_KOD_M4_Shot_SoundSet",
				"M4A1_Shot_1st_iterior_SoundSet",
				"M4A1_Tail_SoundSet",
				"M4A1_InteriorTail_SoundSet",
				"M4A1_Slapback_SoundSet",
				"M4A1_Tail_2D_SoundSet"
			};
			soundSetShotExt[]=
			{
				
				{
					"DZVR_KOD_M4_Silencer_SoundSet",
					"M4A1_silencerTail_SoundSet",
					"M4A1_silencerInteriorTail_SoundSet"
				},
				
				{
					"M4A1_1st_silencerHomeMade_SoundSet",
					"M4A1_silencerHomeMadeTail_SoundSet",
					"M4A1_silencerInteriorHomeMadeTail_SoundSet"
				}
			};
			reloadTime=0.1;
			dispersion=0.0015;
			magazineSlot="magazine";
		};
		class FullAuto: Mode_FullAuto
		{
			soundSetShot1st[]=
			{
				"DZVR_KOD_M4_Shot_SoundSet",
				"M4A1_Shot_1st_iterior_SoundSet"
			};
			soundSetShot[]=
			{
				"DZVR_KOD_M4_Shot_SoundSet",
				"M4A1_Shot_iterior_SoundSet",
				"M4A1_Tail_SoundSet",
				"M4A1_InteriorTail_SoundSet",
				"M4A1_Slapback_SoundSet",
				"M4A1_Tail_2D_SoundSet"
			};
			soundSetShotExt1st[]=
			{
				
				{
					"DZVR_KOD_M4_Silencer_SoundSet"
				},
				
				{
					"M4A1_1st_silencerHomeMade_SoundSet"
				}
			};
			soundSetShotExt[]=
			{
				
				{
					"DZVR_KOD_M4_Silencer_SoundSet",
					"M4A1_silencerTail_SoundSet",
					"M4A1_silencerInteriorTail_SoundSet"
				},
				
				{
					"M4A1_silencerHomeMade_SoundSet",
					"M4A1_silencerHomeMadeTail_SoundSet",
					"M4A1_silencerInteriorHomeMadeTail_SoundSet"
				}
			};
			reloadTime=0.078000002;
			dispersion=0.0015;
			magazineSlot="magazine";
		};
		class OpticsInfo: OpticsInfoRifle
		{
			memoryPointCamera="eye";
			discreteDistance[]={100,200,300,400,500};
			discreteDistanceInitIndex=0;
			modelOptics="-";
			distanceZoomMin=100;
			distanceZoomMax=500;
		};
		class Particles
		{
			class OnFire
			{
				class SmokeCloud
				{
					overrideParticle="weapon_shot_winded_smoke_small";
				};
				class MuzzleFlash
				{
					overrideParticle="weapon_shot_fnx_02";
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
				maxOverheatingValue=60;
				shotsToStartOverheating=20;
				overheatingDecayInterval=1;
				class SmokingBarrel1
				{
					overrideParticle="smoking_barrel_small";
					onlyWithinOverheatLimits[]={0,0.2};
					positionOffset[]={0.1,0,0};
					onlyWithinRainLimits[]={0,0.5};
				};
				class SmokingBarrelHot1
				{
					positionOffset[]={0.2,0,0};
					overrideParticle="smoking_barrel";
					onlyWithinOverheatLimits[]={0.2,0.60000002};
					onlyWithinRainLimits[]={0,0.5};
				};
				class SmokingBarrelHotSteam
				{
					overrideParticle="smoking_barrel_steam";
					positionOffset[]={0.34999999,0,0};
					onlyWithinOverheatLimits[]={0,1};
					onlyWithinRainLimits[]={0.5,1};
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
					hitpoints=450;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\Mk18\data\mk18.rvmat",
								"KOD_weapons\weapons\Mk18\data\sights.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\Mk18\data\mk18.rvmat",
								"KOD_weapons\weapons\Mk18\data\sights.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\Mk18\data\mk18_damage.rvmat",
								"KOD_weapons\weapons\Mk18\data\sights_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\Mk18\data\mk18_damage.rvmat",
								"KOD_weapons\weapons\Mk18\data\sights_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\Mk18\data\mk18_destruct.rvmat",
								"KOD_weapons\weapons\Mk18\data\sights_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_Mk18: KOD_Mk18_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\Mk18\data\mk18_co.paa",
			"KOD_weapons\weapons\Mk18\data\sights_co.paa"
		};
	};
};
