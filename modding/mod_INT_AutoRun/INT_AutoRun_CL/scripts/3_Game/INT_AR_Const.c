// INT AutoRun - shared constants (3_Game: visible to World and Mission).
class INT_AR_Const
{
	static const int RPC_C2S_HELLO = -44101;
	static const int RPC_S2C_ACK = -44102;
	static const int RPC_C2S_RUN = -44103;
	static const string UA_TOGGLE = "UAAutoRunToggle";
	static const float STAMINA_LOW = 0.15;
	static const float STAMINA_OK = 0.35;
	static const float ACK_TIMEOUT_S = 5.0;
	static const float HUD_W = 420.0;
	static const float HUD_H = 40.0;
	static const float HUD_X = 750.0;
	static const float HUD_Y = 905.0;
	static const float SLOT_ABOVE_GRIDBOTTOM = 56.0;
	static const float PILL_GAP = 25.0;
	static const float PILL_BOTTOM_MARGIN = 135.0;
}

// Mutable client session state. Written by PlayerBase.OnRPC (ack),
// read by the MissionGameplay controller.
class INT_AR_Net
{
	static bool m_Acked = false;
	static float m_HelloAt = -1000.0;

	static void OnAck()
	{
		m_Acked = true;
	}

	static bool IsAcked()
	{
		return m_Acked;
	}

	static void Reset()
	{
		m_Acked = false;
		m_HelloAt = -1000.0;
	}
}
