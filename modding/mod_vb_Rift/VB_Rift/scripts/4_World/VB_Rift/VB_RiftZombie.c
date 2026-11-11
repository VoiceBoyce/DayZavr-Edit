// Зомби Изнанки (в конфиге vbRiftZombie = 1, классы VB_RiftZmb*).
// Смерть: глаза и жилы гаснут (материал vbRiftDeadMaterial без свечения), тело лопается облаком красных спор
// с мокрым хлопком; на сервере с шансом ROOT_CHANCE в инвентарь трупа кладётся «Корень Изнанки»,
// с шансом SPORES_CHANCE — «Споры Изнанки» (броски независимые).
// Труп, который игрок застал уже мёртвым (подгрузился позже), просто тёмный — без облака и звука.

class VB_RiftZombie
{
	static const string ROOT_TYPE = "VB_RiftRootPiece";
	static const float ROOT_CHANCE = 0.25;
	static const string SPORES_TYPE = "VB_RiftSpores";
	static const float SPORES_CHANCE = 0.05;
	static const string BURST_SOUND = "VB_RiftBurst_SoundSet";
	static const float BURST_HEIGHT = 1.1;	// примерно грудь, м

	static bool VB_Is(EntityAI zmb)
	{
		return zmb && zmb.ConfigGetBool("vbRiftZombie");
	}

	// сервер
	static void VB_OnKilledServer(ZombieBase zmb)
	{
		if (Math.RandomFloat01() < ROOT_CHANCE)
			VB_DropServer(zmb, ROOT_TYPE);
		if (Math.RandomFloat01() < SPORES_CHANCE)
			VB_DropServer(zmb, SPORES_TYPE);
	}

	// в инвентарь трупа, а если места нет — рядом на землю
	static void VB_DropServer(ZombieBase zmb, string type)
	{
		EntityAI item = zmb.GetInventory().CreateInInventory(type);
		if (!item)
			g_Game.CreateObjectEx(type, zmb.GetPosition(), ECE_PLACE_ON_SURFACE);
	}

	// клиент; justDied = false, если труп просто подгрузился
	static void VB_OnDeathClient(ZombieBase zmb, bool justDied)
	{
		int idx = zmb.GetHiddenSelectionIndex("camo");
		string dead = zmb.ConfigGetString("vbRiftDeadMaterial");
		if (idx >= 0 && dead != "")
			zmb.SetObjectMaterial(idx, dead);

		if (!justDied)
			return;

		vector pos = zmb.GetPosition() + Vector(0, BURST_HEIGHT, 0);
		ParticleManager pm = ParticleManager.GetInstance();
		if (pm)
			pm.PlayInWorld(ParticleList.VB_RIFT_BURST, pos);

		EffectSound snd = SEffectManager.PlaySound(BURST_SOUND, pos);
		if (snd)
			snd.SetAutodestroy(true);
	}
}

modded class ZombieBase
{
	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);

		if (g_Game.IsServer() && VB_RiftZombie.VB_Is(this))
			VB_RiftZombie.VB_OnKilledServer(this);
	}

	override void OnDamageDestroyed(int oldLevel)
	{
		super.OnDamageDestroyed(oldLevel);

		if (!g_Game.IsDedicatedServer() && VB_RiftZombie.VB_Is(this))
			VB_RiftZombie.VB_OnDeathClient(this, oldLevel != -1);
	}
}
