class CfgPatches
{
	class FOG_Gear_Vest_LV119
	{
		units[]=
		{
			"FOG_Vest_LV119_CB",
			"FOG_Vest_LV119_RG",
			"FOG_Vest_LV119_OD",
			"FOG_Vest_LV119_Black",
			"FOG_Vest_LV119_MC",
			"FOG_Vest_LV119_MCB",
			"FOG_Vest_LV119_M81"
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
	class Inventory_Base;
	class Clothing;
	class FOG_Vest_LV119_ColorBase: Clothing
	{
		scope=0;
		displayName="Crye Precision LV-119";
		descriptionShort="Adopted by US-SOCOM and designated LV-MBAV, this carrier accepts MBAV soft inserts and provides a lightweight streamlined system for low-visibility applications.";
		model="FOG_MOD\Vests\LV119\FOG_LV119_G.p3d";
		attachments[]=
		{
			"WalkieTalkie",
			"VestPouch",
			"FOG_vest_panel",
			"FOG_tourniquet",
			"FOG_vest_belly"
		};
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
		weight=6000;
		itemSize[]={5,5};
		varWetMax=0.249;
		heatIsolation=0.5;
		repairableWithKits[]={3};
		repairCosts[]={25};
		hiddenSelections[]=
		{
			"camo_base",
			"camo_radiopouch"
		};
		simpleHiddenSelections[]=
		{
			"selection_radio"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Vests\LV119\data\lv119.rvmat",
			"FOG_MOD\Vests\FOG_AVS\data\Radio_PTT.rvmat"
		};
		class ClothingTypes
		{
			male="FOG_MOD\Vests\LV119\FOG_LV119_M.p3d";
			female="FOG_MOD\Vests\LV119\FOG_LV119_F.p3d";
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
								"FOG_MOD\Vests\LV119\data\lv119.rvmat",
								"FOG_MOD\Vests\FOG_AVS\data\Radio_PTT.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Vests\LV119\data\lv119.rvmat",
								"FOG_MOD\Vests\FOG_AVS\data\Radio_PTT.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Vests\LV119\data\lv119_damage.rvmat",
								"FOG_MOD\Vests\FOG_AVS\data\Radio_PTT_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Vests\LV119\data\lv119_damage.rvmat",
								"FOG_MOD\Vests\FOG_AVS\data\Radio_PTT_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Vests\LV119\data\lv119_destruct.rvmat",
								"FOG_MOD\Vests\FOG_AVS\data\Radio_PTT_destruct.rvmat"
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
	class FOG_Vest_LV119_CB: FOG_Vest_LV119_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\LV119\data\lv119_cb_co.paa",
			"FOG_MOD\Vests\FOG_AVS\data\Radio_PTT_CB_co.paa"
		};
	};
	class FOG_Vest_LV119_RG: FOG_Vest_LV119_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\LV119\data\lv119_rg_co.paa",
			"FOG_MOD\Vests\FOG_AVS\data\Radio_PTT_rg_co.paa"
		};
	};
	class FOG_Vest_LV119_OD: FOG_Vest_LV119_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\LV119\data\lv119_od_co.paa",
			"FOG_MOD\Vests\FOG_AVS\data\Radio_PTT_OD_co.paa"
		};
	};
	class FOG_Vest_LV119_Black: FOG_Vest_LV119_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\LV119\data\lv119_blk_co.paa",
			"FOG_MOD\Vests\FOG_AVS\data\Radio_PTT_Black_co.paa"
		};
	};
	class FOG_Vest_LV119_MC: FOG_Vest_LV119_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\LV119\data\lv119_mc_co.paa",
			"FOG_MOD\Vests\FOG_AVS\data\Radio_PTT_MC_co.paa"
		};
	};
	class FOG_Vest_LV119_MCB: FOG_Vest_LV119_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\LV119\data\lv119_mcb_co.paa",
			"FOG_MOD\Vests\FOG_AVS\data\Radio_PTT_MCB_co.paa"
		};
	};
	class FOG_Vest_LV119_M81: FOG_Vest_LV119_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\LV119\data\lv119_m81_co.paa",
			"FOG_MOD\Vests\FOG_AVS\data\Radio_PTT_M81_co.paa"
		};
	};
};
