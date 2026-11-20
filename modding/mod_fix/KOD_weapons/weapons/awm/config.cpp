class CfgPatches
{
	class KOD_Weapons_AWM
	{
		units[]=
		{
			"KOD_AWM_Scope",
			"KOD_AWM_Suppressor"
		};
		weapons[]=
		{
			"KOD_AWM"
		};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Firearms",
			"DZ_Weapons_Optics",
			"DZ_Weapons_Magazines",
			"DZ_Sounds_Effects",
			"DZ_Scripts"
		};
		magazines[]=
		{
			"Mag_KOD_AWM_5rnd"
		};
		ammo[]={};
	};
};
class cfgVehicles
{
	class Inventory_Base;
	class ItemOptics;
	class KOD_AWM_Scope_Base: ItemOptics
	{
		scope=0;
		displayName="Прицел для AWM";
		descriptionShort="$STR_KOD_AWM_Scope";
		model="KOD_weapons\weapons\AWM\awm_scope.p3d";
		animClass="Binoculars";
		rotationFlags=4;
		reversed=0;
		weight=1113;
		itemSize[]={5,1};
		inventorySlot[]=
		{
			"weaponOpticsHunting"
		};
		simulation="itemoptics";
		dispersionModifier=-0.00034999999;
		dispersionCondition="true";
		recoilModifier[]={1,1,1};
		memoryPointCamera="eyeScope";
		cameraDir="cameraDir";
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=150;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\AWM\data\scope.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\AWM\data\scope.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\AWM\data\scope_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\AWM\data\scope_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\AWM\data\scope_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		class OpticsInfo
		{
			memoryPointCamera="eyeScope";
			cameraDir="cameraDir";
			useModelOptics=1;
			modelOptics="KOD_weapons\weapons\AWM\awm_scope_reticle.p3d";
			preloadOpticType="Preload2DOptic_KODAWMPScope";
			opticsDisablePeripherialVision=0.67000002;
			opticsFlare=1;
			opticsPPEffects[]=
			{
				"OpticsCHAbera3",
				"OpticsBlur1"
			};
			opticsZoomMin="0.3926/8";
			opticsZoomMax="0.3926/25";
			opticsZoomInit="0.3926/8";
			discretefov[]=
			{
				"0.3926/8",
				"0.3926/15",
				"0.3926/25"
			};
			discreteInitIndex=0;
			distanceZoomMin=300;
			distanceZoomMax=2000;
			discreteDistance[]={300,400,500,600,700,800,900,1000,1100,1200,1300,1400,1500,1600,1700,1800,1900,2000};
			discreteDistanceInitIndex=0;
			PPMaskProperties[]={0.5,0.5,0.43000001,0.1};
			PPLensProperties[]={0.30000001,0.15000001,0,0};
			PPBlurProperties=0.30000001;
		};
		class AnimationSources
		{
			class hide
			{
				source="user";
				animPeriod=0.0099999998;
				initPhase=0;
			};
		};
	};
	class Preload2DOptic_KODAWMPScope: Inventory_Base
	{
		scope=0;
		model="KOD_weapons\weapons\AWM\awm_scope_reticle.p3d";
	};
	class KOD_AWM_Scope: KOD_AWM_Scope_Base
	{
		scope=2;
	};
	class ItemSuppressor;
	class KOD_AWM_Suppressor: ItemSuppressor
	{
		scope=2;
		displayName="Глушитель AWM";
		descriptionShort="$STR_KOD_AWM_Suppressor";
		model="KOD_weapons\weapons\AWM\awm_suppressor.p3d";
		rotationFlags=17;
		reversed=0;
		weight=1000;
		itemSize[]={3,1};
		itemModelLength=0.15000001;
		inventorySlot[]=
		{
			"weaponMuzzleAWM"
		};
		simulation="ItemSuppressor";
		dispersionModifier=-9.9999997e-05;
		noiseShootModifier=-0.80000001;
		dispersionCondition="true";
		barrelArmor=3600;
		recoilModifier[]={0.94999999,0.94999999,0.94999999};
		swayModifier[]={1.05,1,1};
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
					hitpoints=8;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\AWM\data\suppressor\AWM.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\AWM\data\suppressor\AWM.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\AWM\data\suppressor\AWM_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\AWM\data\suppressor\AWM_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\AWM\data\suppressor\AWM_destruct.rvmat"
							}
						}
					};
				};
			};
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
	class BoltActionRifle_Base;
	class KOD_AWM_Base: BoltActionRifle_Base
	{
		scope=0;
		displayName="AWM";
		descriptionShort="$STR_KOD_AWM";
		model="KOD_weapons\weapons\AWM\awm.p3d";
		attachments[]=
		{
			"weaponWrap",
			"weaponOpticsHunting",
			"suppressorImpro",
			"weaponMuzzleAWM",
			"KODSticker1",
			"KODSticker2",
			"KODSticker3"
		};
		itemSize[]={10,3};
		weight=6500;
		absorbency=0;
		repairableWithKits[]={1};
		repairCosts[]={30,50};
		PPDOFProperties[]={};
		WeaponLength=1.2;
		chamberSize=1;
		chamberedRound="";
		chamberableFrom[]=
		{
			"KOD_Ammo_338"
		};
		magazines[]=
		{
			"Mag_KOD_AWM_5rnd"
		};
		barrelArmor=4.5999999;
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
				"AWMShot_SoundSet",
				"SVD_Shot_iterior_SoundSet",
				"SVD_Tail_SoundSet",
				"SVD_InteriorTail_SoundSet",
				"SVD_Tail_2D_SoundSet"
			};
			soundSetShotExt[]=
			{
				
				{
					"DZVR_KOD_AWM_Silencer_SoundSet",
					"SVD_silencerTail_SoundSet",
					"SVD_silencerInteriorTail_SoundSet"
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
					hitpoints=200;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\AWM\data\awm.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\AWM\data\awm.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\AWM\data\awm_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\AWM\data\awm_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\AWM\data\awm_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_AWM: KOD_AWM_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\AWM\data\awm_co.paa"
		};
	};
};
class cfgMagazines
{
	class Magazine_Base;
	class Mag_KOD_AWM_5rnd: Magazine_Base
	{
		scope=2;
		displayName="Магазин AWM, 5 пат.";
		descriptionShort="$STR_Mag_KOD_AWM_5Rnd";
		model="KOD_weapons\weapons\AWM\awm_mag.p3d";
		weight=100;
		itemSize[]={2,2};
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
					hitpoints=200;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\AWM\data\awm.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\AWM\data\awm.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\AWM\data\awm_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\AWM\data\awm_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\AWM\data\awm_destruct.rvmat"
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
	class ProxyAWM_suppressor: ProxyAttachment
	{
		scope=0;
		inventorySlot="weaponMuzzleAWM";
		model="KOD_weapons\weapons\AWM\awm_suppressor.p3d";
	};
};
class CfgSlots
{
	class Slot_weaponMuzzleAWM
	{
		name="weaponMuzzleAWM";
		displayName="Глушитель AWM";
		ghostIcon="supressor";
	};
};
class CfgSoundShaders
{
	class base_closeShot_SoundShader;
	class base_midShot_SoundShader;
	class base_distShot_SoundShader;
	class AWMcloseShot_SoundShader: base_closeShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\sounds\AWM_single_1",
				1
			}
		};
		volume=1;
	};
	class AWMmidShot_SoundShader: base_midShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\sounds\AWM_single_1",
				1
			}
		};
		volume=0.80000001;
	};
	class AWMdistShot_SoundShader: base_distShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\sounds\AWM_single_1",
				1
			}
		};
		volume=0.44999999;
	};
};
class CfgSoundSets
{
	class Rifle_Shot_Base_SoundSet;
	class AWMShot_SoundSet: Rifle_Shot_Base_SoundSet
	{
		soundShaders[]=
		{
			"AWMcloseShot_SoundShader",
			"AWMmidShot_SoundShader",
			"AWMdistShot_SoundShader"
		};
	};
};
