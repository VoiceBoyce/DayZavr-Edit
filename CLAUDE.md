# DayZavr-Edit — моды для сервера DayZ

## Договорённости

- **Язык:** всё на русском: ответы, комментарии в коде, документация, скиллы.
- **Git:** только пользователь. Не делать commit/push/checkout/reset/stash и прочие изменяющие команды.
  Чтение (`git status`, `git diff`, `git log`) разрешено.
- **Сборка PBO:** через Mikero's Tools, см. скилл `mikero-pbo`. Тест в игре/на сервере делает пользователь.
- Временные файлы складывать в scratchpad сессии, не в репозиторий.

## Структура

- `modding/<папка_мода>/` — каждый мод: исходники `<Мод>/` (config.cpp, scripts, $PBOPREFIX$.txt)
  и собранный `@<Мод>/Addons/<Мод>.pbo`. Рядом могут лежать серверные `*_Server.pbo`.
  - `MOD_INT_UIFramework_DayZavr`, `mod_vb_DRL_Core`, `mod_vb_DRL_CraftMenu`,
    `mod_vb_MSFZ_Mifical_Pack`, `mod_vb_clothes_dayzavr`, `старые моды` (архив, не трогать без запроса).
- `.claude/skills/` — скиллы (подхватываются автоматически).
- `.claude/knowledge/` — справочные заметки по DayZ, читать по необходимости.
- `.claude/tools/` — вспомогательные Python-инструменты из набора скиллов.
- В `.gitignore`: `*.pbo *.paa *.p3d *.edds *.ogg`.

## Пути на этой машине

| Что | Где |
|---|---|
| DayZ (клиент) | `D:\SteamLibrary\steamapps\common\DayZ` и `G:\SteamLibrary\steamapps\common\DayZ` |
| DayZ Tools | `D:\SteamLibrary\steamapps\common\DayZ Tools` |
| DayZ Server | `G:\SteamLibrary\steamapps\common\DayZServer` |
| Mikero's Tools | `C:\Program Files (x86)\Mikero\DePboTools\bin` |

Диска `P:\` нет.

## Про скиллы из набора

Скиллы и knowledge перенесены из чужой среды. В них встречаются пути и сущности, которых здесь нет:
`P:\`, `H:\Полезное\...`, «vault», `<vault>\AI\00_System\workflow.md`, моды LFPowerGrid/Crate,
MCP-мост. Доменные знания (Enforce Script, config.cpp, DayZ 1.30) использовать, а пути
и шаги процесса сверять с этой машиной и таблицей выше. Если шаг требует несуществующего ресурса,
не выдумывать, а сказать об этом.
