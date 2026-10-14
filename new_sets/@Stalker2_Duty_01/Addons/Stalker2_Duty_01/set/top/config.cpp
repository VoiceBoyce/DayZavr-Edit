class CfgPatches
{
	class S2_Duty_01
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
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class GorkaEJacket_ColorBase;
	class Stalker_2_Duty_01_Top_Colorbase: GorkaEJacket_ColorBase
	{
		scope=0;
		displayName="$STR_Duty_01_Top_01";
		descriptionShort="$STR_Duty_01_Top_02";
		model="Stalker2_Duty_01\set\top\models\Top_g.p3d";
		inventorySlot[]=
		{
			"Body"
		};
		itemInfo[]=
		{
			"Clothing",
			"Body"
		};
		attachments[]=
		{
			"VestPouch_Duty_01"
		};
		weight=1000;
		itemSize[]={4,6};
		quickBarBonus=2;
		ragQuantity=3;
		varWetMax=0.249;
		heatIsolation=0.40000001;
		itemsCargoSize[]={10,5};
		repairableWithKits[]={2,3};
		repairCosts[]={75,50};
		hiddenSelections[]=
		{
			"camo"
		};
		rotationFlags=17;
		class ClothingTypes
		{
			male="Stalker2_Duty_01\set\top\models\Top_m.p3d";
			female="Stalker2_Duty_01\set\top\models\Top_m.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=500;
					transferToAttachmentsCoef=0.5;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"Stalker2_Duty_01\set\top\data\Top.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"Stalker2_Duty_01\set\top\data\Top.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"Stalker2_Duty_01\set\top\data\Top_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"Stalker2_Duty_01\set\top\data\Top_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"Stalker2_Duty_01\set\top\data\Top_destruct.rvmat"
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
						damage=0.94999999;
					};
					class Blood
					{
						damage=0.94999999;
					};
					class Shock
					{
						damage=0;
					};
				};
				class Melee
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
						damage=0.44999999;
					};
				};
				class Infected
				{
					class Health
					{
						damage=0.1;
					};
					class Blood
					{
						damage=0.5;
					};
					class Shock
					{
						damage=0.15000001;
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
						damage=0.1;
					};
					class Shock
					{
						damage=0.1;
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
	class Stalker_2_Duty_01_Top: Stalker_2_Duty_01_Top_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\top\data\Top_co.paa"
		};
	};
	class Stalker_2_Duty_01_Top_ClearSky: Stalker_2_Duty_01_Top_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\top\data\Top_ClearSky.paa"
		};
	};
	class Stalker_2_Duty_01_Top_Korpus: Stalker_2_Duty_01_Top_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\top\data\Top_Korpus.paa"
		};
	};
	class Stalker_2_Duty_01_Top_Monolith: Stalker_2_Duty_01_Top_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\top\data\Top_Monolith.paa"
		};
	};
	class Stalker_2_Duty_01_Top_MSOP: Stalker_2_Duty_01_Top_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\top\data\Top_MSOP.paa"
		};
	};
	class Stalker_2_Duty_01_Top_Warta: Stalker_2_Duty_01_Top_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\top\data\Top_Warta.paa"
		};
	};
};
