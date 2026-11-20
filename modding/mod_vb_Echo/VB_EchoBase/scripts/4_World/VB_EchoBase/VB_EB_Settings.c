// Настройки сети базы (только сервер): Instance_1\VB_Mods\VB_EchoBase\settings.json.
// Единицы (решение пользователя 11-20): энергия — kE, скорость — kE/tick, тик = 10 с (постоянная единица, не зависит
// от того, как часто сервер считает сеть). Объёмы и скорости — степени двойки.
// Вход/выход/радиус каждого устройства — в его конфиге (class VB_EchoRating), улучшенный уровень = новый класс;
// здесь можно переопределить любой класс (devices) и задать общее: лимиты, погоду, ночь, топливо печи.

class VB_EB_FuelEntry
{
	string type;		// класс предмета (точное имя)
	float energy;		// kE тока за одну штуку

	void VB_EB_FuelEntry(string t = "", float e = 0)
	{
		type = t;
		energy = e;
	}
};

// переопределение показателей класса устройства; 0 — оставить как в конфиге
class VB_EB_DeviceOverride
{
	string type;
	float outMax;		// kE/tick
	float inMax;		// kE/tick
	float io;			// kE/tick (накопитель: вход и выход)
	float range;		// м
};

class VB_EB_Settings
{
	static const int VERSION = 2;		// 2 — kE и kE/tick (11-20); старый файл переписывается по умолчанию
	static const float TICK = 10;		// с: единица «tick» в kE/tick
	static const string DIR_ROOT = "$profile:VB_Mods";
	static const string DIR = "$profile:VB_Mods/VB_EchoBase";
	static const string PATH = "$profile:VB_Mods/VB_EchoBase/settings.json";

	int version = VERSION;
	float tickSeconds = 10;				// как часто сервер считает все сети (на цифры kE/tick не влияет)
	int maxSources = 8;
	int maxStorages = 4;
	int maxStations = 2;
	int maxPads = 2;
	int maxRelays = 4;
	float solarOvercastLoss = 0.7;		// доля, которую съедает полная облачность
	float roofCheckHeight = 20;			// м: луч вверх задел крышу/навес — ветряк и панель не работают
	float nightMultiplier = 1.5;		// ночью преобразователь делает больше (лор: пульс Изнанки громче)
	int fusionCrystalNightAllDay = 1;	// 1 — с кристаллом синтеза ночной множитель весь день
	float crystalWearPerKE = 1;			// износ кристалла за 1 kE Эхо (кристалл — 2048)
	float padRadius = 5;				// м: где искать машины и игроков вокруг платформы
	float padHalfWidth = 2.0;			// м: зона зарядки — прямоугольник в осях платформы
	float padHalfLength = 3.0;
	float padHeight = 3.0;				// м над платформой (Echo Runner висит ~1,2 м)
	ref array<ref VB_EB_FuelEntry> fuel;
	ref array<ref VB_EB_DeviceOverride> devices;

	[NonSerialized()]
	protected ref map<string, VB_EB_FuelEntry> m_FuelByType;
	[NonSerialized()]
	protected ref map<string, VB_EB_DeviceOverride> m_OverrideByType;

	static VB_EB_Settings Load()
	{
		VB_EB_Settings s;
		string err;
		if (FileExist(PATH))
		{
			if (!JsonFileLoader<VB_EB_Settings>.LoadFile(PATH, s, err) || !s)
			{
				Print("[VB_EchoBase] настройки не прочитаны, беру по умолчанию: " + err);
				s = null;
			}
			else if (s.version < VERSION)
			{
				Print("[VB_EchoBase] настройки старой версии " + s.version + " (до kE) — переписываю по умолчанию");
				s = null;
			}
		}
		bool write = false;
		if (!s)
		{
			s = new VB_EB_Settings;
			write = true;
		}
		s.Normalize();
		if (write)
		{
			MakeDirectory(DIR_ROOT);
			MakeDirectory(DIR);
			if (!JsonFileLoader<VB_EB_Settings>.SaveFile(PATH, s, err))
				Print("[VB_EchoBase] настройки не записаны: " + err);
		}
		return s;
	}

	// JsonFileLoader не подставляет начальные значения для отсутствующих ключей (E57): ноль и меньше — по умолчанию
	void Normalize()
	{
		VB_EB_Settings d = new VB_EB_Settings;
		if (tickSeconds < 1)
			tickSeconds = d.tickSeconds;
		if (maxSources <= 0)
			maxSources = d.maxSources;
		if (maxStorages <= 0)
			maxStorages = d.maxStorages;
		if (maxStations <= 0)
			maxStations = d.maxStations;
		if (maxPads <= 0)
			maxPads = d.maxPads;
		if (maxRelays < 0)
			maxRelays = d.maxRelays;
		solarOvercastLoss = Math.Clamp(solarOvercastLoss, 0, 1);
		if (roofCheckHeight <= 0)
			roofCheckHeight = d.roofCheckHeight;
		if (nightMultiplier <= 0)
			nightMultiplier = d.nightMultiplier;
		if (crystalWearPerKE < 0)
			crystalWearPerKE = d.crystalWearPerKE;
		if (padRadius <= 0)
			padRadius = d.padRadius;
		if (padHalfWidth <= 0)
			padHalfWidth = d.padHalfWidth;
		if (padHalfLength <= 0)
			padHalfLength = d.padHalfLength;
		if (padHeight <= 0)
			padHeight = d.padHeight;
		if (!fuel || fuel.Count() == 0)
			fuel = DefaultFuel();
		if (!devices)
			devices = new array<ref VB_EB_DeviceOverride>;
		m_FuelByType = new map<string, VB_EB_FuelEntry>;
		foreach (VB_EB_FuelEntry f : fuel)
		{
			if (f && f.type != "" && f.energy > 0)
				m_FuelByType.Set(f.type, f);
		}
		m_OverrideByType = new map<string, VB_EB_DeviceOverride>;
		foreach (VB_EB_DeviceOverride o : devices)
		{
			if (o && o.type != "")
				m_OverrideByType.Set(o.type, o);
		}
	}

	// kE за штуку: степени двойки, кусок сердца Изнанки = целый бак (512)
	static array<ref VB_EB_FuelEntry> DefaultFuel()
	{
		array<ref VB_EB_FuelEntry> list = new array<ref VB_EB_FuelEntry>;
		list.Insert(new VB_EB_FuelEntry("WoodenStick", 2));
		list.Insert(new VB_EB_FuelEntry("Firewood", 16));
		list.Insert(new VB_EB_FuelEntry("VB_RiftRootPiece", 64));
		list.Insert(new VB_EB_FuelEntry("VB_RiftBlood", 128));
		list.Insert(new VB_EB_FuelEntry("VB_RiftFuelCapsule", 256));
		list.Insert(new VB_EB_FuelEntry("VB_RiftHeartChunk", 512));
		return list;
	}

	VB_EB_FuelEntry FindFuel(string type)
	{
		VB_EB_FuelEntry f;
		if (m_FuelByType && m_FuelByType.Find(type, f))
			return f;
		return null;
	}

	VB_EB_DeviceOverride FindOverride(string type)
	{
		VB_EB_DeviceOverride o;
		if (m_OverrideByType && m_OverrideByType.Find(type, o))
			return o;
		return null;
	}
};

// показатели класса устройства: конфиг (class VB_EchoRating) + переопределение из настроек; читается один раз на класс
class VB_EB_Rating
{
	int tier = 1;
	float outMax;		// kE/tick: источник — выработка, преобразователь — Эхо, платформа — отдаёт
	float inMax;		// kE/tick: преобразователь — ток, станция и платформа — берут из сети
	float io;			// kE/tick: накопитель — вход и выход
	float range;		// м: досягаемость преобразователя и ретранслятора

	protected static ref map<string, ref VB_EB_Rating> s_Cache;

	static VB_EB_Rating Of(EntityAI e, VB_EB_Settings s)
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
		}
		if (s)
		{
			VB_EB_DeviceOverride o = s.FindOverride(type);
			if (o)
			{
				if (o.outMax > 0)
					r.outMax = o.outMax;
				if (o.inMax > 0)
					r.inMax = o.inMax;
				if (o.io > 0)
					r.io = o.io;
				if (o.range > 0)
					r.range = o.range;
			}
		}
		s_Cache.Insert(type, r);
		return r;
	}
};
