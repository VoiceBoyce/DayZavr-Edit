// Детали Echo Runner: колесо (разбитое меняется на VB_EchoRunner_Wheel_Destroyed), двери, капот,
// навесные Изнанки (бак, трубопроводы) — свечение ими управляет VB_EchoRunner.

class VB_EchoRunner_Wheel extends CarWheel
{
	override void EEKilled(Object killer)
	{
		string newWheel = "";
		switch (GetType())
		{
		case "VB_EchoRunner_Wheel":
		case "VB_EchoRunner_Wheel_Ruined":
			newWheel = "VB_EchoRunner_Wheel_Destroyed";
			break;
		}
		if (newWheel != "")
		{
			ReplaceWheelLambda lambda = new ReplaceWheelLambda(this, newWheel, NULL);
			lambda.SetTransferParams(true, true, true);
			GetInventory().ReplaceItemWithNew(InventoryMode.SERVER, lambda);
		}
	}

	override int GetMeleeTargetType()
	{
		return EMeleeTargetType.NONALIGNABLE;
	}
}

class VB_EchoRunner_Wheel_Destroyed extends CarWheel {}

class VB_EchoRunner_Door_Driver extends CarDoor {}
class VB_EchoRunner_Door_CoDriver extends CarDoor {}
class VB_EchoRunner_Hood extends CarDoor {}

// антиграв-модуль (этап 8): стоит в слоте колеса; вращение, наклоны и свечение задаёт машина
class VB_EchoRunner_GravModule extends CarWheel
{
	override void EEInit()
	{
		super.EEInit();
		if (!GetGame().IsDedicatedServer() && !VB_EchoRunner.Cast(GetHierarchyParent()))
			VB_EchoRunner.VB_SetSelMaterial(this, "neon", VB_EchoRunner.VB_NEON_IDLE);
	}

	override int GetMeleeTargetType()
	{
		return EMeleeTargetType.NONALIGNABLE;
	}
}

class VB_EchoRunner_RiftPart extends Inventory_Base
{
	override void EEInit()
	{
		super.EEInit();
		// не на машине — только тлеет (на машине материал выставляет VB_EchoRunner)
		if (!GetGame().IsDedicatedServer() && !VB_EchoRunner.Cast(GetHierarchyParent()))
			VB_EchoRunner.VB_SetSelMaterial(this, "neon", VB_EchoRunner.VB_NEON_IDLE);
	}
}
