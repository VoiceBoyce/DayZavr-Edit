class CfgPatches
{
	class VP_SET13
	{
		units[]=
		{
			"Vp_SET13_Belt_Black"
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
	class Vp_SET13_Belt_ColorBase: Clothing
	{
		scope=0;
		RankLevel=3;
		displayName="$STR_Beltsystem";
		descriptionShort="";
		model="Set_Oni\set\Belt\model\Belt_g.p3d";
		attachments[]=
		{
			""
		};
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
		itemsCargoSize[]={4,10};
		quickBarBonus=2;
		varWetMax=0.048999999;
		heatIsolation=0.5;
		weight=550;
		repairableWithKits[]={5,3};
		repairCosts[]={30,25};
		lootCategory="Crafted";
		hiddenSelections[]=
		{
			"camo",
			"karman_belt",
			"kobura",
			"magaz_belt"
		};
		class ClothingTypes
		{
			male="Set_Oni\set\Belt\model\Belt_mf.p3d";
			female="Set_Oni\set\Belt\model\Belt_mf.p3d";
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
								"Set_Oni\set\Belt\data\Belt.rvmat",
								"Set_Oni\set\Belt\data\Karman_belt.rvmat",
								"Set_Oni\set\Belt\data\kobura.rvmat",
								"Set_Oni\set\Belt\data\Magaz_belt.rvmat"
							}
						},
						
						{
							0.69999999,
							
							{
								"Set_Oni\set\Belt\data\Belt.rvmat",
								"Set_Oni\set\Belt\data\Karman_belt.rvmat",
								"Set_Oni\set\Belt\data\kobura.rvmat",
								"Set_Oni\set\Belt\data\Magaz_belt.rvmat"
							}
						},
						
						{
							0.5,
							
							{
								"Set_Oni\set\Belt\data\Belt_damage.rvmat",
								"Set_Oni\set\Belt\data\Karman_belt_damage.rvmat",
								"Set_Oni\set\Belt\data\kobura_damage.rvmat",
								"Set_Oni\set\Belt\data\Magaz_belt_damage.rvmat"
							}
						},
						
						{
							0.30000001,
							
							{
								"Set_Oni\set\Belt\data\Belt_damage.rvmat",
								"Set_Oni\set\Belt\data\Karman_belt_damage.rvmat",
								"Set_Oni\set\Belt\data\kobura_damage.rvmat",
								"Set_Oni\set\Belt\data\Magaz_belt_damage.rvmat"
							}
						},
						
						{
							0,
							
							{
								"Set_Oni\set\Belt\data\Belt_destruct.rvmat",
								"Set_Oni\set\Belt\data\Karman_belt_destruct.rvmat",
								"Set_Oni\set\Belt\data\kobura_destruct.rvmat",
								"Set_Oni\set\Belt\data\Magaz_belt_destruct.rvmat"
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
	class Vp_SET13_Belt_Black: Vp_SET13_Belt_ColorBase
	{
		scope=2;
		hiddenSelectionsTextures[]=
		{
			"Set_Oni\set\Belt\data\Belt_co.paa",
			"Set_Oni\set\Belt\data\Karman_belt_co.paa",
			"Set_Oni\set\Belt\data\kobura_co.paa",
			"Set_Oni\set\Belt\data\Magaz_belt_co.paa"
		};
	};
};
