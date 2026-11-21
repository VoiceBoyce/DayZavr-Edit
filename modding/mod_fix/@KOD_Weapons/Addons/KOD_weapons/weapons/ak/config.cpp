class CfgPatches
{
	class KOD_Weapons_AK
	{
		units[]=
		{
			"KOD_AK_Bttstck"
		};
		weapons[]=
		{
			"KOD_AK15",
			"KOD_AK12"
		};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Firearms",
			"DZ_Sounds_Effects"
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
	class KOD_AK_Base: Rifle_Base
	{
		scope=0;
		attachments[]=
		{
			"weaponWrap",
			"weaponMuzzleAK",
			"weaponOptics",
			"weaponFlashlight",
			"weaponButtstockAK",
			"KODRISBipod",
			"KODSticker1",
			"KODSticker2",
			"KODSticker3"
		};
		itemSize[]={8,3};
		weight=3500;
		absorbency=0;
		repairableWithKits[]={1};
		repairCosts[]={30,25};
		PPDOFProperties[]={};
		barrelArmor=6.4439998;
		initSpeedMultiplier=1;
		WeaponLength=0.92906702;
		chamberSize=1;
		chamberedRound="";
		ejectType=1;
		hiddenSelections[]=
		{
			"color"
		};
		simpleHiddenSelections[]=
		{
			"hide_barrel",
			"hide_ironsights"
		};
		modes[]=
		{
			"FullAuto",
			"SemiAuto"
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
								"KOD_weapons\weapons\AK\data\ak15.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\AK\data\ak15.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\AK\data\ak15_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\AK\data\ak15_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\AK\data\ak15_destruct.rvmat"
							}
						}
					};
				};
			};
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
				maxOverheatingValue=100;
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
	};
	class KOD_AK15_Base: KOD_AK_Base
	{
		scope=0;
		displayName="AK-15";
		descriptionShort="$STR_KOD_AK15";
		model="KOD_weapons\weapons\AK\ak762_long.p3d";
		chamberableFrom[]=
		{
			"Ammo_762x39",
			"Ammo_762x39Tracer"
		};
		magazines[]=
		{
			"Mag_KOD_AKALPHA_30rnd",
			"Mag_AKM_30Rnd",
			"Mag_AKM_Palm30Rnd",
			"Mag_AKM_Palm30Rnd_Black",
			"Mag_AKM_Palm30Rnd_Green",
			"Mag_AKM_Drum75Rnd",
			"Mag_AKM_Drum75Rnd_Green",
			"Mag_AKM_Drum75Rnd_Black",
			"Mag_AKM_Drum_TEST"
		};
		recoilModifier[]={0.85000002,0.85000002,0.85000002};
		swayModifier[]={1.9,1.9,1};
		class SemiAuto: Mode_SemiAuto
		{
			soundSetShot[]=
			{
				"KODAK15_Shot_SoundSet",
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
			reloadTime=0.1;
			dispersion=0.0013;
			magazineSlot="magazine";
		};
		class FullAuto: Mode_FullAuto
		{
			soundSetShot1st[]=
			{
				"KODAK15_Shot_SoundSet",
				"AK74_Shot_1st_iterior_SoundSet"
			};
			soundSetShot[]=
			{
				"KODAK15_Shot_SoundSet",
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
			reloadTime=0.090000004;
			dispersion=0.0013;
			magazineSlot="magazine";
		};
	};
	class KOD_AK15: KOD_AK15_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\AK\data\ak15_black_co.paa"
		};
	};
	class KOD_AK12_Base: KOD_AK_Base
	{
		scope=0;
		displayName="AK-12";
		descriptionShort="$STR_KOD_AK12";
		model="KOD_weapons\weapons\AK\ak545_long.p3d";
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
			"Mag_KOD_RPK16_96rnd",
			"Mag_AK74_45Rnd"
		};
		recoilModifier[]={0.80000001,0.80000001,0.80000001};
		swayModifier[]={1.9,1.9,1};
		class SemiAuto: Mode_SemiAuto
		{
			soundSetShot[]=
			{
				"DZVR_KOD_AK12_Shot_SoundSet",
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
			reloadTime=0.1;
			dispersion=0.0013;
			magazineSlot="magazine";
		};
		class FullAuto: Mode_FullAuto
		{
			soundSetShot1st[]=
			{
				"DZVR_KOD_AK12_Shot_SoundSet",
				"AK74_Shot_1st_iterior_SoundSet"
			};
			soundSetShot[]=
			{
				"DZVR_KOD_AK12_Shot_SoundSet",
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
			reloadTime=0.090000004;
			dispersion=0.0013;
			magazineSlot="magazine";
		};
	};
	class KOD_AK12: KOD_AK12_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\AK\data\ak15_black_co.paa"
		};
	};
};
class cfgVehicles
{
	class Inventory_Base;
	class KOD_AK_Bttstck: Inventory_Base
	{
		scope=2;
		displayName="Телескопический Приклад АК";
		descriptionShort="Телескопический приклад для AK. Лёгкий и удобный.";
		model="KOD_weapons\weapons\AK\buttstock.p3d";
		rotationFlags=17;
		reversed=0;
		inventorySlot[]=
		{
			"weaponButtstockAK"
		};
		weight=200;
		itemSize[]={1,3};
		dispersionModifier=-0.00019999999;
		dispersionCondition="true";
		recoilModifier[]={0.69999999,0.69999999,0.69999999};
		swayModifier[]={1,1,1};
		hiddenSelections[]=
		{
			"color"
		};
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\AK\data\ak15_black_co.paa"
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
								"KOD_weapons\weapons\AK\data\ak15.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\AK\data\ak15.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\AK\data\ak15_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\AK\data\ak15_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\AK\data\ak15_destruct.rvmat"
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
class CfgSoundShaders
{
	class base_closeShot_SoundShader;
	class base_midShot_SoundShader;
	class base_distShot_SoundShader;
	class KODAK15_closeShot_SoundShader: base_closeShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\AK\sound\ak15_close",
				1
			}
		};
		volume=0.69999999;
	};
	class KODAK15_midShot_SoundShader: base_midShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\AK\sound\ak15_mid",
				1
			}
		};
		volume=0.5;
	};
	class KODAK15_distShot_SoundShader: base_distShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\AK\sound\ak15_far",
				1
			}
		};
		volume=0.30000001;
	};
};
class CfgSoundSets
{
	class Rifle_Shot_Base_SoundSet;
	class KODAK15_Shot_SoundSet: Rifle_Shot_Base_SoundSet
	{
		soundShaders[]=
		{
			"KODAK15_closeShot_SoundShader",
			"KODAK15_midShot_SoundShader",
			"KODAK15_distShot_SoundShader"
		};
	};
};
