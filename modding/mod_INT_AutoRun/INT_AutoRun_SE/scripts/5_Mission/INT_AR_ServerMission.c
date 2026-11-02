// INT AutoRun - server ready line (fail-loud companion to the CL SE check).
modded class MissionServer
{
	override void OnMissionStart()
	{
		super.OnMissionStart();
		if (!GetGame().IsDedicatedServer())
			return;
		Print("[INT_AR_SE] ready");
	}
}
