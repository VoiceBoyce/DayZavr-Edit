class MSFZ_Ricar_Base extends ZmbM_HermitSkinny_Base {};

class MSFZ_Ricar extends MSFZ_Ricar_Base
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
	// Запрет добивания со спины
	override bool CanBeBackstabbed()
    {
        return false;
    }
	override bool CanBeSkinned()
    {
        return true;
    }
}
