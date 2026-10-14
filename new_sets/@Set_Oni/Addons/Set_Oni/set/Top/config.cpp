class CfgPatches
{
	class Vp_SET13
	{
		units[]=
		{
			"Vp_SET13_Top_Black"
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
	class Vp_SET13_Top_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_TOP";
		descriptionShort="";
		model="Set_Oni\set\Top\model\Top_g.p3d";
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
		itemsCargoSize[]={9,5};
		repairableWithKits[]={5,2};
		repairCosts[]={75,50};
		hiddenSelections[]=
		{
			"camo",
			"personality"
		};
		class ClothingTypes
		{
			male="Set_Oni\set\Top\model\Top_m.p3d";
			female="Set_Oni\set\Top\model\Top_f.p3d";
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
								"Set_Oni\set\Top\data\Top.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"Set_Oni\set\Top\data\Top.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"Set_Oni\set\Top\data\Top_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"Set_Oni\set\Top\data\Top_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"Set_Oni\set\Top\data\Top_destruct.rvmat"
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
	class Vp_SET13_Top_Black: Vp_SET13_Top_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Set_Oni\set\Top\data\Top_co.paa"
		};
	};
};
