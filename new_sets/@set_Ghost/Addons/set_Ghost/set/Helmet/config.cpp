class CfgPatches
{
	class Vp_Set11
	{
		units[]=
		{
			"Vp_Set14_Helmet"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters",
			"DZ_Scripts"
		};
	};
};
class cfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class Vp_Set14_Helmet_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_Ghost_Helmet";
		descriptionShort="";
		model="set_Ghost\set\Helmet\model\Helmet_g.p3d";
		attachments[]=
		{
			"NVG"
		};
		inventorySlot[]=
		{
			"Headgear"
		};
		simulation="clothing";
		vehicleClass="Clothing";
		itemInfo[]=
		{
			"Clothing",
			"Helmet"
		};
		ragQuantity=6;
		quickBarBonus=2;
		weight=310;
		itemSize[]={4,4};
		varWetMax=0.050000001;
		heatIsolation=1;
		repairableWithKits[]={3,8};
		repairCosts[]={25};
		hiddenSelections[]=
		{
			"AKB",
			"ebalo",
			"Fast_helmet",
			"Flag",
			"Helmet",
			"Mask",
			"Meshok",
			"Mikro_naushniki",
			"Naushniki",
			"PNV",
			"Truba",
			"zalypa",
			"zalypa2_helmet",
			"Zaplatka"
		};
		headSelectionsToHide[]=
		{
			"Clipping_Balaclava"
		};
		class ClothingTypes
		{
			male="set_Ghost\set\Helmet\model\Helmet_mf.p3d";
			female="set_Ghost\set\Helmet\model\Helmet_mf.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=500;
					transferToAttachmentsCoef=0.5;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"set_Ghost\set\Helmet\data\Akb_helmet.rvmat",
								"set_Ghost\set\Helmet\data\Ebalo.rvmat",
								"set_Ghost\set\Helmet\data\Fast_helmet.rvmat",
								"set_Ghost\set\Helmet\data\Flag_helmet.rvmat",
								"set_Ghost\set\Helmet\data\Helmet.rvmat",
								"set_Ghost\set\Helmet\data\Mask.rvmat",
								"set_Ghost\set\Helmet\data\Meshok_helmet.rvmat",
								"set_Ghost\set\Helmet\data\Mikro_helmet.rvmat",
								"set_Ghost\set\Helmet\data\Haushnik.rvmat",
								"set_Ghost\set\Helmet\data\PNV_kper.rvmat",
								"set_Ghost\set\Helmet\data\Truba_helmet.rvmat",
								"set_Ghost\set\Helmet\data\Zalypa_Helmet.rvmat",
								"set_Ghost\set\Helmet\data\Zalypa2_helmet.rvmat",
								"set_Ghost\set\Helmet\data\Zapladka_helmet.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"set_Ghost\set\Helmet\data\Akb_helmet.rvmat",
								"set_Ghost\set\Helmet\data\Ebalo.rvmat",
								"set_Ghost\set\Helmet\data\Fast_helmet.rvmat",
								"set_Ghost\set\Helmet\data\Flag_helmet.rvmat",
								"set_Ghost\set\Helmet\data\Helmet.rvmat",
								"set_Ghost\set\Helmet\data\Mask.rvmat",
								"set_Ghost\set\Helmet\data\Meshok_helmet.rvmat",
								"set_Ghost\set\Helmet\data\Mikro_helmet.rvmat",
								"set_Ghost\set\Helmet\data\Haushnik.rvmat",
								"set_Ghost\set\Helmet\data\PNV_kper.rvmat",
								"set_Ghost\set\Helmet\data\Truba_helmet.rvmat",
								"set_Ghost\set\Helmet\data\Zalypa_Helmet.rvmat",
								"set_Ghost\set\Helmet\data\Zalypa2_helmet.rvmat",
								"set_Ghost\set\Helmet\data\Zapladka_helmet.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"set_Ghost\set\Helmet\data\Akb_helmet_damage.rvmat",
								"set_Ghost\set\Helmet\data\Ebalo_damage.rvmat",
								"set_Ghost\set\Helmet\data\Fast_helmet_damage.rvmat",
								"set_Ghost\set\Helmet\data\Flag_helmet_damage.rvmat",
								"set_Ghost\set\Helmet\data\Helmet_damage.rvmat",
								"set_Ghost\set\Helmet\data\Mask_damage.rvmat",
								"set_Ghost\set\Helmet\data\Meshok_helmet_damage.rvmat",
								"set_Ghost\set\Helmet\data\Haushnik_damage.rvmat",
								"set_Ghost\set\Helmet\data\Mikro_helmet_damage.rvmat",
								"set_Ghost\set\Helmet\data\PNV_kper_damage.rvmat",
								"set_Ghost\set\Helmet\data\Truba_helmet_damage.rvmat",
								"set_Ghost\set\Helmet\data\Zalypa_Helmet_damage.rvmat",
								"set_Ghost\set\Helmet\data\Zalypa2_helmet_damage.rvmat",
								"set_Ghost\set\Helmet\data\Zapladka_helmet_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"set_Ghost\set\Helmet\data\Akb_helmet_damage.rvmat",
								"set_Ghost\set\Helmet\data\Ebalo_damage.rvmat",
								"set_Ghost\set\Helmet\data\Fast_helmet_damage.rvmat",
								"set_Ghost\set\Helmet\data\Flag_helmet_damage.rvmat",
								"set_Ghost\set\Helmet\data\Haushnik_damage.rvmat",
								"set_Ghost\set\Helmet\data\Helmet_damage.rvmat",
								"set_Ghost\set\Helmet\data\Mask_damage.rvmat",
								"set_Ghost\set\Helmet\data\Meshok_helmet_damage.rvmat",
								"set_Ghost\set\Helmet\data\Haushnik_damage.rvmat",
								"set_Ghost\set\Helmet\data\Mikro_helmet_damage.rvmat",
								"set_Ghost\set\Helmet\data\PNV_kper_damage.rvmat",
								"set_Ghost\set\Helmet\data\Truba_helmet_damage.rvmat",
								"set_Ghost\set\Helmet\data\Zalypa_Helmet_damage.rvmat",
								"set_Ghost\set\Helmet\data\Zalypa2_helmet_damage.rvmat",
								"set_Ghost\set\Helmet\data\Zapladka_helmet_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"set_Ghost\set\Helmet\data\Akb_helmet_destruct.rvmat",
								"set_Ghost\set\Helmet\data\Ebalo_destruct.rvmat",
								"set_Ghost\set\Helmet\data\Fast_helmet_destruct.rvmat",
								"set_Ghost\set\Helmet\data\Flag_helmet_destruct.rvmat",
								"set_Ghost\set\Helmet\data\Helmet_destruct.rvmat",
								"set_Ghost\set\Helmet\data\Mask_destruct.rvmat",
								"set_Ghost\set\Helmet\data\Meshok_helmet_destruct.rvmat",
								"set_Ghost\set\Helmet\data\Mikro_helmet_destruct.rvmat",
								"set_Ghost\set\Helmet\data\Haushnik_destruct.rvmat",
								"set_Ghost\set\Helmet\data\PNV_kper_destruct.rvmat",
								"set_Ghost\set\Helmet\data\Truba_helmet_destruct.rvmat",
								"set_Ghost\set\Helmet\data\Zalypa_Helmet_destruct.rvmat",
								"set_Ghost\set\Helmet\data\Zalypa2_helmet_destruct.rvmat",
								"set_Ghost\set\Helmet\data\Zapladka_helmet_destruct.rvmat"
							}
						}
					};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage=0.5;
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
				class Melee
				{
					class Health
					{
						damage=0.5;
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
				class Infected
				{
					class Health
					{
						damage=0.5;
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
				class FragGrenade
				{
					class Health
					{
						damage=0.5;
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
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem
				{
					soundSet="Shirt_pickup_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="Shirt_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class Vp_Set14_Helmet: Vp_Set14_Helmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"set_Ghost\set\Helmet\data\Akb_helmet_co.paa",
			"set_Ghost\set\Helmet\data\Ebalo_co.paa",
			"set_Ghost\set\Helmet\data\Fast_helmet_co.paa",
			"set_Ghost\set\Helmet\data\Flag_helmet_co.paa",
			"set_Ghost\set\Helmet\data\Helmet_co.paa",
			"set_Ghost\set\Helmet\data\Mask_co.paa",
			"set_Ghost\set\Helmet\data\Meshok_helmet_co.paa",
			"set_Ghost\set\Helmet\data\Mikro_helmet_co.paa",
			"set_Ghost\set\Helmet\data\Naushnik_co.paa",
			"set_Ghost\set\Helmet\data\PNV_kper_co.paa",
			"set_Ghost\set\Helmet\data\Truba_helmet_co.paa",
			"set_Ghost\set\Helmet\data\Zalypa_Helmet_co.paa",
			"set_Ghost\set\Helmet\data\Zalypa2_helmet_co.paa",
			"set_Ghost\set\Helmet\data\Zapladka_helmet_co.paa"
		};
	};
};
