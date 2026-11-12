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
    	MSFZ_Scale.Apply(this, 0.3);
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
    	MSFZ_Scale.Apply(this, 0.6);
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
    	MSFZ_Scale.Apply(this, 0.9);
    }
};
