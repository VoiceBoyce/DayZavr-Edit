modded class DayZGame
{	
	protected ref StaticAIBConfig m_StaticAIBConfig;
	protected ref DynamicAIBConfig m_DynamicAIBConfig;
	
    void SetStaticAIBConfig(StaticAIBConfig config)
    {
        m_StaticAIBConfig = config;
    }

    StaticAIBConfig GetStaticAIBConfig()
    {
		return m_StaticAIBConfig;
    }
	
	void SetDynamicAIBConfig(DynamicAIBConfig config)
    {
        m_DynamicAIBConfig = config;
    }

    DynamicAIBConfig GetDynamicAIBConfig()
    {
		return m_DynamicAIBConfig;
    }
};