class CfgPatches
{
	class KOD_Weapons_AR15Skeleton
	{
		units[]=
		{
			"KOD_AR15Skeleton_Buttstock"
		};
		weapons[]=
		{
			"KOD_AR15Skeleton"
		};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Firearms",
			"DZ_Sounds_Effects",
			"DZ_Data"
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
	class KOD_AR15Skeleton_Base: Rifle_Base
	{
		scope=0;
		displayName="AR-15";
		descriptionShort="$STR_KOD_AR15Skeleton";
		weight=2900;
		model="KOD_weapons\weapons\SkeletonizedAR15\ar15.p3d";
		absorbency=0;
		repairableWithKits[]={1};
		repairCosts[]={25};
		PPDOFProperties[]={1,0.60000002,50,200,4,10};
		WeaponLength=1.1;
		initSpeedMultiplier=1;
		attachments[]=
		{
			"weaponWrap",
			"weaponOptics",
			"weaponFlashlight",
			"weaponMuzzleM4",
			"weaponButtstockM4",
			"KODRISBipod",
			"KODSticker1"
		};
		itemSize[]={9,3};
		chamberSize=1;
		barrelArmor=6.75;
		chamberedRound="";
		chamberableFrom[]=
		{
			"Ammo_556x45",
			"Ammo_556x45Tracer"
		};
		magazines[]=
		{
			"Mag_STANAG_60Rnd",
			"Mag_STANAG_30Rnd",
			"Mag_STANAGCoupled_30Rnd",
			"Mag_CMAG_10Rnd",
			"Mag_CMAG_20Rnd",
			"Mag_CMAG_30Rnd",
			"Mag_CMAG_40Rnd",
			"Mag_CMAG_100Rnd_Black",
			"Mag_CMAG_10Rnd_Green",
			"Mag_CMAG_20Rnd_Green",
			"Mag_CMAG_30Rnd_Green",
			"Mag_CMAG_40Rnd_Green",
			"Mag_CMAG_10Rnd_Black",
			"Mag_CMAG_20Rnd_Black",
			"Mag_CMAG_30Rnd_Black",
			"Mag_CMAG_40Rnd_Black"
		};
		ejectType=1;
		recoilModifier[]={0.72000003,0.55000001,0.62};
		swayModifier[]={1.9,1.9,1.05};
		simpleHiddenSelections[]=
		{
			"hide_barrel"
		};
		hiddenSelections[]=
		{
			"color",
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
			reloadTime=0.115;
			dispersion=0.00125;
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
			reloadTime=0.085000001;
			dispersion=0.00125;
			magazineSlot="magazine";
		};
		class OpticsInfo: OpticsInfoRifle
		{
			memoryPointCamera="eye";
			discreteDistance[]={200};
			discreteDistanceInitIndex=0;
			modelOptics="-";
			distanceZoomMin=200;
			distanceZoomMax=200;
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
				shotsToStartOverheating=7;
				overheatingDecayInterval=1;
				class SmokingBarrel1
				{
					positionOffset[]={0.2,0,0};
					overrideParticle="smoking_barrel_small";
					onlyWithinOverheatLimits[]={0,0.2};
					onlyWithinRainLimits[]={0,0.2};
				};
				class SmokingBarrelHot1
				{
					positionOffset[]={0.2,0,0};
					overrideParticle="smoking_barrel";
					onlyWithinOverheatLimits[]={0.2,0.60000002};
					onlyWithinRainLimits[]={0,0.2};
				};
				class SmokingBarrelHot3
				{
					positionOffset[]={0.2,0,0};
					overrideParticle="smoking_barrel_heavy";
					onlyWithinOverheatLimits[]={0.60000002,1};
					onlyWithinRainLimits[]={0,0.2};
				};
				class SmokingBarrelHotSteam
				{
					overrideParticle="smoking_barrel_steam";
					positionOffset[]={0.34999999,0,0};
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
					hitpoints=450;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\SkeletonizedAR15\data\blue.rvmat",
								"KOD_weapons\weapons\SkeletonizedAR15\data\yellow.rvmat",
								"KOD_weapons\weapons\SkeletonizedAR15\data\green.rvmat",
								"KOD_weapons\weapons\SkeletonizedAR15\data\red.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\SkeletonizedAR15\data\blue.rvmat",
								"KOD_weapons\weapons\SkeletonizedAR15\data\yellow.rvmat",
								"KOD_weapons\weapons\SkeletonizedAR15\data\green.rvmat",
								"KOD_weapons\weapons\SkeletonizedAR15\data\red.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\SkeletonizedAR15\data\blue_damage.rvmat",
								"KOD_weapons\weapons\SkeletonizedAR15\data\yellow_damage.rvmat",
								"KOD_weapons\weapons\SkeletonizedAR15\data\green_damage.rvmat",
								"KOD_weapons\weapons\SkeletonizedAR15\data\red_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\SkeletonizedAR15\data\blue_damage.rvmat",
								"KOD_weapons\weapons\SkeletonizedAR15\data\yellow_damage.rvmat",
								"KOD_weapons\weapons\SkeletonizedAR15\data\green_damage.rvmat",
								"KOD_weapons\weapons\SkeletonizedAR15\data\red_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\SkeletonizedAR15\data\blue_destruct.rvmat",
								"KOD_weapons\weapons\SkeletonizedAR15\data\yellow_destruct.rvmat",
								"KOD_weapons\weapons\SkeletonizedAR15\data\green_destruct.rvmat",
								"KOD_weapons\weapons\SkeletonizedAR15\data\red_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_AR15Skeleton: KOD_AR15Skeleton_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\SkeletonizedAR15\data\blue_co.paa",
			"KOD_weapons\weapons\SkeletonizedAR15\data\yellow_co.paa",
			"KOD_weapons\weapons\SkeletonizedAR15\data\green_co.paa",
			"KOD_weapons\weapons\SkeletonizedAR15\data\red_co.paa"
		};
	};
};
class cfgVehicles
{
	class Inventory_Base;
	class KOD_AR15Skeleton_Buttstock: Inventory_Base
	{
		scope=2;
		displayName="Облегчённый приклад AR-15";
		descriptionShort="$STR_cfgVehicles_M4_MPBttstck1";
		model="KOD_weapons\weapons\SkeletonizedAR15\buttstock.p3d";
		rotationFlags=17;
		reversed=0;
		inventorySlot[]=
		{
			"weaponButtstockM4"
		};
		weight=300;
		itemSize[]={2,1};
		dispersionModifier=-0.00034999999;
		dispersionCondition="true";
		recoilModifier[]={0.75,0.75,0.75};
		swayModifier[]={0.94999999,0.94999999,0.94999999};
		hiddenSelections[]=
		{
			"color"
		};
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\SkeletonizedAR15\data\blue_co.paa"
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
								"KOD_weapons\weapons\SkeletonizedAR15\data\blue.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\SkeletonizedAR15\data\blue.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\SkeletonizedAR15\data\blue_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\SkeletonizedAR15\data\blue_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\SkeletonizedAR15\data\blue_destruct.rvmat"
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
