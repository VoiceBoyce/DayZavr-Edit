class CfgPatches
{
	class SM_Characters_Tops_Mercenary
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
	class SM_Mercenary_Jacket_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_mercenary_jacket1";
		descriptionShort="$STR_mercenary_opisanie";
		model="\SM_Set_Mercenary\jacket\models\jacket_g.p3d";
		inventorySlot[]=
		{
			"Body"
		};
		itemInfo[]=
		{
			"Clothing",
			"Body"
		};
		weight=1200;
		itemSize[]={4,3};
		itemsCargoSize[]={8,6};
		varWetMax=0.79000002;
		heatIsolation=0.69999999;
		quickBarBonus=1;
		ragQuantity=4;
		soundAttType="WoolShirt";
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		visibilityModifier=0.89999998;
		simpleHiddenSelections[]=
		{
			"hide_gloves"
		};
		hiddenSelections[]=
		{
			"camo",
			"exoskelet"
		};
		hiddenSelectionsMaterials[]=
		{
			"SM_Set_Mercenary\Jacket_Pants_Boots\Jacket_Pants_Boots.rvmat",
			"SM_Set_Mercenary\Exo\Exo.rvmat"
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
								"SM_Set_Mercenary\Jacket_Pants_Boots\Jacket_Pants_Boots.rvmat",
								"SM_Set_Mercenary\Exo\Exo.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"SM_Set_Mercenary\Jacket_Pants_Boots\Jacket_Pants_Boots.rvmat",
								"SM_Set_Mercenary\Exo\Exo.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"SM_Set_Mercenary\Jacket_Pants_Boots\Jacket_Pants_Boots_damage.rvmat",
								"SM_Set_Mercenary\Exo\Exo_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"SM_Set_Mercenary\Jacket_Pants_Boots\Jacket_Pants_Boots_damage.rvmat",
								"SM_Set_Mercenary\Exo\Exo_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"SM_Set_Mercenary\Jacket_Pants_Boots\Jacket_Pants_Boots_destruct.rvmat",
								"SM_Set_Mercenary\Exo\Exo_destruct.rvmat"
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
			male="\SM_Set_Mercenary\jacket\models\jacket_m.p3d";
			female="\SM_Set_Mercenary\jacket\models\jacket_f.p3d";
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
	class SM_Mercenary_Jacket_Black: SM_Mercenary_Jacket_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"SM_Set_Mercenary\Jacket_Pants_Boots\Jacket_Pants_Boots_Black.paa",
			"\SM_Set_Mercenary\Exo\Exo_Black.paa"
		};
	};
	class SM_Mercenary_Jacket_White: SM_Mercenary_Jacket_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"SM_Set_Mercenary\Jacket_Pants_Boots\Jacket_Pants_Boots_White.paa",
			"SM_Set_Mercenary\Exo\Exo_White.paa"
		};
	};
	class SM_Mercenary_Jacket_Yellow: SM_Mercenary_Jacket_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"SM_Set_Mercenary\Jacket_Pants_Boots\Jacket_Pants_Boots_Yellow.paa",
			"SM_Set_Mercenary\Exo\Exo_Red.paa"
		};
	};
	class SM_Mercenary_Jacket_NO_EXO_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_mercenary_jacket2";
		descriptionShort="$STR_mercenary_opisanie";
		model="\SM_Set_Mercenary\jacket\models\jacket_no_exo_g.p3d";
		inventorySlot[]=
		{
			"Body"
		};
		itemInfo[]=
		{
			"Clothing",
			"Body"
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
		simpleHiddenSelections[]=
		{
			"hide_gloves"
		};
		hiddenSelections[]=
		{
			"camo"
		};
		hiddenSelectionsMaterials[]=
		{
			"SM_Set_Mercenary\Jacket_Pants_Boots\Jacket_Pants_Boots.rvmat"
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
			male="\SM_Set_Mercenary\jacket\models\jacket_no_exo_m.p3d";
			female="\SM_Set_Mercenary\jacket\models\jacket_no_exo_f.p3d";
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
	class SM_Mercenary_Jacket_NO_EXO_Black: SM_Mercenary_Jacket_NO_EXO_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"\SM_Set_Mercenary\Jacket_Pants_Boots\Jacket_Pants_Boots_Black.paa"
		};
	};
	class SM_Mercenary_Jacket_NO_EXO_White: SM_Mercenary_Jacket_NO_EXO_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"SM_Set_Mercenary\Jacket_Pants_Boots\Jacket_Pants_Boots_White.paa"
		};
	};
	class SM_Mercenary_Jacket_NO_EXO_Yellow: SM_Mercenary_Jacket_NO_EXO_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"SM_Set_Mercenary\Jacket_Pants_Boots\Jacket_Pants_Boots_Yellow.paa"
		};
	};
};
