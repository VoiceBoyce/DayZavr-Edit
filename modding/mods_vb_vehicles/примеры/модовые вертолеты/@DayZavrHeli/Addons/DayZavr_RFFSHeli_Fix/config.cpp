class CfgMods 
{
	class DayZavr_RFFSHeli_Fix 
	{
		dir = "DayZavr_RFFSHeli_Fix";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "DayZavr_RFFSHeli_Fix";
		credits = "intenso";
		author = "intenso";
		authorID = "0";
		version = 1;
		extra = 0;
		type = "mod";
		dependencies[] = {"World","Game"};
	};
};

class CfgPatches 
{
	class DayZavr_RFFSHeli_Fix 
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = 
		{ 
			"RFFSHeli_Apache_Heli",
			"RFFSHeli_Apache_Desert_Heli",
			"RFFSHeli_AS350_Heli",
			"RFFSHeli_CH47_Heli",
			"RFFSHeli_EC135_Heli",
			"RFFSHeli_EC135_BlackCamo_Heli",
			"RFFSHeli_R22_Heli",
			"RFFSHeli_S76_Heli",
			"RFFSHeli_S76_WoodlandCamo_Heli"
		};
	};
};

class CfgVehicles 
{
	class RFFSHeli_base;

	class RFFSHeli_Apache: RFFSHeli_base
	{
		attachments[]=
		{
			"AviationBattery",
			"AviationIgniterPlug",
			"AviationHydraulicHoses",
			"AviationWiringHarness",
			"apache_doors_driver",
			"apache_doors_codriver",
			"NivaWheel_1_1",
			"NivaWheel_1_2",
			"NivaWheel_2_1",
			"NivaWheel_2_2",
			"Reflector_1_1"
		};
		class Cargo
		{
			itemsCargoSize[]={10,80};
			allowOwnedCargoManipulation=1;
			openable=0;
		};
		class GUIInventoryAttachmentsProps
		{
			class Engine
			{
				name="$STR_attachment_Engine0";
				description="";
				icon="cat_vehicle_engine";
				attachmentSlots[]=
				{
					"AviationBattery",
					"AviationIgniterPlug",
					"AviationHydraulicHoses",
					"AviationWiringHarness",
					"Reflector_1_1"
				};
			};
			class Body
			{
				name="$STR_attachment_Body0";
				description="";
				icon="cat_vehicle_body";
				attachmentSlots[]=
				{
					"apache_doors_driver",
					"apache_doors_codriver"
				};
			};
			class Storage
			{
				name="Storage";
				description="";
				attachmentSlots[]={};
				icon="woodcrate";
			};
			class Chassis
			{
				name="$STR_attachment_Chassis0";
				description="";
				icon="cat_vehicle_chassis";
				attachmentSlots[]=
				{
					"NivaWheel_1_1",
					"NivaWheel_1_2",
					"NivaWheel_2_1",
					"NivaWheel_2_2"
				};
			};
		};
	};	
	
	class RFFSHeli_Apache_Desert: RFFSHeli_Apache
	{
		scope=2;
		displayName="AH-46 Apache Longbow - Desert";
	};
	
	class RFFSHeli_AS350: RFFSHeli_base
	{
		attachments[]=
		{
			"AviationBattery",
			"AviationIgniterPlug",
			"AviationHydraulicHoses",
			"AviationWiringHarness",
			"AS350_doors_driver",
			"AS350_doors_codriver",
			"AS350_doors_cargo1",
			"AS350_doors_cargo2",
			"NivaWheel_1_1",
			"NivaWheel_1_2",
			"NivaWheel_2_1",
			"NivaWheel_2_2",
			"Reflector_1_1",
			"Reflector_2_1"
		};
		class GUIInventoryAttachmentsProps
		{
			class Engine
			{
				name="$STR_attachment_Engine0";
				description="";
				icon="cat_vehicle_engine";
				attachmentSlots[]=
				{
					"AviationBattery",
					"AviationIgniterPlug",
					"AviationHydraulicHoses",
					"AviationWiringHarness",
					"Reflector_1_1",
					"Reflector_2_1"
				};
			};
			class Body
			{
				name="$STR_attachment_Body0";
				description="";
				icon="cat_vehicle_body";
				attachmentSlots[]=
				{
					"AS350_doors_driver",
					"AS350_doors_codriver",
					"AS350_doors_cargo1",
					"AS350_doors_cargo2",
					"AS350_fuselage"
				};
			};
			class Storage
			{
				name="Storage";
				description="";
				attachmentSlots[]={};
				icon="woodcrate";
			};
			class Chassis
			{
				name="$STR_attachment_Chassis0";
				description="";
				icon="cat_vehicle_chassis";
				attachmentSlots[]=
				{
					"NivaWheel_1_1",
					"NivaWheel_1_2",
					"NivaWheel_2_1",
					"NivaWheel_2_2"
				};
			};
		};
	};
	
	class RFFSHeli_CH47: RFFSHeli_base
	{
		attachments[]=
		{
			"AviationBattery",
			"AviationIgniterPlug",
			"AviationHydraulicHoses",
			"AviationWiringHarness",
			"ch47_doors_cargo1",
			"ch47_doors_cargo2",
			"NivaWheel_1_1",
			"NivaWheel_1_2",
			"NivaWheel_2_1",
			"NivaWheel_2_2",
			"Reflector_1_1",
			"Reflector_2_1",
			"Truck_01_WoodenCrate1",
			"Truck_01_WoodenCrate2",
			"Truck_01_WoodenCrate3",
			"Truck_01_WoodenCrate4",
			"Truck_01_WoodenCrate5",
			"Truck_01_WoodenCrate6"
		};
		class GUIInventoryAttachmentsProps
		{
			class Engine
			{
				name="$STR_attachment_Engine0";
				description="";
				icon="cat_vehicle_engine";
				attachmentSlots[]=
				{
					"AviationBattery",
					"AviationIgniterPlug",
					"AviationHydraulicHoses",
					"AviationWiringHarness",
					"Reflector_1_1",
					"Reflector_2_1"
				};
			};
			class Body
			{
				name="$STR_attachment_Body0";
				description="";
				icon="cat_vehicle_body";
				attachmentSlots[]=
				{
					"ch47_doors_cargo1",
					"ch47_doors_cargo2"
				};
			};
			class Storage
			{
				name="Forward Storage";
				description="";
				attachmentSlots[]=
				{
					"Truck_01_WoodenCrate1",
					"Truck_01_WoodenCrate2",
					"Truck_01_WoodenCrate3",
					"Truck_01_WoodenCrate4",
					"Truck_01_WoodenCrate5",
					"Truck_01_WoodenCrate6"
				};
				icon="woodcrate";
			};
			class AftStorage
			{
				name="Aft Storage";
				description="";
				attachmentSlots[]={};
				icon="woodcrate";
			};
			class Chassis
			{
				name="$STR_attachment_Chassis0";
				description="";
				icon="cat_vehicle_chassis";
				attachmentSlots[]=
				{
					"NivaWheel_1_1",
					"NivaWheel_1_2",
					"NivaWheel_2_1",
					"NivaWheel_2_2"
				};
			};
		};
	};
	
	class RFFSHeli_EC135: RFFSHeli_base
	{
		attachments[]=
		{
			"AviationBattery",
			"AviationIgniterPlug",
			"AviationHydraulicHoses",
			"AviationWiringHarness",
			"ec135_doors_driver",
			"ec135_doors_codriver",
			"ec135_doors_cargo1",
			"ec135_doors_cargo2",
			"ec135_doors_cargo3",
			"ec135_doors_cargo4",
			"NivaWheel_1_1",
			"NivaWheel_1_2",
			"NivaWheel_2_1",
			"NivaWheel_2_2",
			"Reflector_1_1",
			"Reflector_2_1"
		};
		class GUIInventoryAttachmentsProps
		{
			class Engine
			{
				name="$STR_attachment_Engine0";
				description="";
				icon="cat_vehicle_engine";
				attachmentSlots[]=
				{
					"AviationBattery",
					"AviationIgniterPlug",
					"AviationHydraulicHoses",
					"AviationWiringHarness",
					"Reflector_1_1",
					"Reflector_2_1"
				};
			};
			class Body
			{
				name="$STR_attachment_Body0";
				description="";
				icon="cat_vehicle_body";
				attachmentSlots[]=
				{
					"ec135_doors_driver",
					"ec135_doors_codriver",
					"ec135_doors_cargo1",
					"ec135_doors_cargo2",
					"ec135_doors_cargo3",
					"ec135_doors_cargo4",
					"ec135_fuselage"
				};
			};
			class Storage
			{
				name="Storage";
				description="";
				attachmentSlots[]={};
				icon="woodcrate";
			};
			class Chassis
			{
				name="$STR_attachment_Chassis0";
				description="";
				icon="cat_vehicle_chassis";
				attachmentSlots[]=
				{
					"NivaWheel_1_1",
					"NivaWheel_1_2",
					"NivaWheel_2_1",
					"NivaWheel_2_2"
				};
			};
		};
	};
	
	class RFFSHeli_EC135_BlackCamo: RFFSHeli_EC135
	{
		scope=2;
		displayName="Eurocopter EC135 Helicopter - BlackCamo";
	};
	
	class RFFSHeli_R22: RFFSHeli_base
	{
		attachments[]=
		{
			"AviationBattery",
			"AviationIgniterPlug",
			"AviationHydraulicHoses",
			"AviationWiringHarness",
			"r22_doors_driver",
			"r22_doors_codriver",
			"NivaWheel_1_1",
			"NivaWheel_1_2",
			"NivaWheel_2_1",
			"NivaWheel_2_2",
			"Reflector_1_1"
		};
		class Cargo
		{
			itemsCargoSize[]={10,30};
			allowOwnedCargoManipulation=1;
			openable=0;
		};
		class GUIInventoryAttachmentsProps
		{
			class Engine
			{
				name="$STR_attachment_Engine0";
				description="";
				icon="cat_vehicle_engine";
				attachmentSlots[]=
				{
					"AviationBattery",
					"AviationIgniterPlug",
					"AviationHydraulicHoses",
					"AviationWiringHarness",
					"Reflector_1_1"
				};
			};
			class Body
			{
				name="$STR_attachment_Body0";
				description="";
				icon="cat_vehicle_body";
				attachmentSlots[]=
				{
					"r22_doors_driver",
					"r22_doors_codriver"
				};
			};
			class Storage
			{
				name="Storage";
				description="";
				attachmentSlots[]={};
				icon="woodcrate";
			};
			class Chassis
			{
				name="$STR_attachment_Chassis0";
				description="";
				icon="cat_vehicle_chassis";
				attachmentSlots[]=
				{
					"NivaWheel_1_1",
					"NivaWheel_1_2",
					"NivaWheel_2_1",
					"NivaWheel_2_2"
				};
			};
		};
	};
	
	class RFFSHeli_S76: RFFSHeli_base
	{
		attachments[]=
		{
			"AviationBattery",
			"AviationIgniterPlug",
			"AviationHydraulicHoses",
			"AviationWiringHarness",
			"S76_doors_driver",
			"S76_doors_codriver",
			"S76_doors_cargo1",
			"S76_doors_cargo2",
			"NivaWheel_1_1",
			"NivaWheel_1_2",
			"NivaWheel_2_1",
			"NivaWheel_2_2",
			"Reflector_1_1"
		};
		class Cargo
		{
			itemsCargoSize[]={10,85};
			allowOwnedCargoManipulation=1;
			openable=0;
		};
		class GUIInventoryAttachmentsProps
		{
			class Engine
			{
				name="$STR_attachment_Engine0";
				description="";
				icon="cat_vehicle_engine";
				attachmentSlots[]=
				{
					"AviationBattery",
					"AviationIgniterPlug",
					"AviationHydraulicHoses",
					"AviationWiringHarness",
					"Reflector_1_1"
				};
			};
			class Body
			{
				name="$STR_attachment_Body0";
				description="";
				icon="cat_vehicle_body";
				attachmentSlots[]=
				{
					"S76_doors_driver",
					"S76_doors_codriver",
					"S76_doors_cargo1",
					"S76_doors_cargo2"
				};
			};
			class Storage
			{
				name="Storage";
				description="";
				attachmentSlots[]={};
				icon="woodcrate";
			};
			class Chassis
			{
				name="$STR_attachment_Chassis0";
				description="";
				icon="cat_vehicle_chassis";
				attachmentSlots[]=
				{
					"NivaWheel_1_1",
					"NivaWheel_1_2",
					"NivaWheel_2_1",
					"NivaWheel_2_2"
				};
			};
		};
	};
	
	class RFFSHeli_S76_WoodlandCamo: RFFSHeli_S76
	{
		scope=2;
		attachments[]=
		{
			"AviationBattery",
			"AviationIgniterPlug",
			"AviationHydraulicHoses",
			"AviationWiringHarness",
			"S76_doors_driver",
			"S76_doors_codriver",
			"S76_doors_cargo1",
			"S76_doors_cargo2",
			"NivaWheel_1_1",
			"NivaWheel_1_2",
			"NivaWheel_2_1",
			"NivaWheel_2_2",
			"Reflector_1_1"
		};
		class Cargo
		{
			itemsCargoSize[]={10,85};
			allowOwnedCargoManipulation=1;
			openable=0;
		};
		class GUIInventoryAttachmentsProps
		{
			class Engine
			{
				name="$STR_attachment_Engine0";
				description="";
				icon="cat_vehicle_engine";
				attachmentSlots[]=
				{
					"AviationBattery",
					"AviationIgniterPlug",
					"AviationHydraulicHoses",
					"AviationWiringHarness",
					"Reflector_1_1"
				};
			};
			class Body
			{
				name="$STR_attachment_Body0";
				description="";
				icon="cat_vehicle_body";
				attachmentSlots[]=
				{
					"S76_doors_driver",
					"S76_doors_codriver",
					"S76_doors_cargo1",
					"S76_doors_cargo2"
				};
			};
			class Storage
			{
				name="Storage";
				description="";
				attachmentSlots[]={};
				icon="woodcrate";
			};
			class Chassis
			{
				name="$STR_attachment_Chassis0";
				description="";
				icon="cat_vehicle_chassis";
				attachmentSlots[]=
				{
					"NivaWheel_1_1",
					"NivaWheel_1_2",
					"NivaWheel_2_1",
					"NivaWheel_2_2"
				};
			};
		};
	};
	
};
