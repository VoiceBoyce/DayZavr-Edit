class CfgPatches
{
	class KOD_Weapons_SA58
	{
		units[]=
		{
			"KOD_SA58_Bttstck"
		};
		weapons[]=
		{
			"KOD_SA58"
		};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Firearms",
			"DZ_Weapons_Magazines",
			"DZ_Weapons_Firearms_FAL"
		};
		magazines[]=
		{
			"Mag_KOD_SA58_20Rnd"
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
	class KOD_SA58_Base: Rifle_Base
	{
		scope=0;
		displayName="DSA SA58";
		descriptionShort="$STR_KOD_SA58";
		model="KOD_weapons\weapons\SA58\sa58.p3d";
		attachments[]=
		{
			"weaponWrap",
			"weaponOptics",
			"suppressorImpro",
			"weaponButtstockFal",
			"weaponFlashlight",
			"KODRISBipod",
			"KODSticker1",
			"KODSticker2",
			"KODSticker3"
		};
		itemSize[]={8,3};
		weight=4000;
		absorbency=0;
		repairableWithKits[]={5,1};
		repairCosts[]={30,25};
		PPDOFProperties[]={1,0.5,20,170,4,10};
		WeaponLength=0.82906699;
		barrelArmor=3.2;
		initSpeedMultiplier=0.87;
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
			"Mag_FAL_20Rnd",
			"Mag_KOD_Drum308_50rnd",
			"Mag_KOD_SA58_20Rnd"
		};
		ejectType=1;
		recoilModifier[]={0.94999999,0.94999999,0.94999999};
		swayModifier[]={1.7,1.7,1.1};
		simpleHiddenSelections[]=
		{
			"hide_barrel"
		};
		hiddenSelections[]=
		{
			"color",
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
				"FAL_Shot_1st_SoundSet",
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
			reloadTime=0.115;
			dispersion=0.00089999998;
			magazineSlot="magazine";
		};
		class FullAuto: Mode_FullAuto
		{
			soundSetShot1st[]=
			{
				"FAL_Shot_1st_SoundSet",
				"FAL_Shot_1st_iterior_SoundSet"
			};
			soundSetShot[]=
			{
				"FAL_Shot_SoundSet",
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
			reloadTime=0.079999998;
			dispersion=0.00089999998;
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
				maxOverheatingValue=40;
				shotsToStartOverheating=5;
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
								"KOD_weapons\weapons\SA58\data\barrel.rvmat",
								"KOD_weapons\weapons\SA58\data\handguard.rvmat",
								"KOD_weapons\weapons\SA58\data\mount.rvmat",
								"KOD_weapons\weapons\SA58\data\pistolgrip.rvmat",
								"KOD_weapons\weapons\SA58\data\receiver.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\SA58\data\barrel.rvmat",
								"KOD_weapons\weapons\SA58\data\handguard.rvmat",
								"KOD_weapons\weapons\SA58\data\mount.rvmat",
								"KOD_weapons\weapons\SA58\data\pistolgrip.rvmat",
								"KOD_weapons\weapons\SA58\data\receiver.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\SA58\data\barrel_damage.rvmat",
								"KOD_weapons\weapons\SA58\data\handguard_damage.rvmat",
								"KOD_weapons\weapons\SA58\data\mount_damage.rvmat",
								"KOD_weapons\weapons\SA58\data\pistolgrip_damage.rvmat",
								"KOD_weapons\weapons\SA58\data\receiver_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\SA58\data\barrel_damage.rvmat",
								"KOD_weapons\weapons\SA58\data\handguard_damage.rvmat",
								"KOD_weapons\weapons\SA58\data\mount_damage.rvmat",
								"KOD_weapons\weapons\SA58\data\pistolgrip_damage.rvmat",
								"KOD_weapons\weapons\SA58\data\receiver_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\SA58\data\barrel_destruct.rvmat",
								"KOD_weapons\weapons\SA58\data\handguard_destruct.rvmat",
								"KOD_weapons\weapons\SA58\data\mount_destruct.rvmat",
								"KOD_weapons\weapons\SA58\data\pistolgrip_destruct.rvmat",
								"KOD_weapons\weapons\SA58\data\receiver_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_SA58: KOD_SA58_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\SA58\data\handguard_co.paa",
			"KOD_weapons\weapons\SA58\data\mount_co.paa",
			"KOD_weapons\weapons\SA58\data\receiver_co.paa",
			"KOD_weapons\weapons\SA58\data\pistolgrip_co.paa"
		};
	};
};
class cfgMagazines
{
	class Magazine_Base;
	class Mag_KOD_SA58_20Rnd: Magazine_Base
	{
		scope=2;
		displayName="20rd SA58 Mag";
		descriptionShort="Detachable box magazine for SA58 assault rifle. Holds up to 20 rounds of .308 cal.";
		model="KOD_weapons\weapons\SA58\sa58_mag.p3d";
		weight=100;
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
								"\KOD_weapons\weapons\SA58\data\mag.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"\KOD_weapons\weapons\SA58\data\mag.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"\KOD_weapons\weapons\SA58\data\mag_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"\KOD_weapons\weapons\SA58\data\mag_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"\KOD_weapons\weapons\SA58\data\mag_destruct.rvmat"
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
	class Inventory_Base;
	class KOD_SA58_Bttstck: Inventory_Base
	{
		scope=2;
		displayName="SA58 Folding Buttstock";
		descriptionShort="A polymer buttstock for the SA58 battle rifle. Interchangable with FAL buttstocks.";
		model="KOD_weapons\weapons\SA58\sa58_stock.p3d";
		rotationFlags=17;
		reversed=0;
		inventorySlot[]=
		{
			"weaponButtstockFal"
		};
		weight=400;
		itemSize[]={3,1};
		recoilModifier[]={0.75,0.75,0.75};
		dispersionModifier=-0.00030000001;
		dispersionCondition="true";
		hiddenSelections[]=
		{
			"color"
		};
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\SA58\data\stock_co.paa"
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
								"KOD_weapons\weapons\SA58\data\stock.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\SA58\data\stock.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\SA58\data\stock_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\SA58\data\stock_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\SA58\data\stock_destruct.rvmat"
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
	class ProxySA58_stock: ProxyAttachment
	{
		scope=0;
		inventorySlot="weaponButtstockFal";
		model="KOD_weapons\weapons\SA58\sa58_stock.p3d";
	};
};
