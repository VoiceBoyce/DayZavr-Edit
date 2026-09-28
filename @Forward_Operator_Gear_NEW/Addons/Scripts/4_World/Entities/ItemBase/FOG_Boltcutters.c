class FOG_BoltCutters_Base extends ToolBase
{
	void FOG_BoltCutters_Base()
	{
		m_MineDisarmRate = 20;
	}
	override int GetKeyCompatibilityType()
	{
		return 1 << EBuildingLockType.LOCKPICK;
	}
	override void SetActions()
	{
		super.SetActions();

		AddAction(ActionMineBush);
		AddAction(ActionUnrestrainTarget);
		AddAction(ActionLockDoors);
		AddAction(ActionUnlockDoors);
		AddAction(ActionDisarmMine);
		AddAction(ActionDisarmExplosive);
	}
};