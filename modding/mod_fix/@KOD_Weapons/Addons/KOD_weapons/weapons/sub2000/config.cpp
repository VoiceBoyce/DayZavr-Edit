class CfgPatches
{
	class KOD_Weapons_SUB2000
	{
		units[]={};
		weapons[]=
		{
			"KOD_SUB2000",
			"KOD_SUB2000_Suppressed"
		};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Firearms"
		};
		magazines[]=
		{
			"Mag_KOD_SUB2000_30rnd"
		};
	};
};
class Mode_Safe;
class Mode_SemiAuto;
class Mode_Burst;
class Mode_FullAuto;
class OpticsInfoRifle;
class CfgWeapons
{
	class Rifle_Base;
	class KOD_SUB2000_Base: Rifle_Base
	{
		scope=0;
		displayName="Kel-Tec SUB2000";
		descriptionShort="Компактный складной карабин был разработан как оружие-компаньон для тех, кому приходится много передвигаться, в первую очередь как оружие самообороны, а также как оружие для тренировок или развлекательной стрельбы. Питание осуществляется из отъемных магазинов под патроны калибра 9х19 мм.";
		model="KOD_weapons\weapons\SUB2000\sub2000.p3d";
		attachments[]=
		{
			"weaponOptics",
			"weaponWrap",
			"weaponFlashlight",
			"KODSticker1",
			"KODSticker2",
			"KODSticker3"
		};
		itemSize[]={6,3};
		weight=1800;
		absorbency=0;
		repairableWithKits[]={1};
		repairCosts[]={25};
		PPDOFProperties[]={1,0.5,10,140,4,10};
		WeaponLength=0.60000002;
		initSpeedMultiplier=1.15;
		chamberSize=1;
		chamberedRound="";
		chamberableFrom[]=
		{
			"Ammo_9x19"
		};
		magazines[]=
		{
			"Mag_KOD_SUB2000_30rnd"
		};
		barrelArmor=5;
		ejectType=1;
		recoilModifier[]={0.69999999,0.69999999,0.69999999};
		swayModifier[]={0.69999999,0.69999999,0.60000002};
		hiddenSelections[]=
		{
			"color"
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
				"Glock19_Shot_SoundSet",
				"Glock19_Tail_SoundSet",
				"Glock19_InteriorTail_SoundSet"
			};
			soundSetShotExt[]=
			{
				
				{
					"Glock19_silencerPro_SoundSet",
					"Glock19_silencerTail_SoundSet",
					"Glock19_silencerInteriorTail_SoundSet"
				}
			};
			reloadTime=0.125;
			dispersion=0.0035000001;
			magazineSlot="magazine";
		};
		class FullAuto: Mode_FullAuto
		{
			soundSetShot[]=
			{
				"Glock19_Shot_SoundSet",
				"Glock19_Tail_SoundSet",
				"Glock19_InteriorTail_SoundSet"
			};
			soundSetShotExt[]=
			{
				
				{
					"Glock19_silencerPro_SoundSet",
					"Glock19_silencerTail_SoundSet",
					"Glock19_silencerInteriorTail_SoundSet"
				}
			};
			reloadTime=0.067000002;
			dispersion=0.0035000001;
			magazineSlot="magazine";
		};
		class Particles
		{
			class OnFire
			{
				class MuzzleFlashForward
				{
					ignoreIfSuppressed=1;
					illuminateWorld=1;
					overrideParticle="weapon_shot_fnx_01";
				};
				class SmokeCloud
				{
					overrideParticle="weapon_shot_winded_smoke_small";
				};
			};
			class OnOverheating
			{
				shotsToStartOverheating=5;
				maxOverheatingValue=15;
				overheatingDecayInterval=1;
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
		class OpticsInfo: OpticsInfoRifle
		{
			memoryPointCamera="eye";
			discreteDistance[]={25,50,100,150};
			discreteDistanceInitIndex=0;
			modelOptics="-";
			distanceZoomMin=25;
			distanceZoomMax=150;
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
								"KOD_weapons\weapons\SUB2000\data\sub2000_01.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\SUB2000\data\sub2000_01.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\SUB2000\data\sub2000_01_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\SUB2000\data\sub2000_01_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\SUB2000\data\sub2000_01_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_SUB2000: KOD_SUB2000_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\SUB2000\data\sub2000_01_co.paa"
		};
	};
	class KOD_SUB2000_Suppressed_Base: KOD_SUB2000_Base
	{
		scope=0;
		displayName="Kel-Tec SUB2000 (Глуш.)";
		descriptionShort="Компактный складной карабин был разработан как оружие-компаньон для тех, кому приходится много передвигаться, в первую очередь как оружие самообороны, а также как оружие для тренировок или развлекательной стрельбы. Питание осуществляется из отъемных магазинов под патроны калибра 9х19 мм.";
		model="KOD_weapons\weapons\SUB2000\sub2000_suppressed.p3d";
		attachments[]=
		{
			"weaponOptics",
			"weaponWrap",
			"KODSticker1",
			"KODSticker2",
			"KODSticker3"
		};
		hiddenSelections[]=
		{
			"color1",
			"color2"
		};
		class SemiAuto: Mode_SemiAuto
		{
			soundSetShot[]=
			{
				"colt1911_silencer_SoundSet",
				"colt1911_silencerTail_SoundSet",
				"colt1911_silencerInteriorTail_SoundSet"
			};
			reloadTime=0.125;
			dispersion=0.0035000001;
			magazineSlot="magazine";
		};
		class FullAuto: Mode_FullAuto
		{
			soundSetShot[]=
			{
				"colt1911_silencer_SoundSet",
				"colt1911_silencerTail_SoundSet",
				"colt1911_silencerInteriorTail_SoundSet"
			};
			reloadTime=0.067000002;
			dispersion=0.0035000001;
			magazineSlot="magazine";
		};
		class NoiseShoot
		{
			strength=5;
			type="sound";
		};
		class Particles
		{
			class OnFire
			{
				class MuzzleFlash
				{
					overrideParticle="weapon_shot_fnx_02";
				};
				class SmokeCloud
				{
					overrideParticle="weapon_shot_winded_smoke_small";
				};
			};
			class OnOverheating
			{
				shotsToStartOverheating=5;
				maxOverheatingValue=10;
				overheatingDecayInterval=1;
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
					hitpoints=300;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\SUB2000\data\sub2000_01.rvmat",
								"KOD_weapons\weapons\SUB2000\data\sub2000_02.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\SUB2000\data\sub2000_01.rvmat",
								"KOD_weapons\weapons\SUB2000\data\sub2000_02.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\SUB2000\data\sub2000_01_damage.rvmat",
								"KOD_weapons\weapons\SUB2000\data\sub2000_02_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\SUB2000\data\sub2000_01_damage.rvmat",
								"KOD_weapons\weapons\SUB2000\data\sub2000_02_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\SUB2000\data\sub2000_01_destruct.rvmat",
								"KOD_weapons\weapons\SUB2000\data\sub2000_02_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_SUB2000_Suppressed: KOD_SUB2000_Suppressed_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\SUB2000\data\sub2000_01_co.paa",
			"KOD_weapons\weapons\SUB2000\data\sub2000_02_co.paa"
		};
	};
};
class cfgMagazines
{
	class Magazine_Base;
	class Mag_KOD_SUB2000_30rnd: Magazine_Base
	{
		scope=2;
		displayName="Магазин SUB2000, 30 пат.";
		descriptionShort="Магазин для SUB2000. Вмещает до 30 патронов калибра 9x19 мм.";
		model="KOD_weapons\weapons\SUB2000\sub2000_mag.p3d";
		weight=120;
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
					hitpoints=200;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\SUB2000\data\sub2000_02.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\SUB2000\data\sub2000_02.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\SUB2000\data\sub2000_02_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\SUB2000\data\sub2000_02_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\SUB2000\data\sub2000_02_destruct.rvmat"
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
