class CfgPatches
{
	class KOD_Weapons_M16A4
	{
		units[]={};
		weapons[]=
		{
			"KOD_M16A4"
		};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Firearms",
			"DZ_Sounds_Effects",
			"DZ_Data"
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
	class KOD_M16A4_Base: Rifle_Base
	{
		scope=0;
		displayName="M16A4";
		descriptionShort="$STR_KOD_M16A4";
		weight=3400;
		model="KOD_weapons\weapons\M16A4\m16a4.p3d";
		absorbency=0;
		repairableWithKits[]={1};
		repairCosts[]={30,50};
		PPDOFProperties[]={1,0.60000002,50,200,4,10};
		WeaponLength=1.1;
		initSpeedMultiplier=1.05;
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
		itemSize[]={9,3};
		chamberSize=1;
		barrelArmor=3.9000001;
		chamberedRound="";
		chamberableFrom[]=
		{
			"Ammo_556x45",
			"Ammo_556x45Tracer"
		};
		magazines[]=
		{
			"Mag_STANAG_30Rnd",
			"Mag_STANAG_60Rnd",
			"Mag_STANAGCoupled_30Rnd",
			"Mag_CMAG_10Rnd",
			"Mag_CMAG_20Rnd",
			"Mag_CMAG_30Rnd",
			"Mag_CMAG_40Rnd",
			"Mag_CMAG_100Rnd_Black",
			"Mag_CMAG_10Rnd_Green",
			"Mag_CMAG_20Rnd_Green",
			"Mag_CMAG_30Rnd_Green",
			"Mag_CMAG_40Rnd_Green",
			"Mag_CMAG_10Rnd_Black",
			"Mag_CMAG_20Rnd_Black",
			"Mag_CMAG_30Rnd_Black",
			"Mag_CMAG_40Rnd_Black"
		};
		ejectType=1;
		recoilModifier[]={0.63,0.63,0.63};
		swayModifier[]={1.9,1.9,1.05};
		hiddenSelections[]=
		{
			"color1",
			"color2",
			"color3",
			"color4"
		};
		simpleHiddenSelections[]=
		{
			"hide_barrel"
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
				"M16A2_Shot_SoundSet",
				"M16A2_Shot_iterior_SoundSet",
				"M16A2_Tail_SoundSet",
				"M16A2_InteriorTail_SoundSet",
				"M16A2_Slapback_SoundSet",
				"M16A2_Tail_2D_SoundSet"
			};
			soundSetShotExt[]=
			{
				
				{
					"M16A2_silencer_SoundSet",
					"M16A2_silencerTail_SoundSet",
					"M16A2_silencerInteriorTail_SoundSet"
				},
				
				{
					"M16A2_silencerHomeMade_SoundSet",
					"M16A2_silencerHomeMadeTail_SoundSet",
					"M16A2_silencerInteriorHomeMadeTail_SoundSet"
				}
			};
			reloadTime=0.125;
			recoil="recoil_m4";
			recoilProne="recoil_m4_prone";
			dispersion=0.00125;
			magazineSlot="magazine";
		};
		class FullAuto: Mode_FullAuto
		{
			soundSetShot[]=
			{
				"M16A2_Shot_SoundSet",
				"M16A2_Shot_iterior_SoundSet",
				"M16A2_Tail_SoundSet",
				"M16A2_InteriorTail_SoundSet",
				"M16A2_Slapback_SoundSet",
				"M16A2_Tail_2D_SoundSet"
			};
			soundSetShotExt[]=
			{
				
				{
					"M16A2_silencer_SoundSet",
					"M16A2_silencerTail_SoundSet",
					"M16A2_silencerInteriorTail_SoundSet"
				},
				
				{
					"M16A2_silencerHomeMade_SoundSet",
					"M16A2_silencerHomeMadeTail_SoundSet",
					"M16A2_silencerInteriorHomeMadeTail_SoundSet"
				}
			};
			reloadTime=0.075000003;
			recoil="recoil_m4";
			recoilProne="recoil_m4_prone";
			dispersion=0.00125;
			magazineSlot="magazine";
		};
		class OpticsInfo: OpticsInfoRifle
		{
			memoryPointCamera="eye";
			discreteDistance[]={200};
			discreteDistanceInitIndex=0;
			modelOptics="-";
			distanceZoomMin=200;
			distanceZoomMax=200;
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
					overrideParticle="weapon_shot_ump45_01";
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
				shotsToStartOverheating=7;
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
					hitpoints=200;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\M16A4\data\grip.rvmat",
								"KOD_weapons\weapons\M16A4\data\handguard.rvmat",
								"KOD_weapons\weapons\M16A4\data\receiver.rvmat",
								"KOD_weapons\weapons\M16A4\data\stock.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\M16A4\data\grip.rvmat",
								"KOD_weapons\weapons\M16A4\data\handguard.rvmat",
								"KOD_weapons\weapons\M16A4\data\receiver.rvmat",
								"KOD_weapons\weapons\M16A4\data\stock.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\M16A4\data\grip_damage.rvmat",
								"KOD_weapons\weapons\M16A4\data\handguard_damage.rvmat",
								"KOD_weapons\weapons\M16A4\data\receiver_damage.rvmat",
								"KOD_weapons\weapons\M16A4\data\stock_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\M16A4\data\grip_damage.rvmat",
								"KOD_weapons\weapons\M16A4\data\handguard_damage.rvmat",
								"KOD_weapons\weapons\M16A4\data\receiver_damage.rvmat",
								"KOD_weapons\weapons\M16A4\data\stock_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\M16A4\data\grip_destruct.rvmat",
								"KOD_weapons\weapons\M16A4\data\handguard_destruct.rvmat",
								"KOD_weapons\weapons\M16A4\data\receiver_destruct.rvmat",
								"KOD_weapons\weapons\M16A4\data\stock_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_M16A4: KOD_M16A4_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\M16A4\data\grip_co.paa",
			"KOD_weapons\weapons\M16A4\data\handguard_co.paa",
			"KOD_weapons\weapons\M16A4\data\receiver_co.paa",
			"KOD_weapons\weapons\M16A4\data\stock_co.paa"
		};
	};
};
