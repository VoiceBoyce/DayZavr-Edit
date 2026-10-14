class CfgPatches
{
	class TGK_S2_Warta
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
	class TGK_S2_WartaJacket_Base: Clothing
	{
		scope=0;
		displayName="$STR_WartaTop_01";
		descriptionShort="$STR_WartaTop_02";
		model="TGK_S2_Warta\set\top\models\Warta_Jacket_g.p3d";
		inventorySlot[]=
		{
			"Body"
		};
		itemInfo[]=
		{
			"Clothing",
			"Body"
		};
		weight=2000;
		displayWeight=1;
		itemSize[]={4,6};
		itemsCargoSize[]={4,10};
		varWetMin=0.048999999;
		varWetMax=0.55000001;
		absorbency=0.55000001;
		heatIsolation=0.5;
		repairableWithKits[]={2,3};
		repairCosts[]={75,50};
		ragQuantity=5;
		hiddenSelections[]=
		{
			"Warta_Jacket_Camo"
		};
		class ClothingTypes
		{
			male="TGK_S2_Warta\set\top\models\Warta_Jacket_m.p3d";
			female="TGK_S2_Warta\set\top\models\Warta_Jacket_m.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=300;
					transferToAttachmentsCoef=0.5;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"TGK_S2_Warta\set\top\data\Top.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_S2_Warta\set\top\data\Top.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_S2_Warta\set\top\data\Top_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_S2_Warta\set\top\data\Top_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_S2_Warta\set\top\data\Top_destruct.rvmat"
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
	class TGK_S2_WartaJacket: TGK_S2_WartaJacket_Base
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_S2_Warta\set\top\data\Top_co.paa"
		};
	};
};
