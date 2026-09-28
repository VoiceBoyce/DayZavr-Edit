class CfgPatches
{
	class FOG_Characters_Backpacks_Bergen
	{
		units[]=
		{
			"FOG_Bergen_Rucksack_Black",
			"FOG_Bergen_Rucksack_Grey",
			"FOG_Bergen_Rucksack_White",
			"FOG_Bergen_Rucksack_Green",
			"FOG_Bergen_Rucksack_CB",
			"FOG_Bergen_Rucksack_M81",
			"FOG_Bergen_Rucksack_Multicam",
			"FOG_Bergen_Rucksack_MCB",
			"FOG_Bergen_Rucksack_MCAL",
			"FOG_Bergen_Rucksack_MCT",
			"FOG_Bergen_Rucksack_MARPAT",
			"FOG_Bergen_Rucksack_TGR_MCB",
			"FOG_Bergen_Rucksack_AOR1",
			"FOG_Bergen_Rucksack_AOR2"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters",
			"DZ_Characters_Backpacks"
		};
	};
};
class cfgVehicles
{
	class Clothing;
	class FOG_Bergen_Rucksack_ColorBase: Clothing
	{
		displayName="Bergen Rucksack";
		descriptionShort="Bergen Rucksack — сверхвместительный 140-слотовый экспедиционный рюкзак из баллистического кордуры с алюминиевым каркасом. Усиленная система лямок и поясной ремень с гелевыми вставками распределяют вес многосуточного груза для маршей на дальние дистанции | The Bergen Rucksack is an ultra-capacity 140-slot expedition pack crafted from ballistic cordura with aluminum frame. Heavy-duty harness and gel-padded hip belt redistribute multi-day load weight for long-range movement.";
		model="FOG_MOD\Bags\Bergen\Bergen_G.p3d";
		inventorySlot[]=
		{
			"Back"
		};
		itemInfo[]=
		{
			"Clothing",
			"Back"
		};
		rotationFlags=16;
		weight=1400;
		itemSize[]={4,5};
		itemsCargoSize[]={10,14};
		varWetMax=0.40000001;
		heatIsolation=0.69999999;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		soundAttType="Outdoor";
		visibilityModifier=0.94999999;
		attachments[]=
		{
			"Chemlight",
			"WalkieTalkie",
			"Backpack_1"
		};
		hiddenSelections[]=
		{
			"camo"
		};
		hiddenSelectionsMaterials[]=
		{
			"FOG_MOD\Bags\Bergen\data\Bergen.rvmat"
		};
		class ClothingTypes
		{
			male="FOG_MOD\Bags\Bergen\Bergen_M.p3d";
			female="FOG_MOD\Bags\Bergen\Bergen_F.p3d";
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
								"FOG_MOD\Bags\Bergen\data\Bergen.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Bags\Bergen\data\Bergen.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Bags\Bergen\data\Bergen_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Bags\Bergen\data\Bergen_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Bags\Bergen\data\Bergen_destruct.rvmat"
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
						damage=1.0;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=1.0;
					};
				};
				class Melee
				{
					class Health
					{
						damage=1.0;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=1.0;
					};
				};
				class Infected
				{
					class Health
					{
						damage=1.0;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=1.0;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage=1.0;
					};
					class Blood
					{
						damage=0;
					};
					class Shock
					{
						damage=1.0;
					};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem_Light
				{
					soundSet="pickUpBackPack_Plastic_Light_SoundSet";
					id=796;
				};
				class pickUpItem
				{
					soundSet="pickUpBackPack_Plastic_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="taloonbag_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class FOG_Bergen_Rucksack_Black: FOG_Bergen_Rucksack_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\Bergen\data\Bergen_Black_co.paa"
		};
	};
	class FOG_Bergen_Rucksack_Grey: FOG_Bergen_Rucksack_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\Bergen\data\Bergen_Grey_co.paa"
		};
	};
	class FOG_Bergen_Rucksack_White: FOG_Bergen_Rucksack_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\Bergen\data\Bergen_White_co.paa"
		};
	};
	class FOG_Bergen_Rucksack_Green: FOG_Bergen_Rucksack_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\Bergen\data\Bergen_Green_co.paa"
		};
	};
	class FOG_Bergen_Rucksack_CB: FOG_Bergen_Rucksack_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\Bergen\data\Bergen_CB_co.paa"
		};
	};
	class FOG_Bergen_Rucksack_M81: FOG_Bergen_Rucksack_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\Bergen\data\Bergen_M81_co.paa"
		};
	};
	class FOG_Bergen_Rucksack_Multicam: FOG_Bergen_Rucksack_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\Bergen\data\Bergen_Multicam_co.paa"
		};
	};
	class FOG_Bergen_Rucksack_MCB: FOG_Bergen_Rucksack_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\Bergen\data\Bergen_MCB_co.paa"
		};
	};
	class FOG_Bergen_Rucksack_MCAL: FOG_Bergen_Rucksack_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\Bergen\data\Bergen_MCAL_co.paa"
		};
	};
	class FOG_Bergen_Rucksack_MCT: FOG_Bergen_Rucksack_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\Bergen\data\Bergen_MCT_co.paa"
		};
	};
	class FOG_Bergen_Rucksack_MARPAT: FOG_Bergen_Rucksack_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\Bergen\data\Bergen_MARPAT_co.paa"
		};
	};
	class FOG_Bergen_Rucksack_TGR_MCB: FOG_Bergen_Rucksack_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\Bergen\data\Bergen_TGR_MCB_co.paa"
		};
	};
	class FOG_Bergen_Rucksack_AOR1: FOG_Bergen_Rucksack_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\Bergen\data\Bergen_AOR1_co.paa"
		};
	};
	class FOG_Bergen_Rucksack_AOR2: FOG_Bergen_Rucksack_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\Bergen\data\Bergen_AOR2_co.paa"
		};
	};
	class FOG_Bergen_Rucksack_Woodland: FOG_Bergen_Rucksack_M81
	{
	};
	class FOG_Bergen_Rucksack_ERDL: FOG_Bergen_Rucksack_M81
	{
	};
};
