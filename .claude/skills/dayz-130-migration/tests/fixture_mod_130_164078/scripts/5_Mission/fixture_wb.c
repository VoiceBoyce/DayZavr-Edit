[WorkbenchPluginAttribute("Fixture Tool", "fixture", "", "", {"ResourceManager"})]
class FixtureWbTool : WorkbenchPlugin
{
	override void Run()
	{
		ScriptEditor se = Workbench.GetModule("ScriptEditor");
	}
}

#ifdef WORKBENCH
[WorkbenchPluginAttribute("Fixture Guarded", "fixture")]
class FixtureWbGuarded : WorkbenchPlugin
{
	override void Run()
	{
		ScriptEditor se = Workbench.GetModule(ScriptEditor);
	}
}
#endif
