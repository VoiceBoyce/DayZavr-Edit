class CfgPatches
{
	class KOD_Weapons_308DrumMag
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Magazines",
			"DZ_Weapons_Firearms_FAL"
		};
		magazines[]=
		{
			"Mag_KOD_Drum308_50rnd"
		};
	};
};
class cfgWeapons
{
	class Rifle_Base;
	class FAL_Base: Rifle_Base
	{
		magazines[]=
		{
			"Mag_FAL_20Rnd",
			"Mag_KOD_Drum308_50rnd",
			"Mag_KOD_SA58_20Rnd"
		};
	};
	class FAL: FAL_Base
	{
		magazines[]=
		{
			"Mag_FAL_20Rnd",
			"Mag_KOD_Drum308_50rnd",
			"Mag_KOD_SA58_20Rnd"
		};
	};
};
class cfgMagazines
{
	class Magazine_Base;
	class Mag_KOD_Drum308_50rnd: Magazine_Base
	{
		scope=2;
		displayName="Барабанный магазин на 50 пат.";
		descriptionShort="$STR_Mag_KOD_Drum308_50Rnd";
		model="KOD_weapons\magazines\Drum308\mag.p3d";
		weight=400;
		itemSize[]={2,3};
		count=50;
		ammo="Bullet_308Win";
		ammoItems[]=
		{
			"Ammo_308Win",
			"Ammo_308WinTracer",
			"KOD_Ammo_308WinAP"
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
								"KOD_weapons\magazines\Drum308\data\mag.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\magazines\Drum308\data\mag.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\magazines\Drum308\data\mag_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\magazines\Drum308\data\mag_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\magazines\Drum308\data\mag_destruct.rvmat"
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
class CfgNonAIVehicles
{
	class ProxyAttachment;
	class Proxymag: ProxyAttachment
	{
		scope=0;
		inventorySlot="magazine";
		model="KOD_weapons\magazines\Drum308\mag.p3d";
	};
};
