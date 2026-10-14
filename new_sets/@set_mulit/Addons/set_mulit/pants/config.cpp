class CfgPatches
{
	class SM_VP_Multi_Pants_Set
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
	class SM_VP_Multi_Pants_ColorBase: Clothing
	{
		scope=0;
		displayName="Штаны Мульти";
		descriptionShort="Штаны от сета Мульти";
		model="set_mulit\pants\model\pants_g.p3d";
		inventorySlot[]=
		{
			"Legs"
		};
		itemInfo[]=
		{
			"Clothing",
			"Legs"
		};
		quickBarBonus=2;
		weight=310;
		ragQuantity=6;
		itemSize[]={3,2};
		varWetMax=0.050000001;
		heatIsolation=0.5;
		itemsCargoSize[]={8,5};
		repairableWithKits[]={3,8};
		repairCosts[]={75,50};
		hiddenSelections[]=
		{
			"camo",
			"personality"
		};
		class ClothingTypes
		{
			male="set_mulit\pants\model\pants_m.p3d";
			female="set_mulit\pants\model\pants_f.p3d";
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
								"set_mulit\pants\data\pants.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"set_mulit\pants\data\pants.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"set_mulit\pants\data\pants_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"set_mulit\pants\data\pants_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"set_mulit\pants\data\pants_destruct.rvmat"
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
	class SM_VP_Multi_Pants: SM_VP_Multi_Pants_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"set_mulit\pants\data\pants_co.paa"
		};
	};
};
