class CfgPatches
{
	class FOG_MOD_Shirt_L9Patagonia
	{
		units[]=
		{
			"FOG_L9Patagonia_Black",
			"FOG_L9Patagonia_Grey",
			"FOG_L9Patagonia_White",
			"FOG_L9Patagonia_CB",
			"FOG_L9Patagonia_RG",
			"FOG_L9Patagonia_Blue",
			"FOG_L9Patagonia_Red",
			"FOG_L9Patagonia_KHK",
			"FOG_L9Patagonia_MC",
			"FOG_L9Patagonia_MCB",
			"FOG_L9Patagonia_MCT",
			"FOG_L9Patagonia_MCA",
			"FOG_L9Patagonia_MCAL",
			"FOG_L9Patagonia_AMCU",
			"FOG_L9Patagonia_M81",
			"FOG_L9Patagonia_M81Y",
			"FOG_L9Patagonia_WDTGR",
			"FOG_L9Patagonia_BTGR",
			"FOG_L9Patagonia_RTGR",
			"FOG_L9Patagonia_KBTGR",
			"FOG_L9Patagonia_UCP",
			"FOG_L9Patagonia_TBLOCK",
			"FOG_L9Patagonia_RDB",
			"FOG_L9Patagonia_MARPAT",
			"FOG_L9Patagonia_DNC",
			"FOG_L9Patagonia_DCU",
			"FOG_L9Patagonia_ATACSAU",
			"FOG_L9Patagonia_ATACSFG",
			"FOG_L9Patagonia_AOR1",
			"FOG_L9Patagonia_AOR2",
			"FOG_L9Patagonia_ALPF"
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
	class FOG_L9Patagonia_ColorBase: Clothing
	{
		displayName="L9 Patagonia Field Shirt";
		descriptionShort="L9 Patagonia Field Shirt — это прочная полевая рубаха с 49 слотами и закатанными рукавами, изготовленная из износостойкой смесовой ткани с усиленными швами. Идеальный баланс между тактической функциональностью и полевым комфортом для длительного ношения в любых условиях | The L9 Patagonia Field Shirt is a durable 49-slot field shirt with rolled-up sleeves, crafted from rugged ripstop fabric with reinforced stitching. Perfectly balances tactical utility with field comfort for extended wear in all environments.";
		model="\FOG_MOD\Clothes\Shirts\L9_Patagonia\FOG_L9Pata_Field_G.p3d";
		inventorySlot[]=
		{
			"Body"
		};
		itemInfo[]=
		{
			"Clothing",
			"Body"
		};
		weight=800;
		itemSize[]={4,3};
		itemsCargoSize[]={7,7};
		quickBarBonus=2;
		ragQuantity=3;
		varWetMax=0.3;
		heatIsolation=0.75;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		soundAttType="ChemlonDress";
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
					hitpoints=1250;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"FOG_MOD\Clothes\Shirts\L9_Patagonia\data\Jacket_Patagonia.rvmat",
								""
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Clothes\Shirts\L9_Patagonia\data\Jacket_Patagonia.rvmat",
								""
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Clothes\Shirts\L9_Patagonia\data\Jacket_Patagonia.rvmat",
								""
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Clothes\Shirts\L9_Patagonia\data\Jacket_Patagonia_damage.rvmat",
								""
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Clothes\Shirts\L9_Patagonia\data\Jacket_Patagonia_destruct.rvmat",
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
			male="\FOG_MOD\Clothes\Shirts\L9_Patagonia\FOG_L9Pata_Field_M.p3d";
			female="\FOG_MOD\Clothes\Shirts\L9_Patagonia\FOG_L9Pata_Field_F.p3d";
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
	class FOG_L9Patagonia_Black: FOG_L9Patagonia_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\L9_Patagonia\data\Jacket_Patagonia_Black_co.paa",
			""
		};
	};
	class FOG_L9Patagonia_Grey: FOG_L9Patagonia_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\L9_Patagonia\data\Jacket_Patagonia_Grey_co.paa",
			""
		};
	};
	class FOG_L9Patagonia_White: FOG_L9Patagonia_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\L9_Patagonia\data\Jacket_Patagonia_White_co.paa",
			""
		};
	};
	class FOG_L9Patagonia_CB: FOG_L9Patagonia_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\L9_Patagonia\data\Jacket_Patagonia_CB_co.paa",
			""
		};
	};
	class FOG_L9Patagonia_RG: FOG_L9Patagonia_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\L9_Patagonia\data\Jacket_Patagonia_RG_co.paa",
			""
		};
	};
	class FOG_L9Patagonia_Blue: FOG_L9Patagonia_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\L9_Patagonia\data\Jacket_Patagonia_Blue_co.paa",
			""
		};
	};
	class FOG_L9Patagonia_Red: FOG_L9Patagonia_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\L9_Patagonia\data\Jacket_Patagonia_Gripper_co.paa",
			""
		};
	};
	class FOG_L9Patagonia_KHK: FOG_L9Patagonia_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\L9_Patagonia\data\Jacket_Patagonia_KHK_co.paa",
			""
		};
	};
	class FOG_L9Patagonia_MC: FOG_L9Patagonia_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\L9_Patagonia\data\Jacket_Patagonia_MC_co.paa",
			""
		};
	};
	class FOG_L9Patagonia_MCB: FOG_L9Patagonia_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\L9_Patagonia\data\Jacket_Patagonia_MCB_co.paa",
			""
		};
	};
	class FOG_L9Patagonia_MCT: FOG_L9Patagonia_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\L9_Patagonia\data\Jacket_Patagonia_MCT_co.paa",
			""
		};
	};
	class FOG_L9Patagonia_MCA: FOG_L9Patagonia_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\L9_Patagonia\data\Jacket_Patagonia_MCA_co.paa",
			""
		};
	};
	class FOG_L9Patagonia_MCAL: FOG_L9Patagonia_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\L9_Patagonia\data\Jacket_Patagonia_MCAL_co.paa",
			""
		};
	};
	class FOG_L9Patagonia_AMCU: FOG_L9Patagonia_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\L9_Patagonia\data\Jacket_Patagonia_AMCU_co.paa",
			""
		};
	};
	class FOG_L9Patagonia_M81: FOG_L9Patagonia_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\L9_Patagonia\data\Jacket_Patagonia_M81_co.paa",
			""
		};
	};
	class FOG_L9Patagonia_M81Y: FOG_L9Patagonia_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\L9_Patagonia\data\Jacket_Patagonia_M81_Yellow_co.paa",
			""
		};
	};
	class FOG_L9Patagonia_WDTGR: FOG_L9Patagonia_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\L9_Patagonia\data\Jacket_Patagonia_WDTGR_co.paa",
			""
		};
	};
	class FOG_L9Patagonia_BTGR: FOG_L9Patagonia_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\L9_Patagonia\data\Jacket_Patagonia_BTGR_co.paa",
			""
		};
	};
	class FOG_L9Patagonia_RTGR: FOG_L9Patagonia_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\L9_Patagonia\data\Jacket_Patagonia_RTGR_co.paa",
			""
		};
	};
	class FOG_L9Patagonia_KBTGR: FOG_L9Patagonia_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\L9_Patagonia\data\Jacket_Patagonia_KBTGR_co.paa",
			""
		};
	};
	class FOG_L9Patagonia_UCP: FOG_L9Patagonia_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\L9_Patagonia\data\Jacket_Patagonia_UCP_co.paa",
			""
		};
	};
	class FOG_L9Patagonia_TBLOCK: FOG_L9Patagonia_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\L9_Patagonia\data\Jacket_Patagonia_TBLOCK_co.paa",
			""
		};
	};
	class FOG_L9Patagonia_RDB: FOG_L9Patagonia_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\L9_Patagonia\data\Jacket_Patagonia_RDB_co.paa",
			""
		};
	};
	class FOG_L9Patagonia_MARPAT: FOG_L9Patagonia_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\L9_Patagonia\data\Jacket_Patagonia_MARPAT_co.paa",
			""
		};
	};
	class FOG_L9Patagonia_DNC: FOG_L9Patagonia_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\L9_Patagonia\data\Jacket_Patagonia_DNC_co.paa",
			""
		};
	};
	class FOG_L9Patagonia_DCU: FOG_L9Patagonia_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\L9_Patagonia\data\Jacket_Patagonia_DCU_co.paa",
			""
		};
	};
	class FOG_L9Patagonia_ATACSAU: FOG_L9Patagonia_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\L9_Patagonia\data\Jacket_Patagonia_ATACSAU_co.paa",
			""
		};
	};
	class FOG_L9Patagonia_ATACSFG: FOG_L9Patagonia_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\L9_Patagonia\data\Jacket_Patagonia_ATACSFG_co.paa",
			""
		};
	};
	class FOG_L9Patagonia_AOR1: FOG_L9Patagonia_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\L9_Patagonia\data\Jacket_Patagonia_AOR1_co.paa",
			""
		};
	};
	class FOG_L9Patagonia_AOR2: FOG_L9Patagonia_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\L9_Patagonia\data\Jacket_Patagonia_AOR2_co.paa",
			""
		};
	};
	class FOG_L9Patagonia_ALPF: FOG_L9Patagonia_ColorBase
	{
		scope=2;
		visibilityModifier=0.75;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Clothes\Shirts\L9_Patagonia\data\Jacket_Patagonia_ALPF_co.paa",
			""
		};
	};
};
