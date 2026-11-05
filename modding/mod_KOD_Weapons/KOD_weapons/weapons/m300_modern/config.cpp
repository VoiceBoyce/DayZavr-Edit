class CfgPatches
{
	class KOD_M300_Modern
	{
		units[]={};
		weapons[]=
		{
			"KOD_M300M"
		};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Firearms"
		};
		magazines[]={};
		ammo[]={};
	};
};
class Mode_Safe;
class Mode_SemiAuto;
class Mode_Single;
class Mode_Burst;
class Mode_FullAuto;
class OpticsInfoRifle;
class cfgWeapons
{
	class BoltActionRifle_Base;
	class KOD_M300M_Base: BoltActionRifle_Base
	{
		scope=0;
		displayName="CheyTac M300 Tactical";
		descriptionShort="Версия винтовки М300 с облегченным карбоновым цевьём и настраиваемым прикладом. Питание из отъёмного магазина M200/M300, патронник под патрон .408 CheyTac";
		weight=8000;
		model="KOD_weapons\weapons\M300_Modern\m300.p3d";
		attachments[]=
		{
			"weaponWrap",
			"weaponOptics",
			"weaponOpticsHunting",
			"KODRISBipod",
			"KODSticker1",
			"KODSticker2",
			"KODSticker3"
		};
		absorbency=0;
		repairableWithKits[]={1};
		repairCosts[]={30,25};
		PPDOFProperties[]={1,0.5,50,180,4,10};
		initSpeedMultiplier=1.2;
		WeaponLength=1.38986;
		chamberedRound="";
		chamberSize=1;
		chamberableFrom[]=
		{
			"KOD_Ammo_408"
		};
		magazines[]=
		{
			"Mag_KOD_M200_7rnd"
		};
		ejectType=1;
		itemSize[]={10,3};
		barrelArmor=6;
		recoilModifier[]={3.4000001,2.4000001,2.4000001};
		swayModifier[]={2.2,2.2,1.2};
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
		class Single: Mode_Single
		{
			soundSetShot[]=
			{
				"M300_Shot_SoundSet",
				"Win_Tail_SoundSet",
				"Win_InteriorTail_SoundSet"
			};
			soundSetShotExt[]=
			{
				
				{
					"Win_silencerHomeMade_SoundSet",
					"Win_silencerHomeMadeTail_SoundSet",
					"Win_silencerInteriorHomeMadeTail_SoundSet"
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
					hitpoints=300;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\M300_Modern\data\rifle.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\M300_Modern\data\rifle.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\M300_Modern\data\rifle_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\M300_Modern\data\rifle_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\M300_Modern\data\rifle_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_M300M: KOD_M300M_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\M300_Modern\data\rifle_co.paa"
		};
	};
};
