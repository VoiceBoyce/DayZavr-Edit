class CfgPatches
{
	class SM_Characters_Helmet_Mercenary
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
	class SM_Mercenary_Helmet_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_mercenary_helmet";
		descriptionShort="$STR_mercenary_opisanie";
		model="\SM_Set_Mercenary\hat\models\helmet_g.p3d";
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
			"Helmet"
		};
		class ClothingTypes
		{
			male="\SM_Set_Mercenary\hat\models\helmet_m.p3d";
			female="\SM_Set_Mercenary\hat\models\helmet_f.p3d";
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
								"SM_Set_Mercenary\hat\data\Helmet.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"SM_Set_Mercenary\hat\data\Helmet.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"SM_Set_Mercenary\hat\data\Helmet_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"SM_Set_Mercenary\hat\data\Helmet_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"SM_Set_Mercenary\hat\data\Helmet_destruct.rvmat"
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
	class SM_Mercenary_Helmet_Black_and_White: SM_Mercenary_Helmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"SM_Set_Mercenary\hat\data\Helmet_Black.paa"
		};
	};
	class SM_Mercenary_Helmet_White_and_Black: SM_Mercenary_Helmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"SM_Set_Mercenary\hat\data\Helmet_White.paa"
		};
	};
	class SM_Mercenary_Helmet_Red_and_Yellow: SM_Mercenary_Helmet_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"SM_Set_Mercenary\hat\data\Helmet_Yellow.paa"
		};
	};
};
