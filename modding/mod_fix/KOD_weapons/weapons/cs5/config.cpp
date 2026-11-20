class CfgPatches
{
	class KOD_CS5
	{
		units[]={};
		weapons[]=
		{
			"KOD_CS5"
		};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Firearms",
			"DZ_Weapons_Magazines"
		};
		magazines[]=
		{
			"Mag_KOD_CS5_10rnd"
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
	class KOD_CS5_Base: BoltActionRifle_ExternalMagazine_Base
	{
		scope=0;
		displayName="McMillan CS5";
		descriptionShort="Снайперская винтовка НАТО США. Винтовка была специально разработана как мощное, компактное, малошумное и высокоточное оружие для скрытого использования армейскими и правоохранительными подразделениями на близком расстоянии, чаще всего в городских условиях. Питание осуществляется из отъёмных магазинов, патронник под патрон .308 Winchester.";
		model="KOD_weapons\weapons\CS5\cs5.p3d";
		attachments[]=
		{
			"weaponWrap",
			"weaponOptics",
			"weaponOpticsHunting",
			"KODSticker1",
			"KODSticker2",
			"KODSticker3"
		};
		itemSize[]={7,3};
		weight=5000;
		absorbency=0;
		repairableWithKits[]={1};
		repairCosts[]={30,25};
		PPDOFProperties[]={};
		WeaponLength=0.80000001;
		chamberSize=1;
		chamberedRound="";
		chamberableFrom[]=
		{
			"Ammo_308Win",
			"Ammo_308WinTracer",
			"KOD_Ammo_308WinAP"
		};
		magazines[]=
		{
			"Mag_KOD_CS5_10rnd"
		};
		barrelArmor=6.125;
		ejectType=0;
		initSpeedMultiplier=1;
		recoilModifier[]={1,1,1};
		swayModifier[]={1.5,1.5,1};
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
				"Trumpet_silencerHomeMade_SoundSet",
				"Trumpet_silencerHomeMadeTail_SoundSet",
				"Trumpet_silencerInteriorHomeMadeTail_SoundSet"
			};
			reloadTime=1;
			dispersion=0.00069999998;
			magazineSlot="magazine";
		};
		class NoiseShoot
		{
			strength=2;
			type="shot";
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
					overrideParticle="weapon_shot_vss_01";
				};
				class GasPistonBurst
				{
					overrideParticle="weapon_shot_chamber_smoke";
					overridePoint="GasPiston";
					overrideDirectionVector[]={-90,0,0};
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
								"KOD_weapons\weapons\CS5\data\CS5.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\CS5\data\CS5.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\CS5\data\CS5_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\CS5\data\CS5_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\CS5\data\CS5_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class KOD_CS5: KOD_CS5_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\weapons\CS5\data\CS5_co.paa"
		};
	};
};
class cfgMagazines
{
	class Magazine_Base;
	class Mag_KOD_CS5_10rnd: Magazine_Base
	{
		scope=2;
		displayName="Маг. CS-5 на 10 пат.";
		descriptionShort="Отъёмный магазин для винтовки CS-5. Вмещает до 10 патронов калибра .308.";
		model="KOD_weapons\weapons\CS5\cs5_mag.p3d";
		weight=100;
		itemSize[]={1,2};
		count=10;
		ammo="Bullet_308Win";
		ammoItems[]=
		{
			"Ammo_308Win",
			"Ammo_308WinTracer",
			"KOD_Ammo_308WinAP"
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
								"KOD_weapons\weapons\CS5\data\CS5.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\weapons\CS5\data\CS5.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\weapons\CS5\data\CS5_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\weapons\CS5\data\CS5_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\weapons\CS5\data\CS5_destruct.rvmat"
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
