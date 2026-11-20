class CfgPatches
{
	class KOD_Weapons_URBANA
	{
		units[]={};
		weapons[]=
		{
			"KOD_DVL10"
		};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Magazines",
			"DZ_Weapons_Firearms"
		};
		magazines[]=
		{
			"Mag_KOD_DVL10_5rnd"
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
	class BoltActionRifle_Base;
	class KOD_DVL10_Base: BoltActionRifle_Base
	{
		scope=0;
		displayName="ДВЛ-10 M2 Урбана";
		descriptionShort="$STR_KOD_DVL10";
		model="KOD_weapons\weapons\dvl10\dvl10.p3d";
		attachments[]=
		{
			"weaponWrap",
			"weaponOpticsHunting",
			"weaponMuzzleAWM",
			"KODRISBipod",
			"KODSticker1",
			"KODSticker2",
			"KODSticker3"
		};
		itemSize[]={10,3};
		weight=6000;
		absorbency=0;
		repairableWithKits[]={1};
		repairCosts[]={30,50};
		PPDOFProperties[]={};
		initSpeedMultiplier=1.2;
		WeaponLength=1.21;
		chamberSize=1;
		chamberedRound="";
		chamberableFrom[]=
		{
			"KOD_Ammo_338"
		};
		magazines[]=
		{
			"Mag_KOD_DVL10_5rnd"
		};
		barrelArmor=3.888;
		ejectType=0;
		recoilModifier[]={3,2,2};
		swayModifier[]={2.1500001,2.1500001,1.15};
		simpleHiddenSelections[]=
		{
			"hide_barrel",
			"hide_muzzle"
		};
		hiddenSelections[]=
		{
			"camo",
			"camo2",
			"camo3",
			"camo4",
			"camo5"
		};
		modes[]=
		{
			"Single"
		};
		class Single: Mode_SemiAuto
		{
			soundSetShot[]=
			{
				"DZVR_KOD_DVL_Shot_SoundSet",
				"CZ550_Shot_iterior_SoundSet",
				"CZ550_Tail_SoundSet",
				"CZ550_InteriorTail_SoundSet",
				"CZ550_Tail_2D_SoundSet"
			};
			soundSetShotExt[]=
			{
				
				{
					"DZVR_KOD_AWM_Silencer_SoundSet",
					"SVD_silencerTail_SoundSet",
					"SVD_silencerInteriorTail_SoundSet"
				}
			};
			reloadTime=1;
			dispersion=9.9999997e-05;
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
					illuminateWorld=0;
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
					hitpoints=200;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\dvl10\data\dvl_body.rvmat",
								"KOD_weapons\weapons\dvl10\data\dvl_barrel.rvmat",
								"KOD_weapons\weapons\dvl10\data\dvl_barrel1.rvmat",
								"KOD_weapons\weapons\dvl10\data\dvl_grip.rvmat",
								"KOD_weapons\weapons\dvl10\data\dvl_handguard.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\dvl10\data\dvl_body.rvmat",
								"KOD_weapons\weapons\dvl10\data\dvl_barrel.rvmat",
								"KOD_weapons\weapons\dvl10\data\dvl_barrel1.rvmat",
								"KOD_weapons\weapons\dvl10\data\dvl_grip.rvmat",
								"KOD_weapons\weapons\dvl10\data\dvl_handguard.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\dvl10\data\dvl_body_damage.rvmat",
								"KOD_weapons\weapons\dvl10\data\dvl_barrel_damage.rvmat",
								"KOD_weapons\weapons\dvl10\data\dvl_barrel1_damage.rvmat",
								"KOD_weapons\weapons\dvl10\data\dvl_grip_damage.rvmat",
								"KOD_weapons\weapons\dvl10\data\dvl_handguard_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\dvl10\data\dvl_body_badlydamage.rvmat",
								"KOD_weapons\weapons\dvl10\data\dvl_barrel_badlydamage.rvmat",
								"KOD_weapons\weapons\dvl10\data\dvl_barrel1_badlydamage.rvmat",
								"KOD_weapons\weapons\dvl10\data\dvl_grip_badlydamage.rvmat",
								"KOD_weapons\weapons\dvl10\data\dvl_handguard_badlydamage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\dvl10\data\dvl_body_destruct.rvmat",
								"KOD_weapons\weapons\dvl10\data\dvl_barrel_destruct.rvmat",
								"KOD_weapons\weapons\dvl10\data\dvl_barrel1_destruct.rvmat",
								"KOD_weapons\weapons\dvl10\data\dvl_grip_destruct.rvmat",
								"KOD_weapons\weapons\dvl10\data\dvl_handguard_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_DVL10: KOD_DVL10_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"kod_weapons\weapons\dvl10\data\dvl_body_co.paa",
			"KOD_weapons\weapons\dvl10\data\dvl_barrel_co.paa",
			"KOD_weapons\weapons\dvl10\data\dvl_barrel1_co.paa",
			"KOD_weapons\weapons\dvl10\data\dvl_grip_co.paa",
			"KOD_weapons\weapons\dvl10\data\dvl_handguard_co.paa"
		};
	};
};
class CfgMagazines
{
	class Magazine_Base;
	class Mag_KOD_DVL10_5rnd: Magazine_Base
	{
		scope=2;
		displayName="Маг. ДВЛ-10 на 5 пат.";
		descriptionShort="$STR_Mag_KOD_DVL10_5Rnd";
		model="KOD_weapons\weapons\dvl10\dvl10_magazine.p3d";
		weight=120;
		itemSize[]={1,2};
		count=5;
		ammo="KOD_Bullet_338";
		ammoItems[]=
		{
			"KOD_Ammo_338"
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
								"KOD_weapons\weapons\dvl10\data\dvl_magazine.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\dvl10\data\dvl_magazine.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\dvl10\data\dvl_magazine_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\dvl10\data\dvl_magazine_badlydamage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\dvl10\data\dvl_magazine_destruct.rvmat"
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
