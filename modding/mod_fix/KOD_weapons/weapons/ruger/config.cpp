class CfgPatches
{
	class KOD_Weapons_Ruger
	{
		units[]={};
		weapons[]=
		{
			"KOD_Ruger"
		};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Firearms",
			"DZ_Weapons_Magazines",
			"DZ_Sounds_Effects"
		};
		magazines[]=
		{
			"Mag_KOD_Ruger_5rnd"
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
	class KOD_Ruger_Base: BoltActionRifle_Base
	{
		scope=0;
		displayName="Ruger Precision";
		descriptionShort="$STR_KOD_Ruger";
		model="KOD_weapons\weapons\Ruger\ruger.p3d";
		attachments[]=
		{
			"weaponWrap",
			"weaponOpticsHunting",
			"KODSticker1",
			"KODSticker2",
			"KODSticker3"
		};
		itemSize[]={10,3};
		weight=4400;
		absorbency=0;
		repairableWithKits[]={1};
		repairCosts[]={35};
		PPDOFProperties[]={};
		opticsFlare=0;
		WeaponLength=1.9;
		chamberSize=1;
		chamberedRound="";
		chamberableFrom[]=
		{
			"KOD_Ammo_338"
		};
		magazines[]=
		{
			"Mag_KOD_Ruger_5rnd"
		};
		barrelArmor=4.75;
		ejectType=0;
		recoilModifier[]={3,1.5,1.2};
		swayModifier[]={2.5,2.5,1};
		simpleHiddenSelections[]=
		{
			"hide_barrel"
		};
		hiddenSelections[]=
		{
			"color"
		};
		modes[]=
		{
			"Single"
		};
		class Single: Mode_SemiAuto
		{
			soundSetShot[]=
			{
				"RugerShot_SoundSet",
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
					hitpoints=250;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\Ruger\data\ruger.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\Ruger\data\ruger.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\Ruger\data\ruger_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\Ruger\data\ruger_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\Ruger\data\ruger_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_Ruger: KOD_Ruger_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\Ruger\data\ruger_co.paa"
		};
	};
};
class cfgMagazines
{
	class Magazine_Base;
	class Mag_KOD_Ruger_5rnd: Magazine_Base
	{
		scope=2;
		displayName="Маг. Ruger Precision на 5 пат.";
		descriptionShort="$STR_Mag_KOD_Ruger_5Rnd";
		model="KOD_weapons\weapons\Ruger\ruger_mag.p3d";
		weight=150;
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
					hitpoints=250;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\Ruger\data\ruger.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\Ruger\data\ruger.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\Ruger\data\ruger_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\Ruger\data\ruger_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\Ruger\data\ruger_destruct.rvmat"
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
class CfgSoundShaders
{
	class base_closeShot_SoundShader;
	class base_midShot_SoundShader;
	class base_distShot_SoundShader;
	class RugercloseShot_SoundShader: base_closeShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\sounds\ruger_single_1",
				1
			}
		};
		volume=1;
	};
	class RugermidShot_SoundShader: base_midShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\sounds\ruger_single_1",
				1
			}
		};
		volume=0.80000001;
	};
	class RugerdistShot_SoundShader: base_distShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\sounds\ruger_single_1",
				1
			}
		};
		volume=0.44999999;
	};
};
class CfgSoundSets
{
	class Rifle_Shot_Base_SoundSet;
	class RugerShot_SoundSet: Rifle_Shot_Base_SoundSet
	{
		soundShaders[]=
		{
			"RugercloseShot_SoundShader",
			"RugermidShot_SoundShader",
			"RugerdistShot_SoundShader"
		};
	};
};
