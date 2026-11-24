// VB_EchoMaterials — материалы крафта брони экзоскелета Эхо и машин (16 сервер). Список из 14 предметов утверждён
// пользователем 11-24: 7 появляются в мире (types/события), 7 крафтятся на верстаке DRL. Своей логики нет — только
// предметы. Модели и текстуры — tools/em_models.py, tools/em_tex.py (собирать pbo.ps1 -Binarize).
// Отдельный мод @VB_EchoMaterials, ни от чего не зависит (решение пользователя 11-24: по раскладке ТЗ_VB_Echo ядро —
// только энергия). Новые материалы следующих уровней добавлять сюда же.

class CfgPatches
{
	class VB_EchoMaterials
	{
		units[]={"VB_EM_ArmorPlate","VB_EM_CarbonProfile","VB_EM_ServoMotor","VB_EM_HydraulicCylinder","VB_EM_EchoShard",
			"VB_EM_ShockAbsorber","VB_EM_ECU","VB_EM_DamperPad","VB_EM_ExoController","VB_EM_EchoConduit",
			"VB_EM_ResonanceCoil","VB_EM_PowerFrame","VB_EM_BodyPanel","VB_EM_ArmorGlass"};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]={"DZ_Data","DZ_Scripts"};
	};
};
class CfgVehicles
{
	class Inventory_Base;
	class VB_EM_Base: Inventory_Base
	{
		scope=0;
		weight=500;
		itemSize[]={2,2};
		rotationFlags=17;
		class DamageSystem
		{
			class GlobalHealth
			{
				class Health
				{
					hitpoints=100;
				};
			};
		};
	};
	// ---------------- в мире (спавн)
	class VB_EM_ArmorPlate: VB_EM_Base
	{
		scope=2;
		displayName="$STR_VB_EM_ArmorPlate";
		descriptionShort="$STR_VB_EM_ArmorPlate_disc";
		model="VB_EchoMaterials\data\em_armorplate.p3d";
		weight=1800;
		itemSize[]={3,4};
	};
	class VB_EM_CarbonProfile: VB_EM_Base
	{
		scope=2;
		displayName="$STR_VB_EM_CarbonProfile";
		descriptionShort="$STR_VB_EM_CarbonProfile_disc";
		model="VB_EchoMaterials\data\em_carbon.p3d";
		weight=600;
		itemSize[]={5,1};
	};
	class VB_EM_ServoMotor: VB_EM_Base
	{
		scope=2;
		displayName="$STR_VB_EM_ServoMotor";
		descriptionShort="$STR_VB_EM_ServoMotor_disc";
		model="VB_EchoMaterials\data\em_servo.p3d";
		weight=900;
		itemSize[]={2,2};
	};
	class VB_EM_HydraulicCylinder: VB_EM_Base
	{
		scope=2;
		displayName="$STR_VB_EM_HydraulicCylinder";
		descriptionShort="$STR_VB_EM_HydraulicCylinder_disc";
		model="VB_EchoMaterials\data\em_hydraulic.p3d";
		weight=2500;
		itemSize[]={4,1};
	};
	class VB_EM_EchoShard: VB_EM_Base
	{
		scope=2;
		displayName="$STR_VB_EM_EchoShard";
		descriptionShort="$STR_VB_EM_EchoShard_disc";
		model="VB_EchoMaterials\data\em_shard.p3d";
		weight=0;
		weightPerQuantityUnit=150;	// у Inventory_Base по умолчанию 1 г на единицу (E123)
		itemSize[]={1,1};
		canBeSplit=1;
		varQuantityInit=1;
		varQuantityMin=0;
		varQuantityMax=10;
		varQuantityDestroyOnMin=1;
		varStackMax=10;
	};
	class VB_EM_ShockAbsorber: VB_EM_Base
	{
		scope=2;
		displayName="$STR_VB_EM_ShockAbsorber";
		descriptionShort="$STR_VB_EM_ShockAbsorber_disc";
		model="VB_EchoMaterials\data\em_shock.p3d";
		weight=3000;
		itemSize[]={4,2};
	};
	class VB_EM_ECU: VB_EM_Base
	{
		scope=2;
		displayName="$STR_VB_EM_ECU";
		descriptionShort="$STR_VB_EM_ECU_disc";
		model="VB_EchoMaterials\data\em_ecu.p3d";
		weight=800;
		itemSize[]={2,2};
	};
	// ---------------- крафт (верстак DRL)
	class VB_EM_DamperPad: VB_EM_Base
	{
		scope=2;
		displayName="$STR_VB_EM_DamperPad";
		descriptionShort="$STR_VB_EM_DamperPad_disc";
		model="VB_EchoMaterials\data\em_damper.p3d";
		weight=300;
		itemSize[]={2,2};
	};
	class VB_EM_ExoController: VB_EM_Base
	{
		scope=2;
		displayName="$STR_VB_EM_ExoController";
		descriptionShort="$STR_VB_EM_ExoController_disc";
		model="VB_EchoMaterials\data\em_exoctrl.p3d";
		weight=600;
		itemSize[]={2,1};
	};
	class VB_EM_EchoConduit: VB_EM_Base
	{
		scope=2;
		displayName="$STR_VB_EM_EchoConduit";
		descriptionShort="$STR_VB_EM_EchoConduit_disc";
		model="VB_EchoMaterials\data\em_conduit.p3d";
		weight=400;
		itemSize[]={2,2};
	};
	class VB_EM_ResonanceCoil: VB_EM_Base
	{
		scope=2;
		displayName="$STR_VB_EM_ResonanceCoil";
		descriptionShort="$STR_VB_EM_ResonanceCoil_disc";
		model="VB_EchoMaterials\data\em_coil.p3d";
		weight=1200;
		itemSize[]={2,2};
	};
	class VB_EM_PowerFrame: VB_EM_Base
	{
		scope=2;
		displayName="$STR_VB_EM_PowerFrame";
		descriptionShort="$STR_VB_EM_PowerFrame_disc";
		model="VB_EchoMaterials\data\em_frame.p3d";
		weight=8000;
		itemSize[]={6,3};
	};
	class VB_EM_BodyPanel: VB_EM_Base
	{
		scope=2;
		displayName="$STR_VB_EM_BodyPanel";
		descriptionShort="$STR_VB_EM_BodyPanel_disc";
		model="VB_EchoMaterials\data\em_panel.p3d";
		weight=6000;
		itemSize[]={7,4};
	};
	class VB_EM_ArmorGlass: VB_EM_Base
	{
		scope=2;
		displayName="$STR_VB_EM_ArmorGlass";
		descriptionShort="$STR_VB_EM_ArmorGlass_disc";
		model="VB_EchoMaterials\data\em_glass.p3d";
		weight=4000;
		itemSize[]={4,3};
	};
};
