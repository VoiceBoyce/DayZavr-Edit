// VB_MiningResources: данные, нужные и серверу, и игроку.
// Сервер собирает их из settings.json (VB_MR_Settings.BuildShared) и при входе игрока присылает по RPC,
// поэтому новая кирка или другое время добычи работают без пересборки клиента.

// Номер RPC в том же диапазоне, что у планшета (9141011+), с ним не пересекается.
// Отправка ScriptRPC с одной строкой JSON: Param1<string> через RPCSingleParam давал «String CORRUPTED» на клиенте.
const int VB_MR_RPC_SHARED = 9141101;

// Инструмент добычи (запись MineToolsData в settings.json)
class VB_MR_ToolData
{
	string Tool;					// класс инструмента (подходят и наследники)
	float MineTimeMul = 1.0;		// множитель времени добычи
	float ToolDamageMul = 1.0;		// множитель износа инструмента
	float OreChanceAdd = 0;			// прибавка к шансу руды за бросок, %
	int ExtraRollsMin = 0;			// дополнительные броски добычи, минимум
	int ExtraRollsMax = 0;			// дополнительные броски добычи, максимум
	float RareChanceAdd = 0;		// прибавка к шансу редкой находки жилы, %
	string BonusItem;				// находка инструмента при добыче любой жилы (пусто — нет), только на сервере
	float BonusChance = 0;			// шанс находки инструмента за добычу жилы, %
	ref array<string> Veins;		// какие жилы можно добывать (пусто — все)

	void VB_MR_ToolData(string tool = "", float timeMul = 1.0, float damageMul = 1.0, float oreAdd = 0)
	{
		Tool = tool;
		MineTimeMul = timeMul;
		ToolDamageMul = damageMul;
		OreChanceAdd = oreAdd;
		Veins = new array<string>;
	}
}

class VB_MR_VeinInfo
{
	string VeinName;
	float MineTime;
}

// Что во что перерабатывается и сколько штук берётся за раз
class VB_MR_RecipeInfo
{
	string Item;
	string Result;
	float Time;
	int Portion;
}

class VB_MR_SharedData
{
	float PanTime = 10;
	float MeltTemp = 150;
	ref array<ref VB_MR_ToolData> Tools = new array<ref VB_MR_ToolData>;
	ref array<ref VB_MR_VeinInfo> Veins = new array<ref VB_MR_VeinInfo>;
	ref array<ref VB_MR_RecipeInfo> Recipes = new array<ref VB_MR_RecipeInfo>;
}

class VB_MR_Shared
{
	static ref VB_MR_SharedData s_Data;

	// Поиск везде одинаковый: сначала точное имя класса, потом наследование (старые BRDK_* наследуют VB_MR_*)
	static VB_MR_ToolData FindTool(EntityAI item)
	{
		if (!item || !s_Data)
			return null;

		string type = item.GetType();
		foreach (VB_MR_ToolData exact : s_Data.Tools)
		{
			if (exact.Tool == type)
				return exact;
		}
		foreach (VB_MR_ToolData parent : s_Data.Tools)
		{
			if (item.IsKindOf(parent.Tool))
				return parent;
		}
		return null;
	}

	static bool ToolAllowsVein(VB_MR_ToolData tool, Object vein)
	{
		if (!tool || !vein)
			return false;
		if (!tool.Veins || tool.Veins.Count() == 0)
			return true;

		foreach (string name : tool.Veins)
		{
			if (vein.IsKindOf(name))
				return true;
		}
		return false;
	}

	static VB_MR_VeinInfo FindVein(Object vein)
	{
		if (!vein || !s_Data)
			return null;

		string type = vein.GetType();
		foreach (VB_MR_VeinInfo exact : s_Data.Veins)
		{
			if (exact.VeinName == type)
				return exact;
		}
		foreach (VB_MR_VeinInfo parent : s_Data.Veins)
		{
			if (vein.IsKindOf(parent.VeinName))
				return parent;
		}
		return null;
	}

	static VB_MR_RecipeInfo FindRecipe(EntityAI item)
	{
		if (!item || !s_Data)
			return null;

		string type = item.GetType();
		foreach (VB_MR_RecipeInfo exact : s_Data.Recipes)
		{
			if (exact.Item == type)
				return exact;
		}
		foreach (VB_MR_RecipeInfo parent : s_Data.Recipes)
		{
			if (item.IsKindOf(parent.Item))
				return parent;
		}
		return null;
	}

	// Передача по полям: одна длинная строка JSON (2,4 КБ) на клиенте читалась как «String CORRUPTED»
	static void Write(ScriptRPC rpc, VB_MR_SharedData data)
	{
		rpc.Write(data.PanTime);
		rpc.Write(data.MeltTemp);

		rpc.Write(data.Tools.Count());
		foreach (VB_MR_ToolData tool : data.Tools)
		{
			rpc.Write(tool.Tool);
			rpc.Write(tool.MineTimeMul);
			rpc.Write(tool.ToolDamageMul);
			rpc.Write(tool.OreChanceAdd);
			rpc.Write(tool.ExtraRollsMin);
			rpc.Write(tool.ExtraRollsMax);
			rpc.Write(tool.RareChanceAdd);
			int veinCount = 0;
			if (tool.Veins)
				veinCount = tool.Veins.Count();
			rpc.Write(veinCount);
			for (int i = 0; i < veinCount; i++)
				rpc.Write(tool.Veins[i]);
		}

		rpc.Write(data.Veins.Count());
		foreach (VB_MR_VeinInfo vein : data.Veins)
		{
			rpc.Write(vein.VeinName);
			rpc.Write(vein.MineTime);
		}

		rpc.Write(data.Recipes.Count());
		foreach (VB_MR_RecipeInfo recipe : data.Recipes)
		{
			rpc.Write(recipe.Item);
			rpc.Write(recipe.Result);
			rpc.Write(recipe.Time);
			rpc.Write(recipe.Portion);
		}
	}

	static void Receive(ParamsReadContext ctx)
	{
		VB_MR_SharedData data = new VB_MR_SharedData();
		if (!ctx.Read(data.PanTime) || !ctx.Read(data.MeltTemp))
			return;

		int count;
		if (!ctx.Read(count))
			return;
		for (int t = 0; t < count; t++)
		{
			VB_MR_ToolData tool = new VB_MR_ToolData();
			int veinCount;
			if (!ctx.Read(tool.Tool) || !ctx.Read(tool.MineTimeMul) || !ctx.Read(tool.ToolDamageMul) || !ctx.Read(tool.OreChanceAdd))
				return;
			if (!ctx.Read(tool.ExtraRollsMin) || !ctx.Read(tool.ExtraRollsMax) || !ctx.Read(tool.RareChanceAdd) || !ctx.Read(veinCount))
				return;
			for (int v = 0; v < veinCount; v++)
			{
				string veinName;
				if (!ctx.Read(veinName))
					return;
				tool.Veins.Insert(veinName);
			}
			data.Tools.Insert(tool);
		}

		if (!ctx.Read(count))
			return;
		for (int i = 0; i < count; i++)
		{
			VB_MR_VeinInfo vein = new VB_MR_VeinInfo();
			if (!ctx.Read(vein.VeinName) || !ctx.Read(vein.MineTime))
				return;
			data.Veins.Insert(vein);
		}

		if (!ctx.Read(count))
			return;
		for (int r = 0; r < count; r++)
		{
			VB_MR_RecipeInfo recipe = new VB_MR_RecipeInfo();
			if (!ctx.Read(recipe.Item) || !ctx.Read(recipe.Result) || !ctx.Read(recipe.Time) || !ctx.Read(recipe.Portion))
				return;
			data.Recipes.Insert(recipe);
		}

		s_Data = data;
		Print(string.Format("[VB_MiningResources] Данные сервера: инструментов %1, жил %2, рецептов %3", data.Tools.Count(), data.Veins.Count(), data.Recipes.Count()));
	}
}

modded class PlayerBase
{
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);

		// только у игрока: на сервере данные свои (из settings.json), подменить их с клиента нельзя
		if (rpc_type == VB_MR_RPC_SHARED && !GetGame().IsDedicatedServer())
			VB_MR_Shared.Receive(ctx);
	}
}
