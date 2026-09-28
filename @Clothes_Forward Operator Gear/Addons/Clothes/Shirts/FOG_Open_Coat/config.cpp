class CfgPatches
{
	class FOG_Shirt_Tactical_Coat_Shit
	{
		units[]=
		{
			"FOG_Shirt_Tactical_Coat_Black"
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
	class FOG_Shirt_Tactical_Coat_Base: Clothing
	{
		displayName="Tactical Winter Coat";
		descriptionShort="Tactical Winter Coat — исключительно редкий зимний тактический плащ, носится в слоте повязки, но предоставляет 20 дополнительных слотов. Оснащён усиленным капюшоном с подкладкой из медвежьего меха и многослойной арктической изоляцией, делающей его самым надёжным выбором для выживания в экстремальных зимних условиях | The Tactical Winter Coat is an exceptionally rare winter tactical cloak worn in the armband slot that unlocks 20 additional storage slots. Features a reinforced hood lined with bear fur and multi-layer arctic insulation, making it the ultimate choice for surviving extreme winter environments.";
		model="\FOG_MOD\Clothes\Shirts\FOG_Open_Coat\FOG_Winter_Coat_G.p3d";
		inventorySlot[]=
		{
			"Armband"
		};
		itemInfo[]=
		{
			"Clothing",
			"Body"
		};
		weight=870;
		itemSize[]={6,6};
		itemsCargoSize[]={5,4};
		quickBarBonus=5;
		varWetMax=0.249;
		heatIsolation=0.6;
		ragQuantity=4;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		soundAttType="WoolShirt";
		hiddenSelections[]=
		{
			"camo",
			"fur"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=1900;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_Open_Coat\data\opencoat.rvmat",
								"FOG_MOD\Clothes\Shirts\FOG_Open_Coat\data\fur_wind.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_Open_Coat\data\opencoat.rvmat",
								"FOG_MOD\Clothes\Shirts\FOG_Open_Coat\data\fur_wind.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_Open_Coat\data\opencoat_damage.rvmat",
								"FOG_MOD\Clothes\Shirts\FOG_Open_Coat\data\fur_wind.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_Open_Coat\data\opencoat_damage.rvmat",
								"FOG_MOD\Clothes\Shirts\FOG_Open_Coat\data\fur_wind.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_Open_Coat\data\opencoat_destruct.rvmat",
								"FOG_MOD\Clothes\Shirts\FOG_Open_Coat\data\fur_wind.rvmat"
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
					soundSet="SmershVest_pickup_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="SmershVest_drop_SoundSet";
					id=898;
				};
			};
		};
		class ClothingTypes
		{
			male="\FOG_MOD\Clothes\Shirts\FOG_Open_Coat\FOG_Winter_Coat_M.p3d";
			female="\FOG_MOD\Clothes\Shirts\FOG_Open_Coat\FOG_Winter_Coat_F.p3d";
		};
	};
	class FOG_Shirt_Tactical_Coat_Black: FOG_Shirt_Tactical_Coat_Base
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_Open_Coat\data\opencoat_black_co.paa",
			"FOG_MOD\Clothes\Shirts\FOG_Open_Coat\data\opencoat_fur_ca.paa"
		};
	};
};
