//	АБВ
//	Конфиги зон. Читаются один раз при старте сервера и сливаются в один список зон
string	VD_RESPAWNED_ANIMALS_DAY_CONFIGS	=	"$profile:VD_SpavnMutant//CONFIGS//Dnevnoi.txt";
string	VD_RESPAWNED_ANIMALS_NIGHT_CONFIGS	=	"$profile:VD_SpavnMutant//CONFIGS//Nochnoy.txt";
//	Как часто проверять смену дня и ночи, в тиках (1 тик = 2 с)
int		VD_RESPAWNED_ANIMALS_CHANGE_TIMER	=	15;
//	За сколько шагов по 100 мс проходится очередь зон (30 = 3 с)
int		VD_ANIMALS_PASS_STEPS				=	30;
//	Ближе этой дистанции к игроку существо не появляется, м
float	VD_SPAWN_MIN_PLAYER_DISTANCE		=	50.0;
//	Сколько точек пробовать при спавне и через сколько секунд повторить, если все не подошли
int		VD_SPAWN_ATTEMPTS					=	8;
int		VD_SPAWN_RETRY_TIME					=	15;
//	Существо не удаляется, пока игрок ближе этой дистанции к нему самому, м
float	VD_DESPAWN_SAFE_DISTANCE			=	150.0;
//	Сообщения в чат при смене дня и ночи
string	VD_MESSAGE_DAY						=	"Над землёй взошло солнце - монстры уходят в норы. Ты можешь перевести дух, но не расслабляйся.";
string	VD_MESSAGE_NIGHT					=	"На землю опустилась тьма - монстры выходят на охоту. Слышишь шорох? Это не ветер. Это они. Не стой на месте.";
