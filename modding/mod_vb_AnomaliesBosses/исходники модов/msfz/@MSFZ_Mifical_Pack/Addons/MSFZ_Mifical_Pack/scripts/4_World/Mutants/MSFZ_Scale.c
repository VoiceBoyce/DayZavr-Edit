// Масштаб мутанта задаётся абсолютным значением: оси матрицы нормализуются и умножаются на коэффициент.
// Раньше матрица умножалась на коэффициент каждый кадр. Пока анимация её не сбрасывала (сразу после спавна,
// в том числе через админку), коэффициент перемножался кадр за кадром и мутант на время раздувался.
class MSFZ_Scale
{
	static void Apply(IEntity ent, float scale)
	{
		vector transform[4];
		ent.GetTransform(transform);
		for (int i = 0; i < 3; i++)
		{
			vector axis = transform[i];
			axis.Normalize();
			transform[i] = axis * scale;
		}
		ent.SetTransform(transform);
		ent.Update();
	}
};
