modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);

		actions.Insert(ActionSFP_ScannerMutantToggleMode);
		actions.Insert(ActionSFP_ScannerMutantBurnKillAll);
		actions.Insert(ActionSFP_KazinoPressButton);
		actions.Insert(ActionSFP_AlarmStolbToggle);
		actions.Insert(ActionSFP_UnpackBattery9VPacks);
		actions.Insert(ActionSFP_Unpack2CigarettePacks);
		actions.Insert(ActionAI2UseMedkit);
		actions.Insert(ActionUseExpressSyringe);
		actions.Insert(ActionSTThrowBoltFidov);
		actions.Insert(ActionSFPBandageSelfRun);
		actions.Insert(ActionNavalitKuchu);
		actions.Insert(ActionPackRaspack);
		actions.Insert(ActionSFP_SmokeCigarette);
		actions.Insert(ActionSFP_TakeCigaretteFromPack);
	}
};
