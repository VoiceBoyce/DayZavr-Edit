class CfgPatches
{
	class FOG_UFP_Pants_Shit
	{
		units[]=
		{
			"FOG_UFP_Pants_Black",
			"FOG_UFP_Pants_Grey",
			"FOG_UFP_Pants_RG",
			"FOG_UFP_Pants_OD",
			"FOG_UFP_Pants_CB",
			"FOG_UFP_Pants_Tan",
			"FOG_UFP_Pants_Urban",
			"FOG_UFP_Pants_White",
			"FOG_UFP_Pants_Cum",
			"FOG_UFP_Pants_M81",
			"FOG_UFP_Pants_MC",
			"FOG_UFP_Pants_MCB",
			"FOG_UFP_Pants_DCU",
			"FOG_UFP_Pants_6CD",
			"FOG_UFP_Pants_ALP"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Characters",
			"DZ_Characters_Pants"
		};
	};
};
class CfgVehicles
{
	class Clothing;
	class FOG_UFP_Pants_ColorBase: Clothing
	{
		displayName="Urban Fighter Pants";
		descriptionShort="Urban Fighter Pants — тактические брюки с 50 слотами, оснащенные встроенными амортизирующими наколенниками и усиленным поясом с возможностью крепления дополнительного снаряжения. Изготовлены из прочной смесовой ткани с повышенной износостойкостью для городских операций | The Urban Fighter Pants are tactical trousers with 50 slots, featuring integrated shock-absorbing knee pads and a reinforced belt for extra gear attachment. Crafted from durable ripstop fabric with enhanced abrasion resistance for urban operations.";
		model="\FOG_MOD\Clothes\Pants\Urban_Fighter_pants\UFP_G.p3d";
		inventorySlot[]=
		{
			"Legs"
		};
		itemInfo[]=
		{
			"Clothing",
			"Legs"
		};
		weight=270;
		itemSize[]={4,3};
		itemsCargoSize[]={10,5};
		quickBarBonus=1;
		varWetMax=0.40000001;
		heatIsolation=0.60000002;
		ragQuantity=4;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		soundAttType="WoolShirt";
		hiddenSelections[]=
		{
			"camo_belt",
			"camo_kneepads",
			"camo_pants"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=1500;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"FOG_MOD\Clothes\Pants\FOG_Crye_G3Pants\Data\belt.rvmat",
								"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\hero_kneepad.rvmat",
								"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\UFP.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Clothes\Pants\FOG_Crye_G3Pants\Data\belt.rvmat",
								"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\hero_kneepad.rvmat",
								"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\UFP.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Clothes\Pants\FOG_Crye_G3Pants\Data\belt.rvmat",
								"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\hero_kneepad_damage.rvmat",
								"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\UFP_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Clothes\Pants\FOG_Crye_G3Pants\Data\belt.rvmat",
								"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\hero_kneepad_damage.rvmat",
								"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\UFP_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Clothes\Pants\FOG_Crye_G3Pants\Data\belt.rvmat",
								"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\hero_kneepad_destruct.rvmat",
								"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\UFP_destruct.rvmat"
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
			male="\FOG_MOD\Clothes\Pants\Urban_Fighter_pants\UFP_M.p3d";
			female="\FOG_MOD\Clothes\Pants\Urban_Fighter_pants\UFP_F.p3d";
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
	class FOG_UFP_Pants_Black: FOG_UFP_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Pants\FOG_Crye_G3Pants\Data\G3_Belt_Black_co.paa",
			"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\hero_kneepad_black_co.paa",
			"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\UFP_Black_co.paa"
		};
	};
	class FOG_UFP_Pants_Grey: FOG_UFP_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Pants\FOG_Crye_G3Pants\Data\G3_Belt_Grey_co.paa",
			"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\hero_kneepad_black_co.paa",
			"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\UFP_Grey_co.paa"
		};
	};
	class FOG_UFP_Pants_RG: FOG_UFP_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Pants\FOG_Crye_G3Pants\Data\G3_Belt_RG_co.paa",
			"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\hero_kneepad_black_co.paa",
			"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\UFP_RG_co.paa"
		};
	};
	class FOG_UFP_Pants_OD: FOG_UFP_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Pants\FOG_Crye_G3Pants\Data\G3_Belt_CB_co.paa",
			"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\hero_kneepad_black_co.paa",
			"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\UFP_OD_co.paa"
		};
	};
	class FOG_UFP_Pants_CB: FOG_UFP_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Pants\FOG_Crye_G3Pants\Data\G3_Belt_CB_co.paa",
			"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\hero_kneepad_black_co.paa",
			"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\UFP_CB_co.paa"
		};
	};
	class FOG_UFP_Pants_Tan: FOG_UFP_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Pants\FOG_Crye_G3Pants\Data\G3_Belt_Tan_co.paa",
			"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\hero_kneepad_black_co.paa",
			"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\UFP_Tan_co.paa"
		};
	};
	class FOG_UFP_Pants_Urban: FOG_UFP_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\Belt_Urban_co.paa",
			"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\hero_kneepad_black_co.paa",
			"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\UFP_Urban_co.paa"
		};
	};
	class FOG_UFP_Pants_White: FOG_UFP_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Pants\FOG_Crye_G3Pants\Data\G3_Belt_Black_co.paa",
			"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\hero_kneepad_black_co.paa",
			"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\UFP_White_co.paa"
		};
	};
	class FOG_UFP_Pants_Cum: FOG_UFP_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Pants\FOG_Crye_G3Pants\Data\G3_Belt_Black_co.paa",
			"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\hero_kneepad_black_co.paa",
			"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\UFP_Cum_co.paa"
		};
	};
	class FOG_UFP_Pants_M81: FOG_UFP_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Pants\FOG_Crye_G3Pants\Data\G3_Belt_Black_co.paa",
			"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\hero_kneepad_black_co.paa",
			"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\UFP_M81_co.paa"
		};
	};
	class FOG_UFP_Pants_MC: FOG_UFP_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Pants\FOG_Crye_G3Pants\Data\G3_Belt_Black_co.paa",
			"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\hero_kneepad_black_co.paa",
			"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\UFP_MC_co.paa"
		};
	};
	class FOG_UFP_Pants_MCB: FOG_UFP_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Pants\FOG_Crye_G3Pants\Data\G3_Belt_Black_co.paa",
			"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\hero_kneepad_black_co.paa",
			"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\UFP_MCB_co.paa"
		};
	};
	class FOG_UFP_Pants_DCU: FOG_UFP_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Pants\FOG_Crye_G3Pants\Data\G3_Belt_Tan_co.paa",
			"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\hero_kneepad_black_co.paa",
			"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\UFP_DCU_co.paa"
		};
	};
	class FOG_UFP_Pants_6CD: FOG_UFP_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Pants\FOG_Crye_G3Pants\Data\G3_Belt_Tan_co.paa",
			"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\hero_kneepad_black_co.paa",
			"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\UFP_6CD_co.paa"
		};
	};
	class FOG_UFP_Pants_ALP: FOG_UFP_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Pants\FOG_Crye_G3Pants\Data\G3_Belt_Grey_co.paa",
			"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\hero_kneepad_black_co.paa",
			"FOG_MOD\Clothes\Pants\Urban_Fighter_pants\data\UFP_alp_co.paa"
		};
	};
};
