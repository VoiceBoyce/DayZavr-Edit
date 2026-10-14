class CfgPatches
{
	class TGK_FSB_ALPHA
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
	class TGK_FSB_ALPHA_MagPouch_ColorBase: Container_Base
	{
		scope=0;
		displayName="$STR_TGK_FSB_MagPouch01";
		descriptionShort="$STR_TGK_FSB_MagPouch02";
		model="TGK_FSB_Alpha\set\items\SixPouch\models\SixPouch.p3d";
		inventorySlot[]=
		{
			"TGK_Magpouch_a",
			"TGK_Magpouch_b"
		};
		attachments[]=
		{
			"magazine"
		};
		weight=200;
		itemSize[]={1,3};
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		hiddenSelections[]=
		{
			"camo"
		};
		randomQuantity=2;
		rotationFlags=17;
		varWetMax=0.0099999998;
		heatIsolation=0.0099999998;
		lootTag[]=
		{
			"Military_east",
			"Military_west"
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
								"TGK_FSB_Alpha\set\items\SixPouch\data\SixPouch.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_FSB_Alpha\set\items\SixPouch\data\SixPouch.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_FSB_Alpha\set\items\SixPouch\data\SixPouch_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_FSB_Alpha\set\items\SixPouch\data\SixPouch_damage.rvmat"
							}
						},
						
						{
							0.0099999998,
							
							{
								"TGK_FSB_Alpha\set\items\SixPouch\data\SixPouch_destruct.rvmat"
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
	class TGK_FSB_ALPHA_MagPouch_Alpine: TGK_FSB_ALPHA_MagPouch_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\items\SixPouch\data\SixPouch_Alpine.paa"
		};
	};
	class TGK_FSB_ALPHA_MagPouch_Black: TGK_FSB_ALPHA_MagPouch_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\items\SixPouch\data\SixPouch_Black.paa"
		};
	};
	class TGK_FSB_ALPHA_MagPouch_Black_MC: TGK_FSB_ALPHA_MagPouch_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\items\SixPouch\data\SixPouch_Black_MC.paa"
		};
	};
	class TGK_FSB_ALPHA_MagPouch_MC: TGK_FSB_ALPHA_MagPouch_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\items\SixPouch\data\SixPouch_MC.paa"
		};
	};
	class TGK_FSB_ALPHA_MagPouch_ODGreen: TGK_FSB_ALPHA_MagPouch_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\items\SixPouch\data\SixPouch_ODGreen.paa"
		};
	};
	class TGK_FSB_ALPHA_MagPouch_Tropic_MC: TGK_FSB_ALPHA_MagPouch_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\items\SixPouch\data\SixPouch_Tropic_MC.paa"
		};
	};
};
class CfgNonAIVehicles
{
	class ProxyAttachment;
	class Proxymagazine: ProxyAttachment
	{
		scope=2;
		inventorySlot="magazine";
		model="DZ\weapons\proxies\magazine.p3d";
	};
	class ProxyMagPouch1: ProxyAttachment
	{
		scope=2;
		inventorySlot="TGK_Magpouch_a";
		model="TGK_FSB_Alpha\set\proxy\MagPouch1.p3d";
	};
	class ProxyMagPouch2: ProxyAttachment
	{
		scope=2;
		inventorySlot="TGK_Magpouch_b";
		model="TGK_FSB_Alpha\set\proxy\MagPouch2.p3d";
	};
};
class CfgSlots
{
	class Slot_TGK_Magpouch_a
	{
		name="TGK_Magpouch_a";
		displayName="$STR_TGK_FSB_MagPouch01";
		GhostIcon="set:TGK_CustomIcons image:SixPouch";
	};
	class Slot_TGK_Magpouch_b
	{
		name="TGK_Magpouch_b";
		displayName="$STR_TGK_FSB_MagPouch01";
		GhostIcon="set:TGK_CustomIcons image:SixPouch";
	};
};
