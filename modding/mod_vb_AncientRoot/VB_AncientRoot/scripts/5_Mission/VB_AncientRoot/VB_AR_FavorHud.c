// Расположение леса в худе — «живая ветвь» под быстрыми слотами (ТЗ_сервер_18.md п. 2б.11; выбор пользователя
// 2026-11-16: стиль «живая ветвь» на месте полосы под слотами; прежний знак справа убран) — только клиент.
// Шкала −100..+100, ноль посередине; насечки на границах ступеней (−60, −20, +20, +60). В плюсе от центра вправо
// распускаются листья и золотые почки (маска vb_ar_mask_right), в минусе от центра влево ветка чернеет и обрастает
// шипами (vb_ar_mask_left): SetMaskProgress = |значение|/100. Почка-маркер на текущем значении, цвет по ступени; слева
// руна Альгиз, справа название ступени; при изменении над почкой всплывает «+10» / «-25», а ветвь плавно доезжает.
// Раскладка gui/layouts/vb_ar_favor.layout (единицы 1/1080 высоты экрана), текстуры — tools/gen_ar_branch.py и gen_ar_hud.py.
// Показывается сам: у рощи, ночью в лесу, недолго после изменения и всегда у «Врага леса» и «Проклятого».
// Прячется при открытом меню/инвентаре, мёртвом или без сознания. Значение присылает сервер (VB_AR_Favor.RPC_FAVOR).

class VB_AR_FavorHud
{
	static const string LAYOUT = "VB_AncientRoot/gui/layouts/vb_ar_favor.layout";
	static const string TEX_DISC = "VB_AncientRoot/gui/vb_ar_disc_ca.paa";
	static const string TEX_RUNE = "VB_AncientRoot/gui/vb_ar_rune_ca.paa";
	static const string TEX_BRANCH = "VB_AncientRoot/gui/vb_ar_branch_ca.paa";
	static const string TEX_BLOOM = "VB_AncientRoot/gui/vb_ar_bloom_ca.paa";
	static const string TEX_THORN = "VB_AncientRoot/gui/vb_ar_thorn_ca.paa";
	static const string TEX_BUD = "VB_AncientRoot/gui/vb_ar_bud_ca.paa";
	static const string MASK_RIGHT = "VB_AncientRoot/gui/vb_ar_mask_right_ca.paa";
	static const string MASK_LEFT = "VB_AncientRoot/gui/vb_ar_mask_left_ca.paa";

	static const float TRACK_X = 120;				// позиция и ширина ветки в корне раскладки
	static const float TRACK_W = 400;
	static const float BUD_SIZE = 22;
	static const float BUD_Y = 13;

	static const int SHOW_AFTER_CHANGE_MS = 6000;	// после изменения полоса видна столько
	static const int DELTA_MS = 2500;				// «+10» висит столько
	static const int CHECK_MS = 2000;				// как часто пересчитывать «в лесу»
	static const float GROVE_NEAR = 60;
	static const float FOREST_RADIUS = 20;
	static const int FOREST_TREES = 8;
	static const float FADE_SPEED = 3.0;			// единиц прозрачности в секунду
	static const float GROW_SPEED = 30.0;			// с какой скоростью ветвь доезжает до нового значения, ед./с

	static const int COLOR_FRIEND = 0xFFF0C850;		// золотой
	static const int COLOR_OWN = 0xFF7CC45A;		// зелёный
	static const int COLOR_STRANGER = 0xFFB4B4B4;	// серый
	static const int COLOR_ENEMY = 0xFFE88A2A;		// оранжевый
	static const int COLOR_CURSED = 0xFFE03A30;		// красный
	static const int COLOR_DISC = 0xFF000000;
	static const int COLOR_WHITE = 0xFFFFFFFF;

	protected static ref VB_AR_FavorHud s_VB_Instance;

	protected Widget m_VB_Root;
	protected ImageWidget m_VB_Disc;
	protected ImageWidget m_VB_Rune;
	protected ImageWidget m_VB_Branch;
	protected ImageWidget m_VB_Bloom;
	protected ImageWidget m_VB_Thorn;
	protected ImageWidget m_VB_Bud;
	protected TextWidget m_VB_Name;
	protected TextWidget m_VB_Delta;
	protected float m_VB_Alpha;
	protected float m_VB_Time;
	protected float m_VB_Shown;					// показанное значение, плавно догоняет настоящее
	protected bool m_VB_ShownSet;
	protected int m_VB_NextCheck;
	protected bool m_VB_InForest;
	protected int m_VB_ShownTier = -100;

	static void VB_Tick(float dt)
	{
		if (g_Game.IsDedicatedServer())
			return;
		if (!s_VB_Instance)
			s_VB_Instance = new VB_AR_FavorHud();
		s_VB_Instance.VB_Update(dt);
	}

	static void VB_Shutdown()
	{
		if (s_VB_Instance)
			s_VB_Instance.VB_Destroy();
		s_VB_Instance = null;
	}

	protected bool VB_Create()
	{
		if (m_VB_Root)
			return true;
		if (!g_Game.GetWorkspace())
			return false;
		m_VB_Root = g_Game.GetWorkspace().CreateWidgets(LAYOUT);
		if (!m_VB_Root)
			return false;
		m_VB_Disc = ImageWidget.Cast(m_VB_Root.FindAnyWidget("VB_AR_FavorDisc"));
		m_VB_Rune = ImageWidget.Cast(m_VB_Root.FindAnyWidget("VB_AR_FavorRune"));
		m_VB_Branch = ImageWidget.Cast(m_VB_Root.FindAnyWidget("VB_AR_FavorBranch"));
		m_VB_Bloom = ImageWidget.Cast(m_VB_Root.FindAnyWidget("VB_AR_FavorBloom"));
		m_VB_Thorn = ImageWidget.Cast(m_VB_Root.FindAnyWidget("VB_AR_FavorThorn"));
		m_VB_Bud = ImageWidget.Cast(m_VB_Root.FindAnyWidget("VB_AR_FavorBud"));
		m_VB_Name = TextWidget.Cast(m_VB_Root.FindAnyWidget("VB_AR_FavorName"));
		m_VB_Delta = TextWidget.Cast(m_VB_Root.FindAnyWidget("VB_AR_FavorDelta"));
		if (!m_VB_Disc || !m_VB_Rune || !m_VB_Branch || !m_VB_Bloom || !m_VB_Thorn || !m_VB_Bud || !m_VB_Name || !m_VB_Delta)
		{
			m_VB_Root.Unlink();
			m_VB_Root = null;
			return false;
		}
		m_VB_Disc.LoadImageFile(0, TEX_DISC);
		m_VB_Rune.LoadImageFile(0, TEX_RUNE);
		m_VB_Branch.LoadImageFile(0, TEX_BRANCH);
		m_VB_Bloom.LoadImageFile(0, TEX_BLOOM);
		m_VB_Thorn.LoadImageFile(0, TEX_THORN);
		m_VB_Bud.LoadImageFile(0, TEX_BUD);
		m_VB_Bloom.LoadMaskTexture(MASK_RIGHT);
		m_VB_Thorn.LoadMaskTexture(MASK_LEFT);
		m_VB_Bloom.SetMaskTransitionWidth(0.04);	// мягкий край там, где ветвь кончает зарастать
		m_VB_Thorn.SetMaskTransitionWidth(0.04);
		m_VB_Bloom.SetMaskProgress(0);
		m_VB_Thorn.SetMaskProgress(0);
		m_VB_Branch.SetColor(COLOR_WHITE);
		m_VB_Bloom.SetColor(COLOR_WHITE);
		m_VB_Thorn.SetColor(COLOR_WHITE);
		m_VB_Disc.SetColor(COLOR_DISC);
		m_VB_Name.SetShadow(2, 0xFF000000, 0.9, 1, 1);
		m_VB_Delta.SetShadow(2, 0xFF000000, 0.9, 1, 1);
		m_VB_Root.Show(false);
		return true;
	}

	void VB_Destroy()
	{
		if (m_VB_Root)
			m_VB_Root.Unlink();
		m_VB_Root = null;
	}

	void ~VB_AR_FavorHud()
	{
		if (g_Game)
			VB_Destroy();
	}

	static int VB_TierColor(int tier)
	{
		switch (tier)
		{
			case VB_AR_Favor.TIER_FRIEND: return COLOR_FRIEND;
			case VB_AR_Favor.TIER_OWN: return COLOR_OWN;
			case VB_AR_Favor.TIER_ENEMY: return COLOR_ENEMY;
			case VB_AR_Favor.TIER_CURSED: return COLOR_CURSED;
		}
		return COLOR_STRANGER;
	}

	protected bool VB_WantVisible(PlayerBase player, int tier, int now)
	{
		if (!VB_AR_Favor.s_VB_Known || !player || !player.IsAlive() || player.IsUnconscious())
			return false;
		if (g_Game.GetUIManager() && g_Game.GetUIManager().GetMenu())
			return false;
		if (tier <= VB_AR_Favor.TIER_ENEMY)
			return true;
		if (now - VB_AR_Favor.s_VB_ChangedAt < SHOW_AFTER_CHANGE_MS)
			return true;

		vector pos = player.GetPosition();
		if (now >= m_VB_NextCheck)
		{
			m_VB_NextCheck = now + CHECK_MS;
			m_VB_InForest = false;
			if (VB_AR_Pulse.VB_NightFactor() >= VB_AR_Pulse.NIGHT_FULL_ABOVE)
			{
				array<Object> objs = new array<Object>;
				g_Game.GetObjectsAtPosition(pos, FOREST_RADIUS, objs, null);
				int trees = 0;
				foreach (Object o : objs)
				{
					if (o && o.IsTree())
						trees++;
				}
				m_VB_InForest = trees >= FOREST_TREES;
			}
		}
		if (m_VB_InForest)
			return true;
		return VB_AR_Pulse.VB_NearestLiveHeart(pos, GROVE_NEAR) != null;
	}

	void VB_Update(float dt)
	{
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		if (!player)
			return;
		if (!VB_Create())
			return;

		m_VB_Time += dt;
		int now = g_Game.GetTime();
		int tier = VB_AR_Favor.VB_MyTier();
		bool want = VB_WantVisible(player, tier, now);

		// плавное появление и исчезание
		if (want)
			m_VB_Alpha = Math.Min(1, m_VB_Alpha + dt * FADE_SPEED);
		else
			m_VB_Alpha = Math.Max(0, m_VB_Alpha - dt * FADE_SPEED);

		// ветвь плавно доезжает до настоящего значения (с первого показа — сразу на месте)
		float target = Math.Clamp(VB_AR_Favor.s_VB_Value, -100, 100);
		if (!m_VB_ShownSet)
		{
			m_VB_Shown = target;
			m_VB_ShownSet = true;
		}
		float step = GROW_SPEED * dt;
		if (Math.AbsFloat(target - m_VB_Shown) <= step)
			m_VB_Shown = target;
		else if (target > m_VB_Shown)
			m_VB_Shown += step;
		else
			m_VB_Shown -= step;

		if (m_VB_Alpha <= 0)
		{
			if (m_VB_Root.IsVisible())
				m_VB_Root.Show(false);
			return;
		}
		if (!m_VB_Root.IsVisible())
			m_VB_Root.Show(true);

		int color = VB_TierColor(tier);
		if (tier != m_VB_ShownTier)
		{
			m_VB_ShownTier = tier;
			m_VB_Rune.SetColor(color);
			m_VB_Bud.SetColor(color);
			m_VB_Name.SetColor(color);
			m_VB_Name.SetText(VB_AR_Favor.VB_TierName(tier));
		}

		// ветвь: зелень справа от центра, шипы слева
		float pos01 = (m_VB_Shown + 100) / 200;
		m_VB_Bloom.SetMaskProgress(Math.Max(0, m_VB_Shown) / 100);
		m_VB_Thorn.SetMaskProgress(Math.Max(0, -m_VB_Shown) / 100);
		float budX = pos01 * TRACK_W - BUD_SIZE / 2;
		m_VB_Bud.SetPos(budX, BUD_Y);

		// пульс: у «Врага» медленно, у «Проклятого» чаще; у «Друга» дышит почка
		float runeAlpha = 1;
		float budAlpha = 1;
		if (tier == VB_AR_Favor.TIER_ENEMY)
			runeAlpha = 0.7 + 0.3 * Math.Sin(m_VB_Time * Math.PI2 / 2.4);
		else if (tier == VB_AR_Favor.TIER_CURSED)
			runeAlpha = 0.65 + 0.35 * Math.Sin(m_VB_Time * Math.PI2 / 1.0);
		else if (tier == VB_AR_Favor.TIER_FRIEND)
			budAlpha = 0.75 + 0.25 * Math.Sin(m_VB_Time * Math.PI2 / 3.0);
		m_VB_Rune.SetAlpha(runeAlpha * m_VB_Alpha);
		m_VB_Disc.SetAlpha(0.55 * m_VB_Alpha);
		m_VB_Branch.SetAlpha(m_VB_Alpha);
		m_VB_Bloom.SetAlpha(m_VB_Alpha);
		m_VB_Thorn.SetAlpha(m_VB_Alpha);
		m_VB_Bud.SetAlpha(budAlpha * m_VB_Alpha);
		m_VB_Name.SetAlpha(m_VB_Alpha);

		// «+10» / «-25» над почкой после изменения
		int since = now - VB_AR_Favor.s_VB_ChangedAt;
		if (since >= 0 && since < DELTA_MS && VB_AR_Favor.s_VB_LastDelta != 0)
		{
			float d = VB_AR_Favor.s_VB_LastDelta;
			int rounded = Math.Round(d);
			string txt = rounded.ToString();
			if (Math.AbsFloat(d) < 1)
			{
				float tenths = Math.Round(d * 10) / 10;		// изгнание нечисти: +0.1, +0.2
				txt = tenths.ToString();
			}
			int deltaColor = COLOR_CURSED;
			if (d > 0)
			{
				txt = "+" + txt;
				deltaColor = COLOR_OWN;
			}
			m_VB_Delta.SetText(txt);
			m_VB_Delta.SetColor(deltaColor);
			float k = since;
			k = k / DELTA_MS;
			float fade = 1;
			if (k > 0.7)
				fade = (1 - k) / 0.3;
			m_VB_Delta.SetAlpha(fade * m_VB_Alpha);
			float dx = TRACK_X + budX + BUD_SIZE / 2 - 30;
			m_VB_Delta.SetPos(dx, -20 - 10 * k);		// чуть всплывает
			m_VB_Delta.Show(true);
		}
		else if (m_VB_Delta.IsVisible())
		{
			m_VB_Delta.Show(false);
		}
	}
}
