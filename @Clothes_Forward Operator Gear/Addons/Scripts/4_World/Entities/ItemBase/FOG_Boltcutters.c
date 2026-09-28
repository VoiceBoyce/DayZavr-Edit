class FOG_BoltCutters_Base extends ToolBase
{
	void FOG_BoltCutters_Base()
	{
		m_MineDisarmRate = 20;
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