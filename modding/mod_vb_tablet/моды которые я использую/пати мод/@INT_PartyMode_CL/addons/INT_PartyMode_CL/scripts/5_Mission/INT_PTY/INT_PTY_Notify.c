// INT PartyMode (client) — notifications (brief 17) and sounds. Texts come from the stringtable ("%1" = name);
// every message key KEY has a card title KEY_H. While the window is open the card shows inside it, otherwise on
// the HUD (Expedition cards).

class INT_PTY_Notify
{
	static string Tr(string key, string arg)
	{
		string s = Widget.TranslateString("#" + key);
		s.Replace("%1", arg);
		return s;
	}

	// The card only (no sound).
	static void Show(string key, string arg, int kind)
	{
		string title = Widget.TranslateString("#" + key + "_H");
		string text = Tr(key, arg);
		if (INT_PTY_Menu.IsOpen())
			INT_PTY_Menu.ShowToast(kind, title, text);
		else
			INT_PTY_Hud.ShowToast(kind, title, text);
	}

	// The card + the sound of its kind.
	static void Text(string key, string arg, int kind)
	{
		Show(key, arg, kind);
		INT_PTY_Sfx.Notice(kind);
	}

	static void Online(string name)
	{
		Show("STR_INT_PTY_T_ONLINE", name, INT_UIF_ToastKind.SUCCESS);
		INT_PTY_Sfx.Online();
	}

	static void Dead(string name)
	{
		Show("STR_INT_PTY_T_DEAD", name, INT_UIF_ToastKind.ERROR);
		INT_PTY_Sfx.Dead();
	}

	static void InviteIn(string name)
	{
		Show("STR_INT_PTY_T_INVITE_IN", name, INT_UIF_ToastKind.INFO);
		INT_PTY_Sfx.Invite();
	}

	static void Result(int code, int value)
	{
		string key = "";
		int kind = INT_UIF_ToastKind.WARNING;
		if (code == INT_PTY_Const.RES_RATE_LIMITED)
			key = "STR_INT_PTY_R_RATE";
		else if (code == INT_PTY_Const.RES_NOT_SEARCHING)
			key = "STR_INT_PTY_R_NOT_SEARCHING";
		else if (code == INT_PTY_Const.RES_ALREADY_FRIENDS)
			key = "STR_INT_PTY_R_ALREADY_FRIENDS";
		else if (code == INT_PTY_Const.RES_ALREADY_INVITED)
			key = "STR_INT_PTY_R_ALREADY_INVITED";
		else if (code == INT_PTY_Const.RES_TOO_MANY_INVITES)
			key = "STR_INT_PTY_R_TOO_MANY";
		else if (code == INT_PTY_Const.RES_MY_FRIEND_LIMIT)
			key = "STR_INT_PTY_R_MY_LIMIT";
		else if (code == INT_PTY_Const.RES_HIS_FRIEND_LIMIT)
			key = "STR_INT_PTY_R_HIS_LIMIT";
		else if (code == INT_PTY_Const.RES_NOT_ONLINE)
			key = "STR_INT_PTY_R_NOT_ONLINE";
		else if (code == INT_PTY_Const.RES_NO_INVITE)
			key = "STR_INT_PTY_R_NO_INVITE";
		else if (code == INT_PTY_Const.RES_SELF)
			key = "STR_INT_PTY_R_SELF";
		else if (code == INT_PTY_Const.RES_NOT_FRIEND)
			key = "STR_INT_PTY_R_NOT_FRIEND";
		else if (code == INT_PTY_Const.RES_SAVE_FAILED)
		{
			key = "STR_INT_PTY_R_SAVE";
			kind = INT_UIF_ToastKind.ERROR;
		}
		if (key == "")
			return;
		Show(key, value.ToString(), kind);
		INT_PTY_Sfx.Warn();
	}
}
