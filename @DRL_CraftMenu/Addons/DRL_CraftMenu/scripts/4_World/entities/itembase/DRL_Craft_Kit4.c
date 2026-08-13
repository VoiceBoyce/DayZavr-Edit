class DRL_Craft_Kit4 extends ItemBase
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

		if (!GetGame().IsServer())
			return;

		Object obj = GetGame().CreateObjectEx("DRL_Craft_WB4", position, ECE_PLACE_ON_SURFACE);
		if (obj)
		{
			obj.SetPosition(position);
			obj.SetOrientation(orientation);
		}

		GetGame().ObjectDelete(this);
	}
}