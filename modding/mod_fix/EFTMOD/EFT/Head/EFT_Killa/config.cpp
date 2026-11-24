class CfgPatches
{
	class CfgPatches_Killa_Headgear
	{
		units[]=
		{
			"EFT_Killa_ColorBase"
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
	class EFT_Killa_ColorBase: Clothing
	{
		scope=0;
		displayName="#EFT_Killa_v_displayName";
		descriptionShort="#EFT_Killa_v_descriptionShort";
		model="EFT\Head\EFT_Killa\EFT_Killa_g.p3d";
		repairableWithKits[]={8};
		repairCosts[]={50};
		inventorySlot[]=
		{
			"Headgear",
			"Helmet"
		};
		simulation="clothing";
		vehicleClass="Clothing";
		itemInfo[]=
		{
			"Clothing",
			"Headgear"
		};
		weight=2600;
		itemSize[]={2,2};
		varWetMax=0.19;
		heatIsolation=0.69999999;
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
			"EFT\Head\EFT_Killa\data\helm.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"EFT\Head\EFT_Killa\data\helm.rvmat"
		};
		class ClothingTypes
		{
			male="EFT\Head\EFT_Killa\EFT_Killa.p3d";
			female="EFT\Head\EFT_Killa\EFT_Killa.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=210;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"EFT\Head\EFT_Killa\data\helm.rvmat"
							}
						},
						
						{
							0.89999998,
							
							{
								"EFT\Head\EFT_Killa\data\helm.rvmat"
							}
						},
						
						{
							0.40000001,
							
							{
								"EFT\Head\EFT_Killa\data\helm_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"EFT\Head\EFT_Killa\data\helm_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"EFT\Head\EFT_Killa\data\helm_destruct.rvmat"
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
						damage=0.090000004;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.1;
					};
				};
				class Melee
				{
					class Health
					{
						damage=0.15000001;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.15000001;
					};
				};
				class Infected
				{
					class Health
					{
						damage=0.125;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.125;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage=0.34999999;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.1;
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
	class EFT_Killa: EFT_Killa_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"EFT\Head\EFT_Killa\data\helm.paa"
		};
	};
	class EFT_Killa_v: EFT_Killa_ColorBase
	{
		scope=2;
		model="EFT\Head\EFT_Killa\EFT_Killa_g_v.p3d";
		hiddenSelections[]=
		{
			"zbytek"
		};
		hiddenSelectionsTextures[]=
		{
			"EFT\Head\EFT_Killa\data\helm.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"EFT\Head\EFT_Killa\data\helm.rvmat"
		};
		class ClothingTypes
		{
			male="EFT\Head\EFT_Killa\EFT_Killa_v.p3d";
			female="EFT\Head\EFT_Killa\EFT_Killa_v.p3d";
		};
	};
};
