class CfgPatches
{
	class TGK_S2_Warta
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Weapons_Magazines",
			"DZ_Weapons_Firearms",
			"DZ_Data",
			"DZ_Characters",
			"DZ_Scripts"
		};
	};
};
class CfgVehicles
{
	class Container_Base;
	class TGK_S2_WartaPouches_Colorbase: Container_Base
	{
		scope=0;
		displayName="$STR_WartaPouches_01";
		descriptionShort="$STR_WartaPouches_02";
		model="TGK_S2_Warta\set\items\Pouches\models\Pouches.p3d";
		inventorySlot[]=
		{
			"WartaPouches1",
			"WartaPouches2_Korshunov"
		};
		weight=400;
		allowOwnedCargoManipulation=1;
		randomQuantity=2;
		itemSize[]={4,3};
		itemsCargoSize[]={5,6};
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		hiddenSelections[]=
		{
			"camo"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=100;
					transferToAttachmentsCoef=0.5;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"TGK_S2_Warta\set\items\Pouches\data\Pouches.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_S2_Warta\set\items\Pouches\data\Pouches.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_S2_Warta\set\items\Pouches\data\Pouches_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_S2_Warta\set\items\Pouches\data\Pouches_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_S2_Warta\set\items\Pouches\data\Pouches_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class TGK_S2_WartaPouches_v1: TGK_S2_WartaPouches_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_S2_Warta\set\items\Pouches\data\Pouches_co.paa"
		};
	};
};
class CfgNonAIVehicles
{
	class ProxyAttachment;
	class ProxyPouches: ProxyAttachment
	{
		scope=2;
		inventorySlot="WartaPouches1";
		model="TGK_S2_Warta\set\items\Pouches\models\Pouches.p3d";
	};
	class ProxyPouches_dop: ProxyAttachment
	{
		scope=2;
		inventorySlot="WartaPouches2_Korshunov";
		model="TGK_S2_Warta\set\items\Pouches\models\Pouches_dop.p3d";
	};
};
class CfgSlots
{
	class Slot_WartaPouches1
	{
		name="WartaPouches1";
		displayName="$STR_WartaPouches_01";
		GhostIcon="set:Icons image:Pouches";
	};
	class Slot_WartaPouches2_Korshunov
	{
		name="WartaPouches2_Korshunov";
		displayName="$STR_WartaPouches_01";
		GhostIcon="set:Icons image:Pouches";
	};
};
