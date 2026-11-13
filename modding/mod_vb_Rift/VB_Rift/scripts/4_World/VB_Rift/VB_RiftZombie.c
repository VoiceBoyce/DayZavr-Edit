// Зомби Изнанки (в конфиге vbRiftZombie = 1, классы VB_RiftZmb*).
// Смерть: глаза и жилы гаснут (материал vbRiftDeadMaterial без свечения), тело лопается облаком красных спор
// с мокрым хлопком. Труп, который игрок застал уже мёртвым (подгрузился позже), просто тёмный — без облака и звука.
// Лут — разделкой, как у мутантов (решение пользователя 2026-11-13): мёртвого зомби Изнанки можно «Разделать»
// ножом (ванильное ActionSkinning), с шансом выпадают «Корень Изнанки» и «Споры Изнанки» — на землю у ног.
// Здесь только разрешение разделки (CanBeSkinned нужно обеим сторонам: условие действия проверяют и клиент,
// и сервер); что выпадает, решает сервер — VB_Rift_Server, VB_RiftWorldZombie.c (modded ActionSkinning.SpawnItems).

class VB_RiftZombie
{
	static const string BURST_SOUND = "VB_RiftBurst_SoundSet";
	static const float BURST_HEIGHT = 1.1;	// примерно грудь, м

	static bool VB_Is(EntityAI zmb)
	{
		return zmb && zmb.ConfigGetBool("vbRiftZombie");
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
	// мёртвого зомби Изнанки разделывают ножом, как животных (ванильное «Разделать», ActionSkinning:
	// нужен CanBeSkinned и не живой). Ванильные зомби (CanBeSkinned = false) не меняются
	override bool CanBeSkinned()
	{
		if (VB_RiftZombie.VB_Is(this))
			return true;
		return super.CanBeSkinned();
	}

	override void OnDamageDestroyed(int oldLevel)
	{
		super.OnDamageDestroyed(oldLevel);

		if (!g_Game.IsDedicatedServer() && VB_RiftZombie.VB_Is(this))
			VB_RiftZombie.VB_OnDeathClient(this, oldLevel != -1);
	}
}
