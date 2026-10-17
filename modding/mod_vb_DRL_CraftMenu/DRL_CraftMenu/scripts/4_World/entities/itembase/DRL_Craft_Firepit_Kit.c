class DRL_Craft_Firepit_Kit extends ItemBase
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionTogglePlaceObject);
		AddAction(ActionDeployObject);
	}

	override bool IsDeployable()
	{
		return true;
	}

	override void OnPlacementComplete(Man player, vector position = "0 0 0", vector orientation = "0 0 0")
	{
		super.OnPlacementComplete(player, position, orientation);

		if (!g_Game.IsServer())
		{
			return;
		}

		Object obj = g_Game.CreateObjectEx("DRL_Craft_Firepit", position, ECE_PLACE_ON_SURFACE);
		if (obj)
		{
			obj.SetPosition(position);
			obj.SetOrientation(orientation);
		}

		g_Game.ObjectDelete(this);
	}
}
