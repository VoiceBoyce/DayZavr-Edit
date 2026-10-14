class CfgPatches
{
	class SM_Characters_Head_Rabbit
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data"
		};
	};
};
class CfgVehicles
{
	class Clothing;
	class SM_Rabbit_Head_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_sobrmods_setrabbit_ebalo0";
		descriptionShort="$STR_sobrmods_setrabbit_ebalo1";
		model="\SM_Set_Rabbit\models\ebalo\sm_set_rabbit_ebalo_g.p3d";
		repairableWithKits[]={8};
		repairCosts[]={25};
		inventorySlot[]=
		{
			"Headgear",
			"Helmet"
		};
		vehicleClass="Clothing";
		itemInfo[]=
		{
			"Clothing",
			"Headgear"
		};
		weight=1100;
		itemSize[]={4,4};
		varWetMax=0.49000001;
		heatIsolation=0.80000001;
		noNVStrap=1;
		noEyewear=1;
		headSelectionsToHide[]=
		{
			"Clipping_ushanka"
		};
		hiddenSelections[]=
		{
			"ebalo",
			"hat",
			"rabbit_hat"
		};
		class ClothingTypes
		{
			male="\SM_Set_Rabbit\models\ebalo\sm_set_rabbit_ebalo_mf.p3d";
			female="\SM_Set_Rabbit\models\ebalo\sm_set_rabbit_ebalo_mf.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=100;
					transferToAttachmentsCoef=3;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"SM_Set_Rabbit\data\ebalo\sm_set_rabbit_ebalo.rvmat",
								"SM_Set_Rabbit\data\hat\sm_set_rabbit_hat.rvmat",
								"SM_Set_Rabbit\data\rabbit_hat\sm_set_rabbit_rabbit_hat.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"SM_Set_Rabbit\data\ebalo\sm_set_rabbit_ebalo.rvmat",
								"SM_Set_Rabbit\data\hat\sm_set_rabbit_hat.rvmat",
								"SM_Set_Rabbit\data\rabbit_hat\sm_set_rabbit_rabbit_hat.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"SM_Set_Rabbit\data\ebalo\sm_set_rabbit_ebalo_damage.rvmat",
								"SM_Set_Rabbit\data\hat\sm_set_rabbit_hat_damage.rvmat",
								"SM_Set_Rabbit\data\rabbit_hat\sm_set_rabbit_rabbit_hat_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"SM_Set_Rabbit\data\ebalo\sm_set_rabbit_ebalo_damage.rvmat",
								"SM_Set_Rabbit\data\hat\sm_set_rabbit_hat_damage.rvmat",
								"SM_Set_Rabbit\data\rabbit_hat\sm_set_rabbit_rabbit_hat_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"SM_Set_Rabbit\data\ebalo\sm_set_rabbit_ebalo_destruct.rvmat",
								"SM_Set_Rabbit\data\hat\sm_set_rabbit_hat_destruct.rvmat",
								"SM_Set_Rabbit\data\rabbit_hat\sm_set_rabbit_rabbit_hat_destruct.rvmat"
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
						damage=0.25999999;
					};
				};
			};
		};
		soundImpactType="metal";
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem
				{
					soundSet="DarkMotoHelmet_pickup_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="DarkMotoHelmet_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class SM_Rabbit_Head_Blue: SM_Rabbit_Head_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"SM_Set_Rabbit\data\ebalo\sm_set_rabbit_ebalo_co.paa",
			"SM_Set_Rabbit\data\hat\sm_set_rabbit_hat_co.paa",
			"SM_Set_Rabbit\data\rabbit_hat\sm_set_rabbit_rabbit_hat_co.paa"
		};
	};
};
