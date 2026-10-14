class CfgPatches
{
	class TGK_RussianSoldier_v2
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Magazines",
			"DZ_Characters_Vests",
			"DZ_Weapons_Firearms",
			"DZ_Data",
			"DZ_Characters",
			"DZ_Scripts"
		};
		magazines[]={};
	};
};
class cfgVehicles
{
	class Container_Base;
	class TGK_RussianSoldier_v2_MagPouchBig_base: Container_Base
	{
		scope=0;
		displayName="$STR_RusV2_MagPouchBig_01";
		descriptionShort="$STR_RusV2_MagPouchBig_02";
		model="TGK_RussianSoldier_V2\set\Items\MagPouchBig\MagPouchBig.p3d";
		itemSize[]={2,3};
		allowOwnedCargoManipulation=1;
		itemsCargoSize[]={6,4};
		inventorySlot[]=
		{
			"MagPouchBig"
		};
		hiddenSelections[]=
		{
			"camo"
		};
		attachments[]=
		{
			"magazine"
		};
		weight=400;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		randomQuantity=2;
		varQuantityInit=4;
		varQuantityMin=0;
		varQuantityMax=4;
		varQuantityDestroyOnMin=0;
		stackedUnit="ammo";
		quantityBar=1;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=50;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"TGK_RussianSoldier_V2\set\Items\Pouches\Pouches.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_RussianSoldier_V2\set\Items\Pouches\Pouches.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_RussianSoldier_V2\set\Items\Pouches\Pouches_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_RussianSoldier_V2\set\Items\Pouches\Pouches_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_RussianSoldier_V2\set\Items\Pouches\Pouches_destruct.rvmat"
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
					soundSet="pickUpCourierBag_Light_SoundSet";
					id=796;
				};
				class pickUpItem
				{
					soundSet="pickUpCourierBag_SoundSet";
					id=797;
				};
			};
		};
	};
	class TGK_RussianSoldier_v2_MagPouchBig_Alpine: TGK_RussianSoldier_v2_MagPouchBig_base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Items\Pouches\Pouches_Alpine.paa"
		};
	};
	class TGK_RussianSoldier_v2_MagPouchBig_Black: TGK_RussianSoldier_v2_MagPouchBig_base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Items\Pouches\Pouches_Black.paa"
		};
	};
	class TGK_RussianSoldier_v2_MagPouchBig_Black_MC: TGK_RussianSoldier_v2_MagPouchBig_base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Items\Pouches\Pouches_Black_MC.paa"
		};
	};
	class TGK_RussianSoldier_v2_MagPouchBig_EMR: TGK_RussianSoldier_v2_MagPouchBig_base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Items\Pouches\Pouches_EMR.paa"
		};
	};
	class TGK_RussianSoldier_v2_MagPouchBig_MC: TGK_RussianSoldier_v2_MagPouchBig_base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Items\Pouches\Pouches_MC.paa"
		};
	};
};
class CfgNonAIVehicles
{
	class ProxyAttachment;
	class ProxyMagPouchBig: ProxyAttachment
	{
		scope=2;
		inventorySlot="MagPouchBig";
		model="TGK_RussianSoldier_V2\set\Items\MagPouchBig\MagPouchBig.p3d";
	};
};
class CfgSlots
{
	class Slot_MagPouchBig
	{
		name="MagPouchBig";
		displayName="$STR_RusV2_MagPouchBig_01";
		GhostIcon="set:TGK_CustomIcons_RusV2 image:MagPouchBig";
	};
};
