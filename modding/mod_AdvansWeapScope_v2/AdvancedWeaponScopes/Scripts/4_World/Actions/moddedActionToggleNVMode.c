modded class ActionToggleNVMode
{
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		ItemOptics optics;
		if ( Class.CastTo(optics,item) && ( optics.IsKindOf("AD_PVS4_B13") || optics.IsKindOf("AD_PVS4_MosinMount") ))
		{
			if ( optics.FindAttachmentBySlotName("BatteryD") != NULL )
			{
				return true;
			}
			return false;
		}
		return super.ActionCondition(player, target, item);
	}
};