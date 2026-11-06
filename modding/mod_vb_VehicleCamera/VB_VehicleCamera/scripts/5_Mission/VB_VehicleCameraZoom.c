// Колесо мыши в ванилле занято выбором действий (UAPrevAction/UANextAction).
// При открытом круговом меню и с зажатым Alt в транспорте щелчок идёт на расстояние камеры
// и список действий не сдвигается.

modded class ActionMenu
{
	override void NextAction()
	{
		if (VB_VehicleCameraRig.TryZoom(1))
			return;

		super.NextAction();
	}

	override void PrevAction()
	{
		if (VB_VehicleCameraRig.TryZoom(-1))
			return;

		super.PrevAction();
	}
}
