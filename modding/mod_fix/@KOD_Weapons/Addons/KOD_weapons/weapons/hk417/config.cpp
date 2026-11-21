class CfgPatches
{
	class KOD_Weapons_HK417
	{
		units[]={};
		weapons[]=
		{
			"KOD_HK417"
		};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Firearms",
			"DZ_Weapons_Magazines"
		};
		magazines[]=
		{
			"Mag_KOD_HK417_20rnd"
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
	class KOD_HK417_Base: Rifle_Base
	{
		scope=0;
		displayName="HK417";
		descriptionShort="$STR_KOD_HK417";
		weight=3900;
		model="KOD_weapons\weapons\HK417\hk417.p3d";
		absorbency=0;
		repairableWithKits[]={1};
		repairCosts[]={30,25};
		initSpeedMultiplier=1.1;
		PPDOFProperties[]={};
		barrelArmor=4.5;
		WeaponLength=1.1;
		attachments[]=
		{
			"weaponWrap",
			"weaponOptics",
			"weaponFlashlight",
			"suppressorImpro",
			"KODRISBipod",
			"KODSticker1",
			"KODSticker2",
			"KODSticker3"
		};
		itemSize[]={8,3};
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
			"Mag_KOD_HK417_20rnd",
			"Mag_KOD_Drum308_50rnd"
		};
		ejectType=1;
		recoilModifier[]={0.69999999,0.69999999,0.69999999};
		swayModifier[]={2,2,1.1};
		simpleHiddenSelections[]=
		{
			"hide_barrel",
			"hide_ironsights",
			"magazine"
		};
		hiddenSelections[]=
		{
			"color1",
			"color2"
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
				"DZVR_KOD_HK417_Shot_SoundSet",
				"FAL_Shot_1st_iterior_SoundSet",
				"FAL_Tail_SoundSet",
				"FAL_InteriorTail_SoundSet",
				"FAL_Slapback_SoundSet",
				"FAL_Tail_2D_SoundSet"
			};
			soundSetShotExt[]=
			{
				
				{
					"DZVR_KOD_FAL_Silencer_SoundSet",
					"FAL_silencerHomeMadeTail_SoundSet",
					"FAL_silencerInteriorHomeMadeTail_SoundSet"
				}
			};
			reloadTime=0.125;
			dispersion=0.0024999999;
			magazineSlot="magazine";
		};
		class FullAuto: Mode_FullAuto
		{
			soundSetShot1st[]=
			{
				"DZVR_KOD_HK417_Shot_SoundSet",
				"FAL_Shot_1st_iterior_SoundSet"
			};
			soundSetShot[]=
			{
				"DZVR_KOD_HK417_Shot_SoundSet",
				"FAL_Shot_iterior_SoundSet",
				"FAL_Tail_SoundSet",
				"FAL_InteriorTail_SoundSet",
				"FAL_Slapback_SoundSet",
				"FAL_Tail_2D_SoundSet"
			};
			soundSetShotExt1st[]=
			{
				
				{
					"DZVR_KOD_FAL_Silencer_SoundSet"
				}
			};
			soundSetShotExt[]=
			{
				
				{
					"DZVR_KOD_FAL_Silencer_SoundSet",
					"FAL_silencerHomeMadeTail_SoundSet",
					"FAL_silencerInteriorHomeMadeTail_SoundSet"
				}
			};
			reloadTime=0.07;
			dispersion=0.0024999999;
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
					hitpoints=400;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\HK417\data\hk417.rvmat",
								"KOD_weapons\weapons\HK417\data\hk417_acc.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\HK417\data\hk417.rvmat",
								"KOD_weapons\weapons\HK417\data\hk417_acc.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\HK417\data\hk417_damage.rvmat",
								"KOD_weapons\weapons\HK417\data\hk417_acc_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\HK417\data\hk417_damage.rvmat",
								"KOD_weapons\weapons\HK417\data\hk417_acc_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\HK417\data\hk417_destruct.rvmat",
								"KOD_weapons\weapons\HK417\data\hk417_acc_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_HK417: KOD_HK417_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\HK417\data\hk417_co.paa",
			"KOD_weapons\weapons\HK417\data\hk417_acc_co.paa"
		};
	};
};
class cfgMagazines
{
	class Magazine_Base;
	class Mag_KOD_HK417_20rnd: Magazine_Base
	{
		scope=2;
		displayName="Маг. HK417 на 20 пат.";
		descriptionShort="$STR_Mag_KOD_HK417_20Rnd";
		model="KOD_weapons\weapons\HK417\hk417_mag.p3d";
		weight=150;
		itemSize[]={1,2};
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
								"KOD_weapons\weapons\HK417\data\hk417_acc.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\HK417\data\hk417_acc.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\HK417\data\hk417_acc_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\HK417\data\hk417_acc_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\HK417\data\hk417_acc_destruct.rvmat"
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
