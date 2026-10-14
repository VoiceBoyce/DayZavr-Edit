class CfgPatches
{
	class TGK_FSB_ALPHA
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Characters",
			"DZ_Scripts",
			"DZ_Weapons_Explosives",
			"DZ_Characters_Vests",
			"DZ_Characters_Headgear",
			"DZ_Characters_Gloves",
			"DZ_Characters_Backpacks",
			"DZ_Weapons_Melee_Blade",
			"DZ_Gear_Containers",
			"DZ_Gear_Consumables",
			"DZ_Gear_Tools",
			"DZ_Vehicles_Parts",
			"DZ_Gear_Cooking",
			"DZ_Gear_Camping",
			"DZ_Radio",
			"DZ_Gear_Medical"
		};
		skipWhenMissingDependencies=1;
	};
};
class CfgMods
{
	class TGK_FSB_Alpha
	{
		dir="TGK_FSB_Alpha";
		hideName=1;
		hidePicture=1;
		name="TGK_FSB_Alpha";
		author="https://discord.gg/tgkworkshop TWIZICSS";
		version="1.0";
		type="mod";
		dependencies[]=
		{
			"World",
			"Mission"
		};
		class defs
		{
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"TGK_FSB_Alpha/set/scripts/4_World"
				};
			};
			class missionScriptModule
			{
				value="";
				files[]=
				{
					"TGK_FSB_Alpha/set/scripts/5_Mission"
				};
			};
			class imageSets
			{
				files[]=
				{
					"TGK_FSB_Alpha/set/gui/imagesets/TGK_CustomIcons.imageset"
				};
			};
		};
	};
};
class CfgVehicles
{
	class Man;
	class SurvivorBase: Man
	{
		class DayZPlayer
		{
			class P3DAttachments
			{
				P3DProxies[]+=
				{
					"spine1",
					"righthand_dummy",
					"spine2",
					"head",
					"rightlegroll",
					"spine3",
					"lefthand",
					"LeftArm",
					"pelvis",
					"leftshoulder",
					"rightshoulder",
					"rightleg",
					"leftleg",
					"rightupleg",
					"leftupleg",
					"rightuplegroll",
					"lefttuplegroll",
					"LeftArmExtra",
					"leftwristextra"
				};
				BoneNames[]+=
				{
					"Spine1",
					"RightHand_Dummy",
					"Spine2",
					"Head",
					"RightLegRoll",
					"Spine3",
					"LeftHand",
					"LeftArm",
					"Pelvis",
					"LeftShoulder",
					"RightShoulder",
					"RightLeg",
					"LeftLeg",
					"RightUpLeg",
					"LeftUpLeg",
					"RightUpLegRoll",
					"LeftUpLegRoll",
					"LeftArmExtra",
					"leftwristextra"
				};
			};
		};
	};
};
