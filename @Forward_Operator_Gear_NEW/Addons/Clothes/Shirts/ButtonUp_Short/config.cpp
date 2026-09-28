class CfgPatches
{
	class FOG_MOD_Shirt_ButtonUp
	{
		units[]=
		{
			"FOG_ButtonUp_Black",
			"FOG_ButtonUp_Grey",
			"FOG_ButtonUp_Red",
			"FOG_ButtonUp_RG",
			"FOG_ButtonUp_CB",
			"FOG_ButtonUp_Floral_YR",
			"FOG_ButtonUp_Floral_BW",
			"FOG_ButtonUp_Floral_BB",
			"FOG_ButtonUp_Floral_MD",
			"FOG_ButtonUp_Floral_Dark",
			"FOG_ButtonUp_Floral_Bright",
			"FOG_ButtonUp_Flamingo1",
			"FOG_ButtonUp_Flamingo2",
			"FOG_ButtonUp_SkinnyPalms",
			"FOG_ButtonUp_Waves",
			"FOG_ButtonUp_SunsetPalms",
			"FOG_ButtonUp_Clouds",
			"FOG_ButtonUp_MC",
			"FOG_ButtonUp_MCB"
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
	class FOG_ButtonUp_ColorBase: Clothing
	{
		displayName="Button Up Short Sleeve";
		descriptionShort="Button Up Short Sleeve — это тактическая рубашка с коротким рукавом на пуговицах, оснащённая 35 слотами для скрытого ношения снаряжения, выполненная из лёгкого дышащего материала с УФ-защитой для комфорта в жарком климате | The Button Up Short Sleeve is a tactical short-sleeved button-up shirt featuring 35 concealed carry slots, crafted from lightweight breathable fabric with UV protection for optimal comfort in hot weather conditions.";
		model="\FOG_MOD\Clothes\Shirts\ButtonUp_Short\FOG_ButtonUp_G.p3d";
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
		itemSize[]={3,3};
		itemsCargoSize[]={7,5};
		quickBarBonus=1;
		varWetMax=0.60000002;
		heatIsolation=0.44999999;
		ragQuantity=2;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		soundAttType="WoolShirt";
		hiddenSelections[]=
		{
			"camo",
			"personality"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Clothes\Shirts\ButtonUp_Short\data\ButtonUp.rvmat"
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
								"FOG_MOD\Clothes\Shirts\ButtonUp_Short\data\ButtonUp.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Clothes\Shirts\ButtonUp_Short\data\ButtonUp.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Clothes\Shirts\ButtonUp_Short\data\ButtonUp.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Clothes\Shirts\ButtonUp_Short\data\ButtonUp_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Clothes\Shirts\ButtonUp_Short\data\ButtonUp_destruct.rvmat"
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
			male="\FOG_MOD\Clothes\Shirts\ButtonUp_Short\FOG_ButtonUp_M.p3d";
			female="\FOG_MOD\Clothes\Shirts\ButtonUp_Short\FOG_ButtonUp_F.p3d";
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
	class FOG_ButtonUp_Black: FOG_ButtonUp_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\ButtonUp_Short\data\ButtonUp_Black_co.paa"
		};
	};
	class FOG_ButtonUp_Grey: FOG_ButtonUp_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\ButtonUp_Short\data\ButtonUp_Grey_co.paa"
		};
	};
	class FOG_ButtonUp_Red: FOG_ButtonUp_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\ButtonUp_Short\data\ButtonUp_Red_co.paa"
		};
	};
	class FOG_ButtonUp_RG: FOG_ButtonUp_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\ButtonUp_Short\data\ButtonUp_RG_co.paa"
		};
	};
	class FOG_ButtonUp_CB: FOG_ButtonUp_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\ButtonUp_Short\data\ButtonUp_CB_co.paa"
		};
	};
	class FOG_ButtonUp_Floral_YR: FOG_ButtonUp_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\ButtonUp_Short\data\ButtonUp_Floral_YellowRed_co.paa"
		};
	};
	class FOG_ButtonUp_Floral_BW: FOG_ButtonUp_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\ButtonUp_Short\data\ButtonUp_Floral_BlueWhite_co.paa"
		};
	};
	class FOG_ButtonUp_Floral_BB: FOG_ButtonUp_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\ButtonUp_Short\data\ButtonUp_Floral_BlackBlue_co.paa"
		};
	};
	class FOG_ButtonUp_Floral_MD: FOG_ButtonUp_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\ButtonUp_Short\data\ButtonUp_FloralMultiDark_co.paa"
		};
	};
	class FOG_ButtonUp_Floral_Dark: FOG_ButtonUp_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\ButtonUp_Short\data\ButtonUp_FloralDark_co.paa"
		};
	};
	class FOG_ButtonUp_Floral_Bright: FOG_ButtonUp_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\ButtonUp_Short\data\ButtonUp_FloralBright_co.paa"
		};
	};
	class FOG_ButtonUp_Flamingo1: FOG_ButtonUp_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\ButtonUp_Short\data\ButtonUp_Flamingoman1_co.paa"
		};
	};
	class FOG_ButtonUp_Flamingo2: FOG_ButtonUp_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\ButtonUp_Short\data\ButtonUp_Flamingoman2_co.paa"
		};
	};
	class FOG_ButtonUp_SkinnyPalms: FOG_ButtonUp_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\ButtonUp_Short\data\ButtonUp_SkinnyPalms_co.paa"
		};
	};
	class FOG_ButtonUp_Waves: FOG_ButtonUp_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\ButtonUp_Short\data\ButtonUp_waves_co.paa"
		};
	};
	class FOG_ButtonUp_SunsetPalms: FOG_ButtonUp_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\ButtonUp_Short\data\ButtonUp_SunsetPalms_co.paa"
		};
	};
	class FOG_ButtonUp_Clouds: FOG_ButtonUp_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\ButtonUp_Short\data\ButtonUp_clouds_co.paa"
		};
	};
	class FOG_ButtonUp_MC: FOG_ButtonUp_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\ButtonUp_Short\data\ButtonUp_MC_co.paa"
		};
	};
	class FOG_ButtonUp_MCB: FOG_ButtonUp_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\ButtonUp_Short\data\ButtonUp_MCB_co.paa"
		};
	};
};
