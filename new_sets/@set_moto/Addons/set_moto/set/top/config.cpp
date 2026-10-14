class CfgPatches
{
	class Vp_Set11
	{
		units[]=
		{
			"Vp_Set11_Motosuit"
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
class cfgVehicles
{
	class Clothing;
	class Vp_Set11_Motosuit_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_MotoSuit";
		descriptionShort="";
		model="set_moto\set\top\model\moto_top_g.p3d";
		attachments[]={};
		inventorySlot[]=
		{
			"Body"
		};
		itemInfo[]=
		{
			"Clothing",
			"Body",
			"Legs",
			"Mask"
		};
		quickBarBonus=2;
		weight=310;
		ragQuantity=6;
		itemSize[]={4,4};
		varWetMax=0.050000001;
		heatIsolation=1;
		itemsCargoSize[]={10,8};
		repairableWithKits[]={3,8};
		repairCosts[]={75,50};
		hiddenSelections[]=
		{
			"camo",
			"camo2",
			"mask"
		};
		class ClothingTypes
		{
			male="set_moto\set\top\model\moto_top_mf.p3d";
			female="set_moto\set\top\model\moto_top_mf.p3d";
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
								"set_moto\set\top\data\top.rvmat",
								"set_moto\set\top\data\pants.rvmat",
								"set_moto\set\top\data\mask.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"set_moto\set\top\data\top.rvmat",
								"set_moto\set\top\data\pants.rvmat",
								"set_moto\set\top\data\mask.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"set_moto\set\top\data\top_damage.rvmat",
								"set_moto\set\top\data\pants_damage.rvmat",
								"set_moto\set\top\data\mask_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"set_moto\set\top\data\top_damage.rvmat",
								"set_moto\set\top\data\pants_damage.rvmat",
								"set_moto\set\top\data\mask_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"set_moto\set\top\data\top_destruct.rvmat",
								"set_moto\set\top\data\pants_destruct.rvmat",
								"set_moto\set\top\data\mask_destruct.rvmat"
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
	class Vp_Set11_Motosuit: Vp_Set11_Motosuit_ColorBase
	{
		scope=2;
		inventorySlot[]=
		{
			"Body"
		};
		itemInfo[]=
		{
			"Clothing",
			"Body"
		};
		hiddenSelectionsTextures[]=
		{
			"set_moto\set\top\data\top_co.paa",
			"set_moto\set\top\data\pants_co.paa",
			"set_moto\set\top\data\mask_co.paa"
		};
	};
};
