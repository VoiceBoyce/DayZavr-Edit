// Аккумуляторы семейства VB_Echo: обычный автомобильный аккумулятор (слот CarBattery, любая машина), плюс
// носитель энергии ядра (блок VB_Echo в конфиге, заряд = энергия EnergyManager). Неон (выделение «neon»):
// горит, пока есть заряд; у синтеза горит всегда, и сервер держит его заряд полным.
// Машины семейства (Echo Runner) держат заглушенную машину на подушке только с таким аккумулятором (VB_Echo.IsCarrier).

class VB_EchoBattery extends CarBattery
{
	static const string VB_NEON_ON = "vb_echovehicleparts\\data\\er_neon_soft.rvmat";
	static const string VB_NEON_OFF = "vb_echovehicleparts\\data\\er_neon_off.rvmat";
	static const int VB_REFILL_MS = 10000;

	override void EEInit()
	{
		super.EEInit();
		if (!GetGame().IsDedicatedServer())
			VB_UpdateNeon();
		if (GetGame().IsServer() && VB_Echo.IsInfinite(this))
		{
			VB_Refill();
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(VB_Refill, VB_REFILL_MS, true);
		}
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(VB_Refill);
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		VB_UpdateNeon();
	}

	override void EEHealthLevelChanged(int oldLevel, int newLevel, string zone)
	{
		super.EEHealthLevelChanged(oldLevel, newLevel, zone);
		if (!GetGame().IsDedicatedServer())
			VB_UpdateNeon();
	}

	void VB_UpdateNeon()
	{
		int idx = GetHiddenSelectionIndex("neon");
		if (idx < 0)
			return;
		if (VB_Echo.HasCharge(this))
			SetObjectMaterial(idx, VB_NEON_ON);
		else
			SetObjectMaterial(idx, VB_NEON_OFF);
	}

	// синтез: заряд всегда полный (мотор, фары и стоянка на подушке его тратят — сервер возвращает)
	void VB_Refill()
	{
		if (IsRuined() || !GetCompEM())
			return;
		if (GetCompEM().GetEnergy() < GetCompEM().GetEnergyMax())
		{
			GetCompEM().SetEnergy(GetCompEM().GetEnergyMax());
			ConvertEnergyToQuantity();
		}
	}
};

class VB_EchoBatteryFusion extends VB_EchoBattery
{
};
