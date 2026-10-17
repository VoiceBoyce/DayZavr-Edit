////////////////////////////////////////////////////////////////////
//DeRap: config.bin
//Produced from mikero's Dos Tools Dll version 10.13
//https://mikero.bytex.digital/Downloads
//'now' is Wed Jan 21 21:31:13 2026 : 'file' last modified on Wed Jan 21 19:18:55 2026
////////////////////////////////////////////////////////////////////

#define _ARMA_

class CfgPatches
{
	class DRL_CraftMenu
	{
		units[] =
		{
			"DRL_Craft_Sewing_scissors",
			"DRL_Craft_Metalic_thimble",
			"DRL_Craft_ScrewDriver",
			"DRL_Craft_Lamp",
			"DRL_Craft_Table",
			"DRL_Craft_Sewing_Table",
			"DRL_Craft_Tools",
			"DRL_Craft_DrillSmal",
			"DRL_Craft_Paints",
			"DRL_Craft_Anvil",
			"DRL_Craft_Box",
			"DRL_Craft_Grinder",
			"DRL_Craft_Vise",
			"DRL_Craft_Anvil2",
			"DRL_Craft_Drill2",
			"DRL_Craft_Micro",
			"DRL_Craft_lab_bunsen",
			"DRL_Craft_Kit",
			"DRL_Craft_Kit1",
			"DRL_Craft_Kit2",
			"DRL_Craft_Kit3",
			"DRL_Craft_Kit4",
			"DRL_Craft_Kit5",
			"DRL_Craft_Kit6",
			"DRL_Craft_Kit7",
			"DRL_Craft_Kit8",
			"DRL_Craft_Kit9",
			"DRL_Craft_Garden_Kit",
			"DRL_Craft_Cook_Kit",
			"DRL_Craft_Build_Kit",
			"DRL_Craft_Workbench_Kit",
			"DRL_Craft_Research_Table_Kit",
			"DRL_Craft_WB",
			"DRL_Craft_WB1",
			"DRL_Craft_WB2",
			"DRL_Craft_WB3",
			"DRL_Craft_WB4",
			"DRL_Craft_WB5",
			"DRL_Craft_WB6",
			"DRL_Craft_WB7",
			"DRL_Craft_WB8",
			"DRL_Craft_WB9",
			"DRL_Craft_WB_Static",
			"DRL_Craft_Garden",
			"DRL_Craft_Cook",
			"DRL_Craft_Build",
			"DRL_Craft_Workbench",
			"DRL_Craft_Welding_Machine",
			"DRL_Craft_Sewing",
			"DRL_Craft_Microwave",
			"DRL_Craft_NoteBook",
			"DRL_Craft_Microscope",
			"DRL_Craft_Circular",
			"DRL_Craft_Research_Table",
			"DRL_Craft_Research_Table_Static",
			"DRL_Craft_Blueprint",
			"DRL_Craft_Research_Book",
			"DRL_Craft_Truck_Lift",
			"DRL_Craft_Truck_Lift_Static",
			"DRL_Craft_Truck_Lift_Kit",
			"DRL_Craft_Car_Jack",
			"DRL_Craft_Covered_Car",
			"DRL_Craft_Firepit",
			"DRL_Craft_Firepit_Static",
			"DRL_Craft_Firepit_Kit"
		};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Gear_Tools","DZ_Gear_Camping","DZ_Gear_Food","INT_UIFramework"};
	};
};
class CfgMods
{
	class DRL_CraftMenu
	{
		dir = "DRL_CraftMenu";
		picture = "";
		action = "";
		hideName = 0;
		hidePicture = 1;
		name = "DRL_CraftMenu";
		credits = "";
		author = "Drulik";
		authorID = "0";
		version = "1";
		extra = 0;
		type = "mod";
		inputs = "DRL_CraftMenu/modded_inputs.xml";
		dependencies[] = {"Game","World","Mission"};
		class defs
		{
			class imageSets
			{
				files[] =
				{
					"DRL_CraftMenu/UI/imagesets/drl_craft_abstract_050.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_achievement.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_acid.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_ak47u.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_ammo_box.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_anvil_impact.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_apothecary.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_back_forth.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_backward_time.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_baitinfo.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_barbecue.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_battery_pack_alt.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_battery_pack.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_battery_plus.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_beer_stein.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_bloodregen3.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_build1.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_build3.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_button_def.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_button_off.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_button_start.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_cannedperk.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_cardboard_box_closed.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_chain_mail.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_clockwise_rotation.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_clockwork.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_craftmanu.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_cross_mark.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_cycle.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_deseasesshow.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_error.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_info.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_ironcloth1.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_kevlar_vest.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_recipemanu.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_recipemanu2.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_recipemanu_armor.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_recipemanu_armor2.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_recipemanu_build.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_recipemanu_build2.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_recipemanu_cloth.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_recipemanu_cloth2.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_recipemanu_electro.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_recipemanu_food.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_recipemanu_food2.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_recipemanu_medical.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_recipemanu_part.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_recipemanu_parts.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_recipemanu_parts2.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_recipemanu_parts3.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_recipemanu_wepon.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_recipemanu_wepon2.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_recipemanust.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_recycling.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_watch.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_learning_fon.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_work.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_work3.imageset",
					"DRL_CraftMenu/UI/imagesets/drl_craft_work5.imageset"
				};
			};
			class gameScriptModule
			{
				value = "";
				files[] = {"DRL_CraftMenu/scripts/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"DRL_CraftMenu/scripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"DRL_CraftMenu/scripts/5_Mission"};
			};
		};
	};
};
class CfgNonAIVehicles
{
	class ProxyAttachment;
	class ProxyPT: ProxyAttachment
	{
		scope = 2;
		inventorySlot[] = {"DRL_CraftattachPaints"};
		model = "DRL_CraftMenu\model\wbp\Proxy\PT.p3d";
	};
	class ProxySEW: ProxyAttachment
	{
		scope = 2;
		inventorySlot[] = {"DRL_CraftattachSEW"};
		model = "DRL_CraftMenu\model\wbp\Proxy\SEW.p3d";
	};
	class ProxySEWTAB: ProxyAttachment
	{
		scope = 2;
		inventorySlot[] = {"DRL_CraftattachSEWTAB"};
		model = "DRL_CraftMenu\model\wbp\Proxy\SEWTAB.p3d";
	};
	class ProxyNB: ProxyAttachment
	{
		scope = 2;
		inventorySlot[] = {"DRL_CraftattachNB"};
		model = "DRL_CraftMenu\model\wbp\Proxy\NB.p3d";
	};
	class ProxyMP: ProxyAttachment
	{
		scope = 2;
		inventorySlot[] = {"DRL_CraftattachMP"};
		model = "DRL_CraftMenu\model\wbp\Proxy\MP.p3d";
	};
	class ProxyMICRO: ProxyAttachment
	{
		scope = 2;
		inventorySlot[] = {"DRL_CraftattachMICRO"};
		model = "DRL_CraftMenu\model\wbp\Proxy\MICRO.p3d";
	};
	class ProxyLMP: ProxyAttachment
	{
		scope = 2;
		inventorySlot[] = {"DRL_CraftattachLMP"};
		model = "DRL_CraftMenu\model\wbp\Proxy\LMP.p3d";
	};
	class ProxyVic: ProxyAttachment
	{
		scope = 2;
		inventorySlot[] = {"DRL_Craftattach1"};
		model = "DRL_CraftMenu\model\wbp\Proxy\Vic.p3d";
	};
	class ProxyAnv: ProxyAttachment
	{
		scope = 2;
		inventorySlot[] = {"DRL_Craftattach2"};
		model = "DRL_CraftMenu\model\wbp\Proxy\Anv.p3d";
	};
	class ProxyTabl: ProxyAttachment
	{
		scope = 2;
		inventorySlot[] = {"DRL_CraftattachP"};
		model = "DRL_CraftMenu\model\wbp\Proxy\Tabl.p3d";
	};
	class ProxyTool: ProxyAttachment
	{
		scope = 2;
		inventorySlot[] = {"DRL_CraftattachT"};
		model = "DRL_CraftMenu\model\wbp\Proxy\Tool.p3d";
	};
	class ProxyCircul: ProxyAttachment
	{
		scope = 2;
		inventorySlot[] = {"DRL_CraftattachC"};
		model = "DRL_CraftMenu\model\wbp\Proxy\Circul.p3d";
	};
	class ProxyBX: ProxyAttachment
	{
		scope = 2;
		inventorySlot[] = {"DRL_CraftattachBX"};
		model = "DRL_CraftMenu\model\wbp\Proxy\BX.p3d";
	};
	class ProxyWM: ProxyAttachment
	{
		scope = 2;
		inventorySlot[] = {"DRL_CraftattachWM"};
		model = "DRL_CraftMenu\model\wbp\Proxy\WM.p3d";
	};
	class ProxyDR: ProxyAttachment
	{
		scope = 2;
		inventorySlot[] = {"DRL_CraftattachDrill"};
		model = "DRL_CraftMenu\model\wbp\Proxy\DR.p3d";
	};
	class Proxyjack: ProxyAttachment
	{
		scope = 2;
		inventorySlot[] = {"DRL_CraftattachJack"};
		model = "DRL_CraftMenu\model\Truck_Lift\Proxy\jack.p3d";
	};
};
class CfgSlots
{
	class Slot_DRL_Craftattach
	{
		name = "DRL_Craftattach";
		displayName = "#STR_WB_GRINDER";
		selection = "DRL_Craftattach";
		ghostIcon = "cat_vehicle_engine";
		show = "true";
	};
	class Slot_DRL_CraftattachPaints
	{
		name = "DRL_CraftattachPaints";
		displayName = "#STR_WB_PAINTS";
		selection = "DRL_CraftattachPaints";
		ghostIcon = "cat_vehicle_engine";
		show = "true";
	};
	class Slot_DRL_CraftattachSEW
	{
		name = "DRL_CraftattachSEW";
		displayName = "#STR_WB_SEWING_MACHINE";
		selection = "DRL_CraftattachSEW";
		ghostIcon = "cat_vehicle_engine";
		show = "true";
	};
	class Slot_DRL_CraftattachSEWTAB
	{
		name = "DRL_CraftattachSEWTAB";
		displayName = "#STR_WB_SEWING_TABLE";
		selection = "DRL_CraftattachSEWTAB";
		ghostIcon = "cat_bb_watchtower_wall_2";
		show = "true";
	};
	class Slot_DRL_CraftPropsThimble
	{
		name = "DRL_CraftPropsThimble";
		displayName = "#STR_WB_SPOOL";
		selection = "DRL_CraftPropsThimble";
		ghostIcon = "cat_vehicle_engine";
		show = "true";
	};
	class Slot_DRL_CraftPropsScissors
	{
		name = "DRL_CraftPropsScissors";
		displayName = "#STR_WB_SCISSORS";
		selection = "DRL_CraftPropsScissors";
		ghostIcon = "cat_vehicle_engine";
		show = "true";
	};
	class Slot_DRL_CraftattachMP
	{
		name = "DRL_CraftattachMP";
		displayName = "#STR_WB_MICROSCOPE";
		selection = "DRL_CraftattachMP";
		ghostIcon = "cat_vehicle_engine";
		show = "true";
	};
	class Slot_DRL_CraftattachMICRO
	{
		name = "DRL_CraftattachMICRO";
		displayName = "#STR_WB_MICROWAVE";
		selection = "DRL_CraftattachMICRO";
		ghostIcon = "cat_vehicle_engine";
		show = "true";
	};
	class Slot_DRL_CraftattachNB
	{
		name = "DRL_CraftattachNB";
		displayName = "#STR_WB_LAPTOP";
		selection = "DRL_CraftattachNB";
		ghostIcon = "cat_vehicle_engine";
		show = "true";
	};
	class Slot_DRL_CraftattachLMP
	{
		name = "DRL_CraftattachLMP";
		displayName = "#STR_WB_LAMP";
		selection = "DRL_CraftattachLMP";
		ghostIcon = "lights";
		show = "true";
	};
	class Slot_DRL_LMP_Bat
	{
		name = "DRL_LMP_Bat";
		displayName = "#STR_WB_POWER";
		selection = "DRL_LMP_Bat";
		ghostIcon = "electricity";
		show = "true";
	};
	class Slot_DRL_WM_Gas
	{
		name = "DRL_WM_Gas";
		displayName = "#STR_WB_FUEL";
		selection = "DRL_WM_Gas";
		ghostIcon = "gascanister";
		show = "true";
	};
	class Slot_DRL_CraftattachP
	{
		name = "DRL_CraftattachP";
		displayName = "#STR_WB_WORKTOP";
		selection = "DRL_CraftattachP";
		ghostIcon = "cat_bb_watchtower_wall_2";
		show = "true";
	};
	class Slot_DRL_CraftattachT
	{
		name = "DRL_CraftattachT";
		displayName = "#STR_WB_TOOLS";
		selection = "DRL_CraftattachT";
		ghostIcon = "rope";
		show = "true";
	};
	class Slot_DRL_CraftattachC
	{
		name = "DRL_CraftattachC";
		displayName = "#STR_WB_CIRCULAR_SAW";
		selection = "DRL_CraftattachC";
		ghostIcon = "cat_vehicle_engine";
		show = "true";
	};
	class Slot_DRL_CraftattachBX
	{
		name = "DRL_CraftattachBX";
		displayName = "#STR_WB_BOX";
		selection = "DRL_CraftattachBX";
		ghostIcon = "cat_common_cargo";
		show = "true";
	};
	class Slot_DRL_CraftattachWM
	{
		name = "DRL_CraftattachWM";
		displayName = "#STR_WB_WELDER";
		selection = "DRL_CraftattachWM";
		ghostIcon = "explosive";
		show = "true";
	};
	class Slot_DRL_CraftattachDrill
	{
		name = "DRL_CraftattachDrill";
		displayName = "#STR_WB_DRILL";
		selection = "DRL_CraftattachDrill";
		ghostIcon = "cat_vehicle_engine";
		show = "true";
	};
	class Slot_DRL_Craftattach1
	{
		name = "DRL_Craftattach1";
		displayName = "#STR_WB_VISE";
		selection = "DRL_Craftattach1";
		ghostIcon = "cat_vehicle_engine";
		show = "true";
	};
	class Slot_DRL_Craftattach2
	{
		name = "DRL_Craftattach2";
		displayName = "#STR_WB_ANVIL";
		selection = "DRL_Craftattach2";
		ghostIcon = "cat_vehicle_engine";
		show = "true";
	};
	class Slot_DRL_Craftattach3
	{
		name = "DRL_Craftattach3";
		displayName = "#STR_WB_DRILL";
		selection = "DRL_Craftattach3";
		ghostIcon = "cat_vehicle_engine";
	};
	class Slot_DRL_Craftattach4
	{
		name = "DRL_Craftattach4";
		displayName = "#STR_WB_MICROSCOPE";
		selection = "DRL_Craftattach4";
		ghostIcon = "cat_vehicle_engine";
	};
	class Slot_DRL_Craftattach5
	{
		name = "DRL_Craftattach5";
		displayName = "#STR_WB_TRIPOD";
		selection = "DRL_Craftattach5";
		ghostIcon = "cat_vehicle_engine";
		show = "true";
	};
	class Slot_DRL_CraftattachJack
	{
		name = "DRL_CraftattachJack";
		displayName = "#STR_WB_JACK";
		selection = "DRL_CraftattachJack";
		ghostIcon = "cat_vehicle_engine";
		show = "true";
	};
};
class CfgVehicles
{
	class WoodenCrate;
	class Container_Base;
	class Inventory_Base;
	class HouseNoDestruct;
	class DRL_Craft_Blueprint: Inventory_Base
	{
		scope = 2;
		displayName = "#STR_DRL_CRAFT_BLUEPRINT";
		descriptionShort = "#STR_DRL_CRAFT_BLUEPRINT_DESC";
		model = "DRL_CraftMenu\model\Blueprint\Blueprint.p3d";
		weight = 50;
		itemSize[] = {2,1};
		canBeSplit = 1;
		varQuantityInit = 1;
		varQuantityMin = 0;
		varQuantityMax = 10;
		varStackMax = 10;
		rotationFlags = 16;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[] =
					{
						{1.0,{"DRL_CraftMenu\model\Blueprint\Blueprint.rvmat"}},
						{0.7,{"DRL_CraftMenu\model\Blueprint\Blueprint.rvmat"}},
						{0.5,{"DRL_CraftMenu\model\Blueprint\Blueprint_damage.rvmat"}},
						{0.3,{"DRL_CraftMenu\model\Blueprint\Blueprint_damage.rvmat"}},
						{0.0,{"DRL_CraftMenu\model\Blueprint\Blueprint_destruct.rvmat"}}
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
					soundSet = "pickUpPaper_SoundSet";
					id = 797;
				};
				class drop
				{
					soundSet = "dropPaper_SoundSet";
					id = 898;
				};
			};
		};
	};
	class DRL_Craft_Research_Book: Inventory_Base
	{
		scope = 2;
		displayName = "#STR_DRL_RESEARCH_BOOK";
		descriptionShort = "#STR_DRL_RESEARCH_BOOK_DESC";
		model = "DRL_CraftMenu\model\Book\Book.p3d";
		rotationFlags = 16;
		weight = 400;
		itemSize[] = {2,2};
		itemBehaviour = 1;
		isMeleeWeapon = 1;
		absorbency = 0.8;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[] =
					{
						{1.0,{"DRL_CraftMenu\model\Book\Book.rvmat"}},
						{0.7,{"DRL_CraftMenu\model\Book\Book.rvmat"}},
						{0.5,{"DRL_CraftMenu\model\Book\Book_damage.rvmat"}},
						{0.3,{"DRL_CraftMenu\model\Book\Book_damage.rvmat"}},
						{0.0,{"DRL_CraftMenu\model\Book\Book_destruct.rvmat"}}
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
					soundSet = "pickUpPaper_SoundSet";
					id = 797;
				};
				class drop
				{
					soundSet = "dropPaper_SoundSet";
					id = 898;
				};
			};
		};
	};
	class DRL_Craft_Sewing_scissors: Inventory_Base
	{
		scope = 2;
		displayName = "#STR_WB_SCISSORS";
		descriptionShort = "#STR_WB_SCISSORS";
		model = "DRL_CraftMenu\model\wbp\Details\Sewing_scissors\Sewing_scissors.p3d";
		inventorySlot[] = {"DRL_CraftPropsScissors"};
		heavyItem = 1;
		weight = 10000;
		itemSize[] = {2,2};
		itemBehaviour = 0;
		physLayer = "item_large";
		lootTag[] = {"Work"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 999999;
					healthLevels[] = {{1.01,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.01,{}}};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem_Light
				{
					soundSet = "pickUpBarrelLight_SoundSet";
					id = 796;
				};
				class pickUpItem
				{
					soundSet = "pickUpBarrel_SoundSet";
					id = 797;
				};
			};
		};
	};
	class DRL_Craft_Metalic_thimble: Inventory_Base
	{
		scope = 2;
		displayName = "#STR_WB_SPOOL";
		descriptionShort = "#STR_WB_SPOOL";
		model = "DRL_CraftMenu\model\wbp\Details\Metalic_thimble\Metalic_thimble.p3d";
		inventorySlot[] = {"DRL_CraftPropsThimble"};
		heavyItem = 1;
		weight = 10000;
		itemSize[] = {2,2};
		itemBehaviour = 0;
		physLayer = "item_large";
		lootTag[] = {"Work"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 999999;
					healthLevels[] = {{1.01,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.01,{}}};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem_Light
				{
					soundSet = "pickUpBarrelLight_SoundSet";
					id = 796;
				};
				class pickUpItem
				{
					soundSet = "pickUpBarrel_SoundSet";
					id = 797;
				};
			};
		};
	};
	class DRL_Craft_ScrewDriver: Inventory_Base
	{
		scope = 2;
		displayName = "$STR_CfgVehicles_Screwdriver0";
		descriptionShort = "$STR_CfgVehicles_Screwdriver1";
		model = "\dz\gear\tools\Screwdriver.p3d";
		debug_ItemCategory = 2;
		animClass = "Knife";
		repairableWithKits[] = {4};
		repairCosts[] = {5};
		rotationFlags = 17;
		suicideAnim = "onehanded";
		RestrainUnlockType = 1;
		weight = 150;
		itemSize[] = {1,3};
		fragility = 0.008;
		openItemSpillRange[] = {20,40};
		action = "GestureMeleeBayonetStab";
		lockSoundSet = "wrench_loop_SoundSet";
		compatibleLocks[] = {1};
		soundImpactType = "metal";
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 100;
					healthLevels[] =
					{
						{1,{"DZ\gear\tools\data\screwdriver.rvmat"}},
						{0.7,{"DZ\gear\tools\data\screwdriver.rvmat"}},
						{0.5,{"DZ\gear\tools\data\screwdriver_damage.rvmat"}},
						{0.3,{"DZ\gear\tools\data\screwdriver_damage.rvmat"}},
						{0,{"DZ\gear\tools\data\screwdriver_destruct.rvmat"}}
					};
				};
			};
		};
		class MeleeModes
		{
			class Default
			{
				ammo = "MeleeSharpLight_1";
				range = 1.1;
			};
			class Heavy
			{
				ammo = "MeleeSharpHeavy_1";
				range = 1.1;
			};
			class Sprint
			{
				ammo = "MeleeSharpHeavy_1";
				range = 3.3;
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class animalSkinning_in
				{
					soundSet = "animalSkinning_in_SoundSet";
					id = 516;
				};
				class animalSkinning
				{
					soundSet = "animalSkinning_SoundSet";
					id = 517;
				};
				class animalSkinning_out
				{
					soundSet = "animalSkinning_out_SoundSet";
					id = 518;
				};
				class mineScrewdriver_SoundSet
				{
					soundSet = "mineScrewdriver_SoundSet";
					id = 11161;
				};
			};
		};
	};
	class DRL_Craft_Lamp: Inventory_Base
	{
		scope = 2;
		displayName = "#STR_WB_LAMP";
		descriptionShort = "#STR_WB_LAMP";
		model = "DRL_CraftMenu\model\wbp\Details\Lamp\Lamp.p3d";
		soundImpactType = "plastic";
		inventorySlot[] = {"DRL_CraftattachLMP"};
		class EnergyManager
		{
			hasIcon = 1;
			energyUsagePerSecond = 0.14;
			cordTextureFile = "DZ\gear\camping\Data\plug_yellow_CO.paa";
			cordLength = 5;
			plugType = 2;
			updateInterval = 50;
		};
		class Reflectors
		{
			class Beam
			{
				color[] = {0.8,0.9,1,1};
				brightness = 20;
				radius = 60;
				angle = 140;
				angleInnerRatio = 0.25;
				dayLight = 1;
				position = "beamStart";
				direction = "beamTarget";
				hitpoint = "glass_unfolded";
				selection = "glass_unfolded";
			};
		};
		heavyItem = 1;
		weight = 10000;
		itemSize[] = {2,2};
		itemBehaviour = 0;
		physLayer = "item_large";
		lootTag[] = {"Work"};
		class MeleeModes
		{
			class Default
			{
				ammo = "MeleeFistLight";
				range = 1;
			};
			class Heavy
			{
				ammo = "MeleeFistHeavy";
				range = 1;
			};
			class Sprint
			{
				ammo = "MeleeFistHeavy";
				range = 2.8;
			};
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 999999;
					healthLevels[] = {{1.01,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.01,{}}};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class Attack_flashlight_Light1
				{
					soundSet = "Char_Attack_pipemetal_light1_SoundSet";
					id = 806;
				};
				class Attack_flashlight_Light2
				{
					soundSet = "Char_Attack_pipemetal_light2_SoundSet";
					id = 807;
				};
				class Attack_flashlight_Heavy1
				{
					soundSet = "Char_Attack_pipemetal_heavy1_SoundSet";
					id = 808;
				};
				class Attack_flashlight_Heavy2
				{
					soundSet = "Char_Attack_pipemetal_heavy2_SoundSet";
					id = 809;
				};
			};
		};
	};
	class DRL_Craft_Table: Inventory_Base
	{
		scope = 2;
		displayName = "#STR_WB_WORKTOP";
		descriptionShort = "#STR_WB_WORKTOP";
		model = "DRL_CraftMenu\model\wbp\Details\Table\Table.p3d";
		inventorySlot[] = {"DRL_CraftattachP"};
		attachments[] = {"DRL_CraftattachWM"};
		heavyItem = 1;
		weight = 10000;
		itemSize[] = {2,2};
		itemBehaviour = 0;
		physLayer = "item_large";
		lootTag[] = {"Work"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 999999;
					healthLevels[] = {{1.01,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.01,{}}};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem_Light
				{
					soundSet = "pickUpBarrelLight_SoundSet";
					id = 796;
				};
				class pickUpItem
				{
					soundSet = "pickUpBarrel_SoundSet";
					id = 797;
				};
			};
		};
	};
	class DRL_Craft_Sewing_Table: Inventory_Base
	{
		scope = 2;
		displayName = "#STR_WB_SEWING_TABLE";
		descriptionShort = "#STR_WB_SEWING_TABLE";
		model = "DRL_CraftMenu\model\wbp\Details\Sewing_Table\Sewing_Table.p3d";
		inventorySlot[] = {"DRL_CraftattachSEWTAB"};
		attachments[] = {"DRL_CraftattachSEW"};
		heavyItem = 1;
		weight = 10000;
		itemSize[] = {2,2};
		itemBehaviour = 0;
		physLayer = "item_large";
		lootTag[] = {"Work"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 999999;
					healthLevels[] = {{1.01,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.01,{}}};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem_Light
				{
					soundSet = "pickUpBarrelLight_SoundSet";
					id = 796;
				};
				class pickUpItem
				{
					soundSet = "pickUpBarrel_SoundSet";
					id = 797;
				};
			};
		};
	};
	class DRL_Craft_Tools: Inventory_Base
	{
		scope = 2;
		displayName = "#STR_WB_TOOLS";
		descriptionShort = "#STR_WB_TOOLS";
		model = "DRL_CraftMenu\model\wbp\Details\Tools\Tools.p3d";
		inventorySlot[] = {"DRL_CraftattachT"};
		heavyItem = 1;
		weight = 10000;
		itemSize[] = {2,2};
		itemBehaviour = 0;
		physLayer = "item_large";
		lootTag[] = {"Work"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 999999;
					healthLevels[] = {{1.01,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.01,{}}};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem_Light
				{
					soundSet = "pickUpBarrelLight_SoundSet";
					id = 796;
				};
				class pickUpItem
				{
					soundSet = "pickUpBarrel_SoundSet";
					id = 797;
				};
			};
		};
	};
	class DRL_Craft_DrillSmal: Inventory_Base
	{
		scope = 2;
		displayName = "#STR_WB_DRILL";
		descriptionShort = "#STR_WB_DRILL";
		model = "DRL_CraftMenu\model\wbp\Details\Drill\Drill.p3d";
		inventorySlot[] = {"DRL_CraftattachDrill"};
		heavyItem = 1;
		weight = 10000;
		itemSize[] = {2,2};
		itemBehaviour = 0;
		physLayer = "item_large";
		lootTag[] = {"Work"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 999999;
					healthLevels[] = {{1.01,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.01,{}}};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem_Light
				{
					soundSet = "pickUpBarrelLight_SoundSet";
					id = 796;
				};
				class pickUpItem
				{
					soundSet = "pickUpBarrel_SoundSet";
					id = 797;
				};
			};
		};
	};
	class DRL_Craft_Paints: Inventory_Base
	{
		scope = 2;
		displayName = "#STR_WB_PAINTS";
		descriptionShort = "#STR_WB_PAINTS";
		model = "DRL_CraftMenu\model\wbp\Details\Paints\Paints.p3d";
		inventorySlot[] = {"DRL_CraftattachPaints"};
		heavyItem = 1;
		weight = 10000;
		itemSize[] = {2,2};
		itemBehaviour = 0;
		physLayer = "item_large";
		lootTag[] = {"Work"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 999999;
					healthLevels[] = {{1.01,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.01,{}}};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem_Light
				{
					soundSet = "pickUpBarrelLight_SoundSet";
					id = 796;
				};
				class pickUpItem
				{
					soundSet = "pickUpBarrel_SoundSet";
					id = 797;
				};
			};
		};
	};
	class DRL_Craft_Anvil: Inventory_Base
	{
		scope = 2;
		displayName = "#STR_WB_ANVIL";
		descriptionShort = "#STR_WB_ANVIL";
		model = "DRL_CraftMenu\model\wbp\Details\Anvil\Anvil.p3d";
		inventorySlot[] = {"DRL_Craftattach2"};
		heavyItem = 1;
		weight = 10000;
		itemSize[] = {2,2};
		itemBehaviour = 0;
		physLayer = "item_large";
		lootTag[] = {"Work"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 999999;
					healthLevels[] = {{1.01,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.01,{}}};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem_Light
				{
					soundSet = "pickUpBarrelLight_SoundSet";
					id = 796;
				};
				class pickUpItem
				{
					soundSet = "pickUpBarrel_SoundSet";
					id = 797;
				};
			};
		};
	};
	class DRL_Craft_Box: Inventory_Base
	{
		scope = 2;
		displayName = "#STR_WB_BOX";
		descriptionShort = "#STR_WB_BOX";
		model = "DRL_CraftMenu\model\wbp\Details\Box\Box.p3d";
		inventorySlot[] = {"DRL_CraftattachBX"};
		heavyItem = 1;
		weight = 10000;
		itemSize[] = {12,12};
		itemBehaviour = 0;
		physLayer = "item_large";
		lootTag[] = {"Work"};
		class Cargo
		{
			itemsCargoSize[] = {10,10};
			openable = 0;
			allowOwnedCargoManipulation = 1;
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 999999;
					healthLevels[] = {{1.01,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.01,{}}};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem_Light
				{
					soundSet = "pickUpBarrelLight_SoundSet";
					id = 796;
				};
				class pickUpItem
				{
					soundSet = "pickUpBarrel_SoundSet";
					id = 797;
				};
			};
		};
	};
	class DRL_Craft_Grinder: Inventory_Base
	{
		scope = 2;
		displayName = "#STR_WB_GRINDER";
		descriptionShort = "#STR_WB_GRINDER";
		model = "DZ\structures\Furniture\Various\grinder.p3d";
		inventorySlot[] = {"DRL_Craftattach"};
		heavyItem = 1;
		weight = 10000;
		itemSize[] = {2,2};
		itemBehaviour = 0;
		physLayer = "item_large";
		lootTag[] = {"Work"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 999999;
					healthLevels[] = {{1.01,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.01,{}}};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem_Light
				{
					soundSet = "pickUpBarrelLight_SoundSet";
					id = 796;
				};
				class pickUpItem
				{
					soundSet = "pickUpBarrel_SoundSet";
					id = 797;
				};
			};
		};
	};
	class DRL_Craft_Vise: Inventory_Base
	{
		scope = 2;
		displayName = "#STR_WB_VISE";
		descriptionShort = "#STR_WB_VISE";
		model = "DRL_CraftMenu\model\wbp\Details\Vice\Vice.p3d";
		inventorySlot[] = {"DRL_Craftattach1"};
		heavyItem = 1;
		weight = 10000;
		itemSize[] = {2,2};
		itemBehaviour = 0;
		physLayer = "item_large";
		lootTag[] = {"Work"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 999999;
					healthLevels[] = {{1.01,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.01,{}}};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem_Light
				{
					soundSet = "pickUpBarrelLight_SoundSet";
					id = 796;
				};
				class pickUpItem
				{
					soundSet = "pickUpBarrel_SoundSet";
					id = 797;
				};
			};
		};
	};
	class DRL_Craft_Anvil2: Inventory_Base
	{
		scope = 2;
		displayName = "Наковальня";
		descriptionShort = "Наковальня";
		model = "DZ\structures\Furniture\Various\Anvil.p3d";
		inventorySlot[] = {"DRL_Craftattach2"};
		heavyItem = 1;
		weight = 10000;
		itemSize[] = {2,2};
		itemBehaviour = 0;
		physLayer = "item_large";
		lootTag[] = {"Work"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 999999;
					healthLevels[] = {{1.01,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.01,{}}};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem_Light
				{
					soundSet = "pickUpBarrelLight_SoundSet";
					id = 796;
				};
				class pickUpItem
				{
					soundSet = "pickUpBarrel_SoundSet";
					id = 797;
				};
			};
		};
	};
	class DRL_Craft_Drill2: Inventory_Base
	{
		scope = 2;
		displayName = "#STR_WB_DRILL";
		descriptionShort = "#STR_WB_DRILL";
		model = "DZ\structures\Furniture\Various\Drill.p3d";
		inventorySlot[] = {"DRL_Craftattach3"};
		heavyItem = 1;
		weight = 10000;
		itemSize[] = {2,2};
		itemBehaviour = 0;
		physLayer = "item_large";
		lootTag[] = {"Work"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 999999;
					healthLevels[] = {{1.01,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.01,{}}};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem_Light
				{
					soundSet = "pickUpBarrelLight_SoundSet";
					id = 796;
				};
				class pickUpItem
				{
					soundSet = "pickUpBarrel_SoundSet";
					id = 797;
				};
			};
		};
	};
	class DRL_Craft_Micro: Inventory_Base
	{
		scope = 2;
		displayName = "#STR_WB_MICROSCOPE";
		descriptionShort = "#STR_WB_MICROSCOPE";
		model = "DZ\structures\Furniture\School_equipment\lab_microscope.p3d";
		inventorySlot[] = {"DRL_Craftattach4"};
		heavyItem = 1;
		weight = 10000;
		itemSize[] = {2,2};
		itemBehaviour = 0;
		physLayer = "item_large";
		lootTag[] = {"Work"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 999999;
					healthLevels[] = {{1.01,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.01,{}}};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem_Light
				{
					soundSet = "pickUpBarrelLight_SoundSet";
					id = 796;
				};
				class pickUpItem
				{
					soundSet = "pickUpBarrel_SoundSet";
					id = 797;
				};
			};
		};
	};
	class DRL_Craft_lab_bunsen: Inventory_Base
	{
		scope = 2;
		displayName = "#STR_WB_TRIPOD";
		descriptionShort = "#STR_WB_TRIPOD";
		model = "DZ\structures\Furniture\School_equipment\lab_bunsen.p3d";
		inventorySlot[] = {"DRL_Craftattach5"};
		heavyItem = 1;
		weight = 10000;
		itemSize[] = {2,2};
		itemBehaviour = 0;
		physLayer = "item_large";
		lootTag[] = {"Work"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 999999;
					healthLevels[] = {{1.01,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.01,{}}};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem_Light
				{
					soundSet = "pickUpBarrelLight_SoundSet";
					id = 796;
				};
				class pickUpItem
				{
					soundSet = "pickUpBarrel_SoundSet";
					id = 797;
				};
			};
		};
	};
	class DRL_BaseKit: Inventory_Base
	{
		scope = 0;
		displayName = "#STR_Craft_Kit";
		isMeleeWeapon = 0;
		weight = 10000;
		absorbency = 0;
		rotationFlags = 2;
		itemSize[] = {2,2};
		lootCategory = "Materials";
		reversed = 0;
		itemBehaviour = 2;
		model = "DRL_CraftMenu\kit\kit_wbc.p3d";
	};
	class DRL_Craft_Kit: DRL_BaseKit
	{
		scope = 2;
		displayName = "#STR_Craft_Kit";
		descriptionShort = "";
		model = "DRL_CraftMenu\kit\kit_wbc.p3d";
		rotationFlags = 17;
		weight = 980;
		itemSize[] = {1,1};
		itemsCargoSize[] = {0,0};
		canBeDigged = 1;
		isMeleeWeapon = 1;
		allowOwnedCargoManipulation = 1;
		randomQuantity = 2;
		repairableWithKits[] = {8};
		repairCosts[] = {25};
		class Cargo
		{
			itemsCargoSize[] = {0,0};
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 750;
					healthLevels[] =
					{
						{1,{"DRL_CraftMenu\kit\Metal_Armor.rvmat"}},
						{0.7,{"DRL_CraftMenu\kit\Metal_Armor.rvmat"}},
						{0.5,{"DRL_CraftMenu\kit\Metal_Armor_damage.rvmat"}},
						{0.3,{"DRL_CraftMenu\kit\Metal_Armor_damage.rvmat"}},
						{0,{"DRL_CraftMenu\kit\Metal_Armor_destruct.rvmat"}}
					};
				};
			};
		};
		soundImpactType = "plastic";
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem
				{
					soundSet = "smallprotectorcase_pickup_SoundSet";
					id = 797;
				};
				class drop
				{
					soundset = "smallprotectorcase_drop_SoundSet";
					id = 898;
				};
			};
		};
	};
	class DRL_Craft_Kit1: DRL_BaseKit
	{
		scope = 2;
		displayName = "#STR_Craft_Kit";
		descriptionShort = "";
		model = "DRL_CraftMenu\kit\kit_wbc.p3d";
		rotationFlags = 17;
		weight = 980;
		itemSize[] = {1,1};
		itemsCargoSize[] = {0,0};
		canBeDigged = 1;
		isMeleeWeapon = 1;
		allowOwnedCargoManipulation = 1;
		randomQuantity = 2;
		repairableWithKits[] = {8};
		repairCosts[] = {25};
		class Cargo
		{
			itemsCargoSize[] = {0,0};
		};
	};
	class DRL_Craft_Kit2: DRL_BaseKit
	{
		scope = 2;
		displayName = "#STR_Craft_Kit";
		descriptionShort = "";
		model = "DRL_CraftMenu\kit\kit_wbc.p3d";
		rotationFlags = 17;
		weight = 980;
		itemSize[] = {1,1};
		itemsCargoSize[] = {0,0};
		canBeDigged = 1;
		isMeleeWeapon = 1;
		allowOwnedCargoManipulation = 1;
		randomQuantity = 2;
		repairableWithKits[] = {8};
		repairCosts[] = {25};
		class Cargo
		{
			itemsCargoSize[] = {0,0};
		};
	};
	class DRL_Craft_Kit3: DRL_BaseKit
	{
		scope = 2;
		displayName = "#STR_Craft_Kit";
		descriptionShort = "";
		model = "DRL_CraftMenu\kit\kit_wbc.p3d";
		rotationFlags = 17;
		weight = 980;
		itemSize[] = {1,1};
		itemsCargoSize[] = {0,0};
		canBeDigged = 1;
		isMeleeWeapon = 1;
		allowOwnedCargoManipulation = 1;
		randomQuantity = 2;
		repairableWithKits[] = {8};
		repairCosts[] = {25};
		class Cargo
		{
			itemsCargoSize[] = {0,0};
		};
	};
	class DRL_Craft_Kit4: DRL_BaseKit
	{
		scope = 2;
		displayName = "#STR_Craft_Kit";
		descriptionShort = "";
		model = "DRL_CraftMenu\kit\kit_wbc.p3d";
		rotationFlags = 17;
		weight = 980;
		itemSize[] = {1,1};
		itemsCargoSize[] = {0,0};
		canBeDigged = 1;
		isMeleeWeapon = 1;
		allowOwnedCargoManipulation = 1;
		randomQuantity = 2;
		repairableWithKits[] = {8};
		repairCosts[] = {25};
		class Cargo
		{
			itemsCargoSize[] = {0,0};
		};
	};
	class DRL_Craft_Kit5: DRL_BaseKit
	{
		scope = 2;
		displayName = "#STR_Craft_Kit";
		descriptionShort = "";
		model = "DRL_CraftMenu\kit\kit_wbc.p3d";
		rotationFlags = 17;
		weight = 980;
		itemSize[] = {1,1};
		itemsCargoSize[] = {0,0};
		canBeDigged = 1;
		isMeleeWeapon = 1;
		allowOwnedCargoManipulation = 1;
		randomQuantity = 2;
		repairableWithKits[] = {8};
		repairCosts[] = {25};
		class Cargo
		{
			itemsCargoSize[] = {0,0};
		};
	};
	class DRL_Craft_Kit6: DRL_BaseKit
	{
		scope = 2;
		displayName = "#STR_Craft_Kit";
		descriptionShort = "";
		model = "DRL_CraftMenu\kit\kit_wbc.p3d";
		rotationFlags = 17;
		weight = 980;
		itemSize[] = {1,1};
		itemsCargoSize[] = {0,0};
		canBeDigged = 1;
		isMeleeWeapon = 1;
		allowOwnedCargoManipulation = 1;
		randomQuantity = 2;
		repairableWithKits[] = {8};
		repairCosts[] = {25};
		class Cargo
		{
			itemsCargoSize[] = {0,0};
		};
	};
	class DRL_Craft_Kit7: DRL_BaseKit
	{
		scope = 2;
		displayName = "#STR_Craft_Kit";
		descriptionShort = "";
		model = "DRL_CraftMenu\kit\kit_wbc.p3d";
		rotationFlags = 17;
		weight = 980;
		itemSize[] = {1,1};
		itemsCargoSize[] = {0,0};
		canBeDigged = 1;
		isMeleeWeapon = 1;
		allowOwnedCargoManipulation = 1;
		randomQuantity = 2;
		repairableWithKits[] = {8};
		repairCosts[] = {25};
		class Cargo
		{
			itemsCargoSize[] = {0,0};
		};
	};
	class DRL_Craft_Kit8: DRL_BaseKit
	{
		scope = 2;
		displayName = "#STR_Craft_Kit";
		descriptionShort = "";
		model = "DRL_CraftMenu\kit\kit_wbc.p3d";
		rotationFlags = 17;
		weight = 980;
		itemSize[] = {1,1};
		itemsCargoSize[] = {0,0};
		canBeDigged = 1;
		isMeleeWeapon = 1;
		allowOwnedCargoManipulation = 1;
		randomQuantity = 2;
		repairableWithKits[] = {8};
		repairCosts[] = {25};
		class Cargo
		{
			itemsCargoSize[] = {0,0};
		};
	};
	class DRL_Craft_Kit9: DRL_BaseKit
	{
		scope = 2;
		displayName = "#STR_Craft_WB_Player";
		descriptionShort = "";
		model = "DRL_CraftMenu\kit\kit_wbc.p3d";
		rotationFlags = 17;
		weight = 980;
		itemSize[] = {1,1};
		itemsCargoSize[] = {0,0};
		canBeDigged = 1;
		isMeleeWeapon = 1;
		allowOwnedCargoManipulation = 1;
		randomQuantity = 2;
		repairableWithKits[] = {8};
		repairCosts[] = {25};
		class Cargo
		{
			itemsCargoSize[] = {0,0};
		};
	};
	class DRL_Craft_Garden_Kit: DRL_BaseKit
	{
		scope = 2;
		displayName = "#STR_Craft_Garden_Kit";
		descriptionShort = "";
		model = "DRL_CraftMenu\kit\kit_wbc.p3d";
		rotationFlags = 17;
		weight = 980;
		itemSize[] = {1,1};
		itemsCargoSize[] = {0,0};
		canBeDigged = 1;
		isMeleeWeapon = 1;
		allowOwnedCargoManipulation = 1;
		randomQuantity = 2;
		repairableWithKits[] = {8};
		repairCosts[] = {25};
		class Cargo
		{
			itemsCargoSize[] = {0,0};
		};
	};
	class DRL_Craft_Cook_Kit: DRL_BaseKit
	{
		scope = 2;
		displayName = "#STR_Craft_Cook_Kit";
		descriptionShort = "";
		model = "DRL_CraftMenu\kit\kit_wbc.p3d";
		rotationFlags = 17;
		weight = 980;
		itemSize[] = {1,1};
		itemsCargoSize[] = {0,0};
		canBeDigged = 1;
		isMeleeWeapon = 1;
		allowOwnedCargoManipulation = 1;
		randomQuantity = 2;
		repairableWithKits[] = {8};
		repairCosts[] = {25};
		class Cargo
		{
			itemsCargoSize[] = {0,0};
		};
	};
	class DRL_Craft_Build_Kit: DRL_BaseKit
	{
		scope = 2;
		displayName = "#STR_Craft_Build_Kit";
		descriptionShort = "";
		model = "DRL_CraftMenu\kit\kit_wbc.p3d";
		rotationFlags = 17;
		weight = 980;
		itemSize[] = {1,1};
		itemsCargoSize[] = {0,0};
		canBeDigged = 1;
		isMeleeWeapon = 1;
		allowOwnedCargoManipulation = 1;
		randomQuantity = 2;
		repairableWithKits[] = {8};
		repairCosts[] = {25};
		class Cargo
		{
			itemsCargoSize[] = {0,0};
		};
	};
	class DRL_Craft_Workbench_Kit: DRL_BaseKit
	{
		scope = 2;
		displayName = "#STR_Craft_WB_Player";
		descriptionShort = "";
		model = "DRL_CraftMenu\kit\kit_wbc.p3d";
		rotationFlags = 17;
		weight = 980;
		itemSize[] = {1,1};
		itemsCargoSize[] = {0,0};
		canBeDigged = 1;
		isMeleeWeapon = 1;
		allowOwnedCargoManipulation = 1;
		randomQuantity = 2;
		repairableWithKits[] = {8};
		repairCosts[] = {25};
		class Cargo
		{
			itemsCargoSize[] = {0,0};
		};
	};
	class DRL_Craft_Research_Table_Kit: DRL_BaseKit
	{
		scope = 2;
		displayName = "#STR_DRL_RESEARCH_TABLE_KIT";
		descriptionShort = "#STR_DRL_RESEARCH_TABLE_DESC";
		model = "DRL_CraftMenu\kit\kit_wbc.p3d";
		rotationFlags = 17;
		weight = 980;
		itemSize[] = {2,2};
		itemsCargoSize[] = {0,0};
		canBeDigged = 1;
		isMeleeWeapon = 1;
		allowOwnedCargoManipulation = 1;
		randomQuantity = 2;
		repairableWithKits[] = {8};
		repairCosts[] = {25};
		class Cargo
		{
			itemsCargoSize[] = {0,0};
		};
	};
	class DRL_Craft_WB: Container_Base
	{
		scope = 2;
		displayName = "#STR_Verstak";
		descriptionShort = "#STR_Verstak_description";
		model = "\DRL_CraftMenu\model\drl_wbc.p3d";
		overrideDrawArea = 8;
		useEntityHierarchy = "true";
		carveNavmesh = 1;
		canBeDigged = 0;
		heavyItem = 1;
		weight = 20000;
		itemSize[] = {12,12};
		itemBehaviour = 0;
		repairKitType = 5;
		physLayer = "item_large";
		allowOwnedCargoManipulation = 1;
		lootTag[] = {"Work"};
		hiddenSelections[] = {"camoGround"};
		attachments[] =
		{
			"DRL_Craftattach",
			"DRL_Craftattach1",
			"DRL_Craftattach2",
			"DRL_Craftattach3",
			"DRL_Craftattach4",
			"DRL_Craftattach5"
		};
		class Cargo
		{
			itemsCargoSize[] = {10,10};
			openable = 0;
			allowOwnedCargoManipulation = 1;
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 999999;
					healthLevels[] = {{1.01,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.01,{}}};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem_Light
				{
					soundSet = "pickUpBarrelLight_SoundSet";
					id = 796;
				};
				class pickUpItem
				{
					soundSet = "pickUpBarrel_SoundSet";
					id = 797;
				};
			};
		};
	};
	class DRL_Craft_WB1: DRL_Craft_WB
	{
		scope = 2;
		displayName = "#STR_Verstak";
		descriptionShort = "#STR_Verstak_description";
		model = "\DRL_CraftMenu\model\drl_wbc.p3d";
		overrideDrawArea = 8;
		useEntityHierarchy = "true";
		carveNavmesh = 1;
		canBeDigged = 0;
		heavyItem = 1;
		weight = 20000;
		itemSize[] = {12,12};
		itemBehaviour = 0;
		repairKitType = 5;
		physLayer = "item_large";
		allowOwnedCargoManipulation = 1;
		lootTag[] = {"Work"};
		hiddenSelections[] = {"camoGround"};
		attachments[] =
		{
			"DRL_Craftattach",
			"DRL_Craftattach1",
			"DRL_Craftattach2",
			"DRL_Craftattach3",
			"DRL_Craftattach4",
			"DRL_Craftattach5"
		};
		class Cargo
		{
			itemsCargoSize[] = {0,0};
			openable = 0;
			allowOwnedCargoManipulation = 1;
		};
	};
	class DRL_Craft_WB2: DRL_Craft_WB
	{
		scope = 2;
		displayName = "#STR_Verstak";
		descriptionShort = "#STR_Verstak_description";
		model = "\DRL_CraftMenu\model\drl_wbc.p3d";
		overrideDrawArea = 8;
		useEntityHierarchy = "true";
		carveNavmesh = 1;
		canBeDigged = 0;
		heavyItem = 1;
		weight = 20000;
		itemSize[] = {12,12};
		itemBehaviour = 0;
		repairKitType = 5;
		physLayer = "item_large";
		allowOwnedCargoManipulation = 1;
		lootTag[] = {"Work"};
		hiddenSelections[] = {"camoGround"};
		attachments[] =
		{
			"DRL_Craftattach",
			"DRL_Craftattach1",
			"DRL_Craftattach2",
			"DRL_Craftattach3",
			"DRL_Craftattach4",
			"DRL_Craftattach5"
		};
		class Cargo
		{
			itemsCargoSize[] = {0,0};
			openable = 0;
			allowOwnedCargoManipulation = 1;
		};
	};
	class DRL_Craft_WB3: DRL_Craft_WB
	{
		scope = 2;
		displayName = "#STR_Verstak";
		descriptionShort = "#STR_Verstak_description";
		model = "\DRL_CraftMenu\model\drl_wbc.p3d";
		overrideDrawArea = 8;
		useEntityHierarchy = "true";
		carveNavmesh = 1;
		canBeDigged = 0;
		heavyItem = 1;
		weight = 20000;
		itemSize[] = {12,12};
		itemBehaviour = 0;
		repairKitType = 5;
		physLayer = "item_large";
		allowOwnedCargoManipulation = 1;
		lootTag[] = {"Work"};
		hiddenSelections[] = {"camoGround"};
		attachments[] =
		{
			"DRL_Craftattach",
			"DRL_Craftattach1",
			"DRL_Craftattach2",
			"DRL_Craftattach3",
			"DRL_Craftattach4",
			"DRL_Craftattach5"
		};
		class Cargo
		{
			itemsCargoSize[] = {0,0};
			openable = 0;
			allowOwnedCargoManipulation = 1;
		};
	};
	class DRL_Craft_WB4: DRL_Craft_WB
	{
		scope = 2;
		displayName = "#STR_Verstak";
		descriptionShort = "#STR_Verstak_description";
		model = "\DRL_CraftMenu\model\drl_wbc.p3d";
		overrideDrawArea = 8;
		useEntityHierarchy = "true";
		carveNavmesh = 1;
		canBeDigged = 0;
		heavyItem = 1;
		weight = 20000;
		itemSize[] = {12,12};
		itemBehaviour = 0;
		repairKitType = 5;
		physLayer = "item_large";
		allowOwnedCargoManipulation = 1;
		lootTag[] = {"Work"};
		hiddenSelections[] = {"camoGround"};
		attachments[] =
		{
			"DRL_Craftattach",
			"DRL_Craftattach1",
			"DRL_Craftattach2",
			"DRL_Craftattach3",
			"DRL_Craftattach4",
			"DRL_Craftattach5"
		};
		class Cargo
		{
			itemsCargoSize[] = {0,0};
			openable = 0;
			allowOwnedCargoManipulation = 1;
		};
	};
	class DRL_Craft_WB5: DRL_Craft_WB
	{
		scope = 2;
		displayName = "#STR_Verstak";
		descriptionShort = "#STR_Verstak_description";
		model = "\DRL_CraftMenu\model\drl_wbc.p3d";
		overrideDrawArea = 8;
		useEntityHierarchy = "true";
		carveNavmesh = 1;
		canBeDigged = 0;
		heavyItem = 1;
		weight = 20000;
		itemSize[] = {12,12};
		itemBehaviour = 0;
		repairKitType = 5;
		physLayer = "item_large";
		allowOwnedCargoManipulation = 1;
		lootTag[] = {"Work"};
		hiddenSelections[] = {"camoGround"};
		attachments[] =
		{
			"DRL_Craftattach",
			"DRL_Craftattach1",
			"DRL_Craftattach2",
			"DRL_Craftattach3",
			"DRL_Craftattach4",
			"DRL_Craftattach5"
		};
		class Cargo
		{
			itemsCargoSize[] = {0,0};
			openable = 0;
			allowOwnedCargoManipulation = 1;
		};
	};
	class DRL_Craft_WB6: DRL_Craft_WB
	{
		scope = 2;
		displayName = "#STR_Verstak";
		descriptionShort = "#STR_Verstak_description";
		model = "\DRL_CraftMenu\model\drl_wbc.p3d";
		overrideDrawArea = 8;
		useEntityHierarchy = "true";
		carveNavmesh = 1;
		canBeDigged = 0;
		heavyItem = 1;
		weight = 20000;
		itemSize[] = {12,12};
		itemBehaviour = 0;
		repairKitType = 5;
		physLayer = "item_large";
		allowOwnedCargoManipulation = 1;
		lootTag[] = {"Work"};
		hiddenSelections[] = {"camoGround"};
		attachments[] =
		{
			"DRL_Craftattach",
			"DRL_Craftattach1",
			"DRL_Craftattach2",
			"DRL_Craftattach3",
			"DRL_Craftattach4",
			"DRL_Craftattach5"
		};
		class Cargo
		{
			itemsCargoSize[] = {0,0};
			openable = 0;
			allowOwnedCargoManipulation = 1;
		};
	};
	class DRL_Craft_WB7: DRL_Craft_WB
	{
		scope = 2;
		displayName = "#STR_Verstak";
		descriptionShort = "#STR_Verstak_description";
		model = "\DRL_CraftMenu\model\drl_wbc.p3d";
		overrideDrawArea = 8;
		useEntityHierarchy = "true";
		carveNavmesh = 1;
		canBeDigged = 0;
		heavyItem = 1;
		weight = 20000;
		itemSize[] = {12,12};
		itemBehaviour = 0;
		repairKitType = 5;
		physLayer = "item_large";
		allowOwnedCargoManipulation = 1;
		lootTag[] = {"Work"};
		hiddenSelections[] = {"camoGround"};
		attachments[] =
		{
			"DRL_Craftattach",
			"DRL_Craftattach1",
			"DRL_Craftattach2",
			"DRL_Craftattach3",
			"DRL_Craftattach4",
			"DRL_Craftattach5"
		};
		class Cargo
		{
			itemsCargoSize[] = {0,0};
			openable = 0;
			allowOwnedCargoManipulation = 1;
		};
	};
	class DRL_Craft_WB8: DRL_Craft_WB
	{
		scope = 2;
		displayName = "#STR_Verstak";
		descriptionShort = "#STR_Verstak_description";
		model = "\DRL_CraftMenu\model\drl_wbc.p3d";
		overrideDrawArea = 8;
		useEntityHierarchy = "true";
		carveNavmesh = 1;
		canBeDigged = 0;
		heavyItem = 1;
		weight = 20000;
		itemSize[] = {12,12};
		itemBehaviour = 0;
		repairKitType = 5;
		physLayer = "item_large";
		allowOwnedCargoManipulation = 1;
		lootTag[] = {"Work"};
		hiddenSelections[] = {"camoGround"};
		attachments[] =
		{
			"DRL_Craftattach",
			"DRL_Craftattach1",
			"DRL_Craftattach2",
			"DRL_Craftattach3",
			"DRL_Craftattach4",
			"DRL_Craftattach5"
		};
		class Cargo
		{
			itemsCargoSize[] = {0,0};
			openable = 0;
			allowOwnedCargoManipulation = 1;
		};
	};
	class DRL_Craft_WB9: DRL_Craft_WB
	{
		scope = 2;
		displayName = "#STR_Verstak";
		descriptionShort = "#STR_Verstak_description";
		model = "\DRL_CraftMenu\model\drl_wbc.p3d";
		overrideDrawArea = 8;
		useEntityHierarchy = "true";
		carveNavmesh = 1;
		canBeDigged = 0;
		heavyItem = 1;
		weight = 20000;
		itemSize[] = {12,12};
		itemBehaviour = 0;
		repairKitType = 5;
		physLayer = "item_large";
		allowOwnedCargoManipulation = 1;
		lootTag[] = {"Work"};
		hiddenSelections[] = {"camoGround"};
		attachments[] =
		{
			"DRL_Craftattach",
			"DRL_Craftattach1",
			"DRL_Craftattach2",
			"DRL_Craftattach3",
			"DRL_Craftattach4",
			"DRL_Craftattach5"
		};
		class Cargo
		{
			itemsCargoSize[] = {0,0};
			openable = 0;
			allowOwnedCargoManipulation = 1;
		};
	};
	class DRL_Craft_WB_Static: DRL_Craft_WB
	{
		scope = 1;
		displayName = "#STR_Verstak_Static";
		descriptionShort = "#STR_Verstak_Static_desc";
		model = "\DRL_CraftMenu\model\drl_wbc.p3d";
		overrideDrawArea = 8;
		useEntityHierarchy = "true";
		carveNavmesh = 1;
		canBeDigged = 0;
		heavyItem = 1;
		weight = 100000;
		itemSize[] = {12,12};
		itemBehaviour = 0;
		repairKitType = 5;
		physLayer = "item_large";
		allowOwnedCargoManipulation = 0;
		lootTag[] = {"Work"};
		hiddenSelections[] = {"camoGround"};
		class Cargo
		{
			itemsCargoSize[] = {0,0};
			openable = 0;
			allowOwnedCargoManipulation = 0;
		};
	};
	class DRL_Craft_Garden: DRL_Craft_WB
	{
		scope = 2;
		displayName = "#STR_Garden";
		descriptionShort = "#STR_Garden_description";
		model = "\DRL_CraftMenu\model\wb1\Garden.p3d";
		overrideDrawArea = 8;
		useEntityHierarchy = "true";
		carveNavmesh = 1;
		canBeDigged = 0;
		heavyItem = 1;
		weight = 20000;
		itemSize[] = {12,12};
		itemBehaviour = 0;
		repairKitType = 5;
		physLayer = "item_large";
		allowOwnedCargoManipulation = 1;
		lootTag[] = {"Work"};
		hiddenSelections[] = {"camoGround"};
		attachments[] =
		{
			"DRL_Craftattach",
			"DRL_Craftattach1",
			"DRL_Craftattach2",
			"DRL_Craftattach3",
			"DRL_Craftattach4",
			"DRL_Craftattach5"
		};
		class Cargo
		{
			itemsCargoSize[] = {0,0};
			openable = 0;
			allowOwnedCargoManipulation = 1;
		};
	};
	class DRL_Craft_Cook: DRL_Craft_WB
	{
		scope = 2;
		displayName = "#STR_Cook";
		descriptionShort = "#STR_Cook_description";
		model = "\DRL_CraftMenu\model\wb2\cook2.p3d";
		overrideDrawArea = 8;
		useEntityHierarchy = "true";
		carveNavmesh = 1;
		canBeDigged = 0;
		heavyItem = 1;
		weight = 20000;
		itemSize[] = {12,12};
		itemBehaviour = 0;
		repairKitType = 5;
		physLayer = "item_large";
		allowOwnedCargoManipulation = 1;
		lootTag[] = {"Work"};
		hiddenSelections[] = {"camoGround"};
		attachments[] =
		{
			"DRL_Craftattach",
			"DRL_Craftattach1",
			"DRL_Craftattach2",
			"DRL_Craftattach3",
			"DRL_Craftattach4",
			"DRL_Craftattach5"
		};
		class Cargo
		{
			itemsCargoSize[] = {0,0};
			openable = 0;
			allowOwnedCargoManipulation = 1;
		};
	};
	class DRL_Craft_Build: DRL_Craft_WB
	{
		scope = 2;
		displayName = "#STR_Build";
		descriptionShort = "#STR_Build_description";
		model = "\DRL_CraftMenu\model\wb3\stanok1.p3d";
		overrideDrawArea = 8;
		useEntityHierarchy = "true";
		carveNavmesh = 1;
		canBeDigged = 0;
		heavyItem = 1;
		weight = 20000;
		itemSize[] = {12,12};
		itemBehaviour = 0;
		repairKitType = 5;
		physLayer = "item_large";
		allowOwnedCargoManipulation = 1;
		lootTag[] = {"Work"};
		hiddenSelections[] = {"camoGround"};
		attachments[] =
		{
			"DRL_Craftattach",
			"DRL_Craftattach1",
			"DRL_Craftattach2",
			"DRL_Craftattach3",
			"DRL_Craftattach4",
			"DRL_Craftattach5"
		};
		class Cargo
		{
			itemsCargoSize[] = {0,0};
			openable = 0;
			allowOwnedCargoManipulation = 1;
		};
	};
	class DRL_Craft_Workbench: DRL_Craft_WB
	{
		scope = 2;
		displayName = "#STR_Craft_WB_Player";
		descriptionShort = "#STR_Player_Verstak_description";
		model = "\DRL_CraftMenu\model\wbp\wbp.p3d";
		overrideDrawArea = 8;
		useEntityHierarchy = "true";
		carveNavmesh = 1;
		canBeDigged = 0;
		heavyItem = 1;
		weight = 20000;
		itemSize[] = {12,12};
		itemBehaviour = 0;
		repairKitType = 5;
		physLayer = "item_large";
		allowOwnedCargoManipulation = 1;
		lootTag[] = { "Work" };
		hiddenSelections[] = { "camoGround" };
		attachments[] =
		{
			"DRL_Craftattach1",
			"DRL_Craftattach2",
			"DRL_CraftattachDrill",
			"DRL_CraftattachMICRO",
			"DRL_Craftattach5",
			"DRL_CraftattachT",
			"DRL_CraftattachC",
			"DRL_CraftattachLMP",
			"DRL_CraftattachP",
			"DRL_CraftattachSEWTAB",
			"DRL_CraftattachBX",
			"DRL_CraftattachPaints",
			"DRL_CraftattachMP",
			"DRL_CraftattachNB"
		};
		class Cargo
		{
			itemsCargoSize[] = {10,10};
			openable = 0;
			allowOwnedCargoManipulation = 1;
		};
		class GUIInventoryAttachmentsProps
		{
			class Tools
			{
				name = "Tools";
				description = "#STR_WB_TOOLS для изготовления";
				icon = "cat_vehicle_engine";
				attachmentSlots[] =
				{
					"DRL_Craftattach1",
					"DRL_Craftattach2",
					"DRL_CraftattachDrill",
					"DRL_Craftattach5",
					"DRL_CraftattachT",
					"DRL_CraftattachLMP",
					"DRL_CraftattachPaints"
				};
			};
			class Engine
			{
				name = "Улучшения";
				description = "Раширение видов изготовления";
				icon = "cat_bb_watchtower_wall";
				attachmentSlots[] = { "DRL_CraftattachMICRO", "DRL_CraftattachMP", "DRL_CraftattachNB", "DRL_CraftattachC" };
			};
			class Body
			{
				name = "Расширения";
				description = "Дополнительные установки";
				icon = "cat_bb_watchtower_wall_3";
				attachmentSlots[] = { "DRL_CraftattachP", "DRL_CraftattachSEWTAB", };
			};
			class Storage
			{
				name = "Storage";
				description = "Расширение хранения";
				icon = "cat_common_cargo";
				attachmentSlots[] = { "DRL_CraftattachBX" };
			};
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 999999;





					healthLevels[] = { { 1.01, {} }, { 0.69999999, {} }, { 0.5, {} }, { 0.30000001, {} }, { 0.0099999998, {} } };
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem_Light
				{
					soundSet = "pickUpBarrelLight_SoundSet";
					id = 796;
				};
				class pickUpItem
				{
					soundSet = "pickUpBarrel_SoundSet";
					id = 797;
				};
			};
		};
	};
	class DRL_Craft_Welding_Machine: DRL_Craft_WB
	{
		scope = 2;
		displayName = "#STR_WB_WELDER";
		descriptionShort = "#STR_WB_WELDER";
		model = "\DRL_CraftMenu\model\wbp\Details\Welding_Machine\Welding_Machine.p3d";
		overrideDrawArea = 8;
		useEntityHierarchy = "true";
		carveNavmesh = 1;
		canBeDigged = 0;
		heavyItem = 1;
		weight = 20000;
		itemSize[] = {12,12};
		itemBehaviour = 0;
		repairKitType = 5;
		physLayer = "item_large";
		allowOwnedCargoManipulation = 1;
		lootTag[] = {"Work"};
		hiddenSelections[] = {"camoGround"};
		inventorySlot[] = {"DRL_CraftattachWM"};
		attachments[] = {"DRL_WM_Gas"};
		class Cargo
		{
			itemsCargoSize[] = {0,0};
			openable = 0;
			allowOwnedCargoManipulation = 1;
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 999999;
					healthLevels[] = {{1.01,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.01,{}}};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem_Light
				{
					soundSet = "pickUpBarrelLight_SoundSet";
					id = 796;
				};
				class pickUpItem
				{
					soundSet = "pickUpBarrel_SoundSet";
					id = 797;
				};
			};
		};
	};
	class DRL_Craft_Sewing: DRL_Craft_WB
	{
		scope = 2;
		displayName = "#STR_WB_SEWING_MACHINE";
		descriptionShort = "#STR_WB_SEWING_MACHINE";
		model = "DRL_CraftMenu\model\wbp\Details\Sewing\Sewing.p3d";
		inventorySlot[] = {"DRL_CraftattachSEW"};
		attachments[] = {"DRL_CraftPropsThimble","DRL_CraftPropsScissors"};
		weight = 10000;
		useEntityHierarchy = "true";
		carveNavmesh = 1;
		canBeDigged = 0;
		heavyItem = 1;
		itemSize[] = {12,12};
		itemBehaviour = 0;
		repairKitType = 5;
		physLayer = "item_large";
		allowOwnedCargoManipulation = 1;
		lootTag[] = {"Work"};
		class Cargo
		{
			itemsCargoSize[] = {0,0};
			openable = 0;
			allowOwnedCargoManipulation = 1;
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 999999;
					healthLevels[] = {{1.01,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.01,{}}};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem_Light
				{
					soundSet = "pickUpBarrelLight_SoundSet";
					id = 796;
				};
				class pickUpItem
				{
					soundSet = "pickUpBarrel_SoundSet";
					id = 797;
				};
			};
		};
	};
	class DRL_Craft_Microwave: DRL_Craft_WB
	{
		scope = 2;
		displayName = "#STR_WB_MICROWAVE";
		descriptionShort = "#STR_WB_MICROWAVE";
		model = "DRL_CraftMenu\model\wbp\Details\Microwave\Microwave.p3d";
		inventorySlot[] = {"DRL_CraftattachMICRO"};
		attachments[] = {};
		weight = 10000;
		useEntityHierarchy = "true";
		carveNavmesh = 1;
		canBeDigged = 0;
		heavyItem = 1;
		itemSize[] = {12,12};
		itemBehaviour = 0;
		repairKitType = 5;
		physLayer = "item_large";
		allowOwnedCargoManipulation = 1;
		lootTag[] = {"Work"};
		class Cargo
		{
			itemsCargoSize[] = {0,0};
			openable = 0;
			allowOwnedCargoManipulation = 1;
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 999999;
					healthLevels[] = {{1.01,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.01,{}}};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem_Light
				{
					soundSet = "pickUpBarrelLight_SoundSet";
					id = 796;
				};
				class pickUpItem
				{
					soundSet = "pickUpBarrel_SoundSet";
					id = 797;
				};
			};
		};
	};
	class DRL_Craft_NoteBook: DRL_Craft_WB
	{
		scope = 2;
		displayName = "#STR_WB_LAPTOP";
		descriptionShort = "#STR_WB_LAPTOP";
		model = "DRL_CraftMenu\model\wbp\Details\NoteBook\NoteBook.p3d";
		inventorySlot[] = {"DRL_CraftattachNB"};
		attachments[] = {};
		weight = 10000;
		useEntityHierarchy = "true";
		carveNavmesh = 1;
		canBeDigged = 0;
		heavyItem = 1;
		itemSize[] = {12,12};
		itemBehaviour = 0;
		repairKitType = 5;
		physLayer = "item_large";
		allowOwnedCargoManipulation = 1;
		lootTag[] = {"Work"};
		class Cargo
		{
			itemsCargoSize[] = {0,0};
			openable = 0;
			allowOwnedCargoManipulation = 1;
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 999999;
					healthLevels[] = {{1.01,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.01,{}}};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem_Light
				{
					soundSet = "pickUpBarrelLight_SoundSet";
					id = 796;
				};
				class pickUpItem
				{
					soundSet = "pickUpBarrel_SoundSet";
					id = 797;
				};
			};
		};
	};
	class DRL_Craft_Microscope: DRL_Craft_WB
	{
		scope = 2;
		displayName = "#STR_WB_MICROSCOPE";
		descriptionShort = "#STR_WB_MICROSCOPE";
		model = "DRL_CraftMenu\model\wbp\Details\Microscope\Microscope.p3d";
		inventorySlot[] = {"DRL_CraftattachMP"};
		attachments[] = {};
		weight = 10000;
		useEntityHierarchy = "true";
		carveNavmesh = 1;
		canBeDigged = 0;
		heavyItem = 1;
		itemSize[] = {12,12};
		itemBehaviour = 0;
		repairKitType = 5;
		physLayer = "item_large";
		allowOwnedCargoManipulation = 1;
		lootTag[] = {"Work"};
		class Cargo
		{
			itemsCargoSize[] = {0,0};
			openable = 0;
			allowOwnedCargoManipulation = 1;
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 999999;
					healthLevels[] = {{1.01,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.01,{}}};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem_Light
				{
					soundSet = "pickUpBarrelLight_SoundSet";
					id = 796;
				};
				class pickUpItem
				{
					soundSet = "pickUpBarrel_SoundSet";
					id = 797;
				};
			};
		};
	};
	class DRL_Craft_Circular: DRL_Craft_WB
	{
		scope = 2;
		displayName = "#STR_WB_CIRCULAR_SAW";
		descriptionShort = "#STR_WB_CIRCULAR_SAW";
		model = "DRL_CraftMenu\model\wbp\Details\Circular\Circular.p3d";
		inventorySlot[] = {"DRL_CraftattachC"};
		attachments[] = {"DRL_LMP_Bat"};
		overrideDrawArea = 8;
		useEntityHierarchy = "true";
		carveNavmesh = 1;
		canBeDigged = 0;
		heavyItem = 1;
		weight = 20000;
		itemSize[] = {12,12};
		itemBehaviour = 0;
		repairKitType = 5;
		physLayer = "item_large";
		allowOwnedCargoManipulation = 1;
		lootTag[] = {"Work"};
		hiddenSelections[] = {"camoGround"};
		class Cargo
		{
			itemsCargoSize[] = {0,0};
			openable = 0;
			allowOwnedCargoManipulation = 1;
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 999999;
					healthLevels[] = {{1.01,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.01,{}}};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem_Light
				{
					soundSet = "pickUpBarrelLight_SoundSet";
					id = 796;
				};
				class pickUpItem
				{
					soundSet = "pickUpBarrel_SoundSet";
					id = 797;
				};
			};
		};
	};
	class DRL_Craft_Research_Table: Container_Base
	{
		scope = 2;
		displayName = "#STR_DRL_RESEARCH_TABLE";
		descriptionShort = "#STR_DRL_RESEARCH_TABLE_DESC";
		model = "DRL_CraftMenu\model\Research_Table\Research_Table.p3d";
		overrideDrawArea = 8;
		useEntityHierarchy = "true";
		carveNavmesh = 1;
		canBeDigged = 0;
		heavyItem = 1;
		weight = 25000;
		itemSize[] = {12,12};
		itemBehaviour = 0;
		physLayer = "item_large";
		allowOwnedCargoManipulation = 1;
		lootTag[] = {"Work"};
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"DRL_CraftMenu\model\Research_Table\Research_Table_co.paa"};
		class Cargo
		{
			itemsCargoSize[] = {0,0};
			openable = 0;
			allowOwnedCargoManipulation = 1;
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 999999;
					healthLevels[] = {{1.01,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.01,{}}};
				};
			};
		};
	};
	class DRL_Craft_Research_Table_Static: DRL_Craft_Research_Table
	{
		scope = 1;
		displayName = "#STR_DRL_RESEARCH_TABLE_STATIC";
		descriptionShort = "#STR_DRL_RESEARCH_TABLE_STATIC_DESC";
		heavyItem = 1;
		weight = 100000;
		canBeDigged = 0;
		carveNavmesh = 1;
		physLayer = "item_large";
		allowOwnedCargoManipulation = 0;
		class Cargo
		{
			itemsCargoSize[] = {0,0};
			openable = 0;
			allowOwnedCargoManipulation = 0;
		};
	};
	class DRL_Craft_Truck_Lift_Kit: DRL_BaseKit
	{
		scope = 2;
		displayName = "#STR_DRL_TRUCK_LIFT_KIT";
		descriptionShort = "#STR_DRL_TRUCK_LIFT_KIT_DESC";
		model = "DRL_CraftMenu\kit\kit_wbc.p3d";
		rotationFlags = 17;
		weight = 15000;
		itemSize[] = {5,5};
		itemsCargoSize[] = {0,0};
		canBeDigged = 0;
		isMeleeWeapon = 0;
		allowOwnedCargoManipulation = 1;
		class Cargo
		{
			itemsCargoSize[] = {0,0};
		};
	};
	class DRL_Craft_Car_Jack: Inventory_Base
	{
		scope = 2;
		displayName = "#STR_WB_JACK";
		descriptionShort = "#STR_WB_JACK_DESC";
		model = "DRL_CraftMenu\model\Truck_Lift\Details\Car_jack\Car_jack.p3d";
		inventorySlot[] = {"DRL_CraftattachJack"};
		heavyItem = 1;
		weight = 10000;
		itemSize[] = {3,4};
		itemBehaviour = 0;
		physLayer = "item_large";
		lootTag[] = {"Work"};
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"DRL_CraftMenu\model\Truck_Lift\Details\Car_jack\Car_jack_co.paa"};
		hiddenSelectionsMaterials[] = {"DRL_CraftMenu\model\Truck_Lift\Details\Car_jack\Car_jack.rvmat"};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 999999;
					healthLevels[] = {{1.01,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.01,{}}};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem_Light
				{
					soundSet = "pickUpBarrelLight_SoundSet";
					id = 796;
				};
				class pickUpItem
				{
					soundSet = "pickUpBarrel_SoundSet";
					id = 797;
				};
				class drop
				{
					soundSet = "barrel_drop_SoundSet";
					id = 898;
				};
			};
		};
	};
	class DRL_Craft_Truck_Lift: DRL_Craft_WB
	{
		scope = 2;
		displayName = "#STR_DRL_TRUCK_LIFT";
		descriptionShort = "#STR_DRL_TRUCK_LIFT_DESC";
		model = "DRL_CraftMenu\model\Truck_Lift\Truck_Lift.p3d";
		overrideDrawArea = 12;
		useEntityHierarchy = "true";
		carveNavmesh = 1;
		canBeDigged = 0;
		heavyItem = 1;
		weight = 150000;
		itemSize[] = {15,15};
		itemBehaviour = 0;
		physLayer = "item_large";
		allowOwnedCargoManipulation = 1;
		lootTag[] = {"Work"};
		hiddenSelections[] = {"camo"};
		hiddenSelectionsTextures[] = {"DRL_CraftMenu\model\Truck_Lift\Truck_Lift_co.paa"};
		hiddenSelectionsMaterials[] = {"DRL_CraftMenu\model\Truck_Lift\Truck_Lift.rvmat"};
		attachments[] = {"DRL_CraftattachJack", "DRL_CraftattachBX"};
		class Cargo
		{
			itemsCargoSize[] = {10,10};
			openable = 0;
			allowOwnedCargoManipulation = 1;
		};
		class GUIInventoryAttachmentsProps
		{
			class Tools
			{
				name = "Tools";
				description = "#STR_WB_TOOLS";
				icon = "cat_vehicle_engine";
				attachmentSlots[] = {"DRL_CraftattachJack"};
			};
			class Storage
			{
				name = "Storage";
				description = "Расширение хранения";
				icon = "cat_common_cargo";
				attachmentSlots[] = {"DRL_CraftattachBX"};
			};
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 999999;
					healthLevels[] = {{1.01,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.01,{}}};
				};
			};
		};
	};
	class DRL_Craft_Truck_Lift_Static: DRL_Craft_Truck_Lift
	{
		scope = 1;
		displayName = "#STR_DRL_TRUCK_LIFT_STATIC";
		descriptionShort = "#STR_DRL_TRUCK_LIFT_STATIC_DESC";
		heavyItem = 1;
		weight = 200000;
		canBeDigged = 0;
		carveNavmesh = 1;
		physLayer = "item_large";
		allowOwnedCargoManipulation = 0;
		class Cargo
		{
			itemsCargoSize[] = {0,0};
			openable = 0;
			allowOwnedCargoManipulation = 0;
		};
	};
	class DRL_Craft_Covered_Car: HouseNoDestruct
	{
		scope = 1;
		model = "DRL_CraftMenu\model\Truck_Lift\Details\Covered_car\Covered_car.p3d";
	};
	class DRL_Craft_Firepit_Kit: DRL_BaseKit
	{
		scope = 2;
		displayName = "#STR_DRL_FIREPIT_KIT";
		descriptionShort = "#STR_DRL_FIREPIT_KIT_DESC";
		model = "DRL_CraftMenu\kit\kit_wbc.p3d";
		rotationFlags = 17;
		weight = 10000;
		itemSize[] = {5,5};
		itemsCargoSize[] = {0,0};
		canBeDigged = 0;
		isMeleeWeapon = 0;
		allowOwnedCargoManipulation = 1;
		class Cargo
		{
			itemsCargoSize[] = {0,0};
		};
	};
	class DRL_Craft_Firepit: DRL_Craft_WB
	{
		scope = 2;
		displayName = "#STR_WB_FIREPIT";
		descriptionShort = "#STR_WB_FIREPIT_DESC";
		model = "DRL_CraftMenu\model\Firepit\Firepit.p3d";
		overrideDrawArea = 12;
		carveNavmesh = 1;
		canBeDigged = 0;
		heavyItem = 1;
		weight = 50000;
		itemSize[] = {10,10};
		itemBehaviour = 0;
		physLayer = "item_large";
		attachments[] = {};
		allowOwnedCargoManipulation = 1;
		class Cargo
		{
			itemsCargoSize[] = {10,10};
			openable = 0;
			allowOwnedCargoManipulation = 1;
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints = 999999;
					healthLevels[] = {{1.01,{}},{0.7,{}},{0.5,{}},{0.3,{}},{0.01,{}}};
				};
			};
		};
	};
	class DRL_Craft_Firepit_Static: DRL_Craft_Firepit
	{
		scope = 1;
		displayName = "#STR_WB_FIREPIT_STATIC";
		descriptionShort = "#STR_WB_FIREPIT_STATIC_DESC";
		heavyItem = 1;
		weight = 100000;
		canBeDigged = 0;
		carveNavmesh = 1;
		physLayer = "item_large";
		allowOwnedCargoManipulation = 0;
		class Cargo
		{
			itemsCargoSize[] = {10,10};
			openable = 0;
			allowOwnedCargoManipulation = 0;
		};
	};
};
class CfgSoundShaders
{
	class drlcraft_SoundShaders
	{
		samples[] = {{"DRL_CraftMenu\sounds\craft_use",1}};
		volume = 2.0;
		range = 25;
		rangeCurve = "defaultAmpAttenuationCurve";
	};
	class drlcraft2_SoundShaders
	{
		samples[] = {{"DRL_CraftMenu\sounds\craft_use2",1}};
		volume = 2.0;
		range = 25;
		rangeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Avto_SoundShader
	{
		samples[] = {{"DRL_CraftMenu\sounds\craft_avto",1}};
		volume = 2.0;
		range = 25;
		rangeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Box_SoundShader
	{
		samples[] = {{"DRL_CraftMenu\sounds\craft_box",1},{"DRL_CraftMenu\sounds\craft_box2",1}};
		volume = 2.0;
		range = 25;
		rangeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Clothing_SoundShader
	{
		samples[] = {{"DRL_CraftMenu\sounds\craft_clothing",1}};
		volume = 2.0;
		range = 25;
		rangeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Sewing_SoundShader
	{
		samples[] = {{"DRL_CraftMenu\sounds\craft_shvejna",1}};
		volume = 2.0;
		range = 25;
		rangeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Scissors_SoundShader
	{
		samples[] = {{"DRL_CraftMenu\sounds\craft_nojnici",1}};
		volume = 2.0;
		range = 25;
		rangeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Food_SoundShader
	{
		samples[] = {{"DRL_CraftMenu\sounds\craft_food",1},{"DRL_CraftMenu\sounds\craft_food2",1}};
		volume = 2.0;
		range = 25;
		rangeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Grill_SoundShader
	{
		samples[] = {{"DRL_CraftMenu\sounds\craft_grile",1}};
		volume = 2.0;
		range = 25;
		rangeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_WaterMix_SoundShader
	{
		samples[] = {{"DRL_CraftMenu\sounds\craft_water_zames",1}};
		volume = 2.0;
		range = 25;
		rangeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Furniture_SoundShader
	{
		samples[] = {{"DRL_CraftMenu\sounds\craft_furneture",1}};
		volume = 2.0;
		range = 25;
		rangeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Glass_SoundShader
	{
		samples[] = {{"DRL_CraftMenu\sounds\craft_glass",1},{"DRL_CraftMenu\sounds\craft_glass2",1}};
		volume = 2.0;
		range = 25;
		rangeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Metal_SoundShader
	{
		samples[] = {{"DRL_CraftMenu\sounds\craft_metal",1},{"DRL_CraftMenu\sounds\craft_metal2",1}};
		volume = 2.0;
		range = 25;
		rangeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_MetalSawing_SoundShader
	{
		samples[] = {{"DRL_CraftMenu\sounds\craft_metal_sawing",1},{"DRL_CraftMenu\sounds\craft_metal_sawing2",1}};
		volume = 2.0;
		range = 25;
		rangeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Welding_SoundShader
	{
		samples[] = {{"DRL_CraftMenu\sounds\craft_welding",1}};
		volume = 2.0;
		range = 25;
		rangeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Forge_SoundShader
	{
		samples[] = {{"DRL_CraftMenu\sounds\craft_molotok_kovka",1}};
		volume = 2.0;
		range = 25;
		rangeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_HammerWood_SoundShader
	{
		samples[] = {{"DRL_CraftMenu\sounds\craft_molotok_wood",1}};
		volume = 2.0;
		range = 25;
		rangeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Saw_SoundShader
	{
		samples[] = {{"DRL_CraftMenu\sounds\craft_saw",1}};
		volume = 2.0;
		range = 25;
		rangeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Wood_SoundShader
	{
		samples[] = {{"DRL_CraftMenu\sounds\craft_wood",1}};
		volume = 2.0;
		range = 25;
		rangeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Paper_SoundShader
	{
		samples[] = {{"DRL_CraftMenu\sounds\craft_paper",1},{"DRL_CraftMenu\sounds\craft_paper2",1}};
		volume = 2.0;
		range = 25;
		rangeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Pencil_SoundShader
	{
		samples[] = {{"DRL_CraftMenu\sounds\craft_pencil",1}};
		volume = 2.0;
		range = 25;
		rangeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Rubber_SoundShader
	{
		samples[] = {{"DRL_CraftMenu\sounds\craft_rubber",1},{"DRL_CraftMenu\sounds\craft_rubber2",1}};
		volume = 2.0;
		range = 25;
		rangeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Repair_SoundShader
	{
		samples[] = {{"DRL_CraftMenu\sounds\inv_repair_kit_use_fast",1}};
		volume = 2.0;
		range = 25;
		rangeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Vybor_SoundShader
	{
		samples[] = {{"DRL_CraftMenu\sounds\craft_vybor",1}};
		volume = 2.0;
		range = 25;
		rangeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Generic1_SoundShader
	{
		samples[] = {{"DRL_CraftMenu\sounds\craft_zvuk",1}};
		volume = 2.0;
		range = 25;
		rangeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Generic2_SoundShader
	{
		samples[] = {{"DRL_CraftMenu\sounds\craft_zvuk2",1}};
		volume = 2.0;
		range = 25;
		rangeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Generic3_SoundShader
	{
		samples[] = {{"DRL_CraftMenu\sounds\craft_zvuk3",1}};
		volume = 2.0;
		range = 25;
		rangeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Generic4_SoundShader
	{
		samples[] = {{"DRL_CraftMenu\sounds\craft_zvuk4",1}};
		volume = 2.0;
		range = 25;
		rangeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Generic5_SoundShader
	{
		samples[] = {{"DRL_CraftMenu\sounds\craft_zvuk5",1}};
		volume = 2.0;
		range = 25;
		rangeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Generic6_SoundShader
	{
		samples[] = {{"DRL_CraftMenu\sounds\craft_zvuk6",1}};
		volume = 2.0;
		range = 25;
		rangeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Generic7_SoundShader
	{
		samples[] = {{"DRL_CraftMenu\sounds\craft_zvuk7",1}};
		volume = 2.0;
		range = 25;
		rangeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLLearning_Study_SoundShader
	{
		samples[] = {{"DRL_CraftMenu\sounds\studying",1}};
		volume = 1.5;
		range = 5;
		rangeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLLearning_Book_SoundShader
	{
		samples[] = {{"DRL_CraftMenu\sounds\book_viewing",1}};
		volume = 1.5;
		range = 5;
		rangeCurve = "defaultAmpAttenuationCurve";
	};
};
class CfgSoundSets
{
	class DRLCraft_SoundSet
	{
		soundShaders[] = {"drlcraft_SoundShaders"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 1;
		volumeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft2_SoundSet
	{
		soundShaders[] = {"drlcraft2_SoundShaders"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 1;
		volumeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Avto_SoundSet
	{
		soundShaders[] = {"DRLCraft_Avto_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 1;
		volumeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Box_SoundSet
	{
		soundShaders[] = {"DRLCraft_Box_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 1;
		volumeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Clothing_SoundSet
	{
		soundShaders[] = {"DRLCraft_Clothing_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 1;
		volumeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Sewing_SoundSet
	{
		soundShaders[] = {"DRLCraft_Sewing_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 1;
		volumeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Scissors_SoundSet
	{
		soundShaders[] = {"DRLCraft_Scissors_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 1;
		volumeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Food_SoundSet
	{
		soundShaders[] = {"DRLCraft_Food_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 1;
		volumeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Grill_SoundSet
	{
		soundShaders[] = {"DRLCraft_Grill_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 1;
		volumeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_WaterMix_SoundSet
	{
		soundShaders[] = {"DRLCraft_WaterMix_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 1;
		volumeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Furniture_SoundSet
	{
		soundShaders[] = {"DRLCraft_Furniture_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 1;
		volumeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Glass_SoundSet
	{
		soundShaders[] = {"DRLCraft_Glass_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 1;
		volumeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Metal_SoundSet
	{
		soundShaders[] = {"DRLCraft_Metal_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 1;
		volumeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_MetalSawing_SoundSet
	{
		soundShaders[] = {"DRLCraft_MetalSawing_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 1;
		volumeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Welding_SoundSet
	{
		soundShaders[] = {"DRLCraft_Welding_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 1;
		volumeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Forge_SoundSet
	{
		soundShaders[] = {"DRLCraft_Forge_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 1;
		volumeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_HammerWood_SoundSet
	{
		soundShaders[] = {"DRLCraft_HammerWood_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 1;
		volumeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Saw_SoundSet
	{
		soundShaders[] = {"DRLCraft_Saw_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 1;
		volumeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Wood_SoundSet
	{
		soundShaders[] = {"DRLCraft_Wood_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 1;
		volumeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Paper_SoundSet
	{
		soundShaders[] = {"DRLCraft_Paper_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 1;
		volumeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Pencil_SoundSet
	{
		soundShaders[] = {"DRLCraft_Pencil_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 1;
		volumeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Rubber_SoundSet
	{
		soundShaders[] = {"DRLCraft_Rubber_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 1;
		volumeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Repair_SoundSet
	{
		soundShaders[] = {"DRLCraft_Repair_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 1;
		volumeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Vybor_SoundSet
	{
		soundShaders[] = {"DRLCraft_Vybor_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 1;
		volumeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Generic1_SoundSet
	{
		soundShaders[] = {"DRLCraft_Generic1_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 1;
		volumeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Generic2_SoundSet
	{
		soundShaders[] = {"DRLCraft_Generic2_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 1;
		volumeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Generic3_SoundSet
	{
		soundShaders[] = {"DRLCraft_Generic3_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 1;
		volumeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Generic4_SoundSet
	{
		soundShaders[] = {"DRLCraft_Generic4_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 1;
		volumeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Generic5_SoundSet
	{
		soundShaders[] = {"DRLCraft_Generic5_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 1;
		volumeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Generic6_SoundSet
	{
		soundShaders[] = {"DRLCraft_Generic6_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 1;
		volumeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLCraft_Generic7_SoundSet
	{
		soundShaders[] = {"DRLCraft_Generic7_SoundShader"};
		volumeFactor = 1.0;
		spatial = 1;
		doppler = 0;
		loop = 1;
		volumeCurve = "defaultAmpAttenuationCurve";
	};
	class DRLLearning_Study_SoundSet
	{
		soundShaders[] = {"DRLLearning_Study_SoundShader"};
		volumeFactor = 1.0;
		spatial = 0;
		doppler = 0;
		loop = 0;
	};
	class DRLLearning_Book_SoundSet
	{
		soundShaders[] = {"DRLLearning_Book_SoundShader"};
		volumeFactor = 1.0;
		spatial = 0;
		doppler = 0;
		loop = 0;
	};
};
