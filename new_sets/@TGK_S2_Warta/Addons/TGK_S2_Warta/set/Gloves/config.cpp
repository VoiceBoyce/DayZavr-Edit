class CfgPatches
{
	class TGK_S2_Warta
	{
		units[]={};
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
	class TGK_S2_WartaGloves_Colorbase: Clothing
	{
		scope=0;
		displayName="$STR_S2_WartaGloves_01";
		descriptionShort="$STR_S2_WartaGloves_02";
		model="TGK_S2_Warta\set\Gloves\models\Warta_gloves_g.p3d";
		inventorySlot[]=
		{
			"Gloves"
		};
		simulation="clothing";
		itemInfo[]=
		{
			"Clothing",
			"Gloves"
		};
		weight=450;
		itemSize[]={2,2};
		varWetMax=0.048999999;
		heatIsolation=0.75;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		hiddenSelections[]=
		{
			"camo"
		};
		hiddenSelectionsTextures[]=
		{
			"TGK_S2_Warta\set\top\data\Top_co.paa"
		};
		class ClothingTypes
		{
			male="TGK_S2_Warta\set\Gloves\models\Warta_gloves_m.p3d";
			female="TGK_S2_Warta\set\Gloves\models\Warta_gloves_m.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=150;
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
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem
				{
					soundSet="WorkingGloves_pickup_SoundSet";
					id=797;
				};
				class drop
				{
					soundset="WorkingGloves_drop_SoundSet";
					id=898;
				};
			};
		};
	};
	class TGK_S2_WartaGloves: TGK_S2_WartaGloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_S2_Warta\set\top\data\Top_co.paa"
		};
	};
};
