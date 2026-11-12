class SFP_TestRorshak_mask2 extends Mask_Base
{
    override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
	private int m_TextureIndex = 0;
    private autoptr array<string> m_TexturePaths;
    private autoptr array<string> m_MaterialPaths;
    private string m_SelectionName = "FIDOvCAMO";
    
    void SFP_TestRorshak_mask2()
    {
        // Инициализация списка текстур
        m_TexturePaths = new array<string>;
        m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_000_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_001_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_002_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_003_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_004_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_005_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_006_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_007_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_008_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_009_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_010_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_011_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_012_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_013_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_014_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_015_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_016_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_017_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_018_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_019_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_020_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_021_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_022_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_023_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_024_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_025_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_026_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_027_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_028_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_029_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_030_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_031_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_032_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_033_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_034_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_035_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_036_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_037_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_038_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_039_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_040_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_041_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_042_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_043_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_044_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_045_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_046_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_047_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_048_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_049_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_050_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_051_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_052_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_053_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_054_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_055_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_056_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_057_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_058_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_059_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_060_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_061_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_062_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_063_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_064_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_065_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_066_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_067_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_068_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_069_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_070_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_071_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_072_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_073_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_074_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_075_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_076_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_077_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_078_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_079_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_080_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_081_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_082_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_083_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_084_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_085_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_086_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_087_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_088_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_089_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_090_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_091_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_092_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_093_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_094_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_095_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_096_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_097_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_098_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_099_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_100_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_101_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_102_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_103_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_104_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_105_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_106_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_107_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_108_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_109_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_110_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_111_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_112_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_113_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_114_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_115_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_116_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_117_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_118_.paa");
		m_TexturePaths.Insert("SFP2\\1_smot\\TestRorshak\\data\\TestRor2\\TestRorh2_119_.paa");
		
		
		
		
       

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
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(ChangeTexture, 105, true);
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

