// INT Banking - client mission hooks:
//   - registers the ATM window (INT_BANK_MENU_ID) with INT_UIFramework;
//   - pumps the client session every frame (answers are queued in OnRPC and dispatched here);
//   - an incoming transfer shows a toast with a sound, window open or not;
//   - preview hook for screenshots, polled every 2 s while its marker exists in the CLIENT profile:
//       int_bank_preview.txt   window with sample data, no server needed; first line = the test command
//                              (overview / deposit / withdraw / transfer / history / result / noroom /
//                              players / busy / noserver, several words allowed)

modded class MissionGameplay
{
	protected float m_INT_Bank_Poll;
	protected string m_INT_Bank_PreviewLine;
	protected float m_INT_Bank_PreviewTimer = -1;
	protected bool m_INT_Bank_Hooked;
	protected string m_INT_Bank_LiveLine;
	protected ref array<string> m_INT_Bank_LiveSteps;
	protected float m_INT_Bank_LiveTimer = -1;

	override void OnInit()
	{
		super.OnInit();
		INT_UIF_MenuRegistry.Register(INT_BANK_MENU_ID, INT_Bank_Menu);
		INT_Bank_UILook.Register();
		if (!m_INT_Bank_Hooked)
		{
			m_INT_Bank_Hooked = true;
			INT_Bank_Client.Get().OnPush().Insert(INT_Bank_OnPush);
		}
	}

	void INT_Bank_OnPush(INT_Bank_Reply r)
	{
		if (!r || r.Op != INT_Bank_Op.PUSH_TRANSFER_IN)
			return;
		string who = r.S1;
		if (who == "")
			who = r.S2;
		string sum = INT_Bank_UILook.Money(r.A);
		string text = INT_Bank_UILook.Fmt("#STR_INT_BANK_PUSH_IN", sum, who);
		INT_Bank_UISfx.Play(INT_Bank_Sfx.NOTIFY);
		INT_UIF_Toast.Show(text, INT_UIF_ToastKind.SUCCESS, 7);
	}

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		INT_Bank_Client.Get().Pump();
		if (m_INT_Bank_PreviewTimer >= 0)
		{
			m_INT_Bank_PreviewTimer = m_INT_Bank_PreviewTimer - timeslice;
			if (m_INT_Bank_PreviewTimer < 0)
				INT_Bank_RunPreviewCommand();
		}
		if (m_INT_Bank_LiveTimer >= 0)
		{
			m_INT_Bank_LiveTimer = m_INT_Bank_LiveTimer - timeslice;
			if (m_INT_Bank_LiveTimer < 0)
				INT_Bank_LiveStep();
		}
		m_INT_Bank_Poll = m_INT_Bank_Poll + timeslice;
		if (m_INT_Bank_Poll < 2.0)
			return;
		m_INT_Bank_Poll = 0;
		INT_Bank_PollMarker();
	}

	// int_bank_live.txt: "open;page 1;amount 5000;submit;..." - steps run 2.5 s apart on the REAL window.
	protected void INT_Bank_PollLive()
	{
		string path = "$profile:int_bank_live.txt";
		if (!FileExist(path))
			return;
		string line = "";
		FileHandle fh = OpenFile(path, FileMode.READ);
		if (fh != 0)
		{
			FGets(fh, line);
			CloseFile(fh);
		}
		line.TrimInPlace();
		if (line == "" || line == m_INT_Bank_LiveLine)
			return;
		m_INT_Bank_LiveLine = line;
		m_INT_Bank_LiveSteps = new array<string>();
		line.Split(";", m_INT_Bank_LiveSteps);
		Print("[INT_BANK] [LIVE] start: " + line);
		m_INT_Bank_LiveTimer = 0.1;
	}

	protected void INT_Bank_LiveStep()
	{
		m_INT_Bank_LiveTimer = -1;
		if (!m_INT_Bank_LiveSteps || m_INT_Bank_LiveSteps.Count() == 0)
		{
			Print("[INT_BANK] [LIVE] done");
			return;
		}
		string step = m_INT_Bank_LiveSteps.Get(0);
		m_INT_Bank_LiveSteps.RemoveOrdered(0);
		step.TrimInPlace();
		if (step == "open")
		{
			INT_Bank_UIState.SetPreview(false, "");
			INT_UIF_Menu.Open(INT_BANK_MENU_ID);
			Print("[INT_BANK] [LIVE] open");
		}
		else if (step == "close")
		{
			INT_Bank_Menu cm = INT_Bank_Menu.Cast(GetGame().GetUIManager().GetMenu());
			if (cm)
				cm.RequestClose();
		}
		else
		{
			INT_Bank_Menu menu = INT_Bank_Menu.Cast(GetGame().GetUIManager().GetMenu());
			if (menu)
				menu.LiveCommand(step);
			else
				Print("[INT_BANK] [LIVE] no window for: " + step);
		}
		m_INT_Bank_LiveTimer = 2.5;
	}

	protected void INT_Bank_PollMarker()
	{
		if (!GetGame().GetPlayer())
			return;
		INT_Bank_PollLive();
		string path = "$profile:int_bank_preview.txt";
		if (FileExist(path))
		{
			string line = "";
			FileHandle fh = OpenFile(path, FileMode.READ);
			if (fh != 0)
			{
				FGets(fh, line);
				CloseFile(fh);
			}
			line.TrimInPlace();
			if (line == "")
				line = "overview";
			if (line == m_INT_Bank_PreviewLine)
				return;
			m_INT_Bank_PreviewLine = line;
			UIManager um = GetGame().GetUIManager();
			INT_Bank_Menu cur = INT_Bank_Menu.Cast(um.GetMenu());
			if (cur)
				cur.CloseNow();
			else if (um.GetMenu())
				um.CloseAll();
			INT_Bank_UIState.SetPreview(true, line);
			INT_UIF_Menu.Open(INT_BANK_MENU_ID);
			m_INT_Bank_PreviewTimer = 0.9;
			return;
		}
		if (m_INT_Bank_PreviewLine != "")
		{
			m_INT_Bank_PreviewLine = "";
			INT_Bank_UIState.SetPreview(false, "");
			INT_Bank_Menu open = INT_Bank_Menu.Cast(GetGame().GetUIManager().GetMenu());
			if (open)
				open.RequestClose();
		}
	}

	protected void INT_Bank_RunPreviewCommand()
	{
		m_INT_Bank_PreviewTimer = -1;
		INT_Bank_Menu menu = INT_Bank_Menu.Cast(GetGame().GetUIManager().GetMenu());
		if (menu)
			menu.OnTestCommand(m_INT_Bank_PreviewLine);
	}

	override void OnMissionFinish()
	{
		if (m_INT_Bank_Hooked)
		{
			m_INT_Bank_Hooked = false;
			INT_Bank_Client c = INT_Bank_Client.Get();
			if (c && c.OnPush())
				c.OnPush().Remove(INT_Bank_OnPush);
		}
		INT_Bank_UIState.SetPreview(false, "");
		INT_Bank_Client.Reset();
		super.OnMissionFinish();
	}
}
