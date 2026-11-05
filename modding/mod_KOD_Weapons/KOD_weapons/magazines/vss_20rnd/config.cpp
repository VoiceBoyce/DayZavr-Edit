class CfgPatches
{
	class KOD_Weapons_Mag_VSS_20Rnd
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Firearms_VSS",
			"DZ_Weapons_Magazines"
		};
		magazines[]=
		{
			"Mag_KOD_VSS_20rnd",
			"Mag_KOD_VSS_40rnd"
		};
	};
};
class cfgWeapons
{
	class Rifle_Base;
	class VSS_Base: Rifle_Base
	{
		magazines[]=
		{
			"Mag_VSS_10Rnd",
			"Mag_VAL_20Rnd",
			"Mag_Vikhr_30Rnd",
			"Mag_KOD_VSS_20rnd",
			"Mag_KOD_VSS_40rnd"
		};
	};
	class ASVAL: VSS_Base
	{
		magazines[]=
		{
			"Mag_VSS_10Rnd",
			"Mag_VAL_20Rnd",
			"Mag_Vikhr_30Rnd",
			"Mag_KOD_VSS_20rnd",
			"Mag_KOD_VSS_40rnd"
		};
	};
};
class cfgMagazines
{
	class Magazine_Base;
	class Mag_KOD_VSS_20rnd: Magazine_Base
	{
		scope=2;
		displayName="Магазин ВСС, 20 пат.";
		descriptionShort="Съемный коробчатый магазин для ВСС и АС ВАЛ. Вмещает до 20 патронов калибра 9x39 мм.";
		model="KOD_weapons\magazines\vss_20rnd\magVSS_20rnd.p3d";
		weight=200;
		itemSize[]={1,2};
		count=20;
		ammo="Bullet_9x39";
		ammoItems[]=
		{
			"Ammo_9x39",
			"Ammo_9x39AP"
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
								"KOD_weapons\magazines\vss_20rnd\data\dp_magVSS_20rnd.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\magazines\vss_20rnd\data\dp_magVSS_20rnd.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\magazines\vss_20rnd\data\dp_magVSS_20rnd_damaged.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\magazines\vss_20rnd\data\dp_magVSS_20rnd_damaged.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\magazines\vss_20rnd\data\dp_magVSS_20rnd_destruct.rvmat"
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
	class Mag_KOD_VSS_40rnd: Magazine_Base
	{
		scope=2;
		displayName="Спарка Магазинов ВСС, 40 пат.";
		descriptionShort="Два коробчатых магазина для ВСС и АС ВАЛ, скреплённые изолентой. В сумме вмещают 40 патронов калибра 9x39 мм.";
		model="KOD_weapons\magazines\vss_20rnd\magVSS_20rnd_x2.p3d";
		weight=400;
		itemSize[]={1,3};
		count=40;
		ammo="Bullet_9x39";
		ammoItems[]=
		{
			"Ammo_9x39",
			"Ammo_9x39AP"
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
								"KOD_weapons\magazines\vss_20rnd\data\dp_magVSS_20rnd.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"KOD_weapons\magazines\vss_20rnd\data\dp_magVSS_20rnd.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\magazines\vss_20rnd\data\dp_magVSS_20rnd_damaged.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"KOD_weapons\magazines\vss_20rnd\data\dp_magVSS_20rnd_damaged.rvmat"
							}
						},
						
						{
							0,
							
							{
								"KOD_weapons\magazines\vss_20rnd\data\dp_magVSS_20rnd_destruct.rvmat"
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
	class Proxydp_magVSS_20rnd: ProxyAttachment
	{
		scope=0;
		inventorySlot="magazine";
		model="KOD_weapons\magazines\vss_20rnd\magVSS_20rnd.p3d";
	};
	class Proxydp_magVSS_20rnd_x2: ProxyAttachment
	{
		scope=0;
		inventorySlot="magazine";
		model="KOD_weapons\magazines\vss_20rnd\magVSS_20rnd_x2.p3d";
	};
};
