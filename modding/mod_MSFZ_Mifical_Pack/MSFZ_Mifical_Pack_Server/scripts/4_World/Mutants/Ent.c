// Серверная логика Энтов: удар по блоку проходит частично (урон *_Blocked), тяжёлый удар отбрасывает игрока.
modded class MSFZ_Mutant_Ent_Base
{
	protected const float MSFZ_PUSH_HEAVY = 1000; // толчок тяжёлым ударом (и по блоку тоже)

	override bool FightLogic(int pCurrentCommandID, DayZInfectedInputController pInputController, float pDt)
	{
		if (pCurrentCommandID == DayZInfectedConstants.COMMANDID_ATTACK)
			return MSFZ_MutantMelee.AttackHit(this, MSFZ_PUSH_HEAVY, MSFZ_PUSH_HEAVY, true);

		return super.FightLogic(pCurrentCommandID, pInputController, pDt);
	}
};
