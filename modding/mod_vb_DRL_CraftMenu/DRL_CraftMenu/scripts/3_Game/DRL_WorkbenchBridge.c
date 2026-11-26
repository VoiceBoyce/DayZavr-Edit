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
class DRL_RankBridge
{
	protected static int s_Level;
	protected static ref array<string> s_Names = new array<string>();
	static ref ScriptInvoker s_OnChanged = new ScriptInvoker();		// ступень сменилась (открытое окно перерисовывает)

	static void Set(int level, array<string> names)
	{
		bool changed = level != s_Level;
		s_Level = level;
		SetNames(names);
		if (changed && s_OnChanged)
			s_OnChanged.Invoke();
	}

	protected static void SetNames(array<string> names)
	{
		if (!s_Names)
			s_Names = new array<string>();
		s_Names.Clear();
		if (names)
		{
			foreach (string n : names)
			{
				s_Names.Insert(n);
			}
		}
	}

	static int Level()
	{
		return s_Level;
	}

	static bool Meets(DRL_Craft_Recipe r)
	{
		if (!r || r.RequiredRank <= 0)
			return true;
		return s_Level >= r.RequiredRank;
	}

	// Название ступени для окна, уже переведённое: «Боец»; нет названия — «ступень 5»; 0 — «нет ранга».
	static string Title(int level)
	{
		if (level <= 0)
			return Widget.TranslateString("#STR_DRL_RANK_NONE");
		int i = level - 1;
		if (s_Names && i < s_Names.Count() && s_Names[i] != "")
		{
			string n = s_Names[i];
			if (n.IndexOf("#") == 0)
				return Widget.TranslateString(n);
			return n;
		}
		string s = Widget.TranslateString("#STR_DRL_RANK_STEP");
		s.Replace("%1", level.ToString());
		return s;
	}
}