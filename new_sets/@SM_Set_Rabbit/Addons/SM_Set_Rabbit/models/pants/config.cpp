class CfgPatches
{
	class SM_Characters_Pants_Rabbit
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
	class SM_Rabbit_Pants_ColorBase: Clothing
	{
		displayName="$STR_sobrmods_setrabbit_pants0";
		descriptionShort="$STR_sobrmods_setrabbit_pants1";
		model="\SM_Set_Rabbit\models\pants\sm_set_rabbit_pants_g.p3d";
		inventorySlot[]=
		{
			"Legs"
		};
		itemInfo[]=
		{
			"Clothing",
			"Legs"
		};
		weight=270;
		itemSize[]={4,3};
		itemsCargoSize[]={6,5};
		varWetMax=0.79000002;
		heatIsolation=0.69999999;
		quickBarBonus=1;
		ragQuantity=4;
		soundAttType="WoolShirt";
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		visibilityModifier=0.89999998;
		hiddenSelections[]=
		{
			"camoMale",
			"camoFemale",
			"camoGround",
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
								"SM_Set_Rabbit\data\pants\sm_set_rabbit_pants.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"SM_Set_Rabbit\data\pants\sm_set_rabbit_pants.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_destruct.rvmat"
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
			male="\SM_Set_Rabbit\models\pants\sm_set_rabbit_pants_m.p3d";
			female="\SM_Set_Rabbit\models\pants\sm_set_rabbit_pants_m.p3d";
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
	class SM_Rabbit_Pants_Blue: SM_Rabbit_Pants_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"\SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_co.paa",
			"\SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_co.paa",
			"\SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_co.paa"
		};
	};
	class SM_Rabbit_Pants_Blue_Gray: SM_Rabbit_Pants_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"\SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_blue_gray_co.paa",
			"\SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_blue_gray_co.paa",
			"\SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_blue_gray_co.paa"
		};
	};
	class SM_Rabbit_Pants_Blue_Purple: SM_Rabbit_Pants_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"\SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_blue_purple_co.paa",
			"\SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_blue_purple_co.paa",
			"\SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_blue_purple_co.paa"
		};
	};
	class SM_Rabbit_Pants_Gold_Blue: SM_Rabbit_Pants_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"\SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_gold_blue_co.paa",
			"\SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_gold_blue_co.paa",
			"\SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_gold_blue_co.paa"
		};
	};
	class SM_Rabbit_Pants_Gray: SM_Rabbit_Pants_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"\SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_gray_co.paa",
			"\SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_gray_co.paa",
			"\SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_gray_co.paa"
		};
	};
	class SM_Rabbit_Pants_Green: SM_Rabbit_Pants_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"\SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_green_co.paa",
			"\SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_green_co.paa",
			"\SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_green_co.paa"
		};
	};
	class SM_Rabbit_Pants_Pink: SM_Rabbit_Pants_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"\SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_pink_co.paa",
			"\SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_pink_co.paa",
			"\SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_pink_co.paa"
		};
	};
	class SM_Rabbit_Pants_Pink_Pony: SM_Rabbit_Pants_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"\SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_pink_pony_co.paa",
			"\SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_pink_pony_co.paa",
			"\SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_pink_pony_co.paa"
		};
	};
	class SM_Rabbit_Pants_Purple_Blue: SM_Rabbit_Pants_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"\SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_purple_blue_co.paa",
			"\SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_purple_blue_co.paa",
			"\SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_purple_blue_co.paa"
		};
	};
	class SM_Rabbit_Pants_Purple_Pony: SM_Rabbit_Pants_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"\SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_purple_pony_co.paa",
			"\SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_purple_pony_co.paa",
			"\SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_purple_pony_co.paa"
		};
	};
	class SM_Rabbit_Pants_Red: SM_Rabbit_Pants_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"\SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_red_co.paa",
			"\SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_red_co.paa",
			"\SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_red_co.paa"
		};
	};
	class SM_Rabbit_Pants_Sakura: SM_Rabbit_Pants_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"\SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_sakura_co.paa",
			"\SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_sakura_co.paa",
			"\SM_Set_Rabbit\data\pants\sm_set_rabbit_pants_sakura_co.paa"
		};
	};
};
