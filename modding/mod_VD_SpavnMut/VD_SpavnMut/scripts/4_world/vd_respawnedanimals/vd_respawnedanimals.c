//	АБВ
class VD_RespawnedAnimals
{
	private ref array<ref VD_GroupAnimals> m_VD_GroupsAnimals = new array<ref VD_GroupAnimals>;
	private ref map<string, VD_GroupAnimals> m_GroupsByKey = new map<string, VD_GroupAnimals>;

	//	Сетка зон: проверяем только зоны рядом с игроками
	private ref map<int, ref array<VD_GroupAnimals>> m_Grid = new map<int, ref array<VD_GroupAnimals>>;
	private float m_CellSize = 100.0;

	//	Зоны с живыми существами проверяются всегда, даже если игроки ушли
	private ref array<VD_GroupAnimals> m_LiveGroups = new array<VD_GroupAnimals>;
	private ref array<VD_GroupAnimals> m_Queue = new array<VD_GroupAnimals>;
	private ref array<Man> m_Players = new array<Man>;
	private ref PGFilter m_Filter = new PGFilter();

	private int m_BatchIndex = 0;
	private int m_BatchSize = 1;
	private int m_PassStep = 0;

	private int m_ChangeTimer = 0;
	private bool m_IsStarted = false;
	private bool m_IsNight = false;

	void VD_RespawnedAnimals()
	{
		m_Filter.SetFlags(PGPolyFlags.WALK, PGPolyFlags.DISABLED | PGPolyFlags.INSIDE | PGPolyFlags.SWIM | PGPolyFlags.SWIM_SEA, 0);

		LoadConfig(VD_RESPAWNED_ANIMALS_DAY_CONFIGS, false);
		LoadConfig(VD_RESPAWNED_ANIMALS_NIGHT_CONFIGS, true);
		m_GroupsByKey.Clear();
		BuildGrid();
	}

	void ~VD_RespawnedAnimals()
	{
		if (GetGame())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(this.ProcessAnimalsBatch);
		}
	}

	TStringArray LoadFileLines(string path)
	{
		TStringArray lines = {};
		string line;

		if (!FileExist(path))
		{
			PrintWithTime(string.Format("[LoadFileLines] Файл не найден: %1", path));
			return lines;
		}

		FileHandle file = OpenFile(path, FileMode.READ);
		if (file == 0)
		{
			PrintWithTime(string.Format("[LoadFileLines] Не удалось открыть файл: %1", path));
			return lines;
		}

		while (FGets(file, line) > 0)
		{
			line = line.Trim();
			if (line != "" && line.Substring(0, 1) != "#")
				lines.Insert(line);
		}
		CloseFile(file);
		return lines;
	}

	void LoadConfig(string path, bool isNight)
	{
		TStringArray fileLines = LoadFileLines(path);
		if (fileLines.Count() < 3)
		{
			PrintWithTime(string.Format("[LoadConfig] Конфиг пустой или не загружен: %1", path));
			return;
		}

		for (int i = 0; i + 2 < fileLines.Count(); i += 3)
		{
			string classnamesLine = fileLines[i];
			string positionLine = fileLines[i + 1];
			string paramsLine = fileLines[i + 2];

			// --- Класснеймы ---
			TStringArray classnames = {};
			classnamesLine.Split(",", classnames);
			for (int c = 0; c < classnames.Count(); c++)
				classnames[c] = classnames[c].Trim();

			if (classnames.Count() == 0)
				continue;

			// --- Координаты ---
			TStringArray coords = {};
			positionLine.Split(" ", coords);
			if (coords.Count() != 3)
				continue;

			vector position = Vector(coords[0].ToFloat(), coords[1].ToFloat(), coords[2].ToFloat());

			// --- Параметры: количество|респавн|радиус|разброс+|разброс- ---
			TStringArray params = {};
			paramsLine.Split("|", params);
			if (params.Count() < 5)
				continue;

			int count = params[0].ToInt();
			int respawnTime = params[1].ToInt();
			int radius = params[2].ToInt();
			int randomPlus = params[3].ToInt();
			int randomMinus = params[4].ToInt();

			// Одна и та же зона из дневного и ночного файла становится одной зоной
			string key = string.Format("%1|%2|%3|%4", classnamesLine, positionLine, respawnTime, radius);
			VD_GroupAnimals group = m_GroupsByKey.Get(key);
			if (!group)
			{
				group = new VD_GroupAnimals(classnames, position, respawnTime, radius, randomPlus, randomMinus);
				m_VD_GroupsAnimals.Insert(group);
				m_GroupsByKey.Set(key, group);
			}
			group.AddCount(isNight, count);
		}
	}

	private void BuildGrid()
	{
		int dayCount = 0;
		int nightCount = 0;
		foreach (VD_GroupAnimals sizeGroup : m_VD_GroupsAnimals)
		{
			dayCount += sizeGroup.GetCount(false);
			nightCount += sizeGroup.GetCount(true);

			// Размер ячейки = дальность удержания зоны, поэтому хватает соседних ячеек
			float reach = sizeGroup.GetRadius() * 1.2;
			if (reach > m_CellSize)
				m_CellSize = reach;
		}

		foreach (VD_GroupAnimals cellGroup : m_VD_GroupsAnimals)
		{
			vector groupPosition = cellGroup.GetPosition();
			int key = GetCellKey(GetCell(groupPosition[0]), GetCell(groupPosition[2]));
			array<VD_GroupAnimals> cell = m_Grid.Get(key);
			if (!cell)
			{
				cell = new array<VD_GroupAnimals>;
				m_Grid.Set(key, cell);
			}
			cell.Insert(cellGroup);
		}

		PrintWithTime(string.Format("[VD_RespawnedAnimals] Загружено зон: %1, существ днём: %2, ночью: %3, ячейка сетки: %4 м", m_VD_GroupsAnimals.Count(), dayCount, nightCount, m_CellSize));
	}

	private int GetCell(float coordinate)
	{
		return Math.Floor(coordinate / m_CellSize);
	}

	private int GetCellKey(int cellX, int cellZ)
	{
		return cellX + (cellZ * 100000);
	}

	private void Enqueue(VD_GroupAnimals group)
	{
		if (group.m_IsQueued)
			return;

		group.m_IsQueued = true;
		group.m_NearestPlayer = 1000000.0;
		m_Queue.Insert(group);
	}

	//	Очередь прохода: зоны в соседних с игроками ячейках и зоны с живыми существами
	private void BuildQueue()
	{
		m_Queue.Clear();
		m_BatchIndex = 0;
		m_Players.Clear();
		GetGame().GetPlayers(m_Players);

		foreach (Man player : m_Players)
		{
			if (!player)
				continue;

			vector playerPosition = player.GetPosition();
			int cellX = GetCell(playerPosition[0]);
			int cellZ = GetCell(playerPosition[2]);

			for (int offsetX = -1; offsetX <= 1; offsetX++)
			{
				for (int offsetZ = -1; offsetZ <= 1; offsetZ++)
				{
					array<VD_GroupAnimals> cell = m_Grid.Get(GetCellKey(cellX + offsetX, cellZ + offsetZ));
					if (!cell)
						continue;

					foreach (VD_GroupAnimals cellGroup : cell)
					{
						Enqueue(cellGroup);
						cellGroup.OnPlayerNearby(playerPosition);
					}
				}
			}
		}

		for (int i = m_LiveGroups.Count() - 1; i >= 0; i--)
		{
			VD_GroupAnimals liveGroup = m_LiveGroups[i];
			if (liveGroup.HasTracked())
			{
				Enqueue(liveGroup);
			}
			else
			{
				liveGroup.m_IsLive = false;
				m_LiveGroups.Remove(i);
			}
		}

		m_BatchSize = (m_Queue.Count() + VD_ANIMALS_PASS_STEPS - 1) / VD_ANIMALS_PASS_STEPS;
	}

	//	Единственный цикл обработки: шаг раз в 100 мс, новый проход только после конца старого
	void ProcessAnimalsBatch()
	{
		m_PassStep++;
		if (m_PassStep >= VD_ANIMALS_PASS_STEPS && m_BatchIndex >= m_Queue.Count())
		{
			m_PassStep = 0;
			BuildQueue();
		}

		for (int i = 0; i < m_BatchSize && m_BatchIndex < m_Queue.Count(); i++)
		{
			VD_GroupAnimals group = m_Queue[m_BatchIndex];
			m_BatchIndex++;

			group.m_IsQueued = false;
			group.Update(m_Players, m_IsNight, m_Filter);

			if (!group.m_IsLive && group.HasTracked())
			{
				group.m_IsLive = true;
				m_LiveGroups.Insert(group);
			}
		}
	}

	private bool IsNightTime()
	{
		int year, month, day, hour, minute;
		GetGame().GetWorld().GetDate(year, month, day, hour, minute);
		return hour < 6 || hour >= 20;
	}

	void OnUpdate()
	{
		// Фазу определяем сразу на первом тике: сервер может стартовать и ночью
		if (!m_IsStarted)
		{
			m_IsStarted = true;
			m_IsNight = IsNightTime();
			PrintWithTime(string.Format("[VD_RespawnedAnimals] Старт сервера, ночь: %1", m_IsNight));
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.ProcessAnimalsBatch, 100, true);
			return;
		}

		m_ChangeTimer++;
		if (m_ChangeTimer < VD_RESPAWNED_ANIMALS_CHANGE_TIMER)
			return;

		m_ChangeTimer = 0;
		bool isNightNow = IsNightTime();
		if (isNightNow == m_IsNight)
			return;

		m_IsNight = isNightNow;
		PrintWithTime(string.Format("[VD_RespawnedAnimals] Смена фазы, ночь: %1", m_IsNight));
		if (m_IsNight)
			ADU_SendChatMessage(VD_MESSAGE_NIGHT);
		else
			ADU_SendChatMessage(VD_MESSAGE_DAY);
	}

	void PrintWithTime(string message)
	{
		int year, month, day, hour, minute;
		GetGame().GetWorld().GetDate(year, month, day, hour, minute);
		int second = (GetGame().GetTime() / 1000) % 60;  // Переводим миллисекунды в секунды

		Print( string.Format( "Игровое время: [%1:%2:%3] %4", hour, minute, second, message ) );
	}

	static void ADU_SendChatMessage(string message)
	{
		ref array<Man> players = new array<Man>;
		// Get array of active players
		GetGame().GetPlayers(players);
		// Build message parameter
		Param1<string> m_MessageParam = new Param1<string>(message);
		// Loop through player array
		foreach(auto player : players)
		{
			// Send player text message
			GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, m_MessageParam, true, player.GetIdentity());
		}
	}
}
