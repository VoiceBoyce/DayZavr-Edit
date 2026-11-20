class CfgPatches
{
	class KOD_Weapons_M1928A1
	{
		units[]={};
		weapons[]=
		{
			"KOD_M1928A1"
		};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Firearms",
			"DZ_Weapons_Magazines"
		};
		magazines[]=
		{
			"Mag_KOD_M1928A1_30rnd",
			"Mag_KOD_M1928A1_100rnd"
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
	class KOD_M1928A1_Base: Rifle_Base
	{
		scope=0;
		displayName="M1928A1";
		descriptionShort="American submachine gun first invented by John T. Thompson in 1918 during World War I and became infamous during the Prohibition era. Fed from detachable magazines, uses .45 ACP rounds.";
		weight=4900;
		model="KOD_weapons\weapons\M1928A1\thompson.p3d";
		absorbency=0;
		repairableWithKits[]={1};
		repairCosts[]={25};
		PPDOFProperties[]={};
		initSpeedMultiplier=1.1;
		barrelArmor=3;
		WeaponLength=0.60000002;
		attachments[]=
		{
			"weaponWrap",
			"pistolMuzzle",
			"KODSticker1",
			"KODSticker2",
			"KODSticker3"
		};
		itemSize[]={7,3};
		chamberSize=1;
		chamberedRound="";
		chamberableFrom[]=
		{
			"Ammo_45ACP"
		};
		magazines[]=
		{
			"Mag_KOD_M1928A1_30rnd",
			"Mag_KOD_M1928A1_100rnd"
		};
		ejectType=1;
		recoilModifier[]={1.1,1.05,1.0700001};
		swayModifier[]={1.35,1.3200001,1.1};
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
				"FNX45_Shot_SoundSet",
				"FNX45_Tail_SoundSet",
				"FNX45_InteriorTail_SoundSet"
			};
			soundSetShotExt[]=
			{
				{
					"UMP45_silencer_SoundSet",
					"UMP45_silencerTail_SoundSet",
					"UMP45_silencerInteriorTail_SoundSet"
				}
			};
			reloadTime=0.125;
			dispersion=0.0035000001;
			magazineSlot="magazine";
		};
		class FullAuto: Mode_FullAuto
		{
			soundSetShot[]=
			{
				"FNX45_Shot_SoundSet",
				"FNX45_Tail_SoundSet",
				"FNX45_InteriorTail_SoundSet"
			};
			soundSetShotExt[]=
			{
				{
					"UMP45_silencer_SoundSet",
					"UMP45_silencerTail_SoundSet",
					"UMP45_silencerInteriorTail_SoundSet"
				}
			};
			reloadTime=0.07;
			dispersion=0.0035000001;
			magazineSlot="magazine";
		};
		class OpticsInfo: OpticsInfoRifle
		{
			memoryPointCamera="eye";
			discreteDistance[]={50};
			discreteDistanceInitIndex=0;
			modelOptics="-";
			distanceZoomMin=50;
			distanceZoomMax=50;
		};
		class NoiseShoot
		{
			strength=66;
			type="shot";
		};
		class Particles
		{
			class OnFire
			{
				class MuzzleFlash
				{
					overrideParticle="weapon_shot_ump45_01";
					ignoreIfSuppressed=1;
					illuminateWorld=1;
				};
				class SmokeCloud
				{
					overrideParticle="weapon_shot_winded_smoke_small";
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
				shotsToStartOverheating=5;
				maxOverheatingValue=20;
				overheatingDecayInterval=1;
				class SmokingBarrel1
				{
					ignoreIfSuppressed=1;
					overrideParticle="smoking_barrel_small";
					onlyWithinOverheatLimits[]={0,0.30000001};
					onlyWithinRainLimits[]={0,0.2};
				};
				class SmokingBarrelHot1
				{
					ignoreIfSuppressed=1;
					overrideParticle="smoking_barrel";
					positionOffset[]={0.1,0,0};
					onlyWithinOverheatLimits[]={0.30000001,0.89999998};
					onlyWithinRainLimits[]={0,0.2};
				};
				class SmokingBarrelHot2
				{
					ignoreIfSuppressed=1;
					overrideParticle="smoking_barrel_heavy";
					positionOffset[]={0.1,0,0};
					onlyWithinOverheatLimits[]={0.89999998,1};
					onlyWithinRainLimits[]={0,0.2};
				};
				class SmokingBarrelHotSteam
				{
					overrideParticle="smoking_barrel_steam_small";
					positionOffset[]={0.1,0.050000001,0};
					onlyWithinOverheatLimits[]={0,0.5};
					onlyWithinRainLimits[]={0.2,1};
				};
				class SmokingBarrelHotSteam2
				{
					overrideParticle="smoking_barrel_steam";
					positionOffset[]={0.13,0.050000001,0};
					onlyWithinOverheatLimits[]={0.5,1};
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
								"KOD_weapons\weapons\M1928A1\data\m1928a1.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\M1928A1\data\m1928a1.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\M1928A1\data\m1928a1_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\M1928A1\data\m1928a1_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\M1928A1\data\m1928a1_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_M1928A1: KOD_M1928A1_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\M1928A1\data\m1928a1_co.paa"
		};
	};
};
class cfgMagazines
{
	class Magazine_Base;
	class Mag_KOD_M1928A1_30rnd: Magazine_Base
	{
		scope=2;
		displayName="30rd M1928A1 Mag";
		descriptionShort="Detachable box magazine for M1928A1 submachine gun. Holds up to 30 rounds of .45 ACP.";
		model="KOD_weapons\weapons\M1928A1\thompson_30rndmag.p3d";
		weight=150;
		itemSize[]={1,3};
		count=30;
		ammo="Bullet_45ACP";
		ammoItems[]=
		{
			"Ammo_45ACP"
		};
		tracersEvery=0;
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
								"KOD_weapons\weapons\M1928A1\data\30rndmag.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\M1928A1\data\30rndmag.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\M1928A1\data\30rndmag_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\M1928A1\data\30rndmag_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\M1928A1\data\30rndmag_destruct.rvmat"
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
	class Mag_KOD_M1928A1_100rnd: Magazine_Base
	{
		scope=2;
		displayName="100rd M1928A1 Mag";
		descriptionShort="Detachable drum magazine for M1928A1 submachine gun. Holds up to 100 rounds of .45 ACP.";
		model="KOD_weapons\weapons\M1928A1\thompson_100rndmag.p3d";
		weight=250;
		itemSize[]={3,3};
		count=100;
		ammo="Bullet_45ACP";
		ammoItems[]=
		{
			"Ammo_45ACP"
		};
		tracersEvery=0;
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
								"KOD_weapons\weapons\M1928A1\data\m1928a1.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\M1928A1\data\m1928a1.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\M1928A1\data\m1928a1_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\M1928A1\data\m1928a1_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\M1928A1\data\m1928a1_destruct.rvmat"
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
	class ProxyThompson_30rndmag: ProxyAttachment
	{
		scope=0;
		inventorySlot="magazine";
		model="KOD_weapons\weapons\M1928A1\thompson_30rndmag.p3d";
	};
	class ProxyThompson_100rndmag: ProxyAttachment
	{
		scope=0;
		inventorySlot="magazine";
		model="KOD_weapons\weapons\M1928A1\thompson_100rndmag.p3d";
	};
};
