class CfgPatches
{
	class FOG_Gear_Vest_DPC
	{
		units[]=
		{
			"FOG_Vest_DPC_CB",
			"FOG_Vest_DPC_RG",
			"FOG_Vest_DPC_Black",
			"FOG_Vest_DPC_Grey",
			"FOG_Vest_DPC_MC",
			"FOG_Vest_DPC_AOR2"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters",
			"DZ_Scripts",
			"DZ_Characters_Vests"
		};
	};
};
class cfgVehicles
{
	class Clothing;
	class FOG_Vest_DPC_ColorBase: Clothing
	{
		scope=0;
		displayName="Dynamic Principles Plate Carrier";
		descriptionShort="Dynamic Principles Carrier (DPC) — современный тактический жилет, разработанный для универсальности и комфорта в условиях повышенного риска. Изготовлен из высококачественных материалов, обеспечивающих долговечность и надёжную защиту. | The Dynamic Principles Carrier (DPC) is a modern tactical vest designed for versatility and comfort in high-stakes environments. Made from high-quality materials, the DPC offers durability and reliable protection.";
		model="FOG_MOD\Vests\DPC\FOG_DPC_G.p3d";
		attachments[]=
		{
			"WalkieTalkie",
			"VestPouch",
			"FOG_big_patch",
			"FOG_vest_panel",
			"FOG_tourniquet",
			"FOG_vest_belly"
		};
		inventorySlot[]=
		{
			"Vest"
		};
		itemInfo[]=
		{
			"Clothing",
			"Vest"
		};
		quickBarBonus=2;
		weight=6000;
		itemSize[]={5,5};
		varWetMax=0.249;
		heatIsolation=0.5;
		repairableWithKits[]={3};
		repairCosts[]={25};
		hiddenSelections[]=
		{
			"camo"
		};
		simpleHiddenSelections[]=
		{
			"selection_radio"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Vests\DPC\data\DPC_Vest.rvmat"
		};
		class ClothingTypes
		{
			male="FOG_MOD\Vests\DPC\FOG_DPC_M.p3d";
			female="FOG_MOD\Vests\DPC\FOG_DPC_F.p3d";
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
								"FOG_MOD\Vests\DPC\data\DPC_Vest.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Vests\DPC\data\DPC_Vest.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Vests\DPC\data\DPC_Vest.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Vests\DPC\data\DPC_Vest.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Vests\DPC\data\DPC_Vest.rvmat"
							}
						}
					};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage=0.25;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.60000002;
					};
				};
				class Melee
				{
					class Health
					{
						damage=0.25;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.25;
					};
				};
				class Infected
				{
					class Health
					{
						damage=0.25;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.25;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage=0.25;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=0.25;
					};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem
				{
					soundSet="SmershVest_pickup_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="SmershVest_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class FOG_Vest_DPC_CB: FOG_Vest_DPC_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\DPC\data\DPC_Vest_CB_co.paa"
		};
	};
	class FOG_Vest_DPC_RG: FOG_Vest_DPC_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\DPC\data\DPC_Vest_RG_co.paa"
		};
	};
	class FOG_Vest_DPC_Black: FOG_Vest_DPC_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\DPC\data\DPC_Vest_Black_co.paa"
		};
	};
	class FOG_Vest_DPC_Grey: FOG_Vest_DPC_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\DPC\data\DPC_Vest_Grey_co.paa"
		};
	};
	class FOG_Vest_DPC_MC: FOG_Vest_DPC_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\DPC\data\DPC_Vest_MC_co.paa"
		};
	};
	class FOG_Vest_DPC_AOR2: FOG_Vest_DPC_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Vests\DPC\data\DPC_Vest_AOR2_co.paa"
		};
	};
};
