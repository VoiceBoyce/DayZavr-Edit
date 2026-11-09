modded class OOTS_Medicine_PsiBlocade
{
	override void OnConsume(float amount, PlayerBase consumer)
	{
		consumer.PsiBlock(Math.RandomIntInclusive(240, 420));
	}
}

modded class OOTS_Medicine_Anabiotik: VitaminBottle
{
	override void OnConsume(float amount, PlayerBase consumer)
	{
		consumer.BlowOutBlock(Math.RandomIntInclusive(480, 600));
	}
}