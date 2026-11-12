enum ArtefactsRPCs
{
	RPC_DETECTOR = 12546
}

enum PlayerRPCs : ArtefactsRPCs
{
	BLOWOUT_PHASE,
	PSISTORM_PHASE,
	END_SESSION
}

enum HideZoneTriggerRPCs : PlayerRPCs
{
	DEBUG
}