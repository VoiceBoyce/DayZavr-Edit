// показатели класса устройства по конфигу (class VB_EchoRating); читается один раз на класс. Переопределение из
// настроек сервера — в серверном моде VB_Echo_Server (VB_EBS_Rating)
class VB_EB_Rating
{
	int tier = 1;
	float outMax;		// kE/tick: источник — выработка, преобразователь — Эхо, платформа — отдаёт
	float inMax;		// kE/tick: преобразователь — ток, станция и платформа — берут из сети
	float io;			// kE/tick: накопитель — вход и выход
	float range;		// м: досягаемость преобразователя и ретранслятора
	float chargeItem;	// kE/tick: станция — бак и аккумулятор, на гнездо
	float chargeCell;	// kE/tick: станция — ячейка на зарядке, на гнездо
	float drainCell;	// kE/tick: станция — ячейка на разрядке в сеть, на гнездо

	protected static ref map<string, ref VB_EB_Rating> s_Cache;

	static VB_EB_Rating Of(EntityAI e)
	{
		if (!s_Cache)
			s_Cache = new map<string, ref VB_EB_Rating>;
		string type = e.GetType();
		VB_EB_Rating r;
		if (s_Cache.Find(type, r))
			return r;
		r = new VB_EB_Rating;
		string path = "CfgVehicles " + type + " VB_EchoRating";
		if (GetGame().ConfigIsExisting(path))
		{
			if (GetGame().ConfigIsExisting(path + " tier"))
				r.tier = GetGame().ConfigGetInt(path + " tier");
			r.outMax = GetGame().ConfigGetFloat(path + " outMax");
			r.inMax = GetGame().ConfigGetFloat(path + " inMax");
			r.io = GetGame().ConfigGetFloat(path + " io");
			r.range = GetGame().ConfigGetFloat(path + " range");
			r.chargeItem = GetGame().ConfigGetFloat(path + " chargeItem");
			r.chargeCell = GetGame().ConfigGetFloat(path + " chargeCell");
			r.drainCell = GetGame().ConfigGetFloat(path + " drainCell");
		}
		s_Cache.Insert(type, r);
		return r;
	}
};
