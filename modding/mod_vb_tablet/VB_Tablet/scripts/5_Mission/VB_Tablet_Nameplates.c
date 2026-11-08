// Ники над головами (вместо @World_HeadNicknames): эмблема клана и ник, под ними значок и название ранга.
// Видны в пределах дальности из конфига сервера (15 м) и только при прямой видимости.
// Друг из INT_PartyMode, у которого видна метка пати: только эмблема и ранг, ник и здоровье даёт пати, а её метка
// по дефайну RF_NICKNAMES_CL поднимается на 34 px в пределах 10 м. Дальше 10 м над другом остаётся одна метка пати.
// Слой скрыт под любым меню, при скрытом HUD, если игрок мёртв или без сознания.
class VB_Nameplate extends Managed
{
	PlayerBase Body;	// слабая ссылка: тело уходит из сети без предупреждения
	string Steam;
	Widget Root;
	string Sig;			// что нарисовано: режим, ник, ранг, клан
	float LosIn;
	bool Los;
	bool Seen;
}

class VB_TabletNameplates
{
	static const float FRIEND_RANGE = 10;	// = RF_NICK_RANGE в INT_PTY_HudTags
	static const float HEAD_LIFT = 0.32;	// строка под меткой пати: та же точка, что у пати
	static const float PLATE_LIFT = 0.16;	// полная табличка ближе к голове
	static const float LOS_GAP = 0.25;
	static const float ASK_GAP = 30;
	static const float ASK_UNKNOWN = 5;
	static const float ASK_NEAR = 60;		// таблица нужна, только когда рядом есть игроки

	protected static ref VB_TabletNameplates s_Inst;

	protected Widget m_Layer;
	protected ref array<ref VB_Nameplate> m_Items = new array<ref VB_Nameplate>();
	protected float m_AskIn;
	protected float m_SinceAsk = 1000;
	protected int m_SeenVer = -1;

	static VB_TabletNameplates Get()
	{
		if (!s_Inst)
			s_Inst = new VB_TabletNameplates();
		return s_Inst;
	}

	static void Reset()
	{
		if (s_Inst)
		{
			s_Inst.m_Items.Clear();
			if (s_Inst.m_Layer)
				s_Inst.m_Layer.Unlink();
		}
		s_Inst = null;
	}

	void Update(float dt)
	{
		PlayerBase me = PlayerBase.Cast(GetGame().GetPlayer());
		if (!me)
			return;
		VB_TabletClient client = VB_TabletClient.Get();
		VB_NameTags tags = client.Tags;
		m_SinceAsk = m_SinceAsk + dt;
		bool show = tags.Enabled && me.IsAlive() && !me.IsUnconscious() && !GetGame().GetUIManager().GetMenu() && !HudHidden();
		if (!show)
		{
			if (m_Layer && m_Layer.IsVisible())
				m_Layer.Show(false);
			return;
		}
		EnsureLayer();
		if (!m_Layer)
			return;
		if (!m_Layer.IsVisible())
			m_Layer.Show(true);
		if (m_SeenVer != tags.Version)
		{
			m_SeenVer = tags.Version;
			foreach (VB_Nameplate old : m_Items)
				old.Sig = "";
		}
		foreach (VB_Nameplate it : m_Items)
			it.Seen = false;
		vector myPos = me.GetPosition();
		float range = tags.Range;
		bool anyNear = false;
		bool unknown = false;
		array<Man> all = ClientData.m_PlayerBaseList;
		foreach (Man man : all)
		{
			PlayerBase p = PlayerBase.Cast(man);
			if (!p || p == me || !p.IsAlive())
				continue;
			PlayerIdentity ident = p.GetIdentity();
			string steam = "";
			string nick = "";
			if (ident)
			{
				steam = ident.GetPlainId();
				nick = ident.GetName();
			}
#ifdef DIAG_DEVELOPER
			// Стенд DayZDiag: манекен без игрока показывается с рангом и кланом самого игрока, чтобы проверить вид.
			if (!ident && me.GetIdentity())
			{
				steam = me.GetIdentity().GetPlainId();
				nick = "Dummy";
			}
#endif
			if (steam == "")
				continue;
			float dist = vector.Distance(myPos, p.GetPosition());
			if (dist > ASK_NEAR)
				continue;
			anyNear = true;
			if (dist > range)
				continue;
			if (!tags.Knows(steam))
				unknown = true;
			Paint(PlateOf(p, steam), p, steam, nick, dist, dt, tags);
		}
		for (int i = m_Items.Count() - 1; i >= 0; i--)
		{
			VB_Nameplate item = m_Items.Get(i);
			if (item.Seen)
				continue;
			if (!item.Body)
			{
				if (item.Root)
					item.Root.Unlink();
				m_Items.Remove(i);
				continue;
			}
			if (item.Root && item.Root.IsVisible())
				item.Root.Show(false);
		}
		// Таблица ранга и клана: раз в 30 секунд, пока рядом есть игроки; новый игрок рядом спрашивает раньше.
		m_AskIn = m_AskIn - dt;
		if (anyNear && (m_AskIn <= 0 || (unknown && m_SinceAsk > ASK_UNKNOWN)))
		{
			m_AskIn = ASK_GAP;
			m_SinceAsk = 0;
			client.Request(VB_TabletOp.NAMETAGS);
		}
	}

	protected void EnsureLayer()
	{
		if (m_Layer)
			return;
		m_Layer = INT_UIF_Draw.Frame(null, 0, 0, 1, 1, "VBT_Nameplates");
		if (!m_Layer)
			return;
		m_Layer.SetSort(1);
		m_Layer.SetPos(0, 0);
		m_Layer.SetSize(INT_UIF_Scale.ScreenW(), INT_UIF_Scale.ScreenH());
	}

	// Игрок спрятал HUD своей клавишей.
	protected bool HudHidden()
	{
		Mission mission = GetGame().GetMission();
		if (!mission)
			return false;
		IngameHud ih = IngameHud.Cast(mission.GetHud());
		if (!ih)
			return false;
		IngameHudVisibility vis = ih.GetHudVisibility();
		if (!vis)
			return false;
		return vis.IsContextFlagActive(EHudContextFlags.HUD_HIDE);
	}

	protected VB_Nameplate PlateOf(PlayerBase p, string steam)
	{
		foreach (VB_Nameplate it : m_Items)
		{
			if (it.Body == p)
				return it;
		}
		VB_Nameplate fresh = new VB_Nameplate();
		fresh.Body = p;
		fresh.Steam = steam;
		m_Items.Insert(fresh);
		return fresh;
	}

	protected void Paint(VB_Nameplate item, PlayerBase p, string steam, string nick, float dist, float dt, VB_NameTags tags)
	{
		item.Seen = true;
		bool friendTag = PartyShowsTag(steam);
		int bone = p.GetBoneIndexByName("Head");
		vector head = p.GetBonePositionWS(bone);
		if (friendTag)
			head[1] = head[1] + HEAD_LIFT;
		else
			head[1] = head[1] + PLATE_LIFT;
		vector s = GetGame().GetScreenPos(head);
		float sw = INT_UIF_Scale.ScreenW();
		float sh = INT_UIF_Scale.ScreenH();
		bool front = s[2] > 0 && s[0] > -80 && s[0] < sw + 80 && s[1] > -80 && s[1] < sh + 80;
		bool hide = !front || (friendTag && dist > FRIEND_RANGE);
		if (!hide)
		{
			item.LosIn = item.LosIn - dt;
			if (item.LosIn <= 0)
			{
				item.LosIn = LOS_GAP;
				item.Los = SeesHead(p, head);
			}
			hide = !item.Los;
		}
		if (hide)
		{
			if (item.Root && item.Root.IsVisible())
				item.Root.Show(false);
			return;
		}
		string sig = Signature(nick, steam, friendTag, tags);
		if (sig != item.Sig || !item.Root)
		{
			item.Sig = sig;
			Build(item, nick, steam, friendTag, tags);
		}
		if (!item.Root)
			return;
		item.Root.SetPos(Math.Round(s[0]), Math.Round(s[1]));
		float alpha = Math.Clamp(1.15 - dist / tags.Range * 0.4, 0.75, 1);
		item.Root.SetAlpha(alpha);
		if (!item.Root.IsVisible())
			item.Root.Show(true);
	}

	// Луч от камеры до головы: первым должен попасться сам игрок или его вещи.
	protected bool SeesHead(PlayerBase p, vector head)
	{
		vector from = GetGame().GetCurrentCameraPosition();
		vector hitPos;
		vector hitDir;
		int hitComp;
		set<Object> hits = new set<Object>();
		Object me = GetGame().GetPlayer();
		DayZPhysics.RaycastRV(from, head, hitPos, hitDir, hitComp, hits, p, me, true, false, ObjIntersectView);
		// Попадания по порядку: свои вещи (рюкзак, оружие за спиной в виде от третьего лица) пропускаются,
		// вещи цели значат «видно», всё остальное закрывает голову.
		int n = hits.Count();
		for (int i = 0; i < n; i++)
		{
			Object hit = hits.Get(i);
			if (!hit || hit == me)
				continue;
			EntityAI e = EntityAI.Cast(hit);
			if (e && e.GetHierarchyRoot() == me)
				continue;
			if (hit == p || (e && e.GetHierarchyRoot() == p))
				return true;
			return false;
		}
		return true;
	}

	// Пати рисует над этим игроком свою метку: он друг, в сети, включён в показ, метки пати не скрыты.
	protected bool PartyShowsTag(string steam)
	{
#ifdef INT_PARTYMODE_CL
		if (!INT_PTY_Client.s_Friends || !INT_PTY_UI.IsVisible() || !INT_PTY_UI.IsPartShown(INT_PTY_UI.PART_TAGS))
			return false;
		INT_PTY_Settings st = INT_PTY_Client.Settings();
		if (st && st.TagsOn != 1)
			return false;
		foreach (INT_PTY_Friend fr : INT_PTY_Client.s_Friends)
		{
			if (fr && fr.Id == steam)
				return fr.Online && !fr.Dead && INT_PTY_Client.IsShown(fr.Id);
		}
#endif
		return false;
	}

	protected string Signature(string nick, string steam, bool compact, VB_NameTags tags)
	{
		string mode = "f";
		if (compact)
			mode = "c";
		int rank = -1;
		if (tags.Knows(steam))
			rank = tags.Rank.Get(steam);
		string cid = "";
		tags.ClanOf.Find(steam, cid);
		return mode + "|" + nick + "|" + rank.ToString() + "|" + cid;
	}

	// Макет «Над головой в игре»: эмблема 24 и ник 19 белым с тенью; ниже значок 18 и ранг 13 акцентом.
	// Точка привязки внизу по центру: всё рисуется выше неё.
	protected void Build(VB_Nameplate item, string nick, string steam, bool compact, VB_NameTags tags)
	{
		if (item.Root)
			item.Root.Unlink();
		item.Root = INT_UIF_Draw.Frame(m_Layer, 0, 0, 1, 1);
		if (!item.Root)
			return;
		VB_NameTagClan clan = null;
		string cid;
		if (tags.ClanOf.Find(steam, cid))
			tags.Clans.Find(cid, clan);
		int rank = -1;
		if (tags.Knows(steam))
			rank = tags.Rank.Get(steam);
		string rankName = "";
		if (rank >= 0)
			rankName = VBT.T(VBT.RankName(rank));
		float rankW = 0;
		float rw = 0;
		if (rank >= 0)
		{
			rankW = MeasureW(item.Root, rankName, 13);
			rw = 18 + 4 + rankW;
		}
		if (compact)
		{
			// Одна строка под меткой пати: эмблема, значок, ранг.
			float ew = 0;
			if (clan)
				ew = 22 + 6;
			float cw = ew + rw;
			if (cw <= 0)
				return;
			float cx = -cw * 0.5;
			float cy = -26;
			if (clan)
				VBK.Emblem(item.Root, cx, cy, 22, clan.EmbShape, clan.EmbSymbol, clan.Color);
			if (rank >= 0)
				RankRow(item.Root, cx + ew, cy + 2, rank, rankName, rankW);
			return;
		}
		float es = 0;
		if (clan)
			es = 24 + 6;
		float nw = MeasureW(item.Root, nick, 19) + 4;
		float w1 = es + nw;
		float top = -26;
		if (rank >= 0)
			top = -53;
		float x1 = -w1 * 0.5;
		if (clan)
			VBK.Emblem(item.Root, x1, top, 24, clan.EmbShape, clan.EmbSymbol, clan.Color);
		TextWidget name = INT_UIF_Draw.Text(item.Root, x1 + es, top, nw + 8, 24, nick, 19, VB_C_WHITE);
		Shadow(name);
		if (rank >= 0)
			RankRow(item.Root, -rw * 0.5, top + 29, rank, rankName, rankW);
	}

	protected void RankRow(Widget p, float x, float y, int rank, string rankName, float textW)
	{
		VBT.Insignia(p, x, y, 18, rank, VB_C_ACCENT);
		TextWidget t = INT_UIF_Draw.Text(p, x + 22, y - 1, textW + 8, 20, rankName, 13, VB_C_ACCENT);
		Shadow(t);
	}

	// Ширина текста по метрикам движка, как у подсказок фреймворка: оценка TextWidth для этого шрифта мала.
	protected float MeasureW(Widget parent, string text, float size)
	{
		float w = INT_UIF_Draw.TextWidth(text, size);
		TextWidget probe = INT_UIF_Draw.Text(parent, 0, 0, 900, 30, text, size, 0);
		if (!probe)
			return w;
		probe.Update();
		int mw;
		int mh;
		probe.GetTextSize(mw, mh);
		float f = INT_UIF_Scale.Factor();
		if (mw > 0 && f > 0)
			w = mw / f;
		probe.Unlink();
		return w;
	}

	protected void Shadow(TextWidget t)
	{
		if (!t)
			return;
		t.SetShadow(2, 0xFF000000, 0.85, 0, 1);
		t.SetOutline(1, 0x99000000);
	}
}
