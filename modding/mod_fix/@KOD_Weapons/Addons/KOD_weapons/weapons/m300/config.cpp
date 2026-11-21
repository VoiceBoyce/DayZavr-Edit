class CfgPatches
{
	class KOD_M300
	{
		units[]={};
		weapons[]=
		{
			"KOD_M300"
		};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Firearms"
		};
		magazines[]={};
		ammo[]={};
	};
};
class Mode_Safe;
class Mode_SemiAuto;
class Mode_Burst;
class Mode_FullAuto;
class OpticsInfoRifle;
class cfgWeapons
{
	class BoltActionRifle_Base;
	class KOD_M300_Base: BoltActionRifle_Base
	{
		scope=0;
		displayName="CheyTac M300 Praetorian Composite";
		descriptionShort="The M300 Praetorian is the sportsman’s choice for a lightweight rifle to carry over challenging terrains. Fed from a detachable M200/M300 magazine, chambered for .408 CheyTac";
		weight=9500;
		model="KOD_weapons\weapons\M300\M300.p3d";
		attachments[]=
		{
			"weaponWrap",
			"weaponOptics",
			"weaponOpticsHunting",
			"KODSticker1",
			"KODSticker2",
			"KODSticker3"
		};
		absorbency=0;
		repairableWithKits[]={1};
		repairCosts[]={30,25};
		PPDOFProperties[]={1,0.5,50,180,4,10};
		initSpeedMultiplier=1.2;
		WeaponLength=1.38986;
		chamberedRound="";
		chamberSize=1;
		chamberableFrom[]=
		{
			"KOD_Ammo_408"
		};
		magazines[]=
		{
			"Mag_KOD_M200_7rnd"
		};
		ejectType=1;
		itemSize[]={10,3};
		barrelArmor=6;
		recoilModifier[]={3.5,2.5,2.5};
		swayModifier[]={2.3,2.3,1.3};
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
			"Single"
		};
		class Single: Mode_SemiAuto
		{
			soundSetShot[]=
			{
				"M300_Shot_SoundSet",
				"Mosin_Tail_SoundSet",
				"Mosin_InteriorTail_SoundSet"
			};
			soundSetShotExt[]=
			{
				
				{
					"Mosin_silencerHomeMade_SoundSet",
					"Mosin_silencerHomeMadeTail_SoundSet",
					"Mosin_silencerInteriorHomeMadeTail_SoundSet"
				}
			};
			reloadTime=1;
			dispersion=0.00019999999;
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
					overrideParticle="weapon_shot_cz527_01";
					ignoreIfSuppressed=1;
					illuminateWorld=1;
					positionOffset[]={-0.050000001,0,0};
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
		class OpticsInfo: OpticsInfoRifle
		{
			memoryPointCamera="eye";
			discreteDistance[]={100};
			discreteDistanceInitIndex=0;
			modelOptics="-";
			distanceZoomMin=100;
			distanceZoomMax=100;
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
								"KOD_weapons\weapons\M300\data\m300_camo.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\M300\data\m300_camo.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\M300\data\m300_camo_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\M300\data\m300_camo_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\M300\data\m300_camo_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_M300: KOD_M300_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\M300\data\m300_camo_co.paa"
		};
	};
};
class CfgSoundShaders
{
	class base_closeShot_SoundShader;
	class base_midShot_SoundShader;
	class base_distShot_SoundShader;
	class M300_closeShot_SoundShader: base_closeShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\M300\sound\m300_close",
				1
			}
		};
		volume=0.80000001;
	};
	class M300_midShot_SoundShader: base_midShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\M300\sound\m300_mid",
				1
			}
		};
		volume=0.60000002;
	};
	class M300_distShot_SoundShader: base_distShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\M300\sound\m300_far",
				1
			}
		};
		volume=0.5;
	};
};
class CfgSoundSets
{
	class Rifle_Shot_Base_SoundSet;
	class M300_Shot_SoundSet: Rifle_Shot_Base_SoundSet
	{
		soundShaders[]=
		{
			"M300_closeShot_SoundShader",
			"M300_midShot_SoundShader",
			"M300_distShot_SoundShader"
		};
	};
};
