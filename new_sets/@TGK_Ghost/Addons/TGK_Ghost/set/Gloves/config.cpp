class CfgPatches
{
	class TGK_Ghost_Gloves
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
	class TGK_Ghost_Gloves_Colorbase: TacticalGloves_ColorBase
	{
		scope=0;
		displayName="$STR_Ghost_Gloves_01";
		descriptionShort="$STR_Ghost_Gloves_02";
		model="TGK_Ghost\set\Gloves\models\Gloves_g.p3d";
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
			male="TGK_Ghost\set\Gloves\models\Gloves_m.p3d";
			female="TGK_Ghost\set\Gloves\models\Gloves_m.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=200;
					healthLevels[]=
					{
						
						{
							1,
							
							{
								"TGK_Ghost\set\Gloves\data\Combat_gloves.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"TGK_Ghost\set\Gloves\data\Combat_gloves.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"TGK_Ghost\set\Gloves\data\Combat_gloves_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"TGK_Ghost\set\Gloves\data\Combat_gloves_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"TGK_Ghost\set\Gloves\data\Combat_gloves_destruct.rvmat"
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
	class TGK_Ghost_Gloves_Black: TGK_Ghost_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"TGK_Ghost\set\Gloves\data\Gloves_Black.paa"
		};
	};
};
