// Редактор точек боссов (ТЗ 6), сервер: права (Admins в settings.json), правка points.json, призыв и поле
// на точке, «Перечитать конфиги», снимки «что на точках» для открытых редакторов. Каждое действие — в лог [VB_AB].

class VB_AB_EditorServer
{
	static const int LIVE_MS = 2000;			// снимок R_LIVE открытым редакторам
	static const float TP_OFFSET = 8.0;			// м: «Перенестись» ставит рядом с точкой, а не в неё
	static const float MINION_RADIUS = 80.0;	// м: «Стая» вокруг босса, которую убираем вместе с ним

	protected static ref VB_AB_EditorServer s_Instance;

	protected ref TStringArray m_Editors = new TStringArray;	// SteamID с открытым редактором
	protected bool m_LiveOn;

	static void VB_OnRpc(PlayerBase player, PlayerIdentity sender, ParamsReadContext ctx)
	{
		if (!sender)
			return;
		if (!s_Instance)
			s_Instance = new VB_AB_EditorServer();
		s_Instance.VB_Handle(player, sender, ctx);
	}

	protected void VB_Handle(PlayerBase player, PlayerIdentity sender, ParamsReadContext ctx)
	{
		int op;
		if (!ctx.Read(op))
			return;

		// действует всегда свой персонаж отправителя
		PlayerBase actor = PlayerBase.Cast(sender.GetPlayer());
		if (!actor)
			actor = player;
		if (!actor)
			return;

		string uid = sender.GetPlainId();
		string who = sender.GetName() + " (" + uid + ")";
		VB_AB_BossData data = VB_AB_Server.Data();
		if (!data)
		{
			VB_AB_Log.Warn("редактор: сервер боссов не запущен, " + who);
			return;
		}
		if (!data.IsAdmin(uid) && op == VB_AB_EditorNet.OP_OPEN)
			data.ReloadSettings();		// админа можно дописать в settings.json без рестарта
		if (!data.IsAdmin(uid))
		{
			VB_AB_Log.Warn("редактор: нет прав у " + who + " (операция " + op + ")");
			if (op == VB_AB_EditorNet.OP_OPEN)
			{
				ScriptRPC deny = new ScriptRPC();
				deny.Write(VB_AB_EditorNet.R_DENIED);
				deny.Write(uid);
				deny.Send(actor, VB_AB_EditorNet.RPC_REPLY, true, sender);
			}
			return;
		}

		int id;
		int iv;
		float fv;
		string sv;
		vector pos;
		switch (op)
		{
			case VB_AB_EditorNet.OP_OPEN:
				if (m_Editors.Find(uid) < 0)
					m_Editors.Insert(uid);
				VB_AB_Log.Info("редактор: открыл " + who);
				VB_SendAll(actor);
				VB_StartLive();
				break;

			case VB_AB_EditorNet.OP_CLOSE:
				m_Editors.RemoveItem(uid);
				VB_AB_Log.Info("редактор: закрыл " + who);
				break;

			case VB_AB_EditorNet.OP_ADD:
				if (ctx.Read(pos) && ctx.Read(sv))
					VB_Add(actor, who, pos, sv);
				break;

			case VB_AB_EditorNet.OP_DEL:
				if (ctx.Read(id))
					VB_Delete(actor, who, id);
				break;

			case VB_AB_EditorNet.OP_DISABLE:
				if (ctx.Read(id) && ctx.Read(iv))
					VB_SetDisabled(actor, who, id, iv != 0);
				break;

			case VB_AB_EditorNet.OP_WEIGHT:
				if (ctx.Read(id) && ctx.Read(fv))
					VB_SetWeight(actor, who, id, fv);
				break;

			case VB_AB_EditorNet.OP_BOSS_ADD:
				if (ctx.Read(id) && ctx.Read(sv))
					VB_BossAdd(actor, who, id, sv);
				break;

			case VB_AB_EditorNet.OP_BOSS_DEL:
				if (ctx.Read(id) && ctx.Read(sv))
					VB_BossDel(actor, who, id, sv);
				break;

			case VB_AB_EditorNet.OP_TELEPORT:
				if (ctx.Read(id))
					VB_Teleport(actor, who, id);
				break;

			case VB_AB_EditorNet.OP_SUMMON:
				if (ctx.Read(id) && ctx.Read(sv))
					VB_Summon(actor, who, id, sv);
				break;

			case VB_AB_EditorNet.OP_FIELD_OPEN:
				if (ctx.Read(id))
					VB_FieldOpen(actor, who, id);
				break;

			case VB_AB_EditorNet.OP_FIELD_REROLL:
				if (ctx.Read(id))
					VB_FieldReroll(actor, who, id);
				break;

			case VB_AB_EditorNet.OP_DESPAWN:
				if (ctx.Read(id))
					VB_Despawn(actor, who, id);
				break;

			case VB_AB_EditorNet.OP_RELOAD:
				VB_Reload(actor, who);
				break;

			case VB_AB_EditorNet.OP_SET_ACTIVE:
				if (ctx.Read(iv))
					VB_SetActive(actor, who, iv);
				break;
		}
	}

	// --- правка точек -----------------------------------------------------------------------------

	protected VB_AB_PointCfg VB_Point(PlayerBase actor, int id)
	{
		VB_AB_PointCfg p = VB_AB_Server.Data().FindPoint(id);
		if (!p)
			VB_Msg(actor, "STR_VB_AB_Ed_MsgNoPoint", id.ToString());
		return p;
	}

	protected void VB_Add(PlayerBase actor, string who, vector pos, string bossId)
	{
		VB_AB_BossData data = VB_AB_Server.Data();
		if (bossId != "" && !data.GetBoss(bossId))
		{
			VB_Msg(actor, "STR_VB_AB_Ed_MsgUnknownBoss", bossId);
			return;
		}
		if (g_Game.SurfaceIsSea(pos[0], pos[2]) || g_Game.SurfaceIsPond(pos[0], pos[2]))
		{
			VB_Msg(actor, "STR_VB_AB_Ed_MsgWater");
			return;
		}

		VB_AB_PointCfg p = data.AddPoint(pos, bossId);
		VB_AB_Log.Info("редактор: " + who + " поставил точку " + p.Id + " " + p.GetPos().ToString() + ", босс " + bossId);
		VB_PointToEditors(p);

		ScriptRPC sel = new ScriptRPC();
		sel.Write(VB_AB_EditorNet.R_SELECT);
		sel.Write(p.Id);
		sel.Send(actor, VB_AB_EditorNet.RPC_REPLY, true, actor.GetIdentity());
		VB_Msg(actor, "STR_VB_AB_Ed_MsgAdded", p.Id.ToString());
	}

	protected void VB_Delete(PlayerBase actor, string who, int id)
	{
		VB_AB_PointCfg p = VB_Point(actor, id);
		if (!p)
			return;

		vector at = p.GetPos();
		VB_AB_PointRuntime r = VB_AB_Server.FindRuntime(id);
		if (r && r.m_Boss)
			VB_RemoveMinions(r.m_Boss);
		VB_AB_Server.DropPoint(id);
		VB_AB_Server.Data().RemovePoint(id);
		VB_AB_Log.Info("редактор: " + who + " удалил точку " + id + " " + at.ToString());

		foreach (string editor : m_Editors)
		{
			PlayerBase ep = VB_FindPlayer(editor);
			if (!ep)
				continue;
			ScriptRPC rpc = new ScriptRPC();
			rpc.Write(VB_AB_EditorNet.R_POINT_GONE);
			rpc.Write(id);
			rpc.Send(ep, VB_AB_EditorNet.RPC_REPLY, true, ep.GetIdentity());
		}
		VB_LiveToEditors();
		VB_Msg(actor, "STR_VB_AB_Ed_MsgDeleted", id.ToString());
	}

	protected void VB_SetDisabled(PlayerBase actor, string who, int id, bool off)
	{
		VB_AB_PointCfg p = VB_Point(actor, id);
		if (!p)
			return;

		if (off)
		{
			p.Disabled = 1;
			VB_AB_Server.StopPoint(id);
		}
		else
		{
			p.Disabled = 0;
			VB_AB_Server.ResumePoint(id);
		}
		VB_AB_Server.Data().SavePoints();
		string word = "включил";
		if (off)
			word = "выключил";
		VB_AB_Log.Info("редактор: " + who + " " + word + " точку " + id);
		VB_PointToEditors(p);
		VB_LiveToEditors();
		if (off)
			VB_Msg(actor, "STR_VB_AB_Ed_MsgDisabled", id.ToString());
		else
			VB_Msg(actor, "STR_VB_AB_Ed_MsgEnabled", id.ToString());
	}

	protected void VB_SetWeight(PlayerBase actor, string who, int id, float w)
	{
		VB_AB_PointCfg p = VB_Point(actor, id);
		if (!p)
			return;

		p.Weight = Math.Clamp(w, 0.1, 1000);
		VB_AB_Server.Data().SavePoints();
		VB_AB_Log.Info("редактор: " + who + " задал точке " + id + " вес " + p.Weight);
		VB_PointToEditors(p);
	}

	protected void VB_BossAdd(PlayerBase actor, string who, int id, string bossId)
	{
		VB_AB_PointCfg p = VB_Point(actor, id);
		if (!p)
			return;
		if (!VB_AB_Server.Data().GetBoss(bossId))
		{
			VB_Msg(actor, "STR_VB_AB_Ed_MsgUnknownBoss", bossId);
			return;
		}
		if (!p.Bosses)
			p.Bosses = new array<string>;
		if (p.Bosses.Find(bossId) != -1)
		{
			VB_Msg(actor, "STR_VB_AB_Ed_MsgBossHas", VB_BossName(bossId), id.ToString());
			return;
		}

		p.Bosses.Insert(bossId);
		VB_AB_Server.Data().SavePoints();
		VB_AB_Log.Info("редактор: " + who + " добавил в точку " + id + " босса " + bossId);
		VB_PointToEditors(p);
		VB_Msg(actor, "STR_VB_AB_Ed_MsgBossAdded", VB_BossName(bossId), id.ToString());
	}

	protected void VB_BossDel(PlayerBase actor, string who, int id, string bossId)
	{
		VB_AB_PointCfg p = VB_Point(actor, id);
		if (!p || !p.Bosses)
			return;
		if (p.Bosses.Find(bossId) == -1)
			return;

		p.Bosses.RemoveItem(bossId);
		VB_AB_Server.Data().SavePoints();
		VB_AB_Log.Info("редактор: " + who + " убрал из точки " + id + " босса " + bossId);
		VB_PointToEditors(p);
		VB_Msg(actor, "STR_VB_AB_Ed_MsgBossRemoved", VB_BossName(bossId), id.ToString());
	}

	protected void VB_Teleport(PlayerBase actor, string who, int id)
	{
		VB_AB_PointCfg p = VB_Point(actor, id);
		if (!p)
			return;

		vector c = p.GetPos();
		float tx = c[0] + TP_OFFSET;
		float tz = c[2] + TP_OFFSET;
		actor.SetPosition(Vector(tx, g_Game.SurfaceY(tx, tz), tz));
		VB_AB_Log.Info("редактор: " + who + " перенёсся к точке " + id);
	}

	// --- босс на точке ----------------------------------------------------------------------------

	protected void VB_Summon(PlayerBase actor, string who, int id, string bossId)
	{
		VB_AB_PointCfg p = VB_Point(actor, id);
		if (!p)
			return;
		if (bossId != "" && !VB_AB_Server.Data().GetBoss(bossId))
		{
			VB_Msg(actor, "STR_VB_AB_Ed_MsgUnknownBoss", bossId);
			return;
		}

		VB_AB_BossBase b = VB_AB_Server.SummonAt(p, bossId);
		VB_LiveToEditors();
		if (!b)
		{
			VB_AB_Log.Warn("редактор: " + who + " не смог призвать босса на точку " + id);
			VB_Msg(actor, "STR_VB_AB_Ed_MsgSummonFail", id.ToString());
			return;
		}
		VB_AB_Log.Info("редактор: " + who + " призвал " + b.VB_GetId() + " на точку " + id);
		VB_Msg(actor, "STR_VB_AB_Ed_MsgSummoned", VB_BossName(b.VB_GetId()), id.ToString());
	}

	// Живой босс точки; нет — призвать (summon) или сообщить
	protected VB_AB_BossBase VB_LiveBoss(PlayerBase actor, VB_AB_PointCfg p, bool summon)
	{
		VB_AB_PointRuntime r = VB_AB_Server.FindRuntime(p.Id);
		if (r && r.m_Boss && r.m_Boss.IsAlive())
			return r.m_Boss;
		if (!summon)
		{
			VB_Msg(actor, "STR_VB_AB_Ed_MsgNoBoss", p.Id.ToString());
			return null;
		}
		VB_AB_BossBase b = VB_AB_Server.SummonAt(p, "");
		if (!b)
			VB_Msg(actor, "STR_VB_AB_Ed_MsgSummonFail", p.Id.ToString());
		return b;
	}

	// «Раскрыть поле»: без агра, шансы бросаются как в бою (лог поля пишет VB_AB_Field)
	protected void VB_FieldOpen(PlayerBase actor, string who, int id)
	{
		VB_AB_PointCfg p = VB_Point(actor, id);
		if (!p)
			return;
		VB_AB_BossBase b = VB_LiveBoss(actor, p, true);
		if (!b)
			return;
		if (b.VB_GetField())
		{
			VB_Msg(actor, "STR_VB_AB_Ed_MsgFieldAlready", id.ToString());
			return;
		}

		VB_AB_Log.Info("редактор: " + who + " раскрыл поле " + b.VB_GetId() + " на точке " + id);
		b.VB_OpenField();
		VB_LiveToEditors();
		VB_Msg(actor, "STR_VB_AB_Ed_MsgFieldOpened", id.ToString());
	}

	// «Перебросить поле»: свернуть и раскрыть заново (новый бросок шансов, способности тоже заново)
	protected void VB_FieldReroll(PlayerBase actor, string who, int id)
	{
		VB_AB_PointCfg p = VB_Point(actor, id);
		if (!p)
			return;
		VB_AB_BossBase b = VB_LiveBoss(actor, p, false);
		if (!b)
			return;

		VB_AB_Log.Info("редактор: " + who + " перебросил поле " + b.VB_GetId() + " на точке " + id);
		VB_RemoveMinions(b);		// «Стая» встанет заново при раскрытии
		b.VB_RerollField();			// отложенные удары способностей снимаются, поле и набор — заново
		VB_LiveToEditors();
		VB_Msg(actor, "STR_VB_AB_Ed_MsgRerolled", id.ToString());
	}

	protected void VB_Despawn(PlayerBase actor, string who, int id)
	{
		VB_AB_PointCfg p = VB_Point(actor, id);
		if (!p)
			return;
		VB_AB_PointRuntime r = VB_AB_Server.FindRuntime(id);
		if (!r || !r.m_Boss)
		{
			VB_Msg(actor, "STR_VB_AB_Ed_MsgNoBoss", id.ToString());
			return;
		}

		string bossId = r.m_Boss.VB_GetId();
		VB_RemoveMinions(r.m_Boss);
		VB_AB_Server.DespawnAt(id);
		VB_AB_Log.Info("редактор: " + who + " убрал босса " + bossId + " с точки " + id);
		VB_LiveToEditors();
		VB_Msg(actor, "STR_VB_AB_Ed_MsgDespawned", id.ToString());
	}

	// Спутники «Стаи» этого босса (того же скина, VB_IsMinion) рядом с ним
	protected void VB_RemoveMinions(VB_AB_BossBase boss)
	{
		if (!boss)
			return;
		array<Object> objs = new array<Object>;
		g_Game.GetObjectsAtPosition(boss.GetPosition(), MINION_RADIUS, objs, null);
		int n = 0;
		foreach (Object o : objs)
		{
			VB_AB_BossBase m = VB_AB_BossBase.Cast(o);
			if (!m || m == boss || !m.VB_IsMinion() || m.GetType() != boss.GetType())
				continue;
			if (m.IsAlive())
				m.VB_Despawn();
			else
				g_Game.ObjectDelete(m);
			n++;
		}
		if (n > 0)
			VB_AB_Log.Info("редактор: убрано спутников «Стаи»: " + n);
	}

	// --- конфиги ----------------------------------------------------------------------------------

	protected void VB_Reload(PlayerBase actor, string who)
	{
		VB_AB_Log.Info("редактор: " + who + " перечитал конфиги");
		VB_AB_Server.ReloadConfigs();
		VB_AB_BossData data = VB_AB_Server.Data();
		foreach (string editor : m_Editors)
		{
			PlayerBase ep = VB_FindPlayer(editor);
			if (ep)
				VB_SendAll(ep);
		}
		VB_Msg(actor, "STR_VB_AB_Ed_MsgReloaded", data.m_Bosses.Count().ToString(), data.m_Points.Count().ToString());
	}

	protected void VB_SetActive(PlayerBase actor, string who, int n)
	{
		VB_AB_BossData data = VB_AB_Server.Data();
		data.m_Settings.ActivePointsPerRestart = Math.Clamp(n, 1, 200);
		data.SaveSettings();
		VB_AB_Log.Info("редактор: " + who + " задал точек за рестарт " + data.m_Settings.ActivePointsPerRestart);
		foreach (string editor : m_Editors)
		{
			PlayerBase ep = VB_FindPlayer(editor);
			if (!ep)
				continue;
			ScriptRPC rpc = new ScriptRPC();
			rpc.Write(VB_AB_EditorNet.R_SETTINGS);
			VB_WriteSettings(rpc);
			rpc.Send(ep, VB_AB_EditorNet.RPC_REPLY, true, ep.GetIdentity());
		}
		VB_Msg(actor, "STR_VB_AB_Ed_MsgActive", data.m_Settings.ActivePointsPerRestart.ToString());
	}

	// --- отправка ---------------------------------------------------------------------------------

	protected PlayerBase VB_FindPlayer(string plainId)
	{
		array<Man> players = new array<Man>;
		g_Game.GetPlayers(players);
		foreach (Man someone : players)
		{
			if (someone && someone.GetIdentity() && someone.GetIdentity().GetPlainId() == plainId)
				return PlayerBase.Cast(someone);
		}
		return null;
	}

	protected string VB_BossName(string bossId)
	{
		VB_AB_BossCfg b = VB_AB_Server.Data().GetBoss(bossId);
		if (b && b.Name != "")
			return b.Name;
		return bossId;
	}

	// сообщение админу: ключ строки и до двух подстановок, текст собирает клиент на своём языке
	protected void VB_Msg(PlayerBase player, string key, string a1 = "", string a2 = "")
	{
		if (!player || !player.GetIdentity())
			return;
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(VB_AB_EditorNet.R_MSG);
		rpc.Write(key);
		rpc.Write(a1);
		rpc.Write(a2);
		rpc.Send(player, VB_AB_EditorNet.RPC_REPLY, true, player.GetIdentity());
	}

	// порядок полей читает VB_AB_EditorClient.VB_ReadSettings
	protected void VB_WriteSettings(ScriptRPC rpc)
	{
		VB_AB_SettingsFile st = VB_AB_Server.Settings();
		rpc.Write(st.ActivePointsPerRestart);
		rpc.Write(st.WakeRadius);
		rpc.Write(st.MinPointDistance);
		rpc.Write(st.MaxAwakeBosses);
	}

	protected void VB_SendAll(PlayerBase player)
	{
		PlayerIdentity to = player.GetIdentity();
		if (!to)
			return;
		VB_AB_BossData data = VB_AB_Server.Data();

		ScriptRPC begin = new ScriptRPC();
		begin.Write(VB_AB_EditorNet.R_BEGIN);
		VB_WriteSettings(begin);
		begin.Send(player, VB_AB_EditorNet.RPC_REPLY, true, to);

		ScriptRPC cat = new ScriptRPC();
		cat.Write(VB_AB_EditorNet.R_BOSSES);
		cat.Write(data.m_Bosses.Count());
		foreach (string bid, VB_AB_BossCfg b : data.m_Bosses)
		{
			cat.Write(b.Id);
			cat.Write(b.Name);
			cat.Write(b.Classname);
			cat.Write(b.Tier);
			cat.Write(b.Field.Radius);
			cat.Write(b.Field.LeashRadius);
		}
		cat.Send(player, VB_AB_EditorNet.RPC_REPLY, true, to);

		foreach (VB_AB_PointCfg p : data.m_Points)
		{
			VB_SendPoint(player, p);
		}
		VB_SendLive(player);
	}

	protected void VB_SendPoint(PlayerBase player, VB_AB_PointCfg p)
	{
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(VB_AB_EditorNet.R_POINT);
		rpc.Write(p.Id);
		rpc.Write(p.GetPos());
		rpc.Write(p.Weight);
		rpc.Write(p.Disabled);
		int count = 0;
		if (p.Bosses)
			count = p.Bosses.Count();
		rpc.Write(count);
		for (int i = 0; i < count; i++)
		{
			rpc.Write(p.Bosses[i]);
		}
		rpc.Send(player, VB_AB_EditorNet.RPC_REPLY, true, player.GetIdentity());
	}

	protected void VB_PointToEditors(VB_AB_PointCfg p)
	{
		foreach (string editor : m_Editors)
		{
			PlayerBase ep = VB_FindPlayer(editor);
			if (ep)
				VB_SendPoint(ep, p);
		}
	}

	// что сейчас на точках рестарта (порядок полей читает VB_AB_EditorClient.VB_ReadLive)
	protected void VB_SendLive(PlayerBase player)
	{
		array<ref VB_AB_PointRuntime> act = VB_AB_Server.ActivePoints();
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(VB_AB_EditorNet.R_LIVE);
		if (!act)
		{
			rpc.Write(0);
			rpc.Send(player, VB_AB_EditorNet.RPC_REPLY, true, player.GetIdentity());
			return;
		}

		rpc.Write(act.Count());
		float now = VB_AB_Server.Now();
		foreach (VB_AB_PointRuntime r : act)
		{
			int kind = VB_AB_EditorNet.LIVE_WAIT;
			string bossId = "";
			float hp = 0;
			int anoms = 0;
			vector center = r.GetPos();
			int respawn = 0;
			VB_AB_BossBase b = r.m_Boss;
			if (b && b.IsAlive())
			{
				kind = VB_AB_EditorNet.LIVE_AWAKE;
				bossId = b.VB_GetId();
				hp = b.VB_GetHealth01();
				center = b.GetPosition();
				VB_AB_Field f = b.VB_GetField();
				if (f)
				{
					kind = VB_AB_EditorNet.LIVE_FIELD;
					center = f.GetCenter();
					if (f.GetAnomalies())
						anoms = f.GetAnomalies().Count();
				}
			}
			else if (r.m_Done)
			{
				kind = VB_AB_EditorNet.LIVE_DONE;
			}
			else if (r.m_RespawnAt > now)
			{
				kind = VB_AB_EditorNet.LIVE_RESPAWN;
				respawn = r.m_RespawnAt - now;
			}
			rpc.Write(r.m_Cfg.Id);
			rpc.Write(kind);
			rpc.Write(bossId);
			rpc.Write(hp);
			rpc.Write(anoms);
			rpc.Write(center);
			rpc.Write(respawn);
		}
		rpc.Send(player, VB_AB_EditorNet.RPC_REPLY, true, player.GetIdentity());
	}

	protected void VB_LiveToEditors()
	{
		for (int i = m_Editors.Count() - 1; i >= 0; i--)
		{
			PlayerBase ep = VB_FindPlayer(m_Editors[i]);
			if (ep && ep.GetIdentity())
				VB_SendLive(ep);
			else
				m_Editors.Remove(i);		// вышел с сервера, не закрыв редактор
		}
	}

	protected void VB_StartLive()
	{
		if (m_LiveOn)
			return;
		m_LiveOn = true;
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VB_LiveTick, LIVE_MS, true);
	}

	void VB_LiveTick()
	{
		VB_LiveToEditors();
		if (m_Editors.Count() == 0)
		{
			m_LiveOn = false;
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(VB_LiveTick);
		}
	}
}

// Запросы редактора приходят на объект игрока (клиент: VB_AnomaliesBosses, VB_AB_EditorNet.c)
modded class PlayerBase
{
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		if (rpc_type == VB_AB_EditorNet.RPC_EDIT && g_Game.IsServer())
			VB_AB_EditorServer.VB_OnRpc(this, sender, ctx);
	}
};
