class CfgPatches
{
	class KOD_Weapons_Skorpion
	{
		units[]={};
		weapons[]=
		{
			"KOD_Scorpion"
		};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Firearms",
			"DZ_Weapons_Magazines"
		};
		magazines[]=
		{
			"Mag_KOD_Scorpion_30rnd"
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
	class KOD_Scorpion_Base: Rifle_Base
	{
		scope=0;
		displayName="CZ Scorpion";
		descriptionShort="A 9x19 submachine gun designed in Slovakia under the name Laugo, produced in the Czech Republic by the Česká Zbrojovka Uherský Brod.";
		model="KOD_weapons\weapons\Scorpion\scorp.p3d";
		attachments[]=
		{
			"weaponWrap",
			"weaponOptics",
			"pistolMuzzle",
			"KODSticker1",
			"KODSticker2",
			"KODSticker3"
		};
		itemSize[]={7,3};
		weight=2400;
		absorbency=0;
		repairableWithKits[]={1};
		repairCosts[]={30,25};
		PPDOFProperties[]={};
		initSpeedMultiplier=1.0700001;
		WeaponLength=0.60000002;
		barrelArmor=1.5;
		chamberSize=1;
		chamberedRound="";
		chamberableFrom[]=
		{
			"Ammo_9x19"
		};
		magazines[]=
		{
			"Mag_KOD_Scorpion_30rnd"
		};
		ejectType=1;
		recoilModifier[]={0.60000002,0.60000002,0.80000001};
		swayModifier[]={1.15,1.15,1};
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
			"FullAuto",
			"SemiAuto"
		};
		class SemiAuto: Mode_SemiAuto
		{
			soundSetShot[]=
			{
				"KOD_919_Shot_SoundSet",
				"UMP45_Shot_1st_iterior_SoundSet",
				"UMP45_Tail_SoundSet",
				"UMP45_InteriorTail_SoundSet",
				"UMP45_Slapback_SoundSet",
				"UMP45_Tail_2D_SoundSet"
			};
			soundSetShotExt[]=
			{
				
				{
					"UMP45_1st_silencer_SoundSet",
					"UMP45_silencerTail_SoundSet",
					"UMP45_silencerInteriorTail_SoundSet"
				},
				
				{
					"UMP45_1st_silencerHomeMade_SoundSet",
					"UMP45_silencerHomeMadeTail_SoundSet",
					"UMP45_silencerInteriorHomeMadeTail_SoundSet"
				}
			};
			reloadTime=0.115;
			dispersion=0.0020000001;
			magazineSlot="magazine";
		};
		class FullAuto: Mode_FullAuto
		{
			soundSetShot1st[]=
			{
				"KOD_919_Shot_SoundSet",
				"UMP45_Shot_1st_iterior_SoundSet"
			};
			soundSetShot[]=
			{
				"KOD_919_Shot_SoundSet",
				"UMP45_Shot_iterior_SoundSet",
				"UMP45_Tail_SoundSet",
				"UMP45_InteriorTail_SoundSet",
				"UMP45_Slapback_SoundSet",
				"UMP45_Tail_2D_SoundSet"
			};
			soundSetShotExt1st[]=
			{
				
				{
					"UMP45_1st_silencer_SoundSet"
				},
				
				{
					"UMP45_1st_silencerHomeMade_SoundSet"
				}
			};
			soundSetShotExt[]=
			{
				
				{
					"UMP45_silencer_SoundSet",
					"UMP45_silencerTail_SoundSet",
					"UMP45_silencerInteriorTail_SoundSet"
				},
				
				{
					"UMP45_silencerHomeMade_SoundSet",
					"UMP45_silencerHomeMadeTail_SoundSet",
					"UMP45_silencerInteriorHomeMadeTail_SoundSet"
				}
			};
			reloadTime=0.052999999;
			dispersion=0.0020000001;
			magazineSlot="magazine";
		};
		class OpticsInfo: OpticsInfoRifle
		{
			memoryPointCamera="eye";
			discreteDistance[]={100,200,300};
			discreteDistanceInitIndex=0;
			modelOptics="-";
			distanceZoomMin=100;
			distanceZoomMax=300;
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
				shotsToStartOverheating=20;
				maxOverheatingValue=30;
				overheatingDecayInterval=1;
				class SmokingBarrel1
				{
					ignoreIfSuppressed=1;
					overrideParticle="smoking_barrel_small";
					onlyWithinOverheatLimits[]={0,0.30000001};
					onlyWithinRainLimits[]={0,0.2};
				};
				class SmokingBarrelHot1
				{
					ignoreIfSuppressed=1;
					overrideParticle="smoking_barrel";
					positionOffset[]={0.050000001,0,0};
					onlyWithinOverheatLimits[]={0.30000001,0.89999998};
					onlyWithinRainLimits[]={0,0.2};
				};
				class SmokingBarrelHot2
				{
					ignoreIfSuppressed=1;
					overrideParticle="smoking_barrel_heavy";
					positionOffset[]={0.050000001,0,0};
					onlyWithinOverheatLimits[]={0.89999998,1};
					onlyWithinRainLimits[]={0,0.2};
				};
				class SmokingBarrelHotSteam
				{
					overrideParticle="smoking_barrel_steam_small";
					positionOffset[]={0.60000002,0.050000001,0};
					onlyWithinOverheatLimits[]={0,0.5};
					onlyWithinRainLimits[]={0.2,1};
				};
				class SmokingBarrelHotSteam2
				{
					overrideParticle="smoking_barrel_steam";
					positionOffset[]={0.69999999,0.050000001,0};
					onlyWithinOverheatLimits[]={0.5,1};
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
					hitpoints=350;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\Scorpion\data\evo.rvmat",
								"KOD_weapons\weapons\Scorpion\data\evo2.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\Scorpion\data\evo.rvmat",
								"KOD_weapons\weapons\Scorpion\data\evo2.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\Scorpion\data\evo_damage.rvmat",
								"KOD_weapons\weapons\Scorpion\data\evo2_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\Scorpion\data\evo_damage.rvmat",
								"KOD_weapons\weapons\Scorpion\data\evo2_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\Scorpion\data\evo_destruct.rvmat",
								"KOD_weapons\weapons\Scorpion\data\evo2_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_Scorpion: KOD_Scorpion_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\Scorpion\data\evo_co.paa",
			"KOD_weapons\weapons\Scorpion\data\evo2_co.paa"
		};
	};
};
class cfgMagazines
{
	class Magazine_Base;
	class Mag_KOD_Scorpion_30rnd: Magazine_Base
	{
		scope=2;
		displayName="30rd CZ Scorpion Mag";
		descriptionShort="Detachable box magazine for the CZ Scorpion submachine gun. Holds up to 30 rounds of 9x19 mm.";
		model="KOD_weapons\weapons\Scorpion\scorp_mag.p3d";
		weight=70;
		itemSize[]={1,3};
		count=30;
		ammo="Bullet_9x19";
		ammoItems[]=
		{
			"Ammo_9x19"
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
					hitpoints=70;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\Scorpion\data\evo2.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\Scorpion\data\evo2.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\Scorpion\data\evo2_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\Scorpion\data\evo2_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\Scorpion\data\evo2_destruct.rvmat"
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


class CfgSoundShaders {
	class base_closeShot_SoundShader;	// External class reference
	class base_midShot_SoundShader;	// External class reference
	class base_distShot_SoundShader;	// External class reference
	
	class KOD_919_closeShot_SoundShader : base_closeShot_SoundShader {
		samples[] = {{"KOD_weapons\weapons\sounds\shoot_919", 1}};
		volume = 1.0;
	};
	
	class KOD_919_midShot_SoundShader : base_midShot_SoundShader {
		samples[] = {{"KOD_weapons\weapons\sounds\shoot_919", 1}};
		volume = 0.7;
	};
	
	class KOD_919_distShot_SoundShader : base_distShot_SoundShader {
		samples[] = {{"KOD_weapons\weapons\sounds\shoot_919", 1}};
		volume = 0.2;
	};
};

class CfgSoundSets {
	class Rifle_Shot_Base_SoundSet;	// External class reference
	
	class KOD_919_Shot_SoundSet : Rifle_Shot_Base_SoundSet {
		soundShaders[] = {"KOD_919_closeShot_SoundShader", "KOD_919_midShot_SoundShader", "KOD_919_distShot_SoundShader"};
	};
};
