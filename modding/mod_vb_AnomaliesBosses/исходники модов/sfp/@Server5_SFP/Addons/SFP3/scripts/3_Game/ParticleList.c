// Register all particles below!

// Example how to register particles from a mod
/*
modded class ParticleList
{
	static const int MODDED_PARTICLE = RegisterParticle( "mod_folder/" , "my_modded_particle");SFP_MONOLIT_stone shum_tele_up_musor
}
*/

modded class ParticleList
{
	
	static const int SFP_FIRE01_GRILL = RegisterParticle( "SFP3/grafen/prtkl/" , "fire01_grill");
	static const int SFP_INVZ_START = RegisterParticle( "SFP3/grafen/prtkl/" , "sfp_st_anom01");
	static const int SFP_PSEDOG_CLONED = RegisterParticle( "SFP3/grafen/prtkl/" , "sfp_st_psedog_clon");
	static const int SFP_METAL_SPARKS = RegisterParticle( "SFP3/grafen/prtkl/" , "sfp_crash_metal_anom");
	static const int SFP_TELE_UP_TRASH_PRT = RegisterParticle( "SFP3/grafen/prtkl/" , "shum_tele_up_musor");
}