class CfgPatches
{
	class CfgPatches_KiverM_Headgear
	{
		units[]=
		{
			"EFT_KiverM_ColorBase"
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
	class EFT_KiverM_ColorBase: Clothing
	{
		scope=0;
		displayName="#EFT_KiverM_displayName";
		descriptionShort="#EFT_KiverM_displayName";
		model="EFT\Head\EFT_KiverM\EFT_KiverM_g.p3d";
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
		weight=1200;
		itemSize[]={2,2};
		varWetMax=0.30000001;
		heatIsolation=0.5;
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
			"EFT\Head\EFT_KiverM\data\helm.paa"
		};
		hiddenSelectionsMaterials[]=
		{
			"EFT\Head\EFT_KiverM\data\helm.rvmat"
		};
		class ClothingTypes
		{
			male="EFT\Head\EFT_KiverM\EFT_KiverM.p3d";
			female="EFT\Head\EFT_KiverM\EFT_KiverM.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=140;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"EFT\Head\EFT_KiverM\data\helm.rvmat"
							}
						},
						
						{
							0.89999998,
							
							{
								"EFT\Head\EFT_KiverM\data\helm.rvmat"
							}
						},
						
						{
							0.40000001,
							
							{
								"EFT\Head\EFT_KiverM\data\helm_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"EFT\Head\EFT_KiverM\data\helm_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"EFT\Head\EFT_KiverM\data\helm_destruct.rvmat"
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
						damage=0.40000001;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.40000001;
					};
				};
				class Melee
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
						damage=0.34999999;
					};
				};
				class Infected
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
						damage=0.34999999;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage=0.64999998;
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
	class EFT_KiverM: EFT_KiverM_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"EFT\Head\EFT_KiverM\data\helm.paa"
		};
	};
};
