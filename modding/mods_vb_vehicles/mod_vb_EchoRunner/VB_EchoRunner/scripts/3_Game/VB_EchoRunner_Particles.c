// Echo Runner, этап 8: частицы антиграв-модулей (VB_EchoRunner/particles/vb_er_grav.ptc).
// Излучатели по порядку: 0 core — красный луч вниз из излучателя, 1 trail — шлейф в движении (частота от скорости),
// 2 rings — расходящиеся кольца «магнитной подушки», 3 haze — дрожащая дымка под модулем, 4 arcs — искры поля.
// Управляет VB_EchoRunnerFx (4_World): создаёт по источнику на модуль в полёте и задаёт частоту рождения излучателей.

modded class ParticleList
{
	static const int VB_ER_GRAV = RegisterParticle("VB_EchoRunner/particles/", "vb_er_grav");
}
