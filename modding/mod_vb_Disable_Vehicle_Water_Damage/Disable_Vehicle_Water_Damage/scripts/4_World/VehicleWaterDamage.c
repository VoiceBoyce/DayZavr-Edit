// Защита транспорта от воды.
// Ванильный OnUpdate (сервер) копит m_DrownTime, пока двигатель под водой, и после DROWN_ENGINE_THRESHOLD (0.5 с)
// снимает двигателю DROWN_ENGINE_DAMAGE в секунду (машина 350, мотоцикл 150) — транспорт руинится.
// Счётчик держим глубоко в минусе, порог не достигается никогда. Остальное в OnUpdate работает как в ванили.
// Лодки (BoatScript) от воды не повреждаются, их не трогаем.

modded class CarScript
{
	override void OnUpdate(float dt)
	{
		m_DrownTime = -1000;
		super.OnUpdate(dt);
	}
}

modded class MotorbikeScript
{
	override void OnUpdate(float dt)
	{
		m_DrownTime = -1000;
		super.OnUpdate(dt);
	}
}
