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
			"DZ_Scripts",
			"DZ_Characters_Vests"
		};
	};
};
class cfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class TGK_FSB_ALPHA_BackPanel_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_TGK_FSB_ALPHA_BackPanel_01";
		descriptionShort="$STR_TGK_FSB_ALPHA_BackPanel_02";
		model="TGK_FSB_Alpha\set\items\Backpanel\models\Backpanel.p3d";
		inventorySlot[]=
		{
			"BackPanel"
		};
		itemSize[]={4,4};
		itemsCargoSize[]={5,5};
		weight=1000;
		varWetMax=0.0099999998;
		heatIsolation=0.1;
		visibilityModifier=0.80000001;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		soundAttType="Military";
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
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"TGK_FSB_Alpha\set\items\Backpanel\data\Backpanel.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_FSB_Alpha\set\items\Backpanel\data\Backpanel.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_FSB_Alpha\set\items\Backpanel\data\Backpanel_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_FSB_Alpha\set\items\Backpanel\data\Backpanel_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_FSB_Alpha\set\items\Backpanel\data\Backpanel_destruct.rvmat"
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
					soundSet="pickUpBackPack_Metal_Light_SoundSet";
					id=796;
				};
				class pickUpItem
				{
					soundSet="pickUpBackPack_Metal_SoundSet";
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
	class TGK_FSB_ALPHA_BackPanel_Alpine: TGK_FSB_ALPHA_BackPanel_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\items\Backpanel\data\Backpanel_Alpine.paa"
		};
	};
	class TGK_FSB_ALPHA_BackPanel_Black: TGK_FSB_ALPHA_BackPanel_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\items\Backpanel\data\Backpanel_Black.paa"
		};
	};
	class TGK_FSB_ALPHA_BackPanel_Black_MC: TGK_FSB_ALPHA_BackPanel_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\items\Backpanel\data\Backpanel_Black_MC.paa"
		};
	};
	class TGK_FSB_ALPHA_BackPanel_MC: TGK_FSB_ALPHA_BackPanel_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\items\Backpanel\data\Backpanel_MC.paa"
		};
	};
	class TGK_FSB_ALPHA_BackPanel_ODGreen: TGK_FSB_ALPHA_BackPanel_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\items\Backpanel\data\Backpanel_ODGreen.paa"
		};
	};
	class TGK_FSB_ALPHA_BackPanel_Tropic_MC: TGK_FSB_ALPHA_BackPanel_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\items\Backpanel\data\Backpanel_Tropic_MC.paa"
		};
	};
};
class CfgNonAIVehicles
{
	class ProxyAttachment;
	class ProxyBackpanel: ProxyAttachment
	{
		scope=2;
		inventorySlot="BackPanel";
		model="TGK_FSB_Alpha\set\items\Backpanel\models\Backpanel.p3d";
	};
};
class CfgSlots
{
	class Slot_BackPanel
	{
		name="BackPanel";
		displayName="$STR_TGK_FSB_ALPHA_BackPanel_01";
		GhostIcon="set:TGK_CustomIcons image:Backpanel";
	};
};
