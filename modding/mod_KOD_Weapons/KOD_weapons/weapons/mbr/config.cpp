class CfgPatches
{
	class KOD_Weapons_MBR
	{
		units[]={};
		weapons[]=
		{
			"KOD_MBR_Black",
			"KOD_MBR_Tan",
			"KOD_MBR_UCPCamo",
			"KOD_MBR_WoodlandCamo",
			"KOD_MBR_DesertCamo"
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
	class KOD_MBR_Base: Rifle_Base
	{
		scope=0;
		displayName="MBR";
		descriptionShort="Прототип модульной винтовки типа буллпап. Питание осуществляется из отъемных магазинов STANAG под патроны 5.56x45.";
		weight=3400;
		model="KOD_weapons\weapons\MBR\mbr.p3d";
		absorbency=0;
		repairableWithKits[]={1};
		repairCosts[]={30,25};
		PPDOFProperties[]={1,0.60000002,50,100,4,10};
		WeaponLength=1;
		initSpeedMultiplier=1;
		barrelArmor=5.5;
		attachments[]=
		{
			"weaponWrap",
			"weaponOptics",
			"weaponMuzzleM4",
			"KODSticker1",
			"KODSticker2",
			"KODSticker3"
		};
		itemSize[]={7,3};
		chamberSize=1;
		chamberedRound="";
		chamberableFrom[]=
		{
			"Ammo_556x45",
			"Ammo_556x45Tracer"
		};
		magazines[]=
		{
			"Mag_STANAG_30Rnd",
			"Mag_STANAG_60Rnd",
			"Mag_STANAGCoupled_30Rnd",
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
		recoilModifier[]={0.60000002,0.60000002,0.60000002};
		swayModifier[]={1.4,1.4,0.89999998};
		simpleHiddenSelections[]=
		{
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
				"AUG_Shot_1st_SoundSet",
				"AUG_Shot_1st_iterior_SoundSet",
				"AUG_Tail_SoundSet",
				"AUG_InteriorTail_SoundSet",
				"AUG_Slapback_SoundSet",
				"AUG_Tail_2D_SoundSet"
			};
			soundSetShotExt[]=
			{
				
				{
					"AUG_1st_silencer_SoundSet",
					"AUG_silencerTail_SoundSet",
					"AUG_silencerInteriorTail_SoundSet"
				},
				
				{
					"AUG_1st_silencerHomeMade_SoundSet",
					"AUG_silencerHomeMadeTail_SoundSet",
					"AUG_silencerInteriorHomeMadeTail_SoundSet"
				}
			};
			reloadTime=0.1;
			dispersion=0.0011;
			magazineSlot="magazine";
		};
		class FullAuto: Mode_FullAuto
		{
			soundSetShot1st[]=
			{
				"AUG_Shot_1st_SoundSet",
				"AUG_Shot_1st_iterior_SoundSet"
			};
			soundSetShot[]=
			{
				"AUG_Shot_SoundSet",
				"AUG_Shot_iterior_SoundSet",
				"AUG_Tail_SoundSet",
				"AUG_InteriorTail_SoundSet",
				"AUG_Slapback_SoundSet",
				"AUG_Tail_2D_SoundSet"
			};
			soundSetShotExt1st[]=
			{
				
				{
					"AUG_1st_silencer_SoundSet"
				},
				
				{
					"AUG_1st_silencerHomeMade_SoundSet"
				}
			};
			soundSetShotExt[]=
			{
				
				{
					"AUG_silencer_SoundSet",
					"AUG_silencerTail_SoundSet",
					"AUG_silencerInteriorTail_SoundSet"
				},
				
				{
					"AUG_silencerHomeMade_SoundSet",
					"AUG_silencerHomeMadeTail_SoundSet",
					"AUG_silencerInteriorHomeMadeTail_SoundSet"
				}
			};
			reloadTime=0.055;
			dispersion=0.0011;
			magazineSlot="magazine";
		};
		class OpticsInfo: OpticsInfoRifle
		{
			memoryPointCamera="eye";
			discreteDistance[]={100,200,300,400,500,600,700,800};
			discreteDistanceInitIndex=0;
			modelOptics="-";
			distanceZoomMin=100;
			distanceZoomMax=800;
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
					overrideParticle="weapon_shot_ump45_01";
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
				shotsToStartOverheating=20;
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
								"KOD_weapons\weapons\MBR\data\mbr.rvmat",
								"KOD_weapons\weapons\MBR\data\mbr_accs.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\MBR\data\mbr.rvmat",
								"KOD_weapons\weapons\MBR\data\mbr_accs.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\MBR\data\mbr_damage.rvmat",
								"KOD_weapons\weapons\MBR\data\mbr_accs_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\MBR\data\mbr_damage.rvmat",
								"KOD_weapons\weapons\MBR\data\mbr_accs_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\MBR\data\mbr_destruct.rvmat",
								"KOD_weapons\weapons\MBR\data\mbr_accs_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_MBR_Black: KOD_MBR_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\MBR\data\mbr_black_co.paa"
		};
	};
	class KOD_MBR_Tan: KOD_MBR_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\MBR\data\mbr_tan_co.paa"
		};
	};
	class KOD_MBR_UCPCamo: KOD_MBR_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\MBR\data\mbr_ucp_co.paa"
		};
	};
	class KOD_MBR_WoodlandCamo: KOD_MBR_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\MBR\data\mbr_woodland_co.paa"
		};
	};
	class KOD_MBR_DesertCamo: KOD_MBR_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\MBR\data\mbr_desert_co.paa"
		};
	};
};
