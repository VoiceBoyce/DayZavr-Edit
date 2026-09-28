class CfgPatches
{
	class FOG_Gorka_Jacket_Shit
	{
		units[]=
		{
			"FOG_Gorka_Jacket_Khaki",
			"FOG_Gorka_Jacket_Black",
			"FOG_Gorka_Jacket_Green",
			"FOG_Gorka_Jacket_Partizan",
			"FOG_Gorka_Jacket_Autumn",
			"FOG_Gorka_Jacket_Summer",
			"FOG_Gorka_Jacket_White"
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
	class FOG_Gorka_Jacket_ColorBase: Clothing
	{
		displayName="Gorka Jacket Professional";
		descriptionShort="Gorka Jacket Professional — профессиональная штормовая куртка с 80 слотами и встроенными анатомическими налокотниками, созданная для работы в экстремальных погодных условиях. Усиленная мембранная ткань с полной ветро- и влагозащитой, регулируемыми манжетами и системой вентиляции подходит для длительных операций в любом климате | The Gorka Jacket Professional is an elite storm jacket featuring 80 slots and integrated anatomical elbow pads, engineered for extreme weather operations. Reinforced membrane fabric with full wind/water protection, adjustable cuffs and ventilation system ensures performance during prolonged missions in any environment.";
		model="\FOG_MOD\Clothes\Shirts\Gorka_Shirt\Gorka_Shirt_G.p3d";
		inventorySlot[]=
		{
			"Body"
		};
		itemInfo[]=
		{
			"Clothing",
			"Body"
		};
		weight=600;
		itemSize[]={4,3};
		itemsCargoSize[]={8,10};
		quickBarBonus=2;
		ragQuantity=3;
		varWetMax=0.249;
		heatIsolation=0.45;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		soundAttType="ChemlonDress";
		hiddenSelections[]=
		{
			"camo_shirt",
			"camo_pads"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=2300;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"FOG_MOD\Clothes\Shirts\Gorka_Shirt\data\Gorka_Shirt.rvmat",
								"FOG_MOD\Clothes\Shirts\Gorka_Shirt\data\Gorka_pads.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Clothes\Shirts\Gorka_Shirt\data\Gorka_Shirt.rvmat",
								"FOG_MOD\Clothes\Shirts\Gorka_Shirt\data\Gorka_pads.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Clothes\Shirts\Gorka_Shirt\data\Gorka_Shirt_damage.rvmat",
								"FOG_MOD\Clothes\Shirts\Gorka_Shirt\data\Gorka_pads_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Clothes\Shirts\Gorka_Shirt\data\Gorka_Shirt_damage.rvmat",
								"FOG_MOD\Clothes\Shirts\Gorka_Shirt\data\Gorka_pads_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Clothes\Shirts\Gorka_Shirt\data\Gorka_Shirt_destruct.rvmat",
								"FOG_MOD\Clothes\Shirts\Gorka_Shirt\data\Gorka_pads_destruct.rvmat"
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
			male="\FOG_MOD\Clothes\Shirts\Gorka_Shirt\Gorka_Shirt_M.p3d";
			female="\FOG_MOD\Clothes\Shirts\Gorka_Shirt\Gorka_Shirt_F.p3d";
		};
		class Protection
		{
			chemical=0.25;
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
	class FOG_Gorka_Jacket_Khaki: FOG_Gorka_Jacket_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\Gorka_Shirt\data\spetsnaz_shirt_khaki_co.paa",
			"FOG_MOD\Clothes\Shirts\Gorka_Shirt\data\kneepads_black_co.paa"
		};
	};
	class FOG_Gorka_Jacket_Black: FOG_Gorka_Jacket_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\Gorka_Shirt\data\spetsnaz_shirt_Black_co.paa",
			"FOG_MOD\Clothes\Shirts\Gorka_Shirt\data\kneepads_black_co.paa"
		};
	};
	class FOG_Gorka_Jacket_Green: FOG_Gorka_Jacket_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\Gorka_Shirt\data\spetsnaz_shirt_green_co.paa",
			"FOG_MOD\Clothes\Shirts\Gorka_Shirt\data\kneepads_black_co.paa"
		};
	};
	class FOG_Gorka_Jacket_Partizan: FOG_Gorka_Jacket_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\Gorka_Shirt\data\spetsnaz_shirt_Partizan_co.paa",
			"FOG_MOD\Clothes\Shirts\Gorka_Shirt\data\kneepads_black_co.paa"
		};
	};
	class FOG_Gorka_Jacket_Autumn: FOG_Gorka_Jacket_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\Gorka_Shirt\data\spetsnaz_shirt_Autmn_co.paa",
			"FOG_MOD\Clothes\Shirts\Gorka_Shirt\data\kneepads_black_co.paa"
		};
	};
	class FOG_Gorka_Jacket_Summer: FOG_Gorka_Jacket_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\Gorka_Shirt\data\spetsnaz_shirt_Summer_co.paa",
			"FOG_MOD\Clothes\Shirts\Gorka_Shirt\data\kneepads_black_co.paa"
		};
	};
	class FOG_Gorka_Jacket_White: FOG_Gorka_Jacket_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\Gorka_Shirt\data\spetsnaz_shirt_White_co.paa",
			"FOG_MOD\Clothes\Shirts\Gorka_Shirt\data\kneepads_black_co.paa"
		};
	};
};
