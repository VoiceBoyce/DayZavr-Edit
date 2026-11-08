// Лента клана для соклановцев в сети: сообщение клан-чата или событие журнала.
// Клиент кладёт её строкой «КЛАН» в общий чат INT_GlobalChat, если тот стоит; без него строка просто не показывается.
class VB_TabletFeed
{
	static const int CHAT = 0;
	static const int EVENT = 1;

	static void Chat(VB_Clan c, string who, string text)
	{
		VB_ClanJournalRow row = new VB_ClanJournalRow();
		row.Kind = -1;
		row.Who = who;
		row.Arg = text;
		Send(c, CHAT, row);
	}

	// В ленту идут только заметные события: склад, карта и настройки остаются в журнале.
	static void Event(VB_Clan c, VB_ClanJournalRow row)
	{
		switch (row.Kind)
		{
			case VB_ClanEvent.JOINED:
			case VB_ClanEvent.LEFT:
			case VB_ClanEvent.KICKED:
			case VB_ClanEvent.RANK:
			case VB_ClanEvent.LEADER:
			case VB_ClanEvent.LEVEL:
			case VB_ClanEvent.NEWS:
			case VB_ClanEvent.CONTRACT:
			case VB_ClanEvent.WEEK_PRIZE:
				Send(c, EVENT, row);
				break;
		}
	}

	protected static void Send(VB_Clan c, int kind, VB_ClanJournalRow row)
	{
		if (!c || !GetGame() || !GetGame().IsDedicatedServer())
			return;
		int op = VB_TabletOp.CLAN_FEED;
		foreach (VB_ClanMember m : c.Members)
		{
			PlayerBase p = VB_TabletServer.FindPlayer(m.Uid);
			if (!p || !p.GetIdentity())
				continue;
			ScriptRPC rpc = new ScriptRPC();
			rpc.Write(op);
			rpc.Write(kind);
			row.Write(rpc);
			rpc.Send(p, VB_TABLET_RPC_S2C, true, p.GetIdentity());
		}
	}
}
