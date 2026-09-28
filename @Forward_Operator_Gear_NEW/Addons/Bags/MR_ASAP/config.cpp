class CfgPatches
{
	class FOG_Bag_MRASAP
	{
		units[]=
		{
			"FOG_Bag_MRASAP_Black",
			"FOG_Bag_MRASAP_Grey",
			"FOG_Bag_MRASAP_RG",
			"FOG_Bag_MRASAP_CB",
			"FOG_Bag_MRASAP_MC"
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
	class FOG_Bag_MRASAP_ColorBase: Clothing
	{
		scope=0;
		displayName="Ranch ASAP BackPack";
		descriptionShort="Рюкзак АСАП — это универсальная 70-литровая модель с нейтральным дизайном, выполненная из износостойких материалов с удобной системой регулируемых лямок и несколькими функциональными отделениями для организованного хранения вещей | The ASAP backpack is a versatile 70-liter model with neutral design, made of durable materials featuring adjustable strap system and multiple functional compartments for organized storage.";
		model="\FOG_MOD\Bags\MR_ASAP\MR_asap_G.p3d";
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
		itemSize[]={7,10};
		quickBarBonus=3;
		itemsCargoSize[]={7,10};
		weight=600;
		varWetMax=0.40000001;
		heatIsolation=0.5;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		soundAttType="Outdoor";
		randomQuantity=4;
		canBeDigged=0;
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
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\MR_ASAP\data\mysteryASAP_CB_co.paa"
		};
		class ClothingTypes
		{
			male="\FOG_MOD\Bags\MR_ASAP\MR_asap_M.p3d";
			female="\FOG_MOD\Bags\MR_ASAP\MR_asap_M.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=300;
					transferToAttachmentsCoef=0.5;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"FOG_MOD\Bags\MR_ASAP\data\mr_asap.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"FOG_MOD\Bags\MR_ASAP\data\mr_asap.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"FOG_MOD\Bags\MR_ASAP\data\mr_asap_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"FOG_MOD\Bags\MR_ASAP\data\mr_asap_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"FOG_MOD\Bags\MR_ASAP\data\mr_asap_destruct.rvmat"
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
	class FOG_Bag_MRASAP_Black: FOG_Bag_MRASAP_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\MR_ASAP\data\mysteryASAP_blk_co.paa"
		};
	};
	class FOG_Bag_MRASAP_Grey: FOG_Bag_MRASAP_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\MR_ASAP\data\mysteryASAP_gry_co.paa"
		};
	};
	class FOG_Bag_MRASAP_RG: FOG_Bag_MRASAP_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\MR_ASAP\data\mysteryASAP_RG_co.paa"
		};
	};
	class FOG_Bag_MRASAP_CB: FOG_Bag_MRASAP_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\MR_ASAP\data\mysteryASAP_CB_co.paa"
		};
	};
	class FOG_Bag_MRASAP_MC: FOG_Bag_MRASAP_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"FOG_MOD\Bags\MR_ASAP\data\mysteryASAP_mc_co.paa"
		};
	};
};
