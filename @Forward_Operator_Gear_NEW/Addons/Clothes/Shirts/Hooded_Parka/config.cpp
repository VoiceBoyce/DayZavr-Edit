class CfgPatches
{
	class FOG_Hooded_Parka_Shit
	{
		units[]=
		{
			"FOG_Hooded_Parka_OG",
			"FOG_Hooded_Parka_Black",
			"FOG_Hooded_Parka_RG",
			"FOG_Hooded_Parka_CB",
			"FOG_Hooded_Parka_MC",
			"FOG_Hooded_Parka_ALP"
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
	class FOG_Hooded_Parka_ColorBase: Clothing
	{
		displayName="Hooded Parka";
		descriptionShort="Hooded Parka — это утеплённая арктическая куртка с 60 слотами для снаряжения, оснащённая съёмной пайтой и капюшоном с натуральным мехом для максимальной защиты от экстремальных холодов. Многослойная изоляция и ветрозащитная мембрана делают её идеальным выбором для зимних экспедиций | The Hooded Parka is a heavily insulated arctic jacket with 60 gear slots, featuring detachable fur trim and natural fur-lined hood for ultimate protection against extreme cold. Multi-layer insulation and windproof membrane make it perfect for winter expeditions.";
		model="\FOG_MOD\Clothes\Shirts\Hooded_Parka\Hooded_Parka_G.p3d";
		inventorySlot[]=
		{
			"Body"
		};
		itemInfo[]=
		{
			"Clothing",
			"Body"
		};
		weight=900;
		itemSize[]={4,3};
		itemsCargoSize[]={10,6};
		quickBarBonus=3;
		ragQuantity=3;
		varWetMax=0.249;
		heatIsolation=0.69999998;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		noHelmet=1;
		soundAttType="ChemlonDress";
		headSelectionsToHide[]=
		{
			"Clipping_Balaclava"
		};
		hiddenSelections[]=
		{
			"camo"
		};
		simpleHiddenSelections[]=
		{
			"selection_hoodup",
			"selection_hooddown"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=1200;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"FOG_MOD\Clothes\Shirts\Hooded_Parka\data\Hooded_Parka.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Clothes\Shirts\Hooded_Parka\data\Hooded_Parka.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Clothes\Shirts\Hooded_Parka\data\Hooded_Parka_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Clothes\Shirts\Hooded_Parka\data\Hooded_Parka_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Clothes\Shirts\Hooded_Parka\data\Hooded_Parka_destruct.rvmat"
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
			male="\FOG_MOD\Clothes\Shirts\Hooded_Parka\Hooded_Parka_M.p3d";
			female="\FOG_MOD\Clothes\Shirts\Hooded_Parka\Hooded_Parka_F.p3d";
		};
		class Protection
		{
			chemical=0.25;
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
	class FOG_Hooded_Parka_OG: FOG_Hooded_Parka_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\Hooded_Parka\data\Hooded_Parka_co.paa"
		};
	};
	class FOG_Hooded_Parka_Black: FOG_Hooded_Parka_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\Hooded_Parka\data\Hooded_Parka_black_co.paa"
		};
	};
	class FOG_Hooded_Parka_RG: FOG_Hooded_Parka_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\Hooded_Parka\data\Hooded_Parka_RG_co.paa"
		};
	};
	class FOG_Hooded_Parka_CB: FOG_Hooded_Parka_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\Hooded_Parka\data\Hooded_Parka_CB_co.paa"
		};
	};
	class FOG_Hooded_Parka_MC: FOG_Hooded_Parka_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\Hooded_Parka\data\Hooded_Parka_mc_co.paa"
		};
	};
	class FOG_Hooded_Parka_ALP: FOG_Hooded_Parka_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\Hooded_Parka\data\Hooded_Parka_alp_co.paa"
		};
	};
};
