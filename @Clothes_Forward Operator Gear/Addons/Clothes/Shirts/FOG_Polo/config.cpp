class CfgPatches
{
	class FOG_Shirt_Polo_Shit
	{
		units[]=
		{
			"FOG_Shirt_Polo_Blue",
			"FOG_Shirt_Polo_NavyBlue",
			"FOG_Shirt_Polo_Black",
			"FOG_Shirt_Polo_OD",
			"FOG_Shirt_Polo_Trident"
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
	class FOG_Shirt_Polo_ColorBase: Clothing
	{
		displayName="Polo Shirt";
		descriptionShort="High Sleeves for high speeds.";
		model="\FOG_MOD\Clothes\Shirts\FOG_Polo\FOG_Polo_Tucked_G.p3d";
		inventorySlot[]=
		{
			"Body"
		};
		itemInfo[]=
		{
			"Clothing",
			"Body"
		};
		rotationFlags=34;
		weight=270;
		itemSize[]={3,4};
		itemsCargoSize[]={3,3};
		quickBarBonus=1;
		varWetMax=0.60000002;
		heatIsolation=0.40000001;
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
					hitpoints=300;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_Polo\Data\polo.rvmat",
								""
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_Polo\Data\polo.rvmat",
								""
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_Polo\Data\polo_damage.rvmat",
								""
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_Polo\Data\polo_damage.rvmat",
								""
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_Polo\Data\polo_destruct.rvmat",
								""
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
			male="\FOG_MOD\Clothes\Shirts\FOG_Polo\FOG_Polo_Tucked_M.p3d";
			female="\FOG_MOD\Clothes\Shirts\FOG_Polo\FOG_Polo_Tucked_F.p3d";
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
	class FOG_Shirt_Polo_Blue: FOG_Shirt_Polo_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_Polo\Data\polo_blue.paa",
			""
		};
	};
	class FOG_Shirt_Polo_NavyBlue: FOG_Shirt_Polo_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_Polo\Data\polo_navyblue.paa",
			""
		};
	};
	class FOG_Shirt_Polo_Black: FOG_Shirt_Polo_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_Polo\Data\polo_blk.paa",
			""
		};
	};
	class FOG_Shirt_Polo_OD: FOG_Shirt_Polo_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_Polo\Data\polo_od.paa",
			""
		};
	};
	class FOG_Shirt_Polo_Trident: FOG_Shirt_Polo_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_Polo\Data\polo_trident.paa",
			""
		};
	};
};
