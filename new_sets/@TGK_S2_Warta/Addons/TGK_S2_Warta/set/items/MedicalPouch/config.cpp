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
	class TGK_S2_WartaMedicalPouch_Colorbase: Container_Base
	{
		scope=0;
		displayName="$STR_WartaMedicalPouch_01";
		descriptionShort="$STR_WartaMedicalPouch_02";
		model="TGK_S2_Warta\set\items\MedicalPouch\models\MedicalPouch.p3d";
		inventorySlot[]=
		{
			"WartaMedicalPouch1"
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
								"TGK_S2_Warta\set\items\MedicalPouch\data\MedicalPouch.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_S2_Warta\set\items\MedicalPouch\data\MedicalPouch.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_S2_Warta\set\items\MedicalPouch\data\MedicalPouch_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_S2_Warta\set\items\MedicalPouch\data\MedicalPouch_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_S2_Warta\set\items\MedicalPouch\data\MedicalPouch_destruct.rvmat"
							}
						}
					};
				};
			};
		};
	};
	class TGK_S2_WartaMedicalPouch: TGK_S2_WartaMedicalPouch_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_S2_Warta\set\items\MedicalPouch\data\MedicalPouch_co.paa"
		};
	};
};
class CfgNonAIVehicles
{
	class ProxyAttachment;
	class ProxyMedicalPouch: ProxyAttachment
	{
		scope=2;
		inventorySlot="WartaMedicalPouch1";
		model="TGK_S2_Warta\set\items\MedicalPouch\models\MedicalPouch.p3d";
	};
};
class CfgSlots
{
	class Slot_WartaMedicalPouch1
	{
		name="WartaMedicalPouch1";
		displayName="$STR_WartaMedicalPouch_01";
		GhostIcon="set:Icons image:MedPouche";
	};
};
