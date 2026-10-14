class CfgPatches
{
	class Stalker_2_Monolith_Backpack
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters",
			"DZ_Scripts",
			"DZ_Characters_Backpacks"
		};
	};
};
class cfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class TaloonBag_ColorBase;
	class Stalker_2_Monolith_Backpack_ColorBase: TaloonBag_ColorBase
	{
		displayName="$STR_Stalker_2_Monolith_Backpack_01";
		descriptionShort="$STR_Stalker_2_Monolith_Backpack_02";
		model="Stalker2_Monolith_01\set\backpack\models\backpack_g.p3d";
		debug_ItemCategory=9;
		inventorySlot[]=
		{
			"Back"
		};
		itemInfo[]=
		{
			"Clothing",
			"Back"
		};
		attachments[]={};
		rotationFlags=16;
		weight=1320;
		itemSize[]={4,5};
		itemsCargoSize[]={7,6};
		varWetMax=0.79000002;
		heatIsolation=0.5;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		soundAttType="Outdoor";
		randomQuantity=3;
		hiddenSelections[]=
		{
			"camo"
		};
		class ClothingTypes
		{
			male="Stalker2_Monolith_01\set\backpack\models\backpack_m.p3d";
			female="Stalker2_Monolith_01\set\backpack\models\backpack_m.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=500;
					transferToAttachmentsCoef=0.5;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"Stalker2_Monolith_01\set\backpack\data\backpack.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"Stalker2_Monolith_01\set\backpack\data\backpack.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"Stalker2_Monolith_01\set\backpack\data\backpack_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"Stalker2_Monolith_01\set\backpack\data\backpack_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"Stalker2_Monolith_01\set\backpack\data\backpack_destruct.rvmat"
							}
						}
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
					soundSet="pickUpBag_Plastic_Light_SoundSet";
					id=796;
				};
				class pickUpItem
				{
					soundSet="pickUpBag_Plastic_SoundSet";
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
	class S2_Monolith_01_Backpack: Stalker_2_Monolith_Backpack_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\backpack\data\backpack_co.paa"
		};
	};
	class S2_Monolith_01_Backpack_ClearSky: Stalker_2_Monolith_Backpack_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\backpack\data\backpack_ClearSky.paa"
		};
	};
	class S2_Monolith_01_Backpack_Duty: Stalker_2_Monolith_Backpack_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\backpack\data\backpack_Duty.paa"
		};
	};
	class S2_Monolith_01_Backpack_Freedom: Stalker_2_Monolith_Backpack_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\backpack\data\backpack_Freedom.paa"
		};
	};
	class S2_Monolith_01_Backpack_Korpus: Stalker_2_Monolith_Backpack_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\backpack\data\backpack_Korpus.paa"
		};
	};
	class S2_Monolith_01_Backpack_Merc: Stalker_2_Monolith_Backpack_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\backpack\data\backpack_Merc.paa"
		};
	};
	class S2_Monolith_01_Backpack_MSOP: Stalker_2_Monolith_Backpack_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\backpack\data\backpack_MSOP.paa"
		};
	};
	class S2_Monolith_01_Backpack_Neutrals: Stalker_2_Monolith_Backpack_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\backpack\data\backpack_Neutrals.paa"
		};
	};
	class S2_Monolith_01_Backpack_Warta: Stalker_2_Monolith_Backpack_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\backpack\data\backpack_Warta.paa"
		};
	};
};
