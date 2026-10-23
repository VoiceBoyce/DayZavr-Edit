modded class MissionServer
{
	override void OnInit()
	{
		super.OnInit();
		Math.Randomize(-1);
		GetCraft_Config();
		DRL_CraftProcessManager.GetInstance();

		GetRPCManager().AddRPC("DRL_Craft_Config", "CLIENT_RequestCraftConfig", this, SingleplayerExecutionType.Both);
		GetRPCManager().AddRPC("DRL_Craft_Config", "CLIENT_RequestReloadCraftConfig", this, SingleplayerExecutionType.Server);
	}

	void CLIENT_RequestCraftConfig(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type == CallType.Server)
		{
			GetRPCManager().SendRPC("DRL_Craft_Config", "SERVER_CraftConfigResponse", new Param1<DRL_Craft_Config>(GetCraft_Config()), true, sender);
			GetRPCManager().SendRPC("DRL_Craft_Config", "SERVER_LearningConfigResponse", new Param1<DRL_Learning_Config>(GetLearning_Config()), true, sender);

			if (sender)
			{
				string pId = sender.GetPlainId();
				DRL_PlayerLearningData pData = GetPlayerLearningManager().GetPlayerData(pId);
				if (pData)
				{
					array<Man> players = new array<Man>();
					GetGame().GetPlayers(players);
					for (int i = 0; i < players.Count(); i++)
					{
						PlayerBase pb = PlayerBase.Cast(players[i]);
						if (pb && pb.GetIdentity() == sender)
						{
							pb.RPCSingleParam(DRL_CraftRPC.STC_SYNC_LEARNED_RECIPES, new Param1<array<int>>(pData.LearnedRecipeIds), true, sender);
							break;
						}
					}
				}
			}
		}
	}

	bool DRL_IsCraftAdmin(PlayerIdentity identity)
	{
		if (!identity)
		{
			return false;
		}

		DRL_Craft_Config cfg = GetCraft_Config();
		if (!cfg || !cfg.Settings || !cfg.Settings.AdminsID)
		{
			return false;
		}

		string id = identity.GetPlainId();
		for (int i = 0; i < cfg.Settings.AdminsID.Count(); i++)
		{
			if (cfg.Settings.AdminsID.Get(i) == id)
			{
				return true;
			}
		}

		return false;
	}

	void CLIENT_RequestReloadCraftConfig(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server)
		{
			return;
		}

		if (!DRL_IsCraftAdmin(sender))
		{
			return;
		}

		DRL_Craft_Config cfg = GetCraft_Config();
		if (cfg)
		{
			cfg.Load();
			string adminId = sender.GetPlainId();
			string adminName = sender.GetName();
			DRL_CraftLogger.Info("Admin reload requested by: " + adminName + " (" + adminId + ")");

			string msg = "DRL Craft: конфиг обновлён";

			DRL_Craft_Config cc = GetCraft_Config();
			if (cc)
			{
				int recipes = 0;
				int cats = 0;

				if (cc.CraftItems)
				{
					recipes = cc.CraftItems.Count();
				}
				if (cc.Categories)
				{
					cats = cc.Categories.Count();
				}

				DRL_CraftLogger.Info("Reload done. Recipes: " + recipes.ToString() + ", Categories: " + cats.ToString());
				msg = "DRL Craft: конфиг обновлён (Recipes: " + recipes.ToString() + ", Categories: " + cats.ToString() + ")";
			}

			GetRPCManager().SendRPC("DRL_Craft_Config", "SERVER_CraftConfigReloadNotice", new Param1<string>(msg), true, sender);
		}

		autoptr array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);

		for (int i = 0; i < players.Count(); i++)
		{
			PlayerBase pb = PlayerBase.Cast(players.Get(i));
			if (!pb)
			{
				continue;
			}

			PlayerIdentity id = pb.GetIdentity();
			if (!id)
			{
				continue;
			}

			GetRPCManager().SendRPC("DRL_Craft_Config", "SERVER_CraftConfigResponse", new Param1<DRL_Craft_Config>(GetCraft_Config()), true, id);
			GetRPCManager().SendRPC("DRL_Craft_Config", "SERVER_LearningConfigResponse", new Param1<DRL_Learning_Config>(GetLearning_Config()), true, id);

			string pIdReload = id.GetPlainId();
			DRL_PlayerLearningData pDataReload = GetPlayerLearningManager().GetPlayerData(pIdReload);
			if (pDataReload)
			{
				pb.RPCSingleParam(DRL_CraftRPC.STC_SYNC_LEARNED_RECIPES, new Param1<array<int>>(pDataReload.LearnedRecipeIds), true, id);
			}
		}
	}

	override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity)
	{
		super.InvokeOnConnect(player, identity);

		if (player && identity)
		{
			string pId = identity.GetPlainId();
			DRL_PlayerLearningData pData = GetPlayerLearningManager().GetPlayerData(pId);
			if (pData)
			{
				player.RPCSingleParam(DRL_CraftRPC.STC_SYNC_LEARNED_RECIPES, new Param1<array<int>>(pData.LearnedRecipeIds), true, identity);
			}
		}
	}

	override void PlayerDisconnected(PlayerBase player, PlayerIdentity identity, string uid)
	{
		super.PlayerDisconnected(player, identity, uid);

		string pId = uid;
		if (pId == "" && identity)
		{
			pId = identity.GetPlainId();
		}
		if (pId != "")
		{
			GetPlayerLearningManager().OnPlayerDisconnected(pId);
		}
	}
}
