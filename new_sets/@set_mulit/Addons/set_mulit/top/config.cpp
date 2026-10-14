class CfgPatches
{
	class SM_VP_Multi_Jacket_Set
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data"
		};
	};
};
class cfgVehicles
{
	class Clothing;
	class SM_VP_Multi_Jacket_ColorBase: Clothing
	{
		scope=0;
		displayName="Куртка Мульти";
		descriptionShort="Куртка от сета Мульти.";
		model="set_mulit\top\model\top_g.p3d";
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
		ragQuantity=6;
		itemSize[]={3,2};
		varWetMax=0.050000001;
		heatIsolation=0.5;
		itemsCargoSize[]={9,5};
		repairableWithKits[]={3,8};
		repairCosts[]={75,50};
		hiddenSelections[]=
		{
			"camo"
		};
		class ClothingTypes
		{
			male="set_mulit\top\model\top_m.p3d";
			female="set_mulit\top\model\top_f.p3d";
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
								"set_mulit\top\data\top.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"set_mulit\top\data\top.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"set_mulit\top\data\top_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"set_mulit\top\data\top_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"set_mulit\top\data\top_destruct.rvmat"
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
	class SM_VP_Multi_Jacket: SM_VP_Multi_Jacket_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"set_mulit\top\data\top_co.paa"
		};
	};
};
