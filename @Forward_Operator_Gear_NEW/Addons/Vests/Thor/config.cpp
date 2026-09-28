class CfgPatches
{
	class FOG_Vest_Thor
	{
		units[]=
		{
			"FOG_Vest_Thor_Tan",
			"FOG_Vest_Thor_Black",
			"FOG_Vest_Thor_Green",
			"FOG_Vest_Thor_EMR",
			"FOG_Vest_Thor_MC",
			"FOG_Vest_Thor_MCB",
			"FOG_Vest_Thor_M81"
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
	class FOG_Vest_Thor_ColorBase: Clothing
	{
		scope=0;
		displayName="Thor Integrated Carrier";
		descriptionShort="THOR — бронежилет полной защиты, оснащённый мягкими и жёсткими бронеэлементами для защиты от осколков и пуль. Обеспечивает максимальное покрытие корпуса. | The THOR full protection assault body armor equipped with soft and hard armor elements to protect against shrapnel and bullets.";
		model="FOG_MOD\Vests\Thor\Thor_g.p3d";
		attachments[]=
		{
			"WalkieTalkie",
			"Chemlight",
			"FOG_big_patch",
			"FOG_tourniquet",
			"VestHolster",
			"VestPouch",
			"FOG_admin_small",
			"FOG_VestSlotFR",
			"FOG_gren_pouch",
			"FOG_ifak_vest",
			"FOG_vest_panel"
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
		quickBarBonus=3;
		weight=9000;
		itemSize[]={5,5};
		varWetMax=0.249;
		heatIsolation=0.80000001;
		repairableWithKits[]={3};
		repairCosts[]={25};
		hiddenSelections[]=
		{
			"camo",
			"camo_radiopouch"
		};
		simpleHiddenSelections[]=
		{
			"selection_radio"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Vests\Thor\data\Thor.rvmat",
			"FOG_MOD\Vests\FOG_AVS\data\Radio_PTT.rvmat"
		};
		class ClothingTypes
		{
			male="FOG_MOD\Vests\Thor\Thor_m.p3d";
			female="FOG_MOD\Vests\Thor\Thor_f.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=315;
					transferToAttachmentsCoef=0.5;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"FOG_MOD\Vests\Thor\data\Thor.rvmat",
								"FOG_MOD\Vests\FOG_AVS\data\Radio_PTT.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Vests\Thor\data\Thor.rvmat",
								"FOG_MOD\Vests\FOG_AVS\data\Radio_PTT.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Vests\Thor\data\Thor_damage.rvmat",
								"FOG_MOD\Vests\FOG_AVS\data\Radio_PTT.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Vests\Thor\data\Thor_damage.rvmat",
								"FOG_MOD\Vests\FOG_AVS\data\Radio_PTT.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Vests\Thor\data\Thor_destruct.rvmat",
								"FOG_MOD\Vests\FOG_AVS\data\Radio_PTT.rvmat"
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
						damage=0.05;
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
						damage=0.05;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.2;
					};
				};
				class Infected
				{
					class Health
					{
						damage=0.05;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.2;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage=0.05;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.2;
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
	class FOG_Vest_Thor_Tan: FOG_Vest_Thor_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\Thor\data\Thor_Tan.paa",
			"FOG_MOD\Vests\FOG_AVS\data\Radio_PTT_CB_co.paa"
		};
	};
	class FOG_Vest_Thor_Black: FOG_Vest_Thor_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\Thor\data\Thor_Black.paa",
			"FOG_MOD\Vests\FOG_AVS\data\Radio_PTT_Black_co.paa"
		};
	};
	class FOG_Vest_Thor_Green: FOG_Vest_Thor_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\Thor\data\Thor_Green.paa",
			"FOG_MOD\Vests\FOG_AVS\data\Radio_PTT_OD_co.paa"
		};
	};
	class FOG_Vest_Thor_EMR: FOG_Vest_Thor_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\Thor\data\Thor_EMR.paa",
			"FOG_MOD\Vests\FOG_AVS\data\Radio_PTT_OD_co.paa"
		};
	};
	class FOG_Vest_Thor_MC: FOG_Vest_Thor_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\Thor\data\Thor_MC_co.paa",
			"FOG_MOD\Vests\FOG_AVS\data\Radio_PTT_MC_co.paa"
		};
	};
	class FOG_Vest_Thor_MCB: FOG_Vest_Thor_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\Thor\data\Thor_MCB_co.paa",
			"FOG_MOD\Vests\FOG_AVS\data\Radio_PTT_Black_co.paa"
		};
	};
	class FOG_Vest_Thor_M81: FOG_Vest_Thor_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\Thor\data\Thor_M81_co.paa",
			"FOG_MOD\Vests\FOG_AVS\data\Radio_PTT_OD_co.paa"
		};
	};
};
