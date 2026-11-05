class CfgPatches
{
	class KOD_AR501
	{
		units[]={};
		weapons[]=
		{
			"KOD_AR50A1"
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
class Mode_Single;
class Mode_Burst;
class Mode_FullAuto;
class OpticsInfoRifle;
class cfgWeapons
{
	class BoltActionRifle_ExternalMagazine_Base;
	class KOD_AR50A1_Base: BoltActionRifle_ExternalMagazine_Base
	{
		scope=0;
		displayName="ArmaLite AR-50A1";
		descriptionShort="Однозарядная снайперская винтовка калибра 20х102 мм.";
		model="KOD_weapons\weapons\AR501\ar501.p3d";
		attachments[]=
		{
			"weaponWrap",
			"weaponOpticsHunting",
			"weaponOptics",
			"KODSticker1",
			"KODSticker2",
			"KODSticker3"
		};
		itemSize[]={10,3};
		weight=15500;
		repairableWithKits[]={1};
		repairCosts[]={25};
		PPDOFProperties[]={1,0.5,50,160,4,10};
		opticsFlare=0;
		WeaponLength=1.3;
		barrelArmor=3;
		initSpeedMultiplier=1.2;
		chamberSize=1;
		chamberedRound="";
		chamberableFrom[]=
		{
			"KOD_Ammo_20mm"
		};
		magazines[]={};
		DisplayMagazine=0;
		ejectType=0;
		recoilModifier[]={3,1.5,1.2};
		swayModifier[]={2.5,2.5,1};
		simpleHiddenSelections[]=
		{
			"hide_barrel"
		};
		hiddenSelections[]=
		{
			"color1",
			"color2",
			"color3"
		};
		modes[]=
		{
			"Single"
		};
		class Single: Mode_Single
		{
			soundSetShot[]=
			{
				"AR50A1_Shot_SoundSet",
				"CZ550_Tail_SoundSet",
				"CZ550_InteriorTail_SoundSet"
			};
			soundSetShotExt[]=
			{
				
				{
					"CZ550_silencerHomeMade_SoundSet",
					"CZ550_silencerHomeMadeTail_SoundSet",
					"CZ550_silencerInteriorHomeMadeTail_SoundSet"
				}
			};
			reloadTime=1;
			dispersion=9.9999997e-05;
			magazineSlot="magazine";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=300;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\AR501\data\ar501.rvmat",
								"KOD_weapons\weapons\AR501\data\ar501_muzzle.rvmat",
								"KOD_weapons\weapons\AR501\data\ar501_bipod.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\AR501\data\ar501.rvmat",
								"KOD_weapons\weapons\AR501\data\ar501_muzzle.rvmat",
								"KOD_weapons\weapons\AR501\data\ar501_bipod.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\AR501\data\ar501_damage.rvmat",
								"KOD_weapons\weapons\AR501\data\ar501_muzzle_damage.rvmat",
								"KOD_weapons\weapons\AR501\data\ar501_bipod_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\AR501\data\ar501_damage.rvmat",
								"KOD_weapons\weapons\AR501\data\ar501_muzzle_damage.rvmat",
								"KOD_weapons\weapons\AR501\data\ar501_bipod_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\AR501\data\ar501_destruct.rvmat",
								"KOD_weapons\weapons\AR501\data\ar501_muzzle_destruct.rvmat",
								"KOD_weapons\weapons\AR501\data\ar501_bipod_destruct.rvmat"
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
					overrideParticle="weapon_shot_cz527_01";
					ignoreIfSuppressed=1;
					illuminateWorld=1;
					positionOffset[]={-0.050000001,0,0};
				};
				class GasPistonBurstR
				{
					overrideParticle="weapon_shot_chamber_smoke";
					overridePoint="GasPiston";
					overrideDirectionVector[]={0,0,0};
				};
				class GasPistonBurstL
				{
					overrideParticle="weapon_shot_chamber_smoke";
					overridePoint="GasPiston";
					overrideDirectionVector[]={180,0,0};
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
				shotsToStartOverheating=1;
				maxOverheatingValue=4;
				overheatingDecayInterval=3;
				class BarrelSmoke
				{
					overrideParticle="smoking_barrel_small";
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
			strength=150;
			type="shot";
		};
		class OpticsInfo: OpticsInfoRifle
		{
			memoryPointCamera="eye";
			modelOptics="-";
			distanceZoomMin=300;
			distanceZoomMax=300;
			discreteDistance[]={300};
			discreteDistanceInitIndex=0;
		};
	};
	class KOD_AR50A1: KOD_AR50A1_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\AR501\data\ar501_co.paa",
			"KOD_weapons\weapons\AR501\data\ar501_muzzle_co.paa",
			"KOD_weapons\weapons\AR501\data\ar501_bipod_co.paa"
		};
	};
};
class CfgSoundShaders
{
	class base_closeShot_SoundShader;
	class base_midShot_SoundShader;
	class base_distShot_SoundShader;
	class AR50A1_closeShot_SoundShader: base_closeShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\AR501\sound\ar50_close",
				1
			}
		};
		volume=1;
	};
	class AR50A1_midShot_SoundShader: base_midShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\AR501\sound\ar50_mid",
				1
			}
		};
		volume=0.80000001;
	};
	class AR50A1_distShot_SoundShader: base_distShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\AR501\sound\ar50_far",
				1
			}
		};
		volume=0.60000002;
	};
};
class CfgSoundSets
{
	class Rifle_Shot_Base_SoundSet;
	class AR50A1_Shot_SoundSet: Rifle_Shot_Base_SoundSet
	{
		soundShaders[]=
		{
			"AR50A1_closeShot_SoundShader",
			"AR50A1_midShot_SoundShader",
			"AR50A1_distShot_SoundShader"
		};
	};
};
