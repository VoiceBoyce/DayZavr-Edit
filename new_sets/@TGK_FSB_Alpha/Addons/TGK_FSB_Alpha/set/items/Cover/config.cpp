class CfgPatches
{
	class TGK_FSB_ALPHA
	{
		units[]={};
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
class CfgVehicles
{
	class Inventory_Base;
	class TGK_FSB_ALPHA_Helmet_Cover_Base: Inventory_Base
	{
		scope=0;
		displayName="$STR_TGK_FSB_ALPHA_Helmet_Cover_01";
		descriptionShort="$STR_TGK_FSB_ALPHA_Helmet_Cover_02";
		model="TGK_FSB_Alpha\set\items\Cover\Cover.p3d";
		rotationFlags=17;
		weight=100;
		itemSize[]={3,2};
		canBeDigged=1;
		isMeleeWeapon=1;
		allowOwnedCargoManipulation=1;
		randomQuantity=2;
		repairableWithKits[]={8};
		repairCosts[]={25};
		varWetMax=0.0099999998;
		heatIsolation=0.0099999998;
		hiddenSelections[]=
		{
			"camo"
		};
		inventorySlot[]=
		{
			"Helmet_Cover"
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=200;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"TGK_FSB_Alpha\set\Helmet\Cover\Cover.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_FSB_Alpha\set\Helmet\Cover\Cover.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_FSB_Alpha\set\Helmet\Cover\Cover_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_FSB_Alpha\set\Helmet\Cover\Cover_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_FSB_Alpha\set\Helmet\Cover\Cover_destruct.rvmat"
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
				class pickUpItem
				{
					soundSet="smallprotectorcase_pickup_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="smallprotectorcase_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class TGK_FSB_ALPHA_Helmet_Cover_Alpine: TGK_FSB_ALPHA_Helmet_Cover_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Helmet\Cover\Cover_Alpine.paa"
		};
	};
	class TGK_FSB_ALPHA_Helmet_Cover_Black: TGK_FSB_ALPHA_Helmet_Cover_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Helmet\Cover\Cover_Black.paa"
		};
	};
	class TGK_FSB_ALPHA_Helmet_Cover_Black_MC: TGK_FSB_ALPHA_Helmet_Cover_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Helmet\Cover\Cover_Black_MC.paa"
		};
	};
	class TGK_FSB_ALPHA_Helmet_Cover_MC: TGK_FSB_ALPHA_Helmet_Cover_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Helmet\Cover\Cover_MC.paa"
		};
	};
	class TGK_FSB_ALPHA_Helmet_Cover_ODGreen: TGK_FSB_ALPHA_Helmet_Cover_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Helmet\Cover\Cover_ODGreen.paa"
		};
	};
	class TGK_FSB_ALPHA_Helmet_Cover_Tropic_MC: TGK_FSB_ALPHA_Helmet_Cover_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Helmet\Cover\Cover_Tropic_MC.paa"
		};
	};
};
class CfgNonAIVehicles
{
	class ProxyAttachment;
	class ProxyCover: ProxyAttachment
	{
		scope=2;
		inventorySlot="Helmet_Cover";
		model="TGK_FSB_Alpha\set\items\Cover\Cover.p3d";
	};
};
class CfgSlots
{
	class Slot_Helmet_Cover
	{
		name="Helmet_Cover";
		displayName="$STR_TGK_FSB_ALPHA_Helmet_Cover_01";
		GhostIcon="set:TGK_CustomIcons image:Cover";
	};
};
