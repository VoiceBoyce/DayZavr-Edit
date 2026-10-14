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
class cfgVehicles
{
	class Inventory_base;
	class TGK_FSB_ALPHA_Headphones_base: Inventory_base
	{
		scope=0;
		displayName="$STR_FSB_ALPHA_Headphones_01";
		descriptionShort="$STR_FSB_ALPHA_Headphones_02";
		model="TGK_FSB_Alpha\set\items\Headphones\Headphones.p3d";
		weight=250;
		itemSize[]={2,1};
		varWetMax=0.0099999998;
		heatIsolation=0.0099999998;
		inventorySlot[]=
		{
			"Helmet_Headphones"
		};
		hiddenSelections[]=
		{
			"camo"
		};
		rotationFlags=0;
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
	};
	class TGK_FSB_ALPHA_Headphones_Black: TGK_FSB_ALPHA_Headphones_base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\Helmet\Cover\Cover_Black.paa"
		};
	};
};
class CfgNonAIVehicles
{
	class ProxyAttachment;
	class ProxyHeadphones: ProxyAttachment
	{
		scope=2;
		inventorySlot="Helmet_Headphones";
		model="TGK_FSB_Alpha\set\items\Headphones\Headphones.p3d";
	};
};
class CfgSlots
{
	class Slot_Helmet_Headphones
	{
		name="Helmet_Headphones";
		displayName="$STR_FSB_ALPHA_Headphones_01";
		GhostIcon="set:TGK_CustomIcons image:HeadPhones";
	};
};
