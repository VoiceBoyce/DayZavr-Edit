// DayZavr: ящик 5×5 со своим хранилищем 10×10 (расширение станка и подъёмника).
// Чтобы он не стал карманом на 100 мест внутри рюкзака:
// - в чужое хранилище кладётся только пустым;
// - пока лежит в чужом хранилище, в него самого ничего не положить.
// На станке, подъёмнике, в руках и на земле работает как раньше.
class DRL_Craft_Box extends Inventory_Base
{
	protected bool DZV_IsInCargo()
	{
		InventoryLocation il = new InventoryLocation();
		if (!GetInventory() || !GetInventory().GetCurrentInventoryLocation(il))
			return false;
		return il.GetType() == InventoryLocationType.CARGO;
	}

	override bool CanPutInCargo(EntityAI parent)
	{
		if (!super.CanPutInCargo(parent))
			return false;
		CargoBase cargo = GetInventory().GetCargo();
		if (cargo && cargo.GetItemCount() > 0)
			return false;
		return true;
	}

	override bool CanReceiveItemIntoCargo(EntityAI item)
	{
		if (DZV_IsInCargo())
			return false;
		return super.CanReceiveItemIntoCargo(item);
	}

	override bool CanSwapItemInCargo(EntityAI child_entity, EntityAI new_entity)
	{
		if (DZV_IsInCargo())
			return false;
		return super.CanSwapItemInCargo(child_entity, new_entity);
	}
}
