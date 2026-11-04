class CfgPatches
{
	class VP_SET12
	{
		units[] = {"VP_SET12_Belt_Black"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Characters","DZ_Scripts"};
	};
};
class cfgVehicles
{
	class Clothing_Base;
	class Clothing: Clothing_Base{};
	class VP_SET12_Belt_ColorBase: Clothing
	{
		scope = 0;
		RankLevel = 3;
		displayName = "$STR_VBC_Rider_Belt";
		descriptionShort = "$STR_VBC_Rider_Belt_Desc";
		model = "set_Rider\set\Belt\Belt_g.p3d";
		attachments[] = {"Pistol"}; // DayZavr: в модели есть прокси fnp45 (слот pistol)
		vehicleClass = "Clothing";
		simulation = "clothing";
		inventorySlot[] = {"Hips"};
		itemInfo[] = {"Clothing","Hips"};
		itemSize[] = {3,2};
		itemsCargoSize[] = {10,3};
		quickBarBonus = 1;
		varWetMax = 0.249;
		heatIsolation = 0;
		weight = 800;
		repairableWithKits[] = {5,3};
		repairCosts[] = {30.0,25.0};
		lootCategory = "Crafted";
		hiddenSelections[] = {"belt_remen","fast","kobura","remen"};
		hiddenSelectionsTextures[] = {"set_Rider\set\Belt\data\Belt_smdi.paa","set_Rider\set\Belt\data\Fast_belt_co.paa","set_Rider\set\Belt\data\Kobura_co.paa","set_Rider\set\Belt\data\Belt_co.paa"};
		class ClothingTypes
		{
			male = "set_Rider\set\Belt\Belt_mf.p3d";
			female = "set_Rider\set\Belt\Belt_mf.p3d";
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 500;
					healthLevels[] = {{1,{"set_Rider\set\Belt\data\Belt_rem.rvmat","set_Rider\set\Belt\data\Fast_belt.rvmat","set_Rider\set\Belt\data\Kobura.rvmat","set_Rider\set\Belt\data\Belt.rvmat"}},{0.7,{"set_Rider\set\Belt\data\Belt_rem.rvmat","set_Rider\set\Belt\data\Fast_belt.rvmat","set_Rider\set\Belt\data\Kobura.rvmat","set_Rider\set\Belt\data\Belt.rvmat"}},{0.5,{"set_Rider\set\Belt\data\Belt_rem_damage.rvmat","set_Rider\set\Belt\data\Fast_belt_damage.rvmat","set_Rider\set\Belt\data\Kobura_damage.rvmat","set_Rider\set\Belt\data\Belt_damage.rvmat"}},{0.3,{"set_Rider\set\Belt\data\Belt_rem_damage.rvmat","set_Rider\set\Belt\data\Fast_belt_damage.rvmat","set_Rider\set\Belt\data\Kobura_damage.rvmat","set_Rider\set\Belt\data\Belt_damage.rvmat"}},{0,{"set_Rider\set\Belt\data\Belt_rem_destruct.rvmat","set_Rider\set\Belt\data\Fast_belt_destruct.rvmat","set_Rider\set\Belt\data\Kobura_destruct.rvmat","set_Rider\set\Belt\data\Belt_destruct.rvmat"}}};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem
				{
					soundSet = "WorkingGloves_pickup_SoundSet";
					id = 797;
				};
				class drop
				{
					soundset = "WorkingGloves_drop_SoundSet";
					id = 898;
				};
			};
		};
	};
	class VP_SET12_Belt_Black: VP_SET12_Belt_ColorBase
	{
		scope = 2;
		hiddenSelectionsTextures[] = {"set_Rider\set\Belt\data\Belt_rem_co.paa","set_Rider\set\Belt\data\Fast_belt_co.paa","set_Rider\set\Belt\data\Kobura_co.paa","set_Rider\set\Belt\data\Belt_co.paa"};
	};
};
