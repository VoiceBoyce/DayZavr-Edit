class MSFZ_Mutant_Ent_Base extends ZmbM_HermitSkinny_Base
{
    override bool EvaluateDamageHitAnimation( EntityAI pSource, string pComponent, string pAmmoType, out bool pHeavyHit, out int pAnimType, out float pAnimHitDir )
    {
        return false;
    }
	override bool IsBeingBackstabbed()
    {
        return false;
    }
	override bool CanBeSkinned()
    {
        return true;
    }
};
class MSFZ_Mutant_Ent extends MSFZ_Mutant_Ent_Base{};
class MSFZ_Mutant_Ent_Medium extends MSFZ_Mutant_Ent_Base{};
class MSFZ_Mutant_Ent_Hard extends MSFZ_Mutant_Ent_Base{};
modded class MSFZ_Mutant_Ent
{
    void MSFZ_Mutant_Ent()
    {
        SetEventMask(EntityEvent.POSTFRAME);
    }
    override void EOnPostFrame(IEntity other, int extra)
    {
        float scale = 1.6; // Коэффициент увеличения изначально 1
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
modded class MSFZ_Mutant_Ent_Medium
{
    void MSFZ_Mutant_Ent_Medium()
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
modded class MSFZ_Mutant_Ent_Hard
{
    void MSFZ_Mutant_Ent_Hard()
    {
        SetEventMask(EntityEvent.POSTFRAME);
    }
    override void EOnPostFrame(IEntity other, int extra)
    {
        float scale = 2.0; // Коэффициент увеличения изначально 1
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

