class CfgPatches
{
	class FOG_Bag_DrawBridge
	{
		units[]=
		{
			"FOG_Bag_DrawBridge_CB",
			"FOG_Bag_DrawBridge_RG",
			"FOG_Bag_DrawBridge_OD",
			"FOG_Bag_DrawBridge_Tan",
			"FOG_Bag_DrawBridge_Black",
			"FOG_Bag_DrawBridge_Grey",
			"FOG_Bag_DrawBridge_MC",
			"FOG_Bag_DrawBridge_MCAL",
			"FOG_Bag_DrawBridge_MCB",
			"FOG_Bag_DrawBridge_White"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters",
			"DZ_Scripts"
		};
	};
};
class cfgVehicles
{
	class Clothing;
	class FOG_Bag_DrawBridge_ColorBase: Clothing
	{
		scope=0;
		displayName="Drawbridge Bag";
		descriptionShort="Рюкзак Drawbridge - вместительная 90-литровая модель с нейтральным дизайном, выполненная из прочных износостойких материалов. Благодаря продуманной системе отделений и удобной системе креплений он оптимально распределяет нагрузку, подходя как для городского использования, так и для туристических походов. Объём в 90 литров позволяет удобно разместить всё необходимое снаряжение, а усиленная спинка и широкие регулируемые лямки обеспечивают комфорт даже при длительном ношении. | The Drawbridge backpack is a spacious 90-liter model with a neutral design, made from durable wear-resistant materials. Its well-designed compartment system and convenient attachment points optimally distribute the load, making it suitable for both urban use and hiking trips. The 90-liter capacity conveniently accommodates all necessary gear, while the reinforced back panel and wide adjustable straps ensure comfort even during prolonged wear.";
		model="\FOG_MOD\Bags\Drawbridge\models\DrawBridge_g.p3d";
		inventorySlot[]=
		{
			"Back"
		};
		itemInfo[]=
		{
			"Clothing",
			"Back"
		};
		simulation="clothing";
		vehicleClass="Clothing";
		itemSize[]={7,10};
		quickBarBonus=3;
		itemsCargoSize[]={9,10};
		weight=1200;
		varWetMax=0.249;
		heatIsolation=0.5;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		soundAttType="Outdoor";
		randomQuantity=4;
		canBeDigged=0;
		attachments[]={};
		hiddenSelections[]=
		{
			"camoflage"
		};
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\Drawbridge\data\DrawBridge_cb_co.paa"
		};
		class ClothingTypes
		{
			male="\FOG_MOD\Bags\Drawbridge\models\DrawBridge_m.p3d";
			female="\FOG_MOD\Bags\Drawbridge\models\DrawBridge_m.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=300;
					transferToAttachmentsCoef=0.5;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"FOG_MOD\Bags\Drawbridge\data\DrawBridge.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Bags\Drawbridge\data\DrawBridge.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Bags\Drawbridge\data\DrawBridge_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Bags\Drawbridge\data\DrawBridge_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Bags\Drawbridge\data\DrawBridge_destruct.rvmat"
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
				class pickUpItem_Light
				{
					soundSet="pickUpBackPack_Plastic_Light_SoundSet";
					id=796;
				};
				class pickUpItem
				{
					soundSet="pickUpBackPack_Plastic_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="taloonbag_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class FOG_Bag_DrawBridge_CB: FOG_Bag_DrawBridge_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\Drawbridge\data\DrawBridge_cb_co.paa"
		};
	};
	class FOG_Bag_DrawBridge_RG: FOG_Bag_DrawBridge_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\Drawbridge\data\DrawBridge_rg_co.paa"
		};
	};
	class FOG_Bag_DrawBridge_OD: FOG_Bag_DrawBridge_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\Drawbridge\data\DrawBridge_od_co.paa"
		};
	};
	class FOG_Bag_DrawBridge_Tan: FOG_Bag_DrawBridge_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\Drawbridge\data\DrawBridge_tan_co.paa"
		};
	};
	class FOG_Bag_DrawBridge_Black: FOG_Bag_DrawBridge_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\Drawbridge\data\DrawBridge_black_co.paa"
		};
	};
	class FOG_Bag_DrawBridge_Grey: FOG_Bag_DrawBridge_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\Drawbridge\data\DrawBridge_gry_co.paa"
		};
	};
	class FOG_Bag_DrawBridge_MC: FOG_Bag_DrawBridge_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\Drawbridge\data\DrawBridge_MC_co.paa"
		};
	};
	class FOG_Bag_DrawBridge_MCAL: FOG_Bag_DrawBridge_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\Drawbridge\data\DrawBridge_MCAL_co.paa"
		};
	};
	class FOG_Bag_DrawBridge_MCB: FOG_Bag_DrawBridge_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\Drawbridge\data\DrawBridge_MCB_co.paa"
		};
	};
	class FOG_Bag_DrawBridge_White: FOG_Bag_DrawBridge_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\Drawbridge\data\DrawBridge_White_co.paa"
		};
	};
};
