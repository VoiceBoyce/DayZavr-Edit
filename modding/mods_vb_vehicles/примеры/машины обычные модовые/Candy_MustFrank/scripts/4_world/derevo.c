modded class Candy_MustFrank : CarScript
{
	 private float DZVR_TreeFellMinSpeed = 1.0;
     private float DZVR_TreeFellMinImpulse = 1000.0;
	
	
	override void OnContact( string zoneName, vector localPos, IEntity other, Contact data )
	{
	    DZVR_TryToFell(other, data);

        super.OnContact(zoneName, localPos, other, data);
	}	

      private void DZVR_TryToFell(IEntity other, Contact data) {
      if (!g_Game.IsServer()) {
        return;
      }   
      if (GetSpeedometer() < DZVR_TreeFellMinSpeed) {
        return;
      }
      if (data.Impulse < DZVR_TreeFellMinImpulse) {
        return;
      }
      WoodBase tree = WoodBase.Cast(other);
    
      if (!tree || !tree.IsTree()) {
        return;
      }
      if (!tree.IsCuttable() || tree.IsDamageDestroyed()) {
        return;
      }
    
      GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(DZVR_FellTree, 50, false, tree);
    }
    
    private void DZVR_FellTree(WoodBase tree) {
      tree.OnTreeCutDown(this);
      tree.DecreaseHealth(tree.GetMaxHealth(), tree.CanBeAutoDeleted());
    }
	
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx) 
	{
        super.OnRPC(sender, rpc_type, ctx);
          
		  switch (rpc_type) 
		   {
			  			#ifndef SERVER
			     case PlantType.TREE_HARD:
			     	SoundHardTreeFallingPlay();
			     break;
			     
			     
			     case PlantType.TREE_SOFT:
			     	SoundSoftTreeFallingPlay();
			     break;
			     
			     
			     case PlantType.BUSH_HARD:
			     	SoundHardBushFallingPlay();
			     break;
			     
			     case PlantType.BUSH_SOFT:
			     	SoundSoftBushFallingPlay();
			     break;
						#endif
           }
     }
}	