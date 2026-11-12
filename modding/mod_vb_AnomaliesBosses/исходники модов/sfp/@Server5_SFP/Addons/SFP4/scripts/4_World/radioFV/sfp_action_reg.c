modded class ActionConstructor
{
	
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		// SingleUse Actions	
		actions.Insert(ActionSFP_PlayCassette);
		actions.Insert(ActionSFP_StopCassette);
		actions.Insert(ActionSFP_CassetteVolumeUp);
		actions.Insert(ActionSFP_CassetteVolumeDown);
		actions.Insert(ActionSFP_CassetteNextTrack);
		actions.Insert(ActionSFP_CassettePrevTrack);
		
	}
};
