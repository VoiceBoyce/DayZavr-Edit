class CfgPatches
{
	class SM_Characters_Boots_Rabbit
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Characters",
			"DZ_Characters_Shoes"
		};
	};
};
class CfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class SM_Rabbit_Boots_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_sobrmods_setrabbit_boots0";
		descriptionShort="$STR_sobrmods_setrabbit_boots1";
		model="SM_Set_Rabbit\models\boots\sm_set_rabbit_boots_g.p3d";
		inventorySlot[]=
		{
			"Feet"
		};
		itemInfo[]=
		{
			"Clothing",
			"Feet"
		};
		itemSize[]={3,2};
		weight=330;
		durability=0.2;
		varWetMax=1;
		heatIsolation=1;
		repairableWithKits[]={3};
		repairCosts[]={25};
		hiddenSelections[]=
		{
			"boots"
		};
		class ClothingTypes
		{
			male="SM_Set_Rabbit\models\boots\sm_set_rabbit_boots_m.p3d";
			female="SM_Set_Rabbit\models\boots\sm_set_rabbit_boots_f.p3d";
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
								"SM_Set_Rabbit\data\boots\sm_set_rabbit_boots.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"SM_Set_Rabbit\data\boots\sm_set_rabbit_boots.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"SM_Set_Rabbit\data\boots\sm_set_rabbit_boots_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"SM_Set_Rabbit\data\boots\sm_set_rabbit_boots_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"SM_Set_Rabbit\data\boots\sm_set_rabbit_boots_destruct.rvmat"
							}
						}
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
					soundSet="AthleticShoes_pickup_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="AthleticShoes_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class SM_Rabbit_Boots_White: SM_Rabbit_Boots_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"\SM_Set_Rabbit\data\boots\sm_set_rabbit_boots_co.paa"
		};
	};
};
