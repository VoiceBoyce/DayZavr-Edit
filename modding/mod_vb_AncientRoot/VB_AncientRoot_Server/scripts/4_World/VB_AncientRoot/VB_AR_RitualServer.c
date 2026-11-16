// Ритуалы — сочетания даров (ТЗ_сервер_18.md, ветка 2в п.3) — только сервер. У рощи и древней (у ростка нет).
// Два подношения одного игрока у одной рощи подряд, не дольше WINDOW_MS между ними, порядок не важен:
//   «Ритуал волка»  — клык + мясо: WOLF_MS обычные волки не нападают первыми (стражи-волки — как раньше);
//   «Ритуал янтаря» — руна или кристалл + простой янтарь: янтарь с вкраплением и янтарь у ног;
//   «Ритуал роста»  — Семя + пыльца: роща сразу на ступень вверх (в древнюю — по лимиту, VB_AR_Nests.VB_OnGroveDefended),
//                     древняя — рестарт жизни.
// Клык, янтарь, Семя, пыльца сами по себе не дары: роща берёт их только как часть ритуала («Роща ждёт второго дара»).
// Мясо и руна — обычные дары (своя награда и Благословение) и заодно часть ритуала. Каждая часть тратит одно подношение
// из лимита рощи; чтобы начать ритуал компонентом, роще нужно ещё два. Ритуал: Благословение (если вторая часть —
// компонент) и +RITUAL_FAVOR расположения. Части — VB_AR_Offer.VB_RitualPart (клиент, общее).

class VB_AR_RitualPending
{
	string Grove;		// место рощи (ключ)
	int Part;			// VB_AR_Offer.PART_*
	int At;				// g_Game.GetTime()
}

class VB_AR_Ritual
{
	static const int WINDOW_MS = 120000;
	static const int WOLF_MS = 3600000;
	static const float RITUAL_FAVOR = 5.0;
	static const int R_NONE = 0;
	static const int R_WOLF = 1;
	static const int R_AMBER = 2;
	static const int R_GROWTH = 3;
	static const string AMBER = "VB_AR_Amber";
	static const string AMBER_INCL = "VB_AR_AmberInclusion";

	protected static ref map<string, ref VB_AR_RitualPending> s_VB_Pending;	// SteamID -> начатый ритуал
	protected static ref map<string, int> s_VB_WolfUntil;					// SteamID -> до какого времени волки мирные

	protected static string VB_Id(PlayerBase p)
	{
		if (!p || !p.GetIdentity())
			return "";
		return p.GetIdentity().GetPlainId();
	}

	protected static string VB_GroveKey(VB_AR_Heart h)
	{
		vector p = h.GetPosition();
		int x = Math.Round(p[0]);
		int z = Math.Round(p[2]);
		return x.ToString() + "_" + z.ToString();
	}

	// две части — какой ритуал (порядок не важен)
	static int VB_RitualOf(int a, int b)
	{
		if ((a == VB_AR_Offer.PART_FANG && b == VB_AR_Offer.PART_MEAT) || (a == VB_AR_Offer.PART_MEAT && b == VB_AR_Offer.PART_FANG))
			return R_WOLF;
		if ((a == VB_AR_Offer.PART_RUNE && b == VB_AR_Offer.PART_AMBER) || (a == VB_AR_Offer.PART_AMBER && b == VB_AR_Offer.PART_RUNE))
			return R_AMBER;
		if ((a == VB_AR_Offer.PART_SEED && b == VB_AR_Offer.PART_POLLEN) || (a == VB_AR_Offer.PART_POLLEN && b == VB_AR_Offer.PART_SEED))
			return R_GROWTH;
		return R_NONE;
	}

	// чего роща ждёт вторым даром
	static string VB_SecondWord(int part)
	{
		switch (part)
		{
			case VB_AR_Offer.PART_FANG: return "мяса";
			case VB_AR_Offer.PART_AMBER: return "руны или кристалла";
			case VB_AR_Offer.PART_SEED: return "золотой пыльцы";
			case VB_AR_Offer.PART_POLLEN: return "Семени Древнего корня";
		}
		return "второго дара";
	}

	// начатый этим игроком у этой рощи ритуал, ещё не просроченный; null — нет
	protected static VB_AR_RitualPending VB_PendingOf(string id, VB_AR_Heart h)
	{
		if (!s_VB_Pending || id == "")
			return null;
		VB_AR_RitualPending pend;
		if (!s_VB_Pending.Find(id, pend) || !pend)
			return null;
		if (pend.Grove != VB_GroveKey(h) || g_Game.GetTime() - pend.At > WINDOW_MS)
		{
			s_VB_Pending.Remove(id);
			return null;
		}
		return pend;
	}

	protected static void VB_SetPending(string id, VB_AR_Heart h, int part)
	{
		if (!s_VB_Pending)
			s_VB_Pending = new map<string, ref VB_AR_RitualPending>;
		VB_AR_RitualPending pend = new VB_AR_RitualPending();
		pend.Grove = VB_GroveKey(h);
		pend.Part = part;
		pend.At = g_Game.GetTime();
		s_VB_Pending.Set(id, pend);
	}

	protected static void VB_Consume(ItemBase item)
	{
		if (item.ConfigGetBool("canBeSplit") && item.GetQuantity() > 1)
			item.AddQuantity(-1);
		else
			item.DeleteSafe();
	}

	// Компонент (клык, янтарь, Семя, пыльца) поднесён к роще heart. false — роща не взяла (предмет остаётся).
	static bool VB_OnPart(VB_AR_Heart h, PlayerBase p, ItemBase item, int part)
	{
		string id = VB_Id(p);
		if (id == "")
			return false;
		if (h.VB_Stage() < VB_AR_Heart.STAGE_GROVE)
		{
			NotificationSystem.SendNotificationToPlayerExtended(p, 5, "Росток ещё мал", "Ритуалы примет только взрослая роща.");
			return false;
		}
		string what = item.GetType();
		VB_AR_RitualPending pend = VB_PendingOf(id, h);
		if (pend)
		{
			int r = VB_RitualOf(pend.Part, part);
			if (r != R_NONE)
			{
				VB_Consume(item);
				h.VB_TakeRitualPartServer();
				s_VB_Pending.Remove(id);
				VB_Complete(h, p, r, true);
				return true;
			}
		}
		// начало ритуала: роще нужно ещё два подношения на этот рестарт
		if (h.VB_OffersLeft() < 2)
		{
			NotificationSystem.SendNotificationToPlayerExtended(p, 5, "Роща устала", "На ритуал у неё уже не хватит сил до рестарта.");
			return false;
		}
		// прежний начатый компонентом ритуал этим даром не сложился — сказать, что тот дар пропал
		if (pend && pend.Part != VB_AR_Offer.PART_MEAT && pend.Part != VB_AR_Offer.PART_RUNE)
			NotificationSystem.SendNotificationToPlayerExtended(p, 5, "Прежний ритуал не сложился", "Такие дары роща не связывает: начат новый ритуал.");
		VB_Consume(item);
		h.VB_TakeRitualPartServer();
		VB_SetPending(id, h, part);
		string second = VB_SecondWord(part);
		NotificationSystem.SendNotificationToPlayerExtended(p, 7, "Роща ждёт второго дара", "Принеси " + second + " — у тебя две минуты.");
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_Expire, WINDOW_MS + 1000, false, id);
		string who = VB_AR_ItemsServer.VB_Who(p);
		Print("[VB_AncientRoot] ритуал: " + who + " начал ритуал даром " + what + ", ждём " + second);
		return true;
	}

	// Обычный дар принят (мясо, руна): закончить ритуал, если ждал компонент, иначе стать его началом (без подсказки:
	// сочетания — тайна «Книги древних»).
	static void VB_AfterGift(VB_AR_Heart h, PlayerBase p, int part)
	{
		if (part != VB_AR_Offer.PART_MEAT && part != VB_AR_Offer.PART_RUNE)
			return;
		if (h.VB_Stage() < VB_AR_Heart.STAGE_GROVE)
			return;
		string id = VB_Id(p);
		if (id == "")
			return;
		VB_AR_RitualPending pend = VB_PendingOf(id, h);
		if (pend)
		{
			int r = VB_RitualOf(pend.Part, part);
			if (r != R_NONE)
			{
				s_VB_Pending.Remove(id);
				VB_Complete(h, p, r, false);		// Благословение уже дал обычный дар
				return;
			}
		}
		if (h.VB_OffersLeft() >= 1)
			VB_SetPending(id, h, part);
	}

	// начатый компонентом ритуал не дождался второго дара
	static void VB_Expire(string id)
	{
		if (!s_VB_Pending || !s_VB_Pending.Contains(id))
			return;
		VB_AR_RitualPending pend = s_VB_Pending.Get(id);
		if (!pend || g_Game.GetTime() - pend.At < WINDOW_MS)
			return;
		s_VB_Pending.Remove(id);
		if (pend.Part == VB_AR_Offer.PART_MEAT || pend.Part == VB_AR_Offer.PART_RUNE)
			return;		// обычный дар — молчим
		array<Man> players = new array<Man>;
		g_Game.GetPlayers(players);
		PlayerBase pb;
		foreach (Man m : players)
		{
			pb = PlayerBase.Cast(m);
			if (pb && VB_Id(pb) == id)
			{
				NotificationSystem.SendNotificationToPlayerExtended(pb, 5, "Роща не дождалась второго дара", "Ритуал не сложился.");
				break;
			}
		}
	}

	protected static void VB_Complete(VB_AR_Heart h, PlayerBase p, int r, bool bless)
	{
		string id = VB_Id(p);
		string title;
		string text;
		string name;
		if (r == R_WOLF)
		{
			name = "волка";
			if (!s_VB_WolfUntil)
				s_VB_WolfUntil = new map<string, int>;
			s_VB_WolfUntil.Set(id, g_Game.GetTime() + WOLF_MS);
			title = "Ритуал волка";
			text = "Час обычные волки не тронут тебя первыми.";
		}
		else if (r == R_AMBER)
		{
			name = "янтаря";
			VB_AR_ItemsServer.VB_DropAtFeet(p, AMBER_INCL);
			VB_AR_ItemsServer.VB_DropAtFeet(p, AMBER);
			title = "Ритуал янтаря";
			text = "Роща отдала янтарь с вкраплением и ещё кусок янтаря.";
		}
		else
		{
			name = "роста";
			title = "Ритуал роста";
			text = "Роща набирает силу.";
		}
		if (bless)
			h.VB_BlessServer(p, "Роща приняла дары");
		VB_AR_FavorServer.VB_Add(p, RITUAL_FAVOR, "ритуал " + name);
		string who = VB_AR_ItemsServer.VB_Who(p);
		Print("[VB_AncientRoot] ритуал " + name + ": " + who);
		if (r == R_GROWTH)
		{
			// роща может встать заново (сердце удаляется) — не изнутри метода самого сердца: на следующем кадре
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_GrowLater, 0, false, h, p);
			return;
		}
		NotificationSystem.SendNotificationToPlayerExtended(p, 8, title, text);
	}

	// «Ритуал роста»: ступень вверх (VB_AR_Nests.VB_OnGroveDefended) и уведомление
	static void VB_GrowLater(VB_AR_Heart h, PlayerBase p)
	{
		if (!h || h.VB_IsDead())
			return;
		string grew = VB_AR_Nests.VB_OnGroveDefended(h);
		string text = "Роща набирает силу.";
		if (grew != "")
			text = "Роща отвечает: " + grew + ".";
		if (p)
			NotificationSystem.SendNotificationToPlayerExtended(p, 8, "Ритуал роста", text);
	}

	// «Ритуал волка» в силе
	static bool VB_WolfPact(PlayerBase p)
	{
		if (!s_VB_WolfUntil || s_VB_WolfUntil.Count() == 0)
			return false;
		string id = VB_Id(p);
		int until;
		if (id == "" || !s_VB_WolfUntil.Find(id, until))
			return false;
		if (until > g_Game.GetTime())
			return true;
		s_VB_WolfUntil.Remove(id);
		return false;
	}
}

modded class PlayerBase
{
	// «Ритуал волка»: обычные волки (не стражи рощ) не выбирают игрока целью
	override bool CanBeTargetedByAI(EntityAI ai)
	{
		if (!super.CanBeTargetedByAI(ai))
			return false;
		// сначала дешёвая проверка: без ритуала (почти всегда) — сразу дальше
		if (ai && VB_AR_Ritual.VB_WolfPact(this) && ai.IsKindOf("Animal_CanisLupus") && !VB_AR_Guard.VB_Is(ai))
			return false;
		return true;
	}
}
