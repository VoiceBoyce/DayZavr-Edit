/**
 * SFP_Molotov
 *
 * Требования:
 * - Classname: SFP_Molotov (наследуется от Grenade_Base)
 * - При броске и первом контакте с поверхностью/объектом:
 *     - молотов удаляется (считаем, что "разбился")
 *     - на месте контакта создаётся SFP_MolotovFireArea (радиус 6 м, время жизни 24 сек)
 *
 * Примечания:
 * - Мы намеренно выключаем механику гранаты (чека/фитиль/активация), чтобы не срабатывала ванильная логика взрыва.
 * - Зона создаётся только на сервере; клиенты получают её через репликацию объекта.
 */

class SFP_Molotov : Grenade_Base
{
	protected bool m_WasThrown;
	protected bool m_HasTriggered;

	void SFP_Molotov()
	{
		// Отключаем гранатную механику
		SetGrenadeType(EGrenadeType.CHEMICAL);
		SetPinnable(false);
		m_Pinned = true; // держим чеку «вставленной», чтобы не срабатывала Activate() из базовой логики
		SetFuseDelay(999999); // не важно само по себе, но делает базовую логику инертной
	}

	override void OnInventoryExit(Man player)
	{
		super.OnInventoryExit(player);

		// Определяем, что предмет реально БРОСИЛИ (а не просто выбросили/уронили)
		PlayerBase pb = PlayerBase.Cast(player);
		if (pb && pb.GetThrowing() && pb.GetThrowing().IsThrowingAnimationPlaying())
			m_WasThrown = true;
		else
			m_WasThrown = false;
	}

	override void EOnContact(IEntity other, Contact extra)
	{
		super.EOnContact(other, extra);

		if (!m_WasThrown || m_HasTriggered)
			return;

		// Зону создаёт только сервер (клиенты получат её по репликации)
		if (GetGame().IsServer())
		{
			m_HasTriggered = true;
			SpawnFireArea(extra);
			DeleteSafe();
		}
	}

	override void EEKilled(Object killer)
	{
		// Подстраховка: не даём сработать взрывному поведению базового класса
		if (GetGame().IsServer() && !m_HasTriggered)
		{
			m_HasTriggered = true;
			SpawnFireArea(null);
			DeleteSafe();
		}
	}

	protected void SpawnFireArea(Contact extra)
	{
		vector pos;

		if (extra)
			pos = extra.Position;
		else
			pos = GetPosition();

		// Небольшой подъём, чтобы не заспавниться внутри геометрии/земли
		pos[1] = pos[1] + 0.05;

		GetGame().CreateObjectEx("SFP_MolotovFireArea", pos, ECE_PLACE_ON_SURFACE);
	}

	// --- Полностью отключаем активацию гранаты ---
	override protected void OnActivateFinished() {}
	override protected void OnActivateImmediate() {}
	override protected void Activate() {}
	override void Unpin() {}
	
	override void SetActions()
    {
        super.SetActions();

        // Возвращаем базовые Take/Drop, но оставляем молотов без гранатных pin/unpin действий.
        RemoveAction(ActionUnpin);
        RemoveAction(ActionPin);
    }
}
