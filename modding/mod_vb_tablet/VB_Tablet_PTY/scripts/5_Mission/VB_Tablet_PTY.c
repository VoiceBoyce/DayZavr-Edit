// Кланы планшета и друзья INT_PartyMode (сервер):
//   - соклановцы становятся друзьями в пати: ники и здоровье над головой, панель «Группа», пинги T, друзья на картах;
//   - при выходе, исключении или роспуске снимается только дружба, которую создал планшет; старая дружба остаётся;
//   - если игрок сам удалил соклановца из друзей, планшет больше эту пару не добавляет.
// Дружба создаётся, когда оба в сети: при вступлении в клан и при входе на сервер.
// Файл $profile:VB_Mods/VB_Tablet/party_link.json: два переключателя и пары, которыми управляет планшет.

class VB_PtyLinkFile
{
	string Comment = "AutoFriends 1: соклановцы становятся друзьями в INT_PartyMode. RemoveOnLeave 1: при выходе из клана дружба, созданная планшетом, снимается. Остальное заполняет сервер.";
	int AutoFriends = 1;
	int RemoveOnLeave = 1;
	ref array<string> Pairs = new array<string>();			// дружба создана планшетом
	ref array<string> Declined = new array<string>();		// игрок сам удалил соклановца: больше не добавлять
	ref array<string> PendingRemove = new array<string>();	// снять, когда кто-то из двоих зайдёт
}

class VB_TabletPartyLink
{
	static const string FILE = "$profile:VB_Mods/VB_Tablet/party_link.json";

	protected static ref VB_PtyLinkFile s_Data;
	protected static bool s_Busy;	// идёт своё удаление: событие пати не считается отказом игрока
	protected static bool s_Dirty;	// файл пар изменился: пишется один раз в конце события, а не на каждую пару

	static void Init()
	{
		Data();
		VB_TabletClans.OnMemberJoin().Insert(OnJoin);
		VB_TabletClans.OnMemberLeave().Insert(OnLeave);
		INT_PTY_SE_API.OnFriendsChanged().Insert(OnFriendsChanged);
		Print(VB_TABLET_TAG + " кланы и друзья пати связаны: AutoFriends " + s_Data.AutoFriends.ToString() + ", RemoveOnLeave " + s_Data.RemoveOnLeave.ToString());
	}

	static void SetBusy(bool busy)
	{
		s_Busy = busy;
	}

	static VB_PtyLinkFile Data()
	{
		if (s_Data)
			return s_Data;
		s_Data = new VB_PtyLinkFile();
		string error;
		if (FileExist(FILE) && !JsonFileLoader<VB_PtyLinkFile>.LoadFile(FILE, s_Data, error))
		{
			Print(VB_TABLET_TAG + " ошибка в party_link.json: " + error);
			CopyFile(FILE, FILE + ".broken");
			s_Data = new VB_PtyLinkFile();
		}
		if (!s_Data.Pairs)
			s_Data.Pairs = new array<string>();
		if (!s_Data.Declined)
			s_Data.Declined = new array<string>();
		if (!s_Data.PendingRemove)
			s_Data.PendingRemove = new array<string>();
		s_Dirty = true;
		Commit();
		return s_Data;
	}

	static void Save()
	{
		s_Dirty = true;
	}

	// Запись, если были изменения. Вызывается в конце каждого события.
	static void Commit()
	{
		if (!s_Dirty || !s_Data)
			return;
		VB_TabletFiles.Dirs();
		string error;
		if (!VB_SafeJson<VB_PtyLinkFile>.Save(FILE, s_Data, error))
		{
			Print(VB_TABLET_TAG + " не записан party_link.json: " + error);
			return;
		}
		s_Dirty = false;
	}

	// Пара в одном порядке: меньший Steam64 первым. Строки сравниваются числом по двум половинам.
	static string Key(string a, string b)
	{
		if (Less(b, a))
			return b + "|" + a;
		return a + "|" + b;
	}

	protected static bool Less(string a, string b)
	{
		if (a.Length() != b.Length())
			return a.Length() < b.Length();
		int n = a.Length();
		int cut = n - 9;
		if (cut < 0)
			cut = 0;
		string ha = a.Substring(0, cut);
		string hb = b.Substring(0, cut);
		int ia = ha.ToInt();
		int ib = hb.ToInt();
		if (ia != ib)
			return ia < ib;
		string la = a.Substring(cut, n - cut);
		string lb = b.Substring(cut, n - cut);
		int ja = la.ToInt();
		int jb = lb.ToInt();
		return ja < jb;
	}

	protected static void Split(string key, out string a, out string b)
	{
		int bar = key.IndexOf("|");
		a = "";
		b = "";
		if (bar < 0)
			return;
		a = key.Substring(0, bar);
		b = key.Substring(bar + 1, key.Length() - bar - 1);
	}

	protected static void Drop(array<string> list, string key)
	{
		int i = list.Find(key);
		while (i >= 0)
		{
			list.Remove(i);
			i = list.Find(key);
		}
	}

	// --- события ---------------------------------------------------------------------------------------------
	static void OnJoin(VB_Clan c, string uid)
	{
		if (!c || Data().AutoFriends == 0)
			return;
		foreach (VB_ClanMember m : c.Members)
		{
			if (m.Uid != uid)
				Befriend(uid, m.Uid);
		}
		Commit();
	}

	static void OnLeave(VB_Clan c, string uid)
	{
		if (!c)
			return;
		foreach (VB_ClanMember m : c.Members)
		{
			if (m.Uid != uid)
				Unfriend(uid, m.Uid);
		}
		Commit();
	}

	// Вход на сервер: отложенные удаления и дружба с соклановцами в сети.
	static void OnConnect(string uid)
	{
		VB_PtyLinkFile d = Data();
		bool changed = false;
		for (int i = d.PendingRemove.Count() - 1; i >= 0; i--)
		{
			string key = d.PendingRemove.Get(i);
			string a;
			string b;
			Split(key, a, b);
			if (a != uid && b != uid)
				continue;
			if (INT_PTY_SE_Core.VBT_Unfriend(a, b))
			{
				d.PendingRemove.Remove(i);
				changed = true;
			}
		}
		if (changed)
			Save();
		VB_Clan c = VB_TabletClans.OfPlayer(uid);
		if (c)
			OnJoin(c, uid);
		Commit();
	}

	// Игрок сам удалил или добавил друга: планшет запоминает его выбор.
	static void OnFriendsChanged(string a, string b, bool added)
	{
		if (s_Busy)
			return;
		VB_PtyLinkFile d = Data();
		string key = Key(a, b);
		if (added)
		{
			if (d.Declined.Find(key) >= 0)
			{
				Drop(d.Declined, key);
				Save();
				Commit();
			}
			return;
		}
		if (d.Pairs.Find(key) < 0)
			return;
		Drop(d.Pairs, key);
		if (d.Declined.Find(key) < 0)
			d.Declined.Insert(key);
		Save();
		Commit();
	}

	// --- дружба ----------------------------------------------------------------------------------------------
	protected static void Befriend(string a, string b)
	{
		if (a == "" || b == "" || a == b)
			return;
		VB_PtyLinkFile d = Data();
		string key = Key(a, b);
		if (d.Declined.Find(key) >= 0)
			return;
		INT_PTY_SE_Online oa = INT_PTY_SE_Core.Find(a);
		INT_PTY_SE_Online ob = INT_PTY_SE_Core.Find(b);
		if (!oa || !ob || !oa.INT_File || !ob.INT_File)
			return;
		if (oa.INT_File.Untrusted || ob.INT_File.Untrusted)
			return;
		// Уже друзья (или односторонняя запись, которую пати уберёт сама): не трогаем.
		if (oa.INT_File.Has(b) || ob.INT_File.Has(a))
			return;
		int most = INT_PTY_SE_Core.Cfg().MaxFriends;
		if (most > 0 && (oa.INT_File.Friends.Count() >= most || ob.INT_File.Friends.Count() >= most))
			return;
		Drop(d.PendingRemove, key);
		s_Busy = true;
		bool made = INT_PTY_SE_Core.MakeFriends(oa, ob, false);
		s_Busy = false;
		if (!made)
			return;
		if (d.Pairs.Find(key) < 0)
			d.Pairs.Insert(key);
		Save();
		INT_PTY_SE_History.Line(INT_PTY_SE_History.Who(oa.Name, a) + " и " + INT_PTY_SE_History.Who(ob.Name, b) + " стали друзьями через клан планшета");
	}

	protected static void Unfriend(string a, string b)
	{
		VB_PtyLinkFile d = Data();
		string key = Key(a, b);
		if (d.Pairs.Find(key) < 0)
			return;
		Drop(d.Pairs, key);
		if (d.RemoveOnLeave != 0 && !INT_PTY_SE_Core.VBT_Unfriend(a, b) && d.PendingRemove.Find(key) < 0)
			d.PendingRemove.Insert(key);
		Save();
	}
}

modded class INT_PTY_SE_Core
{
	// Снять дружбу a и b от имени того из них, кто в сети. false: оба не в сети, повторить при входе.
	static bool VBT_Unfriend(string a, string b)
	{
		INT_PTY_SE_Online o = Find(a);
		string other = b;
		if (!o)
		{
			o = Find(b);
			other = a;
		}
		if (!o || !o.INT_File || o.INT_File.Untrusted)
			return false;
		if (!o.INT_File.Has(other))
			return true;
		VB_TabletPartyLink.SetBusy(true);
		OnFriendRemove(o, other);
		VB_TabletPartyLink.SetBusy(false);
		// Пати откатывает удаление, если свой файл не записался: тогда повторить при следующем входе.
		return !o.INT_File.Has(other);
	}
}

modded class MissionServer
{
	override void OnInit()
	{
		super.OnInit();
		VB_TabletPartyLink.Init();
	}

	override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity)
	{
		super.InvokeOnConnect(player, identity);
		if (identity)
			VB_TabletPartyLink.OnConnect(identity.GetPlainId());
	}

	override void OnClientReconnectEvent(PlayerIdentity identity, PlayerBase player)
	{
		super.OnClientReconnectEvent(identity, player);
		if (identity)
			VB_TabletPartyLink.OnConnect(identity.GetPlainId());
	}
}
