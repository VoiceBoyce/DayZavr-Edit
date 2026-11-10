class AIBAmmoTypeInfo
{
    string MagType;
    string BulletType;
    int AmmoCount;

    void AIBAmmoTypeInfo(string mag, string bullet, int count)
    {
        MagType = mag;
        BulletType = bullet;
        AmmoCount = count;
    }
}

class AIBAmmoDamageDatabase
{
    static float GetDamageForBullet(string bulletType)
    {
        string path = "CfgAmmo " + bulletType + " DamageApplied Health damage";
        float ammoDamage = GetGame().ConfigGetFloat(path);
        if (ammoDamage > 0)
            return ammoDamage;
        return 0.2;
    }

    static void PrintAllBulletsAndDamages()
    {
        Print("==== SEZNAM BULLET + DAMAGE ====");
        Print("==== KONEC ====");
    }
}

class AIBAmmoTypeDatabase
{
   static string GetBulletForWeapon(string weaponType)
    {
        TStringArray magazineTypes = new TStringArray;
        GetGame().ConfigGetTextArray("CfgWeapons " + weaponType + " magazines", magazineTypes);

        string ammoClass = "";
        if (magazineTypes.Count() == 0)
        {
            TStringArray chamberTypes = new TStringArray;
            GetGame().ConfigGetTextArray("CfgWeapons " + weaponType + " chamberableFrom", chamberTypes);

            if (chamberTypes.Count() > 0)
            {
                GetGame().ConfigGetText("CfgMagazines " + chamberTypes[0] + " ammo", ammoClass);
                if (ammoClass != "")
                    return ammoClass;
            }
            // fallback
            return "Bullet_9x19";
        }

        // Standardně: první magazín
        string magazineType = magazineTypes[0];
        GetGame().ConfigGetText("CfgMagazines " + magazineType + " ammo", ammoClass);
        if (ammoClass != "")
            return ammoClass;
        return "Bullet_9x19";
    }

    static AIBAmmoTypeInfo GetInfoForWeapon(string weaponType)
    {
        TStringArray magazineTypes = new TStringArray;
        GetGame().ConfigGetTextArray("CfgWeapons " + weaponType + " magazines", magazineTypes);

        string magazineType = "";
        string ammoClass = "";  
        int ammoCount = 10;

        if (magazineTypes.Count() > 0)
        {
            magazineType = magazineTypes[0];
            GetGame().ConfigGetText("CfgMagazines " + magazineType + " ammo", ammoClass);
            ammoCount = GetGame().ConfigGetInt("CfgMagazines " + magazineType + " count");
            if (ammoClass == "")
                ammoClass = "Bullet_9x19";
            if (ammoCount <= 0)
                ammoCount = 10;
            return new AIBAmmoTypeInfo(magazineType, ammoClass, ammoCount);
        }
        else
        {
            TStringArray chamberTypes = new TStringArray;
            GetGame().ConfigGetTextArray("CfgWeapons " + weaponType + " chamberableFrom", chamberTypes);
            if (chamberTypes.Count() > 0)
            {
                magazineType = chamberTypes[0];
                GetGame().ConfigGetText("CfgMagazines " + magazineType + " ammo", ammoClass);
                if (ammoClass == "")
                    ammoClass = "Bullet_9x19";
                return new AIBAmmoTypeInfo(magazineType, ammoClass, 1);
            }
        }
        return new AIBAmmoTypeInfo("Mag_Fake_10Rnd", "Bullet_9x19", 10);
    }
}