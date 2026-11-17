//DayZavr: receives the helicopter login notice (INT_HELI_Notice) from the server
modded class PlayerBase
{
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);

		if (rpc_type != INT_HELI_Notice.INT_RPC_S2C_NOTICE || !g_Game.IsClient())
			return;

		Param5<int, bool, string, float, float> calc_data = new Param5<int, bool, string, float, float>(0, false, "", 0, 0);
		if (!ctx.Read(calc_data))
			return;
		INT_HELI_Notice.INT_Set(calc_data.param1, calc_data.param2, calc_data.param3, calc_data.param4, calc_data.param5);
	}
}
