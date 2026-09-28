class CfgPatches
{
	class FOG_Vest_Gen4_stuff
	{
		units[]=
		{
			"FOG_Vest_Gen4_Black",
			"FOG_Vest_Gen4_Green",
			"FOG_Vest_Gen4_Tan",
			"FOG_Vest_Gen4_EMR",
			"FOG_Vest_Gen4_UCP",
			"FOG_Vest_Gen4_NWU",
			"FOG_Vest_Gen4_MARPAT"
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
	class FOG_Vest_Gen4_ColorBase: Clothing
	{
		scope=0;
		displayName="IOTV Gen4 Body Armor";
		descriptionShort="Усовершенствованный наружный тактический жилет (IOTV) Gen IV обеспечивает максимальную свободу движений при выполнении задач. Улучшенная система распределения веса и модульная конструкция позволяют адаптировать жилет под конкретные требования миссии. | The Improved Outer Tactical Vest (IOTV) Gen IV is designed to permit maximum freedom of movement required to assume correct firing positions. The improved design reduces the overall weight of the vest has integrated quick release for rapid doffing.";
		model="FOG_MOD\Vests\IOTVGen4\models\Gen4_g.p3d";
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
			"Belt_Left",
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
		itemSize[]={5,6};
		varWetMax=0.249;
		heatIsolation=0.80000001;
		repairableWithKits[]={3};
		repairCosts[]={25};
		hiddenSelections[]=
		{
			"camo",
			"camo_radio"
		};
		simpleHiddenSelections[]=
		{
			"selection_radio"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Vests\IOTVGen4\data\Gen4.rvmat",
			"FOG_MOD\Vests\FOG_AVS\data\Radio_PTT.rvmat"
		};
		class ClothingTypes
		{
			male="FOG_MOD\Vests\IOTVGen4\models\Gen4_m.p3d";
			female="FOG_MOD\Vests\IOTVGen4\models\Gen4_f.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=350;
					transferToAttachmentsCoef=0.5;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"FOG_MOD\Vests\IOTVGen4\data\Gen4.rvmat",
								"FOG_MOD\Vests\FOG_AVS\data\Radio_PTT.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Vests\IOTVGen4\data\Gen4.rvmat",
								"FOG_MOD\Vests\FOG_AVS\data\Radio_PTT.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Vests\IOTVGen4\data\Gen4_damage.rvmat",
								"FOG_MOD\Vests\FOG_AVS\data\Radio_PTT_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Vests\IOTVGen4\data\Gen4_damage.rvmat",
								"FOG_MOD\Vests\FOG_AVS\data\Radio_PTT_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Vests\IOTVGen4\data\Gen4_destruct.rvmat",
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
						damage=0.1;
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
						damage=0.1;
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
						damage=0.1;
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
						damage=0.1;
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
	class FOG_Vest_Gen4_Black: FOG_Vest_Gen4_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\IOTVGen4\data\Gen4_black_co.paa",
			"FOG_MOD\Vests\FOG_AVS\data\Radio_PTT_Black_co.paa"
		};
	};
	class FOG_Vest_Gen4_Green: FOG_Vest_Gen4_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\IOTVGen4\data\Gen4_green_co.paa",
			"FOG_MOD\Vests\FOG_AVS\data\Radio_PTT_OD_co.paa"
		};
	};
	class FOG_Vest_Gen4_Tan: FOG_Vest_Gen4_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\IOTVGen4\data\Gen4_tan_co.paa",
			"FOG_MOD\Vests\FOG_AVS\data\Radio_PTT_CB_co.paa"
		};
	};
	class FOG_Vest_Gen4_EMR: FOG_Vest_Gen4_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\IOTVGen4\data\Gen4_emr_co.paa",
			"FOG_MOD\Vests\FOG_AVS\data\Radio_PTT_OD_co.paa"
		};
	};
	class FOG_Vest_Gen4_UCP: FOG_Vest_Gen4_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\IOTVGen4\data\Gen4_ucp_co.paa",
			"FOG_MOD\Vests\FOG_AVS\data\Radio_PTT_Black_co.paa"
		};
	};
	class FOG_Vest_Gen4_NWU: FOG_Vest_Gen4_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\IOTVGen4\data\Gen4_nwu_co.paa",
			"FOG_MOD\Vests\FOG_AVS\data\Radio_PTT_Black_co.paa"
		};
	};
	class FOG_Vest_Gen4_MARPAT: FOG_Vest_Gen4_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\IOTVGen4\data\Gen4_marpat_co.paa",
			"FOG_MOD\Vests\FOG_AVS\data\Radio_PTT_Black_co.paa"
		};
	};
};
