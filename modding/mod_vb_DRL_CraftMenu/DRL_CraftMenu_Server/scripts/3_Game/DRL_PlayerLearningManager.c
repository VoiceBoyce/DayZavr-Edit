ref DRL_PlayerLearningManager g_DRL_PlayerLearningManager;

DRL_PlayerLearningManager GetPlayerLearningManager()
{
	if (!g_DRL_PlayerLearningManager)
	{
		g_DRL_PlayerLearningManager = new DRL_PlayerLearningManager();
	}
	return g_DRL_PlayerLearningManager;
}

class DRL_PlayerLearningManager
{
	private const static string DIR_PLAYERS = "$profile:\\DRL_CraftMenu\\Players\\";
	protected ref map<string, ref DRL_PlayerLearningData> m_PlayerAccounts = new map<string, ref DRL_PlayerLearningData>();

	void DRL_PlayerLearningManager()
	{
		if (!FileExist(DIR_PLAYERS))
		{
			MakeDirectory(DIR_PLAYERS);
		}
	}

	ref DRL_PlayerLearningData GetPlayerData(string playerId)
	{
		if (playerId == "")
		{
			playerId = "offline_player";
		}

		ref DRL_PlayerLearningData data;
		if (m_PlayerAccounts.Find(playerId, data))
		{
			if (data)
			{
				return data;
			}
		}

		string filePath = DIR_PLAYERS + playerId + ".json";
		string err;

		data = new DRL_PlayerLearningData(playerId);

		if (FileExist(filePath))
		{
			if (!JsonFileLoader<ref DRL_PlayerLearningData>.LoadFile(filePath, data, err))
			{
				DRL_CraftLogger.Error("[DRL_PlayerLearningManager] Error loading " + filePath + ": " + err);
			}
			else
			{
				DRL_CraftLogger.Info("[DRL_PlayerLearningManager] Loaded player: " + playerId + " (recipes: " + data.LearnedRecipeIds.Count().ToString() + ")");
			}
		}
		else
		{
			SavePlayerData(data);
			DRL_CraftLogger.Info("[DRL_PlayerLearningManager] Created new player file: " + filePath);
		}

		m_PlayerAccounts.Set(playerId, data);
		return data;
	}

	void SavePlayerData(DRL_PlayerLearningData data)
	{
		if (!data)
		{
			return;
		}

		if (!FileExist(DIR_PLAYERS))
		{
			MakeDirectory(DIR_PLAYERS);
		}

		string playerId = data.PlayerId;
		if (playerId == "")
		{
			playerId = "offline_player";
		}

		string filePath = DIR_PLAYERS + playerId + ".json";
		string err;

		if (!JsonFileLoader<ref DRL_PlayerLearningData>.SaveFile(filePath, data, err))
		{
			DRL_CraftLogger.Error("[DRL_PlayerLearningManager] Error saving " + filePath + ": " + err);
		}
		else
		{
			DRL_CraftLogger.Info("[DRL_PlayerLearningManager] Saved player data: " + playerId);
		}
	}

	void OnPlayerDisconnected(string playerId)
	{
		if (playerId == "")
		{
			return;
		}

		if (m_PlayerAccounts.Contains(playerId))
		{
			ref DRL_PlayerLearningData data = m_PlayerAccounts.Get(playerId);
			if (data)
			{
				SavePlayerData(data);
			}
			m_PlayerAccounts.Remove(playerId);
			DRL_CraftLogger.Info("[DRL_PlayerLearningManager] Unloaded player cache on disconnect: " + playerId);
		}
	}
}
