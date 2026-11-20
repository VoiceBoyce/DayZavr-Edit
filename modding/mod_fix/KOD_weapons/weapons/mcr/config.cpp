class CfgPatches
{
	class KOD_Weapons_MCR
	{
		units[]={};
		weapons[]=
		{
			"KOD_MCR_Tan",
			"KOD_MCR_Black",
			"KOD_MCR_BlackTan",
			"KOD_MCR_TanCamo",
			"KOD_MCR_UCPCamo",
			"KOD_MCR_WoodlandCamo"
		};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Firearms"
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
	class KOD_MCR_Base: Rifle_Base
	{
		scope=0;
		displayName="MCR";
		descriptionShort="$STR_KOD_MCR";
		weight=3900;
		model="KOD_weapons\weapons\MCR\rifle.p3d";
		absorbency=0;
		repairableWithKits[]={1};
		repairCosts[]={25};
		PPDOFProperties[]={1,0.60000002,50,150,4,10};
		WeaponLength=1.1;
		barrelArmor=4.888;
		initSpeedMultiplier=1;
		attachments[]=
		{
			"weaponWrap",
			"weaponOptics",
			"weaponOpticsHunting",
			"weaponFlashlight",
			"weaponMuzzleM4",
			"KODRISBipod",
			"KODSticker1",
			"KODSticker2",
			"KODSticker3"
		};
		itemSize[]={9,3};
		chamberSize=1;
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
			"Mag_CMAG_10Rnd_Green",
			"Mag_CMAG_20Rnd_Green",
			"Mag_CMAG_30Rnd_Green",
			"Mag_CMAG_40Rnd_Green",
			"Mag_CMAG_10Rnd_Black",
			"Mag_CMAG_100Rnd_Black",
			"Mag_CMAG_20Rnd_Black",
			"Mag_CMAG_30Rnd_Black",
			"Mag_CMAG_40Rnd_Black"
		};
		ejectType=1;
		recoilModifier[]={0.60000002,0.60000002,0.60000002};
		swayModifier[]={1.8,2.05,1};
		hiddenSelections[]=
		{
			"color"
		};
		simpleHiddenSelections[]=
		{
			"hide_barrel",
			"hide_ironsights"
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
			dispersion=0.0012000001;
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
			reloadTime=0.07;
			dispersion=0.0012000001;
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
					overrideParticle="weapon_shot_ump45_01";
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
				maxOverheatingValue=60;
				shotsToStartOverheating=30;
				overheatingDecayInterval=1;
				class SmokingBarrel1
				{
					positionOffset[]={-0.02,0,0};
					overrideParticle="smoking_barrel_small";
					onlyWithinOverheatLimits[]={0,0.2};
					onlyWithinRainLimits[]={0,0.2};
				};
				class SmokingBarrelHot1
				{
					positionOffset[]={0.1,0,0};
					overrideParticle="smoking_barrel";
					onlyWithinOverheatLimits[]={0.2,0.60000002};
					onlyWithinRainLimits[]={0,0.2};
				};
				class SmokingBarrelHot3
				{
					positionOffset[]={0.30000001,0,0};
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
					hitpoints=450;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\MCR\data\rifle.rvmat",
								"KOD_weapons\weapons\MCR\data\rifle_accs.rvmat",
								"KOD_weapons\weapons\MCR\data\rifle_rail.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\MCR\data\rifle.rvmat",
								"KOD_weapons\weapons\MCR\data\rifle_accs.rvmat",
								"KOD_weapons\weapons\MCR\data\rifle_rail.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\MCR\data\rifle_damage.rvmat",
								"KOD_weapons\weapons\MCR\data\rifle_accs_damage.rvmat",
								"KOD_weapons\weapons\MCR\data\rifle_rail_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\MCR\data\rifle_damage.rvmat",
								"KOD_weapons\weapons\MCR\data\rifle_accs_damage.rvmat",
								"KOD_weapons\weapons\MCR\data\rifle_rail_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\MCR\data\rifle_destruct.rvmat",
								"KOD_weapons\weapons\MCR\data\rifle_accs_destruct.rvmat",
								"KOD_weapons\weapons\MCR\data\rifle_rail_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_MCR_Tan: KOD_MCR_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\MCR\data\rifle_tan_co.paa"
		};
	};
	class KOD_MCR_Black: KOD_MCR_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\MCR\data\rifle_black_co.paa"
		};
	};
	class KOD_MCR_BlackTan: KOD_MCR_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\MCR\data\rifle_tanblack_co.paa"
		};
	};
	class KOD_MCR_TanCamo: KOD_MCR_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\MCR\data\rifle_tancamo_co.paa"
		};
	};
	class KOD_MCR_UCPCamo: KOD_MCR_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\MCR\data\rifle_ucpcamo_co.paa"
		};
	};
	class KOD_MCR_WoodlandCamo: KOD_MCR_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\MCR\data\rifle_camo_co.paa"
		};
	};
};
