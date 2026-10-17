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

## Тест мода через dayz-mcp (агент сам)

`dayz_test_run(project="<Мод>", mode="all", mission="livonia", player_name="Voice", server_wait_s=180,
base_mods=["@CF","@VPPAdminTools_v2"])` → `session_acquire_wait` → инструменты
(world_spawn, player_teleport, capture_screenshot, logs_since…) → `dayz_test_stop(run_id)`.
- Полный modlist 111 в DayZDiag не компилируется (@Notifications, @INT_Trader — строгий компилятор diag); рабочий набор — CF + VPP.
- Серверные моды — параметром `server_mods=["@Servermod"]` (лежит в `G:\DayZ_Test\env111\servermods`).
- Тестовый serverDZ.cfg: `allowFilePatching = 1`; ключи сервера — `G:\Games\DayZavr\keys` (копия keys 111).
- VPP-админка в тесте: конфиг с 111 + ID пользователя `76561198903710848`, пароль как на 111.
- Не блокировать долгим `wait_for` — запустить, сообщить, ждать пользователя.
Перед запуском: `build` мода и копия `@Мод` в `G:\Games\DayZavr`. После смены регистрации MCP
(аргументы в ~/.claude.json) нужно переподключить MCP-сервер в Claude Code (`/mcp`), иначе `daemon_policy_drift`.

## Тест мода в игре вручную (retail сервер 111)

Тестовый сервер `D:\DayZavr\DayZavr-Servers\DayZServer111` (порт 3202) + клиент `G:\Games\DayZavr`:
1. `build` мода → скопировать `@Мод` в `G:\Games\DayZavr\@Мод` (старую версию сначала в бэкап `G:\DayZ_Test\backup\<время>`).
2. Добавить `@Мод` в `modlist_111server.txt` (сервер берёт моды из `G:\Games\DayZavr\`)
   и в `-mod=` в `G:\Games\DayZavr\DayZServer_111_client.bat`. Перед правкой — копии в тот же бэкап.
3. Запуск: `!Start_server_test.bat` (из папки сервера) → дождаться загрузки → `DayZServer_111_client.bat`.
   Запускать через WMI (`Invoke-CimMethod Win32_Process Create`) с CurrentDirectory = папка батника.
4. Логи сервера: `DayZServer111\Instance_1\` (script_*.log, *.RPT, *.ADM); клиента: `%LOCALAPPDATA%\DayZ\`.
   `ANIMATION (E): Can't load ...skeletons.anim.xml` — безвредный шум для любого мода.
`!Start_server.bat` в папке 111 — копия от сервера 16 (боевой 2602), его не запускать.

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
| **dayz-mcp** (тест модов агентом) | `G:\DayZ_Test\dayz-mcp` (v1.4), venv `tools\.venv-mcp`, лаунчер зарегистрирован |
| Игра для dayz-mcp | `G:\Games\DayZavr` (клиент сервера 111 + скопированный `DayZDiag_x64.exe` + `@DayZ_MCP`); `--game-path` в регистрации |
| Тестовая среда dayz-mcp | `G:\DayZ_Test\env111\_server` (копии serverDZ.cfg и миссий 111, без storage) и `\_client\profiles`; `env111\servermods\@Servermod` |
| Политика лаунчера | `%LOCALAPPDATA%\DayZ_MCP\launcher-policy.json`: проект на мод, base_mods = modlist 111. После правки — `build_native_launcher.py --verify-reproducible` + `launcher_registry_update replace-dayz-test-v1 --expected-sha256 <sha256 текущего approved-launchers.json>` |

На `P:\` распакована вся ванилла 1.30 (ExtractPbo по префиксам: `P:\DZ\...`, `P:\scripts\...`).
**Перед тем как писать код против ванильного API, проверять классы и сигнатуры в `P:\scripts`**, а не по памяти.
Если `P:\` не виден: `subst P: G:\DayZ_P`.

## Про скиллы из набора

Скиллы и knowledge перенесены из чужой среды. В них встречаются пути и сущности, которых здесь нет:
`P:\`, `H:\Полезное\...`, «vault», `<vault>\AI\00_System\workflow.md`, моды LFPowerGrid/Crate,
MCP-мост. Доменные знания (Enforce Script, config.cpp, DayZ 1.30) использовать, а пути
и шаги процесса сверять с этой машиной и таблицей выше. Если шаг требует несуществующего ресурса,
не выдумывать, а сказать об этом.
