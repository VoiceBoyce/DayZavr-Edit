class CfgPatches
{
	class FOG_Bag_Trizip
	{
		units[]=
		{
			"FOG_Bag_Trizip_Puke",
			"FOG_Bag_Trizip_Black",
			"FOG_Bag_Trizip_RG",
			"FOG_Bag_Trizip_OD",
			"FOG_Bag_Trizip_CB",
			"FOG_Bag_Trizip_RGCB",
			"FOG_Bag_Trizip_MARPAT",
			"FOG_Bag_Trizip_ATACSFG",
			"FOG_Bag_Trizip_DCU",
			"FOG_Bag_Trizip_ERDL",
			"FOG_Bag_Trizip_MC"
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
	class FOG_Bag_Trizip_ColorBase: Clothing
	{
		scope=0;
		displayName="Trizip Bag";
		descriptionShort="Рюкзак Trizip представляет собой вытянутую 81-литровую модель нейтрального дизайна, изготовленную из прочных водоотталкивающих материалов. Продуманная конструкция с тремя отделениями обеспечивает удобную организацию снаряжения, а эргономичная система лямок позволяет комфортно переносить грузы в различных условиях. | The Trizip backpack is an elongated 81-liter model with neutral design, crafted from durable water-repellent materials. Its smart three-compartment layout offers convenient gear organization, while the ergonomic strap system ensures comfortable load carrying in diverse conditions.";
		model="\FOG_MOD\Bags\Trizip\models\Trizip_g.p3d";
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
		itemSize[]={9,9};
		quickBarBonus=3;
		itemsCargoSize[]={9,9};
		weight=600;
		varWetMax=0.60000002;
		heatIsolation=0.40000001;
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
			"FOG_MOD\Bags\Trizip\data\Trizip_co.paa"
		};
		class ClothingTypes
		{
			male="\FOG_MOD\Bags\Trizip\models\Trizip_m.p3d";
			female="\FOG_MOD\Bags\Trizip\models\Trizip_m.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=1500;
					transferToAttachmentsCoef=0.5;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"FOG_MOD\Bags\Trizip\data\Trizip.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Bags\Trizip\data\Trizip.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Bags\Trizip\data\Trizip_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Bags\Trizip\data\Trizip_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Bags\Trizip\data\Trizip_destruct.rvmat"
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
	class FOG_Bag_Trizip_Puke: FOG_Bag_Trizip_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\Trizip\data\Trizip_co.paa"
		};
	};
	class FOG_Bag_Trizip_Black: FOG_Bag_Trizip_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\Trizip\data\Trizip_black_co.paa"
		};
	};
	class FOG_Bag_Trizip_RG: FOG_Bag_Trizip_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\Trizip\data\Trizip_rg_co.paa"
		};
	};
	class FOG_Bag_Trizip_OD: FOG_Bag_Trizip_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\Trizip\data\Trizip_od_co.paa"
		};
	};
	class FOG_Bag_Trizip_CB: FOG_Bag_Trizip_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\Trizip\data\Trizip_cb_co.paa"
		};
	};
	class FOG_Bag_Trizip_RGCB: FOG_Bag_Trizip_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\Trizip\data\Trizip_rgcb_co.paa"
		};
	};
	class FOG_Bag_Trizip_MARPAT: FOG_Bag_Trizip_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\Trizip\data\Trizip_MARPAT_co.paa"
		};
	};
	class FOG_Bag_Trizip_ATACSFG: FOG_Bag_Trizip_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\Trizip\data\Trizip_ATACSFG_co.paa"
		};
	};
	class FOG_Bag_Trizip_DCU: FOG_Bag_Trizip_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\Trizip\data\Trizip_dcu_co.paa"
		};
	};
	class FOG_Bag_Trizip_ERDL: FOG_Bag_Trizip_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\Trizip\data\Trizip_erdl_co.paa"
		};
	};
	class FOG_Bag_Trizip_MC: FOG_Bag_Trizip_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\Trizip\data\Trizip_mc_co.paa"
		};
	};
};
