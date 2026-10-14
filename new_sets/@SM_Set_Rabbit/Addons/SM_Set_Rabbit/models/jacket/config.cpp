class CfgPatches
{
	class SM_Characters_Tops_Rabbit
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Characters"
		};
	};
};
class CfgVehicles
{
	class Clothing;
	class SM_Rabbit_Jacket_ColorBase: Clothing
	{
		displayName="$STR_sobrmods_setrabbit_jacket0";
		descriptionShort="$STR_sobrmods_setrabbit_jacket1";
		model="\SM_Set_Rabbit\models\jacket\sm_set_rabbit_jacket_g.p3d";
		inventorySlot[]=
		{
			"Body"
		};
		itemInfo[]=
		{
			"Clothing",
			"Body"
		};
		weight=1200;
		itemSize[]={4,3};
		itemsCargoSize[]={8,6};
		varWetMax=0.79000002;
		heatIsolation=0.69999999;
		quickBarBonus=1;
		ragQuantity=4;
		soundAttType="WoolShirt";
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		visibilityModifier=0.89999998;
		simpleHiddenSelections[]=
		{
			"hide_gloves"
		};
		hiddenSelections[]=
		{
			"jacket",
			"jacket2",
			"cep",
			"cep2",
			"personality"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=90;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"SM_Set_Rabbit\data\jacket\sm_set_rabbit_jacket.rvmat",
								"SM_Set_Rabbit\data\jacket_2\sm_set_rabbit_jacket2.rvmat",
								"SM_Set_Rabbit\data\cep\sm_set_rabbit_cep.rvmat",
								"SM_Set_Rabbit\data\cep2\sm_set_rabbit_cep2.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"SM_Set_Rabbit\data\jacket\sm_set_rabbit_jacket.rvmat",
								"SM_Set_Rabbit\data\jacket_2\sm_set_rabbit_jacket2.rvmat",
								"SM_Set_Rabbit\data\cep\sm_set_rabbit_cep.rvmat",
								"SM_Set_Rabbit\data\cep2\sm_set_rabbit_cep2.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"SM_Set_Rabbit\data\jacket\sm_set_rabbit_jacket_damage.rvmat",
								"SM_Set_Rabbit\data\jacket_2\sm_set_rabbit_jacket2_damage.rvmat",
								"SM_Set_Rabbit\data\cep\sm_set_rabbit_cep_damage.rvmat",
								"SM_Set_Rabbit\data\cep2\sm_set_rabbit_cep2_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"SM_Set_Rabbit\data\jacket\sm_set_rabbit_jacket_damage.rvmat",
								"SM_Set_Rabbit\data\jacket_2\sm_set_rabbit_jacket2_damage.rvmat",
								"SM_Set_Rabbit\data\cep\sm_set_rabbit_cep_damage.rvmat",
								"SM_Set_Rabbit\data\cep2\sm_set_rabbit_cep2_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"SM_Set_Rabbit\data\jacket\sm_set_rabbit_jacket_destruct.rvmat",
								"SM_Set_Rabbit\data\jacket_2\sm_set_rabbit_jacket2_destruct.rvmat",
								"SM_Set_Rabbit\data\cep\sm_set_rabbit_cep_destruct.rvmat",
								"SM_Set_Rabbit\data\cep2\sm_set_rabbit_cep2_destruct.rvmat"
							}
						}
					};
				};
			};
			class GlobalArmor
			{
				class Melee
				{
					class Health
					{
						damage=0.94999999;
					};
					class Blood
					{
						damage=1;
					};
					class Shock
					{
						damage=1;
					};
				};
				class Infected
				{
					class Health
					{
						damage=0.94999999;
					};
					class Blood
					{
						damage=1;
					};
					class Shock
					{
						damage=1;
					};
				};
			};
		};
		class ClothingTypes
		{
			male="\SM_Set_Rabbit\models\jacket\sm_set_rabbit_jacket_m.p3d";
			female="\SM_Set_Rabbit\models\jacket\sm_set_rabbit_jacket_f.p3d";
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
	class SM_Rabbit_Jacket_Blue: SM_Rabbit_Jacket_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"SM_Set_Rabbit\data\jacket\sm_set_rabbit_jacket_co.paa",
			"SM_Set_Rabbit\data\jacket_2\sm_set_rabbit_jacket2_co.paa",
			"SM_Set_Rabbit\data\cep\sm_set_rabbit_cep_co.paa",
			"SM_Set_Rabbit\data\cep2\sm_set_rabbit_cep2_co.paa"
		};
	};
	class SM_Rabbit_Jacket_Blue_Gray: SM_Rabbit_Jacket_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"SM_Set_Rabbit\data\jacket\sm_set_rabbit_jacket_blue_gray_co.paa",
			"SM_Set_Rabbit\data\jacket_2\sm_set_rabbit_jacket2_co.paa",
			"SM_Set_Rabbit\data\cep\sm_set_rabbit_cep_co.paa",
			"SM_Set_Rabbit\data\cep2\sm_set_rabbit_cep2_co.paa"
		};
	};
	class SM_Rabbit_Jacket_Blue_Purple: SM_Rabbit_Jacket_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"SM_Set_Rabbit\data\jacket\sm_set_rabbit_jacket_blue_purple_co.paa",
			"SM_Set_Rabbit\data\jacket_2\sm_set_rabbit_jacket2_co.paa",
			"SM_Set_Rabbit\data\cep\sm_set_rabbit_cep_co.paa",
			"SM_Set_Rabbit\data\cep2\sm_set_rabbit_cep2_co.paa"
		};
	};
	class SM_Rabbit_Jacket_Gold_Blue: SM_Rabbit_Jacket_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"SM_Set_Rabbit\data\jacket\sm_set_rabbit_jacket_gold_blue_co.paa",
			"SM_Set_Rabbit\data\jacket_2\sm_set_rabbit_jacket2_co.paa",
			"SM_Set_Rabbit\data\cep\sm_set_rabbit_cep_co.paa",
			"SM_Set_Rabbit\data\cep2\sm_set_rabbit_cep2_co.paa"
		};
	};
	class SM_Rabbit_Jacket_Gray: SM_Rabbit_Jacket_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"SM_Set_Rabbit\data\jacket\sm_set_rabbit_jacket_gray_co.paa",
			"SM_Set_Rabbit\data\jacket_2\sm_set_rabbit_jacket2_co.paa",
			"SM_Set_Rabbit\data\cep\sm_set_rabbit_cep_co.paa",
			"SM_Set_Rabbit\data\cep2\sm_set_rabbit_cep2_co.paa"
		};
	};
	class SM_Rabbit_Jacket_Green: SM_Rabbit_Jacket_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"SM_Set_Rabbit\data\jacket\sm_set_rabbit_jacket_green_co.paa",
			"SM_Set_Rabbit\data\jacket_2\sm_set_rabbit_jacket2_co.paa",
			"SM_Set_Rabbit\data\cep\sm_set_rabbit_cep_co.paa",
			"SM_Set_Rabbit\data\cep2\sm_set_rabbit_cep2_co.paa"
		};
	};
	class SM_Rabbit_Jacket_Pink: SM_Rabbit_Jacket_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"SM_Set_Rabbit\data\jacket\sm_set_rabbit_jacket_pink_co.paa",
			"SM_Set_Rabbit\data\jacket_2\sm_set_rabbit_jacket2_co.paa",
			"SM_Set_Rabbit\data\cep\sm_set_rabbit_cep_co.paa",
			"SM_Set_Rabbit\data\cep2\sm_set_rabbit_cep2_co.paa"
		};
	};
	class SM_Rabbit_Jacket_Pink_Pony: SM_Rabbit_Jacket_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"SM_Set_Rabbit\data\jacket\sm_set_rabbit_jacket_pink_pony_co.paa",
			"SM_Set_Rabbit\data\jacket_2\sm_set_rabbit_jacket2_co.paa",
			"SM_Set_Rabbit\data\cep\sm_set_rabbit_cep_co.paa",
			"SM_Set_Rabbit\data\cep2\sm_set_rabbit_cep2_co.paa"
		};
	};
	class SM_Rabbit_Jacket_Purple_Blue: SM_Rabbit_Jacket_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"SM_Set_Rabbit\data\jacket\sm_set_rabbit_jacket_purple_blue_co.paa",
			"SM_Set_Rabbit\data\jacket_2\sm_set_rabbit_jacket2_co.paa",
			"SM_Set_Rabbit\data\cep\sm_set_rabbit_cep_co.paa",
			"SM_Set_Rabbit\data\cep2\sm_set_rabbit_cep2_co.paa"
		};
	};
	class SM_Rabbit_Jacket_Purple_Pony: SM_Rabbit_Jacket_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"SM_Set_Rabbit\data\jacket\sm_set_rabbit_jacket_purple_pony_co.paa",
			"SM_Set_Rabbit\data\jacket_2\sm_set_rabbit_jacket2_co.paa",
			"SM_Set_Rabbit\data\cep\sm_set_rabbit_cep_co.paa",
			"SM_Set_Rabbit\data\cep2\sm_set_rabbit_cep2_co.paa"
		};
	};
	class SM_Rabbit_Jacket_Red: SM_Rabbit_Jacket_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"SM_Set_Rabbit\data\jacket\sm_set_rabbit_jacket_red_co.paa",
			"SM_Set_Rabbit\data\jacket_2\sm_set_rabbit_jacket2_co.paa",
			"SM_Set_Rabbit\data\cep\sm_set_rabbit_cep_co.paa",
			"SM_Set_Rabbit\data\cep2\sm_set_rabbit_cep2_co.paa"
		};
	};
	class SM_Rabbit_Jacket_Sakura: SM_Rabbit_Jacket_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"SM_Set_Rabbit\data\jacket\sm_set_rabbit_jacket_sakura_co.paa",
			"SM_Set_Rabbit\data\jacket_2\sm_set_rabbit_jacket2_co.paa",
			"SM_Set_Rabbit\data\cep\sm_set_rabbit_cep_co.paa",
			"SM_Set_Rabbit\data\cep2\sm_set_rabbit_cep2_co.paa"
		};
	};
};
