class CfgPatches
{
	class TGK_RussianSoldier_v2
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
	class Container_Base;
	class TGK_RussianSoldier_v2_GroinPouch_ColorBase: Container_Base
	{
		scope=0;
		displayName="$STR_RusV2_GroinPouch01";
		descriptionShort="$STR_RusV2_GroinPouch02";
		model="TGK_RussianSoldier_V2\set\Items\GroinPouch\models\GroinPouch.p3d";
		inventorySlot[]=
		{
			"GroinPouch"
		};
		itemInfo[]=
		{
			"Clothing"
		};
		itemSize[]={3,3};
		itemsCargoSize[]={7,6};
		quickBarBonus=1;
		weight=500;
		randomQuantity=2;
		varQuantityInit=4;
		varQuantityMin=0;
		varQuantityMax=4;
		varQuantityDestroyOnMin=0;
		stackedUnit="ammo";
		quantityBar=1;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		rotationFlags=17;
		varWetMax=0.0099999998;
		heatIsolation=0.0099999998;
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
					hitpoints=150;
					transferToAttachmentsCoef=0.5;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"TGK_RussianSoldier_V2\set\Items\GroinPouch\data\GroinPouch.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_RussianSoldier_V2\set\Items\GroinPouch\data\GroinPouch.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_RussianSoldier_V2\set\Items\GroinPouch\data\GroinPouch_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_RussianSoldier_V2\set\Items\GroinPouch\data\GroinPouch_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_RussianSoldier_V2\set\Items\GroinPouch\data\GroinPouch_destruct.rvmat"
							}
						}
					};
				};
			};
			class GlobalArmor
			{
				class Projectile
				{
					class Health
					{
						damage=1;
					};
					class Blood
					{
						damage=1;
					};
					class Shock
					{
						damage=1;
					};
				};
				class Melee
				{
					class Health
					{
						damage=0.85000002;
					};
					class Blood
					{
						damage=0.85000002;
					};
					class Shock
					{
						damage=0.85000002;
					};
				};
				class Infected
				{
					class Health
					{
						damage=0.94999999;
					};
					class Blood
					{
						damage=0.94999999;
					};
					class Shock
					{
						damage=0.94999999;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage=1;
					};
					class Blood
					{
						damage=1;
					};
					class Shock
					{
						damage=1;
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
					soundSet="SmershVest_pickup_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="SmershVest_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class TGK_RussianSoldier_v2_GroinPouch_Alpine: TGK_RussianSoldier_v2_GroinPouch_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Items\GroinPouch\data\GroinPouch_Alpine.paa"
		};
	};
	class TGK_RussianSoldier_v2_GroinPouch_Black: TGK_RussianSoldier_v2_GroinPouch_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Items\GroinPouch\data\GroinPouch_Black.paa"
		};
	};
	class TGK_RussianSoldier_v2_GroinPouch_Black_MC: TGK_RussianSoldier_v2_GroinPouch_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Items\GroinPouch\data\GroinPouch_Black_MC.paa"
		};
	};
	class TGK_RussianSoldier_v2_GroinPouch_EMR: TGK_RussianSoldier_v2_GroinPouch_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Items\GroinPouch\data\GroinPouch_EMR.paa"
		};
	};
	class TGK_RussianSoldier_v2_GroinPouch_MC: TGK_RussianSoldier_v2_GroinPouch_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_RussianSoldier_V2\set\Items\GroinPouch\data\GroinPouch_MC.paa"
		};
	};
};
class CfgNonAIVehicles
{
	class ProxyAttachment;
	class ProxyGroinPouch: ProxyAttachment
	{
		scope=2;
		inventorySlot="GroinPouch";
		model="TGK_RussianSoldier_V2\set\Items\GroinPouch\models\GroinPouch.p3d";
	};
};
class CfgSlots
{
	class Slot_GroinPouch
	{
		name="GroinPouch";
		displayName="$STR_RusV2_GroinPouch01";
		GhostIcon="set:TGK_CustomIcons_RusV2 image:GroinPouch";
	};
};
