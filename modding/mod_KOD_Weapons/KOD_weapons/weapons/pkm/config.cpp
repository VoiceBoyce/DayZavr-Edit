class CfgPatches
{
	class KOD_Weapons_PKM
	{
		units[]={};
		weapons[]=
		{
			"KOD_PKM"
		};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Firearms",
			"DZ_Sounds_Effects"
		};
		magazines[]=
		{
			"Mag_KOD_PKM_100rnd"
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
	class KOD_PKM_Base: Rifle_Base
	{
		scope=0;
		displayName="ПКМ";
		descriptionShort="$STR_KOD_PKM";
		model="KOD_weapons\weapons\PKM\PKM.p3d";
		attachments[]=
		{
			"weaponWrap",
			"weaponOpticsAK",
			"KODPKPBipod",
			"KODSticker1",
			"KODSticker2",
			"KODSticker3"
		};
		itemSize[]={10,3};
		weight=9000;
		absorbency=0;
		repairableWithKits[]={1};
		repairCosts[]={30,50};
		PPDOFProperties[]={1,0.5,20,170,4,10};
		initSpeedMultiplier=1.1;
		WeaponLength=0.92906702;
		barrelArmor=8.5;
		chamberSize=1;
		chamberedRound="";
		chamberableFrom[]=
		{
			"Ammo_762x54",
			"Ammo_762x54Tracer"
		};
		magazines[]=
		{
			"Mag_KOD_PKP_150rnd",
			"Mag_KOD_PKM_100rnd"
		};
		ejectType=1;
		recoilModifier[]={1.65,1.25,1.35};
		swayModifier[]={1.5,1.2,1.1};
		hiddenSelections[]=
		{
			"color"
		};
		simpleHiddenSelections[]=
		{
			"hide_barrel",
			"ammobelt"
		};
		modes[]=
		{
			"FullAuto"
		};
		class FullAuto: Mode_FullAuto
		{
			soundSetShot[]=
			{
				"PKM_Shot_SoundSet",
				"Mosin_Tail_SoundSet",
				"Mosin_InteriorTail_SoundSet"
			};
			reloadTime=0.086000003;
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
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=700;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\PKM\data\pkm.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\PKM\data\pkm.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\PKM\data\pkm_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\PKM\data\pkm_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\PKM\data\pkm_destruct.rvmat"
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
				maxOverheatingValue=80;
				shotsToStartOverheating=20;
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
	class KOD_PKM: KOD_PKM_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\PKM\data\pkm_co.paa"
		};
	};
};
class cfgMagazines
{
	class Magazine_Base;
	class Mag_KOD_PKM_100rnd: Magazine_Base
	{
		scope=2;
		displayName="Короб ПКМ, 100 пат.";
		descriptionShort="$STR_Mag_KOD_PKM_100Rnd";
		model="KOD_weapons\weapons\PKM\PKM_mag.p3d";
		weight=500;
		itemSize[]={3,3};
		count=100;
		ammo="Bullet_762x54";
		ammoItems[]=
		{
			"Ammo_762x54",
			"Ammo_762x54Tracer"
		};
		tracersEvery=0;
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
								"KOD_weapons\weapons\PKM\data\pkm.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\PKM\data\pkm.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\PKM\data\pkm_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\PKM\data\pkm_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\PKM\data\pkm_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class MagRifle_fill_in
				{
					soundSet="MagRifle_fill_in_SoundSet";
					id=1;
				};
				class MagRifle_fill_loop
				{
					soundSet="MagRifle_fill_loop_SoundSet";
					id=2;
				};
				class MagRifle_fill_out
				{
					soundSet="MagRifle_fill_out_SoundSet";
					id=3;
				};
				class MagRifle_empty_in
				{
					soundSet="MagRifle_empty_in_SoundSet";
					id=4;
				};
				class MagRifle_empty_loop
				{
					soundSet="MagRifle_empty_loop_SoundSet";
					id=5;
				};
				class MagRifle_empty_out
				{
					soundSet="MagRifle_empty_out_SoundSet";
					id=6;
				};
				class MagPistol_fill_in
				{
					soundSet="MagPistol_fill_in_SoundSet";
					id=7;
				};
				class MagPistol_fill_loop
				{
					soundSet="MagPistol_fill_loop_SoundSet";
					id=8;
				};
				class MagPistol_fill_out
				{
					soundSet="MagPistol_fill_out_SoundSet";
					id=9;
				};
				class MagPistol_empty_in
				{
					soundSet="MagPistol_empty_in_SoundSet";
					id=10;
				};
				class MagPistol_empty_loop
				{
					soundSet="MagPistol_empty_loop_SoundSet";
					id=11;
				};
				class MagPistol_empty_out
				{
					soundSet="MagPistol_empty_out_SoundSet";
					id=12;
				};
			};
		};
	};
};
class CfgSoundShaders
{
	class base_closeShot_SoundShader;
	class base_midShot_SoundShader;
	class base_distShot_SoundShader;
	class PKM_closeShot_SoundShader: base_closeShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\sounds\pkm_single_1",
				1
			}
		};
		volume=1;
	};
	class PKM_midShot_SoundShader: base_midShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\sounds\pkm_single_1",
				1
			}
		};
		volume=0.80000001;
	};
	class PKM_distShot_SoundShader: base_distShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\sounds\pkm_single_1",
				1
			}
		};
		volume=0.5;
	};
};
class CfgSoundSets
{
	class Rifle_Shot_Base_SoundSet;
	class PKM_Shot_SoundSet: Rifle_Shot_Base_SoundSet
	{
		soundShaders[]=
		{
			"PKM_closeShot_SoundShader",
			"PKM_midShot_SoundShader",
			"PKM_distShot_SoundShader"
		};
	};
};
class CfgNonAIVehicles
{
	class ProxyAttachment;
	class ProxyPKM_mag: ProxyAttachment
	{
		scope=0;
		inventorySlot="magazine";
		model="KOD_weapons\weapons\PKM\PKM_mag.p3d";
	};
};
