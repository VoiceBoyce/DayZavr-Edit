// Ники над головами: ранг и клан каждого игрока в сети. Клиент сам этого не знает, сервер присылает по запросу.
class VB_NameTagClan
{
	string EmbShape;
	string EmbSymbol;
	int Color;
}

class VB_NameTags
{
	static const int MAX_ROWS = 1000;

	bool Enabled = true;
	float Range = 15;
	bool Loaded;
	int Version;
	ref map<string, int> Rank = new map<string, int>();
	ref map<string, string> ClanOf = new map<string, string>();
	ref map<string, ref VB_NameTagClan> Clans = new map<string, ref VB_NameTagClan>();

	bool Knows(string steam)
	{
		return Rank.Contains(steam);
	}

	bool Read(ParamsReadContext ctx)
	{
		bool on;
		float range;
		int n;
		if (!ctx.Read(on) || !ctx.Read(range) || !ctx.Read(n) || n < 0 || n > MAX_ROWS)
			return false;
		map<string, int> rank = new map<string, int>();
		map<string, string> clanOf = new map<string, string>();
		int i;
		for (i = 0; i < n; i++)
		{
			string id;
			int r;
			string cid;
			if (!ctx.Read(id) || !ctx.Read(r) || !ctx.Read(cid))
				return false;
			rank.Set(id, Math.Clamp(r, 0, VB_RANK_COUNT - 1));
			if (cid != "")
				clanOf.Set(id, cid);
		}
		int m;
		if (!ctx.Read(m) || m < 0 || m > MAX_ROWS)
			return false;
		map<string, ref VB_NameTagClan> clans = new map<string, ref VB_NameTagClan>();
		for (i = 0; i < m; i++)
		{
			string key;
			VB_NameTagClan c = new VB_NameTagClan();
			if (!ctx.Read(key) || !ctx.Read(c.EmbShape) || !ctx.Read(c.EmbSymbol) || !ctx.Read(c.Color))
				return false;
			clans.Set(key, c);
		}
		Enabled = on;
		Range = Math.Clamp(range, 3, 50);
		Rank = rank;
		ClanOf = clanOf;
		Clans = clans;
		Loaded = true;
		Version++;
		return true;
	}
}
