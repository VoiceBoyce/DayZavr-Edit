// Камера транспорта от 3-го лица в стиле GTA: обзор 360° без Alt, возврат за корму на ходу,
// кочки не передаются в картинку. Камера от 1-го лица не меняется.
// Ванильный OnUpdate выполняется как обычно (FOV, ПНВ, служебная часть базового класса),
// а поворот, точка привязки и расстояние перезаписываются в его результате.
// В режиме «Ванильная» к результату добавляется только дальность игрока.

// Настройки игрока: $profile:VB_Mods/VB_VehicleCamera/settings.json на клиенте.
class VB_VehicleCameraSeat
{
	int Mode;			// VB_VehicleCameraStore.MODE_*
	float Distance;		// насколько отдалить от ванильного расстояния, метры

	void VB_VehicleCameraSeat()
	{
		Mode = VB_VehicleCameraStore.MODE_VANILLA;
		Distance = 0;
	}

	void Sanitize()
	{
		if (Mode != VB_VehicleCameraStore.MODE_FREE)
			Mode = VB_VehicleCameraStore.MODE_VANILLA;

		Distance = Math.Clamp(Distance, 0, VB_VehicleCameraStore.ZOOM_MAX);
	}
}

class VB_VehicleCameraSettings
{
	ref VB_VehicleCameraSeat Driver;
	ref VB_VehicleCameraSeat Passenger;

	void VB_VehicleCameraSettings()
	{
		Driver = new VB_VehicleCameraSeat();
		Passenger = new VB_VehicleCameraSeat();
	}
}

class VB_VehicleCameraStore
{
	static const int MODE_VANILLA = 0;
	static const int MODE_FREE = 1;

	static const float ZOOM_STEP = 0.5;		// метров за щелчок колеса
	static const float ZOOM_MAX = 3.0;		// на сколько можно отдалить от ванильного расстояния

	protected static const string DIR_ROOT = "$profile:VB_Mods";
	protected static const string DIR_MOD = "$profile:VB_Mods/VB_VehicleCamera";
	protected static const string FILE_SETTINGS = "$profile:VB_Mods/VB_VehicleCamera/settings.json";

	protected static const float SAVE_DELAY = 1.0;	// секунды после последнего щелчка колеса

	protected static ref VB_VehicleCameraSettings s_Settings;
	protected static float s_SaveDelay;

	static VB_VehicleCameraSettings Get()
	{
		if (!s_Settings)
			Load();

		return s_Settings;
	}

	static VB_VehicleCameraSeat GetSeat(bool driver)
	{
		if (driver)
			return Get().Driver;

		return Get().Passenger;
	}

	protected static void Load()
	{
		s_Settings = new VB_VehicleCameraSettings();
		if (!FileExist(FILE_SETTINGS))
			return;

		string error;
		VB_VehicleCameraSettings loaded;
		if (!JsonFileLoader<VB_VehicleCameraSettings>.LoadFile(FILE_SETTINGS, loaded, error) || !loaded)
		{
			Print("[VB_VehicleCamera] не удалось прочитать настройки: " + error);
			return;
		}

		if (loaded.Driver)
			s_Settings.Driver = loaded.Driver;

		if (loaded.Passenger)
			s_Settings.Passenger = loaded.Passenger;

		s_Settings.Driver.Sanitize();
		s_Settings.Passenger.Sanitize();
	}

	// Дальность меняется по щелчкам, поэтому файл пишем один раз, когда игрок закончил крутить колесо.
	static void MarkDirty()
	{
		s_SaveDelay = SAVE_DELAY;
	}

	static void Tick(float dt)
	{
		if (s_SaveDelay <= 0)
			return;

		s_SaveDelay -= dt;
		if (s_SaveDelay <= 0)
			Save();
	}

	static void Flush()
	{
		if (s_SaveDelay > 0)
			Save();
	}

	static void Save()
	{
		s_SaveDelay = 0;

		MakeDirectory(DIR_ROOT);
		MakeDirectory(DIR_MOD);

		string error;
		if (!JsonFileLoader<VB_VehicleCameraSettings>.SaveFile(FILE_SETTINGS, Get(), error))
			Print("[VB_VehicleCamera] не удалось сохранить настройки: " + error);
	}
}

class VB_VehicleCameraRig
{
	protected static const float PITCH_MIN = -70.0;			// вниз, градусы
	protected static const float PITCH_MAX = 10.0;			// вверх: как ванильный UP_ANGLE_CAP
	protected static const float PITCH_DEFAULT = -6.0;		// куда наклон камеры возвращается на ходу

	protected static const float RECENTER_DELAY = 1.5;		// сколько секунд без мыши до возврата за корму
	protected static const float RECENTER_SPEED = 2.0;		// м/с вперёд, ниже камера не возвращается
	protected static const float RECENTER_SMOOTH_YAW = 0.35;
	protected static const float RECENTER_SMOOTH_PITCH = 1.2;
	protected static const float RECENTER_MAX_YAW_SPEED = 150.0;	// град/с

	protected static const float HEAD_RESET_IDLE = 0.3;		// сколько секунд без мыши до возврата головы
	protected static const float HEAD_RESET_ANGLE = 25.0;	// при каком отклонении камеры голова идёт вперёд

	protected static const float SLOPE_FOLLOW = 0.5;		// какая доля уклона машины идёт в камеру
	protected static const float SLOPE_LIMIT = 20.0;		// градусы
	protected static const float SLOPE_SMOOTH = 0.8;		// большое сглаживание: уклон да, кочки нет

	protected static const float BUMP_SMOOTH = 0.12;		// сглаживание высоты точки привязки
	protected static const float BUMP_LIMIT = 0.5;			// на сколько метров она может отстать от машины

	protected static const float ZOOM_SMOOTH = 0.15;

	protected static const string INPUT_LOOK_BACK = "UAVBCamLookBack";

	protected static bool s_MenuOpen;	// открыто круговое меню: мышь не крутит камеру, колесо меняет дальность
	protected static int s_ZoomTick;	// счётчик щелчков колеса, по нему меню показывает дальность

	protected bool m_Ready;
	protected bool m_ExtraReady;
	protected float m_ExtraSmooth;		// плавная поправка расстояния
	protected float m_ExtraVel[1];
	protected float m_YawWS;			// направление камеры в мире, не зависит от поворота машины
	protected float m_Pitch;			// наклон камеры, заданный игроком
	protected float m_Slope;			// сглаженный уклон машины
	protected float m_PivotY;			// сглаженная высота точки привязки
	protected float m_IdleTime;			// сколько секунд мышь не двигалась

	protected float m_YawVel[1];
	protected float m_PitchVel[1];
	protected float m_SlopeVel[1];
	protected float m_PivotVel[1];

	static void SetMenuOpen(bool open)
	{
		s_MenuOpen = open;
	}

	static int GetZoomTick()
	{
		return s_ZoomTick;
	}

	// Активна ли сейчас камера транспорта от 3-го лица.
	static bool IsVehicleCameraActive()
	{
		DayZPlayer player = DayZPlayer.Cast(g_Game.GetPlayer());
		if (!player)
			return false;

		DayZPlayerCamera camera = player.GetCurrentCamera();
		return DayZPlayerCamera3rdPersonVehicle.Cast(camera) || DayZPlayerCamera3rdPersonVehicleMotorbike.Cast(camera);
	}

	static bool IsDriver(DayZPlayer player, Transport transport)
	{
		return transport.CrewMemberIndex(player) == DayZPlayerConstants.VEHICLESEAT_DRIVER;
	}

	// Настройки того сиденья, на котором сидит локальный игрок. Null, если он не в транспорте.
	static VB_VehicleCameraSeat GetLocalSeat()
	{
		DayZPlayer player = DayZPlayer.Cast(g_Game.GetPlayer());
		if (!player)
			return null;

		HumanCommandVehicle command = player.GetCommand_Vehicle();
		if (!command)
			return null;

		Transport transport = command.GetTransport();
		if (!transport)
			return null;

		return VB_VehicleCameraStore.GetSeat(IsDriver(player, transport));
	}

	// Щелчок колеса при открытом меню или с зажатым Alt в транспорте от 3-го лица.
	// Возвращает true, если он ушёл на камеру и список действий двигать не надо.
	// direction: +1 дальше, -1 ближе.
	static bool TryZoom(float direction)
	{
		if (!s_MenuOpen)
		{
			if (GetUApi().GetInputByID(UALookAround).LocalValue() <= 0 || !IsVehicleCameraActive())
				return false;
		}

		VB_VehicleCameraSeat seat = GetLocalSeat();
		if (seat)
		{
			seat.Distance = Math.Clamp(seat.Distance + direction * VB_VehicleCameraStore.ZOOM_STEP, 0, VB_VehicleCameraStore.ZOOM_MAX);
			s_ZoomTick++;
			VB_VehicleCameraStore.MarkDirty();
		}

		return true;
	}

	// Вызывается из обеих камер после ванильного OnUpdate.
	void Run(DayZPlayer player, HumanInputController input, Transport transport, vector offset, float distance, float startYaw, float startPitch, float dt, DayZPlayerCameraResult result)
	{
		if (!player || !input || !transport || !result)
			return;

		bool driver = IsDriver(player, transport);
		VB_VehicleCameraSeat seat = VB_VehicleCameraStore.GetSeat(driver);

		// Дальность работает в обеих камерах: ванильная при +0 м остаётся обычной ванилью.
		if (!m_ExtraReady)
		{
			m_ExtraReady = true;
			m_ExtraSmooth = seat.Distance;
		}
		m_ExtraSmooth = Math.SmoothCD(m_ExtraSmooth, seat.Distance, m_ExtraVel, ZOOM_SMOOTH, 1000, dt);

		if (seat.Mode != VB_VehicleCameraStore.MODE_FREE)
		{
			// При следующем включении свободная камера начнёт с текущих углов.
			m_Ready = false;
			result.m_fDistance = result.m_fDistance + m_ExtraSmooth;
			return;
		}

		Update(player, input, transport, driver, offset, distance + m_ExtraSmooth, startYaw, startPitch, dt, result);
	}

	protected void Update(DayZPlayer player, HumanInputController input, Transport transport, bool driver, vector offset, float distance, float startYaw, float startPitch, float dt, DayZPlayerCameraResult result)
	{
		// Вход игрока
		float yawDelta = 0;
		float pitchDelta = 0;
		if (!s_MenuOpen)
		{
			vector aim = input.GetAimDelta(dt);
			yawDelta = aim[0] * Math.RAD2DEG;
			pitchDelta = aim[1] * Math.RAD2DEG;

			// Запасной путь: пока свободный взгляд погашен (см. возврат головы ниже), GetAimDelta может не отдавать поворот.
			if (Math.AbsFloat(yawDelta) < 0.0001)
			{
				float uaYaw = GetUApi().GetInputByID(UAAimRight).LocalValue() - GetUApi().GetInputByID(UAAimLeft).LocalValue();
				yawDelta = uaYaw * Math.RAD2DEG * dt;
			}
		}

		// Положение машины и игрока
		vector playerTM[4];
		player.GetTransform(playerTM);

		vector playerAngles = Math3D.MatrixToAngles(playerTM);
		float parentYaw = playerAngles[0];

		vector vehicleAngles = transport.GetOrientation();
		float vehiclePitch = fixAngle_180_180(vehicleAngles[1]);
		vector vehiclePos = transport.GetOrigin();

		if (!m_Ready)
		{
			m_Ready = true;
			m_YawWS = parentYaw + startYaw;
			m_Pitch = Math.Clamp(startPitch, PITCH_MIN, PITCH_MAX);
			m_Slope = 0;
			m_PivotY = vehiclePos[1];
			m_IdleTime = RECENTER_DELAY;
		}

		if (Math.AbsFloat(yawDelta) + Math.AbsFloat(pitchDelta) > 0.001)
			m_IdleTime = 0;
		else
			m_IdleTime += dt;

		m_YawWS += yawDelta;
		m_Pitch = Math.Clamp(m_Pitch + pitchDelta, PITCH_MIN, PITCH_MAX);

		// Возврат за корму: только у водителя и только при движении вперёд.
		// Пассажир не ведёт машину и может смотреть в сторону сколько хочет.
		float yawLocal = fixAngle_180_180(m_YawWS - parentYaw);
		float forwardSpeed = vector.Dot(GetVelocity(transport), transport.GetDirection());

		if (driver && m_IdleTime >= RECENTER_DELAY && forwardSpeed > RECENTER_SPEED)
		{
			yawLocal = Math.SmoothCD(yawLocal, 0, m_YawVel, RECENTER_SMOOTH_YAW, RECENTER_MAX_YAW_SPEED, dt);
			m_Pitch = Math.SmoothCD(m_Pitch, PITCH_DEFAULT, m_PitchVel, RECENTER_SMOOTH_PITCH, 1000, dt);
		}
		else
		{
			m_YawVel[0] = 0;
			m_PitchVel[0] = 0;
		}

		m_YawWS = fixAngle_180_180(parentYaw + yawLocal);

		// Голову считает движок по своему углу свободного взгляда, а он не знает про возврат камеры
		// и упирается в ±160°. Если камера смотрит вперёд и мышь не трогают, гасим этот угол:
		// голова возвращается вперёд. ONE_FRAME снимается сам, поэтому на ногах взгляд не сломается.
		if (m_IdleTime >= HEAD_RESET_IDLE && Math.AbsFloat(yawLocal) < HEAD_RESET_ANGLE)
			input.OverrideFreeLook(HumanInputControllerOverrideType.ONE_FRAME, false);

		// Взгляд назад: пока клавиша зажата, камера мгновенно смотрит назад, m_YawWS не меняется.
		float yawOut = yawLocal;
		float pitchBase = m_Pitch;
		if (!s_MenuOpen && IsLookBackHeld())
		{
			yawOut = 180;
			pitchBase = PITCH_DEFAULT;
		}

		// Уклон дороги: долгий фильтр пропускает подъёмы и спуски, но не кочки.
		// Если камера смотрит вбок или назад, поправка уменьшается и меняет знак.
		float slopeTarget = Math.Clamp(vehiclePitch, -SLOPE_LIMIT, SLOPE_LIMIT) * SLOPE_FOLLOW;
		m_Slope = Math.SmoothCD(m_Slope, slopeTarget, m_SlopeVel, SLOPE_SMOOTH, 1000, dt);

		float pitchFinal = pitchBase + m_Slope * Math.Cos(yawOut * Math.DEG2RAD);
		pitchFinal = Math.Clamp(pitchFinal, PITCH_MIN, PITCH_MAX);

		// Точка привязки: центр машины без её наклона, высота сглажена от тряски
		m_PivotY = Math.SmoothCD(m_PivotY, vehiclePos[1], m_PivotVel, BUMP_SMOOTH, 1000, dt);
		m_PivotY = Math.Clamp(m_PivotY, vehiclePos[1] - BUMP_LIMIT, vehiclePos[1] + BUMP_LIMIT);

		vector headingTM[3];
		Math3D.YawPitchRollMatrix(Vector(parentYaw, 0, 0), headingTM);

		vector pivotWS = vehiclePos - playerTM[3];
		vector pivot = pivotWS.InvMultiply3(headingTM);
		pivot[1] = pivot[1] + m_PivotY - vehiclePos[1];

		// Результат: крен и тангаж родителя движок уже отбрасывает (флаги ванильной камеры)
		Math3D.YawPitchRollMatrix(Vector(yawOut, pitchFinal, 0), result.m_CameraTM);
		result.m_CameraTM[3] = pivot + offset;
		result.m_fDistance = distance;
	}

	protected bool IsLookBackHeld()
	{
		UAInput lookBack = GetUApi().GetInputByName(INPUT_LOOK_BACK);
		return lookBack && lookBack.LocalValue() > 0;
	}
}

modded class DayZPlayerCamera3rdPersonVehicle
{
	protected ref VB_VehicleCameraRig m_VBRig;

	override void OnUpdate(float pDt, out DayZPlayerCameraResult pOutResult)
	{
		super.OnUpdate(pDt, pOutResult);

		if (!m_VBRig)
			m_VBRig = new VB_VehicleCameraRig();

		m_VBRig.Run(m_pPlayer, m_pInput, m_Transport, m_CameraOffsetMS, m_fDistance, m_fLeftRightAngle + m_fLeftRightAngleAdd, m_fUpDownAngle + m_fUpDownAngleAdd, pDt, pOutResult);
	}
}

modded class DayZPlayerCamera3rdPersonVehicleMotorbike
{
	protected ref VB_VehicleCameraRig m_VBRig;

	override void OnUpdate(float pDt, out DayZPlayerCameraResult pOutResult)
	{
		super.OnUpdate(pDt, pOutResult);

		if (!m_VBRig)
			m_VBRig = new VB_VehicleCameraRig();

		m_VBRig.Run(m_pPlayer, m_pInput, m_Transport, m_CameraOffsetMS, m_fDistance, m_fLeftRightAngle + m_fLeftRightAngleAdd, m_fUpDownAngle + m_fUpDownAngleAdd, pDt, pOutResult);
	}
}
