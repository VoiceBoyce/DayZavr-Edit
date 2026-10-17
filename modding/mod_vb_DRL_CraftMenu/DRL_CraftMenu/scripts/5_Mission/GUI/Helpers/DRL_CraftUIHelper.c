class DRL_CraftUIHelper
{
    static const string SEARCH_DEFAULT_STR = "#STR_SEARCH_RECIPE";

    static string ResolveImagePath(string path)
    {
        if (path == "")
        {
            return "";
        }

        if (path.IndexOf("set:") == 0)
        {
            return path;
        }

        if (path.IndexOf("DRL_CraftMenu") != -1 || path.IndexOf("DRL_CraftMain") != -1)
        {
            int lastSlash = path.LastIndexOf("/");
            string fileName = path;
            if (lastSlash != -1)
            {
                fileName = path.Substring(lastSlash + 1, path.Length() - (lastSlash + 1));
            }

            int dotIdx = fileName.LastIndexOf(".");
            if (dotIdx != -1)
            {
                fileName = fileName.Substring(0, dotIdx);
            }

            string safeName = fileName;
            safeName.Replace("-", "_");

            return "set:drl_craft_" + safeName + " image:" + safeName;
        }

        return path;
    }

    static string CleanSearchQuery(string text, string defaultPlaceholder = SEARCH_DEFAULT_STR)
    {
        if (text == "")
        {
            return "";
        }

        string tr = Widget.TranslateString(defaultPlaceholder);
        if (tr != "" && text.IndexOf(tr) == 0)
        {
            if (text.Length() == tr.Length())
            {
                return "";
            }
            return text.Substring(tr.Length(), text.Length() - tr.Length());
        }

        array<string> defaultPhrases = new array<string>;
        defaultPhrases.Insert("#STR_search_recipe");
        defaultPhrases.Insert("#STR_SEARCH_RECIPE");
        defaultPhrases.Insert("STR_SEARCH_RECIPE");
        defaultPhrases.Insert("STR_search_recipe");
        defaultPhrases.Insert("Найти рецепт");
        defaultPhrases.Insert("Find recipe");
        defaultPhrases.Insert("Знайти рецепт");

        foreach (string phrase : defaultPhrases)
        {
            if (text == phrase)
            {
                return "";
            }
            if (text.IndexOf(phrase) == 0 && text.Length() > phrase.Length())
            {
                return text.Substring(phrase.Length(), text.Length() - phrase.Length());
            }
        }

        return text;
    }

    static bool IsSearchDefaultText(string text, string defaultPlaceholder = SEARCH_DEFAULT_STR)
    {
        if (text == "")
        {
            return true;
        }

        return (CleanSearchQuery(text, defaultPlaceholder) == "");
    }

    static string FormatTimeSecs(int t)
    {
        int mm = t / 60;
        int ss = t % 60;

        if (mm > 0 && ss > 0)
        {
            return string.Format("%1 #STR_DRL_MIN %2 #STR_DRL_SEC", mm, ss);
        }

        if (mm > 0)
        {
            return string.Format("%1 #STR_DRL_MIN", mm);
        }

        return string.Format("%1 #STR_DRL_SEC", ss);
    }

    static string CraftTimeLabel(int secs)
    {
        return string.Format("#STR_DRL_CRAFT_TIME  %1", FormatTimeSecs(secs));
    }

    static string FormatPercent(float frac)
    {
        if (frac < 0.0)
        {
            frac = 0.0;
        }
        if (frac > 1.0)
        {
            frac = 1.0;
        }
        int p = Math.Round(frac * 100.0);
        return string.Format("%1%%", p);
    }

    static string PercentText(float frac)
    {
        return FormatPercent(frac);
    }

    static string FormatStateFromDamageFrac(float dmgFrac)
    {
        if (dmgFrac <= 0.0)
        {
            return "#STR_DRL_UNTOUCHED";
        }
        if (dmgFrac < 0.25)
        {
            return "#STR_DRL_PRISTINE";
        }
        if (dmgFrac < 0.50)
        {
            return "#STR_DRL_WORN";
        }
        if (dmgFrac < 0.75)
        {
            return "#STR_DRL_DAMAGED";
        }
        return "#STR_DRL_BADLY_DAMAGED";
    }

    static string HealthLevelToText(int lvl)
    {
        switch (lvl)
        {
            case 0: return "#STR_DRL_PRISTINE";
            case 1: return "#STR_DRL_WORN";
            case 2: return "#STR_DRL_DAMAGED";
            case 3: return "#STR_DRL_BADLY_DAMAGED";
            case 4: return "#STR_DRL_RUINED";
        }
        return "#STR_DRL_ANY";
    }

    static string TextFromMinHealthFrac(int minHF)
    {
        switch (minHF)
        {
            case 0: return "#STR_DRL_PRISTINE";
            case 1: return "#STR_DRL_WORN";
            case 2: return "#STR_DRL_DAMAGED";
            case 3: return "#STR_DRL_BADLY_DAMAGED";
            case 4: return "#STR_DRL_RUINED";
        }
        return "#STR_DRL_ANY";
    }

    static bool GetDisplayNameForClass(string cls, out string outName)
    {
        outName = "";
        if (cls == "")
        {
            return false;
        }

        string path;

        path = "CfgVehicles " + cls + " displayName";
        if (GetGame().ConfigIsExisting(path))
        {
            GetGame().ConfigGetText(path, outName);
            if (outName != "")
            {
                return true;
            }
        }

        path = "CfgWeapons " + cls + " displayName";
        if (GetGame().ConfigIsExisting(path))
        {
            GetGame().ConfigGetText(path, outName);
            if (outName != "")
            {
                return true;
            }
        }

        path = "CfgMagazines " + cls + " displayName";
        if (GetGame().ConfigIsExisting(path))
        {
            GetGame().ConfigGetText(path, outName);
            if (outName != "")
            {
                return true;
            }
        }

        return false;
    }

    static bool GetDescriptionShortForClass(string cls, out string outDesc)
    {
        outDesc = "";
        if (cls == "")
        {
            return false;
        }

        string path;

        path = "CfgVehicles " + cls + " descriptionShort";
        if (GetGame().ConfigIsExisting(path))
        {
            GetGame().ConfigGetText(path, outDesc);
            if (outDesc != "")
            {
                return true;
            }
        }

        path = "CfgWeapons " + cls + " descriptionShort";
        if (GetGame().ConfigIsExisting(path))
        {
            GetGame().ConfigGetText(path, outDesc);
            if (outDesc != "")
            {
                return true;
            }
        }

        path = "CfgMagazines " + cls + " descriptionShort";
        if (GetGame().ConfigIsExisting(path))
        {
            GetGame().ConfigGetText(path, outDesc);
            if (outDesc != "")
            {
                return true;
            }
        }

        return false;
    }

    static void ClearChildren(Widget parent)
    {
        if (!parent)
        {
            return;
        }

        Widget c = parent.GetChildren();
        while (c)
        {
            Widget next = c.GetSibling();
            ClearChildPreviews(c);
            ClearChildUserData(c);
            c.Unlink();
            c = next;
        }
    }

    static void ClearChildUserData(Widget w)
    {
        if (!w)
        {
            return;
        }

        w.SetUserData(null);
        Widget child = w.GetChildren();
        while (child)
        {
            ClearChildUserData(child);
            child = child.GetSibling();
        }
    }

    static void ClearChildPreviews(Widget w)
    {
        if (!w)
        {
            return;
        }

        ItemPreviewWidget ipw = ItemPreviewWidget.Cast(w);
        if (ipw)
        {
            ipw.SetItem(null);
        }

        Widget child = w.GetChildren();
        while (child)
        {
            ClearChildPreviews(child);
            child = child.GetSibling();
        }
    }
}
