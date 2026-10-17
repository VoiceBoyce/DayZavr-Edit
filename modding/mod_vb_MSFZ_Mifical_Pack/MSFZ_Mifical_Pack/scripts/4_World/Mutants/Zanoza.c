class MSFZ_Mutant_Zanoza_Base extends Animal_CanisLupus{};
class MSFZ_Mutant_Zanoza1 extends MSFZ_Mutant_Zanoza_Base {};
class MSFZ_Mutant_Zanoza2 extends MSFZ_Mutant_Zanoza_Base{};
class MSFZ_Mutant_Zanoza3 extends MSFZ_Mutant_Zanoza_Base{};
modded class MSFZ_Mutant_Zanoza1
{
    void MSFZ_Mutant_Zanoza1()
    {
        SetEventMask(EntityEvent.POSTFRAME);
    }
    override void EOnPostFrame(IEntity other, int extra)
    {
        float scale = 0.8; // Коэффициент увеличения изначально 1
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
modded class MSFZ_Mutant_Zanoza2
{
    void MSFZ_Mutant_Zanoza2()
    {
        SetEventMask(EntityEvent.POSTFRAME);
    }
    override void EOnPostFrame(IEntity other, int extra)
    {
        float scale = 0.8; // Коэффициент увеличения изначально 1
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
modded class MSFZ_Mutant_Zanoza3
{
    void MSFZ_Mutant_Zanoza3()
    {
        SetEventMask(EntityEvent.POSTFRAME);
    }
    override void EOnPostFrame(IEntity other, int extra)
    {
        float scale = 0.8; // Коэффициент увеличения изначально 1
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