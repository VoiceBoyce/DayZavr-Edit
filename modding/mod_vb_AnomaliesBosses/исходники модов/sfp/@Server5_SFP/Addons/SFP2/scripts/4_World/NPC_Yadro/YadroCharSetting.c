/*class SFP_YANDRO_CharacterBase: PlayerBase
{
	protected float m_DebugTick = 0;
	
	void SFP_YANDRO_CharacterBase()
	{
		
	}
	
	override void Init()
	{
		SetAllowDamage(false);
		SetCanBeDestroyed(false);
		//TraderID = -1;
		//RegisterNetSyncVariableInt("TraderID");
	}
	
	override void EEInit()
	{
		super.EEInit();
	}
	
	override void EOnInit(IEntity other, int extra)
	{
	
	}
	
	override void OnScheduledTick(float deltaTime)
	{
	
	}
	
	override void CommandHandler(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)	
	{

	}
	
	override void EOnFrame(IEntity other, float timeSlice)
	{
	
	}
	
	override void EOnPostFrame(IEntity other, int extra)
	{
	
	}
	
	override bool	HeadingModel(float pDt, SDayZPlayerHeadingModel pModel)
	{
		return false;
	}
	
	override void OnCommandMoveStart()
	{
		
	}
	
	override void RemoveQuickBarEntityShortcut(EntityAI entity)
	{
		
	}
	
	override void SetEnableQuickBarEntityShortcut(EntityAI entity, bool value)
	{
		
	}
	
	override void EEItemAttached(EntityAI item, string slot_name)
	{

	}
	
	override void EEItemDetached(EntityAI item, string slot_name)
	{
	
	}
	
	override bool CanReceiveAttachment (EntityAI attachment, int slotId)
    {
		return true;
	}
	
	override void SetActions(out TInputActionMap InputActionMap)
	{
		super.SetActions(InputActionMap);
	}
	
	override void SetActionsRemoteTarget(out TInputActionMap InputActionMap)
	{
		super.SetActionsRemoteTarget(InputActionMap);
		RemoveAction(ActionCheckPulseTarget, InputActionMap);
	}
}
*/