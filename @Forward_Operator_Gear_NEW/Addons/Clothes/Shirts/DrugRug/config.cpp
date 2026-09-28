class CfgPatches
{
	class FOG_Shirts_DrugRug
	{
		units[]=
		{
			"FOG_DrugRug_Black",
			"FOG_DrugRug_Blue",
			"FOG_DrugRug_Grey",
			"FOG_DrugRug_Purple",
			"FOG_DrugRug_Red",
			"FOG_DrugRug_BlackStripe",
			"FOG_DrugRug_RedStripe",
			"FOG_DrugRug_BlueStripe",
			"FOG_DrugRug_GreenStripe",
			"FOG_DrugRug_PurpleStripe",
			"FOG_DrugRug_MC"
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
	class FOG_DrugRug_ColorBase: Clothing
	{
		displayName="Drug Rug Hoodie";
		descriptionShort="Наркоманская худи — вязаный байя-свитер из толстой узорчатой ткани с мягкой пушистой текстурой. | A drug rug hoodie is a woven, Baja style pullover made from thick, patterned fabric with a soft, rugged feel. Known for their comfort and unique style.";
		model="\FOG_MOD\Clothes\Shirts\DrugRug\FOG_DrugRug_G.p3d";
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
		varWetMax=0.30000001;
		heatIsolation=0.89999998;
		ragQuantity=4;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		soundAttType="WoolShirt";
		hiddenSelections[]=
		{
			"camo"
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
								"FOG_MOD\Clothes\Shirts\DrugRug\data\DrugRug.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Clothes\Shirts\DrugRug\data\DrugRug.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Clothes\Shirts\DrugRug\data\DrugRug_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Clothes\Shirts\DrugRug\data\DrugRug_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Clothes\Shirts\DrugRug\data\DrugRug_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		class ClothingTypes
		{
			male="\FOG_MOD\Clothes\Shirts\DrugRug\FOG_DrugRug_M.p3d";
			female="\FOG_MOD\Clothes\Shirts\DrugRug\FOG_DrugRug_F.p3d";
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
	class FOG_DrugRug_Black: FOG_DrugRug_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\DrugRug\data\DrugRug_Black_co.paa"
		};
	};
	class FOG_DrugRug_Blue: FOG_DrugRug_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\DrugRug\data\DrugRug_Blue_co.paa"
		};
	};
	class FOG_DrugRug_Grey: FOG_DrugRug_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\DrugRug\data\DrugRug_Grey_co.paa"
		};
	};
	class FOG_DrugRug_Purple: FOG_DrugRug_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\DrugRug\data\DrugRug_Purple_co.paa"
		};
	};
	class FOG_DrugRug_Red: FOG_DrugRug_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\DrugRug\data\DrugRug_Red_co.paa"
		};
	};
	class FOG_DrugRug_BlackStripe: FOG_DrugRug_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\DrugRug\data\DrugRug_BlackStripe_co.paa"
		};
	};
	class FOG_DrugRug_RedStripe: FOG_DrugRug_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\DrugRug\data\DrugRug_RedStripe_co.paa"
		};
	};
	class FOG_DrugRug_BlueStripe: FOG_DrugRug_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\DrugRug\data\DrugRug_BlueStripe_co.paa"
		};
	};
	class FOG_DrugRug_GreenStripe: FOG_DrugRug_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\DrugRug\data\DrugRug_GreenStripe_co.paa"
		};
	};
	class FOG_DrugRug_PurpleStripe: FOG_DrugRug_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\DrugRug\data\DrugRug_PurpleStripe_co.paa"
		};
	};
	class FOG_DrugRug_MC: FOG_DrugRug_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\DrugRug\data\DrugRug_MC_co.paa"
		};
	};
};
