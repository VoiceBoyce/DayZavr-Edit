modded class DayZPlayerCamera3rdPersonVehicle extends DayZPlayerCameraBase {
  void DayZPlayerCamera3rdPersonVehicle(DayZPlayer pPlayer, HumanInputController pInput) {
    if (pPlayer.GetCommand_Vehicle().GetTransport().IsKindOf("dodge_rust_Z")) {
      m_fDistance = 6.6;
      m_CameraOffsetMS = "0.0 1.55 0.0";
    }
  }
}
