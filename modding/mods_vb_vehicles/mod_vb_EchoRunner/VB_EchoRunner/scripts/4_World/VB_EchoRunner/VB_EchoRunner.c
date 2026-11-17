// Echo Runner (VB_EchoRunner): самосборное купе для сервера 16.
// Переопределения — по ванильному Sedan_02 (scripts\4_world\entities\vehicles\inheritedcars\sedan_02.c, 1.30):
// мотор сзади (аккумулятор и свеча под задней крышкой VB_ER_Trunk), радиатор и багажник спереди (VB_ER_Hood).
// Места: 0 — водитель (левый борт), 1 — пассажир. Двери «крыло чайки» — обычные CarDoor со своими слотами.

class VB_EchoRunner extends CarScript
{
	static const string SLOT_DOOR_DRIVER   = "VB_ER_Door_Driver";
	static const string SLOT_DOOR_CODRIVER = "VB_ER_Door_CoDriver";
	static const string SLOT_HOOD          = "VB_ER_Hood";
	static const string SLOT_TRUNK         = "VB_ER_Trunk";

	void VB_EchoRunner()
	{
		m_dmgContactCoef        = 0.0790;

		m_EngineStartOK         = "Sedan_02_engine_start_SoundSet";
		m_EngineStartBattery    = "Sedan_02_engine_failed_start_battery_SoundSet";
		m_EngineStartPlug       = "Sedan_02_engine_failed_start_sparkplugs_SoundSet";
		m_EngineStartFuel       = "Sedan_02_engine_failed_start_fuel_SoundSet";
		m_EngineStop            = "Sedan_02_engine_stop_SoundSet";
		m_EngineStopFuel        = "Sedan_02_engine_stop_fuel_SoundSet";

		m_CarDoorOpenSound      = "offroad_door_open_SoundSet";
		m_CarDoorCloseSound     = "offroad_door_close_SoundSet";

		m_LightsComponent.RegisterLight("Front", new VehicleLightData(new Sedan_02LightProfileFront()));
		m_LightsComponent.RegisterLight("Rear", new VehicleLightData(new Sedan_02LightProfileRear()));

		m_HornComponent.RegisterSound(VehicleHornMode.SHORT, "Sedan_02_Horn_Short_SoundSet");
		m_HornComponent.RegisterSound(VehicleHornMode.LONG, "Sedan_02_Horn_SoundSet");

		SetEnginePos("0 0.6 1.6");   // мотор сзади (нос модели в -Z)
	}

	// ---------------------------------------------------------------- красная подсветка (выделение neon)
	// Горит вместе со светом машины. Пути к .rvmat — с ведущим «\», иначе бинарная модель их не грузит (E75).
	static const string NEON_ON  = "\\VB_EchoRunner\\data\\er_neon_e.rvmat";
	static const string NEON_OFF = "\\VB_EchoRunner\\data\\er_neon.rvmat";

	override void UpdateLightsClient(int newGear = -1)
	{
		super.UpdateLightsClient(newGear);

		int idx = GetHiddenSelectionIndex("neon");
		if (idx < 0)
			return;
		if (LightIsOn())
			SetObjectMaterial(idx, NEON_ON);
		else
			SetObjectMaterial(idx, NEON_OFF);
	}

	// ---------------------------------------------------------------- аккумулятор (контракт 1.30)
	override bool NeedElectricitySourceDevice()
	{
		return true;
	}

	override EntityAI GetElectricitySourceDevice()
	{
		super.GetElectricitySourceDevice();
		return GetInventory().FindAttachment(CarBattery.SLOT_ID);
	}

	override string GetElectricitySourceDeviceType()
	{
		return "CarBattery";
	}

	override bool IsVitalGlowPlug()
	{
		return false;   // бензиновый: нужна свеча зажигания, калильная не нужна
	}

	// ---------------------------------------------------------------- посадка
	override int GetAnimInstance()
	{
		return VehicleAnimInstances.S120;
	}

	override int GetSeatAnimationType(int posIdx)
	{
		switch (posIdx)
		{
			case 0:
				return DayZPlayerConstants.VEHICLESEAT_DRIVER;
			case 1:
				return DayZPlayerConstants.VEHICLESEAT_CODRIVER;
		}
		return 0;
	}

	override bool CrewCanGetThrough(int posIdx)
	{
		switch (posIdx)
		{
			case 0:
				return GetCarDoorsState(SLOT_DOOR_DRIVER) != CarDoorState.DOORS_CLOSED;
			case 1:
				return GetCarDoorsState(SLOT_DOOR_CODRIVER) != CarDoorState.DOORS_CLOSED;
		}
		return false;
	}

	override string GetDoorSelectionNameFromSeatPos(int posIdx)
	{
		switch (posIdx)
		{
			case 0:
				return "doors_driver";
			case 1:
				return "doors_codriver";
		}
		return super.GetDoorSelectionNameFromSeatPos(posIdx);
	}

	override string GetDoorInvSlotNameFromSeatPos(int posIdx)
	{
		switch (posIdx)
		{
			case 0:
				return SLOT_DOOR_DRIVER;
			case 1:
				return SLOT_DOOR_CODRIVER;
		}
		return super.GetDoorInvSlotNameFromSeatPos(posIdx);
	}

	override bool CanReachSeatFromSeat(int currentSeat, int nextSeat)
	{
		if (currentSeat == 0)
			return nextSeat == 1;
		if (currentSeat == 1)
			return nextSeat == 0;
		return false;
	}

	override bool CanReachDoorsFromSeat(string pDoorsSelection, int pCurrentSeat)
	{
		if (pCurrentSeat == 0)
			return pDoorsSelection == "DoorsDriver";
		if (pCurrentSeat == 1)
			return pDoorsSelection == "DoorsCoDriver";
		return false;
	}

	// ---------------------------------------------------------------- двери и крышки
	override int GetCarDoorsState(string slotType)
	{
		CarDoor carDoor;
		Class.CastTo(carDoor, FindAttachmentBySlotName(slotType));
		if (!carDoor)
			return CarDoorState.DOORS_MISSING;

		switch (slotType)
		{
			case "VB_ER_Door_Driver":
				return TranslateAnimationPhaseToCarDoorState("DoorsDriver");
			case "VB_ER_Door_CoDriver":
				return TranslateAnimationPhaseToCarDoorState("DoorsCoDriver");
			case "VB_ER_Hood":
				return TranslateAnimationPhaseToCarDoorState("DoorsHood");
			case "VB_ER_Trunk":
				return TranslateAnimationPhaseToCarDoorState("DoorsTrunk");
		}
		return CarDoorState.DOORS_MISSING;
	}

	override string GetAnimSourceFromSelection(string selection)
	{
		switch (selection)
		{
			case "doors_driver":
				return "DoorsDriver";
			case "doors_codriver":
				return "DoorsCoDriver";
			case "doors_hood":
				return "DoorsHood";
			case "doors_trunk":
				return "DoorsTrunk";
		}
		return "";
	}

	override float OnSound(CarSoundCtrl ctrl, float oldValue)
	{
		if (ctrl == CarSoundCtrl.DOORS)
		{
			float newValue = 0;
			if (GetCarDoorsState(SLOT_DOOR_DRIVER) == CarDoorState.DOORS_CLOSED)
				newValue += 0.5;
			if (GetCarDoorsState(SLOT_DOOR_CODRIVER) == CarDoorState.DOORS_CLOSED)
				newValue += 0.5;
			return Math.Clamp(newValue, 0, 1);
		}
		return super.OnSound(ctrl, oldValue);
	}

	// ---------------------------------------------------------------- доступ к деталям мотора и багажнику
	override bool CanReleaseAttachment(EntityAI attachment)
	{
		if (!super.CanReleaseAttachment(attachment))
			return false;

		string attType = attachment.GetType();
		if (attType == "CarBattery" || attType == "SparkPlug")
		{
			if (GetCarDoorsState(SLOT_TRUNK) == CarDoorState.DOORS_CLOSED || EngineIsOn())
				return false;
		}
		else if (attType == "CarRadiator")
		{
			if (GetCarDoorsState(SLOT_HOOD) == CarDoorState.DOORS_CLOSED || EngineIsOn())
				return false;
		}
		return true;
	}

	override bool CanDisplayAttachmentCategory(string category_name)
	{
		if (!super.CanDisplayAttachmentCategory(category_name))
			return false;

		category_name.ToLower();
		if (category_name.Contains("engine") && GetCarDoorsState(SLOT_TRUNK) == CarDoorState.DOORS_CLOSED)
			return false;
		if (category_name.Contains("trunk") && GetCarDoorsState(SLOT_HOOD) == CarDoorState.DOORS_CLOSED)
			return false;
		return true;
	}

	override bool CanDisplayCargo()
	{
		if (!super.CanDisplayCargo())
			return false;
		return GetCarDoorsState(SLOT_HOOD) != CarDoorState.DOORS_CLOSED;   // багажник спереди
	}

	// ---------------------------------------------------------------- обвесы (заготовка)
	// Обвес ставится в слот, как колесо. Здесь потом: прятать бампер под обвесом «Перед»/«Корма»
	// (анимации hide_bumper_front / hide_bumper_rear) и пересчитывать, что даёт машине каждая деталь.
	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);
		VB_ER_OnObvesChanged();
	}

	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);
		VB_ER_OnObvesChanged();
	}

	protected void VB_ER_OnObvesChanged()
	{
	}

	// ---------------------------------------------------------------- спавн для проверки: всё сразу на местах
	override void OnDebugSpawn()
	{
		FillUpCarFluids();
		Fill(CarFluid.BRAKE, GetFluidCapacity(CarFluid.BRAKE));   // FillUpCarFluids не льёт тормозную

		VB_ER_Attach("CarBattery", "CarBattery");
		VB_ER_Attach("SparkPlug", "SparkPlug");
		VB_ER_Attach("CarRadiator", "CarRadiator");
		VB_ER_Attach("HeadlightH7", "Reflector_1_1");
		VB_ER_Attach("HeadlightH7", "Reflector_2_1");
		VB_ER_Attach("VB_EchoRunner_Wheel", "VB_ER_Wheel_1_1");
		VB_ER_Attach("VB_EchoRunner_Wheel", "VB_ER_Wheel_1_2");
		VB_ER_Attach("VB_EchoRunner_Wheel", "VB_ER_Wheel_2_1");
		VB_ER_Attach("VB_EchoRunner_Wheel", "VB_ER_Wheel_2_2");
		VB_ER_Attach("VB_EchoRunner_Door_Driver", SLOT_DOOR_DRIVER);
		VB_ER_Attach("VB_EchoRunner_Door_CoDriver", SLOT_DOOR_CODRIVER);
		VB_ER_Attach("VB_EchoRunner_Hood", SLOT_HOOD);
		VB_ER_Attach("VB_EchoRunner_Trunk", SLOT_TRUNK);
	}

	protected void VB_ER_Attach(string type, string slot)
	{
		int slotId = InventorySlots.GetSlotIdFromString(slot);
		if (slotId == InventorySlots.INVALID)
			return;
		if (!GetInventory().FindAttachment(slotId))
			GetInventory().CreateAttachmentEx(type, slotId);
	}
}
