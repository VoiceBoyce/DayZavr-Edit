// Телепорт из зоны при заходе на сервер (серверный мод).
// Игрок, загрузившийся внутри настроенной зоны, переносится на случайную точку выхода этой зоны.
// Срабатывает только при заходе: кто зашёл в зону ногами, того мод не трогает.
// Конфиг: $profile:VB_Mods/VB_ZoneTeleport/config.json, читается при старте сервера.

class VB_ZoneExitPoint
{
	vector Position;	// y = 0: поставить на поверхность земли
}

class VB_Zone
{
	string Name;
	vector Center;		// высота не учитывается, зона действует на любой высоте
	float Radius;
	ref array<ref VB_ZoneExitPoint> ExitPoints;

	bool IsValid()
	{
		return Radius > 0 && ExitPoints && ExitPoints.Count() > 0;
	}

	bool Contains(vector pos)
	{
		float dx = pos[0] - Center[0];
		float dz = pos[2] - Center[2];
		return dx * dx + dz * dz <= Radius * Radius;
	}

	VB_ZoneExitPoint GetRandomExitPoint()
	{
		return ExitPoints[Math.RandomInt(0, ExitPoints.Count())];
	}
}

class VB_ZoneTeleportConfig
{
	float TeleportDelaySec = 2;
	string MessageTitle = "Телепорт";
	string Message = "В этой зоне нельзя выходить из игры. Вас перенесло наружу.";
	float MessageDelaySec = 15;	// телепорт проходит ещё на экране загрузки, поэтому сообщение отправляем позже
	float MessageTimeSec = 10;
	ref array<ref VB_Zone> Zones = new array<ref VB_Zone>();
}

class VB_ZoneTeleport : Managed
{
	protected const string DIR_ROOT = "$profile:VB_Mods";
	protected const string DIR_MOD = "$profile:VB_Mods/VB_ZoneTeleport";
	protected const string CONFIG_FILE = "$profile:VB_Mods/VB_ZoneTeleport/config.json";

	protected ref VB_ZoneTeleportConfig m_Config;

	void VB_ZoneTeleport()
	{
		MakeDirectory(DIR_ROOT);
		MakeDirectory(DIR_MOD);
		LoadConfig();
	}

	protected void LoadConfig()
	{
		m_Config = new VB_ZoneTeleportConfig();
		string error;

		if (!FileExist(CONFIG_FILE))
		{
			if (!JsonFileLoader<VB_ZoneTeleportConfig>.SaveFile(CONFIG_FILE, m_Config, error))
				Print("[VB_ZoneTeleport] Не удалось создать конфиг: " + error);
			return;
		}

		if (!JsonFileLoader<VB_ZoneTeleportConfig>.LoadFile(CONFIG_FILE, m_Config, error))
		{
			// Файл не перезаписываем, чтобы не потерять зоны из-за опечатки в нём
			Print("[VB_ZoneTeleport] Ошибка в конфиге, зоны не загружены: " + error);
			m_Config = new VB_ZoneTeleportConfig();
			return;
		}

		if (!m_Config.Zones)
			m_Config.Zones = new array<ref VB_Zone>();

		for (int i = m_Config.Zones.Count() - 1; i >= 0; i--)
		{
			VB_Zone zone = m_Config.Zones[i];
			if (!zone || !zone.IsValid())
			{
				Print("[VB_ZoneTeleport] Зона №" + (i + 1) + " пропущена: нужны Radius > 0 и хотя бы одна точка в ExitPoints");
				m_Config.Zones.Remove(i);
			}
		}
	}

	void OnPlayerLoaded(PlayerBase player)
	{
		if (!player || m_Config.Zones.Count() == 0)
			return;

		int delay = Math.Max(m_Config.TeleportDelaySec, 0) * 1000;
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Process, delay, false, player);
	}

	protected void Process(PlayerBase player)
	{
		if (!player || !player.IsAlive() || !player.GetIdentity())
			return;

		VB_Zone zone = FindZone(player.GetPosition());
		if (zone)
			Teleport(player, zone);
	}

	protected VB_Zone FindZone(vector pos)
	{
		foreach (VB_Zone zone : m_Config.Zones)
		{
			if (zone.Contains(pos))
				return zone;
		}

		return null;
	}

	protected void Teleport(PlayerBase player, VB_Zone zone)
	{
		VB_ZoneExitPoint point = zone.GetRandomExitPoint();
		vector toPos = point.Position;
		if (toPos[1] == 0)
			toPos[1] = g_Game.SurfaceY(toPos[0], toPos[2]);

		player.SetPosition(toPos);

		if (m_Config.Message != "")
		{
			int messageDelay = Math.Max(m_Config.MessageDelaySec, 0) * 1000;
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(SendMessage, messageDelay, false, player);
		}
	}

	protected void SendMessage(PlayerBase player)
	{
		if (player && player.GetIdentity())
			NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), m_Config.MessageTimeSec, m_Config.MessageTitle, m_Config.Message);
	}
}

modded class MissionServer
{
	protected ref VB_ZoneTeleport m_VB_ZoneTeleport;

	override void OnInit()
	{
		super.OnInit();
		m_VB_ZoneTeleport = new VB_ZoneTeleport();
	}

	override void OnClientReadyEvent(PlayerIdentity identity, PlayerBase player)
	{
		super.OnClientReadyEvent(identity, player);
		m_VB_ZoneTeleport.OnPlayerLoaded(player);
	}

	override void OnClientReconnectEvent(PlayerIdentity identity, PlayerBase player)
	{
		super.OnClientReconnectEvent(identity, player);
		m_VB_ZoneTeleport.OnPlayerLoaded(player);
	}
}
