class SFP_TenevnikUtils
{
	// Важно для производительности сервера:
	// клон нужен в первую очередь для ВНЕШНЕГО вида (одежда/оружие/аттачменты),
	// а содержимое карго (внутри одежды/рюкзака/подсумков) игроки не видят.
	// Поэтому карго НЕ копируем, чтобы не плодить сотни лишних EntityAI и не грузить сервер.
	static const bool COPY_CARGO_FROM_PLAYER = false;

	// ВАЖНО:
	// InventorySlots.GetSlotIdFromString ожидает ИМЯ СЛОТА (поле CfgSlots "name"),
	// например "Vest", "Body", "Headgear" ... а НЕ "Slot_Vest".
	static ref array<string> PLAYER_EQUIPMENT_SLOTS = {
		// слоты из твоего attachments[] + типовые слоты экипировки игрока
		"Shoulder",
		"Headgear",
		"Mask",
		"Eyewear",
		"Gloves",
		"Armband",
		"Vest",
		"Body",
		"Back",
		"Hips",
		"Legs",
		"Feet"
	};

	static void CopyPlayerToClone(PlayerBase srcPlayer, EntityAI dstClone)
	{
		if (!srcPlayer || !dstClone)
			return;

		// Копирование экипировки делаем только на сервере (клиент получит результат по сети)
		if (!GetGame().IsServer())
			return;

		// 1) очищаем только управляемые слоты (нельзя удалять все вложения у заражённых)
		for (int i = 0; i < PLAYER_EQUIPMENT_SLOTS.Count(); i++)
		{
			int slotId = InventorySlots.GetSlotIdFromString(PLAYER_EQUIPMENT_SLOTS[i]);
			if (slotId <= 0)
				continue;

			EntityAI existing = dstClone.GetInventory().FindAttachment(slotId);
			if (existing)
				GetGame().ObjectDelete(existing);
		}

		// 2) копируем по слотам
		for (int s = 0; s < PLAYER_EQUIPMENT_SLOTS.Count(); s++)
		{
			int slotId2 = InventorySlots.GetSlotIdFromString(PLAYER_EQUIPMENT_SLOTS[s]);
			if (slotId2 <= 0)
				continue;

			EntityAI srcAtt = srcPlayer.GetInventory().FindAttachment(slotId2);
			if (!srcAtt)
				continue;

			// ВАЖНО: For infected clones with proxies, we must create as вложений into a concrete slot ид.
			EntityAI dstAtt = dstClone.GetInventory().CreateAttachmentEx(srcAtt.GetType(), slotId2);
			if (!dstAtt)
				continue;

			CopyEntityState(srcAtt, dstAtt);
			CopyAttachmentsRecursive(srcAtt, dstAtt);
			if (COPY_CARGO_FROM_PLAYER)
				CopyCargoRecursive(srcAtt, dstAtt);
		}
	}

	static void CopyEntityState(EntityAI src, EntityAI dst)
	{
		if (!src || !dst)
			return;

		// Лучший ванильный хелпер: переносит здоровье/агентов/энергию/состояния магазинов и оружия и т.п.
		// Некоторые предметы всё равно требуют ручного копирования (патроны) — делаем ниже.
		MiscGameplayFunctions.TransferItemProperties(src, dst, true, true, true, true);

		// патроны (магазины) — синхронизируем количество
		Magazine srcMag = Magazine.Cast(src);
		Magazine dstMag = Magazine.Cast(dst);
		if (srcMag && dstMag)
		{
			if (GetGame().IsServer())
				dstMag.ServerSetAmmoCount(srcMag.GetAmmoCount());
		}
	}

	static void CopyAttachmentsRecursive(EntityAI src, EntityAI dst)
	{
		if (!src || !dst)
			return;

		int acnt = src.GetInventory().AttachmentCount();
		for (int i = 0; i < acnt; i++)
		{
			EntityAI srcAtt = src.GetInventory().GetAttachmentFromIndex(i);
			if (!srcAtt)
				continue;

			InventoryLocation il = new InventoryLocation;
			if (!srcAtt.GetInventory().GetCurrentInventoryLocation(il))
				continue;

			if (il.GetType() != InventoryLocationType.ATTACHMENT)
				continue;

			int slotId = il.GetSlot();
			if (slotId <= 0)
				continue;

			// создаём только если у цели есть такой слот
			if (!dst.GetInventory().HasInventorySlot(slotId))
				continue;

			EntityAI dstAtt = dst.GetInventory().CreateAttachmentEx(srcAtt.GetType(), slotId);
			if (!dstAtt)
				continue;

			CopyEntityState(srcAtt, dstAtt);
			CopyAttachmentsRecursive(srcAtt, dstAtt);
			if (COPY_CARGO_FROM_PLAYER)
				CopyCargoRecursive(srcAtt, dstAtt);
		}
	}

	static void CopyCargoRecursive(EntityAI src, EntityAI dst)
	{
		if (!src || !dst)
			return;

		CargoBase srcCargo = src.GetInventory().GetCargo();
		CargoBase dstCargo = dst.GetInventory().GetCargo();
		if (!srcCargo || !dstCargo)
			return;

		int itemCount = srcCargo.GetItemCount();
		for (int i = 0; i < itemCount; i++)
		{
			EntityAI srcItem = srcCargo.GetItem(i);
			if (!srcItem)
				continue;

			InventoryLocation il = new InventoryLocation;
			if (!srcItem.GetInventory().GetCurrentInventoryLocation(il))
				continue;

			EntityAI dstItem;
			if (il.GetType() == InventoryLocationType.CARGO)
			{
				dstItem = dst.GetInventory().CreateEntityInCargoEx(srcItem.GetType(), il.GetIdx(), il.GetRow(), il.GetCol(), il.GetFlip());
			}

			if (!dstItem)
			{
				// фоллбек (если точное размещение не получилось)
				dstItem = dst.GetInventory().CreateInInventory(srcItem.GetType());
			}

			if (!dstItem)
				continue;

			CopyEntityState(srcItem, dstItem);
			CopyAttachmentsRecursive(srcItem, dstItem);
			CopyCargoRecursive(srcItem, dstItem);
		}
	}
}
