---
name: mikero-pbo
description: >
  Распаковка и упаковка PBO для DayZ через Mikero's Tools (ExtractPbo, MakePbo, Rapify, DeRap)
  на этой машине. Основной способ работы с PBO в проекте DayZavr-Edit вместо AddonBuilder.
  Использовать, когда нужно: распаковать мод, посмотреть содержимое PBO, собрать/перепаковать PBO,
  проверить config.cpp на синтаксис, конвертировать config.bin <-> config.cpp.
  Триггеры: "распакуй", "запакуй", "собери pbo", "перепакуй мод", "что внутри pbo", pbo, unpack, pack,
  ExtractPbo, MakePbo, config.bin, rapify, derap.
---

# Mikero's Tools: работа с PBO

Проверено 2026-10-17 на этой машине. Тест: распаковка → упаковка → повторная распаковка
двух реальных модов (`INT_UIFramework.pbo`, `MSFZ_Mifical_Pack_Server.pbo`). Все скрипты,
layout'ы и ресурсы совпали байт в байт.

## Установка

- Папка: `C:\Program Files (x86)\Mikero\DePboTools\bin` (в PATH её нет, вызывать по полному пути).
- Установлены бесплатные версии: DePbo 10.21 (DLL), ExtractPbo 2.35, MakePbo 2.16, Rapify 1.93,
  DeRap 1.86, **DeOgg 1.04**.
- **DeOgg обязателен.** `DePbo64.dll` 10.21 импортирует `deOgg64.dll`. Без него все exe молча падают
  с кодом `-1073741515` (0xC0000135, DLL not found), а Git Bash пишет `DePbo64.dll: cannot open shared object file`.
- Источник: https://mikero.bytex.digital/Downloads. Инсталляторы NSIS ставятся тихо через `/S`,
  сначала DePbo. Копии лежат в `C:\Program Files (x86)\Mikero\DePboTools\downloads\`.

## Обёртка (использовать её)

`.claude/skills/mikero-pbo/scripts/pbo.ps1` — запускать из PowerShell:

```powershell
$pbo = "D:\DayZavr\DayZavr-Edit\.claude\skills\mikero-pbo\scripts\pbo.ps1"
& $pbo list   <файл.pbo>                       # листинг содержимого
& $pbo unpack <файл.pbo> <папка>               # создаст <папка>\<prefix>\, config.bin -> config.cpp
& $pbo unpack <файл.pbo> <папка> -Raw          # оставить config.bin как есть
& $pbo pack   <папка_мода> <итог.pbo>          # MakePbo -D, config.cpp бинаризуется в config.bin
& $pbo lint   <config.cpp>                     # проверка синтаксиса через Rapify
```

Ненулевой код выхода означает ошибку, при этом выводится сообщение Mikero.

## Проверенные факты

- **ExtractPbo**
  - Сам создаёт `<dest>\<prefix>\` и пишет туда `$PBOPREFIX$.txt` (`prefix=`, `product=`, `version=`).
  - По умолчанию конвертирует `config.bin` в `config.cpp` с шапкой `//DeRap ...` и `#define _ARMA_`.
    Флаг `-R` оставляет `.bin` как есть.
  - Путь назначения обязательно с буквой диска: `destination *must* contain a drive specifier`.
  - Запуск без `-P` ждёт нажатия клавиши и вешает shell, поэтому `-P` нужен всегда.
- **MakePbo**
  - Берёт prefix из `$PBOPREFIX$.txt` в корне папки, сам файл в PBO не попадает.
  - Бинаризует `config.cpp` в `config.bin`: комментарии из исходника в PBO не сохраняются.
  - Генерирует `texHeaders.bin`, если в моде есть текстуры.
  - **При ошибке в конфиге** возвращает exit 11 и сообщение вида `circa Line 15 Expected Semicolon OR bad array syntax`.
    PBO при этом **не создаётся**.
  - Предупреждение `$prefix$ contains a different prefix over-ride` при перепаковке распакованного мода безвредно.
- **Rapify `-L`**: lint без записи файла. `No Error(s)` → exit 0, ошибка → exit 21.
- Вывод всех утилит идёт в stderr. В PowerShell 5.1 не использовать `2>&1` напрямую на exe:
  обёртка вызывает их через `cmd /c`. Строку `'chcp' is not recognized` выдаёт инсталлятор Mikero
  из-за окружения, она безвредна.
- Для `.ps1` с русским текстом в Windows PowerShell 5.1 файл нужно сохранять в **UTF-8 с BOM**,
  иначе получается ParserError.

## Не проверено / ограничения

- Упаковка исходников с **небинаризованными (MLOD) .p3d** в бесплатном MakePbo урезана (`-U crippled`).
  Для моделей нужна бинаризация через DayZ Tools или pboProject (платный). Перепаковка уже собранных
  модов (ODOL внутри) работает.
- Подпись PBO (`.bisign`) Mikero не делает, для этого DSSignFile из DayZ Tools
  (`D:\SteamLibrary\steamapps\common\DayZ Tools`).
- Обфускация (`-O`) в бесплатной версии недоступна.

## Рабочий цикл правки мода

1. `unpack` во временную папку или рабочую папку мода в `modding\`.
2. Правка `config.cpp` и скриптов.
3. `lint` для `config.cpp`.
4. `pack` в `@Мод\Addons\Мод.pbo`.
5. Тест на сервере делает пользователь.
