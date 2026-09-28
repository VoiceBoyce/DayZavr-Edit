class CfgPatches
{
	class FOG_511_Shirt_Shit
	{
		units[]=
		{
			"FOG_511_Shirt_Flanel_Black",
			"FOG_511_Shirt_Flanel_Grey",
			"FOG_511_Shirt_Flanel_CB",
			"FOG_511_Shirt_Flanel_Red",
			"FOG_511_Shirt_Flanel_Blue",
			"FOG_511_Shirt_Flanel_Purple",
			"FOG_511_Shirt_Flanel_Green",
			"FOG_511_Shirt_Flanel",
			"FOG_511_Shirt_Flanel_BlackStriped",
			"FOG_511_Shirt_Flanel_RedStriped",
			"FOG_511_Shirt_Flanel_BlueStriped",
			"FOG_511_Shirt_Flanel_GreenStriped"
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
	class FOG_511_Shirt_ColorBase: Clothing
	{
		scope=0;
		displayName="Flanel Operators Shirt";
		descriptionShort="Flanel Operators Shirt — эта странноватая тактическая рубаха с 44 слотами словно бросает вызов законам физики: местные старожилы клянутся, что в её карманы помещается всё от боеприпасов до небольшого бревна, хотя как именно — остаётся загадкой даже для опытных оперативников | The Flanel Operators Shirt is this oddly tactical 44-slot flannel that defies conventional logic: local veterans swear you can fit anything from ammo to a literal log in its pockets, though how exactly remains a mystery even to seasoned operators.";
		model="\FOG_MOD\Clothes\Shirts\FOG_511_flanel\FOG_511_shirt_G.p3d";
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
		itemSize[]={4,4};
		itemsCargoSize[]={11,4};
		quickBarBonus=1;
		varWetMax=0.4;
		heatIsolation=0.4;
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
					hitpoints=750;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_511_flanel\data\FOG_511_Flanel.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_511_flanel\data\FOG_511_Flanel.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_511_flanel\data\FOG_511_Flanel_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_511_flanel\data\FOG_511_Flanel_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_511_flanel\data\FOG_511_Flanel_destruct.rvmat"
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
			male="\FOG_MOD\Clothes\Shirts\FOG_511_flanel\FOG_511_shirt_M.p3d";
			female="\FOG_MOD\Clothes\Shirts\FOG_511_flanel\FOG_511_shirt_F.p3d";
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
	class FOG_511_Shirt_Flanel_Black: FOG_511_Shirt_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_511_flanel\data\FOG_511_Flanel_Black_co.paa"
		};
	};
	class FOG_511_Shirt_Flanel_Grey: FOG_511_Shirt_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_511_flanel\data\FOG_511_Flanel_Grey_co.paa"
		};
	};
	class FOG_511_Shirt_Flanel_CB: FOG_511_Shirt_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_511_flanel\data\FOG_511_Flanel_CB_co.paa"
		};
	};
	class FOG_511_Shirt_Flanel_Red: FOG_511_Shirt_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_511_flanel\data\FOG_511_Flanel_Red_co.paa"
		};
	};
	class FOG_511_Shirt_Flanel_Blue: FOG_511_Shirt_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_511_flanel\data\FOG_511_Flanel_Blue_co.paa"
		};
	};
	class FOG_511_Shirt_Flanel_Purple: FOG_511_Shirt_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_511_flanel\data\FOG_511_Flanel_Purple_co.paa"
		};
	};
	class FOG_511_Shirt_Flanel_Green: FOG_511_Shirt_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_511_flanel\data\FOG_511_Flanel_Green_co.paa"
		};
	};
	class FOG_511_Shirt_Flanel: FOG_511_Shirt_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_511_flanel\data\FOG_511_FlanelTanStripe_co.paa"
		};
	};
	class FOG_511_Shirt_Flanel_BlackStriped: FOG_511_Shirt_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_511_flanel\data\FOG_511_BlackStripe_co.paa"
		};
	};
	class FOG_511_Shirt_Flanel_RedStriped: FOG_511_Shirt_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_511_flanel\data\FOG_511_RedStripe_co.paa"
		};
	};
	class FOG_511_Shirt_Flanel_BlueStriped: FOG_511_Shirt_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_511_flanel\data\FOG_511_BlueStripe_co.paa"
		};
	};
	class FOG_511_Shirt_Flanel_GreenStriped: FOG_511_Shirt_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_511_flanel\data\FOG_511_GreenStripe_co.paa"
		};
	};
};
