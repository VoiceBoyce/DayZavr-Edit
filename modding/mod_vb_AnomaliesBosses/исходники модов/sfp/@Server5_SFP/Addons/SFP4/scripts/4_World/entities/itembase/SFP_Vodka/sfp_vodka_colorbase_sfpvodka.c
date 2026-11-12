// SFP Vodka effects — хук на «питьё»
//
// Важно: предмет (бутылка) может быть перезаполнен другой жидкостью.
// Поэтому «пьянка» включается ТОЛЬКО если в момент глотка
// в бутылке была нужная жидкость (по маске VODKA_LIQUID_MASK).

class SFP_Vodka_ColorBase: GlassBottle
{
	override void OnConsume(float amount, PlayerBase consumer)
	{
		// ВНИМАНИЕ:
		// GetLiquidType() берём ДО super.OnConsume(), чтобы узнать, ЧТО было внутри ДО глотка.
		// После super.OnConsume() quantity уже уменьшится, а в редких случаях и тип может поменяться.
		int liquidTypeBefore = GetLiquidType();

		super.OnConsume(amount, consumer);

		if (!consumer)
			return;

		float qmax = GetQuantityMax();
		float frac = 0.0;
		if (qmax > 0)
		{
			// Доля бутылки, которая была выпита за этот тик
			frac = amount / qmax;
		}
		else
		{
			// Фолбэк: если у предмета почему-то нет quantity max — считаем, что это маленький глоток
			frac = 0.1;
		}

		// Если жидкость не наша — ничего не делаем.
		if ((liquidTypeBefore & SFP_VodkaConstants.VODKA_LIQUID_MASK) == 0)
			return;

		consumer.SFP_AddVodkaFromBottleFraction(frac);
	}
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionOpenVodkaRoz);
	}
}


class SFP_Vodka_GopStop: SFP_Vodka_ColorBase {};
class SFP_Vodka_Fraerok: SFP_Vodka_ColorBase {};
class SFP_Vodka_EcoAlko: SFP_Vodka_ColorBase {};
class SFP_Vodka_Sovok: SFP_Vodka_ColorBase {};
class SFP_Vodka_Pseno: SFP_Vodka_ColorBase {};
class SFP_Vodka_Rus: SFP_Vodka_ColorBase {};
class SFP_Vodka_Sibir: SFP_Vodka_ColorBase {};
class SFP_Vodka_istrinka: SFP_Vodka_ColorBase {};
class SFP_Vodka_Kazak: SFP_Vodka_ColorBase {};
class SFP_Vodka_Pseno025: SFP_Vodka_ColorBase {};
class SFP_Vodka_Rus025: SFP_Vodka_ColorBase {};
class SFP_Vodka_Sibir025: SFP_Vodka_ColorBase {};
class SFP_Vodka_istrinka025: SFP_Vodka_ColorBase {};
class SFP_Vodka_Kazak025: SFP_Vodka_ColorBase {};
class SFP_Vodka_Pseno01: SFP_Vodka_ColorBase {};
class SFP_Vodka_Rus01: SFP_Vodka_ColorBase {};
class SFP_Vodka_Sibir01: SFP_Vodka_ColorBase {};
class SFP_Vodka_istrinka01: SFP_Vodka_ColorBase {};
class SFP_Samogon_Base: SFP_Vodka_ColorBase {};
class SFP_SAMOGON_NastoyX2: SFP_Vodka_ColorBase {};
class SFP_SAMOGON_Krotovuha: SFP_Vodka_ColorBase {};
class SFP_SAMOGON_Rabina: SFP_Vodka_ColorBase {};
class SFP_SAMOGON_Maykish: SFP_Vodka_ColorBase {};

class SFP_Cognac_Bootle: SFP_Vodka_ColorBase {};
class SFP_JackDan_Bootle: SFP_Vodka_ColorBase {};