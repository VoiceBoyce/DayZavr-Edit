class DRL_WorkbenchBG
{
    protected static EntityAI s_LastWorkbench;

    static void Set(EntityAI wb)
	{
		s_LastWorkbench = wb;
	}
    static EntityAI Take()
    {
        EntityAI tmp = s_LastWorkbench;
        s_LastWorkbench = null;
        return tmp;
    }
}

class DRL_LearningBridge
{
	protected static ref array<int> s_LearnedRecipeIds = new array<int>();
	static ref ScriptInvoker s_OnStudyResponse = new ScriptInvoker();

	static void SetClientLearnedRecipes(array<int> ids)
	{
		if (!s_LearnedRecipeIds)
		{
			s_LearnedRecipeIds = new array<int>();
		}
		s_LearnedRecipeIds.Clear();
		if (ids)
		{
			for (int i = 0; i < ids.Count(); i++)
			{
				s_LearnedRecipeIds.Insert(ids[i]);
			}
		}
	}

	static bool HasLearnedRecipe(int recipeId)
	{
		if (!s_LearnedRecipeIds)
		{
			return false;
		}
		return (s_LearnedRecipeIds.Find(recipeId) != -1);
	}

	static void AddClientLearnedRecipe(int recipeId)
	{
		if (!s_LearnedRecipeIds)
		{
			s_LearnedRecipeIds = new array<int>();
		}
		if (s_LearnedRecipeIds.Find(recipeId) == -1)
		{
			s_LearnedRecipeIds.Insert(recipeId);
		}
	}

	static array<int> GetLearnedRecipeIds()
	{
		return s_LearnedRecipeIds;
	}
}

// Ранг игрока для рецептов с RequiredRank (клиент, 2026-11-26). Сервер присылает ступень и названия ступеней
// (STC_SYNC_RANK: при входе, при открытии станции, при отказе в крафте); их даёт дополнение с системой рангов.
// Окно только показывает, решает сервер. Ступень 0 — рангов нет (или ещё не пришло).
// Глобальные переменные и функции, как g_Craft_Config / GetCraft_Config: статический класс-мост, который окно
// вызывало на каждом щелчке, портил память клиента (стенд 2026-11-26, бисекция B1–B10).
int g_DRL_RankLevel;
ref array<string> g_DRL_RankNames;

void DRL_RankSet(int level, array<string> names)
{
	g_DRL_RankLevel = level;
	if (!g_DRL_RankNames)
		g_DRL_RankNames = new array<string>();
	g_DRL_RankNames.Clear();
	if (!names)
		return;
	for (int i = 0; i < names.Count(); i++)
	{
		g_DRL_RankNames.Insert(names[i]);
	}
}

int DRL_RankLevel()
{
	return g_DRL_RankLevel;
}

// рецепт доступен по рангу: передаётся ЧИСЛО r.RequiredRank, не сам рецепт
bool DRL_RankMeets(int requiredRank)
{
	if (requiredRank <= 0)
		return true;
	return g_DRL_RankLevel >= requiredRank;
}

// Название ступени, уже переведённое: «Боец»; нет названия — «ступень 5»; 0 — «нет ранга».
string DRL_RankTitle(int level)
{
	string title = "";
	if (level <= 0)
	{
		title = Widget.TranslateString("#STR_DRL_RANK_NONE");
		return title;
	}
	int idx = level - 1;
	string named = "";
	if (g_DRL_RankNames && idx < g_DRL_RankNames.Count())
		named = g_DRL_RankNames[idx];
	if (named != "")
	{
		if (named.IndexOf("#") == 0)
			title = Widget.TranslateString(named);
		else
			title = named;
		return title;
	}
	title = Widget.TranslateString("#STR_DRL_RANK_STEP");
	title.Replace("%1", level.ToString());
	return title;
}

// «Нужен ранг: Боец»
string DRL_RankNeedLine(int requiredRank)
{
	string needName = DRL_RankTitle(requiredRank);
	string needLine = Widget.TranslateString("#STR_DRL_UI_RL_RANK");
	needLine.Replace("%1", needName);
	return needLine;
}

// «У вас: Выживший»
string DRL_RankYoursLine()
{
	string yoursName = DRL_RankTitle(g_DRL_RankLevel);
	string yoursLine = Widget.TranslateString("#STR_DRL_UI_RANK_YOURS");
	yoursLine.Replace("%1", yoursName);
	return yoursLine;
}

// Уведомление об отказе: «Шлем: нужен ранг «Боец». Свой ранг смотрите в планшете.»
string DRL_RankNotifText(string itemName, int requiredRank)
{
	string notifName = DRL_RankTitle(requiredRank);
	string notifLine = Widget.TranslateString("#STR_DRL_NOTIF_MSG_RANK");
	notifLine.Replace("%1", itemName);
	notifLine.Replace("%2", notifName);
	return notifLine;
}
