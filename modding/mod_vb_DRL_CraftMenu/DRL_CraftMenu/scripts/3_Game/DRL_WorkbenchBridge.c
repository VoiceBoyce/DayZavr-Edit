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