class DRL_CraftInventoryHelper
{
	static EntityAI GetWorkbenchRoot(EntityAI wb)
	{
		if (!wb) return null;
		EntityAI root = wb.GetHierarchyRoot();
		if (root && !root.IsInherited(Man))
		{
			return root;
		}
		return wb;
	}

	static void GetInventorySnapshot(PlayerBase player, EntityAI contextWorkbench, out array<EntityAI> items)
	{
		items = new array<EntityAI>();
		if (player && player.GetInventory())
		{
			player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
		}

		EntityAI targetWb = GetWorkbenchRoot(contextWorkbench);
		if (targetWb && targetWb.GetInventory())
		{
			targetWb.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
		}
	}

	static bool IsFillingByClass(string cls)
	{
		if (cls == "")
		{
			return false;
		}

		int qType = DRL_ItemQuantityHelper.GetQuantityTypeByClass(cls);
		return DRL_ItemQuantityHelper.IsFillingType(qType);
	}

	static int ComputeNeedTotalForClass(string cls, DRL_Craft_Component c)
	{
		if (!c)
		{
			return 0;
		}

		if (c.QuantityFrac <= 0.0)
		{
			return c.Amount;
		}

		string targetCls = cls;
		if (targetCls == "")
		{
			targetCls = c.Classname;
		}

		float qMax = DRL_ItemQuantityHelper.GetQuantityMaxRaw(null, targetCls);
		if (qMax <= 0.0)
		{
			return c.Amount;
		}

		int perUnit = Math.Round(qMax * c.QuantityFrac);
		if (perUnit < 1)
		{
			perUnit = 1;
		}

		return c.Amount * perUnit;
	}

	static int ComputeNeedTotalForComponent(DRL_Craft_Component c)
	{
		if (!c)
		{
			return 0;
		}

		string targetCls = c.Classname;
		if (c.Classnames && c.Classnames.Count() > 0)
		{
			targetCls = c.Classnames[0];
		}

		return ComputeNeedTotalForClass(targetCls, c);
	}

	static int NeedPerCraft(DRL_Craft_Component c, string activeClsOverride = "")
	{
		if (!c)
		{
			return 0;
		}

		if (c.QuantityFrac <= 0.0)
		{
			return c.Amount;
		}

		string targetCls = activeClsOverride;
		if (targetCls == "")
		{
			if (c.Classnames && c.Classnames.Count() > 0)
			{
				targetCls = c.Classnames[0];
			}
			else
			{
				targetCls = c.Classname;
			}
		}

		if (IsFillingByClass(targetCls))
		{
			return ComputeNeedTotalForClass(targetCls, c);
		}

		return c.Amount;
	}

	static int NeedForCount(DRL_Craft_Component c, int countN, string activeClsOverride = "")
	{
		return NeedPerCraft(c, activeClsOverride) * Math.Max(1, countN);
	}

	static int NeedForClassCount(string cls, DRL_Craft_Component c, int countN)
	{
		if (!c)
		{
			return 0;
		}

		if (c.QuantityFrac > 0.0 && IsFillingByClass(cls))
		{
			return ComputeNeedTotalForClass(cls, c) * Math.Max(1, countN);
		}

		return c.Amount * Math.Max(1, countN);
	}

	static string FormatIngredientAmount(string cls, DRL_Craft_Component c, int have, int need, bool withSpaces = false)
	{
		string targetCls = cls;
		if (targetCls == "" && c)
		{
			if (c.Classnames && c.Classnames.Count() > 0)
			{
				targetCls = c.Classnames[0];
			}
			else
			{
				targetCls = c.Classname;
			}
		}

		if (c && c.QuantityFrac > 0.0 && IsFillingByClass(targetCls))
		{
			float qMax = DRL_ItemQuantityHelper.GetQuantityMaxRaw(null, targetCls);
			if (qMax > 0.0)
			{
				int havePct = Math.Round(((float)have / qMax) * 100.0);
				int needPct = Math.Round(((float)need / qMax) * 100.0);
				if (needPct < 1 && need > 0)
				{
					needPct = 1;
				}
				if (withSpaces)
				{
					return havePct.ToString() + "% / " + needPct.ToString() + "%";
				}
				return havePct.ToString() + "%/" + needPct.ToString() + "%";
			}
		}

		if (withSpaces)
		{
			return have.ToString() + " / " + need.ToString();
		}
		return have.ToString() + "/" + need.ToString();
	}

	static string FormatIngredientNeed(string cls, DRL_Craft_Component c, int need)
	{
		string targetCls = cls;
		if (targetCls == "" && c)
		{
			if (c.Classnames && c.Classnames.Count() > 0)
			{
				targetCls = c.Classnames[0];
			}
			else
			{
				targetCls = c.Classname;
			}
		}

		if (c && c.QuantityFrac > 0.0 && IsFillingByClass(targetCls))
		{
			float qMax = DRL_ItemQuantityHelper.GetQuantityMaxRaw(null, targetCls);
			if (qMax > 0.0)
			{
				int needPct = Math.Round(((float)need / qMax) * 100.0);
				if (needPct < 1 && need > 0)
				{
					needPct = 1;
				}
				return needPct.ToString() + "%";
			}
		}

		return need.ToString();
	}

	static int ComputeHaveForClass(string cls, int minHealthFrac, array<EntityAI> cachedItems, float quantityFrac, bool useInheritance, PlayerBase player, EntityAI contextWorkbench)
	{
		if (cls == "")
		{
			return 0;
		}

		if (!player)
		{
			return 0;
		}

		array<EntityAI> items;
		if (cachedItems)
		{
			items = cachedItems;
		}
		else
		{
			GetInventorySnapshot(player, contextWorkbench, items);
		}

		int haveTotal = 0;
		string targetCls = cls;
		targetCls.ToLower();

		for (int i = 0; i < items.Count(); i++)
		{
			EntityAI ent = items[i];
			if (!ent)
			{
				continue;
			}

			InventoryLocation il = new InventoryLocation();
			if (ent.GetInventory() && ent.GetInventory().GetCurrentInventoryLocation(il))
			{
				if (il.GetType() == InventoryLocationType.ATTACHMENT)
				{
					EntityAI parentEnt = il.GetParent();
					if (parentEnt == player)
					{
						continue;
					}
				}
			}

			string typeEnt = ent.GetType();
			typeEnt.ToLower();
			bool isMatch = false;
			if (typeEnt == targetCls)
			{
				isMatch = true;
			}
			else if (useInheritance)
			{
				if (ent.IsKindOf(cls))
				{
					isMatch = true;
				}
			}

			if (!isMatch)
			{
				continue;
			}

			ItemBase ib = ItemBase.Cast(ent);
			if (!ib)
			{
				continue;
			}

			if (ib.IsRuined() || ib.DRL_IsCraftLocked())
			{
				continue;
			}

			int lvl = ib.GetHealthLevel();
			if (lvl > minHealthFrac)
			{
				continue;
			}

			if (quantityFrac > 0.0)
			{
				haveTotal += Math.Round(DRL_ItemQuantityHelper.GetQuantityRaw(ib));
			}
			else
			{
				haveTotal += DRL_ItemQuantityHelper.GetStackAmount(ib);
			}
		}

		return haveTotal;
	}

	static int ComputeHaveForComponent(DRL_Craft_Component c, string activeClsOverride, array<EntityAI> cachedItems, PlayerBase player, EntityAI contextWorkbench)
	{
		if (!c)
		{
			return 0;
		}

		if (c.AttachmentsEx > 0)
		{
			return DRL_CraftComponentResolver.ComputeAvailableCount(c, player, contextWorkbench);
		}

		if (activeClsOverride != "")
		{
			return ComputeHaveForClass(activeClsOverride, c.MinHealthFrac, cachedItems, c.QuantityFrac, c.UseInheritance == 1, player, contextWorkbench);
		}

		if (!player)
		{
			return 0;
		}

		array<EntityAI> items;
		if (cachedItems)
		{
			items = cachedItems;
		}
		else
		{
			GetInventorySnapshot(player, contextWorkbench, items);
		}

		int haveTotal = 0;
		for (int i = 0; i < items.Count(); i++)
		{
			EntityAI ent = items[i];
			if (!ent)
			{
				continue;
			}

			InventoryLocation il = new InventoryLocation();
			if (ent.GetInventory() && ent.GetInventory().GetCurrentInventoryLocation(il))
			{
				if (il.GetType() == InventoryLocationType.ATTACHMENT)
				{
					EntityAI parentEnt = il.GetParent();
					if (parentEnt == player)
					{
						continue;
					}
				}
			}

			bool matchUI = false;
			if (c.Classnames && c.Classnames.Count() > 0)
			{
				string typeEntCls = ent.GetType();
				typeEntCls.ToLower();
				for (int ci = 0; ci < c.Classnames.Count(); ci++)
				{
					string cReq = c.Classnames[ci];
					if (c.UseInheritance == 1)
					{
						if (ent.IsKindOf(cReq))
						{
							matchUI = true;
							break;
						}
					}
					else
					{
						string cReqLower = cReq;
						cReqLower.ToLower();
						if (typeEntCls == cReqLower)
						{
							matchUI = true;
							break;
						}
					}
				}
			}
			else
			{
				if (c.UseInheritance == 1)
				{
					matchUI = ent.IsKindOf(c.Classname);
				}
				else
				{
					string typeEnt = ent.GetType();
					typeEnt.ToLower();
					string reqClsUI = c.Classname;
					reqClsUI.ToLower();
					matchUI = (typeEnt == reqClsUI);
				}
			}

			if (!matchUI)
			{
				continue;
			}

			ItemBase ib = ItemBase.Cast(ent);
			if (!ib)
			{
				continue;
			}

			if (ib.IsRuined())
			{
				continue;
			}

			int lvl = ib.GetHealthLevel();
			if (lvl > c.MinHealthFrac)
			{
				continue;
			}

			if (c.QuantityFrac > 0.0)
			{
				haveTotal += Math.Round(DRL_ItemQuantityHelper.GetQuantityRaw(ib));
			}
			else
			{
				haveTotal += DRL_ItemQuantityHelper.GetStackAmount(ib);
			}
		}

		return haveTotal;
	}

	static void CalculateInventorySlots(PlayerBase player, EntityAI contextWorkbench, out int totalCells, out int usedCells, array<EntityAI> cachedItems = null)
	{
		totalCells = 0;
		usedCells = 0;
		if (!player)
		{
			return;
		}

		array<EntityAI> items;
		if (cachedItems)
		{
			items = cachedItems;
		}
		else
		{
			GetInventorySnapshot(player, contextWorkbench, items);
		}

		for (int i = 0; i < items.Count(); i++)
		{
			EntityAI ent = items[i];
			if (!ent || !ent.GetInventory())
			{
				continue;
			}

			CargoBase cargo = ent.GetInventory().GetCargo();
			if (!cargo)
			{
				continue;
			}

			int w = cargo.GetWidth();
			int h = cargo.GetHeight();
			totalCells += (w * h);

			int cnt = cargo.GetItemCount();
			for (int k = 0; k < cnt; k++)
			{
				EntityAI item = cargo.GetItem(k);
				if (!item)
				{
					continue;
				}

				string cls = item.GetType();
				int sx = 1;
				int sy = 1;

				array<int> dims = new array<int>();
				string p = "CfgVehicles " + cls + " itemSize";
				if (GetGame().ConfigIsExisting(p))
				{
					GetGame().ConfigGetIntArray(p, dims);
				}
				if (dims.Count() < 2)
				{
					dims.Clear();
					p = "CfgWeapons " + cls + " itemSize";
					if (GetGame().ConfigIsExisting(p))
					{
						GetGame().ConfigGetIntArray(p, dims);
					}
				}
				if (dims.Count() < 2)
				{
					dims.Clear();
					p = "CfgMagazines " + cls + " itemSize";
					if (GetGame().ConfigIsExisting(p))
					{
						GetGame().ConfigGetIntArray(p, dims);
					}
				}

				if (dims.Count() >= 2)
				{
					sx = dims[0];
					sy = dims[1];
				}

				usedCells += (sx * sy);
			}
		}
	}
}
