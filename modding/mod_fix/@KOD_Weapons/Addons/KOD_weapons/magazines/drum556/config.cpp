class CfgPatches
{
	class KOD_Weapons_556DrumMag
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Weapons_Magazines"
		};
		magazines[]=
		{
			"Mag_CMAG_100Rnd_Black"
		};
	};
};
class cfgMagazines
{
	class Magazine_Base;
	class Mag_CMAG_30Rnd_Black;
	class Mag_CMAG_100Rnd_Black: Mag_CMAG_30Rnd_Black
	{
		scope=2;
		displayName="Магазин на 100 пат. 5,56x45 мм";
		descriptionShort="Барабанный магазин для патронов 5,56х45 мм. Вмещает 100 патронов.";
		model="KOD_weapons\magazines\Drum556\drum.p3d";
		weight=600;
		itemSize[]={4,3};
		count=100;
		ammo="Bullet_556x45";
		ammoItems[]=
		{
			"Ammo_556x45",
			"Ammo_556x45Tracer"
		};
		tracersEvery=0;
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
								"KOD_weapons\magazines\Drum556\data\mag.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\magazines\Drum556\data\mag.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\magazines\Drum556\data\mag_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\magazines\Drum556\data\mag_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\magazines\Drum556\data\mag_destruct.rvmat"
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
	class Proxydrum: ProxyAttachment
	{
		scope=0;
		inventorySlot="magazine";
		model="KOD_weapons\magazines\Drum556\drum.p3d";
	};
};
