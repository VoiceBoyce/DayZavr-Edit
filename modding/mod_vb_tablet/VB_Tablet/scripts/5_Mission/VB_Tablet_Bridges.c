// Связки планшета с модами сервера: общий чат INT_GlobalChat, друзья INT_PartyMode, карта VanillaPlusPlusMap.
// Правки только здесь, чужие моды не меняются. Чат и друзья подключаются через их дефайны, VPP своего дефайна не имеет:
// его меню узнаётся по имени класса, функции вызываются по имени. Без этих модов всё ниже молча ничего не делает.
class VB_TabletBridge
{
	static const string VPP_MENU = "VPPMapMenu";
	static const string GC_TAG = "#STR_VB_TAB_GC_TAG";

	protected static int s_Vpp = -1;
	protected static VB_TabletClient s_HookedOn;

	// Карта VPP стоит: класс её меню есть среди скриптов.
	static bool HasVpp()
	{
		if (s_Vpp < 0)
		{
			s_Vpp = 0;
			string cls = VPP_MENU;
			if (cls.ToType())
				s_Vpp = 1;
		}
		return s_Vpp == 1;
	}

	// Каждый кадр миссии: подписка на ленту клана (сессия клиента после выхода с сервера новая) и слой на карте VPP.
	static void Tick(float dt)
	{
		VB_TabletClient client = VB_TabletClient.Get();
		if (s_HookedOn != client)
		{
			client.OnFeed.Insert(OnFeed);
			s_HookedOn = client;
		}
		if (HasVpp())
			VB_TabletVppLayer.Get().Tick(dt);
	}

	static void Reset()
	{
		s_HookedOn = null;
		VB_TabletVppLayer.Reset();
	}

	// Лента клана строкой «КЛАН» в цвете клана в общий чат: сообщение клан-чата или событие журнала.
	static void OnFeed(int kind, VB_ClanJournalRow row)
	{
#ifdef INT_GLOBALCHAT_CL
		if (!row)
			return;
		string text;
		if (kind == 0)
			text = row.Who + ": " + row.Arg;
		else
			text = VBA_Journal.Text(row);
		int color = VB_C_ACCENT;
		VB_Clan c = VB_TabletClient.Get().Clan;
		if (c && c.Color != 0)
			color = c.Color;
		INT_GC_Client.PushLocal(text, color, GC_TAG);
#endif
	}

	// Тот же фильтр слов, что у общего чата: его список и защита от обходов вроде «м.а.т». true: текст не отправляем.
	static bool Censored(string text)
	{
#ifdef INT_GLOBALCHAT_CL
		if (text == "")
			return false;
		INT_GC_Filter filter = INT_GC_Client.Filter();
		string matched;
		if (filter && filter.Check(text, matched))
		{
			INT_UIF_Toast.Error(VBT.T(VBK.ResultText(VB_TabletResult.BANNED_WORD)));
			return true;
		}
#endif
		return false;
	}

	// 3D метки VPP прячутся и показываются вместе с метками планшета: одна клавиша на всё.
	static void SyncVpp3D()
	{
		if (!g_Game || !HasVpp())
			return;
		string fn = "ShowAllHidden3dMarkers";
		if (VB_TabletClient.Get().Hide3D)
			fn = "HideAll3dMarkers";
		g_Game.GameScript.CallFunction(g_Game, fn, null, 0);
	}
}
