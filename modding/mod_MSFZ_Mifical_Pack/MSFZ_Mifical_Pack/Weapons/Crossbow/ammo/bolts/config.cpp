class CfgPatches
{
	class MSFZ_Crossbow_Bolt_Projectiles_Ammo
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Weapons_Ammunition","DZ_Weapons_Projectiles"};
		ammo[] = {"MSFZ_Crossbow_Bolt","MSFZ_Crossbow_Bolt_AP","MSFZ_Crossbow_CupidsBolt"};
	};
};
class cfgAmmoTypes
{
	class AType_MSFZ_Crossbow_Bolt
	{
		name = "MSFZ_Crossbow_Bolt";
	};
	class AType_MSFZ_Crossbow_Bolt_AP
	{
		name = "MSFZ_Crossbow_Bolt_AP";
	};
	class AType_MSFZ_Crossbow_CupidsBolt
	{
		name = "MSFZ_Crossbow_CupidsBolt";
	};
};
class CfgAmmo
{
	class Bullet_Base;
	class DefaultAmmo;
	class Bullet_CupidsBolt;
	class MSFZ_Crossbow_Bolt: Bullet_Base
	{
		scope = 1;
		round = "FxRound_crossbow_arrow";
		model = "MSFZ_Mifical_Pack\Weapons\Crossbow\ammo\MSFZ_Crossbow_Bolt.p3d";
		casing = "FxCartridge_762";
		spawnPileType = "MSFZ_Crossbow_BoltStack";
		hit = 11;
		indirectHit = 0;
		indirectHitRange = 0;
		tracerScale = 1;
		caliber = 1.2;
		deflecting = 15;
		typicalSpeed = 320;
		airFriction = -0.0015;
		initSpeed = 320;
		damageBarrel = 250;
		damageBarrelDestroyed = 250;
		weight = 0.018;
		supersonicCrackNear[] = {};
		supersonicCrackFar[] = {};
		impactBehaviour = 0;
		hitAnimation = 1;
		unconRefillModifier = 4;
		class DamageApplied
		{
			type = "Projectile";
			dispersion = 0;
			bleedThreshold = 1;
			class Health
			{
				damage = 1000;
				armorDamage = 5;
			};
			class Blood
			{
				damage = 1000;
			};
			class Shock
			{
				damage = 550;
			};
		};
		class NoiseHit
		{
			strength = 1;
			type = "shot";
		};
	};
	class MSFZ_Crossbow_Bolt_AP: MSFZ_Crossbow_Bolt
	{
		scope = 1;
		round = "FxRound_crossbow_arrow";
		model = "MSFZ_Mifical_Pack\Weapons\Crossbow\ammo\MSFZ_Crossbow_Bolt.p3d";
		casing = "FxCartridge_762";
		spawnPileType = "MSFZ_Crossbow_BoltStack_AP";
		hit = 11;
		indirectHit = 0;
		indirectHitRange = 0;
		tracerScale = 1;
		caliber = 1.2;
		deflecting = 15;
		typicalSpeed = 320;
		airFriction = -0.0015;
		initSpeed = 320;
		damageBarrel = 250;
		damageBarrelDestroyed = 250;
		weight = 0.018;
		supersonicCrackNear[] = {};
		supersonicCrackFar[] = {};
		impactBehaviour = 0;
		hitAnimation = 1;
		unconRefillModifier = 4;
		class DamageApplied
		{
			type = "Projectile";
			dispersion = 0;
			bleedThreshold = 1;
			class Health
			{
				damage = 2000;
				armorDamage = 5;
			};
			class Blood
			{
				damage = 2000;
			};
			class Shock
			{
				damage = 1100;
			};
		};
		class NoiseHit
		{
			strength = 1;
			type = "shot";
		};
	};
	class MSFZ_Crossbow_CupidsBolt: Bullet_CupidsBolt
	{
		scope = 1;
		model = "\dz\weapons\projectiles\bolt_flying.p3d";
		proxyShape = "\dz\weapons\projectiles\bolt_cupids.p3d";
		simulation = "shotIlluminating";
		hit = 11;
		indirectHit = 0;
		indirectHitRange = 0;
		tracerScale = 1;
		caliber = 1.2;
		deflecting = 15;
		typicalSpeed = 320;
		airFriction = -0.0015;
		initSpeed = 320;
		damageBarrel = 250;
		damageBarrelDestroyed = 250;
		weight = 0.018;
		supersonicCrackNear[] = {};
		supersonicCrackFar[] = {};
		impactBehaviour = 0;
		hitAnimation = 1;
		unconRefillModifier = 4;
		round = "FxRound_CupidsBolt";
		dispersion = 0.0065;
		SimulationScriptClass = "CupidsBoltSimulation";
		spawnPileType = "MSFZ_Ammo_CupidsBolt";
		dmgPerUse = 1;
		class DamageApplied
		{
			type = "Projectile";
			dispersion = 0;
			bleedThreshold = 0;
			class Health
			{
				damage = 0;
			};
			class Blood
			{
				damage = 0;
			};
			class Shock
			{
				damage = 0;
			};
		};
	};
};
class CfgVehicles
{
	class FxCartridge;
	class FxRound;
	class FxRound_crossbow_arrow: FxRound
	{
		model = "MSFZ_Mifical_Pack\Weapons\Crossbow\ammo\MSFZ_Crossbow_Bolt.p3d";
	};
};
