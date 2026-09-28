class CfgPatches
{
	class FOG_Tactical_Hoodie_Shit
	{
		units[]=
		{
			"FOG_Tactical_Hoodie_Black",
			"FOG_Tactical_Hoodie_RG",
			"FOG_Tactical_Hoodie_CB",
			"FOG_Tactical_Hoodie_MC",
			"FOG_Tactical_Hoodie_MCB",
			"FOG_Tactical_Hoodie_UWU",
			"FOG_Tactical_Hoodie_ERDL",
			"FOG_Tactical_Hoodie_ERDL_Blk",
			"FOG_Tactical_Hoodie_White"
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
	class FOG_Tactical_Hoodie_ColorBase: Clothing
	{
		displayName="Tactical Hoodie";
		descriptionShort="A simple combat shirt with a hood, comes in a variety of colors.";
		model="\FOG_MOD\Clothes\Shirts\FOG_TacHoodie\FOG_TacHoodie_G.p3d";
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
		itemsCargoSize[]={7,6};
		quickBarBonus=2;
		noHeadgear = 1;
		varWetMax=0.40000001;
		heatIsolation=0.75;
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
			"FOG_MOD\Clothes\Shirts\FOG_TacHoodie\data\FOG_TacHoodie.rvmat"
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
								"FOG_MOD\Clothes\Shirts\FOG_TacHoodie\data\FOG_TacHoodie.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_TacHoodie\data\FOG_TacHoodie.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_TacHoodie\data\FOG_TacHoodie_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_TacHoodie\data\FOG_TacHoodie_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Clothes\Shirts\FOG_TacHoodie\data\FOG_TacHoodie_destruct.rvmat"
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
			male="\FOG_MOD\Clothes\Shirts\FOG_TacHoodie\FOG_TacHoodie_M.p3d";
			female="\FOG_MOD\Clothes\Shirts\FOG_TacHoodie\FOG_TacHoodie_F.p3d";
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
	class FOG_Tactical_Hoodie_Black: FOG_Tactical_Hoodie_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_TacHoodie\data\fog_tachoodie_black_co.paa"
		};
	};
	class FOG_Tactical_Hoodie_RG: FOG_Tactical_Hoodie_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_TacHoodie\data\fog_tachoodie_rg_co.paa"
		};
	};
	class FOG_Tactical_Hoodie_CB: FOG_Tactical_Hoodie_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_TacHoodie\data\fog_tachoodie_cb_co.paa"
		};
	};
	class FOG_Tactical_Hoodie_MC: FOG_Tactical_Hoodie_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_TacHoodie\data\fog_tachoodie_mc_co.paa"
		};
	};
	class FOG_Tactical_Hoodie_MCB: FOG_Tactical_Hoodie_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_TacHoodie\data\fog_tachoodie_mcb_co.paa"
		};
	};
	class FOG_Tactical_Hoodie_UWU: FOG_Tactical_Hoodie_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_TacHoodie\data\fog_tachoodie_uwu_co.paa"
		};
	};
	class FOG_Tactical_Hoodie_ERDL: FOG_Tactical_Hoodie_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_TacHoodie\data\fog_tachoodie_m81_co.paa"
		};
	};
	class FOG_Tactical_Hoodie_ERDL_Blk: FOG_Tactical_Hoodie_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_TacHoodie\data\fog_tachoodie_m81_blk_co.paa"
		};
	};
	class FOG_Tactical_Hoodie_White: FOG_Tactical_Hoodie_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\FOG_TacHoodie\data\fog_tachoodie_white_co.paa"
		};
	};
};
