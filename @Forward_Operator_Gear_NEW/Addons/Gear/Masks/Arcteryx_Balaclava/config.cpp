class CfgPatches
{
	class FOG_MOD_Mask_ArcteryxBalaclava
	{
		units[]=
		{
			"FOG_Arcteryx_Balaclava_CB",
			"FOG_Arcteryx_Balaclava_RG",
			"FOG_Arcteryx_Balaclava_Grey",
			"FOG_Arcteryx_Balaclava_Black",
			"FOG_Arcteryx_Balaclava_White"
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
	class FOG_Arcteryx_Balaclava_ColorBase: Clothing
	{
		scope=0;
		displayName="Arcteryx Balaclava";
		descriptionShort="Arcteryx Balaclava — это тактическая балаклава премиум-класса, изготовленная из высокотехнологичного дышащего материала с ветрозащитными свойствами. Обеспечивает полную защиту лица и шеи в экстремальных условиях, сохраняя при этом отличную вентиляцию и комфорт при длительном ношении | The Arcteryx Balaclava is a premium tactical face mask crafted from advanced breathable windproof fabric. Delivers full-face and neck protection in harsh environments while maintaining optimal airflow and comfort during extended wear.";
		model="\FOG_MOD\Gear\Masks\Arcteryx_Balaclava\FOG_ArcteryxBalaclava_G.p3d";
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		inventorySlot[]=
		{
			"Mask"
		};
		vehicleClass="Clothing";
		simulation="clothing";
		itemInfo[]=
		{
			"Clothing",
			"Mask"
		};
		weight=5;
		itemSize[]={2,2};
		varWetMax=0.249;
		noNVStrap=0;
		noMask=0;
		noHelmet=0;
		noEyewear=0;
		heatIsolation=0.69999999;
		hiddenSelections[]=
		{
			"camo"
		};
		headSelectionsToHide[]=
		{
			"Clipping_Balaclava"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Gear\Masks\Arcteryx_Balaclava\data\arc_bala.rvmat"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=500;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"FOG_MOD\Gear\Masks\Arcteryx_Balaclava\data\arc_bala.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Gear\Masks\Arcteryx_Balaclava\data\arc_bala.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Gear\Masks\Arcteryx_Balaclava\data\arc_bala.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Gear\Masks\Arcteryx_Balaclava\data\arc_bala.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Gear\Masks\Arcteryx_Balaclava\data\arc_bala.rvmat"
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
                        damage=1;
                    };
                };
            };
		class ClothingTypes
		{
			male="\FOG_MOD\Gear\Masks\Arcteryx_Balaclava\FOG_ArcteryxBalaclava_M.p3d";
			female="\FOG_MOD\Gear\Masks\Arcteryx_Balaclava\FOG_ArcteryxBalaclava_F.p3d";
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
	class FOG_Arcteryx_Balaclava_CB: FOG_Arcteryx_Balaclava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Masks\Arcteryx_Balaclava\data\arc_bala_cb_co.paa"
		};
	};
	class FOG_Arcteryx_Balaclava_RG: FOG_Arcteryx_Balaclava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Masks\Arcteryx_Balaclava\data\arc_bala_rg_co.paa"
		};
	};
	class FOG_Arcteryx_Balaclava_Grey: FOG_Arcteryx_Balaclava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Masks\Arcteryx_Balaclava\data\arc_bala_grey_co.paa"
		};
	};
	class FOG_Arcteryx_Balaclava_Black: FOG_Arcteryx_Balaclava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Masks\Arcteryx_Balaclava\data\arc_bala_black_co.paa"
		};
	};
	class FOG_Arcteryx_Balaclava_White: FOG_Arcteryx_Balaclava_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Gear\Masks\Arcteryx_Balaclava\data\arc_bala_white_co.paa"
		};
	};
};
