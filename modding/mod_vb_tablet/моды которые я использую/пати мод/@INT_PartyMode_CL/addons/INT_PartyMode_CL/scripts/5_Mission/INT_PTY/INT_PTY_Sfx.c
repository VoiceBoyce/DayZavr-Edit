// INT PartyMode (client) — 2D interface, ping and notification sounds. Files: tools/gen_pty_sfx.py ->
// data/sound/int_pty_*.ogg; sets: config.cpp CfgSoundSets INT_PTY_*_SoundSet. Same chain as INT_FrameworkSkills /
// INT_GlobalChat / INT_KillFeed (proven on 1.30): SoundParams -> SoundObjectBuilder -> SoundObject.SetKind(WAVEUI)
// -> SoundScene.Play2D -> AbstractWave.Play; the last waves stay referenced until they finish.
// Player switches (INT_PTY_Settings): ClickSounds = interface sounds, NotifySounds = notifications and pings.
// Never plays on a dedicated server.

class INT_PTY_Sfx
{
	// interface (ClickSounds)
	static const string SET_CLICK = "INT_PTY_Click_SoundSet";
	static const string SET_HOVER = "INT_PTY_Hover_SoundSet";
	static const string SET_TAB = "INT_PTY_Tab_SoundSet";
	static const string SET_TOGGLE = "INT_PTY_Toggle_SoundSet";
	static const string SET_OPEN = "INT_PTY_Open_SoundSet";
	static const string SET_CLOSE = "INT_PTY_Close_SoundSet";
	// pings + notifications (NotifySounds)
	static const string SET_PING_IN = "INT_PTY_PingIn_SoundSet";
	static const string SET_PING_MINE = "INT_PTY_PingMine_SoundSet";
	static const string SET_PING_OUT = "INT_PTY_PingOut_SoundSet";
	static const string SET_ONLINE = "INT_PTY_NOnline_SoundSet";
	static const string SET_INVITE = "INT_PTY_NInvite_SoundSet";
	static const string SET_ACCEPT = "INT_PTY_NAccept_SoundSet";
	static const string SET_WARN = "INT_PTY_NWarn_SoundSet";
	static const string SET_DEAD = "INT_PTY_NDead_SoundSet";

	static const float SAME_SET_GAP = 0.05;		// s, the same interface sound again
	static const float HOVER_GAP = 0.08;		// s, hover ticks while the mouse sweeps over a list
	static const float NOTIFY_GAP = 0.25;		// s, the same ping / notification again (a burst = one sound)
	static const float UI_GAP = 0.015;			// s, any two interface sounds
	static const int KEEP_WAVES = 8;

	protected static ref map<string, float> s_LastAt;
	protected static ref array<string> s_Missing;
	protected static ref array<AbstractWave> s_Waves;
	protected static int s_WaveAt;
	protected static float s_LastUi = -100;

	// ---- entry points (other code calls these) ----

	static void Click()
	{
		Play(SET_CLICK);
	}

	static void Hover()
	{
		Play(SET_HOVER);
	}

	static void Tab()
	{
		Play(SET_TAB);
	}

	static void Toggle()
	{
		Play(SET_TOGGLE);
	}

	static void Open()
	{
		Play(SET_OPEN);
	}

	static void Close()
	{
		Play(SET_CLOSE);
	}

	// A ping appeared: my own (lower, shorter) or a friend's.
	static void Ping(bool mine)
	{
		if (mine)
			Play(SET_PING_MINE);
		else
			Play(SET_PING_IN);
	}

	static void PingOut()
	{
		Play(SET_PING_OUT);
	}

	static void Online()
	{
		Play(SET_ONLINE);
	}

	static void Invite()
	{
		Play(SET_INVITE);
	}

	static void Accept()
	{
		Play(SET_ACCEPT);
	}

	static void Warn()
	{
		Play(SET_WARN);
	}

	static void Dead()
	{
		Play(SET_DEAD);
	}

	// Toast kind -> sound: INFO = new request, SUCCESS = friendship made, WARNING = declined / limit, ERROR = dark hit.
	static void Notice(int kind)
	{
		if (kind == INT_UIF_ToastKind.INFO)
			Play(SET_INVITE);
		else if (kind == INT_UIF_ToastKind.SUCCESS)
			Play(SET_ACCEPT);
		else if (kind == INT_UIF_ToastKind.WARNING)
			Play(SET_WARN);
		else if (kind == INT_UIF_ToastKind.ERROR)
			Play(SET_DEAD);
	}

	// ---- player ----

	// true = the set belongs to pings / notifications (NotifySounds), false = interface (ClickSounds).
	static bool IsNotify(string setName)
	{
		if (setName == SET_PING_IN || setName == SET_PING_MINE || setName == SET_PING_OUT)
			return true;
		if (setName == SET_ONLINE || setName == SET_INVITE || setName == SET_ACCEPT)
			return true;
		if (setName == SET_WARN || setName == SET_DEAD)
			return true;
		return false;
	}

	// Plays one CfgSoundSets entry as a 2D UI sound on this client.
	static void Play(string setName)
	{
		if (!GetGame() || GetGame().IsDedicatedServer())
			return;
		if (setName == "")
			return;
		bool notify = IsNotify(setName);
		INT_PTY_Settings st = INT_PTY_SettingsStore.Get();
		if (st)
		{
			if (notify && st.NotifySounds != 1)
				return;
			if (!notify && st.ClickSounds != 1)
				return;
		}
		EnsureInit();
		if (s_Missing.Find(setName) >= 0)
			return;
		float now = GetGame().GetTickTime();
		float last;
		if (!s_LastAt.Find(setName, last))
			last = -100;
		float gap = SAME_SET_GAP;
		if (notify)
			gap = NOTIFY_GAP;
		else if (setName == SET_HOVER)
			gap = HOVER_GAP;
		if (now - last < gap)
			return;
		if (!notify && now - s_LastUi < UI_GAP)
			return;
		SoundParams soundParams = new SoundParams(setName);
		if (!soundParams.IsValid())
		{
			s_Missing.Insert(setName);
			INT_PTY_Log.Warn("набор звука не найден: " + setName);
			return;
		}
		SoundObjectBuilder soundBuilder = new SoundObjectBuilder(soundParams);
		SoundObject soundObject = soundBuilder.BuildSoundObject();
		if (!soundObject)
			return;
		soundObject.SetKind(WaveKind.WAVEUI);
		AbstractSoundScene scene = GetGame().GetSoundScene();
		if (!scene)
			return;
		AbstractWave wave = scene.Play2D(soundObject, soundBuilder);
		if (!wave)
			return;
		wave.Loop(false);
		wave.Play();
		s_LastAt.Set(setName, now);
		if (!notify)
			s_LastUi = now;
		s_Waves.Set(s_WaveAt, wave);
		s_WaveAt++;
		if (s_WaveAt >= KEEP_WAVES)
			s_WaveAt = 0;
	}

	protected static void EnsureInit()
	{
		if (s_Waves)
			return;
		s_LastAt = new map<string, float>();
		s_Missing = new array<string>();
		s_Waves = new array<AbstractWave>();
		for (int i = 0; i < KEEP_WAVES; i++)
		{
			s_Waves.Insert(null);
		}
		s_WaveAt = 0;
	}
}
