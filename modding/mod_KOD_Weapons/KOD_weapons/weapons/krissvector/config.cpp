class CfgPatches
{
	class KOD_Weapons_Vector
	{
		units[]=
		{
			"KOD_Vector_Suppressor",
			"KOD_Vector_Handguard",
			"KOD_Vector_Buttstock"
		};
		weapons[]=
		{
			"KOD_Vector"
		};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Firearms",
			"DZ_Weapons_Magazines",
			"DZ_Weapons_Muzzles"
		};
		magazines[]=
		{
			"Mag_KOD_Vector_30rnd"
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
	class KOD_Vector_Base: Rifle_Base
	{
		scope=0;
		displayName="KRISS Vector";
		descriptionShort="KRISS Vector - пистолет-пулемёт, разработанный американской компанией KRISS USA. Использует патроны калибра .45 ACP.";
		model="KOD_weapons\weapons\KrissVector\vector.p3d";
		attachments[]=
		{
			"weaponButtstockVector",
			"weaponHandguardVector",
			"weaponWrap",
			"weaponOptics",
			"weaponMuzzleVector",
			"KODSticker1",
			"KODSticker2",
			"KODSticker3"
		};
		itemSize[]={6,3};
		weight=2500;
		absorbency=0;
		repairableWithKits[]={1};
		repairCosts[]={30,25};
		PPDOFProperties[]={};
		WeaponLength=0.583269;
		barrelArmor=4.6669998;
		chamberSize=1;
		chamberedRound="";
		chamberableFrom[]=
		{
			"Ammo_45ACP"
		};
		magazines[]=
		{
			"Mag_KOD_Vector_30rnd"
		};
		ejectType=1;
		recoilModifier[]={0.5,0.5,0.5};
		swayModifier[]={1.4,1.4,1.3};
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
				"UMP45_Shot_SoundSet",
				"UMP45_Tail_SoundSet",
				"UMP45_InteriorTail_SoundSet"
			};
			soundSetShotExt[]=
			{
				
				{
					"MP5K_1st_silencer_SoundSet",
					"MP5K_silencerTail_SoundSet",
					"MP5K_silencerInteriorTail_SoundSet"
				},
				
				{
					"MP5K_1st_silencerHomeMade_SoundSet",
					"MP5K_silencerHomeMadeTail_SoundSet",
					"MP5K_silencerInteriorHomeMadeTail_SoundSet"
				}
			};
			reloadTime=0.1;
			dispersion=0.003;
			magazineSlot="magazine";
		};
		class FullAuto: Mode_FullAuto
		{
			soundSetShot1st[]=
			{
				"UMP45_Shot_1st_SoundSet",
				"UMP45_Shot_1st_iterior_SoundSet"
			};
			soundSetShot[]=
			{
				"UMP45_Shot_SoundSet",
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
			reloadTime=0.040000001;
			dispersion=0.003;
			magazineSlot="magazine";
		};
		class Particles
		{
			class OnFire
			{
				class MuzzleFlash
				{
					overrideParticle="weapon_shot_mp5k_01";
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
				shotsToStartOverheating=10;
				maxOverheatingValue=30;
				overheatingDecayInterval=1;
				class SmokingBarrel
				{
					overrideParticle="smoking_barrel_small";
					onlyWithinOverheatLimits[]={0,0.80000001};
					onlyWithinRainLimits[]={0,0.2};
				};
				class SmokingBarrelHotSteam
				{
					overrideParticle="smoking_barrel_steam_small";
					positionOffset[]={0.1,0.02,0};
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
		class OpticsInfo: OpticsInfoRifle
		{
			memoryPointCamera="eye";
			modelOptics="-";
			distanceZoomMin=25;
			distanceZoomMax=25;
			discreteDistance[]={25};
			discreteDistanceInitIndex=0;
		};
		class NoiseShoot
		{
			strength=66;
			type="shot";
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
								"KOD_weapons\weapons\KrissVector\data\vector_body.rvmat",
								"KOD_weapons\weapons\KrissVector\data\vector_accs.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\KrissVector\data\vector_body.rvmat",
								"KOD_weapons\weapons\KrissVector\data\vector_accs.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\KrissVector\data\vector_body_damage.rvmat",
								"KOD_weapons\weapons\KrissVector\data\vector_accs_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\KrissVector\data\vector_body_damage.rvmat",
								"KOD_weapons\weapons\KrissVector\data\vector_accs_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\KrissVector\data\vector_body_destruct.rvmat",
								"KOD_weapons\weapons\KrissVector\data\vector_accs_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_Vector: KOD_Vector_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\KrissVector\data\vector_body_co.paa",
			"KOD_weapons\weapons\KrissVector\data\vector_accs_co.paa"
		};
	};
};
class cfgMagazines
{
	class Magazine_Base;
	class Mag_KOD_Vector_30rnd: Magazine_Base
	{
		scope=2;
		displayName="Магазин KRISS Vector, 30 пат.";
		descriptionShort="Магазин для KRISS Vector. Вмещает до 30 патронов калибра .45 ACP.";
		model="KOD_weapons\weapons\KrissVector\vector_mag.p3d";
		weight=50;
		itemSize[]={1,3};
		count=30;
		ammo="Bullet_45ACP";
		ammoItems[]=
		{
			"Ammo_45ACP"
		};
		hiddenSelections[]=
		{
			"magazine"
		};
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\KrissVector\data\vector_body_co.paa"
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
					hitpoints=220;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\KrissVector\data\vector_body.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\KrissVector\data\vector_body.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\KrissVector\data\vector_body_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\KrissVector\data\vector_body_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\KrissVector\data\vector_body_destruct.rvmat"
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
	class Inventory_Base;
	class KOD_Vector_Suppressor: ItemSuppressor
	{
		scope=2;
		displayName="KRISS Vector Suppressor";
		descriptionShort="KRISS Vector Suppressor.";
		model="KOD_weapons\weapons\KrissVector\vector_suppressor.p3d";
		rotationFlags=17;
		reversed=0;
		weight=200;
		itemSize[]={2,1};
		itemModelLength=0.25;
		inventorySlot[]=
		{
			"weaponMuzzleVector"
		};
		simulation="ItemSuppressor";
		dispersionModifier=-9.9999997e-05;
		noiseShootModifier=-0.94999999;
		dispersionCondition="true";
		barrelArmor=2500;
		recoilModifier[]={0.94999999,0.94999999,0.94999999};
		swayModifier[]={1.05,1.05,1};
		soundIndex=1;
		muzzlePos="usti hlavne";
		hiddenSelections[]=
		{
			"color"
		};
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\KrissVector\data\vector_accs_co.paa"
		};
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
					hitpoints=4;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\KrissVector\data\vector_accs.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\KrissVector\data\vector_accs.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\KrissVector\data\vector_accs_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\KrissVector\data\vector_accs_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\KrissVector\data\vector_accs_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_Vector_Handguard: Inventory_Base
	{
		scope=2;
		displayName="Цевьё KRISS Vector";
		descriptionShort="Цевьё c вертикальной рукояткой для KRISS Vector.";
		model="KOD_weapons\weapons\KrissVector\vector_hndgrd.p3d";
		rotationFlags=17;
		reversed=0;
		weight=250;
		itemSize[]={2,1};
		inventorySlot[]=
		{
			"weaponHandguardVector"
		};
		recoilModifier[]={0.80000001,0.80000001,0.80000001};
		swayModifier[]={0.69999999,0.69999999,0.69999999};
		hiddenSelections[]=
		{
			"color"
		};
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\KrissVector\data\vector_accs_co.paa"
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
								"KOD_weapons\weapons\KrissVector\data\vector_accs.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\KrissVector\data\vector_accs.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\KrissVector\data\vector_accs_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\KrissVector\data\vector_accs_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\KrissVector\data\vector_accs_destruct.rvmat"
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
	class KOD_Vector_Buttstock: Inventory_Base
	{
		scope=2;
		displayName="Приклад KRISS Vector";
		descriptionShort="Приклад для KRISS Vector.";
		model="KOD_weapons\weapons\KrissVector\vector_bttstck.p3d";
		rotationFlags=17;
		reversed=0;
		inventorySlot[]=
		{
			"weaponButtstockVector"
		};
		weight=354;
		itemSize[]={2,1};
		recoilModifier[]={0.80000001,0.80000001,0.80000001};
		swayModifier[]={0.69999999,0.69999999,0.69999999};
		hiddenSelections[]=
		{
			"color"
		};
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\KrissVector\data\vector_accs_co.paa"
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
								"KOD_weapons\weapons\KrissVector\data\vector_accs.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\KrissVector\data\vector_accs.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\KrissVector\data\vector_accs_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\KrissVector\data\vector_accs_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\KrissVector\data\vector_accs_destruct.rvmat"
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
	class ProxyVector_suppressor: ProxyAttachment
	{
		scope=0;
		inventorySlot="weaponMuzzleVector";
		model="KOD_weapons\weapons\KrissVector\vector_suppressor.p3d";
	};
	class ProxyVector_mag: ProxyAttachment
	{
		scope=0;
		inventorySlot="magazine";
		model="KOD_weapons\weapons\KrissVector\vector_mag.p3d";
	};
	class ProxyVector_hndgrd: ProxyAttachment
	{
		scope=0;
		inventorySlot="weaponHandguardVector";
		model="KOD_weapons\weapons\KrissVector\Vector_hndgrd.p3d";
	};
	class ProxyVector_bttstck: ProxyAttachment
	{
		scope=0;
		inventorySlot="weaponButtstockVector";
		model="KOD_weapons\weapons\KrissVector\Vector_bttstck.p3d";
	};
};
class CfgSlots
{
	class Slot_weaponMuzzleVector
	{
		name="weaponMuzzleVector";
		displayName="KRISS Vector Suppressor";
		ghostIcon="supressor";
	};
	class Slot_weaponButtstockVector
	{
		name="weaponButtstockVector";
		displayName="KRISS Vector Buttstock";
		ghostIcon="buttstock";
	};
	class Slot_weaponHandguardVector
	{
		name="weaponHandguardVector";
		displayName="KRISS Vector Handguard";
		ghostIcon="handguard";
	};
};

class CfgSoundShaders {
	class Glock19_silencerCloseShot_SoundShader;	
	class Glock19_silencerMidShot_SoundShader;	
	class Glock19_silencerDistShot_SoundShader;	
	class Glock19_Closure_SoundShader;
	
	class DZVR_pistol_silencerCloseShot_SoundShader : Glock19_silencerCloseShot_SoundShader 
	{
		samples[] = {{"KOD_weapons\weapons\sounds\shoot_sil_pistol", 1}};
		volume = 0.5;
		range=25;
	};
	
	class DZVR_pistol_silencerMidShot_SoundShader : Glock19_silencerMidShot_SoundShader 
	{
		samples[] = {
			{"KOD_weapons\weapons\sounds\shoot_sil_pistol", 1}
			};
		volume = 0.4;
		range=50;
		rangeCurve[] = {{0,0.1},{15,1},{25,0},{50,0}};
	};
	
	class DZVR_pistol_silencerDistShot_SoundShader : Glock19_silencerDistShot_SoundShader 
	{
		samples[] = {{"KOD_weapons\weapons\sounds\shoot_sil_pistol", 0.0}};
		volume = 0.0;
		range=70;
	};
	class DZVR_pistol_Closure_SoundShader : Glock19_Closure_SoundShader 
	{
		samples[] = {{"KOD_weapons\weapons\sounds\shoot_sil_pistol", 1}};
		range=5;
	};
};

class CfgSoundSets {
	class Glock19_silencerPro_SoundSet;	// External class reference
	
	class DZVR_pistol_silencerShot_SoundSet : Glock19_silencerPro_SoundSet 
	{
		soundShaders[] = {"DZVR_pistol_closeShot_SoundShader", "DZVR_pistol_midShot_SoundShader", "DZVR_pistol_distShot_SoundShader", "DZVR_pistol_Closure_SoundShader"};
		sound3DProcessingType="infected3DProcessingType";
		volumeCurve="infectedAttenuationCurve";
		spatial=1;
		doppler=0;
		loop=0;
	};
	
};


