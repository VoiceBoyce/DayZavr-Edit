modded class PluginManager {
    override void Init()
    {
        super.Init();
        RegisterPlugin("LaserDesignatorPlugin",     true,  true);
    }
}

LaserDesignatorPlugin GetLaserDesignatorPlugin()
{
	return LaserDesignatorPlugin.Cast(GetPluginManager().GetPluginByType(LaserDesignatorPlugin));
}
class LaserDesignatorPlugin : PluginBase
{
    bool WorkNonRaised = false;
    bool ScreenCenterMode = true;

    ref array<ref LaserDesignatorConfig> LaserSettings = {};

    void LaserDesignatorPlugin()
    {
        Print("LaserDesignatorPlugin");
        Load();
    }
    void Load()
    {
        if(GetGame().IsDedicatedServer())
        {
		    if (!FileExist("$profile:LaserDesignator"))
		    	MakeDirectory("$profile:LaserDesignator");

		    if (!FileExist("$profile:LaserDesignator/Config.json"))
		    {
                Example();
                Comment();
		    }
		    else
		    {
		    	string error;
		    	if (!JsonFileLoader<LaserDesignatorPlugin>.LoadFile("$profile:LaserDesignator/Config.json", this, error))
		    		Print("[Ri_LaserDesignator] Ошибка в Config.json: " + error);
		    }
        }
    }
    void  Example()
    {
	    for ( int i = 0; i < GetGame().ConfigGetChildrenCount("cfgVehicles"); i++ )
	    {
	    	string child_name;
	    	GetGame().ConfigGetChildName( "cfgVehicles", i, child_name );
            if(GetGame().IsKindOf( child_name, "Ri_LaserDesignator_Base") && child_name!="Ri_LaserDesignator_Base")
            {
                LaserDesignatorConfig lcu = new LaserDesignatorConfig;
                lcu.classname = child_name;
                lcu.alpha = 1.0;
                lcu.red = Math.RandomFloat(0,1.0);
                lcu.green = Math.RandomFloat(0,1.0);
                lcu.blue = Math.RandomFloat(0,1.0);
                array<string> dot = {"R","G","B","W"};
                lcu.dotptc = dot[Math.RandomInt(0,4)];
                LaserSettings.Insert(lcu);
            }
        }
		string error;
		if (!JsonFileLoader<LaserDesignatorPlugin>.SaveFile("$profile:LaserDesignator/Config.json", this, error))
			Print("[Ri_LaserDesignator] Не удалось сохранить Config.json: " + error);
    }

    void  Comment()
    {
        array<string> comm =
        {
            " ",
            "(First start its full random settings for laser colors. File readme created only one time.)",
            " ",
            "WorkNonRaised - mode of the laser, in case of 1, the laser works always",
            "when it is in the player's hands. In case of 0, the laser works only when the",
            "weapon is raised.",
            " ",
            "ScreenCenterMode - vanilla shooting mechanics implies a very casual way of",
            "shooting - in the center of the player's screen, although it can work",
            "realistically enough along the barrel of the weapon, but in this case, when",
            "shooting at close range, the accuracy of the hit is shifted from the center of",
            "the sight on the screen.",
            "In case 1, the laser tends to match the hit point in the center of the screen.",
            "In the case of 0, the laser corresponds to an accurate shot from the barrel of the",
            "weapon, but may not correspond to the final point of impact. (c) because Bohemia.",
            "~1: maybe some strange...",
            " ",
            "addition of laser colors.",
            "dotptc = particle that appears at the end of the laser, available R, G, B, W. ",
            "In case alpha < 1 - the laser beam may not be visible in optical sights.",
            "In cases where alpha<0.5 the beam may not be visible in NVGs.",
            "In cases where alpha or all colors == 0, the beam is no longer visible at all,",
            "but still works, AND IN THAT case, the dot at the end of the beam still works,",
            "which allows the beam to be hidden completely without loss of performance.",
            " ",
            "Button for change mode: L by default, in settings controls > combat.",
            " ",
            "MAX RANGE = ~100m. if longer - game broke lods loading and incorrect create beam for another players.",
            " ",
            "by @Rico_Magic"
        };

		if (!FileExist("$profile:\\LaserDesignator\\readme.txt"))
        {
		    FileHandle file = OpenFile("$profile:\\LaserDesignator\\readme.txt", FileMode.WRITE);
		    CloseFile(file);
		    file = OpenFile("$profile:\\LaserDesignator\\readme.txt", FileMode.APPEND);
            for(int i = 0 ; i < comm.Count(); i++)
            {
            FPrintln(file, comm.Get(i));
            }
		    CloseFile(file);
        }

    }
}
class LaserDesignatorConfig
{
    string classname;
    float  alpha;
    float  red;
    float  green;
    float  blue;
    string dotptc;
}
