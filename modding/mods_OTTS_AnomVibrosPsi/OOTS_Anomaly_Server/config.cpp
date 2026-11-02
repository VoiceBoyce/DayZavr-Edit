class CfgPatches
{
	class OOTS_Anomaly_Server
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","OOTS_Anomaly_Client"};
		ammo[] = {"BaseDamage","ElectraDmg","ZharkaDmg","StudenDmg","TramplinDmg","VoronkaDmg","StekloDmg","DeadZoneDmg","ZastoyDmg","SnowDmg","ParDmg","GrozaDmg","BlackHoleDmg","AtomDmg","LinceDmg","LinceAgrDmg","HoleraDmg","ZeroFakeDmg"};
	};
};
class CfgMods
{
	class OOTS_Anomaly_Server
	{
		author = "";
		type = "mod";
		dependencies[] = {"World","Mission"};
		class defs
		{
			class worldScriptModule
			{
				value = "";
				files[] = {"OOTS_Anomaly_Server/scripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"OOTS_Anomaly_Server/scripts/5_Mission"};
			};
		};
	};
};
class CfgAmmo
{
	class AnomalyBaseAmmo;
	class BaseDamage: AnomalyBaseAmmo
	{
		hit=2;
		indirectHit=0;
		indirectHitRange=0;
		model="shotShell";
		affectSkeleton=1;
		caliber=0.1;
		deflecting=0;
		class DamageApplied
		{
			type="Anomaly";
			class Health
			{
				damage=5;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=0;
			};
		};
		class NoiseHit
		{
			strength=10;
			type="sound";
		};
	};
	
	class ElectraDmg: BaseDamage
	{
		class DamageApplied
		{
			type="Electra";
			class Health
			{
				damage=150;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=1;
			};
		};
	};
	
	class FireShtormDmg: BaseDamage
	{
		class DamageApplied
		{
			type="FireShtorm";
			class Health
			{
				damage=100;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=1;
			};
		};
	};
	
	class LomkaDmg: BaseDamage
	{
		class DamageApplied
		{
			type="Lomka";
			class Health
			{
				damage=100;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=1;
			};
		};
	};
	
	class RoyDmg: BaseDamage
	{
		class DamageApplied
		{
			type="Roy";
			class Health
			{
				damage=100;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=1;
			};
		};
	};
	
	class FlashDmg: BaseDamage
	{
		class DamageApplied
		{
			type="Flash";
			class Health
			{
				damage=100;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=1;
			};
		};
	};
	
	class FireIskraDmg: BaseDamage
	{
		class DamageApplied
		{
			type="FireIskra";
			class Health
			{
				damage=100;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=1;
			};
		};
	};
	
	class MlechniPutDmg: BaseDamage
	{
		class DamageApplied
		{
			type="MlechniPut";
			class Health
			{
				damage=100;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=1;
			};
		};
	};
	
	class ZharkaDmg: BaseDamage
	{
		class DamageApplied
		{
			type="Zharka";
			class Health
			{
				damage=70;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=1;
			};
		};
	};
	
	class StudenDmg: BaseDamage
	{
		class DamageApplied
		{
			type="Studen";
			class Health
			{
				damage=80;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=1;
			};
		};
	};
	
	class TramplinDmg: BaseDamage
	{
		class DamageApplied
		{
			type="Tramplin";
			class Health
			{
				damage=85;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=0;
			};
		};
	};
	
	class VoronkaDmg: BaseDamage
	{
		class DamageApplied
		{
			type="Voronka";
			class Health
			{
				damage=200;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=1;
			};
		};
		class NoiseHit
		{
			strength=10;
			type="sound";
		};
	};
	
	class StekloDmg: BaseDamage
	{
		class DamageApplied
		{
			type="Steklo";
			class Health
			{
				damage=0;
			};
			class Blood
			{
				damage=500;
			};
			class Shock
			{
				damage=1;
			};
		};
	};
	
	class DeadZoneDmg: BaseDamage
	{
		class DamageApplied
		{
			type="DeadZone";
			class Health
			{
				damage=50;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=20;
			};
		};
	};
	
	class ZastoyDmg: BaseDamage
	{
		class DamageApplied
		{
			type="Zastoy";
			class Health
			{
				damage=90;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=50;
			};
		};
	};
	
	class SnowDmg: BaseDamage
	{
		class DamageApplied
		{
			type="Snow";
			class Health
			{
				damage=35;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=0;
			};
		};
	};
	
	class ParDmg: BaseDamage
	{
		class DamageApplied
		{
			type="Par";
			class Health
			{
				damage=70;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=40;
			};
		};
	};
	
	class GrozaDmg: BaseDamage
	{
		class DamageApplied
		{
			type="Groza";
			class Health
			{
				damage=70;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=30;
			};
		};
	};
	
	class BlackHoleDmg: BaseDamage
	{
		class DamageApplied
		{
			type="BlackHole";
			class Health
			{
				damage=30;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=0;
			};
		};
	};
	
	class AtomDmg: BaseDamage
	{
		class DamageApplied
		{
			type="Atom";
			class Health
			{
				damage=125;
			};
			class Blood
			{
				damage=2000;
			};
			class Shock
			{
				damage=25;
			};
		};
	};
	
	class LinceDmg: BaseDamage
	{
		class DamageApplied
		{
			type="Lince";
			class Health
			{
				damage=0;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=0;
			};
		};
	};
	
	class LinceAgrDmg: BaseDamage
	{
		class DamageApplied
		{
			type="LinceAgr";
			class Health
			{
				damage=0;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=40;
			};
		};
	};
	
	class HoleraDmg: BaseDamage
	{
		class DamageApplied
		{
			type="Holera";
			class Health
			{
				damage=15;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=0;
			};
		};
	};
	
	class EmpDmg: BaseDamage
	{
		class DamageApplied
		{
			type="EmpDmg";
			class Health
			{
				damage=0;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=15;
			};
		};
	};
	
	class Anomaly_FireShtorm: BaseDamage
	{
		class DamageApplied
		{
			type="FireShtorm";
			class Health
			{
				damage=0;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=15;
			};
		};
	};
	
	class Anomaly_Lomka: BaseDamage
	{
		class DamageApplied
		{
			type="Lomka";
			class Health
			{
				damage=0;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=15;
			};
		};
	};
	
	class Anomaly_Roy: BaseDamage
	{
		class DamageApplied
		{
			type="Roy";
			class Health
			{
				damage=0;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=15;
			};
		};
	};
	
	class Anomaly_Flash: BaseDamage
	{
		class DamageApplied
		{
			type="Flash";
			class Health
			{
				damage=0;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=15;
			};
		};
	};
	
	class Anomaly_FireIskra: BaseDamage
	{
		class DamageApplied
		{
			type="FireIskra";
			class Health
			{
				damage=0;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=15;
			};
		};
	};
	

	
	class Anomaly_MlechniPut: BaseDamage
	{
		class DamageApplied
		{
			type="MlechniPut";
			class Health
			{
				damage=0;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=15;
			};
		};
	};
	
	
	
	
	
	class ZeroFakeDmg: BaseDamage
	{
		class DamageApplied
		{
			type="ZeroFakeDmg";
			class Health
			{
				damage=0;
			};
			class Blood
			{
				damage=0;
			};
			class Shock
			{
				damage=0;
			};
		};
		class NoiseHit
		{
			strength=0;
			type="sound";
		};
	};
};
