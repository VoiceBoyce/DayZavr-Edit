class CfgPatches
{
	class KOD_SV98
	{
		units[]={};
		weapons[]=
		{
			"KOD_SV98_Green",
			"KOD_SV98_Black"
		};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data"
		};
		magazines[]=
		{
			"Mag_KOD_SV98_10rnd"
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
	class BoltActionRifle_ExternalMagazine_Base;
	class KOD_SV98_Base: BoltActionRifle_ExternalMagazine_Base
	{
		scope=0;
		displayName="СВ-98";
		descriptionShort="Российская магазинная снайперская винтовка, разработанная в 1998-2000 годах конструкторской группой под руководством Владимира Дмитриевича Стронского и серийно выпускаемая ОАО Ижмаш. Питание осуществляется из отъемных магазинов патронами 7,62х54 мм.";
		model="KOD_weapons\weapons\SV98\sv98.p3d";
		attachments[]=
		{
			"weaponWrap",
			"weaponOptics",
			"weaponOpticsHunting",
			"suppressorImpro",
			"KODSticker1",
			"KODSticker2",
			"KODSticker3"
		};
		weight=5000;
		absorbency=0;
		repairableWithKits[]={1};
		repairCosts[]={25};
		PPDOFProperties[]={1,0.5,50,180,4,10};
		WeaponLength=1.18986;
		chamberedRound="";
		chamberSize=1;
		chamberableFrom[]=
		{
			"Ammo_762x54",
			"Ammo_762x54Tracer"
		};
		magazines[]=
		{
			"Mag_KOD_SV98_10rnd",
			"Mag_SV98_10rnd"
		};
		ejectType=1;
		itemSize[]={10,3};
		barrelArmor=3;
		initSpeedMultiplier=1.09;
		recoilModifier[]={1,1,1};
		swayModifier[]={2.3,2.3,1.3};
		simpleHiddenSelections[]=
		{
			"hide_barrel"
		};
		hiddenSelections[]=
		{
			"color1",
			"color2",
			"color3"
		};
		modes[]=
		{
			"Single"
		};
		class Single: Mode_SemiAuto
		{
			soundSetShot[]=
			{
				"KOD_SV98Shot_SoundSet",
				"SV98_Shot_interior_SoundSet",
				"SV98_Tail_SoundSet",
				"SV98_InteriorTail_SoundSet",
				"SV98_Tail_2D_SoundSet"
			};
			soundSetShotExt[]=
			{
				
				{
					"SV98_silencerHomeMade_SoundSet",
					"SV98_silencerHomeMadeTail_SoundSet",
					"SV98_silencerInteriorHomeMadeTail_SoundSet"
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
					hitpoints=500;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\SV98\data\body.rvmat",
								"KOD_weapons\weapons\SV98\data\receiver.rvmat",
								"KOD_weapons\weapons\SV98\data\barrel.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\SV98\data\body.rvmat",
								"KOD_weapons\weapons\SV98\data\receiver.rvmat",
								"KOD_weapons\weapons\SV98\data\barrel.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\SV98\data\body_damage.rvmat",
								"KOD_weapons\weapons\SV98\data\receiver_damage.rvmat",
								"KOD_weapons\weapons\SV98\data\barrel_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\SV98\data\body_damage.rvmat",
								"KOD_weapons\weapons\SV98\data\receiver_damage.rvmat",
								"KOD_weapons\weapons\SV98\data\barrel_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\SV98\data\body_destruct.rvmat",
								"KOD_weapons\weapons\SV98\data\receiver_destruct.rvmat",
								"KOD_weapons\weapons\SV98\data\barrel_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_SV98_Green: KOD_SV98_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\SV98\data\body_green_co.paa",
			"KOD_weapons\weapons\SV98\data\receiver_co.paa",
			"KOD_weapons\weapons\SV98\data\barrel_co.paa"
		};
	};
	class KOD_SV98_Black: KOD_SV98_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\SV98\data\body_black_co.paa",
			"KOD_weapons\weapons\SV98\data\receiver_co.paa",
			"KOD_weapons\weapons\SV98\data\barrel_co.paa"
		};
	};
};
class cfgMagazines
{
	class Magazine_Base;
	class Mag_KOD_SV98_10rnd: Magazine_Base
	{
		scope=2;
		displayName="Маг. СВ-98 на 10 пат.";
		descriptionShort="Отъемный коробчатый магазин для СВ-98. Вмещает до 10 патронов калибра 7,62х54 мм.";
		model="KOD_weapons\weapons\SV98\sv98_mag.p3d";
		weight=70;
		itemSize[]={1,2};
		count=10;
		ammo="Bullet_762x54";
		ammoItems[]=
		{
			"Ammo_762x54",
			"Ammo_762x54Tracer"
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
								"KOD_weapons\weapons\SV98\data\mag.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\SV98\data\mag.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\SV98\data\mag_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\SV98\data\mag_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\SV98\data\mag_destruct.rvmat"
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
	class KOD_SV98closeShot_SoundShader: base_closeShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\sounds\KOD_SV98",
				1
			}
		};
		volume=1;
	};
	class KOD_SV98midShot_SoundShader: base_midShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\sounds\KOD_SV98",
				1
			}
		};
		volume=0.80000001;
	};
	class KOD_SV98distShot_SoundShader: base_distShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\sounds\KOD_SV98",
				1
			}
		};
		volume=0.44999999;
	};
};
class CfgSoundSets
{
	class Rifle_Shot_Base_SoundSet;
	class KOD_SV98Shot_SoundSet: Rifle_Shot_Base_SoundSet
	{
		soundShaders[]=
		{
			"KOD_SV98closeShot_SoundShader",
			"KOD_SV98midShot_SoundShader",
			"KOD_SV98distShot_SoundShader"
		};
	};
};