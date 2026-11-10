// Номера RPC мода. Старые 8832135611 и подобные не помещались в int и обрезались.
class AIB_RPC
{
	static const int WEAPON_FIRE = 2088101;			// выстрел бота: клиент проигрывает звук и вспышку
	static const int BULLET_FLYBY = 2088102;		// промах: свист пули у игрока
	static const int WEAPON_VISIBILITY = 2088103;	// спрятать/показать оружие при броске гранаты

	static const float SOUND_RANGE = 3000.0;		// дальше выстрел бота не слышно (как дальний звук ванильных винтовок)
}
