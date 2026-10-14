class CfgPatches
{
	class SM_VP_Multi_Cap_Set
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
	class SM_VP_Multi_Cap_ColorBase: Clothing
	{
		scope=0;
		displayName="Кепка с тактическими наушниками Мульти";
		descriptionShort="Кепка с тактическими наушниками от сета Мульти";
		model="set_mulit\cap\model\cap_g.p3d";
		inventorySlot[]=
		{
			"Headgear"
		};
		itemInfo[]=
		{
			"Clothing",
			"Headgear"
		};
		ragQuantity=6;
		quickBarBonus=2;
		weight=310;
		itemSize[]={3,2};
		varWetMax=0.050000001;
		heatIsolation=0.5;
		repairableWithKits[]={3,8};
		repairCosts[]={75,50};
		hiddenSelections[]=
		{
			"camo",
			"naushniki",
			"up"
		};
		headSelectionsToHide[]=
		{
			"Clipping_FlatCap"
		};
		class ClothingTypes
		{
			male="set_mulit\cap\model\cap_m.p3d";
			female="set_mulit\cap\model\cap_f.p3d";
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
								"set_mulit\cap\data\kap.rvmat",
								"set_mulit\cap\data\naush.rvmat",
								"set_mulit\cap\data\head.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"set_mulit\cap\data\kap.rvmat",
								"set_mulit\cap\data\naush.rvmat",
								"set_mulit\cap\data\head.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"set_mulit\cap\data\kap_damage.rvmat",
								"set_mulit\cap\data\naush_damage.rvmat",
								"set_mulit\cap\data\head_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"set_mulit\cap\data\kap_damage.rvmat",
								"set_mulit\cap\data\naush_damage.rvmat",
								"set_mulit\cap\data\head_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"set_mulit\cap\data\kap_destruct.rvmat",
								"set_mulit\cap\data\naush_destruct.rvmat",
								"set_mulit\cap\data\head_destruct.rvmat"
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
	class SM_VP_Multi_Cap: SM_VP_Multi_Cap_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"set_mulit\cap\data\kap_co.paa",
			"set_mulit\cap\data\naush_co.paa",
			"set_mulit\cap\data\head_co.paa"
		};
	};
};
