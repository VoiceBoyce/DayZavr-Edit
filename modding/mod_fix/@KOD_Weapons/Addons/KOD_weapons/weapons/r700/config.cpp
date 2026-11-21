class CfgPatches
{
	class KOD_Weapons_R700
	{
		units[]=
		{
			"KOD_Suppressor_308"
		};
		weapons[]=
		{
			"KOD_R700"
		};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Firearms",
			"DZ_Weapons_Magazines",
			"DZ_Weapons_Muzzles",
			"DZ_Sounds_Effects"
		};
		magazines[]=
		{
			"Mag_KOD_R700_7rnd"
		};
		ammo[]={};
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
	class KOD_R700_Base: BoltActionRifle_Base
	{
		scope=0;
		displayName="R700";
		descriptionShort="$STR_KOD_R700";
		model="KOD_weapons\weapons\R700\R700.p3d";
		attachments[]=
		{
			"weaponWrap",
			"weaponOpticsHunting",
			"suppressorImpro",
			"KODSticker1",
			"KODSticker2",
			"KODSticker3"
		};
		weight=8391;
		absorbency=0;
		repairableWithKits[]={1};
		repairCosts[]={30,50};
		PPDOFProperties[]={1,0.5,50,180,4,10};
		WeaponLength=1.18986;
		chamberedRound="";
		chamberSize=1;
		chamberableFrom[]=
		{
			"Ammo_308Win",
			"Ammo_308WinTracer",
			"KOD_Ammo_308WinAP"
		};
		magazines[]=
		{
			"Mag_KOD_R700_7rnd"
		};
		initSpeedMultiplier=1.15;
		ejectType=1;
		itemSize[]={10,3};
		barrelArmor=4;
		recoilModifier[]={2.7,1.9,1.9};
		swayModifier[]={2.25,2.25,1.25};
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
				"R700Shot_SoundSet",
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
			reloadTime=1;
			dispersion=0.00014;
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
					illuminateWorld=1;
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
								"KOD_weapons\weapons\R700\data\action.rvmat",
								"KOD_weapons\weapons\R700\data\barrel22.rvmat",
								"KOD_weapons\weapons\R700\data\chassis_mcm.rvmat",
								"KOD_weapons\weapons\R700\data\efr.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\R700\data\action.rvmat",
								"KOD_weapons\weapons\R700\data\barrel22.rvmat",
								"KOD_weapons\weapons\R700\data\chassis_mcm.rvmat",
								"KOD_weapons\weapons\R700\data\efr.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\R700\data\action_damage.rvmat",
								"KOD_weapons\weapons\R700\data\barrel22_damage.rvmat",
								"KOD_weapons\weapons\R700\data\chassis_mcm_damage.rvmat",
								"KOD_weapons\weapons\R700\data\efr_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\R700\data\action_damage.rvmat",
								"KOD_weapons\weapons\R700\data\barrel22_damage.rvmat",
								"KOD_weapons\weapons\R700\data\chassis_mcm_damage.rvmat",
								"KOD_weapons\weapons\R700\data\efr_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\R700\data\action_destruct.rvmat",
								"KOD_weapons\weapons\R700\data\barrel22_destruct.rvmat",
								"KOD_weapons\weapons\R700\data\chassis_mcm_destruct.rvmat",
								"KOD_weapons\weapons\R700\data\efr_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_R700: KOD_R700_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\R700\data\action_co.paa",
			"KOD_weapons\weapons\R700\data\barrel22_co.paa",
			"KOD_weapons\weapons\R700\data\chassis_mcm_co.paa",
			"KOD_weapons\weapons\R700\data\efr_co.paa"
		};
	};
};
class cfgMagazines
{
	class Magazine_Base;
	class Mag_KOD_R700_7rnd: Magazine_Base
	{
		scope=2;
		displayName="Маг. R700 на 7 пат.";
		descriptionShort="$STR_Mag_KOD_R700_7Rnd";
		model="KOD_weapons\weapons\R700\magazine.p3d";
		weight=100;
		itemSize[]={1,2};
		count=7;
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
					hitpoints=200;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\R700\data\magazine.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\R700\data\magazine.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\R700\data\magazine_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\R700\data\magazine_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\R700\data\magazine_destruct.rvmat"
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
	class KOD_Suppressor_308: ItemSuppressor
	{
		scope=2;
		displayName="Винтовочный Глушитель";
		descriptionShort="$STR_KOD_Suppressor_308.";
		model="KOD_weapons\weapons\R700\suppressorr700.p3d";
		rotationFlags=17;
		reversed=0;
		weight=400;
		itemSize[]={3,1};
		itemModelLength=0.25;
		inventorySlot[]=
		{
			"suppressorImpro",
			"weaponMuzzleM4",
			"weaponMuzzleAK"
		};
		simulation="ItemSuppressor";
		dispersionModifier=-9.9999997e-05;
		noiseShootModifier=-0.94999999;
		dispersionCondition="true";
		barrelArmor=3600;
		recoilModifier[]={0.94999999,0.94999999,0.94999999};
		swayModifier[]={1.05,1.05,1.05};
		soundIndex=1;
		muzzlePos="usti hlavne";
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
					hitpoints=4;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\R700\data\suppressor.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\R700\data\suppressor.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\R700\data\suppressor_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\R700\data\suppressor_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\R700\data\suppressor_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
};
class CfgSoundShaders
{
	class base_closeShot_SoundShader;
	class base_midShot_SoundShader;
	class base_distShot_SoundShader;
	class M14_silencerHomeMadeCloseShot_SoundShader;	
	class M14_silencerHomeMadeMidShot_SoundShader;	
	class M14_silencerHomeMadeDistShot_SoundShader;	
	
	class R700closeShot_SoundShader: base_closeShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\sounds\r700_single_1",
				1
			}
		};
		volume=1;
	};
	class R700midShot_SoundShader: base_midShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\sounds\r700_single_1",
				1
			}
		};
		volume=0.80000001;
	};
	class R700distShot_SoundShader: base_distShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\sounds\r700_single_1",
				1
			}
		};
		volume=0.44999999;
	};
	
	
	
	class DZVR_Rifle_silencerCloseShot_SoundShader : M14_silencerHomeMadeCloseShot_SoundShader 
	{
		samples[] = {{"KOD_weapons\weapons\sounds\shoot_sil_rifle", 1}};
		volume = 0.5;
		range=25;
	};
	
	class DZVR_Rifle_silencerMidShot_SoundShader : M14_silencerHomeMadeMidShot_SoundShader 
	{
		samples[] = {{"KOD_weapons\weapons\sounds\shoot_sil_rifle", 1}};
		volume = 0.4;
		range=90;
		rangeCurve[] = {{0,0.1},{15,1},{25,0},{50,0}};
	};
	
	class DZVR_Rifle_silencerDistShot_SoundShader : M14_silencerHomeMadeDistShot_SoundShader 
	{
		samples[] = {{"KOD_weapons\weapons\sounds\shoot_sil_rifle", 0.1}};
		volume = 0.0;
		range=150;
	};
};
class CfgSoundSets
{
	class Rifle_Shot_Base_SoundSet;
	class M14_silencerHomeMade_SoundSet;
	class R700Shot_SoundSet: Rifle_Shot_Base_SoundSet
	{
		soundShaders[]=
		{
			"R700closeShot_SoundShader",
			"R700midShot_SoundShader",
			"R700distShot_SoundShader"
		};
	};
	class DZVR_Rifle_silencer_SoundSet: M14_silencerHomeMade_SoundSet
	{
		soundShaders[]=
		{
			"DZVR_Rifle_silencerCloseShot_SoundShader",
			"DZVR_Rifle_silencerMidShot_SoundShader",
			"DZVR_Rifle_silencerDistShot_SoundShader"
		};
	};
};


