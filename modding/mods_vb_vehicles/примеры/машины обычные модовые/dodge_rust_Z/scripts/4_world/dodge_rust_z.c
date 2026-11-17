class dodge_rust_Z extends CarScript {
  void dodge_rust_Z() {
    m_dmgContactCoef = 0.000;
    m_EngineStartOK = "dodge_rust_Z_Engine_start_SoundSet";
    m_EngineStartBattery = "offroad_engine_failed_start_battery_SoundSet";
    m_EngineStartPlug = "offroad_engine_failed_start_sparkplugs_SoundSet";
    m_EngineStartFuel = "offroad_engine_failed_start_fuel_SoundSet";
    m_EngineStopFuel = "offroad_engine_stop_fuel_SoundSet";

    m_CarDoorOpenSound = "offroad_door_open_SoundSet";
    m_CarDoorCloseSound = "offroad_door_close_SoundSet";

    m_CarHornShortSoundName = "Offroad_Horn_Short_SoundSet";
    m_CarHornLongSoundName = "Offroad_Horn_SoundSet";

    m_HornComponent.RegisterSound(VehicleHornMode.SHORT, "Offroad_Horn_Short_SoundSet");
    m_HornComponent.RegisterSound(VehicleHornMode.LONG, "Offroad_Horn_SoundSet");

    m_LightsComponent.RegisterLight("Front", new VehicleLightData(new dodge_rust_ZLightProfileFront()));
    m_LightsComponent.RegisterLight("Rear", new VehicleLightData(new dodge_rust_ZLightProfileRear()));

    SetEnginePos("0 0.7 1.6");
  }

  override int GetAnimInstance() // НЕ ТРОГАТЬ
  {
    return VehicleAnimInstances.SEDAN;
  }

  override int GetSeatAnimationType(int posIdx) {
    switch (posIdx) {
    case 0:
      return DayZPlayerConstants.VEHICLESEAT_DRIVER;
    case 1:
      return DayZPlayerConstants.VEHICLESEAT_CODRIVER;
    case 2:
      return DayZPlayerConstants.VEHICLESEAT_PASSENGER_L;
    case 3:
      return DayZPlayerConstants.VEHICLESEAT_PASSENGER_R;
    }

    return 0;
  }

  override bool CanReleaseAttachment(EntityAI attachment) {
    if (!super.CanReleaseAttachment(attachment))
      return false;

    string attType = attachment.GetType();

    if (EngineIsOn() || GetCarDoorsState("dodge_rust_Z_doors_hood") == CarDoorState.DOORS_CLOSED) {
      if (attType == "CarRadiator" || attType == "CarBattery" || attType == "SparkPlug")
        return false;
    }

    return true;
  }

  override bool CanDisplayCargo() {
    if (!super.CanDisplayCargo())
      return false;

    if (GetCarDoorsState("dodge_rust_Z_doors_trunk") == CarDoorState.DOORS_CLOSED)
      return false;

    return true;
  }

  override int GetCarDoorsState(string slotType) {
    CarDoor carDoor;

    Class.CastTo(carDoor, FindAttachmentBySlotName(slotType));
    if (!carDoor)
      return CarDoorState.DOORS_MISSING;

    switch (slotType) {
    case "dodge_rust_Z_doors_driver":
      if (GetAnimationPhase("DoorsDriver") > 0.5)
        return CarDoorState.DOORS_OPEN;
      else
        return CarDoorState.DOORS_CLOSED;

      break;

    case "dodge_rust_Z_doors_codriver":
      if (GetAnimationPhase("DoorsCoDriver") > 0.5)
        return CarDoorState.DOORS_OPEN;
      else
        return CarDoorState.DOORS_CLOSED;

      break;

    case "dodge_rust_Z_doors_cargo1":
      if (GetAnimationPhase("DoorsCargo1") > 0.5)
        return CarDoorState.DOORS_OPEN;
      else
        return CarDoorState.DOORS_CLOSED;

      break;

    case "dodge_rust_Z_doors_cargo2":
      if (GetAnimationPhase("DoorsCargo2") > 0.5)
        return CarDoorState.DOORS_OPEN;
      else
        return CarDoorState.DOORS_CLOSED;
      break;

    case "dodge_rust_Z_doors_hood":
      if (GetAnimationPhase("DoorsHood") > 0.5)
        return CarDoorState.DOORS_OPEN;
      else
        return CarDoorState.DOORS_CLOSED;
      break;

    case "dodge_rust_Z_doors_trunk":
      if (GetAnimationPhase("DoorsTrunk") > 0.5)
        return CarDoorState.DOORS_OPEN;
      else
        return CarDoorState.DOORS_CLOSED;
      break;
    }

    return CarDoorState.DOORS_MISSING;
  }

  override bool CrewCanGetThrough(int posIdx) {
    switch (posIdx) {
    case 0:
      if (GetCarDoorsState("dodge_rust_Z_doors_driver") == CarDoorState.DOORS_CLOSED)
        return false;

      return true;
      break;

    case 1:
      if (GetCarDoorsState("dodge_rust_Z_doors_codriver") == CarDoorState.DOORS_CLOSED)
        return false;

      return true;
      break;

    case 2:
      if (GetCarDoorsState("dodge_rust_Z_doors_cargo1") == CarDoorState.DOORS_CLOSED)
        return false;

      return true;
      break;

    case 3:
      if (GetCarDoorsState("dodge_rust_Z_doors_cargo2") == CarDoorState.DOORS_CLOSED)
        return false;

      return true;
      break;
    }

    return false;
  }

  override string GetDoorSelectionNameFromSeatPos(int posIdx) {
    switch (posIdx) {
    case 0:
      return "doors_driver";
      break;
    case 1:
      return "doors_codriver";
      break;
    case 2:
      return "doors_cargo1";
      break;
    case 3:
      return "doors_cargo2";
      break;
    }

    return super.GetDoorSelectionNameFromSeatPos(posIdx);
  }

  override string GetDoorInvSlotNameFromSeatPos(int posIdx) {
    switch (posIdx) {
    case 0:
      return "dodge_rust_Z_doors_driver";
      break;
    case 1:
      return "dodge_rust_Z_doors_codriver";
      break;
    case 2:
      return "dodge_rust_Z_doors_cargo1";
      break;
    case 3:
      return "dodge_rust_Z_doors_cargo2";
      break;
    }

    return super.GetDoorInvSlotNameFromSeatPos(posIdx);
  }

  override float OnSound(CarSoundCtrl ctrl, float oldValue) {
    switch (ctrl) {
    case CarSoundCtrl.DOORS:
      float newValue = 0;

      //-----
      if (GetCarDoorsState("dodge_rust_Z_doors_driver") == CarDoorState.DOORS_CLOSED)
        newValue += 0.25;

      //-----
      if (GetCarDoorsState("dodge_rust_Z_doors_codriver") == CarDoorState.DOORS_CLOSED)
        newValue += 0.25;

      //-----
      if (GetCarDoorsState("dodge_rust_Z_doors_cargo1") == CarDoorState.DOORS_CLOSED)
        newValue += 0.25;

      //-----
      if (GetCarDoorsState("dodge_rust_Z_doors_cargo2") == CarDoorState.DOORS_CLOSED)
        newValue += 0.25;

      //-----
      if (GetHealthLevel("WindowFront") == GameConstants.STATE_RUINED)
        newValue -= 0.6;

      //-----
      if (GetHealthLevel("WindowBack") == GameConstants.STATE_RUINED)
        newValue -= 0.6;

      if (newValue > 1)
        newValue = 1;

      if (newValue < 0)
        newValue = 0;

      return newValue;
      break;
    }

    return oldValue;
  }

  override string GetAnimSourceFromSelection(string selection) {
    switch (selection) {
    case "doors_driver":
      return "DoorsDriver";
    case "doors_codriver":
      return "DoorsCoDriver";
    case "doors_cargo1":
      return "DoorsCargo1";
    case "doors_cargo2":
      return "DoorsCargo2";
    case "doors_hood":
      return "DoorsHood";
    case "doors_trunk":
      return "DoorsTrunk";
    }

    return "";
  }

  override bool NeedElectricitySourceDevice() { return true; }

  override EntityAI GetElectricitySourceDevice()
  {
    super.GetElectricitySourceDevice();

    return GetInventory().FindAttachment(CarBattery.SLOT_ID);
  }

  override string GetElectricitySourceDeviceType() { return "CarBattery"; }

  override bool IsVitalGlowPlug() { return false; }

  override bool CanReachSeatFromSeat(int currentSeat, int nextSeat) {
    switch (currentSeat) {
    case 0:
      if (nextSeat == 1)
        return true;
      break;
    case 1:
      if (nextSeat == 0)
        return true;
      break;
    case 2:
      if (nextSeat == 3)
        return true;
      break;
    case 3:
      if (nextSeat == 2)
        return true;
      break;
    }

    return false;
  }

  override bool CanReachDoorsFromSeat(string pDoorsSelection, int pCurrentSeat) {
    switch (pCurrentSeat) {
    case 0:
      if (pDoorsSelection == "DoorsDriver") {
        return true;
      }
      break;
    case 1:
      if (pDoorsSelection == "DoorsCoDriver") {
        return true;
      }
      break;
    case 2:
      if (pDoorsSelection == "DoorsCargo1") {
        return true;
      }
      break;
    case 3:
      if (pDoorsSelection == "DoorsCargo2") {
        return true;
      }
      break;
    }

    return false;
  }

  override void OnDebugSpawn() {
    EntityAI entity;

    if (Class.CastTo(entity, this)) {
      entity.GetInventory().CreateInInventory("dodge_rust_Z_doors_driver");
      entity.GetInventory().CreateInInventory("dodge_rust_Z_doors_codriver");
      entity.GetInventory().CreateInInventory("dodge_rust_Z_doors_cargo1");
      entity.GetInventory().CreateInInventory("dodge_rust_Z_doors_cargo2");
      entity.GetInventory().CreateInInventory("dodge_rust_Z_doors_hood");
      entity.GetInventory().CreateInInventory("dodge_rust_Z_doors_trunk");

      entity.GetInventory().CreateInInventory("dodge_rust_Z_wheel");
      entity.GetInventory().CreateInInventory("dodge_rust_Z_wheel");
      entity.GetInventory().CreateInInventory("dodge_rust_Z_wheel");
      entity.GetInventory().CreateInInventory("dodge_rust_Z_wheel");
      entity.GetInventory().CreateInInventory("dodge_rust_Z_wheel");
      entity.GetInventory().CreateInInventory("dodge_rust_Z_wheel");

      entity.GetInventory().CreateInInventory("CarBattery");
      entity.GetInventory().CreateInInventory("SparkPlug");
      entity.GetInventory().CreateInInventory("CarRadiator");
      entity.GetInventory().CreateInInventory("HeadlightH7");
      entity.GetInventory().CreateInInventory("HeadlightH7");
    };

    Fill(CarFluid.FUEL, 50);
    Fill(CarFluid.COOLANT, 6.0);
    Fill(CarFluid.OIL, 4.0);
  }
};







class dodge_rust_Z_Trader extends dodge_rust_Z

override void OnDebugSpawn() {
    EntityAI entity;

    if (Class.CastTo(entity, this)) {
      entity.GetInventory().CreateInInventory("dodge_rust_Z_doors_driver");
      entity.GetInventory().CreateInInventory("dodge_rust_Z_doors_codriver");
      entity.GetInventory().CreateInInventory("dodge_rust_Z_doors_cargo1");
      entity.GetInventory().CreateInInventory("dodge_rust_Z_doors_cargo2");
      entity.GetInventory().CreateInInventory("dodge_rust_Z_doors_hood");
      entity.GetInventory().CreateInInventory("dodge_rust_Z_doors_trunk");

      entity.GetInventory().CreateInInventory("dodge_rust_Z_wheel");
      entity.GetInventory().CreateInInventory("dodge_rust_Z_wheel");
      entity.GetInventory().CreateInInventory("dodge_rust_Z_wheel");
      entity.GetInventory().CreateInInventory("dodge_rust_Z_wheel");
      entity.GetInventory().CreateInInventory("dodge_rust_Z_wheel");
      entity.GetInventory().CreateInInventory("dodge_rust_Z_wheel");

      entity.GetInventory().CreateInInventory("CarBattery");
      entity.GetInventory().CreateInInventory("SparkPlug");
      entity.GetInventory().CreateInInventory("CarRadiator");
      entity.GetInventory().CreateInInventory("HeadlightH7");
      entity.GetInventory().CreateInInventory("HeadlightH7");
    };

    Fill(CarFluid.FUEL, 50);
    Fill(CarFluid.COOLANT, 6.0);
    Fill(CarFluid.OIL, 4.0);
  }