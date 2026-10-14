class CfgPatches
{
	class VP_SET15
	{
		units[]=
		{
			"VP_SET15_Belt_Black"
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
	class Clothing_Base;
	class Clothing: Clothing_Base
	{
	};
	class VP_SET15_Belt_ColorBase: Clothing
	{
		scope=0;
		displayName="$STR_Elegant_Belt";
		descriptionShort="";
		model="set_Elegant\set\Belt\model\belt_g.p3d";
		attachments[]={};
		vehicleClass="Clothing";
		simulation="clothing";
		inventorySlot[]=
		{
			"Hips"
		};
		itemInfo[]=
		{
			"Clothing",
			"Hips"
		};
		itemSize[]={5,5};
		itemsCargoSize[]={10,5};
		quickBarBonus=2;
		varWetMax=0.048999999;
		heatIsolation=0.5;
		weight=550;
		repairableWithKits[]={5,3};
		repairCosts[]={30,25};
		lootCategory="Crafted";
		hiddenSelections[]=
		{
			"belt",
			"belt_magaz",
			"fast_belt",
			"kobura",
			"rem_belt"
		};
		class ClothingTypes
		{
			male="set_Elegant\set\Belt\model\belt_mf.p3d";
			female="set_Elegant\set\Belt\model\belt_mf.p3d";
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
								"set_Elegant\set\Belt\data\Belt.rvmat",
								"set_Elegant\set\Belt\data\Belt_magaz.rvmat",
								"set_Elegant\set\Belt\data\Fast_belt.rvmat",
								"set_Elegant\set\Belt\data\kobura.rvmat",
								"set_Elegant\set\Belt\data\rem_belt.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"set_Elegant\set\Belt\data\Belt.rvmat",
								"set_Elegant\set\Belt\data\Belt_magaz.rvmat",
								"set_Elegant\set\Belt\data\Fast_belt.rvmat",
								"set_Elegant\set\Belt\data\kobura.rvmat",
								"set_Elegant\set\Belt\data\rem_belt.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"set_Elegant\set\Belt\data\Belt_damage.rvmat",
								"set_Elegant\set\Belt\data\Belt_magaz_damage.rvmat",
								"set_Elegant\set\Belt\data\Fast_belt_damage.rvmat",
								"set_Elegant\set\Belt\data\kobura_damage.rvmat",
								"set_Elegant\set\Belt\data\rem_belt_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"set_Elegant\set\Belt\data\Belt_damage.rvmat",
								"set_Elegant\set\Belt\data\Belt_magaz_damage.rvmat",
								"set_Elegant\set\Belt\data\Fast_belt_damage.rvmat",
								"set_Elegant\set\Belt\data\kobura_damage.rvmat",
								"set_Elegant\set\Belt\data\rem_belt_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"set_Elegant\set\Belt\data\Belt_destruct.rvmat",
								"set_Elegant\set\Belt\data\Belt_magaz_destruct.rvmat",
								"set_Elegant\set\Belt\data\Fast_belt_destruct.rvmat",
								"set_Elegant\set\Belt\data\kobura_destruct.rvmat",
								"set_Elegant\set\Belt\data\rem_belt_destruct.rvmat"
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
	class VP_SET15_Belt_Black: VP_SET15_Belt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"set_Elegant\set\Belt\data\Belt_co.paa",
			"set_Elegant\set\Belt\data\Belt_magaz_co.paa",
			"set_Elegant\set\Belt\data\Fast_belt_co.paa",
			"set_Elegant\set\Belt\data\kobura_co.paa",
			"set_Elegant\set\Belt\data\rem_belt_co.paa"
		};
	};
};
