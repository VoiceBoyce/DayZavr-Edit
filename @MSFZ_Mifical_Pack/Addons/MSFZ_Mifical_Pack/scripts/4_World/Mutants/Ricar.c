class MSFZ_Ricar_Base extends ZmbM_HermitSkinny_Base {};

class MSFZ_Ricar extends MSFZ_Ricar_Base
{
    override bool EvaluateDamageHitAnimation( EntityAI pSource, string pComponent, string pAmmoType, out bool pHeavyHit, out int pAnimType, out float pAnimHitDir )
    {
        return false;
    }
	override bool CanBeSkinned()
    {
        return true;
    }
}
