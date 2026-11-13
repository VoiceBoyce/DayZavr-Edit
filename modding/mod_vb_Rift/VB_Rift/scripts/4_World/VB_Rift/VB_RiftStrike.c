// Красная молния, когда кто-то вырезал сердце Изнанки. Сервер шлёт RPC каждому игроку,
// у каждого клиента бьёт в случайной точке в 250–1200 м от него: модель разряда из VB_RiftSky на долю секунды,
// красная вспышка, гром с задержкой по расстоянию — как настоящая гроза. Сколько сердец, столько ударов.
// Нет VB_RiftSky.pbo — модели разряда нет, остаются вспышка и гром.
// Здесь клиентская часть и номер RPC (нужен обеим сторонам); рассылку делает сервер —
// VB_RiftStrike.VB_BroadcastServer в VB_Rift_Server (VB_RiftWorldServer.c).

class VB_RiftStrikeLight extends PointLightBase
{
	void VB_RiftStrikeLight()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(250);
		SetBrightnessTo(40);
		SetCastShadow(false);
		SetFlareVisible(false);
		SetAmbientColor(1.0, 0.1, 0.12);
		SetDiffuseColor(1.0, 0.1, 0.12);
	}
}

class VB_RiftStrike
{
	static const int RPC_STRIKE = 1447186004;	// «VBRT», не пересекается с ванильными ERPCs
	static const float DIST_MIN = 250.0;
	static const float DIST_MAX = 1200.0;
	static const int FLASH_MS = 350;
	static const float SOUND_SPEED = 343.0;

	protected static ref array<ref VB_RiftStrike> s_VB_Active;

	protected Object m_VB_Bolt;
	protected VB_RiftStrikeLight m_VB_Light;
	protected vector m_VB_Pos;
	protected bool m_VB_Heavy;
	protected bool m_VB_Done;

	// клиент: удар где-то вокруг center
	static void VB_Start(vector center)
	{
		if (!s_VB_Active)
			s_VB_Active = new array<ref VB_RiftStrike>;

		for (int i = s_VB_Active.Count() - 1; i >= 0; i--)
		{
			if (s_VB_Active[i].m_VB_Done)
				s_VB_Active.Remove(i);
		}

		VB_RiftStrike strike = new VB_RiftStrike();
		s_VB_Active.Insert(strike);
		strike.VB_Run(center);
	}

	void VB_Run(vector center)
	{
		float a = Math.RandomFloat(0, Math.PI2);
		float d = Math.RandomFloat(DIST_MIN, DIST_MAX);
		float x = center[0] + Math.Cos(a) * d;
		float z = center[2] + Math.Sin(a) * d;
		float y = g_Game.SurfaceY(x, z);
		if (g_Game.SurfaceIsSea(x, z) || y < 0)
			y = 0;	// над морем — в уровень воды, а не в дно
		m_VB_Pos = Vector(x, y, z);
		m_VB_Heavy = Math.RandomFloat01() < 0.5;
		Print("[VB_Rift] удар молнии: " + Math.Round(d).ToString() + " м, " + m_VB_Pos.ToString());

		string boltType = "VB_RiftStrikeBolt";
		if (m_VB_Heavy)
			boltType = "VB_RiftStrikeBoltHeavy";
		m_VB_Bolt = g_Game.CreateObjectEx(boltType, m_VB_Pos, ECE_LOCAL);
		if (m_VB_Bolt)
			m_VB_Bolt.SetOrientation(Vector(Math.RandomFloat(0, 360), 0, 0));

		m_VB_Light = VB_RiftStrikeLight.Cast(ScriptedLightBase.CreateLight(VB_RiftStrikeLight, m_VB_Pos + "0 40 0"));
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_EndFlash, FLASH_MS, false);

		int delayMs = d / SOUND_SPEED * 1000.0;
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_Thunder, delayMs, false);
	}

	void VB_EndFlash()
	{
		if (m_VB_Bolt)
			g_Game.ObjectDelete(m_VB_Bolt);
		m_VB_Bolt = null;
		if (m_VB_Light)
			m_VB_Light.Destroy();
		m_VB_Light = null;
	}

	void VB_Thunder()
	{
		string soundSet = "ThunderNorm_Far_SoundSet";
		if (m_VB_Heavy)
			soundSet = "ThunderHeavy_Far_SoundSet";

		EffectSound snd = SEffectManager.PlaySound(soundSet, m_VB_Pos);
		if (snd)
			snd.SetAutodestroy(true);
		m_VB_Done = true;
	}
}

modded class PlayerBase
{
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);

		if (rpc_type == VB_RiftStrike.RPC_STRIKE && !g_Game.IsDedicatedServer())
			VB_RiftStrike.VB_Start(GetPosition());
	}
}
