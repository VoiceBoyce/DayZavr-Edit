class CfgPatches
{
	class KOD_Weapons_HK21
	{
		units[]={};
		weapons[]=
		{
			"KOD_HK21_Black",
			"KOD_HK21_Old"
		};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Firearms"
		};
		magazines[]=
		{
			"Mag_KOD_HK21_50rnd"
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
	class KOD_HK21_Base: Rifle_Base
	{
		scope=0;
		displayName="HK21";
		descriptionShort="$STR_KOD_HK21";
		model="KOD_weapons\weapons\HK21\hk21.p3d";
		attachments[]=
		{
			"weaponWrap",
			"weaponOptics",
			"suppressorImpro",
			"KODRISBipod",
			"KODSticker1",
			"KODSticker2",
			"KODSticker3"
		};
		itemSize[]={9,3};
		weight=4600;
		absorbency=0;
		winchesterTypeOpticsMount=1;
		repairableWithKits[]={1};
		repairCosts[]={30,25};
		PPDOFProperties[]={1,0.5,20,120,4,10};
		WeaponLength=0.92906702;
		barrelArmor=3.5;
		chamberSize=1;
		chamberedRound="";
		chamberableFrom[]=
		{
			"KOD_Ammo_308WinAP",
			"Ammo_308Win",
			"Ammo_308WinTracer"
		};
		magazines[]=
		{
			"Mag_KOD_HK21_50rnd"
		};
		ejectType=1;
		initSpeedMultiplier=0.80000001;
		recoilModifier[]={0.69999999,0.69999999,0.69999999};
		swayModifier[]={2.2,2.2,1.1};
		simpleHiddenSelections[]=
		{
			"hide_barrel"
		};
		hiddenSelections[]=
		{
			"color1",
			"color2",
			"color3",
			"color4",
			"color5"
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
				"DZVR_KOD_HK21_Shot_SoundSet",
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
			reloadTime=0.1;
			dispersion=0.001;
			magazineSlot="magazine";
		};
		class FullAuto: Mode_FullAuto
		{
			soundSetShot1st[]=
			{
				"DZVR_KOD_HK21_Shot_SoundSet",
				"FAL_Shot_1st_iterior_SoundSet"
			};
			soundSetShot[]=
			{
				"DZVR_KOD_HK21_Shot_SoundSet",
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
			reloadTime=0.067000002;
			dispersion=0.001;
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
				maxOverheatingValue=15;
				shotsToStartOverheating=5;
				overheatingDecayInterval=1;
				class SmokingBarrel1
				{
					positionOffset[]={0.2,0,0};
					overrideParticle="smoking_barrel_small";
					onlyWithinOverheatLimits[]={0,0.2};
					onlyWithinRainLimits[]={0,0.2};
				};
				class SmokingBarrelHot1
				{
					positionOffset[]={0.2,0,0};
					overrideParticle="smoking_barrel";
					onlyWithinOverheatLimits[]={0.2,0.60000002};
					onlyWithinRainLimits[]={0,0.2};
				};
				class SmokingBarrelHot3
				{
					positionOffset[]={0.2,0,0};
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
	};
	class KOD_HK21_Black: KOD_HK21_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\HK21\data\hk21_black_co.paa",
			"KOD_weapons\weapons\HK21\data\compensator_co.paa",
			"KOD_weapons\weapons\HK21\data\sights_co.paa",
			"KOD_weapons\weapons\HK21\data\rail_co.paa",
			"KOD_weapons\weapons\HK21\data\verticalgrip_co.paa"
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
								"KOD_weapons\weapons\HK21\data\hk21_black.rvmat",
								"KOD_weapons\weapons\HK21\data\compensator.rvmat",
								"KOD_weapons\weapons\HK21\data\sights.rvmat",
								"KOD_weapons\weapons\HK21\data\rail.rvmat",
								"KOD_weapons\weapons\HK21\data\verticalgrip.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\HK21\data\hk21_black.rvmat",
								"KOD_weapons\weapons\HK21\data\compensator.rvmat",
								"KOD_weapons\weapons\HK21\data\sights.rvmat",
								"KOD_weapons\weapons\HK21\data\rail.rvmat",
								"KOD_weapons\weapons\HK21\data\verticalgrip.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\HK21\data\hk21_black_damage.rvmat",
								"KOD_weapons\weapons\HK21\data\compensator_damage.rvmat",
								"KOD_weapons\weapons\HK21\data\sights_damage.rvmat",
								"KOD_weapons\weapons\HK21\data\rail_damage.rvmat",
								"KOD_weapons\weapons\HK21\data\verticalgrip_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\HK21\data\hk21_black_damage.rvmat",
								"KOD_weapons\weapons\HK21\data\compensator_damage.rvmat",
								"KOD_weapons\weapons\HK21\data\sights_damage.rvmat",
								"KOD_weapons\weapons\HK21\data\rail_damage.rvmat",
								"KOD_weapons\weapons\HK21\data\verticalgrip_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\HK21\data\hk21_black_destruct.rvmat",
								"KOD_weapons\weapons\HK21\data\compensator_destruct.rvmat",
								"KOD_weapons\weapons\HK21\data\sights_destruct.rvmat",
								"KOD_weapons\weapons\HK21\data\rail_destruct.rvmat",
								"KOD_weapons\weapons\HK21\data\verticalgrip_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_HK21_Old: KOD_HK21_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\HK21\data\hk21_co.paa",
			"KOD_weapons\weapons\HK21\data\compensator_co.paa",
			"KOD_weapons\weapons\HK21\data\sights_co.paa",
			"KOD_weapons\weapons\HK21\data\rail_co.paa",
			"KOD_weapons\weapons\HK21\data\verticalgrip_co.paa"
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
								"KOD_weapons\weapons\HK21\data\hk21.rvmat",
								"KOD_weapons\weapons\HK21\data\compensator.rvmat",
								"KOD_weapons\weapons\HK21\data\sights.rvmat",
								"KOD_weapons\weapons\HK21\data\rail.rvmat",
								"KOD_weapons\weapons\HK21\data\verticalgrip.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\HK21\data\hk21.rvmat",
								"KOD_weapons\weapons\HK21\data\compensator.rvmat",
								"KOD_weapons\weapons\HK21\data\sights.rvmat",
								"KOD_weapons\weapons\HK21\data\rail.rvmat",
								"KOD_weapons\weapons\HK21\data\verticalgrip.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\HK21\data\hk21_damage.rvmat",
								"KOD_weapons\weapons\HK21\data\compensator_damage.rvmat",
								"KOD_weapons\weapons\HK21\data\sights_damage.rvmat",
								"KOD_weapons\weapons\HK21\data\rail_damage.rvmat",
								"KOD_weapons\weapons\HK21\data\verticalgrip_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\HK21\data\hk21_damage.rvmat",
								"KOD_weapons\weapons\HK21\data\compensator_damage.rvmat",
								"KOD_weapons\weapons\HK21\data\sights_damage.rvmat",
								"KOD_weapons\weapons\HK21\data\rail_damage.rvmat",
								"KOD_weapons\weapons\HK21\data\verticalgrip_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\HK21\data\hk21_destruct.rvmat",
								"KOD_weapons\weapons\HK21\data\compensator_destruct.rvmat",
								"KOD_weapons\weapons\HK21\data\sights_destruct.rvmat",
								"KOD_weapons\weapons\HK21\data\rail_destruct.rvmat",
								"KOD_weapons\weapons\HK21\data\verticalgrip_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
};
class cfgMagazines
{
	class Magazine_Base;
	class Mag_KOD_HK21_50rnd: Magazine_Base
	{
		scope=2;
		displayName="50rd HK21 Drum Magazine";
		descriptionShort="Detachable box magazine for the HK21 machine gun. Holds up to 50 rounds of .308 Winchester.";
		model="KOD_weapons\weapons\HK21\hk21_mag.p3d";
		weight=300;
		itemSize[]={2,3};
		count=50;
		ammo="Bullet_308Win";
		ammoItems[]=
		{
			"KOD_Ammo_308WinAP",
			"Ammo_308Win",
			"Ammo_308WinTracer"
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
								"KOD_weapons\weapons\HK21\data\mag.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\HK21\data\mag.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\HK21\data\mag_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\HK21\data\mag_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\HK21\data\mag_destruct.rvmat"
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
