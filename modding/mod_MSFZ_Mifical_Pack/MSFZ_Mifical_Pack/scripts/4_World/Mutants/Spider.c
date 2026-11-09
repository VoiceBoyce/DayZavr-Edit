// Малые пауки теперь появляются в бою, когда ранят матку (серверный PBO, Mutants/Spider.c),
// а не при свежевании туши — модификация ActionSkinning убрана.
class MSFZ_Mutant_Spider_Base extends Animal_CanisLupus{};
class MSFZ_Mutant_Spider1 extends MSFZ_Mutant_Spider_Base{};
class MSFZ_Mutant_Spider2 extends MSFZ_Mutant_Spider_Base{};
class MSFZ_Mutant_Spider3 extends MSFZ_Mutant_Spider_Base{};
modded class MSFZ_Mutant_Spider1
{
    void MSFZ_Mutant_Spider1()
    {
        SetEventMask(EntityEvent.POSTFRAME);
    }
    override void EOnPostFrame(IEntity other, int extra)
    {
        float scale = 0.3; // Коэффициент увеличения изначально 1
        vector transform[4];
        GetTransform(transform);

        transform[0] = transform[0] * scale;
        transform[1] = transform[1] * scale;
        transform[2] = transform[2] * scale;

        SetTransform(transform);
        Update();
    }
};
modded class MSFZ_Mutant_Spider2
{
    void MSFZ_Mutant_Spider2()
    {
        SetEventMask(EntityEvent.POSTFRAME);
    }
    override void EOnPostFrame(IEntity other, int extra)
    {
        float scale = 0.6; // Коэффициент увеличения изначально 1
        vector transform[4];
        GetTransform(transform);

        transform[0] = transform[0] * scale;
        transform[1] = transform[1] * scale;
        transform[2] = transform[2] * scale;

        SetTransform(transform);
        Update();
    }
};
modded class MSFZ_Mutant_Spider3
{
    void MSFZ_Mutant_Spider3()
    {
        SetEventMask(EntityEvent.POSTFRAME);
    }
    override void EOnPostFrame(IEntity other, int extra)
    {
        float scale = 0.9; // Коэффициент увеличения изначально 1
        vector transform[4];
        GetTransform(transform);

        transform[0] = transform[0] * scale;
        transform[1] = transform[1] * scale;
        transform[2] = transform[2] * scale;

        SetTransform(transform);
        Update();
    }
};
