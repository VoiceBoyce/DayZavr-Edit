// Флаг для VB_Tablet_SE: крафт DRL считается, контракты CraftGroups можно выдавать.
modded class MissionServer
{
	override void OnInit()
	{
		// до super: там VB_Tablet_SE собирает список доступных контрактов
		VB_TabletContracts.s_CraftHook = true;
		super.OnInit();
	}
}
