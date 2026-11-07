// INT_PDA — events between layers and parts. The item (4_World) raises them; the client logic (5_Mission) and the
// server part (INT_PDA_SE) subscribe. Nothing here polls: every event comes from the engine (energy manager work
// start/stop, attach/detach, location change) or from a player action.

class INT_PDA_Events
{
	// client: a PDA near the local player changed (switched on/off, battery or module attached/detached, moved).
	static ref ScriptInvoker s_PdaChanged = new ScriptInvoker();
	// client: «Заглянуть в КПК» on this PDA (EntityAI).
	static ref ScriptInvoker s_OpenRequest = new ScriptInvoker();
	// server: a PDA stopped working (switched off / battery empty) — (EntityAI pda).
	static ref ScriptInvoker s_ServerWorkStop = new ScriptInvoker();

	static void PdaChanged(EntityAI pda)
	{
		s_PdaChanged.Invoke(pda);
	}

	static void OpenRequest(EntityAI pda)
	{
		s_OpenRequest.Invoke(pda);
	}

	static void ServerWorkStop(EntityAI pda)
	{
		s_ServerWorkStop.Invoke(pda);
	}
}
