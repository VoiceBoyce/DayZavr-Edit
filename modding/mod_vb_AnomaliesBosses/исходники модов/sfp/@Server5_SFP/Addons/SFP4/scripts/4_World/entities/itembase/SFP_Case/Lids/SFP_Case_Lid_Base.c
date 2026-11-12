class SFP_Case_Lid_Base : Inventory_Base
{
	// Intentionally minimal; physics is handled by the spawning code (ThrowPhysically / impulse).
	override bool CanPutInCargo(EntityAI parent)
	{
		return super.CanPutInCargo(parent);
	}
}
