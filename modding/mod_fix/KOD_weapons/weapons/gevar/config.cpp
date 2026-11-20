class CfgPatches
{
	class KOD_Weapons_Kivaari
	{
		units[]={};
		weapons[]=
		{
			"KOD_Kivaari_Grey",
			"KOD_Kivaari_Black",
			"KOD_Kivaari_Green",
			"KOD_Kivaari_Tan"
		};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Firearms",
			"DZ_Sounds_Effects",
			"DZ_Scripts"
		};
		magazines[]=
		{
			"Mag_KOD_Kivaari_10rnd"
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
	class KOD_Kivaari_Base: Rifle_Base
	{
		scope=0;
		displayName="Kivaari";
		descriptionShort="A semi-automatic precision rifle chambered for .338 Lapua Magnum.";
		model="KOD_weapons\weapons\Gevar\gevar.p3d";
		attachments[]=
		{
			"weaponWrap",
			"weaponOpticsHunting",
			"weaponMuzzleAWM",
			"KODRISBipod",
			"KODSticker1",
			"KODSticker2",
			"KODSticker3"
		};
		itemSize[]={9,3};
		weight=5500;
		absorbency=0;
		repairableWithKits[]={1};
		repairCosts[]={25};
		PPDOFProperties[]={1,0.5,20,170,4,10};
		WeaponLength=1.1;
		chamberSize=1;
		chamberedRound="";
		chamberableFrom[]=
		{
			"KOD_Ammo_338"
		};
		magazines[]=
		{
			"Mag_KOD_Kivaari_10rnd"
		};
		ejectType=1;
		initSpeedMultiplier=1.15;
		barrelArmor=3.8;
		recoilModifier[]={2.0999999,2.3,1.5};
		swayModifier[]={2.0999999,2.0999999,1.2};
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
			"SemiAuto"
		};
		class SemiAuto: Mode_SemiAuto
		{
			soundSetShot[]=
			{
				"GevarMk3_Shot_SoundSet",
				"Repeater_Tail_SoundSet",
				"Repeater_InteriorTail_SoundSet"
			};
			soundSetShotExt[]=
			{
				
				{
					"CR527_silencerHomeMade_SoundSet",
					"CR527_silencerHomeMadeTail_SoundSet",
					"CR527_silencerInteriorHomeMadeTail_SoundSet"
				}
			};
			reloadTime=0.125;
			dispersion=9.9999997e-05;
			magazineSlot="magazine";
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
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\Gevar\Textures\receiver.rvmat",
								"KOD_weapons\weapons\Gevar\Textures\grip.rvmat",
								"KOD_weapons\weapons\Gevar\Textures\stock.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\Gevar\Textures\receiver.rvmat",
								"KOD_weapons\weapons\Gevar\Textures\grip.rvmat",
								"KOD_weapons\weapons\Gevar\Textures\stock.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\Gevar\Textures\receiver_damage.rvmat",
								"KOD_weapons\weapons\Gevar\Textures\grip_damage.rvmat",
								"KOD_weapons\weapons\Gevar\Textures\stock_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\Gevar\Textures\receiver_damage.rvmat",
								"KOD_weapons\weapons\Gevar\Textures\grip_damage.rvmat",
								"KOD_weapons\weapons\Gevar\Textures\stock_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\Gevar\Textures\receiver_destruct.rvmat",
								"KOD_weapons\weapons\Gevar\Textures\grip_destruct.rvmat",
								"KOD_weapons\weapons\Gevar\Textures\stock_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_Kivaari_Grey: KOD_Kivaari_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\Gevar\Textures\receiver_grey_co.paa",
			"KOD_weapons\weapons\Gevar\Textures\grip_black_co.paa",
			"KOD_weapons\weapons\Gevar\Textures\stock_black_co.paa"
		};
	};
	class KOD_Kivaari_Black: KOD_Kivaari_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\Gevar\Textures\receiver_black_co.paa",
			"KOD_weapons\weapons\Gevar\Textures\grip_black_co.paa",
			"KOD_weapons\weapons\Gevar\Textures\stock_black_co.paa"
		};
	};
	class KOD_Kivaari_Green: KOD_Kivaari_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\Gevar\Textures\receiver_green_co.paa",
			"KOD_weapons\weapons\Gevar\Textures\grip_black_co.paa",
			"KOD_weapons\weapons\Gevar\Textures\stock_green_co.paa"
		};
	};
	class KOD_Kivaari_Tan: KOD_Kivaari_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\Gevar\Textures\receiver_tan_co.paa",
			"KOD_weapons\weapons\Gevar\Textures\grip_tan_co.paa",
			"KOD_weapons\weapons\Gevar\Textures\stock_tan_co.paa"
		};
	};
};
class cfgMagazines
{
	class Magazine_Base;
	class Mag_KOD_Kivaari_10rnd: Magazine_Base
	{
		scope=2;
		displayName="Kivaari 10rnd Mag";
		descriptionShort="This magazine holds up to 10 rounds of .338 LM.";
		model="KOD_weapons\weapons\Gevar\gevar_mag.p3d";
		weight=120;
		itemSize[]={2,2};
		count=10;
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
					hitpoints=100;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"KOD_weapons\weapons\Gevar\Textures\mag.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\Gevar\Textures\mag.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\Gevar\Textures\mag_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\Gevar\Textures\mag_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\Gevar\Textures\mag_destruct.rvmat"
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
	class GevarMk3_closeShot_SoundShader: base_closeShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\Gevar\Sounds\gevar_close",
				1
			}
		};
		volume=1;
	};
	class GevarMk3_midShot_SoundShader: base_midShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\Gevar\Sounds\gevar_mid",
				1
			}
		};
		volume=0.69999999;
	};
	class GevarMk3_distShot_SoundShader: base_distShot_SoundShader
	{
		samples[]=
		{
			
			{
				"KOD_weapons\weapons\Gevar\Sounds\gevar_far",
				1
			}
		};
		volume=0.40000001;
	};
};
class CfgSoundSets
{
	class Rifle_Shot_Base_SoundSet;
	class GevarMk3_Shot_SoundSet: Rifle_Shot_Base_SoundSet
	{
		soundShaders[]=
		{
			"GevarMk3_closeShot_SoundShader",
			"GevarMk3_midShot_SoundShader",
			"GevarMk3_distShot_SoundShader"
		};
	};
};
