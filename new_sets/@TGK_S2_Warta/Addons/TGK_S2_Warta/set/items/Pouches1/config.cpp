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
	class TGK_S2_WartaPouches1_Colorbase: Container_Base
	{
		scope=0;
		displayName="$STR_WartaPouches_01";
		descriptionShort="$STR_WartaPouches_02";
		model="TGK_S2_Warta\set\items\Pouches1\models\Pouches1.p3d";
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
								"TGK_S2_Warta\set\Vest_korshunov\data\Vest.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_S2_Warta\set\Vest_korshunov\data\Vest.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_S2_Warta\set\Vest_korshunov\data\Vest_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_S2_Warta\set\Vest_korshunov\data\Vest_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_S2_Warta\set\Vest_korshunov\data\Vest_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class TGK_S2_WartaPouches_v2: TGK_S2_WartaPouches1_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_S2_Warta\set\Vest_korshunov\data\Vest_co.paa"
		};
	};
};
class CfgNonAIVehicles
{
	class ProxyAttachment;
	class ProxyPouches1: ProxyAttachment
	{
		scope=2;
		inventorySlot="WartaPouches1";
		model="TGK_S2_Warta\set\items\Pouches1\models\Pouches1.p3d";
	};
	class ProxyPouches1_dop: ProxyAttachment
	{
		scope=2;
		inventorySlot="WartaPouches2_Korshunov";
		model="TGK_S2_Warta\set\items\Pouches1\models\Pouches1_dop.p3d";
	};
};
