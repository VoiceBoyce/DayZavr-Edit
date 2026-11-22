// Вкладка «Клан» в окне общего чата INT_GlobalChat: третья кнопка рядом с «Глобальный» и «Рядом», Tab по кругу,
// Enter отправляет текст в клан-чат планшета тем же запросом, что и окно планшета. Ответ приходит всем соклановцам
// строкой «КЛАН» в общий чат (VB_TabletBridge) и в историю клан-чата.
// Чат шлёт в глобальный канал всё, что не «Рядом» и не ЛС, поэтому на вкладке «Клан» отправку ведёт только этот код.
// Выбор вкладки живёт до выхода из игры и в настройки чата не пишется: без дополнения чат принял бы её за глобальный.
class VB_TabletGC
{
	static const int CH_CLAN = 100;
	static const int MAX = 200;		// как MSG_MAX окна клан-чата
	static const float WATCH_SEC = 6;	// сколько ждать ответа сервера на отправку
	static const int R_SENT = 0;
	static const int R_WAIT = 1;
	static const int R_BANNED = 2;
	static const int R_NOCLAN = 3;

	static bool s_Clan;
	protected static int s_WatchVer = -1;
	protected static float s_WatchUntil;

	static bool CanUse()
	{
		return VB_TabletClient.Get().Clan != null;
	}

	static bool IsActive()
	{
		return s_Clan && CanUse();
	}

	static int Color()
	{
		VB_Clan c = VB_TabletClient.Get().Clan;
		if (c && c.Color != 0)
			return c.Color;
		return VB_C_ACCENT;
	}

	// Команды и обращения чата уходят как обычно, а не в клан: «#» администратора, /pm, /g, /help.
	static bool IsChatCommand(string text)
	{
		if (INT_GC_Client.IsCommand(text))
			return true;
		string lower = INT_GC_Util.Lower(text);
		array<string> words = {"/pm", "/лс", "/g", "/г", "/l", "/л", "/р", "/help", "/?", "/помощь"};
		foreach (string w : words)
		{
			if (lower == w || lower.IndexOf(w + " ") == 0)
				return true;
		}
		return false;
	}

	static int Send(string text)
	{
		if (!CanUse())
		{
			INT_GC_Client.Notice(VBT.T("#STR_VB_TAB_GC_NOCLAN"), 1);
			return R_NOCLAN;
		}
		INT_GC_Filter filter = INT_GC_Client.Filter();
		string matched;
		if (filter && filter.Check(text, matched))
		{
			INT_GC_Client.Notice(VBT.T(VBK.ResultText(VB_TabletResult.BANNED_WORD)), 2);
			return R_BANNED;
		}
		float now = GetGame().GetTickTime();
		VB_TabletClient client = VB_TabletClient.Get();
		if (now - client.ChatSentAt < VB_TabletClient.CHAT_GAP)
		{
			INT_GC_Client.Notice(VBT.T("#STR_VB_TAB_B_CHAT_WAIT"), 1);
			return R_WAIT;
		}
		client.ChatSentAt = now;
		string body = text;
		if (body.LengthUtf8() > MAX)
			body = body.SubstringUtf8(0, MAX);
		ScriptRPC rpc = client.Begin(VB_TabletOp.CHAT_SEND);
		rpc.Write(body);
		client.Post(rpc);
		// История для стрелки вверх: без повтора подряд и не длиннее, чем держит сам чат.
		array<string> sent = INT_GC_Client.SentHistory();
		if (sent.Count() == 0 || sent.Get(sent.Count() - 1) != body)
			sent.Insert(body);
		while (sent.Count() > INT_GC_Client.SENT_LIMIT)
			sent.RemoveOrdered(0);
		s_WatchVer = client.ResultVersion;
		s_WatchUntil = now + WATCH_SEC;
		return R_SENT;
	}

	// Окно чата уже закрыто, когда сервер отвечает отказом (слишком часто, запрещённое слово):
	// без этого сообщение пропало бы молча. При открытом планшете отказ показывает он сам.
	static void Tick()
	{
		if (s_WatchVer < 0)
			return;
		VB_TabletClient client = VB_TabletClient.Get();
		if (client.ResultVersion != s_WatchVer)
		{
			s_WatchVer = -1;
			int code = client.LastResult;
			if (VBK.ResultIsGood(code) || VB_TabletMenu.Cast(GetGame().GetUIManager().GetMenu()))
				return;
			string why = VBT.T(VBK.ResultText(code));
			INT_GC_Client.Notice(VBT.T("#STR_VB_TAB_GC_FAILED") + " " + why, 2);
			return;
		}
		if (GetGame().GetTickTime() > s_WatchUntil)
			s_WatchVer = -1;
	}
}

modded class INT_GC_Window
{
	Widget m_VbgBox;
	TextWidget m_VbgText;
	ImageWidget m_VbgIcon;
	ButtonWidget m_VbgHit;

	// С 2026-11-22 (вид чата «Экспедиция») каналы — ряд «◆ ГЛОБАЛЬНЫЙ  ◇ РЯДОМ» под шапкой. «◇ КЛАН» третьим справа
	// от «Рядом», тем же видом; не влезает в ширину окна — вкладки нет, Tab тоже без клана.
	override protected void BuildChannels()
	{
		m_VbgBox = null;
		m_VbgText = null;
		m_VbgIcon = null;
		m_VbgHit = null;
		super.BuildChannels();
		if (!VB_TabletGC.CanUse())
			return;
		float size = 11;
		float mark = 9;
		string gl = INT_GC_Look.Caps("#STR_INT_GC_CH_GLOBAL");
		string lo = INT_GC_Look.Caps("#STR_INT_GC_CH_LOCAL");
		string cl = INT_GC_Look.Caps("#STR_VB_TAB_GC_TAG");
		float gw = INT_GC_Measure.Width(gl, size);
		float lw = INT_GC_Measure.Width(lo, size);
		float cw = INT_GC_Measure.Width(cl, size);
		float y = HEADER_H + 1;
		float lx = LEFT + mark + 8 + gw + 22;
		float cx = lx + mark + 8 + lw + 22;
		float cTotal = mark + 8 + cw;
		if (cx + cTotal + 12 > m_W)
			return;
		int muted = INT_GC_Look.Muted();
		float my = (CHAN_H - mark) * 0.5;
		m_VbgBox = INT_UIF_Draw.Frame(m_Root, cx, y, cTotal + 6, CHAN_H, "ChannelClan");
		if (!m_VbgBox)
			return;
		m_VbgIcon = INT_GC_Look.Diamond(m_VbgBox, 0, my, mark, muted, false);
		m_VbgText = INT_UIF_Draw.Text(m_VbgBox, mark + 8, 0, cw + 6, CHAN_H, cl, size, muted);
		Part("ChannelClan", m_VbgBox);
		if (m_Interactive)
			m_VbgHit = INT_UIF_Draw.Hit(m_Root, cx - 4, y, cTotal + 12, CHAN_H, m_Handler);
		PaintChannel(INT_GC_UI.Settings().Channel);
	}

	override void PaintChannel(int channel)
	{
		super.PaintChannel(channel);
		if (!m_VbgBox)
			return;
		bool clan = VB_TabletGC.IsActive();
		if (clan)
		{
			PaintTab(m_ChanGlobalText, m_ChanGlobalMark, false);
			PaintTab(m_ChanLocalText, m_ChanLocalMark, false);
		}
		PaintTab(m_VbgText, m_VbgIcon, clan);
		// Активный клан — цветом клана, а не оранжевым чата.
		if (clan)
		{
			int col = VB_TabletGC.Color();
			if (m_VbgText)
				m_VbgText.SetColor(col);
			if (m_VbgIcon)
				m_VbgIcon.SetColor(col);
		}
		UpdatePlaceholder(channel);
	}

	override void UpdatePlaceholder(int channel)
	{
		super.UpdatePlaceholder(channel);
		if (!m_Placeholder || !m_VbgBox || !VB_TabletGC.IsActive())
			return;
		m_Placeholder.SetText(Widget.TranslateString("#STR_VB_TAB_GC_PH"));
		RefreshPlaceholder();
	}
}

modded class INT_GC_ChatMenu
{
	override void OnBuild(Widget body)
	{
		super.OnBuild(body);
		// Узкое окно строит заголовок без кнопки клана: тогда и канал клана не включается.
		if (!VB_TabletGC.IsActive() || !m_Win || !m_Win.m_VbgBox)
		{
			VB_TabletGC.s_Clan = false;
			return;
		}
		m_Channel = VB_TabletGC.CH_CLAN;
		if (m_Win)
			m_Win.PaintChannel(m_Channel);
	}

	override bool HandleClick(Widget w, int button)
	{
		if (m_Win && w && w == m_Win.m_VbgHit)
		{
			SetChannel(VB_TabletGC.CH_CLAN);
			return true;
		}
		return super.HandleClick(w, button);
	}

	override void SetChannel(int channel)
	{
		if (channel != VB_TabletGC.CH_CLAN)
		{
			VB_TabletGC.s_Clan = false;
			super.SetChannel(channel);
			return;
		}
		if (!VB_TabletGC.CanUse() || !m_Win || !m_Win.m_VbgBox)
			return;
		if (m_Channel == channel)
		{
			FocusEdit();
			return;
		}
		m_Channel = channel;
		VB_TabletGC.s_Clan = true;
		INT_GC_Sfx.Play(INT_GC_Sfx.CLICK);
		m_Win.PaintChannel(channel);
		FocusEdit();
	}

	// Tab по кругу: «Глобальный» → «Рядом» → «Клан». Без кнопки клана как в чате.
	override protected void DoKey(int key)
	{
		if (key != KeyCode.KC_TAB || !m_Win || !m_Win.m_VbgBox || !VB_TabletGC.CanUse())
		{
			super.DoKey(key);
			return;
		}
		float now = GetGame().GetTickTime();
		float last;
		if (m_KeyAt.Find(key, last) && now - last < 0.15)
			return;
		m_KeyAt.Set(key, now);
		int next = INT_GC_Const.CH_LOCAL;
		if (m_Channel == INT_GC_Const.CH_LOCAL)
			next = VB_TabletGC.CH_CLAN;
		else if (m_Channel == VB_TabletGC.CH_CLAN)
			next = INT_GC_Const.CH_GLOBAL;
		SetChannel(next);
	}

	override protected void Submit()
	{
		if (m_Channel != VB_TabletGC.CH_CLAN || !m_Win || !m_Win.m_Edit)
		{
			super.Submit();
			return;
		}
		float now = GetGame().GetTickTime();
		string text = m_Win.m_Edit.GetText();
		if (now - m_OpenedAt < OPEN_GUARD && text == "")
			return;
		text.TrimInPlace();
		if (text == "")
		{
			s_Draft = "";
			RequestClose();
			return;
		}
		// Команда: чат сам решит, куда её. Остальное в клан, а не в глобальный канал.
		if (VB_TabletGC.IsChatCommand(text))
		{
			m_Channel = INT_GC_Const.CH_GLOBAL;
			super.Submit();
			if (VB_TabletGC.IsActive())
				m_Channel = VB_TabletGC.CH_CLAN;
			return;
		}
		int r = VB_TabletGC.Send(text);
		if (r == VB_TabletGC.R_WAIT || r == VB_TabletGC.R_NOCLAN)
		{
			FocusEdit();
			return;
		}
		m_Win.m_Edit.SetText("");
		s_Draft = "";
		m_Recall = -1;
		UpdateCounter();
		if (r == VB_TabletGC.R_SENT)
		{
			INT_GC_Sfx.Play(INT_GC_Sfx.SEND);
			RequestClose();
			return;
		}
		FocusEdit();
	}
}

modded class MissionGameplay
{
	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		VB_TabletGC.Tick();
	}
}
