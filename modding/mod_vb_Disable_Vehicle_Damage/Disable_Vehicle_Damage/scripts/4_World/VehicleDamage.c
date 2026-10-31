// Отключение урона транспорта (машины и мотоциклы).
//
// ДТП: ванильный Transport.OnContact (сервер) копит удары, по ним CheckContactCache наносит урон зонам транспорта,
// экипажу и играет звук аварии. Пустой OnContact — ударов нет, урона нет.
// Сбитые игроки, заражённые и животные урон получают как обычно (это считается на их стороне).
//
// Вода: ванильный OnUpdate (сервер) копит m_DrownTime, пока двигатель под водой, и после DROWN_ENGINE_THRESHOLD
// (0.5 с) снимает двигателю DROWN_ENGINE_DAMAGE в секунду (машина 350, мотоцикл 150) — транспорт руинится.
// Счётчик держим глубоко в минусе, порог не достигается никогда. Остальное в OnUpdate работает как в ванили.
//
// Лодки (BoatScript) не трогаем.

modded class CarScript
{
	override void OnContact(string zoneName, vector localPos, IEntity other, Contact data)
	{
	}

	override void OnUpdate(float dt)
	{
		m_DrownTime = -1000;
		super.OnUpdate(dt);
	}
}

modded class MotorbikeScript
{
	override void OnContact(string zoneName, vector localPos, IEntity other, Contact data)
	{
	}

	override void OnUpdate(float dt)
	{
		m_DrownTime = -1000;
		super.OnUpdate(dt);
	}
}
