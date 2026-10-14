class CfgPatches
{
	class SM_Characters_Boots_Mercenary
	{
		units[]=
		{
			""
		};
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
	class SM_Mercenary_Boots_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_mercenary_boots";
		descriptionShort="$STR_mercenary_opisanie";
		model="SM_Set_Mercenary\boots\models\boots_g.p3d";
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
			"camo"
		};
		class ClothingTypes
		{
			male="SM_Set_Mercenary\boots\models\boots_m.p3d";
			female="SM_Set_Mercenary\boots\models\boots_f.p3d";
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
								"SM_Set_Mercenary\Jacket_Pants_Boots\Jacket_Pants_Boots.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"SM_Set_Mercenary\Jacket_Pants_Boots\Jacket_Pants_Boots.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"SM_Set_Mercenary\Jacket_Pants_Boots\Jacket_Pants_Boots_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"SM_Set_Mercenary\Jacket_Pants_Boots\Jacket_Pants_Boots_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"SM_Set_Mercenary\Jacket_Pants_Boots\Jacket_Pants_Boots_destruct.rvmat"
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
	class SM_Mercenary_Boots_Black: SM_Mercenary_Boots_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"SM_Set_Mercenary\Jacket_Pants_Boots\Jacket_Pants_Boots_Black.paa"
		};
	};
	class SM_Mercenary_Boots_White: SM_Mercenary_Boots_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"SM_Set_Mercenary\Jacket_Pants_Boots\Jacket_Pants_Boots_White.paa"
		};
	};
	class SM_Mercenary_Boots_Red_and_Yellow: SM_Mercenary_Boots_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"SM_Set_Mercenary\Jacket_Pants_Boots\Jacket_Pants_Boots_Yellow.paa"
		};
	};
};
