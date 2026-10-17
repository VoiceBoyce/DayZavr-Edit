# DayZavr-Edit — моды для сервера DayZ

## Договорённости

- **Язык:** всё на русском: ответы, комментарии в коде, документация, скиллы.
- **Git:** только пользователь. Не делать commit/push/checkout/reset/stash и прочие изменяющие команды.
  Чтение (`git status`, `git diff`, `git log`) разрешено.
- **Сборка PBO:** скилл `dayzavr-pbo`, команда `pbo.ps1 build <исходники>`. Тест в игре/на сервере делает пользователь.
- **Подпись:** все моды подписываются ключом **DayZavrN** через DsUtils (`build` делает это сам).
  Приватный ключ не копировать и не класть в репозиторий/моды.
- **Конфиги в исходниках только текстовые** (`config.cpp`). `config.bin` получается только при сборке.
- Временные файлы складывать в scratchpad сессии, не в репозиторий.

## Структура

- `modding/<папка_мода>/` — общая папка мода:
  - `<Мод>/` — **исходники**, здесь правим. Заголовки PBO в `pbo.json` или `$PBOPREFIX$.txt`.
  - `@<Мод>/` — **готовый подписанный мод для теста**: `Addons/<Мод>.pbo` + `.bisign` и `keys/DayZavrN.bikey`.
    Собирается только из `<Мод>/` командой build, руками не правится.
  - Рядом бывают `*_Server.pbo`, types `*.xml`, `Classnames.txt`. Их без запроса не трогаем.
  - `MOD_INT_UIFramework_DayZavr`, `mod_vb_DRL_Core`, `mod_vb_DRL_CraftMenu`,
    `mod_vb_MSFZ_Mifical_Pack`, `mod_vb_clothes_dayzavr`, `старые моды` (архив, не трогать без запроса).
- `.claude/skills/` — скиллы (подхватываются автоматически).
- `.claude/knowledge/` — справочные заметки по DayZ, читать по необходимости.
- `.claude/tools/` — вспомогательные Python-инструменты из набора скиллов.
- В `.gitignore`: `*.pbo *.paa *.p3d *.edds *.ogg`.

## Пути на этой машине

| Что | Где |
|---|---|
| DayZ (клиент, актуальная 1.30.164.078) | `D:\SteamLibrary\steamapps\common\DayZ` |
| **Диск `P:\`** (распакованная ванилла) | `subst P: G:\DayZ_P`. Автоподключение: HKCU\...\Run\DayZ_P_Drive. В DayZ Tools `settings.ini` → ProjectDrive |
| DayZ Tools | `D:\SteamLibrary\steamapps\common\DayZ Tools` |
| DayZ Server | `G:\SteamLibrary\steamapps\common\DayZServer` |
| Mikero's Tools (распаковка, lint) | `C:\Program Files (x86)\Mikero\DePboTools\bin` |
| CfgConvert, FileBank (упаковка) | `D:\SteamLibrary\steamapps\common\DayZ Tools\Bin\CfgConvert`, `...\Bin\PboUtils` |
| DsUtils + ключ DayZavrN | `D:\SteamLibrary\steamapps\common\DayZ Tools\Bin\DsUtils` |
| Серверы (keys) | `D:\DayZavr\DayZavr-Servers\DayZServer*\keys` (боевые, без запроса не трогать) |
| **dayz-mcp** (тест модов агентом) | `G:\DayZ_Test\dayz-mcp` (v1.4), venv `tools\.venv-mcp` |
| Тестовая среда dayz-mcp | `G:\DayZ_Test\env\_server` (serverDZ.cfg, profiles, mpmissions) и `\_client\profiles` |
| Моды для теста | `G:\DayZ_Test\mods` (`@DayZ_MCP` — мост, плюс копии тестируемых `@Мод`) |
| Политика лаунчера | `%LOCALAPPDATA%\DayZ_MCP\launcher-policy.json` (один проект на каждый тестируемый мод) |

На `P:\` распакована вся ванилла 1.30 (ExtractPbo по префиксам: `P:\DZ\...`, `P:\scripts\...`).
**Перед тем как писать код против ванильного API, проверять классы и сигнатуры в `P:\scripts`**, а не по памяти.
Если `P:\` не виден: `subst P: G:\DayZ_P`.

## Про скиллы из набора

Скиллы и knowledge перенесены из чужой среды. В них встречаются пути и сущности, которых здесь нет:
`P:\`, `H:\Полезное\...`, «vault», `<vault>\AI\00_System\workflow.md`, моды LFPowerGrid/Crate,
MCP-мост. Доменные знания (Enforce Script, config.cpp, DayZ 1.30) использовать, а пути
и шаги процесса сверять с этой машиной и таблицей выше. Если шаг требует несуществующего ресурса,
не выдумывать, а сказать об этом.
