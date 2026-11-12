// Registers custom modifiers used by SFP4
modded class ModifiersManager
{
	override void Init()
	{
		super.Init();
		AddModifier(new BusterMdfr);
		AddModifier(new SFP_NonStopStaminaRegenMdfr);
	}
}
