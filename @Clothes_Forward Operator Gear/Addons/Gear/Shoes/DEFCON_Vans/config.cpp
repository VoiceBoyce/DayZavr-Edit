class CfgPatches
{
	class FOG_MOD_DEFCON_Shoes
	{
		units[]=
		{
			"FOG_DEFCONVans_Shoes_Black",
			"FOG_DEFCONVans_Shoes_Grey",
			"FOG_DEFCONVans_Shoes_RG",
			"FOG_DEFCONVans_Shoes_CB",
			"FOG_DEFCONVans_Shoes_KHK",
			"FOG_DEFCONVans_Shoes_Blue",
			"FOG_DEFCONVans_Shoes_Red",
			"FOG_DEFCONVans_Shoes_MC",
			"FOG_DEFCONVans_Shoes_MCB",
			"FOG_DEFCONVans_Shoes_MCT",
			"FOG_DEFCONVans_Shoes_MCAL"
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
	class FOG_DEFCONVans_Shoes_ColorBase: Clothing
	{
		scope=0;
		displayName="DEFCON Vans";
		descriptionShort="DEFCON Vans — сверхпрочные тактические кроссовки, созданные для скоростного перемещения в городской среде. Армированная подошва и усиленные мысы выдерживают экстремальные нагрузки, сохраняя легкость спортивной обуви | The DEFCON Vans are ultra-durable tactical sneakers engineered for rapid urban movement. Reinforced soles and toe caps withstand extreme stress while maintaining athletic shoe lightness.";
		model="\FOG_MOD\Gear\Shoes\DEFCON_Vans\FOG_DEFCON_Vans_G.p3d";
		inventorySlot[]=
		{
			"Feet"
		};
		itemInfo[]=
		{
			"Clothing",
			"Feet"
		};
		itemSize[]={3,3};
		weight=400;
		varWetMax=0.59000003;
		heatIsolation=0.75;
		repairableWithKits[]={5,2};
		repairCosts[]={25,25};
		soundAttType="Boots";
		hiddenSelections[]=
		{
			"camo"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Gear\Shoes\DEFCON_Vans\Data\DEFCON_Vans.rvmat"
		};
		class ClothingTypes
		{
			male="\FOG_MOD\Gear\Shoes\DEFCON_Vans\FOG_DEFCON_Vans_M.p3d";
			female="\FOG_MOD\Gear\Shoes\DEFCON_Vans\FOG_DEFCON_Vans_M.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=300;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"FOG_MOD\Gear\Shoes\DEFCON_Vans\Data\DEFCON_Vans.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Gear\Shoes\DEFCON_Vans\Data\DEFCON_Vans.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Gear\Shoes\DEFCON_Vans\Data\DEFCON_Vans.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Gear\Shoes\DEFCON_Vans\Data\DEFCON_Vans.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Gear\Shoes\DEFCON_Vans\Data\DEFCON_Vans.rvmat"
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
                        damage=0.7;
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
	class FOG_DEFCONVans_Shoes_Black: FOG_DEFCONVans_Shoes_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Shoes\DEFCON_Vans\Data\DEFCON_Vans_Black_co.paa"
		};
	};
	class FOG_DEFCONVans_Shoes_Grey: FOG_DEFCONVans_Shoes_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Shoes\DEFCON_Vans\Data\DEFCON_Vans_Grey_co.paa"
		};
	};
	class FOG_DEFCONVans_Shoes_RG: FOG_DEFCONVans_Shoes_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Shoes\DEFCON_Vans\Data\DEFCON_Vans_RG_co.paa"
		};
	};
	class FOG_DEFCONVans_Shoes_CB: FOG_DEFCONVans_Shoes_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Shoes\DEFCON_Vans\Data\DEFCON_Vans_CB_co.paa"
		};
	};
	class FOG_DEFCONVans_Shoes_KHK: FOG_DEFCONVans_Shoes_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Shoes\DEFCON_Vans\Data\DEFCON_Vans_KHK_co.paa"
		};
	};
	class FOG_DEFCONVans_Shoes_Blue: FOG_DEFCONVans_Shoes_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Shoes\DEFCON_Vans\Data\DEFCON_Vans_Blue_co.paa"
		};
	};
	class FOG_DEFCONVans_Shoes_Red: FOG_DEFCONVans_Shoes_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Shoes\DEFCON_Vans\Data\DEFCON_Vans_Red_co.paa"
		};
	};
	class FOG_DEFCONVans_Shoes_MC: FOG_DEFCONVans_Shoes_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Shoes\DEFCON_Vans\Data\DEFCON_Vans_MC_co.paa"
		};
	};
	class FOG_DEFCONVans_Shoes_MCB: FOG_DEFCONVans_Shoes_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Shoes\DEFCON_Vans\Data\DEFCON_Vans_MCB_co.paa"
		};
	};
	class FOG_DEFCONVans_Shoes_MCT: FOG_DEFCONVans_Shoes_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Shoes\DEFCON_Vans\Data\DEFCON_Vans_MCT_co.paa"
		};
	};
	class FOG_DEFCONVans_Shoes_MCAL: FOG_DEFCONVans_Shoes_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Shoes\DEFCON_Vans\Data\DEFCON_Vans_MCAL_co.paa"
		};
	};
};
