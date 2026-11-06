modded class PlayerBase {

	// MARK: - Private Properties --------------------------------------------------------------------------

	private autoptr RF_NICKNAMES_CL_HeadLabelHud RF_NICKNAMES_CL_headLabelHud;

	// MARK: - Init --------------------------------------------------------------------------

	void PlayerBase() {
		if (GetGame().IsServer()) return;
		this.RF_NICKNAMES_CL_headLabelHud = new RF_NICKNAMES_CL_HeadLabelHud();
		this.RF_NICKNAMES_CL_headLabelHud.ConfigureWith(this);
	}

	void ~PlayerBase() {
		// RF_NICKNAMES_Log().Info(string.Format("~%1", this));
		this.RF_NICKNAMES_CL_headLabelHud = null;
	}

	// MARK: - Override Methods --------------------------------------------------------------------------

	override void EOnFrame(IEntity other, float timeSlice) {
		super.EOnFrame(other, timeSlice);

		if (!this.RF_NICKNAMES_CL_headLabelHud) return;

		if (!IsAlive()) {
			this.RF_NICKNAMES_CL_headLabelHud = null;
			return
		}

		this.RF_NICKNAMES_CL_headLabelHud.OnUpdate();
	}
}
