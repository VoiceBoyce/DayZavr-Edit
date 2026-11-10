class AD_NFATACR : ItemOptics {};
class AD_NFATACR_MosinMount : ItemOptics {};
class AD_NFATACR_B13 : ItemOptics {};

class AD_NFATACR_B13_RMR : ItemOptics
{
	// ATACR — 2D-оптика, для таких прицелов ваниль ставит точку только в 2D-сетку.
	// Точка RMR сверху лежит на 3D-модели (выборка reddot), её ставим сами.
	override void ShowReddot(bool state)
	{
		super.ShowReddot(state);

		if (g_Game.IsHeadlessOrDedicatedServer() || !IsUsingOptics2DModel())
			return;

		int idx = GetHiddenSelectionIndex("reddot");
		if (idx < 0)
			return;

		if (state)
		{
			string tex;
			g_Game.ConfigGetText("cfgVehicles " + GetType() + " OpticsInfo opticSightTexture", tex);
			SetObjectTexture(idx, tex);
		}
		else
		{
			SetObjectTexture(idx, "");
		}
	}

	override void OnWork(float consumed_energy)
	{
		super.OnWork(consumed_energy);

		AD_RmrDot.CheckUnused(this);
	}
};