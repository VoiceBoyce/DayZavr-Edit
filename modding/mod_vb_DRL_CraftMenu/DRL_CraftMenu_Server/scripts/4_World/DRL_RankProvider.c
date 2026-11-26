// Ранги для рецептов с RequiredRank (сервер, 2026-11-26).
// RequiredRank рецепта — ступень 1..N (0 — без ранга); игрок проходит, если его ступень не ниже.
// Свою систему рангов подключает дополнение через modded class DRL_RankProvider (VB_Tablet_DRL — ранги планшета).
// Без дополнения у всех ступень 0: рецепты с рангом закрыты (лучше закрыто, чем даром).
class DRL_RankProvider
{
	protected static ref DRL_RankProvider s_Inst;

	static DRL_RankProvider Get()
	{
		if (!s_Inst)
			s_Inst = new DRL_RankProvider();
		return s_Inst;
	}

	// ступень игрока 1..N; 0 — рангов нет
	int Level(PlayerBase player)
	{
		return 0;
	}

	// названия ступеней для окна по порядку: [0] — ступень 1; ключи #STR_ или текст
	void Names(array<string> names)
	{
	}
}
