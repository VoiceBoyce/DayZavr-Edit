//DayZavr: the notice a player gets after coming back from leaving the game while flying an RFFS helicopter.
//The server module (INT_Helicopters_SE) sends it; the client shows it in INT_HELI_NoticeMenu.
class INT_HELI_Notice
{
	static const int INT_RPC_S2C_NOTICE = 9410212;	//server -> client: Param5<status, moved to the ground, heli name, x, z>

	static const int INT_STATUS_NOT_FOUND = 0;
	static const int INT_STATUS_LANDED = 1;
	static const int INT_STATUS_RUINED = 2;
	static const int INT_STATUS_FLYING = 3;

	static bool s_INT_Pending;
	static float s_INT_ReceivedAt;
	static int s_INT_Status;
	static bool s_INT_Moved;
	static string s_INT_HeliName;
	static float s_INT_X;
	static float s_INT_Z;

	static void INT_Set(int status, bool moved, string heli_name, float x, float z)
	{
		s_INT_Status = status;
		s_INT_Moved = moved;
		s_INT_HeliName = heli_name;
		s_INT_X = x;
		s_INT_Z = z;
		s_INT_ReceivedAt = g_Game.GetTickTime();
		s_INT_Pending = true;
	}

	//First paragraph of the window
	static string INT_TextIntro()
	{
		if (s_INT_Moved)
			return "Вы вышли из игры во время полёта. Чтобы вы не разбились, вас переместили на землю.";
		return "Вы вышли из игры во время полёта.";
	}

	//What happened to the helicopter: two lines, the name on the first (display names run up to ~40 characters)
	static string INT_TextHeli()
	{
		string calc_heli = "Ваш вертолёт " + s_INT_HeliName + "\n";
		switch (s_INT_Status)
		{
			case INT_STATUS_LANDED:
				return calc_heli + "приземлился автоматически:";
			case INT_STATUS_RUINED:
				return calc_heli + "разбит:";
			case INT_STATUS_FLYING:
				return calc_heli + "сейчас в воздухе:";
		}
		return calc_heli + "больше не найден.";
	}

	//Map coordinates (X east, Y north, as on the map sites); empty when the helicopter is gone
	static string INT_TextCoords()
	{
		if (s_INT_Status == INT_STATUS_NOT_FOUND)
			return "";
		int calc_x = s_INT_X;
		int calc_y = s_INT_Z;
		return "X " + calc_x.ToString() + ", Y " + calc_y.ToString();
	}
}
