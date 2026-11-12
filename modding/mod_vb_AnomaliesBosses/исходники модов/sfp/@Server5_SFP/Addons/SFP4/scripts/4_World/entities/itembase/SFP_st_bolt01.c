// st_bolt_fidov
// Script extension for SFP_st_bolt01 (cfg: Inventory_Base).
// Adds a custom action and supports timed cleanup for bolts spawned by the throw action.

class SFP_st_bolt01 extends Inventory_Base
{
	protected bool m_ST_SpawnedAsThrown;

	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionSTThrowBoltFidov);
	}

	// Mark this instance as a thrown bolt and schedule deletion.
	void ST_SetSpawnedAsThrown()
	{
		m_ST_SpawnedAsThrown = true;

		if (GetGame() && GetGame().IsServer())
		{
			// Delete after 12 seconds
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ST_DeleteSelf, 12000, false);
		}
	}

	protected void ST_DeleteSelf()
	{
		if (!GetGame() || !GetGame().IsServer())
			return;

		// If the entity still exists, delete it (even if it was picked up)
		GetGame().ObjectDelete(this);
	}
}
