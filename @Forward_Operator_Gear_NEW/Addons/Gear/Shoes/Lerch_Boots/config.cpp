class CfgPatches
{
	class FOG_MOD_Shoes_Lerch
	{
		units[]=
		{
			"FOG_Lerch_Boots_Black",
			"FOG_Lerch_Boots_Black_Dirty",
			"FOG_Lerch_Boots_MC",
			"FOG_Lerch_Boots_MCB",
			"FOG_Lerch_Boots_Khk",
			"FOG_Lerch_Boots_RG",
			"FOG_Lerch_Boots_CB"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Characters",
			"DZ_Characters_Shoes"
		};
	};
};
class CfgVehicles
{
	class Clothing;
	class FOG_Lerch_Boots_ColorBase: Clothing
	{
		scope=0;
		displayName="Lerch Boots";
		descriptionShort="Lerch Boots — тактические ботинки со шнуровкой, изготовленные из водоотталкивающей кожи и износостойкого текстиля. Усиленная подошва с амортизацией и защитой от проколов обеспечивает устойчивость и комфорт при длительных перемещениях по пересечённой местности | The Lerch Boots are tactical lace-up boots crafted from water-repellent leather and abrasion-resistant textile. The reinforced puncture-proof sole with cushioning delivers stability and comfort during extended movement over rough terrain.";
		model="\FOG_MOD\Gear\Shoes\Lerch_Boots\Lerch_boots_G.p3d";
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
		weight=200;
		varWetMax=0.39000003;
		heatIsolation=0.60000001;
		repairableWithKits[]={5,2};
		repairCosts[]={25,25};
		soundAttType="Boots";
		hiddenSelections[]=
		{
			"camo"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Gear\Shoes\Lerch_Boots\data\lerchboots.rvmat"
		};
		class ClothingTypes
		{
			male="\FOG_MOD\Gear\Shoes\Lerch_Boots\Lerch_boots_M.p3d";
			female="\FOG_MOD\Gear\Shoes\Lerch_Boots\Lerch_boots_F.p3d";
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
								"FOG_MOD\Gear\Shoes\Lerch_Boots\data\lerchboots.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Gear\Shoes\Lerch_Boots\data\lerchboots.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Gear\Shoes\Lerch_Boots\data\lerchboots_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Gear\Shoes\Lerch_Boots\data\lerchboots_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Gear\Shoes\Lerch_Boots\data\lerchboots_destruct.rvmat"
							}
						}
					};
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
	class FOG_Lerch_Boots_Black: FOG_Lerch_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Shoes\Lerch_Boots\data\lerchboots_black_co.paa"
		};
	};
	class FOG_Lerch_Boots_Black_Dirty: FOG_Lerch_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Shoes\Lerch_Boots\data\lerchboots_black_dirty_co.paa"
		};
	};
	class FOG_Lerch_Boots_MC: FOG_Lerch_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Shoes\Lerch_Boots\data\lerchboots_mc_co.paa"
		};
	};
	class FOG_Lerch_Boots_MCB: FOG_Lerch_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Shoes\Lerch_Boots\data\lerchboots_mcb_co.paa"
		};
	};
	class FOG_Lerch_Boots_Khk: FOG_Lerch_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Shoes\Lerch_Boots\data\lerchboots_khk_co.paa"
		};
	};
	class FOG_Lerch_Boots_RG: FOG_Lerch_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Shoes\Lerch_Boots\data\lerchboots_rg_co.paa"
		};
	};
	class FOG_Lerch_Boots_CB: FOG_Lerch_Boots_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Shoes\Lerch_Boots\data\lerchboots_cb_co.paa"
		};
	};
};
