class CfgPatches
{
	class S2_Duty_01
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
	class Stalker_2_Duty_01_Gloves_Colorbase: TacticalGloves_ColorBase
	{
		scope=0;
		displayName="$STR_Duty_01_Gloves_01";
		descriptionShort="$STR_Duty_01_Gloves_02";
		model="Stalker2_Duty_01\set\gloves\models\Gloves_g.p3d";
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
			male="Stalker2_Duty_01\set\gloves\models\Gloves_m.p3d";
			female="Stalker2_Duty_01\set\gloves\models\Gloves_m.p3d";
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
								"Stalker2_Duty_01\set\gloves\data\Combat_gloves.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"Stalker2_Duty_01\set\gloves\data\Combat_gloves.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"Stalker2_Duty_01\set\gloves\data\Combat_gloves_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"Stalker2_Duty_01\set\gloves\data\Combat_gloves_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"Stalker2_Duty_01\set\gloves\data\Combat_gloves_destruct.rvmat"
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
	class Stalker_2_Duty_01_Gloves: Stalker_2_Duty_01_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\gloves\data\Gloves_Black.paa"
		};
	};
	class Stalker_2_Duty_01_Gloves_ClearSky: Stalker_2_Duty_01_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\gloves\data\Gloves_ClearSky.paa"
		};
	};
	class Stalker_2_Duty_01_Gloves_Korpus: Stalker_2_Duty_01_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\gloves\data\Gloves_Korpus.paa"
		};
	};
	class Stalker_2_Duty_01_Gloves_Monolith: Stalker_2_Duty_01_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\gloves\data\Gloves_Monolith.paa"
		};
	};
	class Stalker_2_Duty_01_Gloves_MSOP: Stalker_2_Duty_01_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\gloves\data\Gloves_MSOP.paa"
		};
	};
	class Stalker_2_Duty_01_Gloves_Warta: Stalker_2_Duty_01_Gloves_Colorbase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Stalker2_Duty_01\set\gloves\data\Gloves_Warta.paa"
		};
	};
};
