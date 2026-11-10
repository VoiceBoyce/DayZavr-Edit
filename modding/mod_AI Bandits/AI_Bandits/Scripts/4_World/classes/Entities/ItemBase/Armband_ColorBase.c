modded class Armband_ColorBase extends Clothing
{
	override void SwitchItemSelectionTextureEx(EItemManipulationContext context, Param par = null)
	{
		super.SwitchItemSelectionTextureEx(context, par);

		if (context == EItemManipulationContext.ATTACHING || context == EItemManipulationContext.UPDATE)
		{
			Param1<BanditAI_Base> data = Param1<BanditAI_Base>.Cast(par);
			if (!data)
				return;

			BanditAI_Base player = data.param1;

			int selection_idx = -1;
			string texture;

			array<string> show_selections = new array<string>;
			array<string> hide_selections = new array<string>;

			// has top
			if (player.FindAttachmentBySlotName("Body"))
			{
				show_selections.Insert("camoMale_big_a");
				show_selections.Insert("camoMale_big_b");
				show_selections.Insert("camoFemale_big_a");
				show_selections.Insert("camoFemale_big_b");

				hide_selections.Insert("camoMale_small_a");
				hide_selections.Insert("camoMale_small_b");
				hide_selections.Insert("camoFemale_small_a");
				hide_selections.Insert("camoFemale_small_b");
			}
			// has no top
			else
			{
				hide_selections.Insert("camoMale_big_a");
				hide_selections.Insert("camoMale_big_b");
				hide_selections.Insert("camoFemale_big_a");
				hide_selections.Insert("camoFemale_big_b");

				show_selections.Insert("camoMale_small_a");
				show_selections.Insert("camoMale_small_b");
				show_selections.Insert("camoFemale_small_a");
				show_selections.Insert("camoFemale_small_b");
			}

			int i;
			for (i = 0; i < show_selections.Count(); i++)
			{
				selection_idx = GetHiddenSelectionIndex(show_selections[i]);
				texture = GetHiddenSelectionsData().GetHiddenSelectionsTextures()[selection_idx];
				SetObjectTexture(selection_idx, texture);
			}

			for (i = 0; i < hide_selections.Count(); i++)
			{
				selection_idx = GetHiddenSelectionIndex(hide_selections[i]);
				SetObjectTexture(selection_idx, "");
			}
		}
	}
}