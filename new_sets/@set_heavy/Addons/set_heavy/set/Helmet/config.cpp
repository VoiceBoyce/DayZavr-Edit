class CfgPatches
{
	class Vp_Set16
	{
		units[]=
		{
			"Vp_Set16_Helmet_Black"
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
	class Vp_Set16_Helmet_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_Heavy_Helmet";
		descriptionShort="";
		model="set_heavy\set\Helmet\model\helmet_g.p3d";
		attachments[]=
		{
			"NVG",
			"helmetFlashlight"
		};
		inventorySlot[]=
		{
			"Headgear",
			"Helmet"
		};
		simulation="clothing";
		vehicleClass="Clothing";
		itemInfo[]=
		{
			"Clothing",
			"Headgear"
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
			"helmet",
			"kper_pnv",
			"krep_fanar",
			"mask",
			"ochki",
			"tkan",
			"ushi",
			"vizor",
			"zad_meshok"
		};
		headSelectionsToHide[]=
		{
			"Clipping_Balaclava"
		};
		class ClothingTypes
		{
			male="set_heavy\set\Helmet\model\helmet_mf.p3d";
			female="set_heavy\set\Helmet\model\helmet_mf.p3d";
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
								"set_heavy\set\Helmet\data\helmet.rvmat",
								"set_heavy\set\Helmet\data\krep_pnv.rvmat",
								"set_heavy\set\Helmet\data\krep_fanar.rvmat",
								"set_heavy\set\Helmet\data\mask.rvmat",
								"set_heavy\set\Helmet\data\ochki.rvmat",
								"set_heavy\set\Helmet\data\tkan.rvmat",
								"set_heavy\set\Helmet\data\ushi.rvmat",
								"set_heavy\set\Helmet\data\vizor.rvmat",
								"set_heavy\set\Helmet\data\zad.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"set_heavy\set\Helmet\data\helmet.rvmat",
								"set_heavy\set\Helmet\data\krep_pnv.rvmat",
								"set_heavy\set\Helmet\data\krep_fanar.rvmat",
								"set_heavy\set\Helmet\data\mask.rvmat",
								"set_heavy\set\Helmet\data\ochki.rvmat",
								"set_heavy\set\Helmet\data\tkan.rvmat",
								"set_heavy\set\Helmet\data\ushi.rvmat",
								"set_heavy\set\Helmet\data\vizor.rvmat",
								"set_heavy\set\Helmet\data\zad.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"set_heavy\set\Helmet\data\helmet_damage.rvmat",
								"set_heavy\set\Helmet\data\krep_pnv_damage.rvmat",
								"set_heavy\set\Helmet\data\krep_fanar_damage.rvmat",
								"set_heavy\set\Helmet\data\mask_damage.rvmat",
								"set_heavy\set\Helmet\data\ochki_damage.rvmat",
								"set_heavy\set\Helmet\data\tkan_damage.rvmat",
								"set_heavy\set\Helmet\data\ushi_damage.rvmat",
								"set_heavy\set\Helmet\data\vizor_damage.rvmat",
								"set_heavy\set\Helmet\data\zad_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"set_heavy\set\Helmet\data\helmet_damage.rvmat",
								"set_heavy\set\Helmet\data\krep_pnv_damage.rvmat",
								"set_heavy\set\Helmet\data\krep_fanar_damage.rvmat",
								"set_heavy\set\Helmet\data\mask_damage.rvmat",
								"set_heavy\set\Helmet\data\ochki_damage.rvmat",
								"set_heavy\set\Helmet\data\tkan_damage.rvmat",
								"set_heavy\set\Helmet\data\ushi_damage.rvmat",
								"set_heavy\set\Helmet\data\vizor_damage.rvmat",
								"set_heavy\set\Helmet\data\zad_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"set_heavy\set\Helmet\data\helmet_destruct.rvmat",
								"set_heavy\set\Helmet\data\krep_pnv_destruct.rvmat",
								"set_heavy\set\Helmet\data\krep_fanar_destruct.rvmat",
								"set_heavy\set\Helmet\data\mask_destruct.rvmat",
								"set_heavy\set\Helmet\data\ochki_destruct.rvmat",
								"set_heavy\set\Helmet\data\tkan_destruct.rvmat",
								"set_heavy\set\Helmet\data\ushi_destruct.rvmat",
								"set_heavy\set\Helmet\data\vizor_destruct.rvmat",
								"set_heavy\set\Helmet\data\zad_destruct.rvmat"
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
						damage=0.25;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.5;
					};
				};
				class Melee
				{
					class Health
					{
						damage=0.55000001;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.5;
					};
				};
				class Infected
				{
					class Health
					{
						damage=0.55000001;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.5;
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
						damage=0.25999999;
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
	class Vp_Set16_Helmet_Black: Vp_Set16_Helmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"set_heavy\set\Helmet\data\helmet.paa",
			"set_heavy\set\Helmet\data\krep_pnv.paa",
			"set_heavy\set\Helmet\data\krep_fanar.paa",
			"set_heavy\set\Helmet\data\mask.paa",
			"set_heavy\set\Helmet\data\ochki.paa",
			"set_heavy\set\Helmet\data\tkan.paa",
			"set_heavy\set\Helmet\data\ushi.paa",
			"set_heavy\set\Helmet\data\vizor_ca.paa",
			"set_heavy\set\Helmet\data\zad_meshok.paa"
		};
	};
};
