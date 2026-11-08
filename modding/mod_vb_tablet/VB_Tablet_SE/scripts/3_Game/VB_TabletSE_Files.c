// Файлы JSON без потери данных при обрыве записи (падение сервера, полный диск, убитый процесс).
// Запись: сначала во временный <файл>.tmp; если он записан, прежний файл уходит в <файл>.bak,
// и только потом новый занимает место основного. Обрыв на любом шаге оставляет целую копию.
// Чтение: основной файл, при ошибке копия .bak, потом недописанный .tmp. Каждая попытка в новый объект.
class VB_SafeJson<Class T>
{
	static bool Save(string path, T data, out string error)
	{
		string tmp = path + ".tmp";
		if (!JsonFileLoader<T>.SaveFile(tmp, data, error))
			return false;
		if (FileExist(path) && !CopyFile(path, path + ".bak"))
			Print(VB_TABLET_TAG + " не обновлена копия " + path + ".bak");
		if (!CopyFile(tmp, path))
		{
			error = "не удалось заменить " + path + " (новые данные остались в .tmp)";
			return false;
		}
		DeleteFile(tmp);
		return true;
	}

	// Одна попытка: файл есть и разобран.
	static bool LoadFrom(string file, T data, out string error)
	{
		if (!FileExist(file))
		{
			error = "нет файла " + file;
			return false;
		}
		return JsonFileLoader<T>.LoadFile(file, data, error);
	}
}

class VB_TabletFiles
{
	// Какой из трёх файлов читать по номеру попытки: 0 основной, 1 копия, 2 недописанный.
	static string Attempt(string path, int n)
	{
		if (n == 1)
			return path + ".bak";
		if (n == 2)
			return path + ".tmp";
		return path;
	}

	static void Recovered(string path, int n)
	{
		if (n > 0)
			Print(VB_TABLET_TAG + " ВНИМАНИЕ: " + path + " не прочитан, данные взяты из " + Attempt(path, n));
	}

	static void Dirs()
	{
		MakeDirectory(VB_TabletConfig.DIR_ROOT);
		MakeDirectory(VB_TabletConfig.DIR_MOD);
	}
}
