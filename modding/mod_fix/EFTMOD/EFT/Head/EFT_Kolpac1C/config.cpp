class CfgPatches
{
	class CfgPatches_Kolpac1C_Headgear
	{
		units[]=
		{
			"EFT_Kolpac1C_ColorBase"
		};
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
	class EFT_Kolpac1C_ColorBase: Clothing
	{
		scope=0;
		displayName="#EFT_Kolpac1C_v_displayName";
		descriptionShort="#EFT_Kolpac1C_v_descriptionShort";
		model="EFT\Head\EFT_Kolpac1C\EFT_Kolpac1C_g.p3d";
		repairableWithKits[]={8};
		repairCosts[]={50};
		inventorySlot[]=
		{
			"Headgear"
		};
		simulation="clothing";
		vehicleClass="Clothing";
		itemInfo[]=
		{
			"Clothing",
			"Headgear"
		};
		rotationFlags=2;
		weight=1900;
		itemSize[]={2,2};
		varWetMax=0.19;
		heatIsolation=0.25;
		visibilityModifier=0.94999999;
		headSelectionsToHide[]=
		{
			"Clipping_Mich2001"
		};
		hiddenSelections[]=
		{
			"zbytek"
		};
		hiddenSelectionsTextures[]=
		{
			"EFT\Head\EFT_Kolpac1C\data\helm.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"EFT\Head\EFT_Kolpac1C\data\helm.rvmat"
		};
		class ClothingTypes
		{
			male="EFT\Head\EFT_Kolpac1C\EFT_Kolpac1C.p3d";
			female="EFT\Head\EFT_Kolpac1C\EFT_Kolpac1C.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=150;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"EFT\Head\EFT_Kolpac1C\data\helm.rvmat"
							}
						},
						
						{
							0.89999998,
							
							{
								"EFT\Head\EFT_Kolpac1C\data\helm.rvmat"
							}
						},
						
						{
							0.40000001,
							
							{
								"EFT\Head\EFT_Kolpac1C\data\helm_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"EFT\Head\EFT_Kolpac1C\data\helm_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"EFT\Head\EFT_Kolpac1C\data\helm_destruct.rvmat"
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
						damage=0.25;
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
					soundSet="pickUpPot_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="BallisticHelmet_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class EFT_Kolpac1C: EFT_Kolpac1C_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"EFT\Head\EFT_Kolpac1C\data\helm.paa"
		};
	};
	class EFT_Kolpac1C_v: EFT_Kolpac1C_ColorBase
	{
		scope=2;
		model="EFT\Head\EFT_Kolpac1C\EFT_Kolpac1C_g_v.p3d";
		hiddenSelections[]=
		{
			"zbytek"
		};
		hiddenSelectionsTextures[]=
		{
			"EFT\Head\EFT_Kolpac1C\data\helm.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"EFT\Head\EFT_Kolpac1C\data\helm.rvmat"
		};
		class ClothingTypes
		{
			male="EFT\Head\EFT_Kolpac1C\EFT_Kolpac1C_v.p3d";
			female="EFT\Head\EFT_Kolpac1C\EFT_Kolpac1C_v.p3d";
		};
	};
};
