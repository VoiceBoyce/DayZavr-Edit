class CfgPatches
{
	class FOG_MTF_Shirt_Shit
	{
		units[]=
		{
			"FOG_MTF_Shirt_Grey",
			"FOG_MTF_Shirt_Black",
			"FOG_MTF_Shirt_Blue",
			"FOG_MTF_Shirt_RG",
			"FOG_MTF_Shirt_CB",
			"FOG_MTF_Shirt_KHK",
			"FOG_MTF_Shirt_ERDL",
			"FOG_MTF_Shirt_MC",
			"FOG_MTF_Shirt_MCB",
			"FOG_MTF_Shirt_FK",
			"FOG_MTF_Shirt_TS",
			"FOG_MTF_Shirt_White"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Characters",
			"DZ_Characters_Tops"
		};
	};
};
class CfgVehicles
{
	class Clothing;
	class FOG_MTF_Shirt_ColorBase: Clothing
	{
		displayName="MTF Uniform Top";
		descriptionShort="MTF Uniform Top — тактическая полевая рубаха с длинными рукавами на 54 слота, изготовленная из прочной смесовой ткани с усиленными зонами на локтях и плечах. Универсальный крой и система вентиляции обеспечивают комфорт при длительном ношении в любых климатических условиях | The MTF Uniform Top is a long-sleeved tactical field shirt with 54 slots, constructed from durable ripstop fabric with reinforced elbow and shoulder panels. Its versatile cut and ventilation system provide comfort during prolonged wear in diverse environments.";
		model="\FOG_MOD\Clothes\Shirts\MTF_Uniform_Shirt\MTF_Shirt_G.p3d";
		inventorySlot[]=
		{
			"Body"
		};
		attachments[]=
		{
			"FOG_small_patch"
		};
		itemInfo[]=
		{
			"Clothing",
			"Body"
		};
		weight=270;
		itemSize[]={4,3};
		itemsCargoSize[]={9,6};
		quickBarBonus=1;
		varWetMax=0.2;
		heatIsolation=0.7;
		ragQuantity=4;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		soundAttType="WoolShirt";
		hiddenSelections[]=
		{
			"camo"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Clothes\Shirts\MTF_Uniform_Shirt\data\MTF_Shirt.rvmat"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=1300;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"FOG_MOD\Clothes\Shirts\MTF_Uniform_Shirt\data\MTF_Shirt.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Clothes\Shirts\MTF_Uniform_Shirt\data\MTF_Shirt.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Clothes\Shirts\MTF_Uniform_Shirt\data\MTF_Shirt_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Clothes\Shirts\MTF_Uniform_Shirt\data\MTF_Shirt_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Clothes\Shirts\MTF_Uniform_Shirt\data\MTF_Shirt_destruct.rvmat"
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
			male="\FOG_MOD\Clothes\Shirts\MTF_Uniform_Shirt\MTF_Shirt_M.p3d";
			female="\FOG_MOD\Clothes\Shirts\MTF_Uniform_Shirt\MTF_Shirt_F.p3d";
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
	class FOG_MTF_Shirt_Grey: FOG_MTF_Shirt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\MTF_Uniform_Shirt\data\MTF_Shirt_Grey_co.paa"
		};
	};
	class FOG_MTF_Shirt_Black: FOG_MTF_Shirt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\MTF_Uniform_Shirt\data\MTF_Shirt_Black_co.paa"
		};
	};
	class FOG_MTF_Shirt_Blue: FOG_MTF_Shirt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\MTF_Uniform_Shirt\data\MTF_Shirt_Blue_co.paa"
		};
	};
	class FOG_MTF_Shirt_RG: FOG_MTF_Shirt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\MTF_Uniform_Shirt\data\MTF_Shirt_RG_co.paa"
		};
	};
	class FOG_MTF_Shirt_CB: FOG_MTF_Shirt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\MTF_Uniform_Shirt\data\MTF_Shirt_CB_co.paa"
		};
	};
	class FOG_MTF_Shirt_KHK: FOG_MTF_Shirt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\MTF_Uniform_Shirt\data\MTF_Shirt_KHK_co.paa"
		};
	};
	class FOG_MTF_Shirt_ERDL: FOG_MTF_Shirt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\MTF_Uniform_Shirt\data\MTF_Shirt_ERDL_co.paa"
		};
	};
	class FOG_MTF_Shirt_MC: FOG_MTF_Shirt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\MTF_Uniform_Shirt\data\MTF_Shirt_MC_co.paa"
		};
	};
	class FOG_MTF_Shirt_MCB: FOG_MTF_Shirt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\MTF_Uniform_Shirt\data\MTF_Shirt_MCB_co.paa"
		};
	};
	class FOG_MTF_Shirt_FK: FOG_MTF_Shirt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\MTF_Uniform_Shirt\data\MTF_Shirt_FK_co.paa"
		};
	};
	class FOG_MTF_Shirt_TS: FOG_MTF_Shirt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\MTF_Uniform_Shirt\data\MTF_Shirt_TS_co.paa"
		};
	};
	class FOG_MTF_Shirt_White: FOG_MTF_Shirt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\MTF_Uniform_Shirt\data\MTF_Shirt_White_co.paa"
		};
	};
};
