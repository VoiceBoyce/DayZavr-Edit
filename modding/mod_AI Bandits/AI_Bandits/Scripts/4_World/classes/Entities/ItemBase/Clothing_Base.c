modded class Clothing extends Clothing_Base
{
	override void SwitchItemSelectionTextureEx(EItemManipulationContext context, Param par = null)
	{
		super.SwitchItemSelectionTextureEx(context, par);

		Param1<BanditAI_Base> data = Param1<BanditAI_Base>.Cast(par);

		if (!data)
			return;
		
		BanditAI_Base bandit = data.param1;

		int personalityIndex = GetHiddenSelectionIndex("personality");
		if ( personalityIndex >= 0 )
		{
			int bodyPersonalityIndex = bandit.GetHiddenSelectionIndex("personality");
			string tone_mat = bandit.GetHiddenSelectionsMaterials().Get(bodyPersonalityIndex);
			string tone_texture = bandit.GetHiddenSelectionsTextures().Get(bodyPersonalityIndex);
			
			SetObjectMaterial( personalityIndex, tone_mat );
			SetObjectTexture( personalityIndex, tone_texture );
		}
	}
}