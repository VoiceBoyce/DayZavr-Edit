class CfgPatches
{
	class KOD_Weapons_AK458
	{
		units[]={};
		weapons[]=
		{
			"KOD_AK458_Plastic",
			"KOD_AK458_Wood"
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
			"Mag_KOD_AK458_20rnd"
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
	class KOD_AK458_Base: Rifle_Base
	{
		scope=0;
		displayName="AK-458 SOCOM";
		descriptionShort="Hunting carabine based on AK platform. Fed from detachable Magazines, chambered for .458 SOCOM";
		absorbency=0;
		repairableWithKits[]={1};
		repairCosts[]={30,25};
		PPDOFProperties[]={1,0.5,20,170,4,10};
		WeaponLength=0.72906703;
		barrelArmor=9.8879995;
		chamberSize=1;
		chamberedRound="";
		chamberableFrom[]=
		{
			"KOD_Ammo_458"
		};
		magazines[]=
		{
			"Mag_KOD_AK458_20rnd"
		};
		ejectType=1;
		initSpeedMultiplier=1;
		simpleHiddenSelections[]=
		{
			"hide_barrel"
		};
		hiddenSelections[]=
		{
			"color1",
			"color2"
		};
		modes[]=
		{
			"SemiAuto"
		};
		class SemiAuto: Mode_SemiAuto
		{
			soundSetShot[]=
			{
				"SOCOM458Shot_SoundSet",
				"AK_Tail_SoundSet",
				"AK_InteriorTail_SoundSet"
			};
			soundSetShotExt[]=
			{
				
				{
					"SVD_silencerHomeMade_SoundSet",
					"SVD_silencerHomeMadeTail_SoundSet",
					"SVD_silencerInteriorHomeMadeTail_SoundSet"
				}
			};
			reloadTime=0.13;
			dispersion=0.00079999998;
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
				maxOverheatingValue=25;
				shotsToStartOverheating=10;
				overheatingDecayInterval=1;
				class SmokingBarrel1
				{
					overrideParticle="smoking_barrel_small";
					onlyWithinOverheatLimits[]={0,0.5};
					positionOffset[]={0.1,0,0};
					onlyWithinRainLimits[]={0,0.5};
				};
				class SmokingBarrelHot1
				{
					overrideParticle="smoking_barrel";
					onlyWithinOverheatLimits[]={0.5,0.80000001};
					positionOffset[]={0.1,0,0};
					onlyWithinRainLimits[]={0,0.5};
				};
				class SmokingBarrelHotSteam
				{
					overrideParticle="smoking_barrel_steam";
					positionOffset[]={0.30000001,0,0};
					onlyWithinRainLimits[]={0.5,1};
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
								"KOD_weapons\weapons\Socom458\data\main.rvmat",
								"KOD_weapons\weapons\Socom458\data\second.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\Socom458\data\main.rvmat",
								"KOD_weapons\weapons\Socom458\data\second.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\Socom458\data\main_damage.rvmat",
								"KOD_weapons\weapons\Socom458\data\second_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\Socom458\data\main_damage.rvmat",
								"KOD_weapons\weapons\Socom458\data\second_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\Socom458\data\main_destruct.rvmat",
								"KOD_weapons\weapons\Socom458\data\second_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_AK458_Plastic: KOD_AK458_Base
	{
		scope=2;
		model="KOD_weapons\weapons\Socom458\socom_plastic.p3d";
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
		itemSize[]={7,3};
		weight=3500;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\Socom458\data\main_co.paa",
			"KOD_weapons\weapons\Socom458\data\second_co.paa"
		};
		recoilModifier[]={0.75,0.75,0.75};
		swayModifier[]={0.89999998,0.89999998,0.89999998};
	};
	class KOD_AK458_Wood: KOD_AK458_Base
	{
		scope=2;
		model="KOD_weapons\weapons\Socom458\socom_wood.p3d";
		attachments[]=
		{
			"weaponWrap",
			"weaponOptics",
			"suppressorImpro",
			"KODSticker1",
			"KODSticker2",
			"KODSticker3"
		};
		itemSize[]={8,3};
		weight=4000;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\Socom458\data\main_co.paa",
			"KOD_weapons\weapons\Socom458\data\second_co.paa"
		};
		recoilModifier[]={0.69999999,0.69999999,0.69999999};
		swayModifier[]={1,1,1};
	};
};
class cfgMagazines
{
	class Magazine_Base;
	class Mag_KOD_AK458_20rnd: Magazine_Base
	{
		scope=2;
		displayName="AK-458 SOCOM Magazine";
		descriptionShort="Detachable box magazine for AK-458 SOCOM rifle. Holds up to 20 rounds of .458 SOCOM";
		model="KOD_weapons\weapons\Socom458\socom_mag.p3d";
		weight=100;
		itemSize[]={1,2};
		count=20;
		ammo="KOD_Bullet_458";
		ammoItems[]=
		{
			"KOD_Ammo_458"
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
								"KOD_weapons\weapons\Socom458\data\second.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\Socom458\data\second.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\Socom458\data\second_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\Socom458\data\second_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\Socom458\data\second_destruct.rvmat"
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
	class SOCOM458closeShot_SoundShader: base_closeShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\Socom458\sound\socom458_close",
				1
			}
		};
		volume=0.80000001;
	};
	class SOCOM458midShot_SoundShader: base_midShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\Socom458\sound\socom458_mid",
				1
			}
		};
		volume=0.5;
	};
	class SOCOM458distShot_SoundShader: base_distShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\Socom458\sound\socom458_far",
				1
			}
		};
		volume=0.25;
	};
};
class CfgSoundSets
{
	class Rifle_Shot_Base_SoundSet;
	class SOCOM458Shot_SoundSet: Rifle_Shot_Base_SoundSet
	{
		soundShaders[]=
		{
			"SOCOM458closeShot_SoundShader",
			"SOCOM458midShot_SoundShader",
			"SOCOM458distShot_SoundShader"
		};
	};
};
