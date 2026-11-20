// VB_EchoCore — энергия семейства VB_Echo (ТЗ: modding/mod_vb_Echo/ТЗ_VB_Echo.md).
// Носитель — любой предмет с блоком VB_Echo в своём конфиге (описание блока — в config.cpp ядра). Здесь — чтение
// блока и общие действия с зарядом, одинаковые для баков (заряд = количество предмета) и аккумуляторов
// (заряд = энергия EnergyManager): узнать, списать, добавить. Списывать и добавлять — только на сервере.

class VB_EchoKind
{
	static const int NONE = -1;
	static const int ECHO = 0;		// Эхо — красная, расходуется
	static const int FUSION = 1;	// холодный синтез — бирюзовая, бесконечная (донат)
};

class VB_EchoSpec
{
	bool m_Carrier;
	int m_Kind = -1;
	float m_Capacity;
	bool m_Infinite;
	bool m_Transfer = true;
	bool m_StorageEnergy;			// true — заряд EnergyManager (аккумулятор), false — количество предмета
};

class VB_Echo
{
	static const vector COLOR_ECHO = "1 0.1 0.14";		// красный — Эхо
	static const vector COLOR_FUSION = "0.1 0.95 0.85";	// бирюзовый — холодный синтез

	protected static ref map<string, ref VB_EchoSpec> s_Specs;

	// блок VB_Echo класса (читается один раз на класс)
	static VB_EchoSpec Spec(EntityAI e)
	{
		if (!e)
			return null;
		if (!s_Specs)
			s_Specs = new map<string, ref VB_EchoSpec>;
		string type = e.GetType();
		VB_EchoSpec sp;
		if (s_Specs.Find(type, sp))
			return sp;
		sp = new VB_EchoSpec;
		string path = "CfgVehicles " + type + " VB_Echo";
		if (GetGame().ConfigIsExisting(path))
		{
			sp.m_Carrier = true;
			string kindName;
			GetGame().ConfigGetText(path + " kind", kindName);
			sp.m_Kind = VB_EchoKind.ECHO;
			if (kindName == "fusion")
				sp.m_Kind = VB_EchoKind.FUSION;
			sp.m_Capacity = GetGame().ConfigGetFloat(path + " capacity");
			sp.m_Infinite = GetGame().ConfigGetInt(path + " infinite") == 1;
			if (GetGame().ConfigIsExisting(path + " transfer"))
				sp.m_Transfer = GetGame().ConfigGetInt(path + " transfer") == 1;
			string storageName;
			GetGame().ConfigGetText(path + " storage", storageName);
			sp.m_StorageEnergy = storageName == "energy";
		}
		s_Specs.Insert(type, sp);
		return sp;
	}

	static bool IsCarrier(EntityAI e)
	{
		VB_EchoSpec sp = Spec(e);
		return sp && sp.m_Carrier;
	}

	static int Kind(EntityAI e)
	{
		VB_EchoSpec sp = Spec(e);
		if (!sp || !sp.m_Carrier)
			return VB_EchoKind.NONE;
		return sp.m_Kind;
	}

	static bool IsFusion(EntityAI e)
	{
		return Kind(e) == VB_EchoKind.FUSION;
	}

	static bool IsInfinite(EntityAI e)
	{
		VB_EchoSpec sp = Spec(e);
		return sp && sp.m_Carrier && sp.m_Infinite;
	}

	// можно ли перелить энергию из носителя (синтез — нельзя: питает только то, во что вставлен)
	static bool CanTransfer(EntityAI e)
	{
		VB_EchoSpec sp = Spec(e);
		return sp && sp.m_Carrier && sp.m_Transfer && !sp.m_Infinite;
	}

	static float Capacity(EntityAI e)
	{
		VB_EchoSpec sp = Spec(e);
		if (!sp || !sp.m_Carrier)
			return 0;
		if (sp.m_StorageEnergy)
		{
			if (e.GetCompEM())
				return e.GetCompEM().GetEnergyMax();
			return 0;
		}
		ItemBase ib = ItemBase.Cast(e);
		if (sp.m_Capacity <= 0 && ib)
			return ib.GetQuantityMax();
		return sp.m_Capacity;
	}

	// заряд сейчас: бесконечный — всегда полный, разрушенный — 0. Для сервера (у аккумулятора — точная энергия).
	static float Get(EntityAI e)
	{
		VB_EchoSpec sp = Spec(e);
		if (!sp || !sp.m_Carrier || e.IsRuined())
			return 0;
		if (sp.m_Infinite)
			return Capacity(e);
		if (sp.m_StorageEnergy)
		{
			if (e.GetCompEM())
				return e.GetCompEM().GetEnergy();
			return 0;
		}
		ItemBase ib = ItemBase.Cast(e);
		if (ib)
			return ib.GetQuantity();
		return 0;
	}

	// доля заряда 0..1 — годится и на клиенте (у аккумулятора — по количеству, которое ваниль ведёт от энергии)
	static float Fraction(EntityAI e)
	{
		VB_EchoSpec sp = Spec(e);
		if (!sp || !sp.m_Carrier || e.IsRuined())
			return 0;
		if (sp.m_Infinite)
			return 1;
		ItemBase ib = ItemBase.Cast(e);
		if (ib && ib.GetQuantityMax() > 0)
			return Math.Clamp(ib.GetQuantity() / ib.GetQuantityMax(), 0, 1);
		float cap = Capacity(e);
		if (cap <= 0)
			return 0;
		return Math.Clamp(Get(e) / cap, 0, 1);
	}

	static bool HasCharge(EntityAI e)
	{
		return Fraction(e) > 0;
	}

	// сервер: списать до amount, вернуть, сколько списано. Бесконечный отдаёт сколько просят и не убывает.
	static float Drain(EntityAI e, float amount)
	{
		VB_EchoSpec sp = Spec(e);
		if (!sp || !sp.m_Carrier || amount <= 0 || e.IsRuined())
			return 0;
		if (sp.m_Infinite)
			return amount;
		float have = Get(e);
		float take = Math.Min(have, amount);
		if (take <= 0)
			return 0;
		SetCharge(e, sp, have - take);
		return take;
	}

	// сервер: добавить до amount, вернуть, сколько принято. Бесконечный и разрушенный не принимают.
	static float Add(EntityAI e, float amount)
	{
		VB_EchoSpec sp = Spec(e);
		if (!sp || !sp.m_Carrier || amount <= 0 || e.IsRuined() || sp.m_Infinite)
			return 0;
		float have = Get(e);
		float put = Math.Min(Capacity(e) - have, amount);
		if (put <= 0)
			return 0;
		SetCharge(e, sp, have + put);
		return put;
	}

	protected static void SetCharge(EntityAI e, VB_EchoSpec sp, float value)
	{
		ItemBase ib = ItemBase.Cast(e);
		if (sp.m_StorageEnergy)
		{
			if (!e.GetCompEM())
				return;
			e.GetCompEM().SetEnergy(value);
			if (ib)
				ib.ConvertEnergyToQuantity();		// полоска в инвентаре = заряд (SetEnergy сам её не трогает)
			return;
		}
		if (ib)
			ib.SetQuantity(value);
	}

	static vector ColorOf(int kind)
	{
		if (kind == VB_EchoKind.FUSION)
			return COLOR_FUSION;
		return COLOR_ECHO;
	}
};
