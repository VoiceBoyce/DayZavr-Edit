// INT_PDA (server) — the random character of the vanilla OnClientNewEvent is the class the player chose in the PDA
// «Персонаж» tab while his character is being made (INT_PDA_Chars.s_ForceType, set only around that call).

modded class DayZGame
{
	override string CreateRandomPlayer()
	{
		if (INT_PDA_Chars.s_ForceType != "")
			return INT_PDA_Chars.s_ForceType;
		return super.CreateRandomPlayer();
	}
}
