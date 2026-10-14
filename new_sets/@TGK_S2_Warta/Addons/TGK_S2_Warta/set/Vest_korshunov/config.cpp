class CfgPatches
{
	class TGK_S2_Warta
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Characters"
		};
	};
};
class CfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class TGK_S2_WartaVest_Korshunov_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_WartaKorshunovVest_01";
		descriptionShort="$STR_WartaKorshunovVest_02";
		model="TGK_S2_Warta\set\Vest_korshunov\models\Warta_Korshunov_Vest_g.p3d";
		inventorySlot[]=
		{
			"Vest"
		};
		itemInfo[]=
		{
			"Clothing",
			"Vest"
		};
		attachments[]=
		{
			"WartaPouches1",
			"WartaPouches2_Korshunov",
			"WartaMedicalPouch1"
		};
		weight=8000;
		itemSize[]={5,5};
		itemsCargoSize[]={5,5};
		displayWeight=1;
		varWetMin=0.048999999;
		varWetMax=0.34999999;
		absorbency=0.40000001;
		heatIsolation=0.34999999;
		repairableWithKits[]={3,8};
		repairCosts[]={75,50};
		ragQuantity=1;
		hiddenSelections[]=
		{
			"Warta_Korshunov_Vest"
		};
		class ClothingTypes
		{
			male="TGK_S2_Warta\set\Vest_korshunov\models\Warta_Korshunov_Vest_m.p3d";
			female="TGK_S2_Warta\set\Vest_korshunov\models\Warta_Korshunov_Vest_m.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=650;
					transferToAttachmentsCoef=0.5;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"TGK_S2_Warta\set\Vest_korshunov\data\Vest.rvmat"
							}
						},
						
						{
							0.89999998,
							
							{
								"TGK_S2_Warta\set\Vest_korshunov\data\Vest.rvmat"
							}
						},
						
						{
							0.40000001,
							
							{
								"TGK_S2_Warta\set\Vest_korshunov\data\Vest_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_S2_Warta\set\Vest_korshunov\data\Vest_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_S2_Warta\set\Vest_korshunov\data\Vest_destruct.rvmat"
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
						damage=0.34999999;
					};
					class Blood
					{
						damage=0.2;
					};
					class Shock
					{
						damage=0.44999999;
					};
				};
				class Melee
				{
					class Health
					{
						damage=0.25;
					};
					class Blood
					{
						damage=0.15000001;
					};
					class Shock
					{
						damage=0.40000001;
					};
				};
				class Infected
				{
					class Health
					{
						damage=0.25;
					};
					class Blood
					{
						damage=0.15000001;
					};
					class Shock
					{
						damage=0.40000001;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage=0.40000001;
					};
					class Blood
					{
						damage=0.40000001;
					};
					class Shock
					{
						damage=0.40000001;
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
	class TGK_S2_WartaVest_Korshunov: TGK_S2_WartaVest_Korshunov_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_S2_Warta\set\Vest_korshunov\data\Vest_co.paa"
		};
	};
};
