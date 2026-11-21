class CfgPatches
{
	class KOD_Weapons_KS23
	{
		units[]={};
		weapons[]=
		{
			"KOD_KS23"
		};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data"
		};
		magazines[]=
		{
			"KOD_Ammo_23x75"
		};
		ammo[]=
		{
			"KOD_Bullet_23x75"
		};
	};
};
class Mode_Safe;
class Mode_SemiAuto;
class Mode_Burst;
class Mode_FullAuto;
class OpticsInfoShotgun;
class cfgWeapons
{
	class Shotgun_Base;
	class KOD_KS23_Base: Shotgun_Base
	{
		scope=0;
		displayName="КС-23";
		descriptionShort="Многофункциональное полицейское оружие, предназначенное для пресечения массовых беспорядков, избирательного силового, психического и химического воздействия на правонарушителей. Использует патрон 23х75 мм.";
		model="KOD_weapons\weapons\CS23\cs23.p3d";
		attachments[]=
		{
			"KODSticker1",
			"KODSticker2",
			"KODSticker3"
		};
		itemSize[]={6,3};
		weight=3850;
		absorbency=0;
		repairableWithKits[]={1};
		repairCosts[]={25};
		chamberSize=4;
		chamberedRound="";
		chamberableFrom[]=
		{
			"KOD_Ammo_23x75"
		};
		initSpeedMultiplier=1.9;
		magazines[]={};
		DisplayMagazine=0;
		PPDOFProperties[]={1,0.5,10,140,4,10};
		WeaponLength=1.9;
		barrelArmor=2.25;
		ejectType=0;
		recoilModifier[]={3,3,3};
		swayModifier[]={2.4000001,2.4000001,1.3};
		cartridgeCreateTime[]={0.55000001,0.81999999};
		reloadSkips[]={0.25999999,0.40000001,0.52999997,0.63999999,0.73500001};
		drySound[]=
		{
			"dz\sounds\weapons\shotguns\Izh43\izh43_dry",
			0.5,
			1,
			20
		};
		reloadMagazineSound[]=
		{
			"dz\sounds\weapons\shotguns\mp133\mp133_reload_0",
			0.80000001,
			1,
			20
		};
		hiddenSelections[]=
		{
			"color"
		};
		modes[]=
		{
			"Single"
		};
		class Particles
		{
			class OnFire
			{
				class SmokeCloud
				{
					overrideParticle="weapon_shot_winded_smoke";
				};
				class SmokeCloud2
				{
					overrideParticle="weapon_shot_winded_smoke";
				};
				class SmokeCloud3BadlyDamaged
				{
					overrideParticle="weapon_shot_winded_smoke";
					onlyWithinHealthLabel[]={3,4};
				};
				class MuzzleFlash
				{
					overrideParticle="weapon_shot_mp133_01";
					illuminateWorld=1;
				};
				class BadlyDamagedChamberSmoke
				{
					overrideParticle="weapon_shot_chamber_smoke";
					overridePoint="Nabojnicestart";
					overrideDirectionPoint="Nabojniceend";
					onlyWithinHealthLabel[]={3,4};
				};
				class BadlyDamagedChamberSmokeRaise
				{
					overrideParticle="weapon_shot_chamber_smoke_raise";
					overridePoint="Nabojnicestart";
					onlyWithinHealthLabel[]={3,4};
				};
			};
			class OnOverheating
			{
				maxOverheatingValue=3;
				shotsToStartOverheating=1;
				overheatingDecayInterval=3;
				class SmokingBarrel
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
		class Single: Mode_SemiAuto
		{
			soundSetShot[]=
			{
				"Mp133_Shot_SoundSet",
				"Mp133_Tail_SoundSet",
				"Mp133_InteriorTail_SoundSet"
			};
			reloadTime=1;
			dispersion=0.0099999998;
			firespreadangle=1.9;
			magazineSlot="magazine";
		};
		class OpticsInfo: OpticsInfoShotgun
		{
			memoryPointCamera="eye";
			discreteDistance[]={25};
			discreteDistanceInitIndex=0;
			modelOptics="-";
			distanceZoomMin=25;
			distanceZoomMax=25;
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=500;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\CS23\data\cs23.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\CS23\data\cs23.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\CS23\data\cs23_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\CS23\data\cs23_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\CS23\data\cs23_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_KS23: KOD_KS23_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\CS23\data\cs23_co.paa"
		};
	};
};
class CfgMagazines
{
	class Ammunition_Base;
	class KOD_Ammo_23x75: Ammunition_Base
	{
		scope=2;
		displayName="Шрапнель-25";
		descriptionShort="23х75 мм патрон с зарядом картечи. Дальность применения до 25 метров.";
		model="KOD_weapons\weapons\CS23\ammo.p3d";
		iconCartridge=4;
		weight=40;
		count=10;
		ammo="KOD_Bullet_23x75";
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
								"KOD_weapons\weapons\CS23\data\cs23.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\CS23\data\cs23.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\CS23\data\cs23_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\CS23\data\cs23_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\CS23\data\cs23_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
};
class cfgAmmoTypes
{
	class AType_KOD_Bullet_23x75
	{
		name="KOD_Bullet_23x75";
	};
};
class CfgAmmo
{
	class Shotgun_Base;
	class KOD_Bullet_23x75: Shotgun_Base
	{
		scope=2;
		proxyShape="\dz\weapons\projectiles\shotgunshell_pellets.p3d";
		muzzleFlashParticle="weapon_shot_pellets";
		casing="FxCartridge_12Pellet";
		round="FxRound_12Pellet";
		spawnPileType="KOD_Ammo_23x75";
		hit=6;
		indirectHit=0;
		indirectHitRange=0;
		hitShockHead=20000;
		hitShock=400;
		hitBlood=-200;
		hitHealth=-500;
		bleedChance=2;
		initSpeed=340;
		typicalSpeed=420;
		airFriction=-0.00575;
		caliber=0.55000001;
		deflecting=0;
		dispersion=0.059999999;
		projectilesCount=15;
		damageBarrel=600;
		damageBarrelDestroyed=600;
		weight=0.0049999999;
		impactBehaviour=0;
		hitAnimation=0;
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
				damage=50;
			};
			class Blood
			{
				damage=200;
			};
			class Shock
			{
				damage=70;
			};
		};
		class NoiseHit
		{
			strength=5;
			type="shot";
		};
	};
};
