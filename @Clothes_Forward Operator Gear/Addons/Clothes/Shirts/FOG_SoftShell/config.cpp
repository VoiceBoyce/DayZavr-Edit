class CfgPatches
{
	class FOG_Jacket_SoftShell_Shit
	{
		units[]=
		{
			"FOG_Jacket_SoftShell_Tan",
			"FOG_Jacket_SoftShell_Black",
			"FOG_Jacket_SoftShell_RG",
			"FOG_Jacket_SoftShell_CB",
			"FOG_Jacket_SoftShell_Grey",
			"FOG_Jacket_SoftShell_ERDL",
			"FOG_Jacket_SoftShell_MC",
			"FOG_Jacket_SoftShell_MCB",
			"FOG_Jacket_SoftShell_White"
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
	class FOG_Jacket_SoftShell_ColorBase: Clothing
	{
		displayName="Flexion Softshell Jacket";
		descriptionShort="Flexion Softshell Jacket — это военная куртка на молнии с 50 слотами, выполненная из прочного ветро- и влагозащитного материала. Универсальный дизайн и продуманная система карманов делают её подходящей как для тактических операций, так и для повседневного использования в сложных погодных условиях | The Flexion Softshell Jacket is a military-grade zippered jacket with 50 slots, crafted from durable wind and moisture-resistant fabric. Its versatile design and well-organized pocket system make it ideal for both tactical operations and everyday wear in harsh weather.";
		model="\FOG_MOD\Clothes\Shirts\FOG_SoftShell\FOG_SoftShell_G.p3d";
		inventorySlot[]=
		{
			"Body"
		};
		itemInfo[]=
		{
			"Clothing",
			"Body"
		};
		weight=670;
		itemSize[]={4,3};
		itemsCargoSize[]={10,5};
		quickBarBonus=2;
		varWetMax=0.1;
		heatIsolation=0.8;
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
			"FOG_MOD\Clothes\Shirts\FOG_SoftShell\data\FOG_SoftShell.rvmat"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=800;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_SoftShell\data\FOG_SoftShell.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_SoftShell\data\FOG_SoftShell.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_SoftShell\data\FOG_SoftShell_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_SoftShell\data\FOG_SoftShell_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_SoftShell\data\FOG_SoftShell_destruct.rvmat"
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
			male="\FOG_MOD\Clothes\Shirts\FOG_SoftShell\FOG_SoftShell_M.p3d";
			female="\FOG_MOD\Clothes\Shirts\FOG_SoftShell\FOG_SoftShell_F.p3d";
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
	class FOG_Jacket_SoftShell_Tan: FOG_Jacket_SoftShell_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_SoftShell\data\FOG_Softshell_Tan_co.paa"
		};
	};
	class FOG_Jacket_SoftShell_Black: FOG_Jacket_SoftShell_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_SoftShell\data\FOG_Softshell_BLK_co.paa"
		};
	};
	class FOG_Jacket_SoftShell_RG: FOG_Jacket_SoftShell_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_SoftShell\data\FOG_Softshell_RG_co.paa"
		};
	};
	class FOG_Jacket_SoftShell_CB: FOG_Jacket_SoftShell_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_SoftShell\data\FOG_Softshell_CB_co.paa"
		};
	};
	class FOG_Jacket_SoftShell_Grey: FOG_Jacket_SoftShell_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_SoftShell\data\FOG_Softshell_GRY_co.paa"
		};
	};
	class FOG_Jacket_SoftShell_ERDL: FOG_Jacket_SoftShell_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_SoftShell\data\FOG_Softshell_ERDL_co.paa"
		};
	};
	class FOG_Jacket_SoftShell_MC: FOG_Jacket_SoftShell_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_SoftShell\data\FOG_Softshell_MC_co.paa"
		};
	};
	class FOG_Jacket_SoftShell_MCB: FOG_Jacket_SoftShell_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_SoftShell\data\FOG_Softshell_MCB_co.paa"
		};
	};
	class FOG_Jacket_SoftShell_White: FOG_Jacket_SoftShell_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_SoftShell\data\FOG_Softshell_White_co.paa"
		};
	};
};
