class CfgPatches
{
	class KOD_Ammo_308AP
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Ammunition",
			"DZ_Weapons_Magazines",
			"DZ_Weapons_Firearms_B95",
			"DZ_Weapons_Firearms_CZ550",
			"DZ_Weapons_Firearms_FAL",
			"DZ_Pistols_Encore",
			"DZ_Weapons_Firearms_Winchester70"
		};
		magazines[]=
		{
			"KOD_Ammo_308WinAP"
		};
		ammo[]=
		{
			"KOD_Bullet_308Win"
		};
	};
};
class CfgMagazines
{
	class Magazine_Base;
	class Ammunition_Base;
	class KOD_Ammo_308WinAP: Ammunition_Base
	{
		scope=2;
		displayName="Патроны .308 Win AP";
		descriptionShort="Бронебойный винтовочный патрон .308 Win. Используется в некоторых винтовках.";
		model="\dz\weapons\ammunition\308Win_LooseRounds.p3d";
		iconCartridge=3;
		iconType=2;
		weight=10;
		count=20;
		ammo="KOD_Bullet_308Win";
		hiddenSelections[]=
		{
			"zbytek"
		};
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\ammo\308\data\308_loose_AP_co.paa"
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
								"DZ\weapons\ammunition\data\308_loose.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"DZ\weapons\ammunition\data\308_loose.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"DZ\weapons\ammunition\data\308_loose_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"DZ\weapons\ammunition\data\308_loose_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"DZ\weapons\ammunition\data\308_loose_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class Mag_FAL_20Rnd: Magazine_Base
	{
		ammoItems[]+=
		{
			"KOD_Ammo_308WinAP"
		};
	};
	class Mag_CZ550_4rnd: Magazine_Base
	{
		ammoItems[]+=
		{
			"KOD_Ammo_308WinAP"
		};
	};
	class Mag_CZ550_10rnd: Magazine_Base
	{
		ammoItems[]+=
		{
			"KOD_Ammo_308WinAP"
		};
	};
};
class Muzzle_Base;
class cfgWeapons
{
	class Pistol_Base;
	class Rifle_Base;
	class BoltActionRifle_ExternalMagazine_Base;
	class BoltActionRifle_InnerMagazine_Base;
	class B95_Base: Rifle_Base
	{
		chamberableFrom[]+=
		{
			"KOD_Ammo_308WinAP"
		};
		class SecondMuzzle: Muzzle_Base
		{
			chamberableFrom[]+=
			{
				"KOD_Ammo_308WinAP"
			};
		};
	};
	class CZ550_Base: BoltActionRifle_ExternalMagazine_Base
	{
		chamberableFrom[]+=
		{
			"KOD_Ammo_308WinAP"
		};
	};
	class FAL_Base: Rifle_Base
	{
		chamberableFrom[]+=
		{
			"KOD_Ammo_308WinAP"
		};
	};
	class Winchester70_Base: BoltActionRifle_InnerMagazine_Base
	{
		chamberableFrom[]+=
		{
			"KOD_Ammo_308WinAP"
		};
	};
	class LongHorn_Base: Pistol_Base
	{
		chamberableFrom[]+=
		{
			"KOD_Ammo_308WinAP"
		};
	};
};
class CfgAmmo
{
	class Bullet_Base;
	class KOD_Bullet_308Win: Bullet_Base
	{
		scope=1;
		casing="FxCartridge_762";
		round="FxRound_308Win";
		spawnPileType="KOD_Ammo_308WinAP";
		hit=12;
		indirectHit=0;
		indirectHitRange=0;
		airLock=1;
		initSpeed=770;
		typicalSpeed=940;
		tracerScale=1.2;
		tracerStartTime=-1;
		tracerEndTime=1;
		airFriction=-0.001;
		caliber=1;
		deflecting=10;
		damageBarrel=500;
		damageBarrelDestroyed=500;
		weight=0.0099999998;
		impactBehaviour=1;
		hitAnimation=1;
		unconRefillModifier=2.75;
		class DamageApplied
		{
			type="Projectile";
			dispersion=0;
			bleedThreshold=1;
			defaultDamageOverride[]=
			{
				{0.89999998,1}
			};
			class Health
			{
				damage=140;
				armorDamage=4;
			};
			class Blood
			{
				damage=100;
			};
			class Shock
			{
				damage=130;
			};
		};
		class NoiseHit
		{
			strength=10;
			type="shot";
		};
	};
};
class cfgAmmoTypes
{
	class AType_KOD_Bullet_308Win
	{
		name="KOD_Bullet_308Win";
	};
};
