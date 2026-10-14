class CfgPatches
{
	class Vp_SET13
	{
		units[]=
		{
			"Vp_SET13_Gloves_Black"
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
	class Vp_SET13_Gloves_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_Glove";
		descriptionShort="";
		model="Set_Oni\set\Gloves\model\Gloves_g.p3d";
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
		rotationFlags=34;
		weight=454;
		itemSize[]={2,2};
		varWetMax=0.048999999;
		heatIsolation=1;
		repairableWithKits[]={5,2};
		repairCosts[]={30,25};
		hiddenSelections[]=
		{
			"camo",
			"personality"
		};
		class ClothingTypes
		{
			male="Set_Oni\set\Gloves\model\Gloves_m.p3d";
			female="Set_Oni\set\Gloves\model\Gloves_f.p3d";
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
								"Set_Oni\set\Gloves\data\Gloves.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"Set_Oni\set\Gloves\data\Gloves.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"Set_Oni\set\Gloves\data\Gloves_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"Set_Oni\set\Gloves\data\Gloves_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"Set_Oni\set\Gloves\data\Gloves_destruct.rvmat"
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
	class Vp_SET13_Gloves_Black: Vp_SET13_Gloves_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Set_Oni\set\Gloves\data\Gloves_co.paa"
		};
	};
};
