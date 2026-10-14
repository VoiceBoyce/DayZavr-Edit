class CfgPatches
{
	class Vp_SET15
	{
		units[]=
		{
			"Vp_SET15_Top_Black"
		};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters"
		};
	};
};
class cfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class Vp_SET15_Top_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_Elegant_Top";
		descriptionShort="";
		model="set_Elegant\set\Top\model\top_g.p3d";
		attachments[]={};
		inventorySlot[]=
		{
			"Body"
		};
		itemInfo[]=
		{
			"Clothing",
			"Body"
		};
		quickBarBonus=2;
		weight=310;
		itemSize[]={3,2};
		varWetMax=0.050000001;
		heatIsolation=0.5;
		itemsCargoSize[]={10,5};
		repairableWithKits[]={5,2};
		repairCosts[]={75,50};
		hiddenSelections[]=
		{
			"galstuk",
			"lokot",
			"top1",
			"top2"
		};
		class ClothingTypes
		{
			male="set_Elegant\set\Top\model\top_mf.p3d";
			female="set_Elegant\set\Top\model\top_mf.p3d";
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
								"set_Elegant\set\Top\data\galstuk.rvmat",
								"set_Elegant\set\Top\data\lokot.rvmat",
								"set_Elegant\set\Top\data\top1.rvmat",
								"set_Elegant\set\Top\data\top2.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"set_Elegant\set\Top\data\galstuk.rvmat",
								"set_Elegant\set\Top\data\lokot.rvmat",
								"set_Elegant\set\Top\data\top1.rvmat",
								"set_Elegant\set\Top\data\top2.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"set_Elegant\set\Top\data\galstuk_damage.rvmat",
								"set_Elegant\set\Top\data\lokot_damage.rvmat",
								"set_Elegant\set\Top\data\top1_damage.rvmat",
								"set_Elegant\set\Top\data\top2_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"set_Elegant\set\Top\data\galstuk_damage.rvmat",
								"set_Elegant\set\Top\data\lokot_damage.rvmat",
								"set_Elegant\set\Top\data\top1_damage.rvmat",
								"set_Elegant\set\Top\data\top2_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"set_Elegant\set\Top\data\galstuk_destruct.rvmat",
								"set_Elegant\set\Top\data\lokot_destruct.rvmat",
								"set_Elegant\set\Top\data\top1_destruct.rvmat",
								"set_Elegant\set\Top\data\top2_destruct.rvmat"
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
						damage=0.85000002;
					};
					class Blood
					{
						damage=0.80000001;
					};
					class Shock
					{
						damage=0.80000001;
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
						damage=0.80000001;
					};
					class Shock
					{
						damage=0.80000001;
					};
				};
				class Infected
				{
					class Health
					{
						damage=0.85000002;
					};
					class Blood
					{
						damage=0.80000001;
					};
					class Shock
					{
						damage=0.80000001;
					};
				};
				class FragGrenade
				{
					class Health
					{
						damage=0.85000002;
					};
					class Blood
					{
						damage=0.80000001;
					};
					class Shock
					{
						damage=0.80000001;
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
					soundSet="Shirt_pickup_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="Shirt_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class Vp_SET15_Top_Black: Vp_SET15_Top_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"set_Elegant\set\Top\data\galstuk_co.paa",
			"set_Elegant\set\Top\data\lokot_co.paa",
			"set_Elegant\set\Top\data\top1_co.paa",
			"set_Elegant\set\Top\data\top2_co.paa"
		};
	};
};
