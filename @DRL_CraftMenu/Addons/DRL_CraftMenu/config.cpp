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
		units[] = {"DRL_Craft_Sewing_scissors","DRL_Craft_Metalic_thimble","DRL_Craft_ScrewDriver","DRL_Craft_Lamp","DRL_Craft_Table","DRL_Craft_Sewing_Table","DRL_Craft_Tools","DRL_Craft_DrillSmal","DRL_Craft_Paints","DRL_Craft_Anvil","DRL_Craft_Box","DRL_Craft_Grinder","DRL_Craft_Vise","DRL_Craft_Anvil2","DRL_Craft_Drill2","DRL_Craft_Micro","DRL_Craft_lab_bunsen","DRL_Craft_Kit","DRL_Craft_Kit1","DRL_Craft_Kit2","DRL_Craft_Kit3","DRL_Craft_Kit4","DRL_Craft_Kit5","DRL_Craft_Kit6","DRL_Craft_Kit7","DRL_Craft_Kit8","DRL_Craft_Kit9","DRL_Craft_Garden_Kit","DRL_Craft_Cook_Kit","DRL_Craft_Build_Kit","DRL_Craft_Workbench_Kit","DRL_Craft_WB","DRL_Craft_WB1","DRL_Craft_WB2","DRL_Craft_WB3","DRL_Craft_WB4","DRL_Craft_WB5","DRL_Craft_WB6","DRL_Craft_WB7","DRL_Craft_WB8","DRL_Craft_WB9","DRL_Craft_Garden","DRL_Craft_Cook","DRL_Craft_Build","DRL_Craft_Workbench","DRL_Craft_Welding_Machine","DRL_Craft_Sewing","DRL_Craft_Microwave","DRL_Craft_NoteBook","DRL_Craft_Microscope","DRL_Craft_Circular"};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Gear_Tools","DZ_Gear_Camping"};
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
};
class CfgVehicles
{
	class WoodenCrate;
	class Container_Base;
	class Inventory_Base;
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
					healthLevels[] = {{1,{"DZ\gear\tools\data\screwdriver.rvmat"}},{0.7,{"DZ\gear\tools\data\screwdriver.rvmat"}},{0.5,{"DZ\gear\tools\data\screwdriver_damage.rvmat"}},{0.3,{"DZ\gear\tools\data\screwdriver_damage.rvmat"}},{0,{"DZ\gear\tools\data\screwdriver_destruct.rvmat"}}};
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
					healthLevels[] = {{1,{"DRL_CraftMenu\kit\Metal_Armor.rvmat"}},{0.7,{"DRL_CraftMenu\kit\Metal_Armor.rvmat"}},{0.5,{"DRL_CraftMenu\kit\Metal_Armor_damage.rvmat"}},{0.3,{"DRL_CraftMenu\kit\Metal_Armor_damage.rvmat"}},{0,{"DRL_CraftMenu\kit\Metal_Armor_destruct.rvmat"}}};
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
		attachments[] = {"DRL_Craftattach","DRL_Craftattach1","DRL_Craftattach2","DRL_Craftattach3","DRL_Craftattach4","DRL_Craftattach5"};
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
		attachments[] = {"DRL_Craftattach","DRL_Craftattach1","DRL_Craftattach2","DRL_Craftattach3","DRL_Craftattach4","DRL_Craftattach5"};
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
		attachments[] = {"DRL_Craftattach","DRL_Craftattach1","DRL_Craftattach2","DRL_Craftattach3","DRL_Craftattach4","DRL_Craftattach5"};
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
		attachments[] = {"DRL_Craftattach","DRL_Craftattach1","DRL_Craftattach2","DRL_Craftattach3","DRL_Craftattach4","DRL_Craftattach5"};
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
		attachments[] = {"DRL_Craftattach","DRL_Craftattach1","DRL_Craftattach2","DRL_Craftattach3","DRL_Craftattach4","DRL_Craftattach5"};
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
		attachments[] = {"DRL_Craftattach","DRL_Craftattach1","DRL_Craftattach2","DRL_Craftattach3","DRL_Craftattach4","DRL_Craftattach5"};
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
		attachments[] = {"DRL_Craftattach","DRL_Craftattach1","DRL_Craftattach2","DRL_Craftattach3","DRL_Craftattach4","DRL_Craftattach5"};
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
		attachments[] = {"DRL_Craftattach","DRL_Craftattach1","DRL_Craftattach2","DRL_Craftattach3","DRL_Craftattach4","DRL_Craftattach5"};
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
		attachments[] = {"DRL_Craftattach","DRL_Craftattach1","DRL_Craftattach2","DRL_Craftattach3","DRL_Craftattach4","DRL_Craftattach5"};
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
		attachments[] = {"DRL_Craftattach","DRL_Craftattach1","DRL_Craftattach2","DRL_Craftattach3","DRL_Craftattach4","DRL_Craftattach5"};
		class Cargo
		{
			itemsCargoSize[] = {0,0};
			openable = 0;
			allowOwnedCargoManipulation = 1;
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
		attachments[] = {"DRL_Craftattach","DRL_Craftattach1","DRL_Craftattach2","DRL_Craftattach3","DRL_Craftattach4","DRL_Craftattach5"};
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
		attachments[] = {"DRL_Craftattach","DRL_Craftattach1","DRL_Craftattach2","DRL_Craftattach3","DRL_Craftattach4","DRL_Craftattach5"};
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
		attachments[] = {"DRL_Craftattach","DRL_Craftattach1","DRL_Craftattach2","DRL_Craftattach3","DRL_Craftattach4","DRL_Craftattach5"};
		class Cargo
		{
			itemsCargoSize[] = {0,0};
			openable = 0;
			allowOwnedCargoManipulation = 1;
		};
	};
	class DRL_Craft_Workbench: DRL_Craft_WB
	{
		scope=2;
		displayName="#STR_Craft_WB_Player";
		descriptionShort="#STR_Player_Verstak_description";
		model="\DRL_CraftMenu\model\wbp\wbp.p3d";
		overrideDrawArea=8;
		useEntityHierarchy="true";
		carveNavmesh=1;
		canBeDigged=0;
		heavyItem=1;
		weight=20000;
		itemSize[]={12,12};
		itemBehaviour=0;
		repairKitType=5;
		physLayer="item_large";
		allowOwnedCargoManipulation=1;
		lootTag[]=
		{
			"Work"
		};
		hiddenSelections[]=
		{
			"camoGround"
		};
		attachments[]=
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
			itemsCargoSize[]={10,10};
			openable=0;
			allowOwnedCargoManipulation=1;
		};
		class GUIInventoryAttachmentsProps
		{
			class Tools
			{
				name="Tools";
				description="#STR_WB_TOOLS для изготовления";
				icon="cat_vehicle_engine";
				attachmentSlots[]=
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
				name="Улучшения";
				description="Раширение видов изготовления";
				icon="cat_bb_watchtower_wall";
				attachmentSlots[]=
				{
					"DRL_CraftattachMICRO",
					"DRL_CraftattachMP",
					"DRL_CraftattachNB",
					"DRL_CraftattachC"
				};
			};
			class Body
			{
				name="Расширения";
				description="Дополнительные установки";
				icon="cat_bb_watchtower_wall_3";
				attachmentSlots[]=
				{
					"DRL_CraftattachP",
					"DRL_CraftattachSEWTAB",
				};
			};
			class Storage
			{
				name="Storage";
				description="Расширение хранения";
				icon="cat_common_cargo";
				attachmentSlots[]=
				{
					"DRL_CraftattachBX"
				};
			};
		};
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=999999;
					healthLevels[]=
					{
						
						{
							1.01,
							{}
						},
						
						{
							0.69999999,
							{}
						},
						
						{
							0.5,
							{}
						},
						
						{
							0.30000001,
							{}
						},
						
						{
							0.0099999998,
							{}
						}
					};
				};
			};
		};
		class AnimEvents
		{
			class SoundWeapon
			{
				class pickUpItem_Light
				{
					soundSet="pickUpBarrelLight_SoundSet";
					id=796;
				};
				class pickUpItem
				{
					soundSet="pickUpBarrel_SoundSet";
					id=797;
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
};
class CfgSoundShaders
{
	class drlcraft_SoundShaders
	{
		samples[] = {{"DRL_CraftMenu\sounds\craft_use",1}};
		volume = 4.2;
		range = 5;
	};
	class drlcraft2_SoundShaders
	{
		samples[] = {{"DRL_CraftMenu\sounds\craft_use2",1}};
		volume = 4.2;
		range = 5;
	};
};
class CfgSoundSets
{
	class DRLCraft_SoundSet
	{
		soundShaders[] = {"drlcraft_SoundShaders"};
		volumeFactor = 1.5;
		spatial = 0;
		doppler = 0;
		loop = 1;
	};
	class DRLCraft2_SoundSet
	{
		soundShaders[] = {"drlcraft2_SoundShaders"};
		volumeFactor = 1.5;
		spatial = 0;
		doppler = 0;
		loop = 1;
	};
};
