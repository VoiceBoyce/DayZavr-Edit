// «Лес помнит» (ТЗ_сервер_18.md, ветка 2в п.4) и звуки рощи (п.5): клиентская часть. Логика — сервер
// (VB_AncientRoot_Server, VB_AR_MemoryServer.c; «Голос леса» — VB_AR_FavorServer.c).
//   RPC_WHISPER: шёпот рощи в точке (за спиной у Врага и Проклятого, со стороны рощи у «Голоса леса»).
//   RPC_MEMORY_GRIP: «хватка корней» у Проклятого — корни у ног (как у Хранителя, VB_AR_KeeperFx.VB_ShowGrip),
//     жертва стоит HOLD_MS, потом шагом до WALK_MS; бег не закрывается (без хромоты).

class VB_AR_MemoryFx
{
	static const int RPC_WHISPER = 1447186025;		// сервер -> игрок: Param1<vector> — откуда шёпот
	static const int RPC_MEMORY_GRIP = 1447186026;	// сервер -> игрок: Param2<vector, bool> — где корни, это ли ты

	static void VB_Whisper(vector pos)
	{
		EffectSound snd = SEffectManager.PlaySound(VB_AR_Pulse.WHISPER_SET, pos);
		if (snd)
			snd.SetAutodestroy(true);
	}
}

modded class PlayerBase
{
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		if (g_Game.IsDedicatedServer())
			return;		// только сервер -> клиент
		if (rpc_type == VB_AR_MemoryFx.RPC_WHISPER)
		{
			Param1<vector> w = new Param1<vector>(vector.Zero);
			if (ctx.Read(w))
				VB_AR_MemoryFx.VB_Whisper(w.param1);
			return;
		}
		if (rpc_type != VB_AR_MemoryFx.RPC_MEMORY_GRIP)
			return;
		Param2<vector, bool> g = new Param2<vector, bool>(vector.Zero, false);
		if (!ctx.Read(g))
			return;
		VB_AR_KeeperFx.VB_ShowGrip(g.param1);
		PlayerBase me = PlayerBase.Cast(g_Game.GetPlayer());
		if (g.param2 && me == this)
			VB_AR_HoldClient();
	}

	// корни держат: стоит, потом шагом (без хромоты — бег после этого открыт)
	protected void VB_AR_HoldClient()
	{
		HumanInputController hic = GetInputController();
		if (!hic)
			return;
		hic.OverrideMovementSpeed(true, 0);
		ScriptCallQueue q = g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY);
		q.Remove(VB_AR_GripWalk);
		q.Remove(VB_AR_GripFree);
		q.CallLater(VB_AR_GripWalk, VB_AR_KeeperFx.HOLD_MS, false);
		q.CallLater(VB_AR_GripFree, VB_AR_KeeperFx.WALK_MS, false);
	}
}
