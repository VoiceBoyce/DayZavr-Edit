class CfgPatches
{
	class KOD_Weapons_VSK94
	{
		units[]=
		{
			"KOD_VSK94_Silencer",
			"KOD_VSK94_Silencer_Wrap",
			"KOD_VSK94_Hndgrd_Accs",
			"KOD_VSK94_Hndgrd",
			"KOD_VSK94_Bttstck"
		};
		weapons[]=
		{
			"KOD_VSK94"
		};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Weapons_Firearms",
			"DZ_Scripts"
		};
		magazines[]=
		{
			"Mag_KOD_VSK94_20rnd"
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
	class KOD_VSK94_Base: Rifle_Base
	{
		scope=0;
		displayName="VSK-94";
		descriptionShort="A 9x39mm suppressed designated marksman rifle designed In 1995 in the KBP Tula Instrument-making Design Bureau designed by Vasily Gryazev. Fed from a detachable magazine. Uses 9x39mm rounds.";
		model="KOD_weapons\weapons\VSK94\vsk_receiver.p3d";
		PPDOFProperties[]={1,0.5,50,180.10001,4,10};
		attachments[]=
		{
			"weaponWrap",
			"weaponButtstockKODVSK94",
			"WeaponHandguardKODVSK94",
			"weaponOpticsAK",
			"weaponOptics",
			"weaponFlashlight",
			"weaponMuzzleKODVSK94",
			"KODSticker1",
			"KODSticker2"
		};
		itemSize[]={7,3};
		weight=1600;
		absorbency=0;
		repairableWithKits[]={1};
		repairCosts[]={25};
		WeaponLength=0.52999997;
		barrelArmor=2.9000001;
		chamberSize=1;
		chamberedRound="";
		chamberableFrom[]=
		{
			"Ammo_9x39",
			"Ammo_9x39AP"
		};
		magazines[]=
		{
			"Mag_KOD_VSK94_20rnd"
		};
		ejectType=1;
		recoilModifier[]={1.2,1.2,1.2};
		swayModifier[]={2,2,1.1};
		simpleHiddenSelections[]=
		{
			"hide_rail"
		};
		hiddenSelections[]=
		{
			"color1",
			"color2",
			"color3"
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
				"ots14_Shot_SoundSet",
				"ots14_Tail_SoundSet",
				"ots14_InteriorTail_SoundSet"
			};
			soundSetShotExt[]=
			{
				
				{
					"PB6P9_silencerPro_SoundSet",
					"PB6P9_silencerTail_SoundSet",
					"PB6P9_silencerInteriorTail_SoundSet"
				}
			};
			reloadTime=0.125;
			dispersion=0.0022;
			magazineSlot="magazine";
		};
		class FullAuto: Mode_FullAuto
		{
			soundSetShot[]=
			{
				"ots14_Shot_SoundSet",
				"ots14_Tail_SoundSet",
				"ots14_InteriorTail_SoundSet"
			};
			soundSetShotExt[]=
			{
				
				{
					"PB6P9_silencerPro_SoundSet",
					"PB6P9_silencerTail_SoundSet",
					"PB6P9_silencerInteriorTail_SoundSet"
				}
			};
			reloadTime=0.08;
			dispersion=0.0022;
			magazineSlot="magazine";
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
				maxOverheatingValue=30;
				shotsToStartOverheating=15;
				overheatingDecayInterval=1;
				class SmokingBarrel1
				{
					overrideParticle="smoking_barrel_small";
					onlyWithinOverheatLimits[]={0,1};
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
		class OpticsInfo: OpticsInfoRifle
		{
			memoryPointCamera="eye";
			discreteDistance[]={50,100,150,200,250,300};
			discreteDistanceInitIndex=0;
			modelOptics="-";
			distanceZoomMin=50;
			distanceZoomMax=300;
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=550;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\VSK94\data\vsk94_receiver.rvmat",
								"KOD_weapons\weapons\VSK94\data\vsk94_butt_sight.rvmat",
								"KOD_weapons\weapons\VSK94\data\vsk94_scopemount.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\VSK94\data\vsk94_receiver.rvmat",
								"KOD_weapons\weapons\VSK94\data\vsk94_butt_sight.rvmat",
								"KOD_weapons\weapons\VSK94\data\vsk94_scopemount.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\VSK94\data\vsk94_receiver_damage.rvmat",
								"KOD_weapons\weapons\VSK94\data\vsk94_butt_sight_damage.rvmat",
								"KOD_weapons\weapons\VSK94\data\vsk94_scopemount_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\VSK94\data\vsk94_receiver_damage.rvmat",
								"KOD_weapons\weapons\VSK94\data\vsk94_butt_sight_damage.rvmat",
								"KOD_weapons\weapons\VSK94\data\vsk94_scopemount_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\VSK94\data\vsk94_receiver_destruct.rvmat",
								"KOD_weapons\weapons\VSK94\data\vsk94_butt_sight_destruct.rvmat",
								"KOD_weapons\weapons\VSK94\data\vsk94_scopemount_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_VSK94: KOD_VSK94_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\VSK94\data\vsk94_receiver_co.paa",
			"KOD_weapons\weapons\VSK94\data\vsk94_butt_sight_co.paa",
			"KOD_weapons\weapons\VSK94\data\vsk94_scopemount_co.paa"
		};
	};
};
class CfgMagazines
{
	class Magazine_Base;
	class Mag_KOD_VSK94_20rnd: Magazine_Base
	{
		scope=2;
		displayName="20rd VSK-94 Mag";
		descriptionShort="Detachable box magazine for VSK-94. Holds up to 20 rounds of 9x39mm.";
		model="KOD_weapons\weapons\VSK94\vsk_mag.p3d";
		weight=120;
		itemSize[]={1,2};
		count=20;
		ammo="Bullet_9x39";
		ammoItems[]=
		{
			"Ammo_9x39",
			"Ammo_9x39AP"
		};
		tracersEvery=0;
		hiddenSelections[]=
		{
			"magazine"
		};
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\VSK94\data\vsk94_mag_co.paa"
		};
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
								"KOD_weapons\weapons\VSK94\data\vsk94_mag.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\VSK94\data\vsk94_mag.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\VSK94\data\vsk94_mag_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\VSK94\data\vsk94_mag_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\VSK94\data\vsk94_mag_destruct.rvmat"
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
class cfgVehicles
{
	class ItemSuppressor;
	class KOD_VSK94_Silencer_Base: ItemSuppressor
	{
		scope=0;
		displayName="VSK-94 Silencer";
		descriptionShort="A muzzle device for VSK-94. Reduces sound intensity and muzzle flash generated by firing.";
		model="KOD_weapons\weapons\VSK94\vsk_silencer.p3d";
		rotationFlags=17;
		reversed=0;
		weight=150;
		itemSize[]={4,1};
		itemModelLength=0.36000001;
		barrelArmor=2250;
		hasRailFunctionality=1;
		inventorySlot[]=
		{
			"weaponMuzzleKODVSK94"
		};
		simulation="ItemSuppressor";
		dispersionModifier=-0.00050000002;
		dispersionCondition="true";
		noiseShootModifier=-0.94999999;
		recoilModifier[]={0.89999998,0.89999998,0.89999998};
		swayModifier[]={1.2,1.2,1.2};
		selectionFireAnim="zasleh";
		soundIndex=1;
		muzzlePos="usti hlavne";
		hiddenSelections[]=
		{
			"color1",
			"color2"
		};
		class Particles
		{
			class OnFire
			{
				class MuzzleFlash
				{
					overrideParticle="weapon_shot_fnx_02";
					onlyWithinHealthLabel[]={0,3};
				};
			};
			class OnOverheating
			{
				shotsToStartOverheating=3;
				maxOverheatingValue=10;
				overheatingDecayInterval=1;
				class SmokeTrail
				{
					overrideParticle="smoking_barrel_small";
					onlyWithinRainLimits[]={0,0.2};
				};
				class SmokingBarrelHotSteamSmall
				{
					overrideParticle="smoking_barrel_steam_small";
					positionOffset[]={0,0.1,0};
					onlyWithinOverheatLimits[]={0,1};
					onlyWithinRainLimits[]={0.2,1};
				};
			};
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=5;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\VSK94\data\vsk94_silencer.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\VSK94\data\vsk94_silencer.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\VSK94\data\vsk94_silencer_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\VSK94\data\vsk94_silencer_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\VSK94\data\vsk94_silencer_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_VSK94_Silencer: KOD_VSK94_Silencer_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\VSK94\data\vsk94_silencer_co.paa"
		};
	};
	class KOD_VSK94_Silencer_Wrap: KOD_VSK94_Silencer_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\VSK94\data\vsk94_silencer_co.paa",
			"KOD_weapons\weapons\VSK94\data\vsk94_silencer_wrap_co.paa"
		};
	};
	class Inventory_Base;
	class KOD_VSK94_Hndgrd_Base: Inventory_Base
	{
		scope=0;
		displayName="VSK-94 Handguard";
		descriptionShort="A polymer handguard for VSK-94 rifle.";
		model="KOD_weapons\weapons\VSK94\vsk_handguard.p3d";
		rotationFlags=17;
		reversed=0;
		weight=300;
		itemSize[]={2,1};
		inventorySlot[]=
		{
			"WeaponHandguardKODVSK94"
		};
		recoilModifier[]={0.69999999,0.69999999,0.69999999};
		swayModifier[]={0.89999998,0.89999998,0.89999998};
		hiddenSelections[]=
		{
			"color1",
			"color2"
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
								"KOD_weapons\weapons\VSK94\data\vsk94_hndgrd.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\VSK94\data\vsk94_hndgrd.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\VSK94\data\vsk94_hndgrd_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\VSK94\data\vsk94_hndgrd_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\VSK94\data\vsk94_hndgrd_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		isMeleeWeapon=1;
		class MeleeModes
		{
			class Default
			{
				ammo="MeleeLightBlunt";
				range=1;
			};
			class Heavy
			{
				ammo="MeleeLightBlunt_Heavy";
				range=1;
			};
			class Sprint
			{
				ammo="MeleeLightBlunt_Heavy";
				range=2.8;
			};
		};
	};
	class KOD_VSK94_Hndgrd_Accs: KOD_VSK94_Hndgrd_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\VSK94\data\vsk94_hndgrd_co.paa",
			"KOD_weapons\weapons\VSK94\data\vsk94_hndgrd_co.paa"
		};
	};
	class KOD_VSK94_Hndgrd: KOD_VSK94_Hndgrd_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\VSK94\data\vsk94_hndgrd_co.paa"
		};
	};
	class KOD_VSK94_Bttstck: Inventory_Base
	{
		scope=2;
		displayName="VSK-94 Buttstock";
		descriptionShort="A steel buttstock for VSK-94 rifle.";
		model="KOD_weapons\weapons\VSK94\vsk_buttstock.p3d";
		rotationFlags=17;
		reversed=0;
		inventorySlot[]=
		{
			"weaponButtstockKODVSK94"
		};
		weight=350;
		itemSize[]={2,1};
		recoilModifier[]={0.80000001,0.80000001,0.80000001};
		swayModifier[]={0.69999999,0.69999999,0.69999999};
		hiddenSelections[]=
		{
			"color"
		};
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\VSK94\data\vsk94_butt_sight_co.paa"
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
								"KOD_weapons\weapons\VSK94\data\vsk94_butt_sight.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\VSK94\data\vsk94_butt_sight.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\VSK94\data\vsk94_butt_sight_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\VSK94\data\vsk94_butt_sight_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\VSK94\data\vsk94_butt_sight_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		isMeleeWeapon=1;
		class MeleeModes
		{
			class Default
			{
				ammo="MeleeLightBlunt";
				range=1;
			};
			class Heavy
			{
				ammo="MeleeLightBlunt_Heavy";
				range=1;
			};
			class Sprint
			{
				ammo="MeleeLightBlunt_Heavy";
				range=2.8;
			};
		};
	};
};
class CfgNonAIVehicles
{
	class ProxyAttachment;
	class Proxyvsk_silencer: ProxyAttachment
	{
		scope=0;
		inventorySlot="weaponMuzzleKODVSK94";
		model="KOD_weapons\weapons\VSK94\vsk_silencer.p3d";
	};
	class Proxyvsk_mag: ProxyAttachment
	{
		scope=0;
		inventorySlot="magazine";
		model="KOD_weapons\weapons\VSK94\vsk_mag.p3d";
	};
	class Proxyvsk_handguard: ProxyAttachment
	{
		scope=0;
		inventorySlot="WeaponHandguardKODVSK94";
		model="KOD_weapons\weapons\VSK94\vsk_handguard.p3d";
	};
	class Proxyvsk_buttstock: ProxyAttachment
	{
		scope=0;
		inventorySlot="weaponButtstockKODVSK94";
		model="KOD_weapons\weapons\VSK94\vsk_buttstock.p3d";
	};
};
class CfgSlots
{
	class Slot_weaponMuzzleKODVSK94
	{
		name="weaponMuzzleKODVSK94";
		displayName="VSK-94 Silencer";
		ghostIcon="supressor";
	};
	class Slot_WeaponHandguardKODVSK94
	{
		name="WeaponHandguardKODVSK94";
		displayName="VSK-94 Handguard";
		ghostIcon="handguard";
	};
	class Slot_weaponButtstockKODVSK94
	{
		name="weaponButtstockKODVSK94";
		displayName="VSK-94 Buttstock";
		ghostIcon="buttstock";
	};
};
