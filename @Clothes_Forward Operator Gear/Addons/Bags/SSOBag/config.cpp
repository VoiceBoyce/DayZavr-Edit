class CfgPatches
{
	class FOG_Bag_SSOBag
	{
		units[]=
		{
			"FOG_Bag_SSO_Tan",
			"FOG_Bag_SSO_Black",
			"FOG_Bag_SSO_MC",
			"FOG_Bag_SSO_CB",
			"FOG_Bag_SSO_MCB",
			"FOG_Bag_SSO_Toxic"
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
	class FOG_Bag_SSOBag_ColorBase: Clothing
	{
		scope=0;
		displayName="SSO Backpack";
		descriptionShort="Рюкзак SSO представляет собой универсальную 100-литровую модель нейтрального дизайна, выполненную из прочных износостойких материалов. Благодаря продуманной системе отделений и регулируемым лямкам обеспечивает комфортное размещение груза и удобство при длительном использовании. Подходит для туристических походов и повседневного применения. | The SSO backpack is a versatile 100-liter model with neutral design, made of durable wear-resistant materials. Featuring a well-thought-out compartment system and adjustable straps, it ensures comfortable load distribution and ease of use during extended wear. Suitable for hiking trips and everyday use.";
		model="\FOG_MOD\Bags\SSOBag\models\SSOBag_g.p3d";
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
		itemSize[]={10,10};
		quickBarBonus=3;
		itemsCargoSize[]={10,10};
		weight=1400;
		varWetMax=0.40000001;
		heatIsolation=0.68000001;
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
			"FOG_MOD\Bags\SSOBag\data\SSOBag_co.paa"
		};
		class ClothingTypes
		{
			male="\FOG_MOD\Bags\SSOBag\models\SSOBag_m.p3d";
			female="\FOG_MOD\Bags\SSOBag\models\SSOBag_m.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=1200;
					transferToAttachmentsCoef=0.5;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"FOG_MOD\Bags\SSOBag\data\SSOBag.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Bags\SSOBag\data\SSOBag.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Bags\SSOBag\data\SSOBag_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Bags\SSOBag\data\SSOBag_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Bags\SSOBag\data\SSOBag_destruct.rvmat"
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
	class FOG_Bag_SSO_Tan: FOG_Bag_SSOBag_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\SSOBag\data\SSOBag_co.paa"
		};
	};
	class FOG_Bag_SSO_Black: FOG_Bag_SSOBag_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\SSOBag\data\SSOBag_black_co.paa"
		};
	};
	class FOG_Bag_SSO_MC: FOG_Bag_SSOBag_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\SSOBag\data\SSOBag_mc_co.paa"
		};
	};
	class FOG_Bag_SSO_CB: FOG_Bag_SSOBag_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\SSOBag\data\SSOBag_cb_co.paa"
		};
	};
	class FOG_Bag_SSO_MCB: FOG_Bag_SSOBag_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\SSOBag\data\SSOBag_mcb_co.paa"
		};
	};
	class FOG_Bag_SSO_Toxic: FOG_Bag_SSOBag_ColorBase
	{
		scope=2;
		displayName="Toxic's SSO";
		descriptionShort="Yea he's toxic, but I love him anyways.";
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\SSOBag\data\SSOBag_rg_cb_co.paa"
		};
	};
};
