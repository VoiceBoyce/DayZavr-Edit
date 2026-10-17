---
name: dayzavr-pbo
description: >
  Сборка, распаковка, подпись и проверка PBO модов DayZavr на этой машине. Структура модов
  modding/<папка>/<Мод> (исходники) -> <папка>/@<Мод> (готовый подписанный мод). Скрипт pbo.ps1:
  build/unpack/list/lint/sign/check. Использовать всегда, когда нужно собрать мод для теста,
  распаковать PBO, посмотреть, что внутри, проверить config.cpp, подписать ключом DayZavrN,
  перевести config.bin в config.cpp. Триггеры: "собери мод", "запакуй", "распакуй", "перепакуй",
  "подпиши", "что внутри pbo", pbo, build, unpack, pack, bisign, bikey, config.bin, derap, rapify.
---

# Сборка PBO для DayZavr

Проверено 2026-10-17 на MSFZ_Mifical_Pack: распаковка → DeRap всех конфигов → сборка → сравнение
с оригинальным PBO. **Все 536 файлов совпали байт в байт**, включая 33 `config.bin`.

## Структура мода (договорённость с пользователем)

```
modding\mod_vb_MSFZ_Mifical_Pack\          общая папка мода
├── MSFZ_Mifical_Pack\                     ИСХОДНИКИ: здесь правим
│   ├── config.cpp, */config.cpp           текстовые конфиги (не .bin!)
│   ├── scripts\, *.p3d, *.paa, *.rvmat ...
│   ├── pbo.json                           заголовки PBO: prefix/product/version (в PBO не попадает)
│   └── texHeaders.bin                     из оригинала, пакуется как есть
├── @MSFZ_Mifical_Pack\                    ГОТОВЫЙ МОД для теста (результат build)
│   ├── Addons\MSFZ_Mifical_Pack.pbo
│   ├── Addons\MSFZ_Mifical_Pack.pbo.DayZavrN.bisign
│   └── keys\DayZavrN.bikey
└── прочее (types xml, Classnames.txt, *_Server.pbo): пока не трогаем
```

## Команды

```powershell
$pbo = "D:\DayZavr\DayZavr-Edit\.claude\skills\dayzavr-pbo\scripts\pbo.ps1"
& $pbo build  <папка исходников>               # главная: исходники -> @Мод (lint, сборка, подпись, keys, проверка)
& $pbo lint   <папка или config.cpp>           # синтаксис всех config.cpp (Mikero Rapify)
& $pbo unpack <файл.pbo> <папка>               # создаст <папка>\<prefix>\, config.bin -> config.cpp
& $pbo unpack <файл.pbo> <папка> -Raw          # оставить config.bin как есть
& $pbo list   <файл.pbo>                       # листинг
& $pbo pack   <папка> <итог.pbo> [-NoSign]     # упаковка в произвольное место
& $pbo sign   <файл.pbo>                       # подписать ключом DayZavrN
& $pbo check  <папка Addons>                   # проверить подписи
```

Ненулевой код выхода означает ошибку, текст выводится красным. Другой ключ задаётся через `-Key <имя>`.
Сборка MSFZ (312 МБ) занимает около 15 секунд.

## Как устроен build

1. **Lint**: Mikero `Rapify -L` по каждому `config.cpp`. При ошибке сборка останавливается, в сообщении номер строки.
2. **Staging**: исходники копируются (robocopy) во `%TEMP%`, оригинал не меняется. `pbo.json` и `$PBOPREFIX$*` исключаются.
3. **CfgConvert** (DayZ Tools): `config.cpp` → `config.bin` в staging-копии.
4. **FileBank** (DayZ Tools `Bin\PboUtils`): упаковка с `-property` из `pbo.json`
   (или `$PBOPREFIX$.txt`, или prefix = имя папки) и стандартным `exclude.lst`.
5. Готовый PBO перемещается в `@Мод\Addons\`. При любой ошибке на шагах 1–4 рабочий PBO остаётся прежним.
6. **DSSignFile** → `.bisign`, `DayZavrN.bikey` → `@Мод\keys\`, затем **DSCheckSignatures**.

## Почему не MakePbo (важно)

Бесплатный **Mikero MakePbo не умеет паковать моды с моделями**: на **любом** DayZ `.p3d` (ODOL 56),
даже на 6 КБ, он падает с `DeP3d:Out of memory` → `Pbo: p3d error`, exit 11. Проверено на 7 моделях.
Кроме того, он всегда проверяет ссылки на файлы через диск `P:\`, которого здесь нет, и засыпает вывод
`missing file(s)`. Поэтому Mikero используется только для **распаковки (ExtractPbo), lint (Rapify) и DeRap**.

## Инструменты

| Что | Где |
|---|---|
| Mikero: ExtractPbo 2.35, Rapify 1.93, DeRap 1.86, MakePbo 2.16, DePbo DLL 10.21, DeOgg | `C:\Program Files (x86)\Mikero\DePboTools\bin` |
| CfgConvert | `D:\SteamLibrary\steamapps\common\DayZ Tools\Bin\CfgConvert\CfgConvert.exe` |
| FileBank, exclude.lst | `D:\SteamLibrary\steamapps\common\DayZ Tools\Bin\PboUtils\` |
| DSSignFile, DSCheckSignatures, ключи DayZavrN | `D:\SteamLibrary\steamapps\common\DayZ Tools\Bin\DsUtils\` |

## Подводные камни (проверено)

- **DeOgg обязателен для Mikero.** `DePbo64.dll` импортирует `deOgg64.dll`. Без него все exe падают
  с `-1073741515` (0xC0000135). Инсталляторы лежат на https://mikero.bytex.digital/Downloads, ставятся тихо через `/S`, DePbo первым.
- **ExtractPbo**: путь назначения с буквой диска. Без `-P` ждёт клавишу и вешает shell. Понимает только
  файлы с расширением `.pbo` (у бэкапа `.pbo.orig` выдаёт exit 11).
- **DeRap** пишет `config.cpp` рядом с `.bin` и добавляет шапку с абсолютным путём, временем и `#define _ARMA_`.
  Шапку вырезать. Комментарии автора из `.bin` не восстанавливаются.
- **Rapify `-L`**: `No Error(s)` → exit 0, ошибка → exit 21.
- Вывод Mikero идёт в stderr. В PowerShell 5.1 вызывать через `cmd /c "... 2>&1"`, не `2>&1` напрямую.
- `.ps1` с кириллицей сохранять в **UTF-8 с BOM**, иначе Windows PowerShell 5.1 выдаёт ParserError.
  Инструмент Write сохраняет без BOM, поэтому после него BOM нужно вернуть.
- В PowerShell не называть функцию `H`: это алиас `Get-History`.
- **Путь к корню диска в `cmd /c`**: `"P:\"` ломает кавычки (`\"` экранирует кавычку), и ExtractPbo падает с
  `Cannot make folder`. Писать `"P:\\"`.
- Не следить за логом через `tail -f` из Git Bash, пока его пишет PowerShell: файл блокируется,
  и `Add-Content` молча теряет строки.
- Защита сессии запрещает `Remove-Item` по пути `P:\...`. Удалять через реальный путь `G:\DayZ_P\...`.

## Ванилла на P:\

`P:\` = `subst` на `G:\DayZ_P`. Распакованы все 158 PBO DayZ 1.30.164.078 (142 881 файл, ~31 ГБ),
сверка поштучно 2026-10-17. Структура по префиксам: `P:\DZ\<модуль>\`, `P:\scripts\1_Core..5_Mission\`,
`P:\Core`, `P:\gui`, `P:\graphics`, `P:\system`, `P:\bin`, `P:\languagecore`. Конфиги распакованы в `config.cpp`.
После обновления DayZ ванилла устаревает: заново распаковать в чистую папку тем же способом (ExtractPbo по каждому PBO в `"P:\\"`).

## Подпись

- **Ключ сервера `DayZavrN`.** `DayZavrN.biprivatekey` никуда не копировать. В мод кладётся только `.bikey`.
- `DayZavrN.bikey` совпадает (md5) с ключами в `D:\DayZavr\DayZavr-Servers\*\keys\`. `DayZavrN_v1.bikey` — **другой** ключ.
- **DSCheckSignatures всегда возвращает 0**, даже при `is wrong` и `No signature found`. Результат проверять по тексту.
- После перепаковки старый `.bisign` недействителен, `build`/`sign` подписывают заново.
- Подпись покрывает не все байты: правка внутри текстуры подпись не ломает, правка в `.c`-скрипте ломает.

## Новый мод в работу (из готового @-мода)

1. `unpack @Мод\Addons\Мод.pbo` во временную папку, затем перенести `<prefix>\` в `modding\<папка>\<Мод>\`.
   Префикс сохраняется в `$PBOPREFIX$.txt`, он подхватывается при сборке.
2. Если в исходниках `config.bin`: перевести их в `config.cpp` (DeRap + вырезать шапку + lint), `.bin` удалить.
3. `build` и сравнить с оригиналом: распаковать оба с `-Raw` и сравнить хеши файлов. Расхождений быть не должно.
