class CfgPatches
{
	class KOD_SVU
	{
		units[]={};
		weapons[]=
		{
			"KOD_SVU"
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
	class KOD_SVU_Base: Rifle_Base
	{
		scope=0;
		displayName="SVU";
		descriptionShort="Semi-automatic bullpup marksmen rifle based on SVD, compatible with AK scopes. Fed from detachable Magazines, chambered for 7.62x54mm";
		model="KOD_weapons\weapons\SVU\SVU.p3d";
		attachments[]=
		{
			"weaponWrap",
			"weaponOpticsAK",
			"suppressorImpro",
			"KODSticker1",
			"KODSticker2",
			"KODSticker3"
		};
		itemSize[]={8,3};
		weight=5000;
		absorbency=0;
		repairableWithKits[]={1};
		repairCosts[]={30,25};
		PPDOFProperties[]={1,0.5,20,170,4,10};
		WeaponLength=0.98986;
		chamberSize=1;
		chamberedRound="";
		chamberableFrom[]=
		{
			"Ammo_762x54",
			"Ammo_762x54Tracer"
		};
		magazines[]=
		{
			"Mag_SVD_10Rnd"
		};
		ejectType=1;
		barrelArmor=8.25;
		initSpeedMultiplier=0.89999998;
		recoilModifier[]={0.88,0.88,0.88};
		swayModifier[]={1.5,1.5,1};
		hiddenSelections[]=
		{
			"color"
		};
		simpleHiddenSelections[]=
		{
			"hide_barrel",
			"folding_raised",
			"folding_lowered"
		};
		modes[]=
		{
			"SemiAuto"
		};
		class SemiAuto: Mode_SemiAuto
		{
			soundSetShot[]=
			{
				"SVU_Shot_SoundSet",
				"SVD_Tail_SoundSet",
				"SVD_InteriorTail_SoundSet"
			};
			soundSetShotExt[]=
			{
				
				{
					"SVD_silencerHomeMade_SoundSet",
					"SVD_silencerHomeMadeTail_SoundSet",
					"SVD_silencerInteriorHomeMadeTail_SoundSet"
				}
			};
			reloadTime=0.125;
			dispersion=0.00075000001;
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
				class MuzzleFlash
				{
					overrideParticle="weapon_shot_akm_01";
					ignoreIfSuppressed=1;
					illuminateWorld=1;
				};
				class SmokeCloud1
				{
					overrideParticle="weapon_shot_winded_smoke";
				};
				class GasPistonSmokeRaiseR
				{
					overrideParticle="weapon_shot_chamber_smoke_raise";
					overridePoint="GasPiston";
					overrideDirectionVector[]={0,0,0};
				};
				class GasPistonSmokeRaiseL
				{
					overrideParticle="weapon_shot_chamber_smoke_raise";
					overridePoint="GasPiston";
					overrideDirectionVector[]={180,0,0};
				};
			};
			class OnOverheating
			{
				shotsToStartOverheating=5;
				maxOverheatingValue=30;
				overheatingDecayInterval=1;
				class SmokingBarrel1
				{
					overridePoint="GasPiston";
					positionOffset[]={0.050000001,-0.02,0};
					overrideParticle="smoking_barrel_small";
					onlyWithinOverheatLimits[]={0,0.2};
					onlyWithinRainLimits[]={0,0.2};
				};
				class SmokingBarrelHot1
				{
					overridePoint="GasPiston";
					positionOffset[]={0.12,-0.02,0};
					overrideParticle="smoking_barrel";
					onlyWithinOverheatLimits[]={0.2,0.60000002};
					onlyWithinRainLimits[]={0,0.2};
				};
				class SmokingBarrelHot3
				{
					overridePoint="GasPiston";
					positionOffset[]={0.20999999,-0.02,0};
					overrideParticle="smoking_barrel_heavy";
					onlyWithinOverheatLimits[]={0.60000002,1};
					onlyWithinRainLimits[]={0,0.2};
				};
				class Steam
				{
					overrideParticle="smoking_barrel_steam_small";
					positionOffset[]={0.40000001,0.02,0};
					onlyWithinOverheatLimits[]={0,0.5};
					onlyWithinRainLimits[]={0.2,1};
				};
				class Steam2
				{
					overrideParticle="smoking_barrel_steam";
					positionOffset[]={0.44999999,0.02,0};
					onlyWithinOverheatLimits[]={0.5,1};
					onlyWithinRainLimits[]={0.2,1};
				};
				class OpenChamberSmoke
				{
					onlyIfBoltIsOpen=1;
					overrideParticle="smoking_barrel_small";
					overridePoint="Nabojnicestart";
					onlyWithinOverheatLimits[]={0,1};
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
					hitpoints=220;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\SVU\data\svu.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\SVU\data\svu.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\SVU\data\svu_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\SVU\data\svu_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\SVU\data\svu_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_SVU: KOD_SVU_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\SVU\data\svu_co.paa"
		};
	};
};
class CfgSoundShaders
{
	class base_closeShot_SoundShader;
	class base_midShot_SoundShader;
	class base_distShot_SoundShader;
	class SVU_closeShot_SoundShaderBase: base_closeShot_SoundShader
	{
		range=2500;
	};
	class SVU_midShot_SoundShaderBase: base_midShot_SoundShader
	{
		volume=1;
		range=2500;
	};
	class SVU_distShot_SoundShaderBase: base_distShot_SoundShader
	{
		range=2500;
	};
	class SVU_closeShot_SoundShader: SVU_closeShot_SoundShaderBase
	{
		samples[]=
		{
			
			{
				"DZ\sounds\weapons\firearms\SVD\SVD_close_shot_01",
				1
			},
			
			{
				"DZ\sounds\weapons\firearms\SVD\SVD_close_shot_02",
				1
			},
			
			{
				"DZ\sounds\weapons\firearms\SVD\SVD_close_shot_03",
				1
			}
		};
		volume=0.4943282;
	};
	class SVU_midShot_SoundShader: SVU_midShot_SoundShaderBase
	{
		samples[]=
		{
			
			{
				"DZ\sounds\weapons\firearms\SVD\SVD_mid_shot_01",
				1
			},
			
			{
				"DZ\sounds\weapons\firearms\SVD\SVD_mid_shot_02",
				1
			},
			
			{
				"DZ\sounds\weapons\firearms\SVD\SVD_mid_shot_03",
				1
			}
		};
		volume=0.4943282;
	};
	class SVU_distShot_SoundShader: SVU_closeShot_SoundShader
	{
		samples[]=
		{
			
			{
				"DZ\sounds\weapons\firearms\SVD\SVD_dist_shot_01",
				1
			},
			
			{
				"DZ\sounds\weapons\firearms\SVD\SVD_dist_shot_02",
				1
			},
			
			{
				"DZ\sounds\weapons\firearms\SVD\SVD_dist_shot_03",
				1
			}
		};
		volume=0.4943282;
	};
};
class CfgSoundSets
{
	class Rifle_Shot_Base_SoundSet;
	class SVU_Shot_SoundSet: Rifle_Shot_Base_SoundSet
	{
		soundShaders[]=
		{
			"SVU_closeShot_SoundShader",
			"SVU_midShot_SoundShader",
			"SVU_distShot_SoundShader",
			"SVD_Closure_SoundShader"
		};
	};
};
