class CfgPatches
{
	class FOG_Cronen_Shirt_Shit
	{
		units[]=
		{
			"FOG_Cronen_Shirt_Black",
			"FOG_Cronen_Shirt_RG",
			"FOG_Cronen_Shirt_CB",
			"FOG_Cronen_Shirt_MC"
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
	class FOG_Cronen_Shirt_ColorBase: Clothing
	{
		displayName="Cronen Athletic Shirt";
		descriptionShort="Cronen Athletic Shirt это спортивно-тактическая рубашка с 42 слотами для снаряжения, выполненная из эластичного влагоотводящего материала с сетчатыми вставками для активного использования в любых условиях | The Cronen Athletic Shirt is a sport-tactical top with 42 gear slots, made from stretchy moisture-wicking fabric with mesh ventilation panels for high-intensity use in various environments.";
		model="\FOG_MOD\Clothes\Shirts\Cronen_athletic\cronen_shirt_G.p3d";
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
		itemsCargoSize[]={6,7};
		quickBarBonus=2;
		varWetMax=0.80000001;
		heatIsolation=0.60000001;
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
					hitpoints=720;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"FOG_MOD\Clothes\Shirts\Cronen_athletic\data\cronen_shirt.rvmat",
								""
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Clothes\Shirts\Cronen_athletic\data\cronen_shirt.rvmat",
								""
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Clothes\Shirts\Cronen_athletic\data\cronen_shirt_damage.rvmat",
								""
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Clothes\Shirts\Cronen_athletic\data\cronen_shirt_damage.rvmat",
								""
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Clothes\Shirts\Cronen_athletic\data\cronen_shirt_destruct.rvmat",
								""
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
			male="\FOG_MOD\Clothes\Shirts\Cronen_athletic\cronen_shirt_M.p3d";
			female="\FOG_MOD\Clothes\Shirts\Cronen_athletic\cronen_shirt_F.p3d";
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
	class FOG_Cronen_Shirt_Black: FOG_Cronen_Shirt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\Cronen_athletic\data\cronen_shirt_black_co.paa",
			""
		};
	};
	class FOG_Cronen_Shirt_RG: FOG_Cronen_Shirt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\Cronen_athletic\data\cronen_shirt_RG_co.paa",
			""
		};
	};
	class FOG_Cronen_Shirt_CB: FOG_Cronen_Shirt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\Cronen_athletic\data\cronen_shirt_CB_co.paa",
			""
		};
	};
	class FOG_Cronen_Shirt_MC: FOG_Cronen_Shirt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\Cronen_athletic\data\cronen_shirt_MC_co.paa",
			""
		};
	};
};
