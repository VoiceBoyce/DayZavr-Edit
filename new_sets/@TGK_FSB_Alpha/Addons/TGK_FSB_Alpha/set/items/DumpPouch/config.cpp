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
	class Container_Base;
	class TGK_FSB_ALPHA_DumpPouch_ColorBase: Container_Base
	{
		scope=0;
		displayName="$STR_FSB_ALPHA_DumpPouch_01";
		descriptionShort="$STR_FSB_ALPHA_DumpPouch_02";
		model="TGK_FSB_Alpha\set\items\DumpPouch\models\DumpPouch.p3d";
		inventorySlot[]=
		{
			"DumpPouch"
		};
		weight=750;
		itemSize[]={3,3};
		itemsCargoSize[]={5,6};
		quickBarBonus=1;
		varWetMax=0.0099999998;
		heatIsolation=0.0099999998;
		repairableWithKits[]={3,8};
		repairCosts[]={75,50};
		visibilityModifier=0.94999999;
		rotationFlags=17;
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
					hitpoints=200;
					transferToAttachmentsCoef=0.5;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"TGK_FSB_Alpha\set\items\DumpPouch\data\DumpPouch.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_FSB_Alpha\set\items\DumpPouch\data\DumpPouch.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_FSB_Alpha\set\items\DumpPouch\data\DumpPouch_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_FSB_Alpha\set\items\DumpPouch\data\DumpPouch_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_FSB_Alpha\set\items\DumpPouch\data\DumpPouch_destruct.rvmat"
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
	class TGK_FSB_ALPHA_DumpPouch_Alpine: TGK_FSB_ALPHA_DumpPouch_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\items\DumpPouch\data\DumpPouch_Alpine.paa"
		};
	};
	class TGK_FSB_ALPHA_DumpPouch_Black: TGK_FSB_ALPHA_DumpPouch_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\items\DumpPouch\data\DumpPouch_Black.paa"
		};
	};
	class TGK_FSB_ALPHA_DumpPouch_Black_MC: TGK_FSB_ALPHA_DumpPouch_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\items\DumpPouch\data\DumpPouch_Black_MC.paa"
		};
	};
	class TGK_FSB_ALPHA_DumpPouch_MC: TGK_FSB_ALPHA_DumpPouch_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\items\DumpPouch\data\DumpPouch_MC.paa"
		};
	};
	class TGK_FSB_ALPHA_DumpPouch_ODGreen: TGK_FSB_ALPHA_DumpPouch_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\items\DumpPouch\data\DumpPouch_ODGreen.paa"
		};
	};
	class TGK_FSB_ALPHA_DumpPouch_Tropic_MC: TGK_FSB_ALPHA_DumpPouch_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\items\DumpPouch\data\DumpPouch_Tropic_MC.paa"
		};
	};
};
class CfgNonAIVehicles
{
	class ProxyAttachment;
	class ProxyDumpPouch: ProxyAttachment
	{
		scope=2;
		inventorySlot="DumpPouch";
		model="TGK_FSB_Alpha\set\items\DumpPouch\models\DumpPouch.p3d";
	};
};
class CfgSlots
{
	class Slot_DumpPouch
	{
		name="DumpPouch";
		displayName="$STR_FSB_ALPHA_DumpPouch_01";
		GhostIcon="set:TGK_CustomIcons image:DumpPouch";
	};
};
