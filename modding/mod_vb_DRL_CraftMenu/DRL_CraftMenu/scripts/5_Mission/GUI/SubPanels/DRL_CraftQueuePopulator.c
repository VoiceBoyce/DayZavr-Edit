class DRL_CraftQueuePopulator
{
    static const int RECIPES_PER_FRAME = 2;
    static const float TICK_RATE = 0.016666;

    protected DRL_CraftMenuUI m_Menu;
    protected ref Timer m_Timer;
    protected ref array<DRL_Craft_Recipe> m_Queue;
    protected ref array<EntityAI> m_CachedInventory;
    protected int m_CurrentIndex;

    void DRL_CraftQueuePopulator(DRL_CraftMenuUI menu)
    {
        m_Menu = menu;
        m_Queue = new array<DRL_Craft_Recipe>();
        m_CachedInventory = new array<EntityAI>();
        m_CurrentIndex = 0;
    }

    void ~DRL_CraftQueuePopulator()
    {
        Stop();
    }

    void Start(array<DRL_Craft_Recipe> recipes, array<EntityAI> cachedInventory)
    {
        Stop();

        if (!recipes || recipes.Count() == 0 || !m_Menu)
        {
            return;
        }

        m_CurrentIndex = 0;
        m_Queue.Clear();
        for (int i = 0; i < recipes.Count(); i++)
        {
            m_Queue.Insert(recipes[i]);
        }

        m_CachedInventory.Clear();
        if (cachedInventory)
        {
            for (int k = 0; k < cachedInventory.Count(); k++)
            {
                m_CachedInventory.Insert(cachedInventory[k]);
            }
        }

        if (!m_Timer)
        {
            m_Timer = new Timer(CALL_CATEGORY_GUI);
        }

        m_Timer.Run(TICK_RATE, this, "OnTick", null, true);
    }

    void Stop()
    {
        if (m_Timer)
        {
            m_Timer.Stop();
        }

        if (m_Queue)
        {
            m_Queue.Clear();
        }
        m_CurrentIndex = 0;
    }

    bool IsRunning()
    {
        if (m_Timer)
        {
            return m_Timer.IsRunning();
        }
        return false;
    }

    protected void OnTick()
    {
        if (!m_Menu || !m_Queue || m_CurrentIndex >= m_Queue.Count())
        {
            Stop();
            if (m_Menu)
            {
                m_Menu.OnRecipePopulationFinished();
            }
            return;
        }

        int countThisFrame = 0;
        while (m_CurrentIndex < m_Queue.Count() && countThisFrame < RECIPES_PER_FRAME)
        {
            DRL_Craft_Recipe r = m_Queue[m_CurrentIndex];
            m_CurrentIndex++;

            if (r)
            {
                m_Menu.CreateRecipeRowAsync(r, m_CachedInventory);
            }

            countThisFrame++;
        }

        if (m_CurrentIndex >= m_Queue.Count())
        {
            Stop();
            if (m_Menu)
            {
                m_Menu.OnRecipePopulationFinished();
            }
        }
    }
}
