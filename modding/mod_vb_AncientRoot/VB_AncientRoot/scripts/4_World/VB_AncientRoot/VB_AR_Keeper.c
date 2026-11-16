// Хранитель рощи — мини-босс (ТЗ_сервер_18.md, ветка 2в п.2): клиентская часть и общее для обеих сторон.
// Логика умений — сервер (VB_AncientRoot_Server, VB_AR_KeeperServer.c). Здесь:
//   «Хватка корней» (RPC_GRIP на игрока): у ног жертвы вырастает обвивка (локальная VB_AR_Wrap_27, без коллизии)
//     на GRIP_SHOW_MS, трещат корни; жертва стоит HOLD_MS, потом идёт только шагом до WALK_MS, бежать не может LIMP_MS
//     (PlayerBase.CanSprint — его зовут и клиент, и сервер; сервер ставит свой срок сам).
//   «Кора» (RPC_BARK на тело Хранителя): золотой свет и вспышка спор на время коры — свечение ярче.

class VB_AR_KeeperFx
{
	static const int RPC_BARK = 1447186023;		// сервер -> все: Param1<int> — на сколько мс кора (RPC тела Хранителя)
	static const int RPC_GRIP = 1447186024;		// сервер -> игрок: Param2<vector, bool> — где корни, это ли ты
	static const int GRIP_SHOW_MS = 4000;
	static const int HOLD_MS = 2000;
	static const int WALK_MS = 4000;
	static const int LIMP_MS = 10000;
	static const string GRIP_WRAP = "VB_AR_Wrap_27";

	// корни у ног: локальная обвивка (у всех игроков рядом своя), треск и вспышка спор
	static void VB_ShowGrip(vector pos)
	{
		vector p = Vector(pos[0], g_Game.SurfaceY(pos[0], pos[2]), pos[2]);
		Object wrap = g_Game.CreateObjectEx(GRIP_WRAP, p, ECE_LOCAL);
		if (wrap)
		{
			wrap.SetOrientation(Vector(Math.RandomFloat(0, 360), 0, 0));
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_DeleteLocal, GRIP_SHOW_MS, false, wrap);
		}
		EffectSound snd = SEffectManager.PlaySound(VB_AR_Pulse.CREAK_SET, p);
		if (snd)
			snd.SetAutodestroy(true);
		ParticleManager pm = ParticleManager.GetInstance();
		if (pm)
			pm.PlayInWorld(ParticleList.VB_AR_BURST, p + "0 0.3 0");
	}

	static void VB_DeleteLocal(Object o)
	{
		if (o && g_Game)
			g_Game.ObjectDelete(o);
	}
}

class VB_AR_BarkLight extends PointLightBase
{
	void VB_AR_BarkLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(9.0);
		SetBrightnessTo(3.0);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetAmbientColor(1.0, 0.6, 0.15);
		SetDiffuseColor(1.0, 0.6, 0.15);
	}
}

modded class PlayerBase
{
	protected int m_VB_AR_LimpUntil;		// g_Game.GetTime() своей стороны: до этого бежать нельзя

	void VB_AR_SetLimp(int ms)
	{
		m_VB_AR_LimpUntil = g_Game.GetTime() + ms;
	}

	override bool CanSprint()
	{
		if (m_VB_AR_LimpUntil > 0 && g_Game.GetTime() < m_VB_AR_LimpUntil)
			return false;		// «Хватка корней» Хранителя: хромает
		return super.CanSprint();
	}

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		if (rpc_type != VB_AR_KeeperFx.RPC_GRIP || g_Game.IsDedicatedServer())
			return;		// только сервер -> клиент: присланное клиентом серверу не исполняем
		Param2<vector, bool> p = new Param2<vector, bool>(vector.Zero, false);
		if (!ctx.Read(p))
			return;
		VB_AR_KeeperFx.VB_ShowGrip(p.param1);
		PlayerBase me = PlayerBase.Cast(g_Game.GetPlayer());		// this == GetPlayer() не компилируется (E82)
		if (p.param2 && me == this)
			VB_AR_GripClient();
	}

	// жертва: стоит, потом шагом, бег закрыт до LIMP_MS
	protected void VB_AR_GripClient()
	{
		VB_AR_SetLimp(VB_AR_KeeperFx.LIMP_MS);
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

	void VB_AR_GripWalk()
	{
		HumanInputController hic = GetInputController();
		if (hic)
			hic.OverrideMovementSpeed(true, 1);
	}

	void VB_AR_GripFree()
	{
		HumanInputController hic = GetInputController();
		if (hic)
			hic.OverrideMovementSpeed(false, 0);
	}
}

modded class ZombieBase
{
	protected VB_AR_BarkLight m_VB_AR_BarkLight;

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		if (rpc_type != VB_AR_KeeperFx.RPC_BARK)
			return;
		Param1<int> p = new Param1<int>(0);
		if (!ctx.Read(p))
			return;
		VB_AR_BarkClient(p.param1);
	}

	// «Кора»: свечение ярче на время коры
	protected void VB_AR_BarkClient(int ms)
	{
		if (g_Game.IsDedicatedServer())
			return;
		if (!m_VB_AR_BarkLight)
		{
			m_VB_AR_BarkLight = VB_AR_BarkLight.Cast(ScriptedLightBase.CreateLight(VB_AR_BarkLight, GetPosition()));
			if (m_VB_AR_BarkLight)
				m_VB_AR_BarkLight.AttachOnObject(this, "0 1.6 0");
		}
		ParticleManager pm = ParticleManager.GetInstance();
		if (pm)
			pm.PlayInWorld(ParticleList.VB_AR_BURST, GetPosition() + "0 1.2 0");
		EffectSound snd = SEffectManager.PlaySound(VB_AR_Pulse.CREAK_SET, GetPosition());
		if (snd)
			snd.SetAutodestroy(true);
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_AR_BarkEnd);
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_AR_BarkEnd, ms, false);
	}

	void VB_AR_BarkEnd()
	{
		if (m_VB_AR_BarkLight)
			m_VB_AR_BarkLight.Destroy();
		m_VB_AR_BarkLight = null;
	}

	override void EEDelete(EntityAI parent)
	{
		if (g_Game)
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_AR_BarkEnd);
		VB_AR_BarkEnd();
		super.EEDelete(parent);
	}
}
