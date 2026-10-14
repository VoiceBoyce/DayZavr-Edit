class CfgPatches
{
	class Stalker_2_Monolith_02_Pouches_BAZA
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
	class Stalker_2_Monolith_02_Pouches_ColorBase: Container_Base
	{
		scope=0;
		displayName="$STR_Stalker_2_Monolith_02_Pouches_01";
		descriptionShort="$STR_Stalker_2_Monolith_02_Pouches_02";
		model="Stalker2_Monolith_02\set\Pouches\Pouches.p3d";
		itemSize[]={4,2};
		inventorySlot[]=
		{
			"Stalker_2_Monolith_02_Pouches1"
		};
		hiddenSelections[]=
		{
			"camo"
		};
		itemsCargoSize[]={4,10};
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
								"Stalker2_Monolith_02\set\jacket\data\Top.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"Stalker2_Monolith_02\set\jacket\data\Top.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"Stalker2_Monolith_02\set\jacket\data\Top_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"Stalker2_Monolith_02\set\jacket\data\Top_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"Stalker2_Monolith_02\set\jacket\data\Top_destruct.rvmat"
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
				class drop
				{
					soundset="bloodbag_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class Stalker_2_Monolith_02_Pouches: Stalker_2_Monolith_02_Pouches_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_02\set\jacket\data\Top_co.paa"
		};
	};
};
class CfgNonAIVehicles
{
	class ProxyAttachment;
	class ProxyPouches: ProxyAttachment
	{
		scope=2;
		inventorySlot="Stalker_2_Monolith_02_Pouches1";
		model="Stalker2_Monolith_02\set\Pouches\Pouches.p3d";
	};
};
class CfgSlots
{
	class Slot_Stalker_2_Monolith_02_Pouches1
	{
		name="Stalker_2_Monolith_02_Pouches1";
		displayName="$STR_Stalker_2_Monolith_02_Pouches_01";
		GhostIcon="set:Stalker_Monolith_02 image:Pouches";
	};
};
