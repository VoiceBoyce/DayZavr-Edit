// Энергоячейки Эхо (идея пользователя 11-20): одна модель, кольцо на крышке — уровень (hiddenSelection «tier» из
// конфига класса), ядро — заряд (hiddenSelection «neon», красит клиент): пустая — серое, Эхо — красное, синтез —
// бирюзовое. Заряд = количество предмета (блок VB_Echo в конфиге, storage = "quantity"), меняют станция базы
// (зарядка / разрядка в сеть) и будущее снаряжение. Синтез бесконечный и не переливается (VB_Echo.CanTransfer).

class VB_EchoCell_Base extends Inventory_Base
{
	static const string TEX_EMPTY = "vb_echocore\\data\\er_neon_empty_co.paa";
	static const string TEX_ECHO = "vb_echocore\\data\\er_neon_co.paa";
	static const string TEX_FUSION = "vb_echocore\\data\\er_neon_fusion_co.paa";
	static const string MAT_EMPTY = "vb_echocore\\data\\er_neon_empty.rvmat";
	static const string MAT_IDLE = "vb_echocore\\data\\er_neon_idle.rvmat";

	override void EEInit()
	{
		super.EEInit();
		if (GetGame().IsDedicatedServer())
			return;
		VB_Paint();
		// ещё раз чуть позже: сразу после появления движок может наложить текстуры конфига поверх (как у бака)
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(VB_Paint, 300, false);
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		if (!GetGame().IsDedicatedServer())
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(VB_Paint);
	}

	// заряд изменился (станция, снаряжение, админ) — перекрасить
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		VB_Paint();
	}

	void VB_Paint()
	{
		int idx = GetHiddenSelectionIndex("neon");
		if (idx < 0)
			return;
		if (!VB_Echo.HasCharge(this))
		{
			SetObjectTexture(idx, TEX_EMPTY);
			SetObjectMaterial(idx, MAT_EMPTY);
			return;
		}
		if (VB_Echo.IsFusion(this))
			SetObjectTexture(idx, TEX_FUSION);
		else
			SetObjectTexture(idx, TEX_ECHO);
		SetObjectMaterial(idx, MAT_IDLE);
	}
};

class VB_EchoCell_Mk1 extends VB_EchoCell_Base {};
class VB_EchoCell_Mk2 extends VB_EchoCell_Base {};
class VB_EchoCell_Mk3 extends VB_EchoCell_Base {};
class VB_EchoCell_Mk4 extends VB_EchoCell_Base {};
class VB_EchoCell_Fusion extends VB_EchoCell_Base {};
