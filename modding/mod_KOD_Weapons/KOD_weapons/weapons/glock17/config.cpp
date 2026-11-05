class CfgPatches
{
	class KOD_Weapons_Glock17
	{
		units[]=
		{
			"KOD_Glock17_FlashHider",
			"KOD_Glock17_Compensator"
		};
		weapons[]=
		{
			"KOD_Glock17_Custom_Black",
			"KOD_Glock17_Custom_Tan",
			"KOD_Glock17_Custom_BlackTan",
			"KOD_Glock17_Custom_TanBlack",
			"KOD_Glock17_Standard_Black",
			"KOD_Glock17_Standard_Tan",
			"KOD_Glock17_Standard_BlackTan",
			"KOD_Glock17_Standard_TanBlack"
		};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Pistols",
			"DZ_Pistols_Glock"
		};
		magazines[]=
		{
			"Mag_KOD_Glock17_Black_16rnd",
			"Mag_KOD_Glock17_Tan_16rnd",
			"Mag_KOD_Glock17_Black_30rnd",
			"Mag_KOD_Glock17_Tan_30rnd"
		};
	};
};
class Mode_Safe;
class Mode_SemiAuto;
class Mode_Burst;
class Mode_FullAuto;
class OpticsInfoPistol;
class cfgWeapons
{
	class Pistol_Base;
	class Glock19_Base: Pistol_Base
	{
		magazines[]+=
		{
			"Mag_Glock_15Rnd",
			"Mag_KOD_Glock17_Black_16rnd",
			"Mag_KOD_Glock17_Black_30rnd",
			"Mag_KOD_Glock17_Tan_16rnd",
			"Mag_KOD_Glock17_Tan_30rnd"
		};
	};
	class KOD_Glock17_Base: Pistol_Base
	{
		scope=0;
		displayName="Glock 17";
		descriptionShort="Австрийский пистолет, разработанный компанией Glock для вооруженных сил Австрии. Питание осуществляется из отъёмного магазина. Использует патроны 9х19 мм.";
		attachments[]=
		{
			"pistolOptics",
			"pistolFlashlight",
			"pistolMuzzle",
			"KODSticker1"
		};
		itemSize[]={3,2};
		weight=1200;
		absorbency=0;
		repairableWithKits[]={1};
		repairCosts[]={25};
		PPDOFProperties[]={1,0.5,10,200,4,10};
		WeaponLength=0.63;
		barrelArmor=2;
		chamberSize=1;
		chamberedRound="";
		magazines[]=
		{
			"Mag_Glock_15Rnd",
			"Mag_KOD_Glock17_Black_16rnd",
			"Mag_KOD_Glock17_Black_30rnd",
			"Mag_KOD_Glock17_Tan_16rnd",
			"Mag_KOD_Glock17_Tan_30rnd"
		};
		chamberableFrom[]=
		{
			"Ammo_9x19"
		};
		ejectType=1;
		hiddenSelections[]=
		{
			"color1",
			"color2",
			"color3"
		};
		simpleHiddenSelections[]=
		{
			"hide_ironsights"
		};
		modes[]=
		{
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
			reloadTime=0.13;
			dispersion=0.0024999999;
			magazineSlot="magazine";
		};
		class NoiseShoot
		{
			strength=50;
			type="shot";
		};
		class OpticsInfo: OpticsInfoPistol
		{
			memoryPointCamera="eye";
			discreteDistance[]={25};
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
								"KOD_weapons\weapons\Glock17\data\barrel.rvmat",
								"KOD_weapons\weapons\Glock17\data\firemode.rvmat",
								"KOD_weapons\weapons\Glock17\data\handle_marksman.rvmat",
								"KOD_weapons\weapons\Glock17\data\handle_normal.rvmat",
								"KOD_weapons\weapons\Glock17\data\sightbase.rvmat",
								"KOD_weapons\weapons\Glock17\data\sights.rvmat",
								"KOD_weapons\weapons\Glock17\data\slide_marksman.rvmat",
								"KOD_weapons\weapons\Glock17\data\slide_standard.rvmat",
								"KOD_weapons\weapons\Glock17\data\trigger.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\Glock17\data\barrel.rvmat",
								"KOD_weapons\weapons\Glock17\data\firemode.rvmat",
								"KOD_weapons\weapons\Glock17\data\handle_marksman.rvmat",
								"KOD_weapons\weapons\Glock17\data\handle_normal.rvmat",
								"KOD_weapons\weapons\Glock17\data\sightbase.rvmat",
								"KOD_weapons\weapons\Glock17\data\sights.rvmat",
								"KOD_weapons\weapons\Glock17\data\slide_marksman.rvmat",
								"KOD_weapons\weapons\Glock17\data\slide_standard.rvmat",
								"KOD_weapons\weapons\Glock17\data\trigger.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\Glock17\data\barrel_damage.rvmat",
								"KOD_weapons\weapons\Glock17\data\firemode_damage.rvmat",
								"KOD_weapons\weapons\Glock17\data\handle_marksman_damage.rvmat",
								"KOD_weapons\weapons\Glock17\data\handle_normal_damage.rvmat",
								"KOD_weapons\weapons\Glock17\data\sightbase_damage.rvmat",
								"KOD_weapons\weapons\Glock17\data\sights_damage.rvmat",
								"KOD_weapons\weapons\Glock17\data\slide_marksman_damage.rvmat",
								"KOD_weapons\weapons\Glock17\data\slide_standard_damage.rvmat",
								"KOD_weapons\weapons\Glock17\data\trigger_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\Glock17\data\barrel_damage.rvmat",
								"KOD_weapons\weapons\Glock17\data\firemode_damage.rvmat",
								"KOD_weapons\weapons\Glock17\data\handle_marksman_damage.rvmat",
								"KOD_weapons\weapons\Glock17\data\handle_normal_damage.rvmat",
								"KOD_weapons\weapons\Glock17\data\sightbase_damage.rvmat",
								"KOD_weapons\weapons\Glock17\data\sights_damage.rvmat",
								"KOD_weapons\weapons\Glock17\data\slide_marksman_damage.rvmat",
								"KOD_weapons\weapons\Glock17\data\slide_standard_damage.rvmat",
								"KOD_weapons\weapons\Glock17\data\trigger_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\Glock17\data\barrel_destruct.rvmat",
								"KOD_weapons\weapons\Glock17\data\firemode_destruct.rvmat",
								"KOD_weapons\weapons\Glock17\data\handle_marksman_destruct.rvmat",
								"KOD_weapons\weapons\Glock17\data\handle_normal_destruct.rvmat",
								"KOD_weapons\weapons\Glock17\data\sightbase_destruct.rvmat",
								"KOD_weapons\weapons\Glock17\data\sights_destruct.rvmat",
								"KOD_weapons\weapons\Glock17\data\slide_marksman_destruct.rvmat",
								"KOD_weapons\weapons\Glock17\data\slide_standard_destruct.rvmat",
								"KOD_weapons\weapons\Glock17\data\trigger_destruct.rvmat"
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
	};
	class KOD_Glock17_Custom_Base: KOD_Glock17_Base
	{
		model="KOD_weapons\weapons\Glock17\glock17_custom.p3d";
		recoilModifier[]={0.85000002,0.85000002,0.85000002};
		swayModifier[]={1.12,1.12,0.82999998};
	};
	class KOD_Glock17_Custom_Black: KOD_Glock17_Custom_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\Glock17\data\handle_marksman_black_co.paa",
			"KOD_weapons\weapons\Glock17\data\slide_marksman_black_co.paa",
			"KOD_weapons\weapons\Glock17\data\sightbase_black_co.paa"
		};
	};
	class KOD_Glock17_Custom_Tan: KOD_Glock17_Custom_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\Glock17\data\handle_marksman_tan_co.paa",
			"KOD_weapons\weapons\Glock17\data\slide_marksman_tan_co.paa",
			"KOD_weapons\weapons\Glock17\data\sightbase_tan_co.paa"
		};
	};
	class KOD_Glock17_Custom_BlackTan: KOD_Glock17_Custom_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\Glock17\data\handle_marksman_black_co.paa",
			"KOD_weapons\weapons\Glock17\data\slide_marksman_tan_co.paa",
			"KOD_weapons\weapons\Glock17\data\sightbase_tan_co.paa"
		};
	};
	class KOD_Glock17_Custom_TanBlack: KOD_Glock17_Custom_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\Glock17\data\handle_marksman_tan_co.paa",
			"KOD_weapons\weapons\Glock17\data\slide_marksman_black_co.paa",
			"KOD_weapons\weapons\Glock17\data\sightbase_black_co.paa"
		};
	};
	class KOD_Glock17_Standard_Base: KOD_Glock17_Base
	{
		model="KOD_weapons\weapons\Glock17\glock17.p3d";
		recoilModifier[]={0.89999998,0.89999998,0.89999998};
		swayModifier[]={1.12,1.12,0.82999998};
	};
	class KOD_Glock17_Standard_Black: KOD_Glock17_Standard_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\Glock17\data\handle_normal_black_co.paa",
			"KOD_weapons\weapons\Glock17\data\slide_standard_black_co.paa",
			"KOD_weapons\weapons\Glock17\data\sightbase_black_co.paa"
		};
	};
	class KOD_Glock17_Standard_Tan: KOD_Glock17_Standard_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\Glock17\data\handle_normal_tan_co.paa",
			"KOD_weapons\weapons\Glock17\data\slide_standard_tan_co.paa",
			"KOD_weapons\weapons\Glock17\data\sightbase_tan_co.paa"
		};
	};
	class KOD_Glock17_Standard_BlackTan: KOD_Glock17_Standard_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\Glock17\data\handle_normal_black_co.paa",
			"KOD_weapons\weapons\Glock17\data\slide_standard_tan_co.paa",
			"KOD_weapons\weapons\Glock17\data\sightbase_tan_co.paa"
		};
	};
	class KOD_Glock17_Standard_TanBlack: KOD_Glock17_Standard_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\Glock17\data\handle_normal_tan_co.paa",
			"KOD_weapons\weapons\Glock17\data\slide_standard_black_co.paa",
			"KOD_weapons\weapons\Glock17\data\sightbase_black_co.paa"
		};
	};
};
class cfgMagazines
{
	class Magazine_Base;
	class Mag_KOD_Glock17_Black_16rnd: Magazine_Base
	{
		scope=2;
		displayName="Маг. Glock-17 на 16 пат.";
		descriptionShort="Съемный коробчатый магазин для пистолета Glock 17. Вмещает до 16 патронов калибра 9x19 мм.";
		model="KOD_weapons\weapons\Glock17\glock17_mag_s.p3d";
		weight=100;
		itemSize[]={1,2};
		count=16;
		ammo="Bullet_9x19";
		ammoItems[]=
		{
			"Ammo_9x19"
		};
		hiddenSelections[]=
		{
			"magazine"
		};
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\Glock17\data\mag_standard_black_co.paa"
		};
		tracersEvery=0;
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
								"KOD_weapons\weapons\Glock17\data\mag_standard.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\Glock17\data\mag_standard.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\Glock17\data\mag_standard_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\Glock17\data\mag_standard_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\Glock17\data\mag_standard_destruct.rvmat"
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
	class Mag_KOD_Glock17_Tan_16rnd: Mag_KOD_Glock17_Black_16rnd
	{
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\Glock17\data\mag_standard_tan_co.paa"
		};
	};
	class Mag_KOD_Glock17_Black_30rnd: Magazine_Base
	{
		scope=2;
		displayName="Маг. Glock-17 на 30 пат.";
		descriptionShort="Съемный коробчатый магазин для пистолета Glock 17. Вмещает до 30 патронов калибра 9x19 мм.";
		model="KOD_weapons\weapons\Glock17\glock17_mag_l.p3d";
		weight=200;
		itemSize[]={1,3};
		count=30;
		ammo="Bullet_9x19";
		ammoItems[]=
		{
			"Ammo_9x19"
		};
		hiddenSelections[]=
		{
			"magazine"
		};
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\Glock17\data\mag_large_black_co.paa"
		};
		tracersEvery=0;
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
								"KOD_weapons\weapons\Glock17\data\mag_large.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\Glock17\data\mag_large.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\Glock17\data\mag_large_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\Glock17\data\mag_large_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\Glock17\data\mag_large_destruct.rvmat"
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
	class Mag_KOD_Glock17_Tan_30rnd: Mag_KOD_Glock17_Black_30rnd
	{
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\Glock17\data\mag_large_tan_co.paa"
		};
	};
};
class cfgVehicles
{
	class ItemSuppressor;
	class KOD_Glock17_FlashHider: ItemSuppressor
	{
		scope=2;
		displayName="Пистолетный Пламегаситель";
		descriptionShort="Дульное устройство, уменьшающее вспышку при стрельбе.";
		model="KOD_weapons\weapons\Glock17\glock17_flashhider.p3d";
		rotationFlags=17;
		reversed=0;
		weight=70;
		itemSize[]={1,1};
		itemModelLength=0.039999999;
		barrelArmor=20.834;
		inventorySlot[]=
		{
			"pistolMuzzle"
		};
		soundIndex=0;
		muzzlePos="usti hlavne";
		class Particles
		{
			class OnFire
			{
				class MuzzleFlash
				{
					overrideParticle="weapon_shot_fnx_02";
					illuminateWorld=1;
				};
			};
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=40;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"DZ\weapons\attachments\data\gemtech.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"DZ\weapons\attachments\data\gemtech.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"DZ\weapons\attachments\data\gemtech_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"DZ\weapons\attachments\data\gemtech_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"DZ\weapons\attachments\data\gemtech_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_Glock17_Compensator: ItemSuppressor
	{
		scope=2;
		displayName="Пистолетный Компенсатор";
		descriptionShort="Дульное устройство, уменьшающее отдачу при стрельбе.";
		model="KOD_weapons\weapons\Glock17\glock17_compensator.p3d";
		rotationFlags=17;
		reversed=0;
		weight=70;
		itemSize[]={1,1};
		itemModelLength=0.039999999;
		barrelArmor=20.834;
		inventorySlot[]=
		{
			"pistolMuzzle"
		};
		soundIndex=0;
		muzzlePos="usti hlavne";
		recoilModifier[]={0.69999999,0.69999999,0.69999999};
		swayModifier[]={1,1,1};
		class Particles
		{
			class OnFire
			{
				class MuzzleFlash
				{
					overrideParticle="weapon_shot_fnx_01";
					illuminateWorld=1;
				};
			};
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=40;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"DZ\weapons\attachments\data\gemtech.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"DZ\weapons\attachments\data\gemtech.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"DZ\weapons\attachments\data\gemtech_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"DZ\weapons\attachments\data\gemtech_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"DZ\weapons\attachments\data\gemtech_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
};
class CfgNonAIVehicles
{
	class ProxyAttachment;
	class Proxyglock17_mag_l: ProxyAttachment
	{
		scope=0;
		inventorySlot="magazine";
		model="KOD_weapons\weapons\Glock17\glock17_mag_l.p3d";
	};
	class Proxyglock17_mag_s: ProxyAttachment
	{
		scope=0;
		inventorySlot="magazine";
		model="KOD_weapons\weapons\Glock17\glock17_mag_s.p3d";
	};
	class Proxyglock17_flashhider: ProxyAttachment
	{
		scope=0;
		inventorySlot="pistolMuzzle";
		model="KOD_weapons\weapons\Glock17\glock17_flashhider.p3d";
	};
	class Proxyglock17_compensator: ProxyAttachment
	{
		scope=0;
		inventorySlot="pistolMuzzle";
		model="KOD_weapons\weapons\Glock17\glock17_compensator.p3d";
	};
};
