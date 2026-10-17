class DRL_CraftSoundHelper
{
    static const string SOUND_DEFAULT = "DRLCraft_SoundSet";
    static const string SOUND_FOOD    = "DRLCraft2_SoundSet";

    static ref TStringArray s_AllSoundSets;

    static void InitSoundRegistry()
    {
        if (s_AllSoundSets)
        {
            return;
        }

        s_AllSoundSets = new TStringArray();
        s_AllSoundSets.Insert("DRLCraft_SoundSet");
        s_AllSoundSets.Insert("DRLCraft2_SoundSet");
        s_AllSoundSets.Insert("DRLCraft_Avto_SoundSet");
        s_AllSoundSets.Insert("DRLCraft_Box_SoundSet");
        s_AllSoundSets.Insert("DRLCraft_Clothing_SoundSet");
        s_AllSoundSets.Insert("DRLCraft_Sewing_SoundSet");
        s_AllSoundSets.Insert("DRLCraft_Scissors_SoundSet");
        s_AllSoundSets.Insert("DRLCraft_Food_SoundSet");
        s_AllSoundSets.Insert("DRLCraft_Grill_SoundSet");
        s_AllSoundSets.Insert("DRLCraft_WaterMix_SoundSet");
        s_AllSoundSets.Insert("DRLCraft_Furniture_SoundSet");
        s_AllSoundSets.Insert("DRLCraft_Glass_SoundSet");
        s_AllSoundSets.Insert("DRLCraft_Metal_SoundSet");
        s_AllSoundSets.Insert("DRLCraft_MetalSawing_SoundSet");
        s_AllSoundSets.Insert("DRLCraft_Welding_SoundSet");
        s_AllSoundSets.Insert("DRLCraft_Forge_SoundSet");
        s_AllSoundSets.Insert("DRLCraft_HammerWood_SoundSet");
        s_AllSoundSets.Insert("DRLCraft_Saw_SoundSet");
        s_AllSoundSets.Insert("DRLCraft_Wood_SoundSet");
        s_AllSoundSets.Insert("DRLCraft_Paper_SoundSet");
        s_AllSoundSets.Insert("DRLCraft_Pencil_SoundSet");
        s_AllSoundSets.Insert("DRLCraft_Rubber_SoundSet");
        s_AllSoundSets.Insert("DRLCraft_Repair_SoundSet");
        s_AllSoundSets.Insert("DRLCraft_Vybor_SoundSet");
        s_AllSoundSets.Insert("DRLCraft_Generic1_SoundSet");
        s_AllSoundSets.Insert("DRLCraft_Generic2_SoundSet");
        s_AllSoundSets.Insert("DRLCraft_Generic3_SoundSet");
        s_AllSoundSets.Insert("DRLCraft_Generic4_SoundSet");
        s_AllSoundSets.Insert("DRLCraft_Generic5_SoundSet");
        s_AllSoundSets.Insert("DRLCraft_Generic6_SoundSet");
        s_AllSoundSets.Insert("DRLCraft_Generic7_SoundSet");
    }

    static int GetSoundId(string soundSetName)
    {
        if (soundSetName == "")
        {
            return 0;
        }

        InitSoundRegistry();
        int idx = s_AllSoundSets.Find(soundSetName);
        if (idx != -1)
        {
            return idx + 1;
        }
        return 1;
    }

    static string GetSoundSet(int soundId)
    {
        if (soundId <= 0)
        {
            return "";
        }

        InitSoundRegistry();
        int idx = soundId - 1;
        if (idx >= 0 && idx < s_AllSoundSets.Count())
        {
            return s_AllSoundSets.Get(idx);
        }
        return SOUND_DEFAULT;
    }

    static string GetSoundSetForRecipe(DRL_Craft_Recipe r)
    {
        DRL_Craft_Config cfg = GetCraft_Config();
        if (!cfg || !cfg.Settings)
        {
            return SOUND_DEFAULT;
        }

        string snd = cfg.Settings.CraftSoundSet;
        if (snd == "")
        {
            snd = SOUND_DEFAULT;
        }

        if (r && cfg.Categories)
        {
            for (int i = 0; i < cfg.Categories.Count(); i++)
            {
                DRL_Craft_Category cat = cfg.Categories.Get(i);
                if (cat && cat.ShowAll != 1 && cat.List && cat.List.Find(r.RecipeId) > -1)
                {
                    if (cat.CategoryCraftSoundSet != "")
                    {
                        snd = cat.CategoryCraftSoundSet;
                    }
                    break;
                }
            }
        }

        return snd;
    }
}
