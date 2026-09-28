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
			"FOG_Vest_LV119_Grey",
			"FOG_Vest_LV119_White",
			"FOG_Vest_LV119_MC",
			"FOG_Vest_LV119_MCB",
			"FOG_Vest_LV119_MCT",
			"FOG_Vest_LV119_MCAL",
			"FOG_Vest_LV119_M81",
			"FOG_Vest_LV119_AOR1",
			"FOG_Vest_LV119_AOR2",
			"FOG_Vest_LV119_DNC"
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
	class FOG_Vest_LV119_ColorBase: Clothing
	{
		scope=0;
		displayName="Crye Precision LV-119";
		descriptionShort="Crye Precision LV119 — лёгкий модульный бронежилет, разработанный по современным минималистичным стандартам. Из прочного ламината и стрейч-материалов, обеспечивает низкий профиль, высокую мобильность и масштабируемость нагрузки. | The Crye Precision LV119 is a lightweight, modular plate carrier developed from modern minimalist armor trends. Built with durable laminate and stretch materials, it offers a slim profile, high mobility, and scalable load bearing. Used widely across military and special operations units, it supports front/back plates with adaptable placards and cummerbunds for mission specific setups.";
		model="FOG_MOD\Vests\LV119\FOG_LV119_G.p3d";
		attachments[]=
		{
			"WalkieTalkie",
			"VestPouch",
			"FOG_big_patch",
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
			"camo"
		};
		simpleHiddenSelections[]=
		{
			"selection_radio"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Vests\LV119\data\lv119.rvmat"
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
								"FOG_MOD\Vests\LV119\data\lv119.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Vests\LV119\data\lv119.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Vests\LV119\data\lv119.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Vests\LV119\data\lv119_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Vests\LV119\data\lv119_destruct.rvmat"
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
			"FOG_MOD\Vests\LV119\data\lv119_cb_co.paa"
		};
	};
	class FOG_Vest_LV119_RG: FOG_Vest_LV119_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\LV119\data\lv119_rg_co.paa"
		};
	};
	class FOG_Vest_LV119_OD: FOG_Vest_LV119_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\LV119\data\lv119_od_co.paa"
		};
	};
	class FOG_Vest_LV119_Black: FOG_Vest_LV119_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\LV119\data\lv119_black_co.paa"
		};
	};
	class FOG_Vest_LV119_Grey: FOG_Vest_LV119_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\LV119\data\lv119_grey_co.paa"
		};
	};
	class FOG_Vest_LV119_White: FOG_Vest_LV119_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\LV119\data\lv119_white_co.paa"
		};
	};
	class FOG_Vest_LV119_MC: FOG_Vest_LV119_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\LV119\data\lv119_mc_co.paa"
		};
	};
	class FOG_Vest_LV119_MCB: FOG_Vest_LV119_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\LV119\data\lv119_mcb_co.paa"
		};
	};
	class FOG_Vest_LV119_MCT: FOG_Vest_LV119_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\LV119\data\lv119_mct_co.paa"
		};
	};
	class FOG_Vest_LV119_MCAL: FOG_Vest_LV119_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\LV119\data\lv119_mcal_co.paa"
		};
	};
	class FOG_Vest_LV119_M81: FOG_Vest_LV119_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\LV119\data\lv119_m81_co.paa"
		};
	};
	class FOG_Vest_LV119_AOR1: FOG_Vest_LV119_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\LV119\data\lv119_aor1_co.paa"
		};
	};
	class FOG_Vest_LV119_AOR2: FOG_Vest_LV119_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\LV119\data\lv119_aor2_co.paa"
		};
	};
	class FOG_Vest_LV119_DNC: FOG_Vest_LV119_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\LV119\data\lv119_dnc_co.paa"
		};
	};
};
