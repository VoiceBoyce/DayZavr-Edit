class CfgPatches
{
	class KOD_MDR
	{
		units[]={};
		weapons[]=
		{
			"KOD_MDR_Black",
			"KOD_MDR_Tan"
		};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Firearms",
			"DZ_Sounds_Effects"
		};
		magazines[]=
		{
			"Mag_KOD_MDR_30rnd"
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
	class KOD_MDR_Base: Rifle_Base
	{
		scope=0;
		displayName="MDR";
		descriptionShort="Desert Tech Micro Dynamic Rifle, более известная как просто MDR - это модульное мультикалиберное автоматическое оружие, разработанное американской компанией Desert Tech в 2014 году. Данный вариант представляет из себя модификацию под .308 патрон, с питанием из собственных маазинов на 30 патронов, либо из барабанов на 50 патронов .308.";
		weight=3400;
		model="KOD_weapons\weapons\MDR\mdr.p3d";
		absorbency=0;
		repairableWithKits[]={1};
		repairCosts[]={30,25};
		PPDOFProperties[]={1,0.60000002,50,100,4,10};
		WeaponLength=1;
		barrelArmor=5.5;
		attachments[]=
		{
			"weaponWrap",
			"weaponOptics",
			"weaponFlashlight",
			"weaponMuzzleM4",
			"KODRISBipod",
			"KODSticker1",
			"KODSticker2",
			"KODSticker3"
		};
		itemSize[]={7,3};
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
			"Mag_KOD_MDR_30rnd",
			"Mag_KOD_Drum308_50rnd"
		};
		ejectType=1;
		recoilModifier[]={1.1,1.1,1.1};
		swayModifier[]={1.1,1.1,1};
		simpleHiddenSelections[]=
		{
			"hide_barrel",
			"hide_ironsights"
		};
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
				"MDR_Shot_SoundSet",
				"AUG_Shot_1st_iterior_SoundSet",
				"AUG_Tail_SoundSet",
				"AUG_InteriorTail_SoundSet",
				"AUG_Slapback_SoundSet",
				"AUG_Tail_2D_SoundSet"
			};
			soundSetShotExt[]=
			{
				
				{
					"AUG_1st_silencer_SoundSet",
					"AUG_silencerTail_SoundSet",
					"AUG_silencerInteriorTail_SoundSet"
				},
				
				{
					"AUG_1st_silencerHomeMade_SoundSet",
					"AUG_silencerHomeMadeTail_SoundSet",
					"AUG_silencerInteriorHomeMadeTail_SoundSet"
				}
			};
			reloadTime=0.1;
			dispersion=0.0011;
			magazineSlot="magazine";
		};
		class FullAuto: Mode_FullAuto
		{
			soundSetShot1st[]=
			{
				"MDR_Shot_SoundSet",
				"AUG_Shot_1st_iterior_SoundSet"
			};
			soundSetShot[]=
			{
				"MDR_Shot_SoundSet",
				"AUG_Shot_iterior_SoundSet",
				"AUG_Tail_SoundSet",
				"AUG_InteriorTail_SoundSet",
				"AUG_Slapback_SoundSet",
				"AUG_Tail_2D_SoundSet"
			};
			soundSetShotExt1st[]=
			{
				
				{
					"AUG_1st_silencer_SoundSet"
				},
				
				{
					"AUG_1st_silencerHomeMade_SoundSet"
				}
			};
			soundSetShotExt[]=
			{
				
				{
					"AUG_silencer_SoundSet",
					"AUG_silencerTail_SoundSet",
					"AUG_silencerInteriorTail_SoundSet"
				},
				
				{
					"AUG_silencerHomeMade_SoundSet",
					"AUG_silencerHomeMadeTail_SoundSet",
					"AUG_silencerInteriorHomeMadeTail_SoundSet"
				}
			};
			reloadTime=0.075000003;
			dispersion=0.0011;
			magazineSlot="magazine";
		};
		class OpticsInfo: OpticsInfoRifle
		{
			memoryPointCamera="eye";
			discreteDistance[]={300};
			discreteDistanceInitIndex=0;
			modelOptics="-";
			distanceZoomMin=300;
			distanceZoomMax=300;
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
					overrideParticle="weapon_shot_ak74_01";
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
				maxOverheatingValue=60;
				shotsToStartOverheating=20;
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
								"KOD_weapons\weapons\MDR\data\mdr.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\MDR\data\mdr.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\MDR\data\mdr_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\MDR\data\mdr_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\MDR\data\mdr_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_MDR_Black: KOD_MDR_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\MDR\data\mdr_black_co.paa"
		};
	};
	class KOD_MDR_Tan: KOD_MDR_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\MDR\data\mdr_tan_co.paa"
		};
	};
};
class cfgMagazines
{
	class Magazine_Base;
	class Mag_KOD_MDR_30rnd: Magazine_Base
	{
		scope=2;
		displayName="30rd MDR Mag";
		descriptionShort="Съёмный магазин для винтовки MDR. Снаряжается .308 патронами.";
		model="KOD_weapons\weapons\MDR\mdr_mag.p3d";
		weight=120;
		itemSize[]={1,3};
		count=30;
		ammo="Bullet_308Win";
		ammoItems[]=
		{
			"Ammo_308Win",
			"Ammo_308WinTracer",
			"KOD_Ammo_308WinAP"
		};
		hiddenSelections[]=
		{
			"magazine"
		};
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\MDR\data\mdr_tan_co.paa"
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
								"KOD_weapons\weapons\MDR\data\mdr.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\MDR\data\mdr.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\MDR\data\mdr_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\MDR\data\mdr_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\MDR\data\mdr_destruct.rvmat"
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
	class Proxymdr_mag: ProxyAttachment
	{
		scope=0;
		inventorySlot="magazine";
		model="KOD_weapons\weapons\MDR\mdr_mag.p3d";
	};
};
class CfgSoundShaders
{
	class base_closeShot_SoundShader;
	class base_midShot_SoundShader;
	class base_distShot_SoundShader;
	class MDR_closeShot_SoundShader: base_closeShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\MDR\sound\MDR_close",
				1
			}
		};
		volume=1;
	};
	class MDR_midShot_SoundShader: base_midShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\MDR\sound\MDR_mid",
				1
			}
		};
		volume=0.60000002;
	};
	class MDR_distShot_SoundShader: base_distShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\MDR\sound\MDR_far",
				1
			}
		};
		volume=0.40000001;
	};
};
class CfgSoundSets
{
	class Rifle_Shot_Base_SoundSet;
	class MDR_Shot_SoundSet: Rifle_Shot_Base_SoundSet
	{
		soundShaders[]=
		{
			"MDR_closeShot_SoundShader",
			"MDR_midShot_SoundShader",
			"MDR_distShot_SoundShader"
		};
	};
};
