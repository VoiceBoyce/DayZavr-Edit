// Банкомат на клиенте: мини-окно «Положить» и «Снять», своя голограмма установки, запуск из действия.

// Окно банкомата. Живёт на экране «Склад денег»: действие у банкомата открывает планшет на нём.
// Все проверки делает сервер, здесь только то, что клиент и так знает: склад и деньги при себе.
class VB_TabletAtmWindow extends Managed
{
	static const float BUSY_SEC = 3.0;

	protected VB_TabletMenu m_Menu;
	protected ref VBT_Modal m_Modal;
	protected int m_State;	// 0 окно, 1 нет права, 2 инвентарь заполнен
	protected int m_Mode;	// 0 положить, 1 снять
	protected string m_Amt;
	protected string m_ErrText;
	protected float m_BusyAt = -100;
	protected ref VBT_Input m_Field;
	protected ref VBT_Button m_Ok;
	protected ref VBT_Button m_Back;
	protected ref array<ref VBT_Button> m_Quick;
	protected ref array<int> m_QuickSum;
	protected ref VBT_Button m_All;
	protected ref array<Widget> m_TabHits;
	protected Widget m_Slot;
	protected Widget m_Sums;
	protected TextWidget m_Note;
	protected TextWidget m_Err;

	void VB_TabletAtmWindow(VB_TabletMenu menu)
	{
		m_Menu = menu;
	}

	bool IsOpen()
	{
		return m_Modal && m_Modal.IsOpen();
	}

	void Open()
	{
		m_Mode = 0;
		m_Amt = "";
		if (VB_TabletClient.Get().Can(VB_ClanPerm.BANK))
			Paint();
		else
			PaintDenied();
	}

	void Close()
	{
		SetFocus(null);
		if (m_Modal)
			m_Modal.Close();
		m_Modal = null;
		m_Field = null;
		m_Ok = null;
		m_Back = null;
		m_All = null;
		m_Slot = null;
		m_Sums = null;
		m_Note = null;
		m_Err = null;
	}

	protected void PaintDenied()
	{
		Close();
		m_State = 1;
		float w = 420;
		float h = 196;
		m_Modal = new VBT_Modal(m_Menu, w, h);
		Widget b = m_Modal.Body;
		INT_UIF_Draw.Text(b, 28, 22, w - 90, 32, "#STR_VB_TAB_C_ATM_TITLE", 22, VB_C_TEXT);
		INT_UIF_Draw.MText(b, 28, 66, w - 56, 44, "#STR_VB_TAB_C_ATM_DENIED", 15, VB_C_DIM);
		m_Back = new VBT_Button(b, 28, h - 68, w - 56, 44, "#STR_VB_TAB_C_OK", false, "", m_Menu.GetHitHandler());
	}

	// Операция отменена целиком: на землю ничего не падает.
	protected void PaintNoRoom()
	{
		Close();
		m_State = 2;
		float w = 420;
		float h = 236;
		m_Modal = new VBT_Modal(m_Menu, w, h);
		Widget b = m_Modal.Body;
		INT_UIF_Draw.Text(b, 28, 22, w - 90, 32, "#STR_VB_TAB_C_FULL_TITLE", 22, VB_C_TEXT);
		INT_UIF_Draw.MText(b, 28, 66, w - 56, 88, "#STR_VB_TAB_C_FULL_TEXT", 15, VB_C_DIM);
		m_Back = new VBT_Button(b, 28, h - 68, w - 56, 44, "#STR_VB_TAB_C_OK", true, "", m_Menu.GetHitHandler());
	}

	protected void Paint()
	{
		Close();
		VB_TabletClient client = VB_TabletClient.Get();
		VB_Clan c = client.Clan;
		if (!c)
			return;
		m_State = 0;
		float w = 440;
		float h = 440;
		m_Modal = new VBT_Modal(m_Menu, w, h);
		Widget b = m_Modal.Body;
		VBK.Emblem(b, 28, 22, 32, c.EmbShape, c.EmbSymbol, c.Color);
		INT_UIF_Draw.Text(b, 70, 20, 300, 36, VBT.Fit(c.Title, 300, 20), 20, VB_C_TEXT);
		VBT.Cap(b, 28, 74, 180, "#STR_VB_TAB_C_ON_BANK");
		VBT.Cap(b, 232, 74, 180, "#STR_VB_TAB_C_ON_HAND");
		m_Sums = INT_UIF_Draw.Frame(b, 0, 90, w, 40);
		PaintSums();
		m_TabHits = new array<Widget>();
		array<string> names = {"#STR_VB_TAB_C_ATM_PUT", "#STR_VB_TAB_C_ATM_TAKE"};
		for (int i = 0; i < 2; i++)
		{
			float x = 28 + i * 196;
			bool on = i == m_Mode;
			int fill = VB_C_PANEL2;
			int border = VB_C_LINE2;
			int ink = VB_C_TEXT;
			if (on)
			{
				fill = VB_C_ACCENT;
				border = 0;
				ink = VB_C_ON;
			}
			INT_UIF_Surface.Box(b, x, 146, 188, 38, 8, fill, border);
			INT_UIF_Draw.Text(b, x, 146, 188, 38, names.Get(i), 15, ink, INT_UIF_Align.CENTER);
			Widget tabHit = INT_UIF_Draw.Hit(b, x, 146, 188, 38, m_Menu.GetHitHandler());
			tabHit.SetName("VBT_AtmTab_" + i.ToString());
			m_TabHits.Insert(tabHit);
		}
		m_Field = new VBT_Input(b, 28, 196, w - 56, 52, "#STR_VB_TAB_C_ATM_SUM", 9, m_Amt);
		m_Quick = new array<ref VBT_Button>();
		m_QuickSum = new array<int>();
		m_QuickSum.Insert(10000);
		m_QuickSum.Insert(100000);
		m_QuickSum.Insert(500000);
		for (int k = 0; k < 3; k++)
		{
			int sum = m_QuickSum.Get(k);
			m_Quick.Insert(new VBT_Button(b, 28 + k * 98, 256, 92, 32, VBT.Num(sum), false, "", m_Menu.GetHitHandler(), 13));
		}
		m_All = new VBT_Button(b, 322, 256, 90, 32, "#STR_VB_TAB_C_ATM_ALL", false, "", m_Menu.GetHitHandler(), 13);
		m_Note = INT_UIF_Draw.Text(b, 28, 296, w - 56, 22, "", 14, VB_C_DIM);
		m_Err = INT_UIF_Draw.Text(b, 28, 320, w - 56, 40, "", 14, VB_C_BAD);
		m_Slot = INT_UIF_Draw.Frame(b, 28, h - 68, w - 56, 48);
		Sync();
		m_Field.Focus();
	}

	protected void PaintSums()
	{
		if (!m_Sums)
			return;
		VB_TabletClient client = VB_TabletClient.Get();
		INT_UIF_Widgets.ClearChildren(m_Sums);
		int bank = 0;
		if (client.Clan)
			bank = client.Clan.Bank;
		VBK.Money(m_Sums, 28, 6, bank, 22);
		VBK.Money(m_Sums, 232, 6, client.Me.Cash, 22, VB_C_TEXT);
	}

	// Подпись кнопки, «станет на складе» и текст ошибки.
	protected void Sync()
	{
		VB_TabletClient client = VB_TabletClient.Get();
		VB_Clan c = client.Clan;
		if (!c || !m_Slot)
			return;
		int v = m_Amt.ToInt();
		int after = c.Bank + v;
		if (m_Mode == 1)
			after = c.Bank - v;
		if (after < 0)
			after = 0;
		int days = after / VB_TabletClient.Get().ClanTax;
		SetError("");
		string note = "";
		int noteColor = VB_C_DIM;
		if (v > 0)
		{
			note = VBT.T("#STR_VB_TAB_C_ATM_AFTER") + " " + VBT.Num(after);
			if (c.TaxFree <= 0)
			{
				note = note + " · " + VBA_Util.Sub("#STR_VB_TAB_C_ATM_TAX", "{n}", days.ToString());
				if (days <= VBA_TAX_LOW)
					noteColor = VB_C_BAD;
			}
		}
		m_Note.SetText(note);
		m_Note.SetColor(noteColor);
		string name = "#STR_VB_TAB_C_ATM_PUT";
		if (m_Mode == 1)
			name = "#STR_VB_TAB_C_ATM_TAKE";
		string label = name;
		if (v > 0)
			label = VBT.T(name) + " " + VBT.Num(v);
		INT_UIF_Widgets.ClearChildren(m_Slot);
		m_Ok = new VBT_Button(m_Slot, 0, 0, 384, 48, label, true, "", m_Menu.GetHitHandler(), 16);
		m_Ok.Hit.SetName("VBT_AtmOk");
		if (v <= 0)
		{
			m_Ok.Hit.Show(false);
			m_Ok.Bg.SetFill(VB_C_PANEL3);
		}
	}

	protected int MaxSum()
	{
		VB_TabletClient client = VB_TabletClient.Get();
		VB_Clan c = client.Clan;
		if (!c)
			return 0;
		if (m_Mode == 1)
			return c.Bank;
		int room = Math.Max(0, VB_TabletClient.Get().BankMax - c.Bank);
		return Math.Min(client.Me.Cash, room);
	}

	protected void SetAmount(int v)
	{
		m_Amt = "";
		if (v > 0)
			m_Amt = v.ToString();
		if (m_Field)
			m_Field.Set(m_Amt);
		Sync();
	}

	protected void Submit()
	{
		VB_TabletClient client = VB_TabletClient.Get();
		VB_Clan c = client.Clan;
		int v = m_Amt.ToInt();
		if (!c || v <= 0)
			return;
		float now = GetGame().GetTickTime();
		if (now - m_BusyAt < BUSY_SEC)
			return;
		int op = VB_TabletOp.ATM_DEPOSIT;
		if (m_Mode == 1)
		{
			if (v > c.Bank)
			{
				SetError(VBT.T("#STR_VB_TAB_C_ATM_NO_FUNDS") + " " + VBT.Num(c.Bank));
				return;
			}
			op = VB_TabletOp.ATM_WITHDRAW;
		}
		else if (v > client.Me.Cash)
		{
			SetError(VBT.T("#STR_VB_TAB_C_ATM_NO_CASH") + " " + VBT.Num(client.Me.Cash));
			return;
		}
		// Повторный клик ничего не отправит, пока не пришёл ответ. Сервер держит свою паузу.
		m_BusyAt = now;
		ScriptRPC rpc = client.Begin(op);
		rpc.Write(v);
		client.Post(rpc);
	}

	void OnResult(int code)
	{
		m_BusyAt = -100;
		if (!IsOpen() || m_State != 0)
			return;
		if (code == VB_TabletResult.NO_ROOM)
		{
			PaintNoRoom();
			return;
		}
		if (code == VB_TabletResult.OK)
		{
			SetAmount(0);
			return;
		}
		SetError(VBT.T(VBK.ResultText(code)));
	}

	void Refresh()
	{
		if (!VB_TabletClient.Get().Clan)
		{
			Close();
			return;
		}
		if (m_State != 0)
			return;
		PaintSums();
		// Sync сбрасывает ошибку: после новых данных с сервера текст ошибки остаётся.
		string keep = m_ErrText;
		Sync();
		SetError(keep);
	}

	protected void SetError(string text)
	{
		m_ErrText = text;
		if (m_Err)
			m_Err.SetText(text);
	}

	void OnInput(VBT_Input field)
	{
		if (field != m_Field)
			return;
		string raw = field.Get();
		string digits = "";
		string allowed = "0123456789";
		int n = raw.Length();
		for (int i = 0; i < n; i++)
		{
			string ch = raw.Get(i);
			if (allowed.IndexOf(ch) >= 0 && (digits != "" || ch != "0"))
				digits = digits + ch;
		}
		m_Amt = digits;
		if (digits != raw)
			field.Set(digits);
		Sync();
	}

	void OnSubmit(VBT_Input field)
	{
		if (field == m_Field)
			Submit();
	}

	bool OnHit(Widget w)
	{
		if (!IsOpen())
			return false;
		if (m_Modal.IsDismiss(w))
		{
			if (m_State == 2)
				Paint();
			else
				Close();
			return true;
		}
		if (m_State != 0)
		{
			if (m_Back && w == m_Back.Hit)
			{
				if (m_State == 2)
					Paint();
				else
					Close();
			}
			return true;
		}
		int tab = m_TabHits.Find(w);
		if (tab >= 0)
		{
			if (tab != m_Mode)
			{
				m_Mode = tab;
				m_Amt = "";
				Paint();
			}
			return true;
		}
		for (int i = 0; i < m_Quick.Count(); i++)
		{
			if (w == m_Quick.Get(i).Hit)
			{
				SetAmount(m_Amt.ToInt() + m_QuickSum.Get(i));
				return true;
			}
		}
		if (w == m_All.Hit)
		{
			SetAmount(MaxSum());
			return true;
		}
		if (m_Ok && w == m_Ok.Hit)
		{
			Submit();
			return true;
		}
		return false;
	}

	bool OnHover(Widget w, bool enter)
	{
		if (!IsOpen())
			return false;
		if (m_Back && w == m_Back.Hit)
		{
			m_Back.Hover(enter);
			return true;
		}
		if (m_State != 0)
			return false;
		foreach (VBT_Button b : m_Quick)
		{
			if (w == b.Hit)
			{
				b.Hover(enter);
				return true;
			}
		}
		if (w == m_All.Hit)
		{
			m_All.Hover(enter);
			return true;
		}
		if (m_Ok && w == m_Ok.Hit)
		{
			m_Ok.Hover(enter);
			return true;
		}
		return false;
	}
}

// Своя голограмма: ванильная привязана к предмету в руках. Объект-призрак только на клиенте.
// Левая кнопка мыши ставит, правая отменяет. Положение проверит и сохранит сервер.
class VB_TabletAtmPlacer extends Managed
{
	static const float REACH_M = 7.0;
	static const float NEAR_M = 1.5;
	static const float FRONT_YAW = 0.0;	// поворот модели лицом к игроку
	static const string GHOST = "VB_TabletAtmGhost";

	protected static ref VB_TabletAtmPlacer s_Active;

	protected Object m_Ghost;
	protected Widget m_Bar;
	protected bool m_Armed;
	protected vector m_Spot;
	protected float m_Yaw;

	static void Begin()
	{
		if (!s_Active)
			s_Active = new VB_TabletAtmPlacer();
	}

	static void Update()
	{
		if (!s_Active)
			return;
		if (!s_Active.Advance())
			Finish();
	}

	static void Finish()
	{
		if (!s_Active)
			return;
		s_Active.Release();
		s_Active = null;
	}

	protected void Release()
	{
		if (m_Ghost)
			GetGame().ObjectDelete(m_Ghost);
		m_Ghost = null;
		if (m_Bar)
			m_Bar.Unlink();
		m_Bar = null;
		if (m_Armed)
		{
			Mission mission = GetGame().GetMission();
			if (mission)
				mission.RemoveActiveInputExcludes({"actions"}, true);
		}
		m_Armed = false;
	}

	// Голограмма появляется, когда планшет закрылся и кнопка мыши отпущена: клик по кнопке не поставит банкомат.
	protected void Arm()
	{
		m_Armed = true;
		Mission mission = GetGame().GetMission();
		if (mission)
			mission.AddActiveInputExcludes({"actions"});
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		m_Ghost = GetGame().CreateObjectEx(GHOST, player.GetPosition(), ECE_LOCAL | ECE_KEEPHEIGHT | ECE_NOSURFACEALIGN);
		BuildBar();
	}

	protected void BuildBar()
	{
		float f = INT_UIF_Scale.Factor();
		float ox = (INT_UIF_Scale.ScreenW() / f - 1920) * 0.5;
		float oy = (INT_UIF_Scale.ScreenH() / f - 1080) * 0.5;
		m_Bar = INT_UIF_Draw.Frame(null, ox + 690, oy + 920, 540, 76);
		m_Bar.SetSort(INT_UIF_SORT_POPUP);
		INT_UIF_Surface.Box(m_Bar, 0, 0, 540, 76, 10, VB_C_PANEL, VB_C_LINE2);
		VBT.Corners(m_Bar, 540, 76, VB_C_ACCENT, 14);
		INT_UIF_Surface.Box(m_Bar, 16, 18, 40, 40, 8, VB_C_PANEL2);
		VBT.Ico(m_Bar, 26, 28, 20, "vault", VB_C_GOLD);
		INT_UIF_Draw.Text(m_Bar, 70, 14, 180, 26, "#STR_VB_TAB_C_ATM_TITLE", 16, VB_C_TEXT);
		INT_UIF_Draw.Text(m_Bar, 70, 40, 180, 22, "#STR_VB_TAB_C_ATM_PICK", 14, VB_C_DIM);
		// Порядок как у кнопок мыши: слева поставить, справа отмена.
		INT_UIF_Surface.Box(m_Bar, 262, 20, 124, 36, 8, VB_C_ACCENT);
		INT_UIF_Draw.Text(m_Bar, 262, 20, 124, 36, "#STR_VB_TAB_C_ATM_PLACE", 14, VB_C_ON, INT_UIF_Align.CENTER);
		INT_UIF_Surface.Box(m_Bar, 396, 20, 124, 36, 8, VB_C_PANEL2, VB_C_LINE2);
		INT_UIF_Draw.Text(m_Bar, 396, 20, 124, 36, "#STR_VB_TAB_C_ATM_CANCEL", 14, VB_C_TEXT, INT_UIF_Align.CENTER);
	}

	// Точка на поверхности там, куда смотрит игрок: взгляд в стену даёт пол у этой стены.
	protected void MoveGhost(PlayerBase player)
	{
		vector from = GetGame().GetCurrentCameraPosition();
		vector dir = GetGame().GetCurrentCameraDirection();
		vector to = from + dir * REACH_M;
		vector head = player.GetDirection();
		vector own = player.GetPosition();
		vector hit;
		vector normal;
		int comp;
		vector spot = own + head * REACH_M * 0.5;
		if (DayZPhysics.RaycastRV(from, to, hit, normal, comp, null, m_Ghost, player, false, false, ObjIntersectView))
			spot = hit - dir * 0.3;
		if (vector.DistanceSq(spot, own) < NEAR_M * NEAR_M)
			spot = own + head * NEAR_M;
		vector top = spot;
		top[1] = top[1] + 0.1;
		vector low = top;
		low[1] = low[1] - 8.0;
		vector ground;
		if (DayZPhysics.RaycastRV(top, low, ground, normal, comp, null, m_Ghost, player, false, false, ObjIntersectView))
			spot = ground;
		else
			spot[1] = GetGame().SurfaceY(spot[0], spot[2]);
		vector angles = player.GetOrientation();
		m_Yaw = Math.NormalizeAngle(angles[0] + FRONT_YAW);
		m_Spot = spot;
		if (m_Ghost)
		{
			m_Ghost.SetPosition(spot);
			m_Ghost.SetOrientation(Vector(m_Yaw, 0, 0));
		}
	}

	protected void Send()
	{
		VB_TabletClient client = VB_TabletClient.Get();
		float x = m_Spot[0];
		float y = m_Spot[1];
		float z = m_Spot[2];
		float yaw = m_Yaw;
		ScriptRPC rpc = client.Begin(VB_TabletOp.ATM_PLACE);
		rpc.Write(x);
		rpc.Write(y);
		rpc.Write(z);
		rpc.Write(yaw);
		client.Post(rpc);
	}

#ifdef DIAG_DEVELOPER
	// Стенд не умеет нажимать мышь: в диагностике банкомат ставится клавишей.
	static void DiagPlace()
	{
		if (!s_Active)
			return;
		s_Active.Send();
		Finish();
	}
#endif

	// false: режим закончен.
	bool Advance()
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player || !VB_TabletMenu.CanOpen())
			return false;
		// Пока закрывается планшет, ждём. Любое окно поверх режима установки его отменяет.
		if (GetGame().GetUIManager().GetMenu())
			return !m_Armed;
		bool left = (GetMouseState(MouseState.LEFT) & 0x80000000) != 0;
		bool right = (GetMouseState(MouseState.RIGHT) & 0x80000000) != 0;
		if (!m_Armed)
		{
			if (left || right)
				return true;
			Arm();
		}
		MoveGhost(player);
		if (left)
		{
			Send();
			return false;
		}
		return !right;
	}
}

modded class MissionGameplay
{
	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		// HELLO сразу после входа: шапка клана нужна действию у банкомата, сервер по нему шлёт предупреждение о налоге.
		VB_TabletClient client = VB_TabletClient.Get();
		client.RequestHello();
		VB_TabletAtmPlacer.Update();
		if (client.AtmWanted)
		{
			client.AtmWanted = false;
			VB_TabletPageBank.s_OpenAtm = true;
			VB_TabletMenu.OpenAtm();
		}
	}

	override void OnMissionFinish()
	{
		VB_TabletAtmPlacer.Finish();
		super.OnMissionFinish();
	}
}
