class CfgPatches
{
	class FOG_Bag_MRCommsBag
	{
		units[]=
		{
			"FOG_Bag_CommsBag_CB",
			"FOG_Bag_CommsBag_RG",
			"FOG_Bag_CommsBag_Tan",
			"FOG_Bag_CommsBag_Black",
			"FOG_Bag_CommsBag_White",
			"FOG_Bag_CommsBag_MC",
			"FOG_Bag_CommsBag_MCAL",
			"FOG_Bag_CommsBag_MCB",
			"FOG_Bag_CommsBag_MCT",
			"FOG_Bag_CommsBag_M81"
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
	class FOG_Bag_CommsBag_Base: Clothing
	{
		scope=0;
		displayName="Comms Bag";
		descriptionShort="Этот вместительный рюкзак радиста с длинной антенной улавливает непонятные, но очень интересные сигналы неизвестного происхождения. Его антенна постоянно дрожит и вибрирует, будто пытается поймать что то важное, а внутри достаточно места как для обычных вещей, так и для необычных находок. | This spacious backpack with a long antenna picks up strange yet intriguing signals of unknown origin. Its antenna constantly quivers and vibrates as if trying to catch something important, while offering ample space inside for both everyday items and extraordinary discoveries.";
		model="\FOG_MOD\Bags\CommsBag\FOG_CommsBag_G.p3d";
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
		itemSize[]={10,12};
		quickBarBonus=3;
		itemsCargoSize[]={10,12};
		weight=1200;
		varWetMax=0.249;
		heatIsolation=0.5;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		soundAttType="Outdoor";
		randomQuantity=4;
		canBeDigged=0;
		attachments[]={};
		hiddenSelections[]=
		{
			"camo"
		};
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\CommsBag\data\MR_CommsBag_Tan_co.paa"
		};
		class ClothingTypes
		{
			male="\FOG_MOD\Bags\CommsBag\FOG_CommsBag_M.p3d";
			female="\FOG_MOD\Bags\CommsBag\FOG_CommsBag_M.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=750;
					transferToAttachmentsCoef=0.5;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"FOG_MOD\Bags\CommsBag\data\MR_CommsBag.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Bags\CommsBag\data\MR_CommsBag.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Bags\CommsBag\data\MR_CommsBag.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Bags\CommsBag\data\MR_CommsBag.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Bags\CommsBag\data\MR_CommsBag.rvmat"
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
	class FOG_Bag_CommsBag_CB: FOG_Bag_CommsBag_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\CommsBag\data\MR_CommsBag_CG_co.paa"
		};
	};
	class FOG_Bag_CommsBag_RG: FOG_Bag_CommsBag_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\CommsBag\data\MR_CommsBag_RG_co.paa"
		};
	};
	class FOG_Bag_CommsBag_Tan: FOG_Bag_CommsBag_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\CommsBag\data\MR_CommsBag_Tan_co.paa"
		};
	};
	class FOG_Bag_CommsBag_Black: FOG_Bag_CommsBag_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\CommsBag\data\MR_CommsBag_Black_co.paa"
		};
	};
	class FOG_Bag_CommsBag_White: FOG_Bag_CommsBag_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\CommsBag\data\MR_CommsBag_White_co.paa"
		};
	};
	class FOG_Bag_CommsBag_MC: FOG_Bag_CommsBag_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\CommsBag\data\MR_CommsBag_MC_co.paa"
		};
	};
	class FOG_Bag_CommsBag_MCAL: FOG_Bag_CommsBag_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\CommsBag\data\MR_CommsBag_MCAL_co.paa"
		};
	};
	class FOG_Bag_CommsBag_MCB: FOG_Bag_CommsBag_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\CommsBag\data\MR_CommsBag_MCB_co.paa"
		};
	};
	class FOG_Bag_CommsBag_MCT: FOG_Bag_CommsBag_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\CommsBag\data\MR_CommsBag_MCT_co.paa"
		};
	};
	class FOG_Bag_CommsBag_M81: FOG_Bag_CommsBag_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\CommsBag\data\MR_CommsBag_M81_co.paa"
		};
	};
};
