// SFP Hazmat helmet implementation
// - Enables NVG toggle on keybind (default: L) by inheriting from Mich2001Helmet
//   (ToggleNVGActionInput force-targets Mich2001Helmet or NVGHeadstrap).
// - Provides full biological + chemical protection for the head slot.

class SFP_Hazmat_Helm extends Mich2001Helmet
{
	// NOTE:
	// We intentionally inherit from Mich2001Helmet so the vanilla ToggleNVGActionInput
	// (ActionInput.c) will recognize this helmet as a valid NVG target.
	// This keeps vanilla mechanics intact while enabling NVG toggle for this custom helmet.

	override float GetProtectionLevel(int type, bool consider_filter = false, int system = 0)
	{
		// Keep vanilla safety checks (ruined/empty items provide no protection)
		if (IsDamageDestroyed() || (HasQuantity() && GetQuantity() <= 0))
			return 0;

		// Full protection for head area
		if (type == DEF_BIOLOGICAL)
			return 1.0;

		if (type == DEF_CHEMICAL)
			return 1.0;

		return super.GetProtectionLevel(type, consider_filter, system);
	}
}
