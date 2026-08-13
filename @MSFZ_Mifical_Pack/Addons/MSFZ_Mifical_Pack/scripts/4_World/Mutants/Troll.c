class MSFZ_Troll_Base extends ZmbM_HermitSkinny_Base
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

class MSFZ_Troll extends MSFZ_Troll_Base
{
    void MSFZ_Troll()
    {
        SetEventMask(EntityEvent.POSTFRAME);
    }
    override void EOnPostFrame(IEntity other, int extra)
    {
        float scale = 1.8; // Коэффициент увеличения изначально 1
        vector transform[4];
        GetTransform(transform);
    
        transform[0] = transform[0] * scale;
        transform[1] = transform[1] * scale;
        transform[2] = transform[2] * scale;
    
        SetTransform(transform);
        Update();
    }
    override void EOnFrame(IEntity other, float timeSlice)
    {
        vector rotation = GetYawPitchRoll();
        SetYawPitchRoll(Vector(rotation[0], 0, 0));
    }
};
