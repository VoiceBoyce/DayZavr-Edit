class CfgPatches
{
	class KOD_Weapons_Ammo_762x25
	{
		units[]=
		{
			"KOD_AmmoBox_762x25_50rnd"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Projectiles"
		};
		magazines[]=
		{
			"KOD_Ammo_762x25"
		};
		ammo[]=
		{
			"KOD_Bullet_762x25"
		};
	};
};
class cfgAmmoTypes
{
	class AType_KOD_Bullet_762x25
	{
		name="KOD_Bullet_762x25";
	};
};
class CfgAmmo
{
	class Bullet_Base;
	class KOD_Bullet_762x25: Bullet_Base
	{
		scope=2;
		casing="FxCartridge_9mm";
		round="FxRound_9mm";
		spawnPileType="KOD_Ammo_762x25";
		hit=7.5;
		indirectHit=0;
		indirectHitRange=0;
		tracerScale=1;
		muzzleFlashParticle="weapon_shot_mp5k_02_boris";
		caliber=0.85000002;
		deflecting=32;
		visibleFire=3.5;
		audibleFire=5;
		typicalSpeed=320;
		airFriction=-0.0016;
		supersonicCrackNear[]={};
		supersonicCrackFar[]={};
		initSpeed=330;
		weight=0.0060000001;
		class DamageApplied
		{
			type="Projectile";
			dispersion=0;
			bleedThreshold=1;
			class Health
			{
				damage=60;
			};
			class Blood
			{
				damage=120;
			};
			class Shock
			{
				damage=20;
			};
		};
		class NoiseHit
		{
			strength=40;
			type="shot";
		};
	};
};
class CfgVehicles
{
	class Box_Base;
	class KOD_AmmoBox_762x25_50rnd: Box_Base
	{
		scope=2;
		displayName="762x25 50 Rounds";
		descriptionShort="Коробка патронов калибра 7.62х25  50шт.";
		model="DZ\weapons\ammunition\9mm_25rnd_box.p3d";
		hiddenSelections[]=
		{
			"zbytek"
		};
		hiddenSelectionsTextures[]=
		{
			"KOD_weapons\ammo\762x25\data\762x25_box_co.paa"
		};
		rotationFlags=17;
		weight=100;
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
								"DZ\weapons\ammunition\data\9mm_25roundbox.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"DZ\weapons\ammunition\data\9mm_25roundbox.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"DZ\weapons\ammunition\data\9mm_25roundbox_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"DZ\weapons\ammunition\data\9mm_25roundbox_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"DZ\weapons\ammunition\data\9mm_25roundbox_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		class Resources
		{
			class KOD_Ammo_762x25
			{
				value=50;
				variable="quantity";
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class interact
				{
					soundset="ammoboxUnpack_SoundSet";
					id=70;
				};
			};
		};
	};
};
class CfgMagazines
{
	class Ammunition_Base;
	class KOD_Ammo_762x25: Ammunition_Base
	{
		scope=2;
		displayName="7,62×25 мм ТТ";
		descriptionShort="Советский пистолетный унитарный патрон с полуфланцевой гильзой бутылочной формы, разработанный на базе немецкого патрона 7,63×25 мм Маузер.";
		model="KOD_weapons\ammo\762x25\762x25.p3d";
		iconCartridge=3;
		weight=5;
		count=50;
		ammo="KOD_Bullet_762x25";
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
								"KOD_weapons\ammo\762x25\data\762x25_mat.rvmat"
							}
						},
						
						{
							0.69999999,
							{}
						},
						
						{
							0.5,
							
							{
								"KOD_weapons\ammo\762x25\data\762x25_mat_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							{}
						},
						
						{
							0,
							
							{
								"KOD_weapons\ammo\762x25\data\762x25_mat_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
};
