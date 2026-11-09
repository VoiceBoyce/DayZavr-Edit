// Матка пауков (Spider3): когда её ранят до 70% и до 30% здоровья, вокруг вылупляются малые пауки.
// Раньше малые пауки появлялись при свежевании туши (модификация ActionSkinning в клиентском PBO — убрана).
modded class MSFZ_Mutant_Spider3
{
	protected const int   MSFZ_BABY_COUNT  = 3;  // сколько малых пауков за раз
	protected const float MSFZ_BABY_RADIUS = 5;  // радиус появления, м

	protected bool m_MSFZ_Babies70;
	protected bool m_MSFZ_Babies30;

	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);

		if (!GetGame().IsServer() || !IsAlive())
			return;

		// Создание объектов переносим из обработчика урона на следующий кадр (CallLater 0):
		// создавать сущности прямо внутри EEHitBy небезопасно для движка.
		float health = GetHealth01("", "");
		if (!m_MSFZ_Babies70 && health <= 0.7)
		{
			m_MSFZ_Babies70 = true;
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(MSFZ_SpawnBabies, 0, false);
		}
		if (!m_MSFZ_Babies30 && health <= 0.3)
		{
			m_MSFZ_Babies30 = true;
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(MSFZ_SpawnBabies, 0, false);
		}
	}

	protected void MSFZ_SpawnBabies()
	{
		if (!GetGame() || IsSetForDeletion())
			return;

		vector center = GetPosition();
		// Под землёй (бункер, пещера) малыши появляются на высоте матки и ближе к ней, иначе их выносило на поверхность
		bool underground = center[1] < GetGame().SurfaceY(center[0], center[2]) - 3;
		float radius = MSFZ_BABY_RADIUS;
		if (underground)
			radius = 2;

		for (int i = 0; i < MSFZ_BABY_COUNT; i++)
		{
			float x = center[0] + Math.RandomFloat(-radius, radius);
			float z = center[2] + Math.RandomFloat(-radius, radius);
			float y = center[1];
			if (!underground)
				y = GetGame().SurfaceY(x, z);
			GetGame().CreateObject("MSFZ_Mutant_Spider1", Vector(x, y, z), false, true);
		}
	}
};
