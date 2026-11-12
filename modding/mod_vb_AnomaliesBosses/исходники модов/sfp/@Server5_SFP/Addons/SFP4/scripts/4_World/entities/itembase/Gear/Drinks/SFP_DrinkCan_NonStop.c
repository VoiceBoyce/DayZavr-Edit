// SFP NonStop energy drink can
// Config classname: SFP_DrinkCan_NonStop (config inherits from SodaCan_Pipsi)
// We inherit from SodaCan_ColorBase here because SodaCan_Pipsi is config-only.
class SFP_DrinkCan_NonStop : SodaCan_ColorBase
{
	override void OnConsume(float amount, PlayerBase consumer)
	{
		super.OnConsume(amount, consumer);

		if (!consumer)
			return;


		// Convert consumed quantity to seconds:
		// 1 standard sip (UAQuantityConsumed.DRINK) = +10 seconds by default.
		float sipQty = UAQuantityConsumed.DRINK;
		if (sipQty <= 0)
			sipQty = 65.0;

		float sips = amount / sipQty;
		if (sips <= 0)
			return;

		float addedSeconds = sips * SFP_NonStopConstants.BUFF_SECONDS_PER_SIP;

		ModifiersManager mm = consumer.GetModifiersManager();
		if (!mm)
			return;

		ModifierBase mb = mm.GetModifier(SFP_NonStopConstants.MDF_NONSTOP_STAMINA_REGEN);
		SFP_NonStopStaminaRegenMdfr mdfr;
		if (Class.CastTo(mdfr, mb))
		{
			mdfr.AddDuration(addedSeconds);
		}
	}
}
class SFP_DrinkCan_Stalker: SFP_DrinkCan_NonStop {};
class SFP_DrinkCan_RedBull: SFP_DrinkCan_NonStop {};
class SFP_DrinkCan_Monster: SFP_DrinkCan_NonStop {};
class SFP_DrinkCan_MonsterTheDoc: SFP_DrinkCan_NonStop {};