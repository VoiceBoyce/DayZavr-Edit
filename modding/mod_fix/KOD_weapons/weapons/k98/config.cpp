class CfgPatches
{
	class KOD_Weapons_K98
	{
		units[]={};
		weapons[]=
		{
			"KOD_K98"
		};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Firearms",
			"DZ_Weapons_Ammunition",
			"DZ_Weapons_Projectiles"
		};
		magazines[]=
		{
			"KOD_Ammo_792x57"
		};
		ammo[]=
		{
			"KOD_Bullet_792x57"
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
	class BoltActionRifle_InnerMagazine_Base;
	class KOD_K98_Base: BoltActionRifle_InnerMagazine_Base
	{
		scope=0;
		displayName="Karabiner 98k";
		descriptionShort="$STR_KOD_K98";
		model="KOD_weapons\weapons\K98\k98.p3d";
		itemSize[]={10,3};
		attachments[]=
		{
			"weaponWrap",
			"suppressorImpro",
			"weaponOpticsMosin",
			"KODSticker1",
			"KODSticker2",
			"KODSticker3"
		};
		weight=3700;
		absorbency=0;
		repairableWithKits[]={1};
		repairCosts[]={25};
		PPDOFProperties[]={1,0.5,20,135,4,10};
		DisplayMagazine=0;
		WeaponLength=1.22271;
		barrelArmor=3.5;
		initSpeedMultiplier=1.15;
		chamberSize=5;
		chamberedRound="";
		chamberableFrom[]=
		{
			"KOD_Ammo_792x57"
		};
		magazines[]={};
		ejectType=0;
		recoilModifier[]={1,1,1};
		swayModifier[]={2.5,2.5,1.3};
		reloadSkips[]={0.31999999,0.41,0.5,0.58999997,0.69,0.76999998};
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
				"Mosin9130_Shot_SoundSet",
				"Mosin9130_Shot_iterior_SoundSet",
				"Mosin9130_Tail_SoundSet",
				"Mosin9130_InteriorTail_SoundSet",
				"Mosin9130_Tail_2D_SoundSet"
			};
			soundSetShotExt[]=
			{
				
				{
					"Mosin9130_silencerHomeMade_SoundSet",
					"Mosin9130_silencerHomeMadeTail_SoundSet",
					"Mosin9130_silencerInteriorHomeMadeTail_SoundSet"
				}
			};
			reloadTime=1;
			dispersion=0.00089999998;
			magazineSlot="magazine";
		};
		class Particles
		{
			class OnFire
			{
				class MuzzleFlash
				{
					overrideParticle="weapon_shot_mosin9130_01";
					ignoreIfSuppressed=1;
					illuminateWorld=1;
					positionOffset[]={-0.029999999,0,0};
				};
			};
			class OnOverheating
			{
				shotsToStartOverheating=1;
				maxOverheatingValue=3;
				overheatingDecayInterval=4;
				class BarrelSmoke
				{
					overrideParticle="smoking_barrel_small";
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
		class OpticsInfo: OpticsInfoRifle
		{
			memoryPointCamera="eye";
			modelOptics="-";
			distanceZoomMin=100;
			distanceZoomMax=100;
			discreteDistance[]={100,200,300,400,500,600,700,800,900,1000};
			discreteDistanceInitIndex=0;
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=250;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\K98\data\k98.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\K98\data\k98.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\K98\data\k98_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\K98\data\k98_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\K98\data\k98_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_K98: KOD_K98_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\K98\data\k98_co.paa"
		};
	};
};
class cfgMagazines
{
	class Ammunition_Base;
	class KOD_Ammo_792x57: Ammunition_Base
	{
		scope=2;
		displayName="Патроны 7.92x57 мм";
		descriptionShort="$STR_KOD_Ammo_792x57";
		model="KOD_weapons\weapons\K98\ammo.p3d";
		weight=40;
		count=20;
		ammo="KOD_Bullet_792x57";
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=100;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\K98\data\k98.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\K98\data\k98.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\K98\data\k98_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\K98\data\k98_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\K98\data\k98_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
};
class CfgAmmo
{
	class Bullet_Base;
	class KOD_Bullet_792x57: Bullet_Base
	{
		scope=2;
		casing="FxCartridge_762";
		round="FxRound_762";
		spawnPileType="KOD_Ammo_792x57";
		hit=12;
		indirectHit=0;
		indirectHitRange=0;
		airLock=1;
		initSpeed=755;
		typicalSpeed=755;
		airFriction=-0.00079999998;
		caliber=1;
		deflecting=10;
		damageBarrel=500;
		damageBarrelDestroyed=500;
		weight=0.0099999998;
		impactBehaviour=1;
		class DamageApplied
		{
			type="Projectile";
			dispersion=0;
			bleedThreshold=1;
			defaultDamageOverride[]=
			{
				{0.5,1}
			};
			class Health
			{
				damage=115;
			};
			class Blood
			{
				damage=100;
			};
			class Shock
			{
				damage=105;
			};
		};
		class NoiseHit
		{
			strength=11;
			type="shot";
		};
	};
};
class cfgAmmoTypes
{
	class AType_KOD_Bullet_792x57
	{
		name="KOD_Bullet_792x57";
	};
};
class CfgSoundShaders
{
	class base_closeShot_SoundShader;
	class base_midShot_SoundShader;
	class base_distShot_SoundShader;
	class KODK98_closeShot_SoundShader: base_closeShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\K98\sound\k98_close",
				1
			}
		};
		volume=1;
	};
	class KODK98_midShot_SoundShader: base_midShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\K98\sound\k98_mid",
				1
			}
		};
		volume=0.80000001;
	};
	class KODK98_distShot_SoundShader: base_distShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\K98\sound\k98_far",
				1
			}
		};
		volume=0.60000002;
	};
};
class CfgSoundSets
{
	class Rifle_Shot_Base_SoundSet;
	class KODK98_Shot_SoundSet: Rifle_Shot_Base_SoundSet
	{
		soundShaders[]=
		{
			"KODK98_closeShot_SoundShader",
			"KODK98_midShot_SoundShader",
			"KODK98_distShot_SoundShader"
		};
	};
};
