class CfgPatches
{
	class TGK_Casual_Top
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Characters",
			"DZ_Data"
		};
	};
};
class CfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class TGK_Casual_Top_Base: Clothing
	{
		scope=0;
		displayName="$STR_Casual_Top01";
		descriptionShort="$STR_Casual_Top02";
		model="TGK_CasualSoldier\set\Top\models\Top_g.p3d";
		inventorySlot[]=
		{
			"Body"
		};
		itemInfo[]=
		{
			"Clothing",
			"Body"
		};
		weight=1000;
		itemSize[]={4,6};
		varWetMax=0.85000002;
		heatIsolation=1;
		quickBarBonus=2;
		repairableWithKits[]={2,3};
		itemsCargoSize[]={7,6};
		repairCosts[]={75,50};
		hiddenSelections[]=
		{
			"camo"
		};
		class ClothingTypes
		{
			male="TGK_CasualSoldier\set\Top\models\Top_m.p3d";
			female="TGK_CasualSoldier\set\Top\models\Top_m.p3d";
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
								"TGK_CasualSoldier\set\Top\data\Hoodie_Zip.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_CasualSoldier\set\Top\data\Hoodie_Zip.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_CasualSoldier\set\Top\data\Hoodie_Zip_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_CasualSoldier\set\Top\data\Hoodie_Zip_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_CasualSoldier\set\Top\data\Hoodie_Zip_destruct.rvmat"
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
	class TGK_Casual_Top_Black: TGK_Casual_Top_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_CasualSoldier\set\Top\data\Hoodie_Zip_Black.paa"
		};
	};
	class TGK_Casual_Top_Blue: TGK_Casual_Top_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_CasualSoldier\set\Top\data\Hoodie_Zip_Blue.paa"
		};
	};
	class TGK_Casual_Top_Gray: TGK_Casual_Top_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_CasualSoldier\set\Top\data\Hoodie_Zip_Gray.paa"
		};
	};
	class TGK_Casual_Top_Green: TGK_Casual_Top_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_CasualSoldier\set\Top\data\Hoodie_Zip_Green.paa"
		};
	};
	class TGK_Casual_Top_Tan: TGK_Casual_Top_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_CasualSoldier\set\Top\data\Hoodie_Zip_Tan.paa"
		};
	};
};
