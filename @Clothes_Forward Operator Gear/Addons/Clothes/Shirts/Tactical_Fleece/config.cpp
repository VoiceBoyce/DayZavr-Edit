class CfgPatches
{
	class FOG_Tactical_Fleece_Shit
	{
		units[]=
		{
			"FOG_Tactical_Fleece_Grey",
			"FOG_Tactical_Fleece_Black",
			"FOG_Tactical_Fleece_RG",
			"FOG_Tactical_Fleece_CB",
			"FOG_Tactical_Fleece_MC",
			"FOG_Tactical_Fleece_Arctic",
			"FOG_Tactical_Fleece_White"
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
	class FOG_Tactical_Fleece_ColorBase: Clothing
	{
		displayName="Tactical Fleece Jacket";
		descriptionShort="Tactical Fleece Jacket — утеплённая тактическая куртка на флисовой подкладке с 63 слотами для снаряжения, сочетающая терморегулирующие свойства и подвижность. Усиленные плечевые вставки и скрытые карманы делают её идеальной для зимних операций без потери манёвренности | The Tactical Fleece Jacket is an insulated tactical outer layer with 63 slots, blending thermal regulation with unrestricted movement. Reinforced shoulder panels and concealed pockets optimize it for cold-weather operations without compromising mobility.";
		model="\FOG_MOD\Clothes\Shirts\Tactical_Fleece\Tactical_Fleece_G.p3d";
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
		itemsCargoSize[]={7,9};
		quickBarBonus=4;
		varWetMax=0.30000001;
		heatIsolation=0.69999998;
		ragQuantity=4;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		soundAttType="WoolShirt";
		hiddenSelections[]=
		{
			"camo"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Clothes\Shirts\Tactical_Fleece\data\tactical_fleece.rvmat"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=1520;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"FOG_MOD\Clothes\Shirts\Tactical_Fleece\data\tactical_fleece.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Clothes\Shirts\Tactical_Fleece\data\tactical_fleece.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Clothes\Shirts\Tactical_Fleece\data\tactical_fleece_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Clothes\Shirts\Tactical_Fleece\data\tactical_fleece_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Clothes\Shirts\Tactical_Fleece\data\tactical_fleece_destruct.rvmat"
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
		};
		class ClothingTypes
		{
			male="\FOG_MOD\Clothes\Shirts\Tactical_Fleece\Tactical_Fleece_M.p3d";
			female="\FOG_MOD\Clothes\Shirts\Tactical_Fleece\Tactical_Fleece_F.p3d";
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
	class FOG_Tactical_Fleece_Grey: FOG_Tactical_Fleece_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\Tactical_Fleece\data\tactical_fleece_grey_co.paa"
		};
	};
	class FOG_Tactical_Fleece_Black: FOG_Tactical_Fleece_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\Tactical_Fleece\data\tactical_fleece_black_co.paa"
		};
	};
	class FOG_Tactical_Fleece_RG: FOG_Tactical_Fleece_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\Tactical_Fleece\data\tactical_fleece_RG_co.paa"
		};
	};
	class FOG_Tactical_Fleece_CB: FOG_Tactical_Fleece_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\Tactical_Fleece\data\tactical_fleece_CB_co.paa"
		};
	};
	class FOG_Tactical_Fleece_MC: FOG_Tactical_Fleece_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\Tactical_Fleece\data\tactical_fleece_MC_co.paa"
		};
	};
	class FOG_Tactical_Fleece_Arctic: FOG_Tactical_Fleece_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\Tactical_Fleece\data\tactical_fleece_arctic_co.paa"
		};
	};
	class FOG_Tactical_Fleece_White: FOG_Tactical_Fleece_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\Tactical_Fleece\data\tactical_fleece_White_co.paa"
		};
	};
};
