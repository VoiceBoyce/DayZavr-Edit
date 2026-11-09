class MSFZ_Mutant_Ent_Base extends ZmbM_HermitSkinny_Base
{
	// Голос ванильных заражённых мутантам не нужен: их рык, шаги и угрозы заданы в CfgAIBehaviours и AnimEvents.
	// Без этого клиент при каждой синхронизации ищет наборы <Класс>_CalmMove_SoundSet и т.п. и пишет предупреждения в лог.
	override protected void HandleSoundEvents()
	{
	}
    override bool EvaluateDamageHitAnimation( EntityAI pSource, string pComponent, string pAmmoType, out bool pHeavyHit, out int pAnimType, out float pAnimHitDir )
    {
        return false;
    }
	// Запрет добивания со спины (ваниль: ZombieBase.CanBeBackstabbed = true, добивание убивает с одного удара)
	override bool CanBeBackstabbed()
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
    	MSFZ_Scale.Apply(this, 1.6);
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
    	MSFZ_Scale.Apply(this, 1.8);
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
    	MSFZ_Scale.Apply(this, 2.0);
    }
};
