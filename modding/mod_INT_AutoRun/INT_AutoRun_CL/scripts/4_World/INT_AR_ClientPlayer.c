// INT AutoRun - client side of the handshake: receives the server ack.
modded class PlayerBase
{
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		if (GetGame().IsDedicatedServer())
			return;
		if (rpc_type == INT_AR_Const.RPC_S2C_ACK)
		{
			INT_AR_Net.OnAck();
			Print("[INT_AR] ack from server");
		}
	}
}
