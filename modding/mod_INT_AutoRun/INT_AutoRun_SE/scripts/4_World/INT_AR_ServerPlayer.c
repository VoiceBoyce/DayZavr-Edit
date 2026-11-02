// INT AutoRun - server side of the handshake: answers client hello with an ack.
// Requires the client part (constants); runs only on the dedicated server.
modded class PlayerBase
{
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		if (!GetGame().IsDedicatedServer())
			return;
		if (!sender)
			return;
		if (rpc_type == INT_AR_Const.RPC_C2S_HELLO)
		{
			ScriptRPC rpc = new ScriptRPC();
			rpc.Write(true);
			rpc.Send(this, INT_AR_Const.RPC_S2C_ACK, true, sender);
			Print("[INT_AR_SE] ack sent");
		}
		if (rpc_type == INT_AR_Const.RPC_C2S_RUN)
		{
			bool run = false;
			int idx = 0;
			if (!ctx.Read(run))
				return;
			if (!ctx.Read(idx))
				return;
			HumanInputController hic = GetInputController();
			if (!hic)
				return;
			if (run)
			{
				hic.OverrideMovementSpeed(HumanInputControllerOverrideType.ENABLED, idx);
				hic.OverrideMovementAngle(HumanInputControllerOverrideType.ENABLED, 1);
			}
			else
			{
				hic.OverrideMovementSpeed(HumanInputControllerOverrideType.DISABLED, 0);
				hic.OverrideMovementAngle(HumanInputControllerOverrideType.DISABLED, 0);
			}
		}
	}
}
