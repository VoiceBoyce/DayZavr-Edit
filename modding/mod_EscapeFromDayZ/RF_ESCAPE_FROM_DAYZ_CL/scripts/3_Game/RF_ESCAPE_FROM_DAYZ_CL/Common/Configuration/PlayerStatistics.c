class RF_ESCAPE_FROM_DAYZ_PlayerStatistics: Managed {

    // MARK: - Internal Properties

    // Стимайди игрока
	string steamId64;

	// Убийство
	int kills;

	// Смерть
	int deaths;

	// Максимальная дистанция убийства игрока
	float playerKilledMaxDistance;

    // Максимальная дистанция убийства зомби
    float zombieKilledMaxDistance;

    // Время проведенное в пределах PVP зоны
    int timeInsidePVPZone;

	// Кол-во выходов в PVP зону
	int raidsAmount;
}

typedef array<autoptr RF_ESCAPE_FROM_DAYZ_PlayerStatistics> RF_ESCAPE_FROM_DAYZ_PlayerStatisticsArray;
typedef map<string, autoptr RF_ESCAPE_FROM_DAYZ_PlayerStatistics> RF_ESCAPE_FROM_DAYZ_PlayerStatisticsMap;