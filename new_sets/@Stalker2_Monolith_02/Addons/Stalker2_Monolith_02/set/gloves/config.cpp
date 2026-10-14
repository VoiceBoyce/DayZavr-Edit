class CfgPatches
{
	class Stalker_2_Monolith_02_Gloves
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
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class TacticalGloves_ColorBase;
	class Stalker_2_Monolith_02_Gloves_ColorBase: TacticalGloves_ColorBase
	{
		scope=0;
		displayName="$STR_Stalker_2_Monolith_02_Gloves_01";
		descriptionShort="$STR_Stalker_2_Monolith_02_Gloves_02";
		model="Stalker2_Monolith_02\set\gloves\models\gloves_g.p3d";
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
		varWetMax=1;
		heatIsolation=0.5;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		rotationFlags=17;
		hiddenSelections[]=
		{
			"camo",
			"personality"
		};
		class ClothingTypes
		{
			male="Stalker2_Monolith_02\set\gloves\models\gloves_m.p3d";
			female="Stalker2_Monolith_02\set\gloves\models\gloves_m.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=500;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"Stalker2_Monolith_02\set\gloves\data\Gloves.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"Stalker2_Monolith_02\set\gloves\data\Gloves.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"Stalker2_Monolith_02\set\gloves\data\Gloves_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"Stalker2_Monolith_02\set\gloves\data\Gloves_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"Stalker2_Monolith_02\set\gloves\data\Gloves_destruct.rvmat"
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
	class S2_Monolith_02_Gloves: Stalker_2_Monolith_02_Gloves_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Monolith_02\set\gloves\data\Gloves.paa"
		};
	};
};
