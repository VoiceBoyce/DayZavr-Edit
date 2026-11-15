// Молнии вырезанной сердцевины в цвете «Древнего корня» (модели — tools/gen_ar_lightning.py), без патча неба.
class CfgPatches
{
	class VB_RiftSky
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data"};
	};
};

class CfgVehicles
{
	class HouseNoDestruct;
	class VB_RiftStrikeBolt: HouseNoDestruct
	{
		scope = 1;
		model = "\VB_RiftSky\data\vb_blesk1.p3d";
	};
	class VB_RiftStrikeBoltHeavy: HouseNoDestruct
	{
		scope = 1;
		model = "\VB_RiftSky\data\vb_blesk2.p3d";
	};
};
