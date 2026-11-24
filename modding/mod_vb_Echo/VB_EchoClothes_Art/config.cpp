// VB_EchoClothes_Art — артефактам OOTS слоты баночек пояса Эхо (VB_EC_ArtJar1..6) и габарит модели
// vbArtBox[] = {центр x, y, z, наибольший размер} LOD0 — по нему скрипт пояса вписывает копию в баночку.
// Сгенерировано tools/ec_art_slots.py из config и моделей DD_Artifacts (@OTTS_Artifacts), руками не править.
// Без @OTTS_Artifacts этот PBO не грузить (requiredAddons DD_Artifacts) — на сервере 16 он есть всегда.
class CfgPatches
{
	class VB_EchoClothes_Art
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]={"DZ_Data","DD_Artifacts","VB_EchoClothes"};
	};
};
class CfgVehicles
{
	class Inventory_Base;
	class DD_Artifact_Angel: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0066,0.1077,-0.0483,0.1337};
	};
	class DD_Artifact_AngelFool: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0033,0.1167,-0.0333,0.1288};
	};
	class DD_Artifact_Babble: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0187,0.1303,-0.0328,0.1619};
	};
	class DD_Artifact_Battery: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={0.0011,0.1257,-0.0278,0.1391};
	};
	class DD_Artifact_BatteryBlue: DD_Artifact_Battery
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={0.0011,0.1257,-0.0278,0.1391};
	};
	class DD_Artifact_BloodOfStone: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0004,0.1256,-0.0261,0.1521};
	};
	class DD_Artifact_BloodStone_New: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0022,0.1176,-0.0336,0.0943};
	};
	class DD_Artifact_Braen: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0036,0.1207,-0.0332,0.1522};
	};
	class DD_Artifact_BreakingMeet: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0026,0.1414,-0.0268,0.1698};
	};
	class DD_Artifact_Burn: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0019,0.1085,-0.0330,0.0887};
	};
	class DD_Artifact_Colobok: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0013,0.1150,-0.0338,0.1062};
	};
	class DD_Artifact_Compass: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0053,0.1151,-0.0335,0.1153};
	};
	class DD_Artifact_Crustal: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={0.0017,0.1300,-0.0334,0.1279};
	};
	class DD_Artifact_CrustalThorn: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0025,0.1191,-0.0348,0.1109};
	};
	class DD_Artifact_DarkSoul: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0012,0.1104,-0.0320,0.1054};
	};
	class DD_Artifact_DemonEye: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0012,0.1104,-0.0320,0.1054};
	};
	class DD_Artifact_DevelHert: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0043,0.1178,-0.0342,0.1267};
	};
	class DD_Artifact_Drop: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={0.0002,0.1101,-0.0351,0.0828};
	};
	class DD_Artifact_Dummy: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0036,0.1453,-0.0328,0.1609};
	};
	class DD_Artifact_Eye: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={0.0002,0.1229,-0.0302,0.1307};
	};
	class DD_Artifact_FireBall: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0019,0.1107,-0.0339,0.0999};
	};
	class DD_Artifact_Flame: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0018,0.1076,-0.0343,0.0959};
	};
	class DD_Artifact_Flash: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0017,0.1042,-0.0336,0.0815};
	};
	class DD_Artifact_Flower: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0151,0.0253,0.0165,0.2463};
	};
	class DD_Artifact_GoldenFish: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0076,0.1266,-0.0370,0.1664};
	};
	class DD_Artifact_Gravy: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0066,0.1607,-0.0328,0.1669};
	};
	class DD_Artifact_Insulator: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={0.0893,0.0120,-0.0002,0.2572};
	};
	class DD_Artifact_JellyFish: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={0.0025,0.1214,-0.0334,0.1291};
	};
	class DD_Artifact_Klyaksa: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0055,0.1034,-0.0345,0.1048};
	};
	class DD_Artifact_Lens: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0038,0.1115,-0.0335,0.1013};
	};
	class DD_Artifact_Light: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0009,0.0988,-0.0329,0.0783};
	};
	class DD_Artifact_Mikrob: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0011,0.1058,-0.0336,0.0821};
	};
	class DD_Artifact_Mohovik: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0024,0.1065,-0.0336,0.1204};
	};
	class DD_Artifact_MomisBeads: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={0.0001,0.1342,-0.0339,0.1450};
	};
	class DD_Artifact_Monolit: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0033,0.1073,-0.0344,0.0980};
	};
	class DD_Artifact_MoonLight: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0015,0.1124,-0.0361,0.1032};
	};
	class DD_Artifact_NanoCell: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0031,0.1160,-0.0327,0.1261};
	};
	class DD_Artifact_NightStar: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0049,0.1199,-0.0331,0.1288};
	};
	class DD_Artifact_Oasis: Inventory_Base
	{
		inventorySlot[]={"VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0002,0.0722,0.0324,0.3011};
	};
	class DD_Artifact_Plasma: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0012,0.1081,-0.0334,0.0408};
	};
	class DD_Artifact_Proleller: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={0.0011,0.1328,-0.0314,0.1439};
	};
	class DD_Artifact_SeaUrchin: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0052,0.1209,-0.0326,0.1288};
	};
	class DD_Artifact_ShineOfTheForest: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0025,0.1095,-0.0329,0.1002};
	};
	class DD_Artifact_SnowFlake: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0031,0.1164,-0.0320,0.1145};
	};
	class DD_Artifact_Soul: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0017,0.1093,-0.0335,0.1044};
	};
	class DD_Artifact_Sparkler: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0026,0.1296,-0.0310,0.1283};
	};
	class DD_Artifact_Spring: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0009,0.1434,-0.0317,0.1516};
	};
	class DD_Artifact_SteeringWheel: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.1424,0.0397,0.0279,0.3235};
	};
	class DD_Artifact_StoneFlower: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0274,0.0503,0.0089,0.1910};
	};
	class DD_Artifact_Svetlyak: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0015,0.1061,-0.0337,0.0842};
	};
	class DD_Artifact_Thorn: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0010,0.1233,-0.0317,0.1290};
	};
	class DD_Artifact_Twist: Inventory_Base
	{
		inventorySlot[]={"Artefacts","Artefacts1","Artefacts2","VB_EC_ArtJar1","VB_EC_ArtJar2","VB_EC_ArtJar3","VB_EC_ArtJar4","VB_EC_ArtJar5","VB_EC_ArtJar6"};
		vbArtBox[]={-0.0043,0.1286,-0.0317,0.1283};
	};
};
