class CfgPatches
{
	class SM_Characters_Vest_Rabbit
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
	class SM_Rabbit_Vest_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_sobrmods_setrabbit_vest0";
		descriptionShort="$STR_sobrmods_setrabbit_vest1";
		model="\SM_Set_Rabbit\models\vest\models\rabbit_vest_g.p3d";
		inventorySlot[]=
		{
			"Vest"
		};
		itemInfo[]=
		{
			"Clothing",
			"Vest"
		};
		weight=12000;
		itemSize[]={5,5};
		quickBarBonus=1;
		varWetMax=0.49000001;
		heatIsolation=0.80000001;
		repairableWithKits[]={3,8};
		repairCosts[]={75,50};
		visibilityModifier=0.94999999;
		class ClothingTypes
		{
			male="\SM_Set_Rabbit\models\vest\models\rabbit_vest_m.p3d";
			female="\SM_Set_Rabbit\models\vest\models\rabbit_vest_f.p3d";
		};
		hiddenSelections[]=
		{
			"Cumberbund",
			"PlateFrame"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=225;
					transferToAttachmentsCoef=0.5;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"SM_Set_Rabbit\data\vest\texture1\rabbit_vest_cumberbund.rvmat",
								"SM_Set_Rabbit\data\vest\texture2\rabbit_vest_plateframe.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"SM_Set_Rabbit\data\vest\texture1\rabbit_vest_cumberbund.rvmat",
								"SM_Set_Rabbit\data\vest\texture2\rabbit_vest_plateframe.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"SM_Set_Rabbit\data\vest\texture1\rabbit_vest_cumberbund_damage.rvmat",
								"SM_Set_Rabbit\data\vest\texture2\rabbit_vest_plateframe_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"SM_Set_Rabbit\data\vest\texture1\rabbit_vest_cumberbund_damage.rvmat",
								"SM_Set_Rabbit\data\vest\texture2\rabbit_vest_plateframe_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"SM_Set_Rabbit\data\vest\texture1\rabbit_vest_cumberbund_destruct.rvmat",
								"SM_Set_Rabbit\data\vest\texture2\rabbit_vest_plateframe_destruct.rvmat"
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
						damage=0.30000001;
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
						damage=0.30000001;
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
	class SM_Rabbit_Vest: SM_Rabbit_Vest_ColorBase
	{
		scope=2;
		hiddenSelections[]=
		{
			"Cumberbund",
			"PlateFrame"
		};
		hiddenSelectionsTextures[]=
		{
			"SM_Set_Rabbit\data\vest\texture1\rabbit_vest_cumberbund_co.paa",
			"SM_Set_Rabbit\data\vest\texture2\rabbit_vest_plateframe_co.paa"
		};
	};
};
