class CfgPatches
{
	class FOG_Bag_SATL
	{
		units[]=
		{
			"FOG_Bag_SATL_MC",
			"FOG_Bag_SATL_RG",
			"FOG_Bag_SATL_CB",
			"FOG_Bag_SATL_KHK",
			"FOG_Bag_SATL_Black",
			"FOG_Bag_SATL_Grey"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters",
			"DZ_Scripts"
		};
	};
};
class cfgVehicles
{
	class Clothing;
	class FOG_Bag_SATL_ColorBase: Clothing
	{
		scope=0;
		displayName="SATL BackPack";
		descriptionShort="Рюкзак SATL это вместительная 140 литровая тактическая модель с креплениями для пистолета, топора и ножен, а также специальными отделениями для фляги, компаса и рации, обеспечивающая удобный доступ ко всему снаряжению в походных условиях | The SATL backpack is a spacious 140 liter tactical model with mounts for pistol, axe and scabbard, along with dedicated compartments for flask, compass and radio, offering convenient access to all gear during expeditions.";
		model="\FOG_MOD\Bags\MR_SATL\FOG_MR_SATL_G.p3d";
		attachments[]=
		{
			"Pistol",
			"FOG_big_patch",
			"FOG_ifak_vest",
			"Belt_Back",
			"Belt_Left",
			"Chemlight",
			"FOG_navagation_slot",
			"WalkieTalkie",
			"Backpack_1"
		};
		inventorySlot[]=
		{
			"Back"
		};
		itemInfo[]=
		{
			"Clothing",
			"Back"
		};
		simulation="clothing";
		vehicleClass="Clothing";
		rotationFlags=16;
		itemSize[]={10,14};
		quickBarBonus=4;
		itemsCargoSize[]={10,14};
		weight=1600;
		varWetMax=0.249;
		heatIsolation=0.75;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		soundAttType="Outdoor";
		randomQuantity=4;
		canBeDigged=0;
		hiddenSelections[]=
		{
			"camo"
		};
		class ClothingTypes
		{
			male="\FOG_MOD\Bags\MR_SATL\FOG_MR_SATL_M.p3d";
			female="\FOG_MOD\Bags\MR_SATL\FOG_MR_SATL_F.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=1800;
					transferToAttachmentsCoef=0.5;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"FOG_MOD\Bags\MR_SATL\data\SATL.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Bags\MR_SATL\data\SATL.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Bags\MR_SATL\data\SATL.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Bags\MR_SATL\data\SATL.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Bags\MR_SATL\data\SATL.rvmat"
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
	class FOG_Bag_SATL_MC: FOG_Bag_SATL_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\MR_SATL\data\SATL_MC_co.paa"
		};
	};
	class FOG_Bag_SATL_RG: FOG_Bag_SATL_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\MR_SATL\data\SATL_RG_co.paa"
		};
	};
	class FOG_Bag_SATL_CB: FOG_Bag_SATL_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\MR_SATL\data\SATL_CB_co.paa"
		};
	};
	class FOG_Bag_SATL_KHK: FOG_Bag_SATL_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\MR_SATL\data\SATL_KHK_co.paa"
		};
	};
	class FOG_Bag_SATL_Black: FOG_Bag_SATL_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\MR_SATL\data\SATL_Black_co.paa"
		};
	};
	class FOG_Bag_SATL_Grey: FOG_Bag_SATL_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\MR_SATL\data\SATL_GRY_co.paa"
		};
	};
};
