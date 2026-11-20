class CfgPatches
{
	class KOD_Weapons_PPSh
	{
		units[]={};
		weapons[]=
		{
			"KOD_PPSh"
		};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Firearms",
			"DZ_Weapons_Magazines",
			"DZ_Sounds_Effects"
		};
		magazines[]=
		{
			"Mag_KOD_PPSh_71rnd"
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
	class KOD_PPSh_Base: Rifle_Base
	{
		scope=0;
		displayName="PPSh-41";
		descriptionShort="Soviet submachine gun developed in 1940 by G.S. Shpagin. The PPSh, along with the PPS-43, was the main submachine gun of the Soviet Armed Forces during the Great Patriotic War. Chambered for 7.62x25 mm rounds.";
		model="KOD_weapons\Weapons\PPSH\ppsh.p3d";
		itemSize[]={8,3};
		attachments[]=
		{
			"weaponWrap",
			"KODSticker1",
			"KODSticker2",
			"KODSticker3"
		};
		weight=3600;
		absorbency=0;
		repairableWithKits[]={1};
		repairCosts[]={30,25};
		PPDOFProperties[]={1,0.5,50,160,4,10};
		WeaponLength=0.80000001;
		initSpeedMultiplier=1;
		barrelArmor=1.367;
		chamberSize=1;
		chamberedRound="";
		chamberableFrom[]=
		{
			"KOD_Ammo_762x25"
		};
		magazines[]=
		{
			"Mag_KOD_PPSh_71rnd"
		};
		ejectType=1;
		recoilModifier[]={0.89999998,0.89999998,0.89999998};
		swayModifier[]={1.1,1.1,0.89999998};
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
			"FullAuto"
		};
		class FullAuto: Mode_FullAuto
		{
			soundSetShot[]=
			{
				"PPSh_Shot_SoundSet",
				"MP5K_Tail_SoundSet",
				"MP5K_InteriorTail_SoundSet"
			};
			soundSetShotExt[]=
			{
				
				{
					"MP5K_silencerPro_SoundSet",
					"MP5K_silencerTail_SoundSet",
					"MP5K_silencerInteriorTail_SoundSet"
				}
			};
			reloadTime=0.090000004;
			dispersion=0.0049999999;
			magazineSlot="magazine";
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
		class Particles
		{
			class OnFire
			{
				class MuzzleFlash
				{
					overrideParticle="weapon_shot_ak74_01";
					ignoreIfSuppressed=1;
					illuminateWorld=1;
				};
				class SmokeCloud
				{
					overrideParticle="weapon_shot_winded_smoke_small";
				};
			};
			class OnOverheating
			{
				shotsToStartOverheating=3;
				maxOverheatingValue=20;
				overheatingDecayInterval=1;
				class SmokingBarrel
				{
					overrideParticle="smoking_barrel_small";
					onlyWithinOverheatLimits[]={0,0.80000001};
					onlyWithinRainLimits[]={0,0.2};
				};
				class SmokingBarrelHotSteam
				{
					overrideParticle="smoking_barrel_steam_small";
					positionOffset[]={0.1,0.02,0};
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
					hitpoints=250;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\PPSH\data\ppsh.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\PPSH\data\ppsh.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\PPSH\data\ppsh_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\PPSH\data\ppsh_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\PPSH\data\ppsh_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_PPSh: KOD_PPSh_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\PPSH\data\ppsh_co.paa"
		};
	};
};
class cfgMagazines
{
	class Magazine_Base;
	class Mag_KOD_PPSh_71rnd: Magazine_Base
	{
		scope=2;
		displayName="71rd PPSh-41 Mag";
		descriptionShort="Detachable box magazine for the PPSh-41. Holds up to 71 rounds of 7.62x25 mm.";
		model="KOD_weapons\Weapons\PPSH\ppsh_mag.p3d";
		weight=100;
		itemSize[]={3,3};
		count=71;
		ammo="KOD_Bullet_762x25";
		ammoItems[]=
		{
			"KOD_Ammo_762x25"
		};
		tracersEvery=0;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=120;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\PPSH\data\ppsh.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\PPSH\data\ppsh.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\PPSH\data\ppsh_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\PPSH\data\ppsh_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\PPSH\data\ppsh_destruct.rvmat"
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
	class PPSh_closeShot_SoundShader: base_closeShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\PPSH\sound\ppsh_close",
				1
			}
		};
		volume=1;
	};
	class PPSh_midShot_SoundShader: base_midShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\PPSH\sound\ppsh_mid",
				1
			}
		};
		volume=0.60000002;
	};
	class PPSh_distShot_SoundShader: base_distShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\PPSH\sound\ppsh_far",
				1
			}
		};
		volume=0.30000001;
	};
};
class CfgSoundSets
{
	class Pistol_Shot_Base_SoundSet;
	class PPSh_Shot_SoundSet: Pistol_Shot_Base_SoundSet
	{
		soundShaders[]=
		{
			"PPSh_closeShot_SoundShader",
			"PPSh_midShot_SoundShader",
			"PPSh_distShot_SoundShader",
			"Glock19_Closure_SoundShader"
		};
	};
};
