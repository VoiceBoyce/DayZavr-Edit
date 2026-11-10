// Точка RMR у прицелов, где RMR стоит вторым режимом поверх основного (OpticsInfoWeaponOverride):
// AD_ACOG_RMR (+ _B13, _MosinMount) и AD_NFATACR_B13_RMR.
// Ваниль включает батарейный прицел только в режиме оптики (DayZPlayerImplement.SwitchOptics),
// а режим RMR для игры это «механика»: прицел выключен, и точка не рисуется.
// Здесь режим RMR включает прицел на сервере так же, как ваниль включает оптику.
//
// Клиент у этих прицелов батарейку не включает и не выключает: ваниль на клиенте при выходе из оптики
// сама сбрасывает флаг «включён», а сервер, у которого прицел выключился и включился в одном кадре,
// значение не меняет и второй раз не шлёт. Клиент оставался «выключенным», точки не было (проверено на стенде).
// Поэтому состояние держит только сервер, клиент получает его синхронизацией и только прячет точку.

class AD_RmrDot
{
	static bool IsRmrDotOptic(ItemOptics optic)
	{
		if (!optic || !optic.HasEnergyManager())
			return false;
		return optic.IsKindOf("AD_ACOG_RMR_Base") || optic.IsKindOf("AD_NFATACR_B13_RMR");
	}

	// Клиент сетевой игры: батарейку не трогаем, ей распоряжается сервер
	static bool IsNetClient()
	{
		return g_Game.IsMultiplayer() && !g_Game.IsServer();
	}

	// Прицел сейчас в деле: через него целятся (RMR или оптика) или смотрят, держа в руках
	static bool IsInUse(ItemOptics optic)
	{
		DayZPlayerImplement player = DayZPlayerImplement.Cast(optic.GetHierarchyRootPlayer());
		if (!player || !player.IsAlive())
			return false;

		EntityAI inHands = player.GetEntityInHands();
		if (!inHands)
			return false;
		if (inHands == optic)
			return true;

		return inHands == optic.GetHierarchyParent() && (player.IsInIronsights() || player.IsInOptics());
	}

	// Страховка из OnWork прицела: включён без дела (игрок умер или вышел, целясь через RMR) — выключить,
	// чтобы не садил батарейку. Выключаем отложенно, не изнутри обновления энергии.
	// (ItemOptics — движковый класс, modded для него нельзя, поэтому вызов стоит в каждом классе прицела.)
	static void CheckUnused(ItemOptics optic)
	{
		if (g_Game.IsServer() && optic && !IsInUse(optic))
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Call(AD_RmrDot.SwitchOffIfUnused, optic);
	}

	static void SwitchOffIfUnused(ItemOptics optic)
	{
		if (optic && optic.GetCompEM() && optic.GetCompEM().IsSwitchedOn() && !IsInUse(optic))
			optic.GetCompEM().SwitchOff();
	}
}

modded class DayZPlayerImplement
{
	protected ItemOptics m_AD_RmrDotOptic;

	override void SetIronsights(bool value)
	{
		super.SetIronsights(value);

		AD_UpdateRmrDotPower(value);
	}

	// Как ваниль, но у наших прицелов на клиенте без SwitchOn/SwitchOff (см. шапку файла)
	override void SwitchOptics(ItemOptics optic, bool state)
	{
		if (!AD_RmrDot.IsNetClient() || !AD_RmrDot.IsRmrDotOptic(optic))
		{
			super.SwitchOptics(optic, state);
			return;
		}

		if (!optic.IsSightOnly())
		{
			if (state)
			{
				optic.EnterOptics();
				optic.OnOpticEnter();
			}
			else
			{
				optic.ExitOptics();
				optic.OnOpticExit();
			}
		}

		if (m_CameraOptics != state)
			SetOptics(state);
	}

	protected void AD_UpdateRmrDotPower(bool ironsights)
	{
		ItemOptics optic;
		if (ironsights)
		{
			Weapon_Base weapon = Weapon_Base.Cast(GetEntityInHands());
			if (weapon)
				optic = weapon.GetAttachedOptics();
			if (!AD_RmrDot.IsRmrDotOptic(optic))
				optic = null;
		}

		// Вышли из режима RMR: выключить прицел (если переходим в его оптику, её ваниль включит сама)
		if (m_AD_RmrDotOptic && m_AD_RmrDotOptic != optic)
		{
			if (AD_RmrDot.IsNetClient())
				m_AD_RmrDotOptic.ShowReddot(false);
			else if (m_AD_RmrDotOptic.GetCompEM() && !m_AD_RmrDotOptic.IsInOptics())
				m_AD_RmrDotOptic.GetCompEM().SwitchOff();
			m_AD_RmrDotOptic = null;
		}

		// Вошли в режим RMR: на сервере включить, как ваниль включает прицел при входе в оптику.
		// Клиент покажет точку сам (камера каждый кадр вызывает UpdateOpticsReddotVisibility).
		if (optic && optic.GetCompEM())
		{
			if (!AD_RmrDot.IsNetClient() && !optic.GetCompEM().IsSwitchedOn())
				optic.GetCompEM().SwitchOn();
			m_AD_RmrDotOptic = optic;
		}
	}
}
