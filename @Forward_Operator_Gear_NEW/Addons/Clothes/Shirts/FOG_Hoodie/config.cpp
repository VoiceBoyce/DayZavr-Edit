class CfgPatches
{
	class FOG_Hoodie_Jacket_Shit
	{
		units[]=
		{
			"FOG_Hoodie_Jacket_Black",
			"FOG_Hoodie_Jacket_Nike_G",
			"FOG_Hoodie_Jacket_Patagonia",
			"FOG_Hoodie_Jacket_FOG",
			"FOG_Hoodie_Jacket_CDAJ",
			"FOG_Hoodie_Jacket_MC",
			"FOG_Hoodie_Jacket_ERDL",
			"FOG_Hoodie_Jacket_PERDL",
			"FOG_Hoodie_Jacket_Thrasher",
			"FOG_Hoodie_Jacket_Supreme",
			"FOG_Hoodie_Jacket_MLG"
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
	class FOG_Hoodie_Jacket_ColorBase: Clothing
	{
		displayName="Pull Over Hoodie";
		descriptionShort="Pull Over Hoodie — это тактическое худи с 45 слотами, где технология -Smart-Cuff- позволяет зажать кнопку -i- для автоматического закатывания рукавов, превращая его из уютного повседневного элемента в боевую экипировку за секунду. Следы потертостей на капюшоне намекают, что оно повидало не одну ночную вылазку | The Pull Over Hoodie is a tactical 45-slot hooded sweatshirt featuring -Smart-Cuff- tech - hold the -i- button to auto-roll sleeves, transitioning from cozy loungewear to mission-ready gear in seconds. Faint wear marks on the hood suggest it's been through more than a few midnight ops.";
		model="\FOG_MOD\Clothes\Shirts\FOG_Hoodie\FOG_Hoodie_G.p3d";
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
		itemsCargoSize[]={5,9};
		quickBarBonus=5;
		varWetMax=0.39999999;
		heatIsolation=0.6;
		ragQuantity=4;
		repairableWithKits[]={5,9};
		repairCosts[]={30,25};
		soundAttType="WoolShirt";
		hiddenSelections[]=
		{
			"camo",
			"personality"
		};
		simpleHiddenSelections[]=
		{
			"sleeves_rolled",
			"sleeves_unrolled"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=1000;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_Hoodie\data\FOG_Hoodie.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_Hoodie\data\FOG_Hoodie.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_Hoodie\data\FOG_Hoodie_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_Hoodie\data\FOG_Hoodie_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_Hoodie\data\FOG_Hoodie_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		class GlobalArmor
		{
			class Melee
			{
				class Health
				{
					damage=0.89999998;
				};
				class Blood
				{
					damage=0.89999998;
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
					damage=0.89999998;
				};
				class Blood
				{
					damage=0.89999998;
				};
				class Shock
				{
					damage=1;
				};
			};
		};
		class ClothingTypes
		{
			male="\FOG_MOD\Clothes\Shirts\FOG_Hoodie\FOG_Hoodie_M.p3d";
			female="\FOG_MOD\Clothes\Shirts\FOG_Hoodie\FOG_Hoodie_F.p3d";
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
	class FOG_Hoodie_Jacket_Black: FOG_Hoodie_Jacket_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_Hoodie\data\FOG_Hoodie_black_co.paa"
		};
	};
	class FOG_Hoodie_Jacket_Nike_G: FOG_Hoodie_Jacket_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_Hoodie\data\FOG_Hoodie_nike_co.paa"
		};
	};
	class FOG_Hoodie_Jacket_Patagonia: FOG_Hoodie_Jacket_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_Hoodie\data\FOG_Hoodie_pata_co.paa"
		};
	};
	class FOG_Hoodie_Jacket_FOG: FOG_Hoodie_Jacket_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_Hoodie\data\FOG_Hoodie_FOG_co.paa"
		};
	};
	class FOG_Hoodie_Jacket_CDAJ: FOG_Hoodie_Jacket_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_Hoodie\data\FOG_Hoodie_CDAJ_co.paa"
		};
	};
	class FOG_Hoodie_Jacket_MC: FOG_Hoodie_Jacket_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_Hoodie\data\FOG_Hoodie_MC_co.paa"
		};
	};
	class FOG_Hoodie_Jacket_ERDL: FOG_Hoodie_Jacket_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_Hoodie\data\FOG_Hoodie_ERDL_co.paa"
		};
	};
	class FOG_Hoodie_Jacket_PERDL: FOG_Hoodie_Jacket_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_Hoodie\data\FOG_Hoodie_Purple_ERDL_co.paa"
		};
	};
	class FOG_Hoodie_Jacket_Thrasher: FOG_Hoodie_Jacket_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_Hoodie\data\FOG_Hoodie_Thrasher_co.paa"
		};
	};
	class FOG_Hoodie_Jacket_Supreme: FOG_Hoodie_Jacket_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_Hoodie\data\FOG_Hoodie_Supreme_co.paa"
		};
	};
	class FOG_Hoodie_Jacket_MLG: FOG_Hoodie_Jacket_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_Hoodie\data\FOG_Hoodie_MLG_co.paa"
		};
	};
	class FOG_Hoodie_Jacket_Rolled_Black: FOG_Hoodie_Jacket_Black
	{
	};
	class FOG_Hoodie_Jacket_Rolled_Nike_G: FOG_Hoodie_Jacket_Nike_G
	{
	};
	class FOG_Hoodie_Jacket_Rolled_Patagonia: FOG_Hoodie_Jacket_Patagonia
	{
	};
	class FOG_Hoodie_Jacket_Rolled_FOG: FOG_Hoodie_Jacket_FOG
	{
	};
	class FOG_Hoodie_Jacket_Rolled_CDAJ: FOG_Hoodie_Jacket_CDAJ
	{
	};
	class FOG_Hoodie_Jacket_Rolled_MC: FOG_Hoodie_Jacket_MC
	{
	};
	class FOG_Hoodie_Jacket_Rolled_ERDL: FOG_Hoodie_Jacket_ERDL
	{
	};
	class FOG_Hoodie_Jacket_Rolled_PERDL: FOG_Hoodie_Jacket_PERDL
	{
	};
	class FOG_Hoodie_Jacket_Rolled_Thrasher: FOG_Hoodie_Jacket_Thrasher
	{
	};
	class FOG_Hoodie_Jacket_Rolled_Supreme: FOG_Hoodie_Jacket_Supreme
	{
	};
	class FOG_Hoodie_Jacket_Rolled_MLG: FOG_Hoodie_Jacket_MLG
	{
	};
};
