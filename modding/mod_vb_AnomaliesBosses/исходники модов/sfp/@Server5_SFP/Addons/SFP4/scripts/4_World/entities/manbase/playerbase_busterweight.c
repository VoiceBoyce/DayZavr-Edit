// Buster — carry-бонус (совместимо с другими модами)
//
// Раньше бонус делался через override StaminaHandler.Update(), что часто ломает "кастомную стамину"
// и любые моды, которые тоже правят Update.
//
// Теперь мы делаем игрока "легче" на CARRY_BONUS_G, пока активен BusterMdfr.
// Так бонус автоматически учитывается и ванилью, и другими модами (если они берут вес через GetWeightEx).
//
// ВАЖНО: GetWeightEx кешируется. Поэтому в BusterMdfr.OnActivate/OnDeactivate мы вызываем SetWeightDirty().

modded class PlayerBase
{
	override protected float GetWeightSpecialized(bool forceRecalc = false)
	{
		float totalWeight = super.GetWeightSpecialized(forceRecalc);

		ModifiersManager mm = GetModifiersManager();
		if (mm)
		{
			ModifierBase m = mm.GetModifier(BusterWorldConstants.MDF_BUSTER);
			if (m && m.IsActive())
			{
				totalWeight = Math.Max(0.0, totalWeight - BusterWorldConstants.CARRY_BONUS_G);
			}
		}

		return totalWeight;
	}
}
