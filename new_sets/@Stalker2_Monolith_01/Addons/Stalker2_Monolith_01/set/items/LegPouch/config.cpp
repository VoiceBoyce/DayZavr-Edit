class CfgPatches
{
	class Stalker_2_Monolith_LegPouch
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
	class Stalker_2_Monolith_LegPouch_ColorBase: Container_Base
	{
		scope=0;
		displayName="$STR_Stalker_2_Monolith_LegPouch_01";
		descriptionShort="$STR_Stalker_2_Monolith_LegPouch_02";
		model="Stalker2_Monolith_01\set\items\LegPouch\models\LegPouch.p3d";
		inventorySlot[]=
		{
			"LegPouch"
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
								"Stalker2_Monolith_01\set\items\LegPouch\data\LegPouch.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"Stalker2_Monolith_01\set\items\LegPouch\data\LegPouch.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"Stalker2_Monolith_01\set\items\LegPouch\data\LegPouch_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"Stalker2_Monolith_01\set\items\LegPouch\data\LegPouch_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"Stalker2_Monolith_01\set\items\LegPouch\data\LegPouch_destruct.rvmat"
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
	class S2_Monolith_01_LegPouch: Stalker_2_Monolith_LegPouch_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\items\LegPouch\data\LegPouch_co.paa"
		};
	};
	class S2_Monolith_01_LegPouch_ClearSky: Stalker_2_Monolith_LegPouch_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\items\LegPouch\data\LegPouch_ClearSky.paa"
		};
	};
	class S2_Monolith_01_LegPouch_Duty: Stalker_2_Monolith_LegPouch_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\items\LegPouch\data\LegPouch_Duty.paa"
		};
	};
	class S2_Monolith_01_LegPouch_Freedom: Stalker_2_Monolith_LegPouch_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\items\LegPouch\data\LegPouch_Freedom.paa"
		};
	};
	class S2_Monolith_01_LegPouch_Korpus: Stalker_2_Monolith_LegPouch_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\items\LegPouch\data\LegPouch_Korpus.paa"
		};
	};
	class S2_Monolith_01_LegPouch_Merc: Stalker_2_Monolith_LegPouch_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\items\LegPouch\data\LegPouch_Merc.paa"
		};
	};
	class S2_Monolith_01_LegPouch_MSOP: Stalker_2_Monolith_LegPouch_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\items\LegPouch\data\LegPouch_MSOP.paa"
		};
	};
	class S2_Monolith_01_LegPouch_Neutrals: Stalker_2_Monolith_LegPouch_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\items\LegPouch\data\LegPouch_Neutrals.paa"
		};
	};
	class S2_Monolith_01_LegPouch_Warta: Stalker_2_Monolith_LegPouch_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_01\set\items\LegPouch\data\LegPouch_Warta.paa"
		};
	};
};
class CfgNonAIVehicles
{
	class ProxyAttachment;
	class ProxyLegPouch: ProxyAttachment
	{
		scope=2;
		inventorySlot="LegPouch";
		model="Stalker2_Monolith_01\set\items\LegPouch\models\LegPouch.p3d";
	};
};
class CfgSlots
{
	class Slot_LegPouch
	{
		name="LegPouch";
		displayName="$STR_Stalker_2_Monolith_LegPouch_01";
		GhostIcon="set:TGK_CustomIconsS2MONOLITH image:LegPouch";
	};
};
