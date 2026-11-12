/*class SFP_Game_VolkKura : House
{
    private int m_TextureIndex = 0;
    private autoptr array<string> m_TexturePaths;
    private autoptr array<string> m_MaterialPaths;
    private string m_SelectionName = "display";
    
    void SFP_Game_VolkKura()
    {
        // Инициализация списка текстур
        m_TexturePaths = new array<string>;
        m_TexturePaths.Insert("SFP2\\1_item\\portable_games\\ussrVolk\\data\\gif\\sfp_animated_display_volk01.paa");
		m_TexturePaths.Insert("SFP2\\1_item\\portable_games\\ussrVolk\\data\\gif\\sfp_animated_display_volk02.paa");
		m_TexturePaths.Insert("SFP2\\1_item\\portable_games\\ussrVolk\\data\\gif\\sfp_animated_display_volk03.paa");
		m_TexturePaths.Insert("SFP2\\1_item\\portable_games\\ussrVolk\\data\\gif\\sfp_animated_display_volk04.paa");
		m_TexturePaths.Insert("SFP2\\1_item\\portable_games\\ussrVolk\\data\\gif\\sfp_animated_display_volk05.paa");
		m_TexturePaths.Insert("SFP2\\1_item\\portable_games\\ussrVolk\\data\\gif\\sfp_animated_display_volk06.paa");
		m_TexturePaths.Insert("SFP2\\1_item\\portable_games\\ussrVolk\\data\\gif\\sfp_animated_display_volk07.paa");
		m_TexturePaths.Insert("SFP2\\1_item\\portable_games\\ussrVolk\\data\\gif\\sfp_animated_display_volk08.paa");
		m_TexturePaths.Insert("SFP2\\1_item\\portable_games\\ussrVolk\\data\\gif\\sfp_animated_display_volk09.paa");
		m_TexturePaths.Insert("SFP2\\1_item\\portable_games\\ussrVolk\\data\\gif\\sfp_animated_display_volk10.paa");
		m_TexturePaths.Insert("SFP2\\1_item\\portable_games\\ussrVolk\\data\\gif\\sfp_animated_display_volk11.paa");
		m_TexturePaths.Insert("SFP2\\1_item\\portable_games\\ussrVolk\\data\\gif\\sfp_animated_display_volk12.paa");
       
		//----------------------------------------------------------------------------------------------------------------
        // Инициализация списка материалов (оставьте пустой, если хотите использовать стандартный) 
        m_MaterialPaths = new array<string>;
        //m_MaterialPaths.Insert("path\\to\\material1.rvmat");
        m_MaterialPaths.Insert("");  // Пустая строка для стандартного материала
        //m_MaterialPaths.Insert("path\\to\\material3.rvmat");
        
        // Установить начальные текстуру и материал
        SetObjectTexture(GetHiddenSelectionIndex(m_SelectionName), m_TexturePaths.Get(m_TextureIndex));
        SetObjectMaterial(GetHiddenSelectionIndex(m_SelectionName), m_MaterialPaths.Get(m_TextureIndex));
        
        // Запуск таймера
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(ChangeTexture, 407, true);
    }
    
    void ChangeTexture()
    {
        // Сменить текстуру
        SetObjectTexture(GetHiddenSelectionIndex(m_SelectionName), m_TexturePaths.Get(m_TextureIndex));
        
        // Сменить материал, если путь указан
		//Здесь проверяется, если путь к материалу в m_MaterialPaths не пустой, то устанавливается новый материал. В противном случае, материал не изменяется 
        if (m_MaterialPaths.Get(m_TextureIndex) != "")
        {
            SetObjectMaterial(GetHiddenSelectionIndex(m_SelectionName), m_MaterialPaths.Get(m_TextureIndex));
        }
        
        // Обновить индекс для следующей текстуры и материала
        m_TextureIndex++;
        
        // Зациклить списки текстур и материалов, если достигнут конец
        if (m_TextureIndex >= m_TexturePaths.Count())
        {
            m_TextureIndex = 0;
        }
    }
}

*/