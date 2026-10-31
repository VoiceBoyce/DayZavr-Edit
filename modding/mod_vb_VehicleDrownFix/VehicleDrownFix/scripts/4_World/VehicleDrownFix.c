// Исправление затопления двигателя в DayZ 1.30 (серверный мод).
// В ванили точка двигателя m_DrownEnginePos задаётся только в InitializeParticles(),
// а та на выделенном сервере сразу выходит. Позиция остаётся 0 0 0 (центр модели у днища),
// и сервер считает двигатель утонувшим даже на мели. Плюс переменная static, общая на все машины.
// Здесь у каждой машины своя точка, вычисляется при первом обращении.

modded class CarScript
{
	protected vector m_DZVR_DrownPos;
	protected bool m_DZVR_DrownPosReady;

	override vector GetEnginePosWS()
	{
		if (!m_DZVR_DrownPosReady)
		{
			m_DZVR_DrownPosReady = true;
			if (MemoryPointExists("drown_engine"))
				m_DZVR_DrownPos = GetMemoryPointPos("drown_engine");
			else if (MemoryPointExists("dmgZone_engine"))
				m_DZVR_DrownPos = GetMemoryPointPos("dmgZone_engine");
		}
		return ModelToWorld(m_DZVR_DrownPos);
	}
}

modded class MotorbikeScript
{
	protected vector m_DZVR_DrownPos;
	protected bool m_DZVR_DrownPosReady;

	override vector GetEnginePosWS()
	{
		if (!m_DZVR_DrownPosReady)
		{
			m_DZVR_DrownPosReady = true;
			if (MemoryPointExists("engine"))
				m_DZVR_DrownPos = GetMemoryPointPos("engine");
		}
		return ModelToWorld(m_DZVR_DrownPos);
	}
}
