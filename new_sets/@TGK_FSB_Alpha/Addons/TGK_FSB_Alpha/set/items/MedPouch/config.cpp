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
	class TGK_FSB_Alpha_MedPouch_ColorBase: Container_Base
	{
		scope=0;
		displayName="$STR_FSB_Alpha_MedPouch_01";
		descriptionShort="$STR_FSB_Alpha_MedPouch_02";
		model="TGK_FSB_Alpha\set\items\MedPouch\models\MedPouch.p3d";
		itemSize[]={2,3};
		inventorySlot[]=
		{
			"TGK_MedPouch"
		};
		hiddenSelections[]=
		{
			"camo"
		};
		itemsCargoSize[]={5,4};
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		quantityBar=1;
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
								"TGK_FSB_Alpha\set\items\MedPouch\data\MedPouch.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_FSB_Alpha\set\items\MedPouch\data\MedPouch.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_FSB_Alpha\set\items\MedPouch\data\MedPouch_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_FSB_Alpha\set\items\MedPouch\data\MedPouch_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_FSB_Alpha\set\items\MedPouch\data\MedPouch_destruct.rvmat"
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
				class BloodBag_start
				{
					soundSet="BloodBag_start_SoundSet";
					id=201;
				};
				class BloodBag_spear
				{
					soundSet="BloodBag_spear_SoundSet";
					id=202;
				};
				class BloodBag_loop
				{
					soundSet="BloodBag_loop_SoundSet";
					id=203;
				};
				class BloodBag_loop2
				{
					soundSet="BloodBag_loop_SoundSet";
					id=204;
				};
				class BloodBag_end
				{
					soundSet="BloodBag_end_SoundSet";
					id=205;
				};
				class drop
				{
					soundset="bloodbag_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class TGK_FSB_Alpha_MedPouch_Alpine: TGK_FSB_Alpha_MedPouch_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\items\MedPouch\data\MedPouch_Alpine.paa"
		};
	};
	class TGK_FSB_Alpha_MedPouch_Black: TGK_FSB_Alpha_MedPouch_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\items\MedPouch\data\MedPouch_Black.paa"
		};
	};
	class TGK_FSB_Alpha_MedPouch_Black_MC: TGK_FSB_Alpha_MedPouch_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\items\MedPouch\data\MedPouch_Black_MC.paa"
		};
	};
	class TGK_FSB_Alpha_MedPouch_MC: TGK_FSB_Alpha_MedPouch_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\items\MedPouch\data\MedPouch_MC.paa"
		};
	};
	class TGK_FSB_Alpha_MedPouch_ODGreen: TGK_FSB_Alpha_MedPouch_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\items\MedPouch\data\MedPouch_ODGreen.paa"
		};
	};
	class TGK_FSB_Alpha_MedPouch_Tropic_MC: TGK_FSB_Alpha_MedPouch_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_FSB_Alpha\set\items\MedPouch\data\MedPouch_Tropic_MC.paa"
		};
	};
};
class CfgNonAIVehicles
{
	class ProxyAttachment;
	class ProxyMedPouch: ProxyAttachment
	{
		scope=2;
		inventorySlot="TGK_MedPouch";
		model="TGK_FSB_Alpha\set\items\MedPouch\models\MedPouch.p3d";
	};
};
class CfgSlots
{
	class Slot_TGK_MedPouch
	{
		name="TGK_MedPouch";
		displayName="$STR_FSB_Alpha_MedPouch_01";
		GhostIcon="set:TGK_CustomIcons image:MedPouch";
	};
};
