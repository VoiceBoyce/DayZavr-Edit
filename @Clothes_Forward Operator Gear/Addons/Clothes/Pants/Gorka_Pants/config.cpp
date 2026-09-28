class CfgPatches
{
	class FOG_Gorka_Pants_Shit
	{
		units[]=
		{
			"FOG_Gorka_Pants_Khaki",
			"FOG_Gorka_Pants_Black",
			"FOG_Gorka_Pants_White"
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
	class FOG_Gorka_Pants_ColorBase: Clothing
	{
		displayName="Gorka Professional";
		descriptionShort="Интегрированные наколенники. 64 слота. Gorka Professional это специализированные тактические брюки увеличенной вместимости с 64 слотами и интегрированными наколенниками, созданные для профессионалов, работающих в экстремальных условиях, где важны защита, удобство и быстрый доступ к снаряжению | The Gorka Professional are high-capacity tactical pants with 64 slots and built-in knee pads, designed for professionals operating in extreme environments where protection, comfort and rapid gear access are critical.";
		model="\FOG_MOD\Clothes\Pants\Gorka_Pants\Gorka_Pants_G.p3d";
		ContinuouActions[]=
		{
			"AT_WRING_CLOTHES"
		};
		inventorySlot[]=
		{
			"Legs"
		};
		simulation="clothing";
		itemInfo[]=
		{
			"Clothing",
			"Legs"
		};
		itemSize[]={5,4};
		itemsCargoSize[]={8,8};
		weight=300;
		ragQuantity=3;
		varWetMax=0.249;
		heatIsolation=0.75;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		quickBarBonus=2;
		hiddenSelections[]=
		{
			"camo_pants",
			"camo_pads"
		};
		hiddenSelectionsTextures[]={};
		class ClothingTypes
		{
			male="\FOG_MOD\Clothes\Pants\Gorka_Pants\Gorka_Pants_M.p3d";
			female="\FOG_MOD\Clothes\Pants\Gorka_Pants\Gorka_Pants_F.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=1200;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"FOG_MOD\Clothes\Pants\Gorka_Pants\data\Gorka_Pants.rvmat",
								"FOG_MOD\Clothes\Shirts\Gorka_Shirt\data\Gorka_pads.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Clothes\Pants\Gorka_Pants\data\Gorka_Pants.rvmat",
								"FOG_MOD\Clothes\Shirts\Gorka_Shirt\data\Gorka_pads.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Clothes\Pants\Gorka_Pants\data\Gorka_Pants_damage.rvmat",
								"FOG_MOD\Clothes\Shirts\Gorka_Shirt\data\Gorka_pads_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Clothes\Pants\Gorka_Pants\data\Gorka_Pants_damage.rvmat",
								"FOG_MOD\Clothes\Shirts\Gorka_Shirt\data\Gorka_pads_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Clothes\Pants\Gorka_Pants\data\Gorka_Pants_destruct.rvmat",
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
	class FOG_Gorka_Pants_Khaki: FOG_Gorka_Pants_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Pants\Gorka_Pants\data\spetsnaz_pants_khaki_co.paa",
			"FOG_MOD\Clothes\Shirts\Gorka_Shirt\data\kneepads_black_co.paa"
		};
	};
	class FOG_Gorka_Pants_Black: FOG_Gorka_Pants_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Pants\Gorka_Pants\data\spetsnaz_pants_Black_co.paa",
			"FOG_MOD\Clothes\Shirts\Gorka_Shirt\data\kneepads_black_co.paa"
		};
	};
	class FOG_Gorka_Pants_White: FOG_Gorka_Pants_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Pants\Gorka_Pants\data\spetsnaz_pants_White_co.paa",
			"FOG_MOD\Clothes\Shirts\Gorka_Shirt\data\kneepads_black_co.paa"
		};
	};
};
