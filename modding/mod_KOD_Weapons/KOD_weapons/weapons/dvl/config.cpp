class CfgPatches
{
	class KOD_Weapons_DVL
	{
		units[]=
		{
			"AmmoBox_40lw_20Rnd"
		};
		weapons[]=
		{
			"KOD_DVL10M1"
		};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Projectiles",
			"DZ_Weapons_Magazines",
			"DZ_Weapons_Ammunition",
			"DZ_Weapons_Firearms"
		};
		magazines[]=
		{
			"Mag_KOD_DVL10M1_5rnd",
			"KOD_Ammo_40lw"
		};
		ammo[]=
		{
			"Bullet_40lw"
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
	class KOD_DVL10M1_Base: BoltActionRifle_Base
	{
		scope=0;
		displayName="ДВЛ-10 M1";
		descriptionShort="$STR_KOD_DVL10M1";
		model="KOD_weapons\weapons\DVL\dvl.p3d";
		attachments[]=
		{
			"weaponWrap",
			"weaponOptics",
			"weaponOpticsHunting",
			"KODRISBipod",
			"KODSticker1",
			"KODSticker2",
			"KODSticker3"
		};
		itemSize[]={8,3};
		weight=5700;
		absorbency=0;
		repairableWithKits[]={1};
		repairCosts[]={30,50};
		initSpeedMultiplier=1.2;
		PPDOFProperties[]={};
		WeaponLength=1.0599999;
		barrelArmor=3.888;
		chamberSize=1;
		chamberedRound="";
		chamberableFrom[]=
		{
			"KOD_Ammo_40lw"
		};
		magazines[]=
		{
			"Mag_KOD_DVL10M1_5rnd"
		};
		ejectType=0;
		recoilModifier[]={0.80000001,0.80000001,0.80000001};
		swayModifier[]={1.4,1.4,0.89999998};
		simpleHiddenSelections[]=
		{
			"hide_barrel"
		};
		hiddenSelections[]=
		{
			"color1",
			"color2",
			"color3",
			"color4"
		};
		modes[]=
		{
			"Single"
		};
		class Single: Mode_SemiAuto
		{
			soundSetShot[]=
			{
				"Trumpet_silencerHomeMade_SoundSet",
				"Trumpet_silencerHomeMadeTail_SoundSet",
				"Trumpet_silencerInteriorHomeMadeTail_SoundSet"
			};
			reloadTime=1;
			dispersion=0.00050000002;
			magazineSlot="magazine";
		};
		class NoiseShoot
		{
			strength=3;
			type="shot";
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
					overrideParticle="weapon_shot_vss_01";
				};
				class GasPistonBurst
				{
					overrideParticle="weapon_shot_chamber_smoke";
					overridePoint="GasPiston";
					overrideDirectionVector[]={-90,0,0};
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
								"KOD_weapons\weapons\DVL\data\dvl.rvmat",
								"KOD_weapons\weapons\DVL\data\choke_muffler.rvmat",
								"KOD_weapons\weapons\DVL\data\muffler.rvmat",
								"KOD_weapons\weapons\DVL\data\grip.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\DVL\data\dvl.rvmat",
								"KOD_weapons\weapons\DVL\data\choke_muffler.rvmat",
								"KOD_weapons\weapons\DVL\data\muffler.rvmat",
								"KOD_weapons\weapons\DVL\data\grip.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\DVL\data\dvl_damage.rvmat",
								"KOD_weapons\weapons\DVL\data\choke_muffler_damage.rvmat",
								"KOD_weapons\weapons\DVL\data\muffler_damage.rvmat",
								"KOD_weapons\weapons\DVL\data\grip_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\DVL\data\dvl_damage.rvmat",
								"KOD_weapons\weapons\DVL\data\choke_muffler_damage.rvmat",
								"KOD_weapons\weapons\DVL\data\muffler_damage.rvmat",
								"KOD_weapons\weapons\DVL\data\grip_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\DVL\data\dvl_destruct.rvmat",
								"KOD_weapons\weapons\DVL\data\choke_muffler_destruct.rvmat",
								"KOD_weapons\weapons\DVL\data\muffler_destruct.rvmat",
								"KOD_weapons\weapons\DVL\data\grip_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_DVL10M1: KOD_DVL10M1_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\DVL\data\dvl_co.paa",
			"KOD_weapons\weapons\DVL\data\choke_muffler_co.paa",
			"KOD_weapons\weapons\DVL\data\muffler_co.paa",
			"KOD_weapons\weapons\DVL\data\grip_co.paa"
		};
	};
};
class cfgMagazines
{
	class Magazine_Base;
	class Mag_KOD_DVL10M1_5rnd: Magazine_Base
	{
		scope=2;
		displayName="Маг. ДВЛ-10 М1 на 5 пат.";
		descriptionShort="$STR_Mag_KOD_DVL10M1_5Rnd";
		model="KOD_weapons\weapons\DVL\dvl_mag.p3d";
		weight=150;
		itemSize[]={1,2};
		count=5;
		ammo="Bullet_40lw";
		ammoItems[]=
		{
			"KOD_Ammo_40lw"
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
								"KOD_weapons\weapons\DVL\data\dvl_mag.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\DVL\data\dvl_mag.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\DVL\data\dvl_mag_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\DVL\data\dvl_mag_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\DVL\data\dvl_mag_destruct.rvmat"
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
	class Ammunition_Base;
	class KOD_Ammo_40lw: Ammunition_Base
	{
		scope=2;
		displayName="Патроны .40 Lobaev Whisper";
		descriptionShort="Винтовочный патрон .40 Lobaev Whisper.";
		model="KOD_weapons\weapons\DVL\ammo_40lw.p3d";
		weight=10;
		count=20;
		ammo="Bullet_40lw";
		itemSize[]={1,2};
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
								"KOD_weapons\weapons\DVL\data\40lw.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\DVL\data\40lw.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\DVL\data\40lw_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\DVL\data\40lw_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\DVL\data\40lw_destruct.rvmat"
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
	class AType_Bullet_40lw
	{
		name="Bullet_40lw";
	};
};
class CfgAmmo
{
	class Bullet_Base;
	class Bullet_40lw: Bullet_Base
	{
		scope=2;
		cartridge="FxCartridge_762";
		spawnPileType="KOD_Ammo_40lw";
		hit=12;
		indirectHit=0;
		indirectHitRange=0;
		visibleFire=22;
		audibleFire=22;
		visibleFireTime=3;
		cost=1.2;
		airLock=1;
		typicalSpeed=600;
		initSpeed=600;
		airFriction=-0.00085000001;
		caliber=1.1;
		deflecting=10;
		damageBarrel=6;
		damageBarrelDestroyed=60;
		weight=0.017000001;
		impactBehaviour=1;
		hitAnimation=1;
		class DamageApplied
		{
			type="Projectile";
			dispersion=0;
			bleedThreshold=1;
			defaultDamageOverride[]=
			{
				{0.97000003,1}
			};
			class Health
			{
				damage=400;
			};
			class Blood
			{
				damage=400;
			};
			class Shock
			{
				damage=400;
			};
		};
		class NoiseHit
		{
			strength=8;
			type="shot";
		};
	};
};
class CfgVehicles
{
	class Box_Base;
	class AmmoBox_40lw_20Rnd: Box_Base
	{
		scope=2;
		displayName="Коробка патронов .40 Lobaev Whisper";
		descriptionShort="Патроны .40 Lobaev Whisper в коробке.";
		model="KOD_weapons\weapons\DVL\AmmoBox_40lw.p3d";
		rotationFlags=17;
		weight=250;
		class DamageSystem
		{
			class Health
			{
				hitpoints=100;
				healthLevels[]=
				{
					
					{
						1,
						
						{
							"KOD_weapons\weapons\DVL\data\ammobox_40lw.rvmat"
						}
					},
					
					{
						0.69999999,
						
						{
							"KOD_weapons\weapons\DVL\data\ammobox_40lw.rvmat"
						}
					},
					
					{
						0.5,
						
						{
							"KOD_weapons\weapons\DVL\data\ammobox_40lw_damage.rvmat"
						}
					},
					
					{
						0.30000001,
						
						{
							"KOD_weapons\weapons\DVL\data\ammobox_40lw_damage.rvmat"
						}
					},
					
					{
						0,
						
						{
							"KOD_weapons\weapons\DVL\data\ammobox_40lw_destruct.rvmat"
						}
					}
				};
			};
		};
		class Resources
		{
			class KOD_Ammo_40lw
			{
				value=20;
				variable="quantity";
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class interact
				{
					soundset="ammoboxUnpack_SoundSet";
					id=70;
				};
			};
		};
	};
};
