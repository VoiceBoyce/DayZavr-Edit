class CfgPatches
{
	class KOD_Weapons_M200
	{
		units[]=
		{
			"KOD_M200_Suppressor",
			"KOD_M200_Bipod_Grey",
			"KOD_M200_Bipod_Black"
		};
		weapons[]=
		{
			"KOD_M200_Grey",
			"KOD_M200_Black"
		};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Firearms",
			"DZ_Weapons_Muzzles",
			"DZ_Weapons_Magazines",
			"DZ_Scripts",
			"KOD_Bipod"
		};
		magazines[]=
		{
			"Mag_KOD_M200_7rnd"
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
	class KOD_M200_Base: BoltActionRifle_Base
	{
		scope=0;
		displayName="CheyTac M200 Intervention";
		descriptionShort="Famous bolt-action sniper rifle, one of the longest ranges of all modern-day sniper rifles, fed from a detachable magazine 7-round magazines. Chambered in .408";
		model="KOD_weapons\weapons\M200\m200.p3d";
		attachments[]=
		{
			"weaponWrap",
			"weaponOptics",
			"weaponOpticsHunting",
			"M200Suppressor",
			"KODM200Bipod",
			"KODSticker1",
			"KODSticker2",
			"KODSticker3"
		};
		itemSize[]={10,3};
		weight=12300;
		absorbency=0;
		repairableWithKits[]={1};
		repairCosts[]={30,50};
		initSpeedMultiplier=1.15;
		PPDOFProperties[]={};
		WeaponLength=1.3;
		chamberSize=1;
		chamberedRound="";
		chamberableFrom[]=
		{
			"KOD_Ammo_408"
		};
		magazines[]=
		{
			"Mag_KOD_M200_7rnd"
		};
		barrelArmor=3.2;
		ejectType=1;
		recoilModifier[]={2.5,2,1.7};
		swayModifier[]={3,2.2,1.2};
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
				"m200_Shot_SoundSet",
				"LongHorn_Tail_SoundSet",
				"LongHorn_InteriorTail_SoundSet"
			};
			soundSetShotExt[]=
			{
				
				{
					"m200_silencerHomeMade_SoundSet",
					"LongHorn_silencerHomeMadeTail_SoundSet",
					"LongHorn_silencerInteriorHomeMadeTail_SoundSet"
				}
			};
			reloadTime=1;
			dispersion=9.9999997e-05;
			magazineSlot="magazine";
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
								"KOD_weapons\weapons\M200\data\m200.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\M200\data\m200.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\M200\data\m200_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\M200\data\m200_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\M200\data\m200_destruct.rvmat"
							}
						}
					};
				};
			};
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
	};
	class KOD_M200_Grey: KOD_M200_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\M200\data\m200_co.paa"
		};
	};
	class KOD_M200_Black: KOD_M200_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\M200\data\m200_black_co.paa"
		};
	};
};
class cfgMagazines
{
	class Magazine_Base;
	class Mag_KOD_M200_7rnd: Magazine_Base
	{
		scope=2;
		displayName="M200/M300 Magazine";
		descriptionShort="Magazine for M200/M300 rifles. Holds up to 7 .408 Rounds";
		model="KOD_weapons\weapons\M200\m200_mag.p3d";
		weight=200;
		itemSize[]={3,3};
		count=7;
		ammo="KOD_Bullet_408";
		ammoItems[]=
		{
			"KOD_Ammo_408"
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
								"KOD_weapons\weapons\M200\data\m200.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\M200\data\m200.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\M200\data\m200_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\M200\data\m200_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\M200\data\m200_destruct.rvmat"
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
	class KOD_M200_Suppressor: ItemSuppressor
	{
		scope=2;
		displayName="M200 Suppressor";
		descriptionShort="A muzzle device for CheyTac M200. Reduces sound intensity and muzzle flash generated by firing.";
		model="KOD_weapons\weapons\M200\m200_suppressor.p3d";
		rotationFlags=17;
		reversed=0;
		weight=300;
		itemSize[]={3,1};
		itemModelLength=0.15000001;
		inventorySlot[]=
		{
			"M200Suppressor"
		};
		simulation="ItemSuppressor";
		dispersionModifier=-9.9999997e-05;
		noiseShootModifier=-0.89999998;
		dispersionCondition="true";
		barrelArmor=800;
		recoilModifier[]={0.94999999,0.94999999,0.94999999};
		swayModifier[]={1,1,1};
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
					hitpoints=5;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\M200\data\suppressor.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\M200\data\suppressor.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\M200\data\suppressor_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\M200\data\suppressor_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\M200\data\suppressor_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_Bipod_Base;
	class KOD_M200_Bipod_Base: KOD_Bipod_Base
	{
		scope=0;
		displayName="Сошки M200";
		descriptionShort="Сошки для M200. Значительно уменьшают отдачу при стрельбе лёжа.";
		model="KOD_weapons\weapons\M200\m200_bipod.p3d";
		weight=200;
		itemSize[]={1,3};
		inventorySlot[]=
		{
			"KODM200Bipod"
		};
		recoilDecrease=0.2;
		hiddenSelections[]=
		{
			"color"
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
								"KOD_weapons\weapons\PKP\data\pkp.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\PKP\data\pkp.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\PKP\data\pkp_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\PKP\data\pkp_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\PKP\data\pkp_destruct.rvmat"
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
	class KOD_M200_Bipod_Grey: KOD_M200_Bipod_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\M200\data\m200_co.paa"
		};
	};
	class KOD_M200_Bipod_Black: KOD_M200_Bipod_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\M200\data\m200_black_co.paa"
		};
	};
};
class CfgNonAIVehicles
{
	class ProxyAttachment;
	class ProxyM200_suppressor: ProxyAttachment
	{
		scope=0;
		inventorySlot="M200Suppressor";
		model="KOD_weapons\weapons\M200\m200_suppressor.p3d";
	};
	class Proxym200_bipod: ProxyAttachment
	{
		scope=0;
		inventorySlot="KODM200Bipod";
		model="KOD_weapons\weapons\M200\m200_bipod.p3d";
	};
};
class CfgSoundShaders
{
	class baseCharacter_SoundShader;
	class m200_Closure_SoundShader
	{
		samples[]=
		{
			
			{
				"DZ\sounds\weapons\firearms\SVD\SVD_closure_01",
				1
			},
			
			{
				"DZ\sounds\weapons\firearms\SVD\SVD_closure_02",
				1
			}
		};
		range=5;
		volume=0.44668359;
	};
	class m200_closeShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\M200\sound\m200_close",
				1
			}
		};
		volume=2;
		range=100;
		rangeCurve="closeShotCurve";
	};
	class m200_midShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\M200\sound\m200_mid",
				1
			}
		};
		volume=0.56234133;
		range=1500;
		rangeCurve[]=
		{
			{0,0.2},
			{50,1},
			{300,0},
			{1500,0}
		};
	};
	class m200_distShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\M200\sound\m200_far",
				1
			}
		};
		volume=1;
		range=3500;
		rangeCurve[]=
		{
			{0,0},
			{50,0},
			{300,1},
			{3500,1}
		};
	};
	class m200_silencerHomeMadeCloseShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\M200\sound\m200_silenced_close",
				1
			}
		};
		volume=1;
		range=100;
		rangeCurve="closeShotCurve";
	};
	class m200_silencerHomeMadeMidShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\M200\sound\m200_silenced_mid",
				1
			}
		};
		volume=0.56234133;
		range=500;
		rangeCurve[]=
		{
			{0,0.2},
			{50,1},
			{140,0},
			{250,0}
		};
	};
	class m200_silencerHomeMadeDistShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\M200\sound\m200_silenced_far",
				1
			}
		};
		volume=1;
		range=800;
		rangeCurve[]=
		{
			{0,0},
			{50,0},
			{140,1},
			{250,1}
		};
	};
};
class CfgSoundSets
{
	class Rifle_Shot_Base_SoundSet;
	class Rifle_silencerHomemade_Base_SoundSet;
	class m200_Shot_SoundSet: Rifle_Shot_Base_SoundSet
	{
		soundShaders[]=
		{
			"m200_closeShot_SoundShader",
			"m200_midShot_SoundShader",
			"m200_distShot_SoundShader",
			"m200_Closure_SoundShader"
		};
	};
	class m200_silencerHomeMade_SoundSet: Rifle_silencerHomemade_Base_SoundSet
	{
		soundShaders[]=
		{
			"m200_silencerHomeMadeCloseShot_SoundShader",
			"m200_silencerHomeMadeMidShot_SoundShader",
			"m200_silencerHomeMadeDistShot_SoundShader",
			"m200_Closure_SoundShader"
		};
	};
};
class CfgSlots
{
	class Slot_M200Suppressor
	{
		name="M200Suppressor";
		displayName="M200Suppressor";
		ghostIcon="supressor";
	};
	class Slot_KODM200Bipod
	{
		name="KODM200Bipod";
		displayName="Сошки М200";
		GhostIcon="set:kod_icons image:Bipod";
	};
};
