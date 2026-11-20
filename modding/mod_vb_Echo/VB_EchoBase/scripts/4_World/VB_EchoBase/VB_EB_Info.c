// Состояние сети в курсоре действий (под названием устройства, решение 11-20 вместо сообщений в чат).
// Итоги тика сети есть только на сервере: клиент, пока смотрит на устройство, раз в секунду просит их RPC
// (VB_EB_Device.VB_RequestInfo), сервер отвечает цифрами (VB_EB_NetInfo), строки собирает клиент —
// так названия предметов идут на языке игрока. Заряд самих устройств и содержимое отсеков клиент знает сам
// (количество предмета и энергия синхронизируются ванилью). Курсор — scripts/5_Mission/VB_EchoBase/VB_EB_Cursor.c.

class VB_EB_RPC
{
	static const int INFO_REQUEST = 0x56454201;	// клиент → сервер: «что в сети у этого устройства»
	static const int INFO = 0x56454202;			// сервер → клиент: VB_EB_NetInfo
};

// цвета строк — как у неона (решение 11-20): ток жёлтый, Эхо красный, синтез бирюзовый, нет энергии серый
class VB_EB_Colors
{
	static const int TEXT = 0xFFD7D7D7;
	static const int DIM = 0xFF8C8C8C;
	static const int WARN = 0xFFF0A046;
	static const int CURRENT = 0xFFF2D250;
	static const int ECHO = 0xFFFF5C5C;
	static const int FUSION = 0xFF3CEBD7;
};

// итоги последнего тика для одного устройства; порядок полей в Write и Read один и тот же
class VB_EB_NetInfo
{
	// сеть
	bool m_InNet;
	float m_Prod;			// ток источников, kE/tick
	float m_Conv;			// сделано Эхо, kE/tick
	float m_Use;			// взято станциями и платформами, kE/tick
	bool m_Night;
	float m_NightMul;
	float m_Pool;			// буфер преобразователя + накопители, kE
	float m_PoolMax;
	int m_Crystal;			// 0 — нет или выработан, 1 — энергетический, 2 — холодного синтеза
	int m_Sources;
	int m_Storages;
	int m_Stations;
	int m_Pads;
	int m_Relays;
	int m_Skipped;			// устройств в радиусе сверх лимита
	// само устройство
	float m_In;				// kE/tick, взято в последнем тике
	float m_Out;			// kE/tick, отдано / выработано (у ветряка и панели — сейчас)
	float m_OutMax;
	float m_InMax;
	float m_Io;
	float m_Range;
	float m_Drain;			// kE/tick, отдано в сеть ячейками в гнёздах разрядки (станция, накопитель)
	float m_FuelStock;		// kE: запас топлива печи (порция + отсек)
	bool m_Covered;			// над источником крыша
	float m_Wind;			// м/с
	float m_Sun;			// 0..1
	float m_Cloud;			// 0..1
	float m_FuelLeft;		// kE в текущей порции печи
	string m_BurnType;		// класс сжигаемой порции
	bool m_HasFuel;			// в отсеке печи есть топливо из настроек

	void Write(ParamsWriteContext ctx)
	{
		ctx.Write(m_InNet);
		ctx.Write(m_Prod);
		ctx.Write(m_Conv);
		ctx.Write(m_Use);
		ctx.Write(m_Night);
		ctx.Write(m_NightMul);
		ctx.Write(m_Pool);
		ctx.Write(m_PoolMax);
		ctx.Write(m_Crystal);
		ctx.Write(m_Sources);
		ctx.Write(m_Storages);
		ctx.Write(m_Stations);
		ctx.Write(m_Pads);
		ctx.Write(m_Relays);
		ctx.Write(m_Skipped);
		ctx.Write(m_In);
		ctx.Write(m_Out);
		ctx.Write(m_OutMax);
		ctx.Write(m_InMax);
		ctx.Write(m_Io);
		ctx.Write(m_Range);
		ctx.Write(m_Covered);
		ctx.Write(m_Wind);
		ctx.Write(m_Sun);
		ctx.Write(m_Cloud);
		ctx.Write(m_FuelLeft);
		ctx.Write(m_BurnType);
		ctx.Write(m_HasFuel);
		ctx.Write(m_Drain);
		ctx.Write(m_FuelStock);
	}

	bool Read(ParamsReadContext ctx)
	{
		if (!ctx.Read(m_InNet) || !ctx.Read(m_Prod) || !ctx.Read(m_Conv) || !ctx.Read(m_Use))
			return false;
		if (!ctx.Read(m_Night) || !ctx.Read(m_NightMul) || !ctx.Read(m_Pool) || !ctx.Read(m_PoolMax))
			return false;
		if (!ctx.Read(m_Crystal) || !ctx.Read(m_Sources) || !ctx.Read(m_Storages) || !ctx.Read(m_Stations))
			return false;
		if (!ctx.Read(m_Pads) || !ctx.Read(m_Relays) || !ctx.Read(m_Skipped))
			return false;
		if (!ctx.Read(m_In) || !ctx.Read(m_Out) || !ctx.Read(m_OutMax) || !ctx.Read(m_InMax))
			return false;
		if (!ctx.Read(m_Io) || !ctx.Read(m_Range) || !ctx.Read(m_Covered) || !ctx.Read(m_Wind))
			return false;
		if (!ctx.Read(m_Sun) || !ctx.Read(m_Cloud) || !ctx.Read(m_FuelLeft) || !ctx.Read(m_BurnType) || !ctx.Read(m_HasFuel) || !ctx.Read(m_Drain) || !ctx.Read(m_FuelStock))
			return false;
		return true;
	}
};

// строки курсора: текст и цвет парами
class VB_EB_Lines
{
	ref array<string> m_Text = new array<string>;
	ref array<int> m_Color = new array<int>;

	void Clear()
	{
		m_Text.Clear();
		m_Color.Clear();
	}

	void Add(string text, int color)
	{
		m_Text.Insert(text);
		m_Color.Insert(color);
	}

	int Count()
	{
		return m_Text.Count();
	}

	// число для строки: от 100 — целое, меньше — один знак после запятой (без «.0»)
	static string N(float v)
	{
		if (Math.AbsFloat(v) >= 100)
		{
			int whole = Math.Round(v);
			return whole.ToString();
		}
		float r = Math.Round(v * 10) / 10;
		if (r == Math.Round(r))
		{
			int i = Math.Round(r);
			return i.ToString();
		}
		return r.ToString();
	}

	// название класса на языке игрока (ConfigGetText переводит $STR_ сам); вызывать на клиенте
	static string ClassName(string type)
	{
		string name;
		if (!GetGame().ConfigGetText("CfgVehicles " + type + " displayName", name) || name == "")
			name = type;
		return name;
	}
};
