class CfgPatches
{
	class FOG_Gear_Vest_LVMBAV
	{
		units[]=
		{
			"FOG_Vest_LVMBAV_CB",
			"FOG_Vest_LVMBAV_RG",
			"FOG_Vest_LVMBAV_Black",
			"FOG_Vest_LVMBAV_Tan"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters",
			"DZ_Scripts",
			"DZ_Characters_Vests"
		};
	};
};
class cfgVehicles
{
	class Clothing;
	class FOG_Vest_LVMBAV_ColorBase: Clothing
	{
		scope=0;
		displayName="Crye Precision LV-MBAV";
		descriptionShort="Принят на вооружение US-SOCOM как LV-MBAV. Этот бронежилет принимает мягкие вставки MBAV и обеспечивает лёгкую обтекаемую систему для малозаметного ношения. | Adopted by US-SOCOM and designated LV-MBAV, this carrier accepts MBAV soft inserts and provides a lightweight streamlined system for low-visibility applications.";
		model="FOG_MOD\Vests\MBAV\FOG_MBAV_G.p3d";
		attachments[]={};
		inventorySlot[]=
		{
			"Vest"
		};
		itemInfo[]=
		{
			"Clothing",
			"Vest"
		};
		quickBarBonus=2;
		weight=5000;
		itemSize[]={5,5};
		varWetMax=0.249;
		heatIsolation=0.5;
		repairableWithKits[]={3};
		repairCosts[]={25};
		hiddenSelections[]=
		{
			"camo1",
			"camo2"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Vests\MBAV\data\mbav_main.rvmat",
			"FOG_MOD\Vests\MBAV\data\mbav_side.rvmat"
		};
		class ClothingTypes
		{
			male="FOG_MOD\Vests\MBAV\FOG_MBAV_M.p3d";
			female="FOG_MOD\Vests\MBAV\FOG_MBAV_F.p3d";
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
								"FOG_MOD\Vests\MBAV\data\mbav_main.rvmat",
								"FOG_MOD\Vests\MBAV\data\mbav_side.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Vests\MBAV\data\mbav_main.rvmat",
								"FOG_MOD\Vests\MBAV\data\mbav_side.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Vests\MBAV\data\mbav_main_damage.rvmat",
								"FOG_MOD\Vests\MBAV\data\mbav_side_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Vests\MBAV\data\mbav_main_damage.rvmat",
								"FOG_MOD\Vests\MBAV\data\mbav_side_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Vests\MBAV\data\mbav_main_destruct.rvmat",
								"FOG_MOD\Vests\MBAV\data\mbav_side_destruct.rvmat"
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
						damage=0.30000001;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.60000002;
					};
				};
				class Melee
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
						damage=0.25;
					};
				};
				class Infected
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
						damage=0.25;
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
						damage=0.25;
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
					soundSet="SmershVest_pickup_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="SmershVest_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class FOG_Vest_LVMBAV_CB: FOG_Vest_LVMBAV_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\MBAV\data\mbav_main_cb_co.paa",
			"FOG_MOD\Vests\MBAV\data\mbav_side_cb_co.paa"
		};
	};
	class FOG_Vest_LVMBAV_RG: FOG_Vest_LVMBAV_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\MBAV\data\mbav_main_rg_co.paa",
			"FOG_MOD\Vests\MBAV\data\mbav_side_rg_co.paa"
		};
	};
	class FOG_Vest_LVMBAV_Black: FOG_Vest_LVMBAV_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\MBAV\data\mbav_main_black_co.paa",
			"FOG_MOD\Vests\MBAV\data\mbav_side_black_co.paa"
		};
	};
	class FOG_Vest_LVMBAV_Tan: FOG_Vest_LVMBAV_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\MBAV\data\mbav_main_co.paa",
			"FOG_MOD\Vests\MBAV\data\mbav_side_co.paa"
		};
	};
};
