class CfgPatches
{
	class FOG_Shirts_Rugby
	{
		units[]=
		{
			"FOG_RugbyShirt_BLK",
			"FOG_RugbyShirt_CB",
			"FOG_RugbyShirt_KHK",
			"FOG_RugbyShirt_MC",
			"FOG_RugbyShirt_MCB",
			"FOG_RugbyShirt_MCT",
			"FOG_RugbyShirt_RG"
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
	class FOG_RugbyShirt_ColorBase: Clothing
	{
		displayName="Velocity Systems Rugby Shirt";
		descriptionShort="Velocity Systems Rugby Shirt — невероятно вместительная тактическая рубаха на 40 слотов, ломающая стереотипы о вместимости одежды. Её инновационный крой и эластичные панели позволяют разместить снаряжение, сопоставимое по объёму с компактным рюкзаком, сохраняя при этом спортивный силуэт | The Velocity Systems Rugby Shirt is a mind-bending 40-slot tactical shirt that redefines gear capacity. Its revolutionary cut and stretch panels accommodate loadouts rivaling compact backpacks while maintaining an athletic profile.";
		model="\FOG_MOD\Clothes\Shirts\VSRugby\FOG_VSRugby_G.p3d";
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
		itemsCargoSize[]={10,4};
		quickBarBonus=6;
		varWetMax=0.5;
		heatIsolation=0.59999999;
		ragQuantity=4;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		soundAttType="WoolShirt";
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
					hitpoints=550;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"FOG_MOD\Clothes\Shirts\VSRugby\data\Rugby_shirt.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Clothes\Shirts\VSRugby\data\Rugby_shirt.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Clothes\Shirts\VSRugby\data\Rugby_shirt_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Clothes\Shirts\VSRugby\data\Rugby_shirt_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Clothes\Shirts\VSRugby\data\Rugby_shirt_destruct.rvmat"
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
			male="\FOG_MOD\Clothes\Shirts\VSRugby\FOG_VSRugby_M.p3d";
			female="\FOG_MOD\Clothes\Shirts\VSRugby\FOG_VSRugby_F.p3d";
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
	class FOG_RugbyShirt_BLK: FOG_RugbyShirt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\VSRugby\data\Rugby_shirt_BLK_co.paa"
		};
	};
	class FOG_RugbyShirt_CB: FOG_RugbyShirt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\VSRugby\data\Rugby_shirt_CB_co.paa"
		};
	};
	class FOG_RugbyShirt_KHK: FOG_RugbyShirt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\VSRugby\data\Rugby_shirt_KHK_co.paa"
		};
	};
	class FOG_RugbyShirt_MC: FOG_RugbyShirt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\VSRugby\data\Rugby_shirt_MC_co.paa"
		};
	};
	class FOG_RugbyShirt_MCB: FOG_RugbyShirt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\VSRugby\data\Rugby_shirt_MCB_co.paa"
		};
	};
	class FOG_RugbyShirt_MCT: FOG_RugbyShirt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\VSRugby\data\Rugby_shirt_MCT_co.paa"
		};
	};
	class FOG_RugbyShirt_RG: FOG_RugbyShirt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\VSRugby\data\Rugby_shirt_RG_co.paa"
		};
	};
};
