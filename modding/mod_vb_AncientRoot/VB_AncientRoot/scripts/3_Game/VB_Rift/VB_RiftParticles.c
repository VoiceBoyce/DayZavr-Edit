// Частицы Изнанки. Файлы .ptc — текст, лежат в VB_Rift/particles/.
modded class ParticleList
{
	// светящиеся споры поднимаются от сердца + тусклое красное свечение у земли (только ночью, у ближайших сердец)
	static const int VB_RIFT_SPORES = RegisterParticle("VB_Rift/particles/", "vb_rift_spores");
	// зомби Изнанки лопается при смерти: облако красных спор, багровая дымка и короткая вспышка (один раз)
	static const int VB_RIFT_BURST = RegisterParticle("VB_Rift/particles/", "vb_rift_burst");}
