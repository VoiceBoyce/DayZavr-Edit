modded class PluginManager
{
	override void Init()
	{
		super.Init();
		//----------------------------------------------------------------------
		// 						   Register modules
		//----------------------------------------------------------------------
		//		        Module Class Name 				Client	Server
		//----------------------------------------------------------------------
		RegisterPlugin("BlowOutManager", 		    false,   true);
		RegisterPlugin("AnomalyTeleportManager", 	false,   true);
	}
}