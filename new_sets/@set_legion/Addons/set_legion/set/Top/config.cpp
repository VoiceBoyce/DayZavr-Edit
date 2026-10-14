class CfgPatches
{
	class DZ_Characters_Tops
	{
		units[]=
		{
			"Vp_SET20_Top"
		};
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
	class Vp_SET20_Top_ColorBase: Clothing
	{
		displayName="$STR_Legion_Top";
		descriptionShort="";
		model="set_legion\set\Top\model\top_g.p3d";
		inventorySlot[]=
		{
			"Body"
		};
		itemInfo[]=
		{
			"Clothing",
			"Body"
		};
		weight=3000;
		itemSize[]={5,5};
		itemsCargoSize[]={6,4};
		varWetMax=1;
		heatIsolation=1;
		quickBarBonus=1;
		featherQuantity=20;
		repairableWithKits[]={5,2};
		repairCosts[]={25,25};
		hiddenSelections[]=
		{
			"top",
			"top1",
			"top2",
			"znak"
		};
		class ClothingTypes
		{
			male="set_legion\set\Top\model\top_mf.p3d";
			female="set_legion\set\Top\model\top_mf.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=1500;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"set_legion\set\Top\data\top.rvmat",
								"set_legion\set\Top\data\top1.rvmat",
								"set_legion\set\Top\data\top2.rvmat",
								"set_legion\set\Top\data\znak.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"set_legion\set\Top\data\top.rvmat",
								"set_legion\set\Top\data\top1.rvmat",
								"set_legion\set\Top\data\top2.rvmat",
								"set_legion\set\Top\data\znak.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"set_legion\set\Top\data\top_damage.rvmat",
								"set_legion\set\Top\data\top1_damage.rvmat",
								"set_legion\set\Top\data\top2_damage.rvmat",
								"set_legion\set\Top\data\znak_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"set_legion\set\Top\data\top_damage.rvmat",
								"set_legion\set\Top\data\top1_damage.rvmat",
								"set_legion\set\Top\data\top2_damage.rvmat",
								"set_legion\set\Top\data\znak_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"set_legion\set\Top\data\top_destruct.rvmat",
								"set_legion\set\Top\data\top1_destruct.rvmat",
								"set_legion\set\Top\data\top2_destruct.rvmat",
								"set_legion\set\Top\data\znak_destruct.rvmat"
							}
						}
					};
				};
			};
			class GlobalArmor
			{
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
	class Vp_SET20_Top: Vp_SET20_Top_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"set_legion\set\Top\data\top_co.paa",
			"set_legion\set\Top\data\top1_co.paa",
			"set_legion\set\Top\data\top2_co.paa",
			"set_legion\set\Top\data\znak_co.paa"
		};
	};
};
