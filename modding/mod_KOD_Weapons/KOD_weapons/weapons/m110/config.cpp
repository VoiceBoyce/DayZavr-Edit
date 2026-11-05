class CfgPatches
{
	class KOD_Weapons_M110
	{
		units[]={};
		weapons[]=
		{
			"KOD_M110_Tan",
			"KOD_M110_Black"
		};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Firearms",
			"DZ_Sounds_Effects",
			"DZ_Weapons_Magazines"
		};
		magazines[]=
		{
			"Mag_KOD_M110_10rnd",
			"Mag_KOD_M110_20rnd"
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
	class KOD_M110_Base: Rifle_Base
	{
		scope=0;
		displayName="M110";
		descriptionShort="$STR_KOD_M110_Tan";
		weight=3900;
		model="KOD_weapons\weapons\M110\m110.p3d";
		absorbency=0;
		repairableWithKits[]={1};
		repairCosts[]={25};
		PPDOFProperties[]={1,0.60000002,50,200,4,10};
		WeaponLength=1.1;
		barrelArmor=3.888;
		initSpeedMultiplier=1.1;
		attachments[]=
		{
			"weaponWrap",
			"weaponOptics",
			"weaponOpticsHunting",
			"weaponFlashlight",
			"suppressorImpro",
			"KODRISBipod",
			"KODSticker1",
			"KODSticker2",
			"KODSticker3"
		};
		itemSize[]={9,3};
		chamberSize=1;
		chamberedRound="";
		chamberableFrom[]=
		{
			"Ammo_308Win",
			"Ammo_308WinTracer",
			"KOD_Ammo_308WinAP"
		};
		magazines[]=
		{
			"Mag_KOD_M110_10rnd",
			"Mag_KOD_M110_20rnd"
		};
		ejectType=1;
		recoilModifier[]={1.3,1.3,1.3};
		swayModifier[]={2.2,2.2,1.15};
		hiddenSelections[]=
		{
			"color"
		};
		simpleHiddenSelections[]=
		{
			"hide_barrel",
			"folding_lowered",
			"folding_raised"
		};
		modes[]=
		{
			"SemiAuto"
		};
		class SemiAuto: Mode_SemiAuto
		{
			soundSetShot[]=
			{
				"M110_Shot_SoundSet",
				"M14_Tail_SoundSet",
				"M14_InteriorTail_SoundSet"
			};
			soundSetShotExt[]=
			{
				
				{
					"DZVR_Rifle_silencer_SoundSet",
					"M14_silencerHomeMadeTail_SoundSet",
					"M14_silencerInteriorHomeMadeTail_SoundSet"
				}
			};
			reloadTime=0.125;
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
				maxOverheatingValue=120;
				shotsToStartOverheating=2;
				overheatingDecayInterval=1;
				class SmokingBarrel1
				{
					positionOffset[]={-0.02,0,0};
					overrideParticle="smoking_barrel_small";
					onlyWithinOverheatLimits[]={0,0.2};
					onlyWithinRainLimits[]={0,0.2};
				};
				class SmokingBarrelHot1
				{
					positionOffset[]={0.1,0,0};
					overrideParticle="smoking_barrel";
					onlyWithinOverheatLimits[]={0.2,0.60000002};
					onlyWithinRainLimits[]={0,0.2};
				};
				class SmokingBarrelHot3
				{
					positionOffset[]={0.30000001,0,0};
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
					hitpoints=300;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\M110\data\m110.rvmat",
								"KOD_weapons\weapons\M110\data\ironsights.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\M110\data\m110.rvmat",
								"KOD_weapons\weapons\M110\data\ironsights.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\M110\data\m110_damage.rvmat",
								"KOD_weapons\weapons\M110\data\ironsights_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\M110\data\m110_damage.rvmat",
								"KOD_weapons\weapons\M110\data\ironsights_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\M110\data\m110_destruct.rvmat",
								"KOD_weapons\weapons\M110\data\ironsights_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_M110_Tan: KOD_M110_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\M110\data\m110_co.paa"
		};
	};
	class KOD_M110_Black: KOD_M110_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\M110\data\m110_black_co.paa"
		};
	};
};
class cfgMagazines
{
	class Magazine_Base;
	class Mag_KOD_M110_10rnd: Magazine_Base
	{
		scope=2;
		displayName="Маг. M110 на 10 пат.";
		descriptionShort="$STR_Mag_KOD_M110_10Rnd";
		model="KOD_weapons\weapons\M110\m110_mag_10rnd.p3d";
		weight=75;
		itemSize[]={1,2};
		count=10;
		ammo="Bullet_308Win";
		ammoItems[]=
		{
			"Ammo_308Win",
			"Ammo_308WinTracer",
			"KOD_Ammo_308WinAP"
		};
		tracersEvery=0;
		inventorySlot[]+=
		{
			"magazine",
			"magazine2",
			"magazine3",
			"magazine_vest"
		};
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
								"KOD_weapons\weapons\M110\data\m110.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\M110\data\m110.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\M110\data\m110_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\M110\data\m110_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\M110\data\m110_destruct.rvmat"
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
	class Mag_KOD_M110_20rnd: Magazine_Base
	{
		scope=2;
		displayName="Маг. M110 на 20 пат.";
		descriptionShort="$STR_Mag_KOD_M110_20Rnd";
		model="KOD_weapons\weapons\M110\m110_mag.p3d";
		weight=150;
		itemSize[]={1,3};
		count=20;
		ammo="Bullet_308Win";
		ammoItems[]=
		{
			"Ammo_308Win",
			"Ammo_308WinTracer",
			"KOD_Ammo_308WinAP"
		};
		tracersEvery=0;
		inventorySlot[]+=
		{
			"magazine",
			"magazine2",
			"magazine3",
			"magazine_vest"
		};
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
								"KOD_weapons\weapons\M110\data\m110.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\M110\data\m110.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\M110\data\m110_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\M110\data\m110_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\M110\data\m110_destruct.rvmat"
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
class CfgNonAIVehicles
{
	class ProxyAttachment;
	class Proxym110_mag: ProxyAttachment
	{
		scope=0;
		inventorySlot="magazine";
		model="KOD_weapons\weapons\M110\m110_mag.p3d";
	};
	class Proxym110_mag_10rnd: ProxyAttachment
	{
		scope=0;
		inventorySlot="magazine";
		model="KOD_weapons\weapons\M110\m110_mag_10rnd.p3d";
	};
};
class CfgSoundShaders
{
	class base_closeShot_SoundShader;
	class base_midShot_SoundShader;
	class base_distShot_SoundShader;
	class M110_closeShot_SoundShader: base_closeShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\M110\sound\M110_close",
				1
			}
		};
		volume=0.80000001;
	};
	class M110_midShot_SoundShader: base_midShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\M110\sound\M110_mid",
				1
			}
		};
		volume=0.5;
	};
	class M110_distShot_SoundShader: base_distShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\M110\sound\M110_far",
				1
			}
		};
		volume=0.34999999;
	};
};
class CfgSoundSets
{
	class Rifle_Shot_Base_SoundSet;
	class M110_Shot_SoundSet: Rifle_Shot_Base_SoundSet
	{
		soundShaders[]=
		{
			"M110_closeShot_SoundShader",
			"M110_midShot_SoundShader",
			"M110_distShot_SoundShader"
		};
	};
};
