// INT_PDA (client) — the survivor shown by the «Персонаж» preview: a LOCAL character (ECE_LOCAL: no network id, the
// server never knows it) a few metres under the ground below the player, its physics body removed and simulation off
// (the enforce-script-reference «client-local preview entity» recipe, measured for SurvivorM_*), dressed in the
// clothes of the player's spawn set (CreateAttachment makes local items on a local character — measured 2026-10-26).
// PlayerPreviewWidget.SetPlayer draws it. The preview must be pointed at another character BEFORE this one is
// dropped. Its head often stays grey (the face texture comes with the server's beard state; setting it here turned the
// whole skin white — measured 2026-10-26; owner: an old game bug, leave it).

class INT_PDA_CharDummy
{
	static const float DEPTH = 6;			// metres under the player's feet

	protected PlayerBase m_Man;
	protected string m_Type;
	protected string m_WearKey;
	int m_Worn;								// clothes really on the dummy (self-test, log)

	PlayerBase Get()
	{
		return m_Man;
	}

	string MadeOf()
	{
		return m_Type;
	}

	// The dummy of this class in these clothes (the same one again when nothing changed).
	PlayerBase Make(string type, array<string> wear)
	{
		string key = WearKey(wear);
		if (m_Man && m_Type == type && m_WearKey == key)
			return m_Man;
		PlayerBase me = PlayerBase.Cast(GetGame().GetPlayer());
		if (!me || !INT_PDA_Chars.IsSurvivor(type))
			return null;
		vector pos = me.GetPosition();
		pos[1] = pos[1] - DEPTH;
		Object o = GetGame().CreateObjectEx(type, pos, ECE_LOCAL | ECE_NOLIFETIME | ECE_KEEPHEIGHT | ECE_NOSURFACEALIGN);
		PlayerBase man = PlayerBase.Cast(o);
		if (!man)
		{
			if (o)
				GetGame().ObjectDelete(o);
			INT_PDA_Log.Warn("превью: локальный персонаж не создан: " + type);
			return null;
		}
		dBodyDestroy(man);
		man.DisableSimulation(true);
		man.SetAllowDamage(false);
		PlayerBase old = m_Man;
		m_Man = man;
		m_Type = type;
		m_WearKey = key;
		Dress(wear);
		// the caller points the preview at the new one; the old one goes a frame later (never under a live preview)
		if (old)
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(INT_PDA_CharDummy.DeleteLater, old);
		return m_Man;
	}

	// Gone with the page; call after the preview was pointed at the real player (deleted a frame later as well).
	void Drop()
	{
		if (m_Man)
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(INT_PDA_CharDummy.DeleteLater, m_Man);
		m_Man = null;
		m_Type = "";
		m_WearKey = "";
		m_Worn = 0;
	}

	static void DeleteLater(PlayerBase old)
	{
		if (old && GetGame())
			GetGame().ObjectDelete(old);
	}

	// A class that does not exist (a mod not loaded on this server) is skipped: the dummy stays without that item.
	protected void Dress(array<string> wear)
	{
		m_Worn = 0;
		if (!wear)
			return;
		foreach (string type : wear)
		{
			if (type == "" || !GetGame().ConfigIsExisting("CfgVehicles " + type))
				continue;
			if (m_Man.GetInventory().CreateAttachment(type))
				m_Worn++;
		}
	}

	protected string WearKey(array<string> wear)
	{
		string k = "";
		if (!wear)
			return k;
		foreach (string w : wear)
			k = k + w + ";";
		return k;
	}
}
