class DRL_CraftStatsHelper
{
	static bool ReadInt(string path, out int val)
	{
		if (GetGame().ConfigIsExisting(path))
		{
			val = GetGame().ConfigGetInt(path);
			return true;
		}
		return false;
	}

	static bool ReadFloat(string path, out float val)
	{
		if (GetGame().ConfigIsExisting(path))
		{
			val = GetGame().ConfigGetFloat(path);
			return true;
		}
		return false;
	}

	static void UpdateResultStats(string cls, TextWidget statsTextW)
	{
		if (!statsTextW)
		{
			return;
		}

		if (!cls || cls == "")
		{
			statsTextW.SetText("");
			return;
		}

		string basePath = "CfgVehicles " + cls;
		if (!GetGame().ConfigIsExisting(basePath))
		{
			basePath = "CfgWeapons " + cls;
			if (!GetGame().ConfigIsExisting(basePath))
			{
				basePath = "CfgMagazines " + cls;
				if (!GetGame().ConfigIsExisting(basePath))
				{
					statsTextW.SetText("");
					return;
				}
			}
		}

		int hpMax = -1;
		int tmpInt = 0;
		if (ReadInt(basePath + " DamageSystem GlobalHealth Health hitpoints", tmpInt))
		{
			hpMax = tmpInt;
		}

		float projCoef = -1.0;
		float meleeCoef = -1.0;
		float infectCoef = -1.0;
		float grenadeCoef = -1.0;
		float tmp;

		if (ReadFloat(basePath + " DamageSystem GlobalArmor Projectile Health damage", tmp))
		{
			projCoef = tmp;
		}

		if (ReadFloat(basePath + " DamageSystem GlobalArmor Melee Health damage", tmp))
		{
			meleeCoef = tmp;
		}

		if (ReadFloat(basePath + " DamageSystem GlobalArmor Infected Health damage", tmp))
		{
			infectCoef = tmp;
		}
		else if (ReadFloat(basePath + " DamageSystem GlobalArmor Zombie Health damage", tmp))
		{
			infectCoef = tmp;
		}
		else if (ReadFloat(basePath + " DamageSystem GlobalArmor MeleeZombie Health damage", tmp))
		{
			infectCoef = tmp;
		}

		if (ReadFloat(basePath + " DamageSystem GlobalArmor FragGrenade Health damage", tmp))
		{
			grenadeCoef = tmp;
		}
		else if (ReadFloat(basePath + " DamageSystem GlobalArmor Explosion Health damage", tmp))
		{
			grenadeCoef = tmp;
		}

		int projProt = -1;
		int meleeProt = -1;
		int infectProt = -1;
		int grenadeProt = -1;

		if (projCoef >= 0.0)
		{
			float p = (1.0 - projCoef) * 100.0;
			if (p < 0.0) p = 0.0;
			if (p > 100.0) p = 100.0;
			projProt = Math.Round(p);
		}

		if (meleeCoef >= 0.0)
		{
			float p2 = (1.0 - meleeCoef) * 100.0;
			if (p2 < 0.0) p2 = 0.0;
			if (p2 > 100.0) p2 = 100.0;
			meleeProt = Math.Round(p2);
		}

		if (infectCoef >= 0.0)
		{
			float p3 = (1.0 - infectCoef) * 100.0;
			if (p3 < 0.0) p3 = 0.0;
			if (p3 > 100.0) p3 = 100.0;
			infectProt = Math.Round(p3);
		}

		if (grenadeCoef >= 0.0)
		{
			float p4 = (1.0 - grenadeCoef) * 100.0;
			if (p4 < 0.0) p4 = 0.0;
			if (p4 > 100.0) p4 = 100.0;
			grenadeProt = Math.Round(p4);
		}

		string t = "#STR_DRL_STATS_TITLE\n";

		if (hpMax >= 0)
			t += "#STR_DRL_STATS_DURABILITY " + hpMax.ToString() + " hp\n";
		else
			t += "#STR_DRL_STATS_DURABILITY_NONE\n";

		if (projProt >= 0)
			t += "#STR_DRL_STATS_BULLET " + projProt.ToString() + "%\n";
		else
			t += "#STR_DRL_STATS_BULLET_NONE\n";

		if (meleeProt >= 0)
			t += "#STR_DRL_STATS_KNIFE " + meleeProt.ToString() + "%\n";
		else
			t += "#STR_DRL_STATS_KNIFE_NONE\n";

		if (infectProt >= 0)
			t += "#STR_DRL_STATS_INFECTED " + infectProt.ToString() + "%\n";
		else
			t += "#STR_DRL_STATS_INFECTED_NONE\n";

		if (grenadeProt >= 0)
			t += "#STR_DRL_STATS_GRENADE " + grenadeProt.ToString() + "%";
		else
			t += "#STR_DRL_STATS_GRENADE_NONE\n";

		statsTextW.SetText(t);
	}

	static void GetAttachmentSlotsForClass(string cls, out array<string> outSlots)
	{
		outSlots = new array<string>();
		if (cls == "") return;

		string p = "CfgWeapons " + cls + " attachments";
		if (GetGame().ConfigIsExisting(p))
		{
			array<string> tmp = new array<string>();
			GetGame().ConfigGetTextArray(p, tmp);
			for (int i = 0; i < tmp.Count(); i++)
				outSlots.Insert(tmp[i]);
		}

		p = "CfgVehicles " + cls + " attachments";
		if (GetGame().ConfigIsExisting(p))
		{
			array<string> tmp2 = new array<string>();
			GetGame().ConfigGetTextArray(p, tmp2);
			for (int j = 0; j < tmp2.Count(); j++)
				outSlots.Insert(tmp2[j]);
		}

		outSlots.Sort();
		int k = 1;
		while (k < outSlots.Count())
		{
			if (outSlots[k] == outSlots[k - 1])
				outSlots.Remove(k);
			else
				k++;
		}
	}

	static bool GetSlotIconPath(string slotName, out string outPath)
	{
		outPath = "";
		if (slotName == "") return false;

		string base = "CfgSlots Slot_" + slotName + " ";
		string val = "";

		if (GetGame().ConfigIsExisting(base + "ghostIcon"))
			GetGame().ConfigGetText(base + "ghostIcon", val);

		if (val == "" && GetGame().ConfigIsExisting(base + "icon"))
			GetGame().ConfigGetText(base + "icon", val);

		if (val == "")
			return false;

		if (val.IndexOf("set:") == 0)
			outPath = val;
		else
			outPath = "set:dayz_inventory image:" + val;

		return true;
	}

	static void RenderAttachmentIconsForClass(string cls, Widget attachmentIconsWrap)
	{
		if (!attachmentIconsWrap) return;

		DRL_CraftUIHelper.ClearChildren(attachmentIconsWrap);

		if (cls == "") return;

		array<string> slots;
		GetAttachmentSlotsForClass(cls, slots);
		if (slots.Count() == 0) return;

		for (int i = 0; i < slots.Count(); i++)
		{
			string slot = slots[i];
			Widget row = GetGame().GetWorkspace().CreateWidgets("DRL_CraftMenu/UI/layout/AttachmentIcon.layout", attachmentIconsWrap);
			ImageWidget img = ImageWidget.Cast(row.FindAnyWidget("AttachmentIconImage"));
			if (!img) continue;

			string iconPath;
			if (GetSlotIconPath(slot, iconPath))
			{
				img.LoadImageFile(0, DRL_CraftUIHelper.ResolveImagePath(iconPath));
				img.SetImage(0);
			}
			else
			{
				img.LoadImageFile(0, "set:dayz_inventory image:missing");
				img.SetImage(0);
			}

			img.Show(true);
			row.Show(true);
		}

		attachmentIconsWrap.Update();
	}
}
