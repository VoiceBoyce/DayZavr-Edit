class CfgPatches
{
	class FOG_MOD_Pants_FieldShorts
	{
		units[]=
		{
			"FOG_FieldShorts_White",
			"FOG_FieldShorts_Grey",
			"FOG_FieldShorts_RG",
			"FOG_FieldShorts_CB",
			"FOG_FieldShorts_KHK",
			"FOG_FieldShorts_Black",
			"FOG_FieldShorts_M81",
			"FOG_FieldShorts_MC",
			"FOG_FieldShorts_MC_Ranger",
			"FOG_FieldShorts_MCB",
			"FOG_FieldShorts_UCP",
			"FOG_FieldShorts_AOR1",
			"FOG_FieldShorts_AOR2"
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
	class FOG_FieldShorts_ColorBase: Clothing
	{
		displayName="Field Shorts";
		descriptionShort="Field Shorts это тактические шорты с 35 слотами для хранения снаряжения, выполненные из лёгкого дышащего материала с влагоотталкивающим покрытием, обеспечивающие удобство ношения в жарких условиях | The Field Shorts are tactical shorts with 35 gear storage slots, crafted from lightweight breathable material with moisture-repellent coating, designed for comfortable wear in hot climates.";
		model="\FOG_MOD\Clothes\Pants\FieldShorts\FOG_FieldShorts_G.p3d";
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
		itemsCargoSize[]={7,5};
		quickBarBonus=1;
		varWetMax=0.80000001;
		heatIsolation=0.2;
		ragQuantity=4;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		soundAttType="WoolShirt";
		hiddenSelections[]=
		{
			"camo",
			"personality"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=750;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"FOG_MOD\Clothes\Pants\FieldPants\data\FieldPants.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Clothes\Pants\FieldPants\data\FieldPants.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Clothes\Pants\FieldPants\data\FieldPants.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Clothes\Pants\FieldPants\data\FieldPants_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Clothes\Pants\FieldPants\data\FieldPants_destruct.rvmat"
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
						damage=0.85000002;
					};
					class Blood
					{
						damage=0.85000002;
					};
					class Shock
					{
						damage=0.85000002;
					};
				};
				class Infected
				{
					class Health
					{
						damage=0.85000002;
					};
					class Blood
					{
						damage=0.85000002;
					};
					class Shock
					{
						damage=0.85000002;
					};
				};
			};
		};
		class ClothingTypes
		{
			male="\FOG_MOD\Clothes\Pants\FieldShorts\FOG_FieldShorts_M.p3d";
			female="\FOG_MOD\Clothes\Pants\FieldShorts\FOG_FieldShorts_F.p3d";
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
	class FOG_FieldShorts_White: FOG_FieldShorts_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Pants\FieldPants\data\FOG_FieldPants_White_co.paa"
		};
	};
	class FOG_FieldShorts_Grey: FOG_FieldShorts_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Pants\FieldPants\data\FOG_FieldPants_Grey_co.paa"
		};
	};
	class FOG_FieldShorts_RG: FOG_FieldShorts_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Pants\FieldPants\data\FOG_FieldPants_RG_co.paa"
		};
	};
	class FOG_FieldShorts_CB: FOG_FieldShorts_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Pants\FieldPants\data\FOG_FieldPants_CB_co.paa"
		};
	};
	class FOG_FieldShorts_KHK: FOG_FieldShorts_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Pants\FieldPants\data\FOG_FieldPants_KHK_co.paa"
		};
	};
	class FOG_FieldShorts_Black: FOG_FieldShorts_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Pants\FieldPants\data\FOG_FieldPants_Black_co.paa"
		};
	};
	class FOG_FieldShorts_M81: FOG_FieldShorts_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Pants\FieldPants\data\FOG_FieldPants_M81_co.paa"
		};
	};
	class FOG_FieldShorts_MC: FOG_FieldShorts_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Pants\FieldPants\data\FOG_FieldPants_MC_co.paa"
		};
	};
	class FOG_FieldShorts_MC_Ranger: FOG_FieldShorts_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Pants\FieldPants\data\FOG_FieldPants_MC_Ranger_co.paa"
		};
	};
	class FOG_FieldShorts_MCB: FOG_FieldShorts_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Pants\FieldPants\data\FOG_FieldPants_MCB_co.paa"
		};
	};
	class FOG_FieldShorts_UCP: FOG_FieldShorts_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Pants\FieldPants\data\FOG_FieldPants_UCP_co.paa"
		};
	};
	class FOG_FieldShorts_AOR1: FOG_FieldShorts_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Pants\FieldPants\data\FOG_FieldPants_AOR1_co.paa"
		};
	};
	class FOG_FieldShorts_AOR2: FOG_FieldShorts_ColorBase
	{
		scope=2;
		visibilityModifier=0.69999999;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Pants\FieldPants\data\FOG_FieldPants_AOR2_co.paa"
		};
	};
};
