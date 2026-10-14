class CfgPatches
{
	class TGK_FSB_Alpha
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
		magazines[]={};
	};
};
class cfgVehicles
{
	class Container_Base;
	class TGK_FSB_Alpha_IFAK_ColorBase: Container_Base
	{
		scope=0;
		displayName="$STR_FSB_Alpha_IFAK_01";
		descriptionShort="$STR_FSB_Alpha_IFAK_02";
		model="TGK_FSB_Alpha\set\items\IFAK\models\IFAK.p3d";
		itemSize[]={2,3};
		itemsCargoSize[]={5,4};
		inventorySlot[]=
		{
			"TGK_IFAK"
		};
		hiddenSelections[]=
		{
			"camo"
		};
		weight=400;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		rotationFlags=17;
		varWetMax=0.0099999998;
		heatIsolation=0.0099999998;
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
								"TGK_FSB_Alpha\set\items\IFAK\data\IFAK.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_FSB_Alpha\set\items\IFAK\data\IFAK.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_FSB_Alpha\set\items\IFAK\data\IFAK_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_FSB_Alpha\set\items\IFAK\data\IFAK_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_FSB_Alpha\set\items\IFAK\data\IFAK_destruct.rvmat"
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
	class TGK_FSB_Alpha_IFAK_Alpine: TGK_FSB_Alpha_IFAK_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\items\IFAK\data\IFAK_Alpine.paa"
		};
	};
	class TGK_FSB_Alpha_IFAK_Black: TGK_FSB_Alpha_IFAK_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\items\IFAK\data\IFAK_Black.paa"
		};
	};
	class TGK_FSB_Alpha_IFAK_Black_MC: TGK_FSB_Alpha_IFAK_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\items\IFAK\data\IFAK_Black_MC.paa"
		};
	};
	class TGK_FSB_Alpha_IFAK_MC: TGK_FSB_Alpha_IFAK_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\items\IFAK\data\IFAK_MC.paa"
		};
	};
	class TGK_FSB_Alpha_IFAK_ODGreen: TGK_FSB_Alpha_IFAK_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\items\IFAK\data\IFAK_ODGreen.paa"
		};
	};
	class TGK_FSB_Alpha_IFAK_Tropic_MC: TGK_FSB_Alpha_IFAK_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\items\IFAK\data\IFAK_Tropic_MC.paa"
		};
	};
};
class CfgNonAIVehicles
{
	class ProxyAttachment;
	class ProxyIFAK: ProxyAttachment
	{
		scope=2;
		inventorySlot="TGK_IFAK";
		model="TGK_FSB_Alpha\set\items\IFAK\models\IFAK.p3d";
	};
};
class CfgSlots
{
	class Slot_TGK_IFAK
	{
		name="TGK_IFAK";
		displayName="$STR_FSB_Alpha_IFAK_01";
		GhostIcon="set:TGK_CustomIcons image:IFAK";
	};
};
