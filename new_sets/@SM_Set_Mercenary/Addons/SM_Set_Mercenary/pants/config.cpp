class CfgPatches
{
	class SM_Characters_Pants_Mercenary
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
	class SM_Mercenary_Pants_ColorBase: Clothing
	{
		displayName="$STR_mercenary_pants";
		descriptionShort="$STR_mercenary_opisanie";
		model="\SM_Set_Mercenary\pants\models\pants_g.p3d";
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
			"camo",
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
			male="\SM_Set_Mercenary\pants\models\pants_m.p3d";
			female="\SM_Set_Mercenary\pants\models\pants_f.p3d";
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
	class SM_Mercenary_Pants_Black: SM_Mercenary_Pants_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"SM_Set_Mercenary\Jacket_Pants_Boots\Jacket_Pants_Boots_Black.paa"
		};
	};
	class SM_Mercenary_Pants_White: SM_Mercenary_Pants_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"SM_Set_Mercenary\Jacket_Pants_Boots\Jacket_Pants_Boots_White.paa"
		};
	};
	class SM_Mercenary_Pants_Red_and_Yellow: SM_Mercenary_Pants_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"SM_Set_Mercenary\Jacket_Pants_Boots\Jacket_Pants_Boots_Yellow.paa"
		};
	};
};
