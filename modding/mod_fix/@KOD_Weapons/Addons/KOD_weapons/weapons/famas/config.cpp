class CfgPatches
{
	class KOD_Weapons_FAMAS
	{
		units[]=
		{
			"KOD_FAMAS_Bipod"
		};
		weapons[]=
		{
			"KOD_FAMAS"
		};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Firearms",
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
	class KOD_FAMAS_Base: Rifle_Base
	{
		scope=0;
		displayName="FAMAS";
		descriptionShort="$STR_KOD_FAMAS";
		model="KOD_weapons\weapons\Famas\famas.p3d";
		attachments[]=
		{
			"weaponWrap",
			"weaponOptics",
			"weaponMuzzleM4",
			"KODFAMASBipod",
			"KODSticker1",
			"KODSticker2",
			"KODSticker3"
		};
		itemSize[]={7,3};
		weight=3800;
		absorbency=0;
		repairableWithKits[]={1};
		repairCosts[]={30,25};
		PPDOFProperties[]={1,0.5,20,170,4,10};
		initSpeedMultiplier=1;
		WeaponLength=0.64999998;
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
			"Mag_STANAGCoupled_30Rnd",
			"Mag_STANAG_60Rnd",
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
		recoilModifier[]={0.62,0.60000002,0.58999997};
		swayModifier[]={1.2,1.5,0.94999999};
		hiddenSelections[]=
		{
			"color"
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
				"Famas_Shot_SoundSet",
				"Famas_Tail_SoundSet",
				"Famas_InteriorTail_SoundSet"
			};
			soundSetShotExt[]=
			{
				
				{
					"Famas_silencer_SoundSet",
					"Famas_silencerTail_SoundSet",
					"Famas_silencerInteriorTail_SoundSet"
				},
				
				{
					"Famas_silencerHomeMade_SoundSet",
					"Famas_silencerHomeMadeTail_SoundSet",
					"Famas_silencerInteriorHomeMadeTail_SoundSet"
				}
			};
			reloadTime=0.1;
			dispersion=0.0020000001;
			magazineSlot="magazine";
		};
		class FullAuto: Mode_FullAuto
		{
			soundSetShot[]=
			{
				"Famas_Shot_SoundSet",
				"Famas_Tail_SoundSet",
				"Famas_InteriorTail_SoundSet"
			};
			soundSetShotExt[]=
			{
				
				{
					"Famas_silencer_SoundSet",
					"Famas_silencerTail_SoundSet",
					"Famas_silencerInteriorTail_SoundSet"
				},
				
				{
					"Famas_silencerHomeMade_SoundSet",
					"Famas_silencerHomeMadeTail_SoundSet",
					"Famas_silencerInteriorHomeMadeTail_SoundSet"
				}
			};
			reloadTime=0.064999998;
			dispersion=0.0020000001;
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
				shotsToStartOverheating=10;
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
					hitpoints=450;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\Famas\data\famas.rvmat",
								"KOD_weapons\weapons\Famas\data\bipod.rvmat",
								"KOD_weapons\weapons\Famas\data\rails.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\Famas\data\famas.rvmat",
								"KOD_weapons\weapons\Famas\data\bipod.rvmat",
								"KOD_weapons\weapons\Famas\data\rails.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\Famas\data\famas_damage.rvmat",
								"KOD_weapons\weapons\Famas\data\bipod_damage.rvmat",
								"KOD_weapons\weapons\Famas\data\rails_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\Famas\data\famas_damage.rvmat",
								"KOD_weapons\weapons\Famas\data\bipod_damage.rvmat",
								"KOD_weapons\weapons\Famas\data\rails_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\Famas\data\famas_destruct.rvmat",
								"KOD_weapons\weapons\Famas\data\bipod_destruct.rvmat",
								"KOD_weapons\weapons\Famas\data\rails_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_FAMAS: KOD_FAMAS_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\Famas\data\famas_co.paa"
		};
	};
};
class cfgVehicles
{
	class KOD_Bipod_Base;
	class KOD_FAMAS_Bipod: KOD_Bipod_Base
	{
		scope=2;
		displayName="Сошки FAMAS";
		descriptionShort="Быстросъемные сошки для FAMAS. Значительно уменьшают отдачу при стрельбе лёжа.";
		model="KOD_weapons\weapons\Famas\famas_bipod.p3d";
		weight=200;
		itemSize[]={1,3};
		inventorySlot[]=
		{
			"KODFAMASBipod"
		};
		recoilDecrease=0.2;
		hiddenSelections[]=
		{
			"color"
		};
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\Famas\data\bipod_co.paa"
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
								"KOD_weapons\weapons\Famas\data\bipod.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\Famas\data\bipod.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\Famas\data\bipod_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\Famas\data\bipod_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\Famas\data\bipod_destruct.rvmat"
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
	class Proxyfamas_bipod: ProxyAttachment
	{
		scope=0;
		inventorySlot="KODFAMASBipod";
		model="KOD_weapons\weapons\Famas\famas_bipod.p3d";
	};
};
class CfgSlots
{
	class Slot_KODFAMASBipod
	{
		name="KODFAMASBipod";
		displayName="Сошки FAMAS";
		GhostIcon="set:kod_icons image:Bipod";
	};
};
