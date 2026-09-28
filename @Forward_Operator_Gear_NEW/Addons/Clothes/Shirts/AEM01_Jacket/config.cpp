class CfgPatches
{
	class FOG_Shirts_AEM01_Jacket
	{
		units[]=
		{
			"FOG_AEM01_Jacket_Black",
			"FOG_AEM01_Jacket_Grey",
			"FOG_AEM01_Jacket_RG",
			"FOG_AEM01_Jacket_CB",
			"FOG_AEM01_Jacket_OD",
			"FOG_AEM01_Jacket_White",
			"FOG_AEM01_Jacket_Blue",
			"FOG_AEM01_Jacket_Red",
			"FOG_AEM01_Jacket_MC",
			"FOG_AEM01_Jacket_MC_Black",
			"FOG_AEM01_Jacket_MCB",
			"FOG_AEM01_Jacket_M81"
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
	class FOG_AEM01_Jacket_ColorBase: Clothing
	{
		displayName="AEM01 Jacket";
		descriptionShort="Куртка AEM01 — стильная и прочная куртка для активного отдыха. Известна своим комфортом, теплом и прочностью. | AEM01 Jacket is a stylish and durable jacket designed for various outdoor activities. Known for its comfort, warmth, and durability.";
		model="\FOG_MOD\Clothes\Shirts\AEM01_Jacket\FOG_AEM01_Jacket_G.p3d";
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
		itemsCargoSize[]={9,5};
		quickBarBonus=2;
		varWetMax=0.30000001;
		heatIsolation=0.60000002;
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
								"FOG_MOD\Clothes\Shirts\AEM01_Jacket\data\AEM_1_Jacket.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Clothes\Shirts\AEM01_Jacket\data\AEM_1_Jacket.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Clothes\Shirts\AEM01_Jacket\data\AEM_1_Jacket.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Clothes\Shirts\AEM01_Jacket\data\AEM_1_Jacket_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Clothes\Shirts\AEM01_Jacket\data\AEM_1_Jacket_destruct.rvmat"
							}
						}
					};
				};
			};
		};
		class ClothingTypes
		{
			male="\FOG_MOD\Clothes\Shirts\AEM01_Jacket\FOG_AEM01_Jacket_M.p3d";
			female="\FOG_MOD\Clothes\Shirts\AEM01_Jacket\FOG_AEM01_Jacket_F.p3d";
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
	class FOG_AEM01_Jacket_Black: FOG_AEM01_Jacket_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\AEM01_Jacket\data\AEM_1_Jacket_Black_co.paa"
		};
	};
	class FOG_AEM01_Jacket_Grey: FOG_AEM01_Jacket_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\AEM01_Jacket\data\AEM_1_Jacket_Gray_co.paa"
		};
	};
	class FOG_AEM01_Jacket_RG: FOG_AEM01_Jacket_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\AEM01_Jacket\data\AEM_1_Jacket_RG_co.paa"
		};
	};
	class FOG_AEM01_Jacket_CB: FOG_AEM01_Jacket_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\AEM01_Jacket\data\AEM_1_Jacket_CB_co.paa"
		};
	};
	class FOG_AEM01_Jacket_OD: FOG_AEM01_Jacket_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\AEM01_Jacket\data\AEM_1_Jacket_OD_co.paa"
		};
	};
	class FOG_AEM01_Jacket_White: FOG_AEM01_Jacket_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\AEM01_Jacket\data\AEM_1_Jacket_White_co.paa"
		};
	};
	class FOG_AEM01_Jacket_Blue: FOG_AEM01_Jacket_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\AEM01_Jacket\data\AEM_1_Jacket_Blue_co.paa"
		};
	};
	class FOG_AEM01_Jacket_Red: FOG_AEM01_Jacket_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\AEM01_Jacket\data\AEM_1_Jacket_Red_co.paa"
		};
	};
	class FOG_AEM01_Jacket_MC: FOG_AEM01_Jacket_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\AEM01_Jacket\data\AEM_1_Jacket_MC_co.paa"
		};
	};
	class FOG_AEM01_Jacket_MC_Black: FOG_AEM01_Jacket_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\AEM01_Jacket\data\AEM_1_Jacket_MC_Black_co.paa"
		};
	};
	class FOG_AEM01_Jacket_MCB: FOG_AEM01_Jacket_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\AEM01_Jacket\data\AEM_1_Jacket_MCB_co.paa"
		};
	};
	class FOG_AEM01_Jacket_M81: FOG_AEM01_Jacket_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\AEM01_Jacket\data\AEM_1_Jacket_M81_co.paa"
		};
	};
};
